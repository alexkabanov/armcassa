#include "productdialog.h"
#include "ui_productdialog.h"
#include <QKeyEvent>
#include <QtSql>

ProductDialog::ProductDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProductDialog)
{
    ui->setupUi(this);
    keyPressEater = new KeyPressEater(this);
    setEventFilterAllChilds(keyPressEater);
}

ProductDialog::~ProductDialog()
{
    delete keyPressEater;
    delete ui;
}

void ProductDialog::setBarcodePrefix(const QString &value)
{
    barcodePrefix = value;
}

void ProductDialog::setProdName(const QString &value)
{
    ui->textEdit_productName->setText(value);
}

void ProductDialog::setProdBarcode(const QString &value)
{
    ui->lineEdit_prodBarcode->setText(value);
}

void ProductDialog::setProdPrice(const QString &value)
{
    ui->lineEdit_prodPrice->setText(value);
}

void ProductDialog::setProdUnit(const QString &value)
{
    ui->lineEdit_unitName->setText(value);
}

void ProductDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ProductDialog::setEventFilterAllChilds(QObject * obj)
{
    for (int i = 0; i < obj->children().size(); ++i) {
         setEventFilterAllChilds(obj->children().at(i));
         obj->children().at(i)->installEventFilter(keyPressEater);
         //qDebug(obj->children().at(i)->objectName().toAscii());
    }
}

bool ProductDialog::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        //qDebug("Ate key press %d, %d", keyEvent->key(), Qt::Key_Enter );
        //ui->label_lastProduct->setText("keyBufer");

        if (keyEvent->key() == Qt::Key_0)
             keyBufer.append("0");
        else if (keyEvent->key() == Qt::Key_1)
            keyBufer.append("1");
        else if (keyEvent->key() == Qt::Key_2)
            keyBufer.append("2");
        else if (keyEvent->key() == Qt::Key_3)
            keyBufer.append("3");
        else if (keyEvent->key() == Qt::Key_4)
            keyBufer.append("4");
        else if (keyEvent->key() == Qt::Key_5)
            keyBufer.append("5");
        else if (keyEvent->key() == Qt::Key_6)
            keyBufer.append("6");
        else if (keyEvent->key() == Qt::Key_7)
            keyBufer.append("7");
        else if (keyEvent->key() == Qt::Key_8)
            keyBufer.append("8");
        else if (keyEvent->key() == Qt::Key_9)
            keyBufer.append("9");
        else if (((keyEvent->key() == Qt::Key_Return) || (keyEvent->key() == Qt::Key_Enter))
                && (keyEvent->modifiers() == Qt::NoModifier))
        {
            if ( ! keyBufer.isEmpty() )
            {
                showProduct(keyBufer);
                keyBufer.clear();
            }
        }
        else if (((keyEvent->key() == Qt::Key_Return) || (keyEvent->key() == Qt::Key_Enter))
            && (keyEvent->modifiers() == Qt::ShiftModifier))
            this->accept();


        ui->lineEdit_status->setText(keyBufer);
    }

    return QWidget::event(event);
}

void ProductDialog::clearProduct()
{
    ui->lineEdit_productCode->clear();
    ui->textEdit_productName->clear();
    ui->lineEdit_prodBarcode->clear();
    ui->lineEdit_prodPrice->clear();
    ui->lineEdit_unitName->clear();
    ui->lineEdit_status->clear();
}

void ProductDialog::showProduct(const QString &barcode)
{
    QString select;

    clearProduct();

    if ((barcode.indexOf(barcodePrefix) == 0) && ( ! barcodePrefix.isEmpty()))
    {
        select = QObject::tr("select pr.code, pr.name, pr.art, pr.weigth, pc.barcode, un.unit_code, pc.value "
                 "from products pr "
                 "  ,units un "
                 "  ,prices pc "
                 "where pr.art = trim(LEADING '0' FROM '%1') "
                 "  and pr.product_id = pc.product_id "
                 "  and pc.unit_id = un.unit_id").arg(barcode.mid(2,5).trimmed());
    }
    else
    {
        select = QObject::tr("select pr.code, pr.name, pr.art, pr.weigth, pc.barcode, un.unit_code, pc.value "
                 "from products pr "
                 "  ,units un "
                 "  ,prices pc "
                 "where pc.barcode = '%1' "
                 "  and pr.product_id = pc.product_id "
                 "  and pc.unit_id = un.unit_id").arg(barcode);
    }



    QSqlQuery query;
    query.exec(select);
    if ( query.lastError().type() != QSqlError::NoError )
    {
        ui->lineEdit_status->setText(QString("Database error: %1")
                                .arg(query.lastError().text()));
    }
    else
    {
        if (query.size() == 1)
        {
            query.first();
            ui->lineEdit_productCode->setText(query.value(0).toString());
            ui->textEdit_productName->setText(query.value(1).toString());
            ui->lineEdit_prodBarcode->setText(query.value(4).toString());
            ui->lineEdit_prodPrice->setText(tr("%1 руб.").arg(query.value(6).toDouble(), 0, 'f', 2));
            ui->lineEdit_unitName->setText(query.value(5).toString());
        }
        else ui->lineEdit_status->setText(QObject::tr("Товар не найден!"));

    }
}

bool ProductDialog::eventFilter(QObject *obj, QEvent *event)
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
