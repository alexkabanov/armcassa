#include "mainsales.h"
#include "ui_mainsales.h"


#include "selectproduct.h"
#include "kkm.h"

#include <QMessageBox>
#include <QKeyEvent>
#include <QModelIndex>
#include <QtSql>
#include <QPushButton>

void MainSales::setEventFilterAllChilds(QObject * obj)
{
    for (int i = 0; i < obj->children().size(); ++i) {
         setEventFilterAllChilds(obj->children().at(i));
         obj->children().at(i)->installEventFilter(keyPressEater);
         //qDebug(obj->children().at(i)->objectName().toAscii());
    }
}

MainSales::MainSales(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainSales)
{
    ui->setupUi(this);
    this->showFullScreen();

    QObject::connect(&rpt, SIGNAL(appProduct(QString)),
                     this, SLOT(appProduct(QString)));

    keyPressEater = new KeyPressEater(this);
    setEventFilterAllChilds(this);

    /*QObjectList::iterator i;
    for (i = this->children().begin(); i != this->children().end(); ++i)
        *i->installEventFilter(keyPressEater);*/
    /*ui->->tableView->installEventFilter(keyPressEater);
    ui->label_total->installEventFilter(keyPressEater);*/

    connectToDB();  //подключаемся к базе данных    
    rpt.setPort(settingsDialog.scanerPort());
    rpt.setPrefix(settingsDialog.scanerPrefix());
    rpt.setSuffix(settingsDialog.scanerSuffix());
    rpt.start();  //включаем сканер

    if (db.isOpen())
        db.transaction();  //открываем

    currentSaleId = this->getNewSaleId();
    refreshContent();

    state = ENTER_BARCODE;

    //ui->label_clientMoney->setVisible(false);
}

MainSales::~MainSales()
{
    db.rollback();
    rpt.stop();
    rpt.wait();
    delete keyPressEater;
    delete ui;
}

void MainSales::connectToDB()
{
    if (db.isOpen()) db.close();
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName(settingsDialog.dbName());
    db.setHostName(settingsDialog.dbHost());
    db.setUserName(settingsDialog.dbUser());
    db.setPassword(settingsDialog.dbPassword());
    if (!db.open())
    {
        ui->lineEdit_satatus->setText(QObject::tr("Ошибка подключения к базе данных: ")
                                      .prepend(db.lastError().text()));
        return;
    }

    QSqlQuery q;
    q.exec(QObject::tr("SET NAMES 'UTF-8';"));
}

void MainSales::disconnectDB()
{
    db.close();
}

QString MainSales::getNewSaleId()
{
    QSqlQuery newSale;
    newSale.exec("select get_new_sale_id();");
    if ( newSale.lastError().type() != QSqlError::NoError )
    {
        ui->lineEdit_satatus->setText(QString("Database error: %1")
                                .arg(newSale.lastError().text()));
    }
    else
    {
        newSale.first();
        return newSale.value(0).toString();
    }
    return QObject::tr("");
}

void MainSales::addWeigthProduct(const QString& barcode)
{
    QString art;
    QString count;

    art = barcode.mid(2,5).trimmed();
    count = barcode.mid(7,5);

    QSqlQuery insert;

    ui->lineEdit_satatus->clear();

    if (currentSaleId.isEmpty())
    {
        currentSaleId = getNewSaleId();
    }

    QString query_str = QString("select `slae_weigth_prod`(%1, '%2', %3);")
        .arg(currentSaleId)
        .arg(art)
        .arg(count);

    insert.exec(query_str);

    if ( insert.lastError().type() != QSqlError::NoError )
    {
        ui->lineEdit_satatus->setText(QString("Query: %1; database error: %2")
                                .arg(query_str)
                                .arg(insert.lastError().text()));
    }
    else
    {
        insert.first();
        if (insert.value(0).toString() != "OK")
        {
            if (insert.value(0).toString() == "Prod not found!")
                ui->lineEdit_satatus->setText(QObject::tr("Товар не найден!"));
            else
                ui->lineEdit_satatus->setText(QString("Query: %1; database error: %2")
                                        .arg(query_str)
                                        .arg(insert.value(0).toString()));

        }
    }

    refreshContent();
}

