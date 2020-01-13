#ifndef MAINSALES_H
#define MAINSALES_H

#include "settingsdialog.h"
#include "productdialog.h"

#include <QMainWindow>
#include <QtSql>
#include "readportthread.h"
#include "saleitemtableview.h"
#include "keypresseater.h"

namespace Ui
{
    class MainSales;
}

enum SaleState {ENTER_BARCODE, ENTER_CLIENT_MONEY, SHOW_RETURN_MONEY, NEW_SALE};

class MainSales : public QMainWindow
{
    Q_OBJECT

public:
    MainSales(QWidget *parent = 0);
    ~MainSales();

    SettingsDialog settingsDialog;
    ProductDialog productDialog;

    KeyPressEater * keyPressEater;

    SaleState state;

    double totalMoney;
    double clientMoney;
    double returnMoney;

    QString currentSaleId;
    QString keyBufer;
    QString lastBarcode;
    //QString clientMoney;

    void enterBarcode(int key);
    void enterClientMoney(int key);
    bool event(QEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

    QString getNewSaleId();

    void connectToDB();
    void disconnectDB();

    void refreshContent();
    void showTotal();

    void printCheck();
    void printCheck2();

    void setEventFilterAllChilds(QObject * obj);

    void addWeigthProduct(const QString& barcode);
    QString getSaleNumber(int saleId);

    ReadPortThread rpt;
    QSqlDatabase db;
    SaleItemTableView saleItems;
protected:
    void keyPressEvent(QEvent *event);

private:
    Ui::MainSales *ui;

private slots:
    void appProduct(const QString& barcode);
};

#endif // MAINSALES_H
