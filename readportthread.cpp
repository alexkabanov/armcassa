#include "readportthread.h"
#include <QtGlobal>
#include <QByteArray>

ReadPortThread::ReadPortThread(QObject *parent)
            : QThread(parent)
{
    stopped = false;
    //wind = NULL;
}

void ReadPortThread::setPort(int value)
{
    this->port = value;
}

void ReadPortThread::setPrefix(const QString & value)
{
    QByteArray byteArray;
    byteArray = QByteArray::fromHex(value.toLocal8Bit());
    this->prefix = QString::fromAscii(byteArray.data());
}

void ReadPortThread::setSuffix(const QString & value)
{
    QByteArray byteArray;
    byteArray = QByteArray::fromHex(value.toLocal8Bit());
    this->suffix = QString::toLocal8Bit(byteArray.data());

}

void ReadPortThread::run()
{    
    HANDLE Port;
    DCB dcb;
    COMMTIMEOUTS CommTimeOuts;



    CloseHandle(Port);
    Port = CreateFileA(tr("\\\\.\\COM").append(QString().number(this->port)).toAscii(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (Port == INVALID_HANDLE_VALUE)
    {
        MessageBoxA(NULL, "���������� ������� ���������������� ����", "Error", MB_OK);
        return;
    }

    if(GetCommState(Port, &dcb)==0)
    {
        CloseHandle(Port);
        MessageBoxA(NULL, "������ ��������� ���������� �����", "Error", MB_OK);
        // ������ !
    }

    dcb.DCBlength         = 28;
    dcb.BaudRate          = 9600;
    dcb.fBinary           = 1;
    dcb.fParity           = 0;
    dcb.fOutxCtsFlow      = 0;
    dcb.fOutxDsrFlow      = 0;
    dcb.fDtrControl       = 1;
    dcb.fDsrSensitivity   = 0;
    dcb.fTXContinueOnXoff = 0;
    dcb.fOutX             = 1;
    dcb.fInX              = 1;
    dcb.fErrorChar        = 0;
    dcb.fNull             = 0;
    dcb.fRtsControl       = 1;
    dcb.fAbortOnError     = 0;
    dcb.fDummy2           = 0;
    dcb.wReserved         = 0;
    dcb.XonLim            = 0;
    dcb.XoffLim           = 0;
    dcb.ByteSize          = 8;
    dcb.Parity            = 0;
    dcb.StopBits          = 0;
    dcb.XonChar           = 0;
    dcb.XoffChar          = 0;
    dcb.ErrorChar         = 0;
    dcb.EofChar           = 0;
    dcb.EvtChar           = 0;
    dcb.wReserved1        = 0;

    CommTimeOuts.ReadIntervalTimeout= 10;
    CommTimeOuts.ReadTotalTimeoutMultiplier = 1;
    // �������� ���� ���� � ����� ������ ������� ��� ���������� ������
    // ���� �� �������� 110 ���
    CommTimeOuts.ReadTotalTimeoutConstant = 100;
    // ������������ � ������ ������ ��� ����� �������� �������
    CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
    CommTimeOuts.WriteTotalTimeoutConstant = 0;
    SetCommTimeouts(Port, &CommTimeOuts);


    PurgeComm(Port, PURGE_RXCLEAR);
    PurgeComm(Port, PURGE_TXCLEAR);

    SetCommState(Port,&dcb);

    DWORD dwHasRead;
    char pValue[255];

    QString str;
    QString suff;
    suff = this->suffix;


    stopped = false;
    while (! stopped)
    {
        ReadFile(Port, pValue, 255, &dwHasRead, NULL);
        if (dwHasRead > 0)
        {
            str.append(QString::toLocal8Bit(pValue, dwHasRead));

            //str.append(QString::number(dwHasRead));
            //barcode.changeBarcode(str);

            /*QByteArray hex1;
            hex1 = QByteArray(pValue, dwHasRead);*/

            QString barcode;
            int end_index;
            end_index = str.indexOf(suff); //� ����� ������
            while (end_index >= 0)
            {
                barcode = str.left(end_index);
                barcode = barcode.remove(QChar('F'), Qt::CaseSensitive);
                emit appProduct(barcode);
                //MessageBoxA(NULL, barcode.toAscii().data(), "", MB_OK);
                str.remove(0, end_index + suff.length());
                end_index = str.indexOf(suff); //� ����� ������
            }
        }
    }

    CloseHandle(Port);
}

void ReadPortThread::stop()
{
    stopped = true;
}

