#include "kkm.h"
#include <QVariant>
#include <QMessageBox>
#include <QTime>
#include <math.h>

const char KKM::ENQ = 0x05;
const char KKM::STX = 0x02;
const char KKM::ACK = 0x06;
const char KKM::NAK = 0x15;

const char KKM::SALE = 0x80;
const char KKM::CLOSESALE = 0x85;
const char KKM::GET_STATE = 0x11;
const char KKM::CONTINUE_PRINT = 0xB0;
const char KKM::CANCEL_PRINT =0x88;
const char KKM::BEEP = 0x13;
const char KKM::CUT = 0x25;
const char KKM::TITLE = 0x18;


KKM::KKM(QObject *parent) :
    QObject(parent)
{
    timeout = 1000;
    connected = false;
    pass = 0x0000001e;
}

void KKM::setPort(int value)
{
    port_number = value;
}

void KKM::setPortSpeed(int value)
{
    port_speed = value;
}

void KKM::beep()
{
    QByteArray ba;
    QByteArray resMsg;
    char LRC;

    ba.append(KKM::STX);
    ba.append((char)5);
    ba.append(KKM::BEEP);
    ba.append((char*) &pass, 4);

    LRC = ba.at(1);

    for (int i = 2; i<ba.size(); i++)
        LRC = LRC ^ ba.at(i);

    ba.append(LRC);

    char res = sendToKKM(ba);

    if (res == KKM::ACK)
        resMsg = readMessage_new();
    else
    {
        log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                    +QString(" beep - Соединение потеряно!"));
        disconnectFromKKM();
    }
}

void KKM::cancelPrint()
{
    QByteArray ba;
    QByteArray resMsg;
    char LRC;

    ba.append(KKM::STX);
    ba.append((char)5);
    ba.append(KKM::CANCEL_PRINT);
    ba.append((char*) &pass, 4);

    LRC = ba.at(1);

    for (int i = 2; i<ba.size(); i++)
        LRC = LRC ^ ba.at(i);

    ba.append(LRC);

    char res = sendToKKM(ba);

    if (res == KKM::ACK)
        resMsg = readMessage_new();
    else
    {
        log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                    +QString(" beep - Соединение потеряно!"));
        disconnectFromKKM();
    }
}

void KKM::continuePrint()
{
    QByteArray ba;
    QByteArray resMsg;
    char LRC;

    ba.append(KKM::STX);
    ba.append((char)5);
    ba.append(KKM::CONTINUE_PRINT);
    ba.append((char*) &pass, 4);

    LRC = ba.at(1);

    for (int i = 2; i<ba.size(); i++)
        LRC = LRC ^ ba.at(i);

    ba.append(LRC);

    char res = sendToKKM(ba);

    if (res == KKM::ACK)
        resMsg = readMessage_new();
    else
    {
        log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                    +QString(" beep - Соединение потеряно!"));
        disconnectFromKKM();
    }
}

void KKM::cut()
{
    QByteArray ba;
    QByteArray resMsg;
    char LRC;

    if ( ! isConnected() ) return;

    /*ba.append(&KKM::ENQ, 1);
    char res = sendToKKM(ba);
    if (res == KKM::ACK)
    {
        resMsg = readMessage_new();
        res = sendToKKM(ba);
        if (res == KKM::ACK)
        {
            resMsg = readMessage_new();
            res = sendToKKM(ba);
            if (res == KKM::ACK)
                resMsg = readMessage_new();
        }
    }
    ba.clear();*/

    ba.append(KKM::STX);
    ba.append((char)6);
    ba.append(KKM::CUT);
    ba.append((char*) &pass, 4);
    ba.append((char)0);


    LRC = ba.at(1);

    for (int i = 2; i<ba.size(); i++)
        LRC = LRC ^ ba.at(i);

    ba.append(LRC);

    char res = sendToKKM(ba);

    if (res == KKM::ACK)
        resMsg = readMessage_new();
    else
    {
        log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                    +QString(" - cut - Соединение потеряно!"));
        disconnectFromKKM();
    }
}

QByteArray KKM::getState()
{
    QByteArray ba;
    char LRC;

    ba.append(KKM::STX);
    ba.append((char)5);
    ba.append(KKM::GET_STATE);
    ba.append((char*) &pass, 4);

    LRC = ba.at(1);

    for (int i = 2; i<ba.size(); i++)
        LRC = LRC ^ ba.at(i);

    ba.append(LRC);

    char res = sendToKKM(ba);

    if (res != KKM::ACK)
    {
        log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                    +QString(" - getState - Соединение потеряно!"));
        disconnectFromKKM();
        return NULL;
    }
    return readMessage_new();
    //return stateMsg.at(14);
}

