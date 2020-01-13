#ifndef READPORTTHREAD_H
#define READPORTTHREAD_H

#include <QThread>
#include <QLabel>
#include <windows.h>

class ReadPortThread : public QThread
{
    Q_OBJECT
public:
    ReadPortThread(QObject *parent = 0);

    virtual void run();
    void stop();
    void setPort(int);
    void setPrefix(const QString &);
    void setSuffix(const QString &);

signals:
    void appProduct(const QString& barcode);
private:
     volatile bool stopped;
     volatile int port;
     QString prefix;
     QString suffix;

};

#endif // READPORTTHREAD_H
