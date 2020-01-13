#include "selectproduct.h"
#include "ui_selectproduct.h"

selectProduct::selectProduct(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::selectProduct)
{
    ui->setupUi(this);
    productsModel.setQuery("select p.product_id, p.basic_unit_id, p.code, p.name, u.unit_code, p.weigth "
                           "from products p, units u "
                           "where p.basic_unit_id = u.unit_id "
                           "order by p.name, p.code");
    ui->tableView->setModel(&productsModel);
}

selectProduct::~selectProduct()
{
    delete ui;
}

void selectProduct::changeEvent(QEvent *e)
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