void KKM::sale(const QString & prodName, double count, double price)
{
    QByteArray ba;
    QByteArray resMsg;
    char LRC;

    /*ba.append(&KKM::ENQ, 1);
    char res = sendToKKM(ba);
    if (res == KKM::ACK)
    {
        resMsg = readMessage_new();
        res = sendToKKM(ba);
        if (res == KKM::ACK)
        {
            resMsg = readMessage_new();
            res = sendToKKM(ba);
            if (res == KKM::ACK)
                resMsg = readMessage_new();
        }
    }
    ba.clear();*/



    unsigned long prepCount = (unsigned long) floor(count*1000+0.5);
    unsigned long prepPrice = (unsigned long) floor(price*100+0.5);
    QString prepText;

    if ( ! isConnected() ) return;

    prepText = prodName.leftJustified(40, ' ');

    ba.append(KKM::STX);
    ba.append((char)60);
    ba.append(KKM::SALE);
    ba.append((char*) &pass, 4);


    ba.append((char*) &prepCount, 4);
    ba.append((char)0);
    ba.append((char*) &prepPrice, 4);
    ba.append((char)0);

    ba.append((char)0); //номер отеда
    // налоговые группы
    ba.append((char)0);
    ba.append((char)0);
    ba.append((char)0);
    ba.append((char)0);

    ba.append(prepText.toAscii(), 40);

    LRC = ba.at(1);

    for (int i = 2; i<ba.size(); i++)
        LRC = LRC ^ ba.at(i);

    ba.append(LRC);

    char res = sendToKKM(ba);

    if (res == KKM::ACK)
        resMsg = readMessage_new();
    else
    {
        log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                    +QString(" - sale - Соединение потеряно!"));
        disconnectFromKKM();
    }

    log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
               +QString(" - sale price=")+QString::number(price)
               +QString(" count=")+QString::number(count));
}

void KKM::closeSale(const QString & totalText, double totalPrice)
{
    QByteArray ba;
    QByteArray resMsg;
    char LRC;

    unsigned long prepTotalPrice = (unsigned long) floor(totalPrice*100+0.5);
    QString prepText;

    if ( ! isConnected() ) return;

    /*ba.append(&KKM::ENQ, 1);
    char res = sendToKKM(ba);
    if (res == KKM::ACK)
    {
        resMsg = readMessage_new();
        res = sendToKKM(ba);
        if (res == KKM::ACK)
        {
            resMsg = readMessage_new();
            res = sendToKKM(ba);
            if (res == KKM::ACK)
                resMsg = readMessage_new();
        }
    }
    ba.clear();*/

    prepText = totalText.leftJustified(40, ' ');

    ba.append(KKM::STX);
    ba.append((char)71);
    ba.append(KKM::CLOSESALE);
    ba.append((char*) &pass, 4);

    //сумма
    ba.append((char*) &prepTotalPrice, 4);
    ba.append((char)0);

    //суммы типов оплаты
    ba.append((char)0);
    ba.append((char)0);  ba.append((char)0);
    ba.append((char)0);  ba.append((char)0);
    ba.append((char)0);
    ba.append((char)0);  ba.append((char)0);
    ba.append((char)0);  ba.append((char)0);
    ba.append((char)0);
    ba.append((char)0);  ba.append((char)0);
    ba.append((char)0);  ba.append((char)0);

    //скидка/набавка
    ba.append((char)0);  ba.append((char)0);

    // налоговые группы
    ba.append((char)0);
    ba.append((char)0);
    ba.append((char)0);
    ba.append((char)0);

    ba.append(prepText.toAscii(), 40);

    LRC = ba.at(1);

    for (int i = 2; i<ba.size(); i++)
        LRC = LRC ^ ba.at(i);

    ba.append(LRC);

    char res = sendToKKM(ba);

    log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
               +QString(" - sale totalPrice=")+QString::number(totalPrice));


    if (res == KKM::ACK)
        resMsg = readMessage_new();
    else
    {
        log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                    +QString(" - closeSale - Соединение потеряно!"));
        disconnectFromKKM();
    }

}


void KKM::docTitle(WORD number, const QString & title)
{
    QByteArray ba;
    QByteArray resMsg;
    char LRC;

    ba.append(KKM::STX);
    ba.append((char)37);
    ba.append(KKM::TITLE);
    ba.append((char*) &pass, 4);

    ba.append(title.leftJustified(30, ' ').toAscii().data(), 30);

    ba.append((char*) &number, 2);


    LRC = ba.at(1);

    for (int i = 2; i<ba.size(); i++)
        LRC = LRC ^ ba.at(i);

    ba.append(LRC);

    char res = sendToKKM(ba);

    if (res == KKM::ACK)
        resMsg = readMessage_new();
    else
    {
        log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                    +QString(" docTitle - Соединение потеряно!"));
        disconnectFromKKM();
    }
}

