#ifndef KEYPRESSEATER_H
#define KEYPRESSEATER_H

#include <QObject>

class KeyPressEater : public QObject
{
    Q_OBJECT

public:
    KeyPressEater(QObject * parent);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // KEYPRESSEATER_H
