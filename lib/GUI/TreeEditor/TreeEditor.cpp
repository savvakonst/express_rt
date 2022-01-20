
#define _USE_MATH_DEFINES

#include "TreeEditor.h"

TreeEditor::TreeEditor(QWidget *parent) : QTreeWidget(parent), update_signal_(this) {
    this->setAutoFillBackground(true);
    this->setBackgroundRole(QPalette().Base);
    auto p = this->palette();
    p.setColor(backgroundRole(), QColor("#D2DCDF"));
    this->setPalette(p);

    this->setHeaderItem(new QTreeWidgetItem({"", "свойства"}, 4));

    this->setStyleSheet(
        "QHeaderView::section {background-color: #D2DCDF; alternate-background-color: #f6fafb;};"
        //"alternate-background-color: #f6fafb;background-color: #D2DCDF; border-style: outset; border-width: 0px;"
    );

    this->setAlternatingRowColors(true);
}

void TreeEditor::setupProperties(DataSchema_ifs *ds, QTreeWidgetItem *parent_item) {
    auto item = parent_item ? new QTreeWidgetItem(parent_item) : new QTreeWidgetItem(this);

    // item->setFlags(item->flags() | Qt::ItemIsEditable);
    // parent_item->setFlags(parent_item->flags() | Qt::ItemIsTristate |
    //                       Qt::ItemIsUserCheckable);

    auto name = ds->description_.c_str();
    item->setText(0, name);

    if (ds->isMap()) {
        auto map_list = ds->getMapList();
        for (auto i : map_list) {
            setupProperties(i, item);
        }
    } else if (ds->isArray()) {
        // TODO: realise this branch of logic
    } else {
        auto type = ds->getRepresentationType();
        auto cm = constructors_map_.find(type);

        if (cm != constructors_map_.end()) {
            TreeWidgetWrapper_ifs *w;
            auto list = *(*cm).second;
            for (treeWidgetWrapperConstructor wrapperConstructor : list)
                if (w = wrapperConstructor(ds, nullptr)) break;
            if (w) {
                // if (!w->getWidget()->metaObject()->inherits(&QComboBox::staticMetaObject))
                w->getWidget()->setStyleSheet(
                    "QWidget {background-color: rgba(0,0,0,0);border-style: outset; border-width: 0px;}"
                    "QWidget QAbstractItemView {\n"
                    "    background: white;"
                    "}");

                this->setItemWidget(item, 1, w->getWidget());
                w->addSignal(&update_signal_);

                delete w;
            } else {
                auto error = "no compatible constructor for type: \"" + type + "\"\n";
                error_message_ += error;
                item->setText(1, QString::fromStdString(error));
            }
        } else {
            auto error = "unknown type: \"" + type + "\"\n";
            error_message_ += error;
            item->setText(1, QString::fromStdString(error));
        }
    }
}
void TreeEditor::addExtensionUint(ExtensionUnit *uint) {
    if (strcmp(uint->type, "tree_widget_wrapper")) return;

    QString str = QString::fromStdString(uint->name);
    QStringList str_list = str.split('|');

    for (auto &i : str_list) {
        auto type = std::string(i.toStdString());

        auto constructor_ptr = (treeWidgetWrapperConstructor)uint->ptr;

        auto cm = constructors_map_.find(type);
        if (cm == constructors_map_.end()) {
            constructors_map_[type] = new constructorList_t{constructor_ptr};
        } else {
            (*cm).second->push_back(constructor_ptr);
        }
    }
}