void KKM::connectToKKM()
{
    QByteArray resMsg;

    CloseHandle(Port);
    Port = CreateFileA(tr("\\\\.\\COM").append(QString::number(port_number)).toAscii(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (Port == INVALID_HANDLE_VALUE)
    {
        log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                    +QString(" connectToKKM - Невозможно открыть последовательный порт."));
        return;
    }

    if(GetCommState(Port, &dcb)==0)
    {
        log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                    +QString(" connectToKKM - Ошибка получения параметров порта."));
        disconnectFromKKM();
    }

    dcb.DCBlength         = 28;
    dcb.BaudRate          = this->port_speed;
    dcb.fBinary           = 1;
    dcb.fParity           = 0;
    dcb.fOutxCtsFlow      = 0;
    dcb.fOutxDsrFlow      = 0;
    dcb.fDtrControl       = 0;
    dcb.fDsrSensitivity   = 0;
    dcb.fTXContinueOnXoff = 0;
    dcb.fOutX             = 0;
    dcb.fInX              = 0;
    dcb.fErrorChar        = 0;
    dcb.fNull             = 0;
    dcb.fRtsControl       = 0;
    dcb.fAbortOnError     = 0;
    dcb.fDummy2           = 0;
    dcb.wReserved         = 0;
    dcb.XonLim            = 2048;
    dcb.XoffLim           = 512;
    dcb.ByteSize          = 8;
    dcb.Parity            = 0;
    dcb.StopBits          = 0;
    dcb.XonChar           = 0;
    dcb.XoffChar          = 0;
    dcb.ErrorChar         = 0;
    dcb.EofChar           = 0;
    dcb.EvtChar           = 0;
    dcb.wReserved1        = 0;

    CommTimeOuts.ReadIntervalTimeout= 100;
    CommTimeOuts.ReadTotalTimeoutMultiplier = 100;
    CommTimeOuts.ReadTotalTimeoutConstant = 1;
    CommTimeOuts.WriteTotalTimeoutMultiplier = 100;
    CommTimeOuts.WriteTotalTimeoutConstant = 0;
    SetCommTimeouts(Port, &CommTimeOuts);

    PurgeComm(Port, PURGE_RXCLEAR);
    PurgeComm(Port, PURGE_TXCLEAR);
    PurgeComm(Port, PURGE_RXABORT);
    PurgeComm(Port, PURGE_TXABORT);

    SetCommState(Port,&dcb);

    connected = true;

    int attempt = 0;
    while (attempt < 3 && connected == true){
        DWORD dwWriteBytes = 0;
        WriteFile(Port, &KKM::ENQ, 1, &dwWriteBytes, NULL);

        char res = readResult();

        if (res == KKM::ACK)
        {
            resMsg = readMessage_new();
        }
        else if (res == KKM::NAK)
            connected = true;
        else
            connected = false;
        attempt++;
    }
    if (connected == false)
    {
        log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                    +QString(" connectToKKM - Не удалось подключиться к ФК! Нейзвестная ошибка."));
        disconnectFromKKM();
    }
}

void KKM::disconnectFromKKM()
{
    connected = false;
    CloseHandle(Port);
}

bool KKM::isConnected()
{
    return connected;
}

char KKM::sendToKKM(const QByteArray &message)
{
    DWORD dwWriteBytes = 0;

    if ( ! isConnected() ) return 0;

    WriteFile(Port, message.data(), message.size(), &dwWriteBytes, NULL);

    if (dwWriteBytes <= 0)
    {
        log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                    +QString(" sendToKKM - Соединение потеряно!"));
        disconnectFromKKM();
        return 0;
    }

    return readResult();
}

char KKM::readResult()
{
    char pValue[255];
    DWORD dwHasRead = 0;

    if ( ! isConnected() ) return 0;

    ReadFile(Port, pValue, 1, &dwHasRead, NULL);
    if (dwHasRead > 0)
        inbufer.append(pValue, dwHasRead);
    else
    {
        log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                    +QString(" readResult - Соединение потеряно!"));
        disconnectFromKKM();
        return 0;
    }

    log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
               +QString(" readResult - inbuffer!")+inbufer.toHex());

    if (inbufer.at(0) == KKM::ACK)
    {
        inbufer.remove(0,1);
        return KKM::ACK;
    }
    else if (inbufer.at(0) == KKM::NAK)
    {
        inbufer.remove(0,1);
        return KKM::NAK;
    }
    else
        return 0;
}

