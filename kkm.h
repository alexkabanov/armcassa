#ifndef KKM_H
#define KKM_H

#include <QObject>
#include <QStringList>
#include <windows.h>


class KKM : public QObject
{
    Q_OBJECT

    HANDLE Port;
    DCB dcb;
    COMMTIMEOUTS CommTimeOuts;

    DWORD pass;

    int port_number;
    int port_speed;

    QStringList log;

    bool connected;

    int timeout;

    QByteArray inbufer;

public:
    static const char ENQ;
    static const char STX;
    static const char ACK;
    static const char NAK;

    static const char SALE;
    static const char CLOSESALE;
    static const char BEEP;
    static const char CUT;
    static const char GET_STATE;
    static const char TITLE;
    static const char CONTINUE_PRINT;
    static const char CANCEL_PRINT;

    bool isConnected();
    void cancelPrint();
    void continuePrint();
    void beep();
    void cut();
    void sale(const QString & prodName, double count, double price);
    void closeSale(const QString & totalText, double totalPrice);
    QByteArray getState();
    void docTitle(WORD number, const QString & title);

    char readResult();
    void connectToKKM();
    void disconnectFromKKM();
    QByteArray readMessage_new();
    QByteArray readMessage_old();

    char sendToKKM(const QByteArray &message);

    explicit KKM(QObject *parent = 0);

    QString getLog();
    void setPass(DWORD value);
    void setPort(int value);
    void setPortSpeed(int value);
    QString GetStateText(char state, char substate);
    QString GetErrorText(int value);
signals:

public slots:

};

#endif // KKM_H
