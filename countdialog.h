#ifndef COUNTDIALOG_H
#define COUNTDIALOG_H

#include <QDialog>

namespace Ui {
    class CountDialog;
}

class CountDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CountDialog(QWidget *parent = 0);
    ~CountDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CountDialog *ui;
};

#endif // COUNTDIALOG_H