QByteArray KKM::readMessage_old()
{
    char pValue[255];
    DWORD dwHasRead;
    QByteArray recMessage;

    if ( ! isConnected() ) return NULL;

    QTime time_end;
    QTime current_time;
    time_end = QTime::currentTime().addMSecs(this->timeout);
    current_time = QTime::currentTime();
    dwHasRead = SetFilePointer(Port, 0, 0, FILE_BEGIN);
    while (inbufer.size() <= 0 && current_time <= time_end)
    {
        ReadFile(Port, pValue, 255, &dwHasRead, NULL);
        current_time = QTime::currentTime();
        if (dwHasRead > 0)
            inbufer.append(pValue, dwHasRead);
    }

    log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                +QString(" readMessage1 - inbuffer: ")+inbufer.toHex());

    if (current_time > time_end)
    {
        log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                    +QString(" readMessage1 - Соединение потеряно!"));
        disconnectFromKKM();
        return NULL;
    }

    if (inbufer.at(0) == KKM::STX)
    {
        inbufer.remove(0,1);

        time_end = QTime::currentTime().addMSecs(this->timeout);
        current_time = QTime::currentTime();
        dwHasRead = SetFilePointer(Port, 0, 0, FILE_BEGIN);
        while (inbufer.size() <= 0 && current_time <= time_end)
        {
            ReadFile(Port, pValue, 255, &dwHasRead, NULL);
            current_time = QTime::currentTime();
            if (dwHasRead > 0)
                inbufer.append(pValue, dwHasRead);
        }

        if (current_time > time_end)
        {
            log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                        +QString(" readMessage2 - Соединение потеряно!"));
            disconnectFromKKM();
            return NULL;
        }

        //длинна сообщения
        unsigned char length = inbufer.at(0);
        recMessage.append(inbufer.at(0));
        inbufer.remove(0,1);

        for (int i=0; i<length; i++)
        {
            time_end = QTime::currentTime().addMSecs(this->timeout);
            current_time = QTime::currentTime();
            dwHasRead = SetFilePointer(Port, 0, 0, FILE_BEGIN);
            while (inbufer.size() <= 0 && current_time <= time_end)
            {
                ReadFile(Port, pValue, 255, &dwHasRead, NULL);
                current_time = QTime::currentTime();
                if (dwHasRead > 0)
                    inbufer.append(pValue, dwHasRead);
            }

            if (current_time > time_end)
            {
                log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                            +QString(" readMessage3 - Соединение потеряно!"));
                disconnectFromKKM();
                return NULL;
            }

            recMessage.append(inbufer.at(0));
            inbufer.remove(0,1);
        }

        //подсчет контрольной суммы
        char LRC = recMessage.at(0);
        for (int i=1; i<recMessage.size(); i++)
        {
            LRC = LRC ^ recMessage.at(i);
        }

        //считываем контрольную сумму
        time_end = QTime::currentTime().addMSecs(this->timeout);
        current_time = QTime::currentTime();
        dwHasRead = SetFilePointer(Port, 0, 0, FILE_BEGIN);
        while (inbufer.size() <= 0 && current_time <= time_end)
        {
            ReadFile(Port, pValue, 255, &dwHasRead, NULL);
            current_time = QTime::currentTime();
            if (dwHasRead > 0)
                inbufer.append(pValue, dwHasRead);
        }

        if (current_time > time_end)
        {
            log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                        +QString(" readMessage4 - Соединение потеряно!"));
            disconnectFromKKM();
            return NULL;
        }

        Sleep(100);

        //проверяем контрольную сумму
        if (inbufer.at(0) == LRC)
        {
            int attempt = 0;
            while (attempt < 10 && dwHasRead < 1)
            {
                WriteFile(Port, &KKM::ACK, 1, &dwHasRead, NULL);
                attempt++;
            }

            if (dwHasRead < 1)
            {
                log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                            +QString(" readMessage5 - Соединение потеряно!"));
                disconnectFromKKM();
                return NULL;
            }

            if (recMessage.at(2) != 0)
                log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                            +QString(" - FK: ")
                            +GetErrorText(recMessage.at(2)));
        }
        else
        {
            WriteFile(Port, &KKM::NAK, 1, &dwHasRead, NULL);
            log.append(QString("Получение сообщения: отправка NAK ")
                       +QString::number(dwHasRead));
        }
        inbufer.remove(0,1);
    }

    Sleep(100);

    return recMessage;
}