void MainSales::appProduct(const QString& barcode)
{
    QSqlQuery insert;

    ui->lineEdit_satatus->clear();

    if (productDialog.isVisible())
    {
        productDialog.showProduct(barcode);
        return;
    }

    if (currentSaleId.isEmpty())
    {
        currentSaleId = getNewSaleId();
    }

    if (barcode.indexOf(this->settingsDialog.barcodePrefix()) == 0)
    {
        addWeigthProduct(barcode);
        refreshContent();
        return;
    }



    QString query_str = QString("select `slae_from_barcode`(%1, '%2');")
        .arg(currentSaleId)
        .arg(barcode);

    insert.exec(query_str);

    if ( insert.lastError().type() != QSqlError::NoError )
    {
        ui->lineEdit_satatus->setText(QString("Query: %1; database error: %2")
                                .arg(query_str)
                                .arg(insert.lastError().text()));
    }
    else
    {
        insert.first();
        if (insert.value(0).toString() != "OK")
        {
            if (insert.value(0).toString() == "Prod not found!")
                ui->lineEdit_satatus->setText(QObject::tr("Товар не найден!"));
            else
                ui->lineEdit_satatus->setText(QString("Query: %1; database error: %2")
                                        .arg(query_str)
                                        .arg(insert.value(0).toString()));

        }
    }

    refreshContent();
}

void MainSales::refreshContent()
{
    QString saleId;
    if ( ! currentSaleId.isEmpty() && currentSaleId.length() > 0 )
        saleId = currentSaleId;
    else
        saleId = "si.sale_id+1";

    QString query_str;
    query_str= QString("SELECT "
                        "  @rank := @rank + 1 AS rank, "
                        "  si.sale_item_id, "
                        "  pr.name, "
                        "  si.count, "
                        "  un.unit_code, "
                        "  si.price, "
                        "  si.price*si.count "
                        "FROM "
                        "  sale_items si "
                        "  , products pr "
                        "  , units un "
                        "  ,(select @rank:=0) r "
                        "WHERE "
                        "  si.product_id = pr.product_id "
                        "  and si.unit_id = un.unit_id "
                        "  and si.sale_id = %1 "
                        "ORDER BY "
                        "  si.sale_item_id ").arg(saleId);



    saleItems.setQuery(query_str);

    saleItems.setHeaderData(0, Qt::Horizontal,
                            QObject::tr("№"));
    saleItems.setHeaderData(1, Qt::Horizontal,
                            QObject::tr("Id"));
    saleItems.setHeaderData(2, Qt::Horizontal,
                            QObject::tr("Наименование товара"));
    saleItems.setHeaderData(3, Qt::Horizontal,
                            QObject::tr("Кол-во"));
    saleItems.setHeaderData(4, Qt::Horizontal,
                            QObject::tr("Ед."));
    saleItems.setHeaderData(5, Qt::Horizontal,
                            QObject::tr("Цена"));
    saleItems.setHeaderData(6, Qt::Horizontal,
                            QObject::tr("Сумма"));


    ui->tableView->setModel(&saleItems);

    ui->tableView->setAlternatingRowColors(true);
    /*ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();*/
    ui->tableView->setColumnHidden(0,true);
    ui->tableView->setColumnHidden(1,true);
    ui->tableView->setColumnWidth(1, (int)(0.025*ui->tableView->width()));
    ui->tableView->setColumnWidth(2, (int)(0.50*ui->tableView->width()));
    ui->tableView->setColumnWidth(3, (int)(0.1*ui->tableView->width()));
    ui->tableView->setColumnWidth(4, (int)(0.05*ui->tableView->width()));
    ui->tableView->setColumnWidth(5, (int)(0.15*ui->tableView->width()));
    ui->tableView->setColumnWidth(6, (int)(0.15*ui->tableView->width()));

    showTotal();

    ui->label_docNumber->setText(QString("Продажа №%1")
                                 .arg(getSaleNumber(currentSaleId.toInt())));

    if (ui->tableView->currentIndex().row() > 0)
        ui->tableView->scrollTo(ui->tableView->currentIndex());
    else
        ui->tableView->scrollToBottom();
}

