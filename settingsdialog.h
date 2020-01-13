#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    Ui::SettingsDialog *ui;

    QSettings * settings;

    QString dbHost();
    QString dbName();
    QString dbUser();
    QString dbPassword();

    int scanerPort();
    QString scanerPrefix();
    QString scanerSuffix();

    QString barcodePrefix();

    int printerPort();
    int printerPortSpeed();

    void setDBName(const QString &value);
    void setDBHost(const QString &value);
    void setDBUser(const QString &value);
    void setDBPassword(const QString &value);

    void setScanerPort(int value);
    void setScanerPrefix(const QString &value);
    void setScanerSuffix(const QString &value);

    void setBarcodePrefix(const QString &value);

    void loadSettings();
    void saveDefaultSettings();
    void saveSettings();

    void setPrinterPort(int value);
    void setPrinterPortSpeed(int value);
protected:
    void changeEvent(QEvent *e);    

private slots:
    void on_pushButton_clicked();
};

#endif // SETTINGSDIALOG_H