QByteArray KKM::readMessage_new()
{
    char pValue[255];
    DWORD dwHasRead;
    QByteArray recMessage;

    if ( ! isConnected() ) return NULL;

    CommTimeOuts.ReadIntervalTimeout= 100;
    CommTimeOuts.ReadTotalTimeoutMultiplier = 100;
    CommTimeOuts.ReadTotalTimeoutConstant = 10000;
    CommTimeOuts.WriteTotalTimeoutMultiplier = 100;
    CommTimeOuts.WriteTotalTimeoutConstant = 0;
    SetCommTimeouts(Port, &CommTimeOuts);

    ReadFile(Port, pValue, 1, &dwHasRead, NULL);
    if (dwHasRead > 0)
        inbufer.append(pValue, dwHasRead);
    else
    {
        log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                    +QString(" readMessage1 - Соединение потеряно!"));
        disconnectFromKKM();
        return NULL;
    }

    if (inbufer.at(0) == KKM::STX)
    {
        inbufer.clear();

        ReadFile(Port, pValue, 1, &dwHasRead, NULL);
        if (dwHasRead > 0)
            inbufer.append(pValue, dwHasRead);
        else
        {
            log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                        +QString(" readMessage2 - Соединение потеряно!"));
            disconnectFromKKM();
            return NULL;
        }

        //длинна сообщения
        unsigned char length = inbufer.at(0);
        recMessage.append(inbufer.at(0));
        inbufer.clear();

        CommTimeOuts.ReadIntervalTimeout= 100;
        CommTimeOuts.ReadTotalTimeoutMultiplier = 100;
        CommTimeOuts.ReadTotalTimeoutConstant = 1;
        CommTimeOuts.WriteTotalTimeoutMultiplier = 100;
        CommTimeOuts.WriteTotalTimeoutConstant = 0;
        SetCommTimeouts(Port, &CommTimeOuts);

        ReadFile(Port, pValue, length, &dwHasRead, NULL);
        if (dwHasRead > 0)
            inbufer.append(pValue, dwHasRead);
        else
        {
            log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                        +QString(" readMessage3 - Соединение потеряно!"));
            disconnectFromKKM();
            return NULL;
        }

        recMessage.append(inbufer);
        inbufer.clear();


        //подсчет контрольной суммы
        char LRC = recMessage.at(0);
        for (int i=1; i<recMessage.size(); i++)
        {
            LRC = LRC ^ recMessage.at(i);
        }

        //считываем контрольную сумму
        ReadFile(Port, pValue, 1, &dwHasRead, NULL);
        if (dwHasRead > 0)
            inbufer.append(pValue, dwHasRead);
        else
        {
            log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                        +QString(" readMessage4 - Соединение потеряно!"));
            disconnectFromKKM();
            return NULL;
        }

        //проверяем контрольную сумму
        if (inbufer.at(0) == LRC)
        {
            int attempt = 0;
            dwHasRead = 0;
            while (attempt < 10 && dwHasRead < 1)
            {
                WriteFile(Port, &KKM::ACK, 1, &dwHasRead, NULL);
                attempt++;
            }

            if (dwHasRead < 1)
            {
                log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                            +QString(" readMessage5 - Соединение потеряно!"));
                disconnectFromKKM();
                return NULL;
            }

            if (recMessage.at(2) != 0)
                log.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                            +QString(" - FK: ")
                            +recMessage.toHex()
                            +GetErrorText(recMessage.at(2)));
        }
        else
        {
            WriteFile(Port, &KKM::NAK, 1, &dwHasRead, NULL);
            log.append(QString("Получение сообщения: отправка NAK ")
                       +QString::number(dwHasRead));
        }
        inbufer.remove(0,1);
    }

    return recMessage;
}


QString KKM::getLog()
{
    return log.join(QByteArray::fromHex("0d0a"));
}

void KKM::setPass(DWORD value)
{
    pass = value;
}


/*
KKM::Comand::Comand()
{
    length = 0;
}

void KKM::Comand::addParam(uint value)
{
    params.append(QVariant(value));
}

void KKM::Comand::prepare()
{
    int length;

    this->clear();

    QList<QVariant>::iterator it;
    for (it = params.begin(); it != params.end(); ++it)
    {
        if (it->type() == QVariant::UInt)
            length +=4;
    }

    data = new unsigned char[length];
}

void KKM::Comand::clear()
{
    if (length > 0)
        delete data;
    length = 0;
    params.clear();
}

*/


