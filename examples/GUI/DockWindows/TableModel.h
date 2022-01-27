//
// Created by SVK on 27.01.2022.
//

#ifndef EXRT_TABLEMODEL_H
#define EXRT_TABLEMODEL_H
#include <QAbstractTableModel>


class TableModel : public QAbstractTableModel {
   private:
    int rowCount(const QModelIndex &parent) const override {
        return 5;  // сделаем фиксированно 5 строк в таблице
        //если вы станете использовать скажем QList, то пишите return list.size();
    }
    int columnCount(const QModelIndex &parent) const override {
        return 4;  // количество колонок сделаем также фиксированным
    }

    QVariant data(const QModelIndex &index, int role) const {
        if (role == Qt::DisplayRole) {
            QString answer =
                QString("row = ") + QString::number(index.row()) + "  col = " + QString::number(index.column());
            // строкой выше мы формируем ответ. QString::number преобразует число в текст
            return QVariant(answer);
        }
        return QVariant();
    }
};



#endif  // EXRT_TABLEMODEL_H