void MainSales::showTotal()
{
    QSqlQuery selectTotal;
    QString query_str;

    if (currentSaleId.isEmpty())
    {
        ui->label_total->setText("0.00 руб.");
        return;
    }

    query_str = QObject::tr("select sum(si.price * si.count) "
                            "from sale_items si "
                            "where si.sale_id = %1;"
                            ).arg(currentSaleId);
    selectTotal.exec(query_str);

    if ( selectTotal.lastError().type() != QSqlError::NoError )
    {
        QMessageBox::critical(
                this
                ,QObject::tr("result")
                ,QString("Query: %1; database error: %2")
                    .arg(query_str)
                    .arg(selectTotal.lastError().text())
                ,QMessageBox::Ok
                ,QMessageBox::NoButton);
    }
    else
    {
        selectTotal.first();
        totalMoney = selectTotal.value(0).toDouble();
        ui->label_total->setText(QObject::tr("%1 руб.").arg(totalMoney, 0, 'f', 2));
    }
}

void MainSales::enterBarcode(int key)
{
    if (key == Qt::Key_0)
         keyBufer.append("0");
    else if (key == Qt::Key_1)
        keyBufer.append("1");
    else if (key == Qt::Key_2)
        keyBufer.append("2");
    else if (key == Qt::Key_3)
        keyBufer.append("3");
    else if (key == Qt::Key_4)
        keyBufer.append("4");
    else if (key == Qt::Key_5)
        keyBufer.append("5");
    else if (key == Qt::Key_6)
        keyBufer.append("6");
    else if (key == Qt::Key_7)
        keyBufer.append("7");
    else if (key == Qt::Key_8)
        keyBufer.append("8");
    else if (key == Qt::Key_9)
        keyBufer.append("9");
    else if (key == Qt::Key_Backspace)
        keyBufer.remove(keyBufer.length()-1, 1);
    else if (key == Qt::Key_Up)
    {
        int row = ui->tableView->currentIndex().row();


        if (ui->tableView->model()->rowCount() > 0)
        {
            if (row < 0)
                ui->tableView->selectRow(ui->tableView->model()->rowCount()-1);
            else
                ui->tableView->selectRow(row-1);
            //ui->lineEdit_satatus->setText(QString().number(row));
        }
    }
    else if (key == Qt::Key_Down)
    {
        int row = ui->tableView->currentIndex().row();
        if (ui->tableView->model()->rowCount() > 0)
        {
            if (row < 0)
                ui->tableView->selectRow(ui->tableView->model()->rowCount()-1);
            else
                ui->tableView->selectRow(row+1);
            //ui->lineEdit_satatus->setText(QString().number(row));
        }
    }
    else if (key == Qt::Key_Delete)
    {
        if (ui->tableView->model()->rowCount() > 0)
            if (saleItems.removeRow(ui->tableView->currentIndex().row()))
                keyBufer.clear();
        refreshContent();
    }
    else if (key == Qt::Key_Plus)
    {
        if (ui->tableView->model()->rowCount() > 0)
            if (saleItems.addProduct(ui->tableView->currentIndex().row(), 1.0f))
                keyBufer.clear();

        QModelIndex index = ui->tableView->currentIndex();
        refreshContent();
        ui->tableView->setCurrentIndex(index);
    }
    else if (key == Qt::Key_Minus)
    {
        int rowCount;
        rowCount = ui->tableView->model()->rowCount();
        if (rowCount > 0)
            if (saleItems.addProduct(ui->tableView->currentIndex().row(), -1.0f))
                keyBufer.clear();

        QModelIndex index = ui->tableView->currentIndex();
        refreshContent();
        if (rowCount == ui->tableView->model()->rowCount())
            ui->tableView->setCurrentIndex(index);
    }
    else if ((key == Qt::Key_Return) || (key == Qt::Key_Enter))
    {
        if (keyBufer.isEmpty())
        {
            if (ui->tableView->model()->rowCount() > 0)
            {
                QFont font;
                font = ui->label_total->font();
                font.setPointSize(46);
                ui->label_total->setFont(font);

                font = ui->label_total->font();
                font.setPointSize(86);
                ui->label_clientMoney->setFont(font);

                font = ui->label_total->font();
                font.setPointSize(46);
                ui->label_returnMoney->setFont(font);
                state = ENTER_CLIENT_MONEY;
            }
        }
        else
        {
            appProduct(keyBufer);
            keyBufer.clear();
            refreshContent();
            showTotal();
        }
    }

    ui->lineEdit_2->setText(keyBufer);
}

