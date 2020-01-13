#ifndef PRODUCTDIALOG_H
#define PRODUCTDIALOG_H

#include <QDialog>
#include "keypresseater.h"

namespace Ui {
    class ProductDialog;
}

class ProductDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProductDialog(QWidget *parent = 0);
    ~ProductDialog();

    KeyPressEater * keyPressEater;
    QString keyBufer;
    QString barcodePrefix;

    void clearProduct();
    void showProduct(const QString &);

    void setEventFilterAllChilds(QObject * obj);

    bool event(QEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

    void setBarcodePrefix(const QString &);

    void setProdName(const QString &);
    void setProdBarcode(const QString &);
    void setProdPrice(const QString &);
    void setProdUnit(const QString &);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ProductDialog *ui;
};

#endif // PRODUCTDIALOG_H