QString KKM::GetErrorText(int value)
{
        switch (value){

                case 0x00: return QObject::tr("Ошибок нет");
                case 0x01: return QObject::tr("Неисправен накопитель ФП 1, ФП 2 или часы");
                case 0x02: return QObject::tr("Отсутствует ФП 1");
                case 0x03: return QObject::tr("Отсутствует ФП 2");
                case 0x04: return QObject::tr("Некорректные параметры в команде обращения к ФП");
                case 0x05: return QObject::tr("Нет запрошенных данных");
                case 0x06: return QObject::tr("ФП в режиме вывода данных");
                case 0x07: return QObject::tr("Некорректные параметры в команде для данной реализации ФП");
                case 0x08: return QObject::tr("Команда не поддерживается в данной реализации ФП");
                case 0x09: return QObject::tr("Некорректная длина команды");
                case 0x0A: return QObject::tr("Формат данных не BCD");
                case 0x0B: return QObject::tr("Неисправна ячейка памяти ФП при записи итога");
// 0x10                           QObject::
                case 0x11: return QObject::tr("Не введена лицензия");
                case 0x12: return QObject::tr("Заводской номер уже введен");
                case 0x13: return QObject::tr("Текущая дата меньше даты последней записи в ФП");
                case 0x14: return QObject::tr("Область сменных итогов ФП переполнена");
                case 0x15: return QObject::tr("Смена уже открыта");
                case 0x16: return QObject::tr("Смена не открыта");
                case 0x17: return QObject::tr("Номер первой смены больше номера последней смены");
                case 0x18: return QObject::tr("Дата первой смены больше даты последней смены");
                case 0x19: return QObject::tr("Нет данных в ФП");
                case 0x1A: return QObject::tr("Область перерегистраций в ФП переполнена");
                case 0x1B: return QObject::tr("Заводской номер не введен");
                case 0x1C: return QObject::tr("В заданном диапазоне есть поврежденная запись");
                case 0x1D: return QObject::tr("Повреждена последняя запись сменных итогов");
                case 0x1F: return QObject::tr("Отсутствует память регистров");
// 0x20                           QObject::
                case 0x20: return QObject::tr("Переполнение денежного регистра при добавлении");
                case 0x21: return QObject::tr("Вычитаемая сумма больше содержимого денежного регистра");
                case 0x22: return QObject::tr("Неверная дата");
                case 0x23: return QObject::tr("Нет записи активизации");
                case 0x24: return QObject::tr("Область активизаций переполнена");
                case 0x25: return QObject::tr("Нет активизиции с запрашиваемым номером");
                case 0x2F: return QObject::tr("ЭКЛЗ не отвечает");
// 0x30                           QObject::
                case 0x30: return QObject::tr("ЭКЛЗ ответила NAK");
                case 0x31: return QObject::tr("ЭКЛЗ: ошибка формата");
                case 0x32: return QObject::tr("ЭКЛЗ: ошибка контрольной суммы");
                case 0x33: return QObject::tr("Некорректные параметры в команде");
                case 0x35: return QObject::tr("Некорректный параметр при данных настройках");
                case 0x36: return QObject::tr("Некорректные параметры в команде для данной реализации ФР");
                case 0x37: return QObject::tr("Команда не поддерживается в данной реализации ФР");
                case 0x38: return QObject::tr("Ошибка в ПЗУ");
                case 0x39: return QObject::tr("Внутренняя ошибка ПО ФР");
                case 0x3A: return QObject::tr("Переполнение накопления по надбавкам в смене");
                case 0x3C: return QObject::tr("ЭКЛЗ: Неверный регистрационный номер");
                case 0x3E: return QObject::tr("Переполнение накопления по секциям в смене");
                case 0x3F: return QObject::tr("Переполнение накопления по скидкам в смене");
// 0x40                           QObject::
                case 0x40: return QObject::tr("Переполнение диапазона скидок");
                case 0x41: return QObject::tr("Переполнение диапазона оплаты наличными");
                case 0x42: return QObject::tr("Переполнение диапазона оплаты типом 2");
                case 0x43: return QObject::tr("Переполнение диапазона оплаты типом 3");
                case 0x44: return QObject::tr("Переполнение диапазона оплаты типом 4");
                case 0x45: return QObject::tr("Cумма всех типов оплаты меньше итога чека");
                case 0x46: return QObject::tr("Не хватает наличности в кассе");
                case 0x47: return QObject::tr("Переполнение накопления по налогам в смене");
                case 0x48: return QObject::tr("Переполнение итога чека");
                case 0x49: return QObject::tr("Операция невозможна в открытом чеке данного типа");
                case 0x4A: return QObject::tr("Открыт чек - операция невозможна");
                case 0x4B: return QObject::tr("Буфер чека переполнен");
                case 0x4C: return QObject::tr("Переполнение накопления по обороту налогов в смене");
                case 0x4D: return QObject::tr("Вносимая безналичной оплатой сумма больше суммы чека");
                case 0x4E: return QObject::tr("Смена превысила 24 часа");
                case 0x4F: return QObject::tr("Неверный пароль");
// 0x50                           QObject::
                case 0x50: return QObject::tr("Идет печать предыдущей команды");
                case 0x51: return QObject::tr("переполнение накоплений наличными в смене");
                case 0x52: return QObject::tr("переполнение накоплений по типу оплаты 2 в смене");
                case 0x53: return QObject::tr("переполнение накоплений по типу оплаты 3 в смене");
                case 0x54: return QObject::tr("переполнение накоплений по типу оплаты 4 в смене");
                case 0x56: return QObject::tr("Нет документа для повтора");
                case 0x57: return QObject::tr("ЭКЛЗ: Количество закрытых смен не совпадает с ФП");
                case 0x58: return QObject::tr("Ожидание команды продолжения печати");
                case 0x59: return QObject::tr("Документ открыт другим оператором");
                case 0x5A: return QObject::tr("Скидка превышает накопления в чеке");
                case 0x5B: return QObject::tr("Переполнение диапазона надбавок");
                case 0x5C: return QObject::tr("Понижено напряжение 24В");
                case 0x5D: return QObject::tr("Таблица не определена");
                case 0x5E: return QObject::tr("Некорректная операция");
                case 0x5F: return QObject::tr("Отрицательный итог чека");
// 0x60                           QObject::
                case 0x60: return QObject::tr("Переполнение при умножении");
                case 0x61: return QObject::tr("Переполнение диапазона цены");
                case 0x62: return QObject::tr("Переполнение диапазона количества");
                case 0x63: return QObject::tr("Переполнение диапазона отдела");
                case 0x64: return QObject::tr("ФП отсутствует");
                case 0x65: return QObject::tr("Не хватает денег в секции");
                case 0x66: return QObject::tr("Переполнение денег в секции");
                case 0x67: return QObject::tr("Ошибка связи с ФП");
                case 0x68: return QObject::tr("Не хватает денег по обороту налогов");
                case 0x69: return QObject::tr("Переполнение денег по обороту налогов");
                case 0x6A: return QObject::tr("Ошибка питания в момент ответа по I2C");
                case 0x6B: return QObject::tr("Нет чековой ленты");
                case 0x6C: return QObject::tr("Нет контрольной ленты");
                case 0x6D: return QObject::tr("Не хватает денег по налогу");
                case 0x6E: return QObject::tr("Переполнение денег по налогу");
                case 0x6F: return QObject::tr("Переполнение по выплате в смене");
// 0x70                           QObject::
                case 0x70: return QObject::tr("Переполнение ФП");
                case 0x71: return QObject::tr("Ошибка отрезчика");
                case 0x72: return QObject::tr("Команда не поддерживается в данном подрежиме");
                case 0x73: return QObject::tr("Команда не поддерживается в данном режиме");
                case 0x74: return QObject::tr("Ошибка ОЗУ");
                case 0x75: return QObject::tr("Ошибка питания");
                case 0x76: return QObject::tr("Ошибка принтера: нет импульсов с тахогенератора");
                case 0x77: return QObject::tr("Ошибка принтера: нет сигнала с датчиков");
                case 0x78: return QObject::tr("Замена ПО");
                case 0x79: return QObject::tr("Замена ФП");
                case 0x7A: return QObject::tr("Поле не редактируется");
                case 0x7B: return QObject::tr("Ошибка оборудования");
                case 0x7C: return QObject::tr("Не совпадает дата");
                case 0x7D: return QObject::tr("Неверный формат даты");
                case 0x7E: return QObject::tr("Неверное значение в поле длины");
                case 0x7F: return QObject::tr("Переполнение диапазона итога");
// 0x80                           QObject::
                case 0x80: return QObject::tr("Ошибка связи с ФП");
                case 0x81: return QObject::tr("Ошибка связи с ФП");
                case 0x82: return QObject::tr("Ошибка связи с ФП");
                case 0x83: return QObject::tr("Ошибка связи с ФП");
                case 0x84: return QObject::tr("Переполнение наличности");
                case 0x85: return QObject::tr("Переполнение по продажам в смене");
                case 0x86: return QObject::tr("Переполнение по покупкам в смене");
                case 0x87: return QObject::tr("Переполнение по возвратам продаж в смене");
                case 0x88: return QObject::tr("Переполнение по возвратам покупок в смене");
                case 0x89: return QObject::tr("Переполнение по внесению в смене");
                case 0x8A: return QObject::tr("Переполнение по надбавкам в чеке");
                case 0x8B: return QObject::tr("Переполнение по скидкам в чеке");
                case 0x8C: return QObject::tr("Отрицательный итог надбавки в чеке");
                case 0x8D: return QObject::tr("Отрицательный итог скидки в чеке");
                case 0x8E: return QObject::tr("Нулевой итог чека");
                case 0x8F: return QObject::tr("Касса не фискализирована");
// 0x90                           QObject::
                case 0x90: return QObject::tr("Поле превышает размер установленный в настройках");
                case 0x91: return QObject::tr("Выход за границу поля печати при данных настройках шрифта");
                case 0x92: return QObject::tr("Наложение полей");
                case 0x93: return QObject::tr("Восстановление ОЗУ прошло успешно");
// 0xA0                           QObject::
                case 0xA0: return QObject::tr("Ошибка связи с ЭКЛЗ");
                case 0xA1: return QObject::tr("ЭКЛЗ отсутствует");
                case 0xA2: return QObject::tr("ЭКЛЗ: Некорректный формат или параметр команды");
                case 0xA3: return QObject::tr("Некорректное состояние ЭКЛЗ");
                case 0xA4: return QObject::tr("Авария ЭКЛЗ");
                case 0xA5: return QObject::tr("Авария КС в составе ЭКЛЗ");
                case 0xA6: return QObject::tr("Исчерпан временной ресурс ЭКЛЗ");
                case 0xA7: return QObject::tr("ЭКЛЗ переполнена");
                case 0xA8: return QObject::tr("ЭКЛЗ: Неверные дата или время");
                case 0xA9: return QObject::tr("ЭКЛЗ: Нет запрошенных данных");
                case 0xAA: return QObject::tr("Переполнение ЭКЛЗ (отрицательный итог документа)");
// 0xB0                           QObject::
                case 0xB0: return QObject::tr("ЭКЛЗ: Переполнение в параметре количество");
                case 0xB1: return QObject::tr("ЭКЛЗ: Переполнение в параметре сумма");
                case 0xB2: return QObject::tr("ЭКЛЗ: Уже активизирована");
// 0xC0                           QObject::
                case 0xC0: return QObject::tr("Контроль даты и времени (подтвердите дату и время)");
                case 0xC1: return QObject::tr("ЭКЛЗ: суточный отчет с гашением прервать нельзя");
                case 0xC2: return QObject::tr("Превышение напряжения блока питания");
                case 0xC3: return QObject::tr("Несовпадение итогов чека с ЭКЛЗ");
                case 0xC4: return QObject::tr("Несовпадение номеров смен");
                case 0xC5: return QObject::tr("Буфер подкладного документа пуст");
                case 0xC6: return QObject::tr("Подкладной документ отсутствует");
                case 0xC7: return QObject::tr("Поле не редактируется в данном режиме");
                case 0xC8: return QObject::tr("Ошибка связи с принтером");
                default: return QObject::tr("Неизвестная ошибка");
        }
}