void MainSales::enterClientMoney(int key)
{
    int dot_index = keyBufer.indexOf(".");

    if (key == Qt::Key_Backspace)
        keyBufer.remove(keyBufer.length()-1, 1);

    if ((key == Qt::Key_Return) || (key == Qt::Key_Enter))
    {

        clientMoney = keyBufer.toDouble();
        returnMoney = clientMoney-totalMoney;
        ui->label_returnMoney->setText(QObject::tr("%1 руб.").arg(returnMoney, 0, 'f', 2));
        QFont font;
        font = ui->label_total->font();
        font.setPointSize(46);
        ui->label_total->setFont(font);

        font = ui->label_total->font();
        font.setPointSize(46);
        ui->label_clientMoney->setFont(font);

        font = ui->label_total->font();
        font.setPointSize(86);
        ui->label_returnMoney->setFont(font);
        state = SHOW_RETURN_MONEY;
    }

    if (dot_index >= 0)
        if (keyBufer.length() - dot_index >= 3)
            return;

    if (key == Qt::Key_0)
         keyBufer.append("0");
    else if (key == Qt::Key_1)
        keyBufer.append("1");
    else if (key == Qt::Key_2)
        keyBufer.append("2");
    else if (key == Qt::Key_3)
        keyBufer.append("3");
    else if (key == Qt::Key_4)
        keyBufer.append("4");
    else if (key == Qt::Key_5)
        keyBufer.append("5");
    else if (key == Qt::Key_6)
        keyBufer.append("6");
    else if (key == Qt::Key_7)
        keyBufer.append("7");
    else if (key == Qt::Key_8)
        keyBufer.append("8");
    else if (key == Qt::Key_9)
        keyBufer.append("9");
    else if (key == Qt::Key_Comma
             || key == Qt::Key_Period)
    {
        if (keyBufer.indexOf(".") < 0)
            keyBufer.append(".");
    }

    ui->label_clientMoney->setText(QObject::tr("%1 руб.").arg(keyBufer.toDouble(), 0, 'f', 2));
    //QObject::tr("%1 руб.").arg(selectTotal.value(0).toDouble(), 0, 'f', 2);
}

