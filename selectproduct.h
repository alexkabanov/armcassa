#ifndef SELECTPRODUCT_H
#define SELECTPRODUCT_H

#include <QDialog>
#include <QtSql>

namespace Ui {
    class selectProduct;
}

class selectProduct : public QDialog
{
    Q_OBJECT

public:
    explicit selectProduct(QWidget *parent = 0);
    ~selectProduct();

    QSqlQueryModel productsModel;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::selectProduct *ui;
};

#endif // SELECTPRODUCT_H