QString KKM::GetStateText(char state, char substate)
{
    switch (state){
        case 0: return QObject::tr("Ошибок нет");
        case 1: return QObject::tr("Выдача данных.");
        case 2: return QObject::tr("Открытая смена, 24 часа не кончились.");
        case 3: return QObject::tr("Открытая смена, 24 часа кончились.");
        case 4: return QObject::tr("Закрытая смена.");
        case 5: return QObject::tr("Блокировка по неправильному паролю налогового инспектора");
        case 6: return QObject::tr("Ожидание подтверждения ввода даты.");
        case 7: return QObject::tr("Разрешение изменения положения десятичной точки.");
        case 8:      if (substate == 0) return QObject::tr("Открытый документ: Продажа.");
                else if (substate == 1) return QObject::tr("Открытый документ: Покупка.");
                else if (substate == 2) return QObject::tr("Открытый документ: Возврат продажи.");
                else if (substate == 3) return QObject::tr("Открытый документ: Возврат покупки.");
                else return QObject::tr("Открытый документ: ХЗ.");
        case 9: return QObject::tr("Режим разрешения технологического обнуления.");
        case 10: return QObject::tr("Тестовый прогон.");
        case 11: return QObject::tr("Печать полного фис. отчета.");
        case 12: return QObject::tr("Печать отчёта ЭКЛЗ.");
        case 13:     if (substate == 0) return QObject::tr("Работа с фискальным подкладным документом: Продажа (открыт).");
                else if (substate == 1) return QObject::tr("Работа с фискальным подкладным документом: Покупка (открыт).");
                else if (substate == 2) return QObject::tr("Работа с фискальным подкладным документом: Возврат продажи (открыт).");
                else if (substate == 3) return QObject::tr("Работа с фискальным подкладным документом: Возврат покупки (открыт).");
                else return QObject::tr("Работа с фискальным подкладным документом: ХЗ.");
        case 14:     if (substate == 0) return QObject::tr("Печать подкладного документа: Ожидание загрузки.");
                else if (substate == 1) return QObject::tr("Печать подкладного документа: Загрузка и позиционирование.");
                else if (substate == 2) return QObject::tr("Печать подкладного документа: Позиционирование.");
                else if (substate == 3) return QObject::tr("Печать подкладного документа: Печать.");
                else if (substate == 4) return QObject::tr("Печать подкладного документа: Печать закончена.");
                else if (substate == 5) return QObject::tr("Печать подкладного документа: Выброс документа.");
                else if (substate == 6) return QObject::tr("Печать подкладного документа: Ожидание извлечения.");
                else return QObject::tr("Печать подкладного документа: ХЗ.");
        case 15: return QObject::tr("Фискальный подкладной документ сформирован.");
        default: return QObject::tr("Неизвестый режим.");
    }
}