bool MainSales::event(QEvent *event)
{
    QSqlQuery newSale;

    if (event->type() != QEvent::KeyPress)
        return QWidget::event(event);

    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);


    if (keyEvent->modifiers() == Qt::ControlModifier
            && keyEvent->key() == Qt::Key_F12)
    {
        if ( settingsDialog.exec() == QDialog::Accepted )
            this->settingsDialog.saveSettings();
        this->settingsDialog.loadSettings();
        this->rpt.stop();
        this->rpt.wait();
        this->rpt.setPort(this->settingsDialog.scanerPort());
        this->rpt.setPrefix(settingsDialog.scanerPrefix());
        this->rpt.setSuffix(this->settingsDialog.scanerSuffix());
        this->rpt.start();
        return true;
    }
    else if (((keyEvent->key() == Qt::Key_Return) || (keyEvent->key() == Qt::Key_Enter))
            && (keyEvent->modifiers() == Qt::ShiftModifier))
    {
        productDialog.setBarcodePrefix(settingsDialog.barcodePrefix());
        productDialog.exec();
        return true;
    }


    if (state == ENTER_CLIENT_MONEY)
    {
        enterClientMoney(keyEvent->key());
        return true;
    }
    else if (state == SHOW_RETURN_MONEY)
    {
        if (keyEvent->key() == Qt::Key_Backspace)
        {
            keyBufer.clear();
            ui->label_clientMoney->setText(QObject::tr("%1 руб.").arg(clientMoney, 0, 'f', 2));
            QFont font;
            font = ui->label_total->font();
            font.setPointSize(46);
            ui->label_total->setFont(font);

            font = ui->label_total->font();
            font.setPointSize(86);
            ui->label_clientMoney->setFont(font);

            font = ui->label_total->font();
            font.setPointSize(46);
            ui->label_returnMoney->setFont(font);
            state = ENTER_CLIENT_MONEY;
        }
        else if (((keyEvent->key() == Qt::Key_Return) || (keyEvent->key() == Qt::Key_Enter))
            && (keyEvent->modifiers() == Qt::NoModifier))
            if (ui->tableView->model()->rowCount() > 0)
            {
                printCheck();

                db.commit();
                db.transaction();

                newSale.exec("select get_new_sale_id();");
                if ( newSale.lastError().type() != QSqlError::NoError )
                {
                    ui->lineEdit_satatus->setText(QString("Database error: %1")
                                            .arg(newSale.lastError().text()));
                }
                else
                {
                    newSale.first();
                    currentSaleId = newSale.value(0).toString();
                    //ui->lineEdit_satatus->setText(currentSaleId);
                }

                QFont font;
                font = ui->label_total->font();
                font.setPointSize(86);
                ui->label_total->setFont(font);

                font = ui->label_total->font();
                font.setPointSize(46);
                ui->label_clientMoney->setFont(font);

                font = ui->label_total->font();
                font.setPointSize(46);
                ui->label_returnMoney->setFont(font);
                refreshContent();
                showTotal();
                clientMoney = 0.0;
                returnMoney = 0.0;
                ui->label_clientMoney->setText(QObject::tr("%1 руб.").arg(clientMoney, 0, 'f', 2));
                ui->label_returnMoney->setText(QObject::tr("%1 руб.").arg(returnMoney, 0, 'f', 2));
                keyBufer.clear();
                state = ENTER_BARCODE;
            }
        return true;
    }
    else if (state == ENTER_BARCODE)
    {
        enterBarcode(keyEvent->key());
        return true;
    }



    return QWidget::event(event);
}

bool MainSales::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        //QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        //qDebug("Ate key press %a", keyBufer);
        //ui->label_lastProduct->setText("keyBufer");
        return true;
    } else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }

}

QString MainSales::getSaleNumber(int saleId)
{
    QString query_str = QString("select s.sale_number from sales s where s.sale_id = %1;").arg(saleId);


    QSqlQuery selectSaleNumber;

    selectSaleNumber.exec(query_str);

    if ( selectSaleNumber.lastError().type() != QSqlError::NoError )
    {
        QMessageBox::critical(
                this
                ,QObject::tr("result")
                ,QString("Query: %1; database error: %2")
                    .arg(query_str)
                    .arg(selectSaleNumber.lastError().text())
                ,QMessageBox::Ok
                ,QMessageBox::NoButton);
    }
    else
    {
        selectSaleNumber.first();
        return selectSaleNumber.value(0).toString();
    }
    return QString("-1");
}

