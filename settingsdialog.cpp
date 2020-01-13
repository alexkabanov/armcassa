#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QFile>

#include "../kkm/kkm.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    if ( ! QFile::exists(QObject::tr("config.ini")))
    {
        settings = new QSettings(QObject::tr("config.ini"), QSettings::IniFormat);
        saveDefaultSettings();
        loadSettings();
    }
    else
    {
        settings = new QSettings(QObject::tr("config.ini"), QSettings::IniFormat);
        loadSettings();
    }

}

SettingsDialog::~SettingsDialog()
{
    delete settings;
    delete ui;
}

void SettingsDialog::changeEvent(QEvent *e)
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


QString SettingsDialog::dbHost()
{
    return ui->dbhost_lineEdit->text();
}

QString SettingsDialog::dbName()
{
    return ui->dbname_lineEdit->text();
}

QString SettingsDialog::dbUser()
{
    return ui->dbuser_lineEdit->text();
}

QString SettingsDialog::dbPassword()
{
    return ui->dbpassword_lineEdit->text();
}

int SettingsDialog::scanerPort()
{
    return ui->port_comboBox->currentIndex()+1;
}

QString SettingsDialog::scanerPrefix()
{
    return ui->prefix_lineEdit->text();
}

QString SettingsDialog::scanerSuffix()
{
    return ui->suffix_lineEdit->text();
}

QString SettingsDialog::barcodePrefix()
{
    return ui->barcodePrefix_lineEdit->text();
}

int SettingsDialog::printerPort()
{
    return ui->printerPort_comboBox->currentIndex()+1;
}

int SettingsDialog::printerPortSpeed()
{
    int cur_idx = ui->printerPortSpeed_comboBox->currentIndex();
    if (cur_idx == 0) return 2400;
    if (cur_idx == 1) return 4800;
    if (cur_idx == 2) return 9600;
    if (cur_idx == 3) return 19200;
    if (cur_idx == 4) return 38400;
    if (cur_idx == 5) return 57600;
    if (cur_idx == 6) return 115200;
}



void SettingsDialog::setDBName(const QString &value)
{
    ui->dbname_lineEdit->setText(value);
}

void SettingsDialog::setDBHost(const QString &value)
{
    ui->dbhost_lineEdit->setText(value);
}

void SettingsDialog::setDBUser(const QString &value)
{
    ui->dbuser_lineEdit->setText(value);
}

void SettingsDialog::setDBPassword(const QString &value)
{
    ui->dbpassword_lineEdit->setText(value);
}

void SettingsDialog::setScanerPort(int value)
{
    ui->port_comboBox->setCurrentIndex(value-1);
}

void SettingsDialog::setScanerPrefix(const QString &value)
{
    ui->prefix_lineEdit->setText(value);
}

void SettingsDialog::setScanerSuffix(const QString &value)
{
    ui->suffix_lineEdit->setText(value);
}

void SettingsDialog::setBarcodePrefix(const QString &value)
{
    ui->barcodePrefix_lineEdit->setText(value);
}

void SettingsDialog::setPrinterPort(int value)
{
    ui->printerPort_comboBox->setCurrentIndex(value-1);
}

void SettingsDialog::setPrinterPortSpeed(int value)
{
    int cur_idx = 2;
    if (value == 2400) cur_idx = 0;
    if (value == 4800) cur_idx = 1;
    if (value == 9600) cur_idx = 2;
    if (value == 19200) cur_idx = 3;
    if (value == 38400) cur_idx = 4;
    if (value == 57600) cur_idx = 5;
    if (value == 115200) cur_idx = 6;
    ui->printerPortSpeed_comboBox->setCurrentIndex(cur_idx);
}



void SettingsDialog::loadSettings()
{
    if ( ! settings->value("database/host").isNull())
        setDBHost( settings->value("database/host").toString() );
    if ( ! settings->value("database/dbname").isNull())
        setDBName( settings->value("database/dbname").toString() );
    if ( ! settings->value("database/user").isNull())
        setDBUser( settings->value("database/user").toString() );
    if ( ! settings->value("database/password").isNull())
        setDBPassword( settings->value("database/password").toString() );
    if ( ! settings->value("scaner/port").isNull())
        setScanerPort( settings->value("scaner/port").toInt() );
    if ( ! settings->value("scaner/prefix").isNull())
        setScanerPrefix( settings->value("scaner/prefix").toString() );
    if ( ! settings->value("scaner/suffix").isNull())
        setScanerSuffix( settings->value("scaner/suffix").toString() );
    if ( ! settings->value("barcode/prefix").isNull())
        setBarcodePrefix( settings->value("barcode/prefix").toString() );
    if ( ! settings->value("printer/port").isNull())
        setPrinterPort( settings->value("printer/port").toInt() );
    if ( ! settings->value("printer/speed").isNull())
        setPrinterPortSpeed( settings->value("printer/speed").toInt() );
}

void SettingsDialog::saveSettings()
{
    settings->setValue("database/host",  dbHost());
    settings->setValue("database/dbname",  dbName());
    settings->setValue("database/user", dbUser());
    settings->setValue("database/password", dbPassword());
    settings->setValue("scaner/port", scanerPort());
    settings->setValue("scaner/prefix", scanerPrefix());
    settings->setValue("scaner/suffix", scanerSuffix());
    settings->setValue("barcode/prefix", barcodePrefix());
    settings->setValue("printer/port", printerPort());
    settings->setValue("printer/speed", printerPortSpeed());


}

void SettingsDialog::saveDefaultSettings()
{
    settings->setValue("database/host", "localhost");
    settings->setValue("database/dbname", "armcassa");
    settings->setValue("database/user", "root");
    settings->setValue("database/password", "7211");
    settings->setValue("scaner/port", "1");
    settings->setValue("scaner/prefix", "");
    settings->setValue("scaner/suffix", "0d0a");
    settings->setValue("barcode/prefix", "20");
    settings->setValue("printer/port", "2");
    settings->setValue("printer/speed", "2");
}



void SettingsDialog::on_pushButton_clicked()
{
    /*KKM kkm;
    kkm.connectToKKM();
    if ( ! kkm.isConnected())    return;
    //kkm.docTitle(55, QString("Докууумент!"));
    kkm.beep();
    kkm.sale("Товарчег", 3.456f, 1.654f);
    kkm.sale("Еще Товарчег", 3.456f, 1.654f);
    kkm.closeSale("Спасибо за покупку!", 11.432448f);
    //ui->lineEdit->setText(kkm.getLog());
    //ui->lineEdit->setText(kkm.GetStateText(kkm.getState().at(16), 0));
    kkm.disconnectFromKKM();*/
}
