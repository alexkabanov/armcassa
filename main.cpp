#include <QApplication>
#include "mainsales.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec *codec1251 = QTextCodec::codecForName("CP1251");
    QTextCodec::setCodecForLocale(codec1251);

    MainSales w;
    w.show();
    return a.exec();
}