void MainSales::printCheck2()
{
    KKM kkm;
    kkm.setPort(settingsDialog.printerPort());
    kkm.connectToKKM();


    int weit = 0;

    QModelIndex index;

    if ( ! kkm.isConnected())
    {
        QFile f;
        f.setFileName("printer.log");
        f.open(QIODevice::Append);
        f.write(kkm.getLog().toLocal8Bit());
        f.close();
        return;
    }

    QByteArray resMsg;
    resMsg = kkm.getState();
    int state = resMsg.at(16);
    int substate = resMsg.at(17);

    if (state == 0x03)
    {
        QMessageBox::critical(this, QObject::tr("Ошибка печати чека!")
        ,QObject::tr("Открытая смена, 24 часа кончились. Снемите отчет с гашением. Воспользуйтесь программой \"Тест драйвера ФР\", Регистрация -> Отчеты -> Снять отчет с гашением")
        ,QMessageBox::Ok);
        return;
    }


    if (substate == 1) return;
    if (substate == 2) return;
    if (substate == 4) return;
    if (substate == 3)
    {
        // продолжение печати
        kkm.continuePrint();
    }

    if (state == 8) kkm.cancelPrint();

    double count, price, total_price;

    price = 0.0f;
    total_price = 0.0f;
    QString product;
    for (int i=0; i<ui->tableView->model()->rowCount(); i++)
    {
        // ждем когда отпечатает предидущее
        weit = 0;
        resMsg = kkm.getState();
        state = resMsg.at(16);
        substate = resMsg.at(17);
        while (substate == 5 && weit <= 5) {
            Sleep(100);
            resMsg = kkm.getState();
            state = resMsg.at(16);
            substate = resMsg.at(17);
            weit++;
        }

        index = ui->tableView->model()->index(i, 2);
        product = ui->tableView->model()->data(index, Qt::EditRole).toString();

        index = ui->tableView->model()->index(i, 5);
        price = ui->tableView->model()->data(index, Qt::EditRole).toDouble();

        index = ui->tableView->model()->index(i, 3);
        count = ui->tableView->model()->data(index, Qt::EditRole).toDouble();

        //округляю также как в  фискальнике
        total_price = total_price + floor((floor(price*100+0.5)*floor(count*1000+0.5))/1000+0.5)/100;
        kkm.sale(product, count, price);
    }
    // ждем когда отпечатает предидущее
    weit = 0;
    resMsg = kkm.getState();
    state = resMsg.at(16);
    substate = resMsg.at(17);
    Sleep(500);
    while (substate == 5 && weit <= 15) {
        Sleep(500);
        resMsg = kkm.getState();
        state = resMsg.at(16);
        substate = resMsg.at(17);
        weit++;
    }
    kkm.closeSale("Спасибо за покупку!", total_price);
    // ждем когда отпечатает предидущее
    weit = 0;
    resMsg = kkm.getState();
    state = resMsg.at(16);
    substate = resMsg.at(17);
    Sleep(500);
    while (substate == 5 && weit <= 15) {
        Sleep(500);
        resMsg = kkm.getState();
        state = resMsg.at(16);
        substate = resMsg.at(17);
        weit++;
    }
    //kkm.cut();
    // ждем когда отпечатает предидущее
    weit = 0;
    resMsg = kkm.getState();
    state = resMsg.at(16);
    substate = resMsg.at(17);
    Sleep(500);
    while (substate == 5 && weit <= 15) {
        Sleep(500);
        resMsg = kkm.getState();
        state = resMsg.at(16);
        substate = resMsg.at(17);
        weit++;
    }
    kkm.disconnectFromKKM();

    QFile f;
    f.setFileName("printer.log");
    f.open(QIODevice::Append);
    f.write(kkm.getLog().toLocal8Bit());
    f.close();
}


