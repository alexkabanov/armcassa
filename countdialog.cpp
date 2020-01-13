#include "countdialog.h"
#include "ui_countdialog.h"

CountDialog::CountDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CountDialog)
{
    ui->setupUi(this);
}

CountDialog::~CountDialog()
{
    delete ui;
}

void CountDialog::changeEvent(QEvent *e)
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
