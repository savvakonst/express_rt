//
// Created by SVK on 27.01.2022.
//

#ifndef EXRT_TABLEMODEL_H
#define EXRT_TABLEMODEL_H
#include <QAbstractTableModel>

#include "convtemplate/ConversionTemplate.h"

class TableModel : public QAbstractTableModel {
   public:

    TableModel():conv_template_cnt_(0) {
    }

    TableModel(std::list<ConversionTemplate* > &list ):conv_template_cnt_(list.size()) {
        //conversion_template.
    }


    int rowCount(const QModelIndex &parent) const override {
        return conv_template_cnt_;
    }

    int columnCount(const QModelIndex &parent) const override {
        return 4;
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
        if (role == Qt::DisplayRole) return QString("column = ") + QString::number(section);
        return QVariant();
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

   protected:
    size_t  conv_template_cnt_ = 0;
};

#endif  // EXRT_TABLEMODEL_H