void MainSales::printCheck()
{
    int retry = 0;
    QString advMode;
    QString errorDesc;

    /*do {
        drvfr.SetComNumber(settingsDialog.printerPort());

        int speed = settingsDialog.printerPortSpeed();
        int cur_idx = 2;
        if (speed == 2400) cur_idx = 0;
        if (speed == 4800) cur_idx = 1;
        if (speed == 9600) cur_idx = 2;
        if (speed == 19200) cur_idx = 3;
        if (speed == 38400) cur_idx = 4;
        if (speed == 57600) cur_idx = 5;
        if (speed == 115200) cur_idx = 6;

        drvfr.SetBaudRate(cur_idx);
        drvfr.SetTimeout(100);
        drvfr.SetPassword(30);

        drvfr.Connect();

        if (drvfr.ResultCode() != 0) {
            errorDesc = drvfr.ResultCodeDescription();
            QMessageBox msgBox(this);
            msgBox.setText(tr("Ошибка печати чека"));
            msgBox.setInformativeText(tr("Код ошибки: ") + QString::number(drvfr.ResultCode()) + tr("; Ошибка: ") + errorDesc);
            msgBox.setIcon(QMessageBox::Warning);
            QPushButton *retryButton = msgBox.addButton(tr("Повторить"), QMessageBox::ActionRole);
            QPushButton *cancelButton = msgBox.addButton(tr("Отменить"), QMessageBox::AcceptRole);

            msgBox.exec();
            if (msgBox.clickedButton() == retryButton) retry = 1;
            if (msgBox.clickedButton() == cancelButton) return;
        }
        else
            retry = 0;
    }
    while (retry > 0);*/

    double price = 0.0;
    double total_price = 0.0;
    double count = 0.0;
    QString product;
    QModelIndex index;
    for (int i=0; i<ui->tableView->model()->rowCount(); i++)
    {
        index = ui->tableView->model()->index(i, 2);
        product = ui->tableView->model()->data(index, Qt::EditRole).toString();

        index = ui->tableView->model()->index(i, 5);
        price = ui->tableView->model()->data(index, Qt::EditRole).toDouble();

        index = ui->tableView->model()->index(i, 3);
        count = ui->tableView->model()->data(index, Qt::EditRole).toDouble();

        total_price = total_price + ceil((ceil(price*100)*ceil(count*1000))/1000)/100;


        /*do {
            drvfr.SetPassword(30);
            drvfr.SetQuantity(ceil(count*1000.0)/1000.0);
            drvfr.SetPrice((qlonglong)(ceil(price*10000.0)));
            drvfr.SetDepartment(0);
            drvfr.SetTax1(0);
            drvfr.SetTax2(0);
            drvfr.SetTax3(0);
            drvfr.SetTax4(0);
            drvfr.SetStringForPrinting(product.left(39));

            drvfr.Sale();


            if (drvfr.ResultCode() != 0)
            {
                if (drvfr.ResultCode() == 80)
                {
                    retry = 1;
                    Sleep(500);
                }
                else
                {
                    errorDesc = drvfr.ResultCodeDescription();
                    drvfr.SetPassword(30);
                    drvfr.GetECRStatus();
                    advMode = drvfr.ECRAdvancedModeDescription();

                    QMessageBox msgBox(this);
                    msgBox.setText(tr("Ошибка печати чека"));
                    msgBox.setInformativeText(tr("Код ошибки: ") + QString::number(drvfr.ResultCode())
                                              + tr("; Ошибка: ") + errorDesc
                                              + tr("; Подрежим ККМ: ") + advMode);
                    msgBox.setIcon(QMessageBox::Warning);
                    QPushButton *retryButton = msgBox.addButton(tr("Повторить"), QMessageBox::ActionRole);
                    QPushButton *cancelButton = msgBox.addButton(tr("Отменить"), QMessageBox::AcceptRole);

                    msgBox.exec();
                    if (msgBox.clickedButton() == retryButton) retry = 1;
                    if (msgBox.clickedButton() == cancelButton) return;
                }
            }
            else
                retry = 0;

        }
        while (retry > 0);*/
    }

    /*do {
        drvfr.SetPassword(30);
        drvfr.SetSumm1((qlonglong)(ceil(total_price*10000.0)));
        drvfr.SetSumm2(0);
        drvfr.SetSumm3(0);
        drvfr.SetSumm4(0);
        drvfr.SetDiscountOnCheck(0);
        drvfr.SetTax1(0);
        drvfr.SetTax2(0);
        drvfr.SetTax3(0);
        drvfr.SetTax4(0);
        drvfr.SetStringForPrinting(tr("Спасибо за покупку!"));

        drvfr.CloseCheck();

        if (drvfr.ResultCode() != 0) {
            if (drvfr.ResultCode() == 80)
            {
                retry = 1;
                Sleep(500);
            }
            else
            {
                errorDesc = drvfr.ResultCodeDescription();
                drvfr.SetPassword(30);
                drvfr.GetECRStatus();
                advMode = drvfr.ECRAdvancedModeDescription();

                QMessageBox msgBox(this);
                msgBox.setText(tr("Ошибка печати чека"));
                msgBox.setInformativeText(tr("Код ошибки: ") + QString::number(drvfr.ResultCode())
                                          + tr("; Ошибка: ") + errorDesc
                                          + tr("; Подрежим ККМ: ") + advMode);
                msgBox.setIcon(QMessageBox::Warning);
                QPushButton *retryButton = msgBox.addButton(tr("Повторить"), QMessageBox::ActionRole);
                QPushButton *cancelButton = msgBox.addButton(tr("Отменить"), QMessageBox::AcceptRole);

                msgBox.exec();
                if (msgBox.clickedButton() == retryButton) retry = 1;
                if (msgBox.clickedButton() == cancelButton) return;
            }
        }
        else
            retry = 0;
    }
    while (retry > 0);

    drvfr.Disconnect();*/

    /*************************************************************************/
}
