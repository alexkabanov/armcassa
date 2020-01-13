#ifndef SALEITEMTABLEVIEW_H
#define SALEITEMTABLEVIEW_H

#include <QSqlQueryModel>
#include <QTableView>

class SaleItemTableView : public QSqlQueryModel {
    Q_OBJECT
public:
    SaleItemTableView(QObject *parent = 0);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;
    /*bool setData(const QModelIndex &index,
                 const QVariant &value, int role);*/
    bool addProduct(int row, float count);
    bool removeRow ( int row, const QModelIndex & parent = QModelIndex() );

private:
    void refresh();
};

#endif // SALEITEMTABLEVIEW_H
