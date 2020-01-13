#include "saleitemtableview.h"

#include <QtGui>
#include <QtSql>


// Таблица базы данных: пользовательская модель и представление

SaleItemTableView::SaleItemTableView(QObject *parent)
                : QSqlQueryModel(parent) {
    refresh();
}

Qt::ItemFlags SaleItemTableView::flags(
        const QModelIndex &index) const {

    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
        flags |= Qt::ItemIsSelectable;
    return flags;
}

QVariant SaleItemTableView::data(
            const QModelIndex &index,
            int role) const {

    QVariant value = QSqlQueryModel::data(index, role);

    switch (role) {

    case Qt::DisplayRole: // Данные для отображения
        if (index.column() == 5 ||
            index.column() == 6)
            return tr("%1 руб.")
                   .arg(value.toDouble(), 0, 'f', 2);
        else
            return value;

    case Qt::EditRole:    // Данные для редактирования
        if (index.column() == 5 ||
            index.column() == 6)
            return value;
        else
            return value;

    case Qt::TextAlignmentRole: // Выравнивание
        if(index.column() == 0
           || index.column() == 1
           || index.column() == 3
           || index.column() == 5
           || index.column() == 6)
            return int(Qt::AlignRight | Qt::AlignVCenter);
        else
            return int(Qt::AlignLeft | Qt::AlignVCenter);

    case Qt::SizeHintRole:  // Размер ячейки
        if (index.column() == 0
        || index.column() == 1)
            return QSize(40, 10);
        else if (index.column() == 2)
            return QSize(200, 10);
        else if (index.column() == 3
                 ||index.column() == 4)
            return QSize(60, 10);
        else if (index.column() == 5)
            return QSize(100, 10);
        else
            return QSize(100, 10);
        qDebug() << "SizeHintRole\n";
    }
    return value;
}
/*
bool SaleItemTableView::setData(
            const QModelIndex &index,
            const QVariant &value,
            int) {
    if (index.column() < 1 || index.column() > 4)
        return false;

    QModelIndex primaryKeyIndex = QSqlQueryModel::index(
                index.row(), 0);
    int id = QSqlQueryModel::data(primaryKeyIndex).toInt();

    //clear(); // Если надо полностью перерисовать таблицу.

    bool ok;
    QSqlQuery query;
    if (index.column() == 1) {
        query.prepare("update employee set name = ? where id = ?");
        query.addBindValue(value.toString());
        query.addBindValue(id);
    }else if(index.column() == 2) {
        query.prepare("update employee set born = ? where id = ?");
        query.addBindValue(value.toDate());
        query.addBindValue(id);
    }else if(index.column() == 3) {
        query.prepare("update employee set salary = ? where id = ?");
        query.addBindValue(value.toDouble());
        query.addBindValue(id);
    }else if(index.column() == 4) {
        query.prepare("update employee set married = ? where id = ?");
        query.addBindValue(value.toBool());
        query.addBindValue(id);
    }
    ok = query.exec();
    refresh();
    return ok;
}*/

bool SaleItemTableView::addProduct(int row, float value)
{
    QModelIndex primaryKeyIndex = QSqlQueryModel::index(
                row, 1);
    QModelIndex countIndex = QSqlQueryModel::index(
                row, 3);
    int id = QSqlQueryModel::data(primaryKeyIndex).toInt();

    float count = QSqlQueryModel::data(countIndex).toFloat();

    bool ok;
    QSqlQuery query;

    if (value+count > 0)
    {
        query.prepare("update sale_items set count=count+? where sale_item_id = ?");
        query.addBindValue(value);
    }
    else
        query.prepare("delete from sale_items where sale_item_id = ?");
    query.addBindValue(id);

    ok = query.exec();

    if ( query.lastError().type() != QSqlError::NoError )
    {
        QMessageBox::critical(
                NULL
                ,QObject::tr("result")
                ,QString("Query: database error: %2")
                    .arg(query.lastError().text())
                ,QMessageBox::Ok
                ,QMessageBox::NoButton);
    }

    refresh();
    return ok;
}

bool SaleItemTableView::removeRow ( int row, const QModelIndex & parent)
{
    QModelIndex primaryKeyIndex = QSqlQueryModel::index(
                row, 1);
    QModelIndex countIndex = QSqlQueryModel::index(
                row, 3);
    int id = QSqlQueryModel::data(primaryKeyIndex).toInt();

    int count = QSqlQueryModel::data(countIndex).toInt();

    bool ok;
    QSqlQuery query;

    /*if (count > 1)
        query.prepare("update sale_items set count=count-1 where sale_item_id = ?");
    else*/
    query.prepare("delete from sale_items where sale_item_id = ?");
    query.addBindValue(id);

    ok = query.exec();

    if ( query.lastError().type() != QSqlError::NoError )
    {
        QMessageBox::critical(
                NULL
                ,QObject::tr("result")
                ,QString("Query: database error: %2")
                    .arg(query.lastError().text())
                ,QMessageBox::Ok
                ,QMessageBox::NoButton);
    }

    refresh();
    return ok;
}

void SaleItemTableView::refresh() {
    setHeaderData(0, Qt::Horizontal,
                  QObject::tr("№"));
    setHeaderData(1, Qt::Horizontal,
                  QObject::tr("Id"));
    setHeaderData(2, Qt::Horizontal,
                  QObject::tr("Нименование товара"));
    setHeaderData(3, Qt::Horizontal,
                  QObject::tr("Кол-во"));
    setHeaderData(4, Qt::Horizontal,
                  QObject::tr("Ед."));
    setHeaderData(5, Qt::Horizontal,
                  QObject::tr("Цена"));
}

