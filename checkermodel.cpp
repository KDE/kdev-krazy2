/*
 * This file is part of KDevelop Krazy2 Plugin.
 *
 * Copyright 2012 Daniel Calviño Sánchez <danxuliu@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "checkermodel.h"

#include <KLocalizedString>

#include "checker.h"

class Item {
public:

    enum ItemType {
        FileTypeItem,
        CheckerItem,
        ExtraHeaderItem
    };

    Item():
        m_parent(0),
        m_checker(0) {
    }

    ~Item() {
        qDeleteAll(m_children);
    }

    ItemType type() const {
        return m_type;
    }

    void setType(ItemType type) {
        m_type = type;
    }

    const QList<Item*>& children() const {
        return m_children;
    }

    void addChild(Item* item) {
        item->setParent(this);
        m_children.append(item);
    }

    Item* parent() const {
        return m_parent;
    }

    void setParent(Item* parent) {
        m_parent = parent;
    }

    const QString& fileType() const {
        return m_fileType;
    }

    void setFileType(const QString& fileType) {
        m_fileType = fileType;
    }

    const Checker* checker() const {
        return m_checker;
    }

    void setChecker(const Checker* checker) {
        m_checker = checker;
    }

private:

    ItemType m_type;
    QList<Item*> m_children;
    Item* m_parent;

    QString m_fileType;
    const Checker* m_checker;

};

//public:

CheckerModel::CheckerModel(QObject* parent /*= 0*/):
    QAbstractItemModel(parent) {
}

CheckerModel::~CheckerModel() {
    qDeleteAll(m_fileTypeItems);
}

QModelIndex CheckerModel::index(int row, int column, const QModelIndex& parent /* = QModelIndex()*/) const {
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    Item* item;
    if (!parent.isValid()) {
        item = m_fileTypeItems.at(row);
    } else {
        item = static_cast<Item*>(parent.internalPointer())->children().at(row);
    }

    return createIndex(row, column, item);
}

QModelIndex CheckerModel::parent(const QModelIndex& index) const {
    if (!index.isValid()) {
        return QModelIndex();
    }

    Item* childItem = static_cast<Item*>(index.internalPointer());
    Item* parentItem = childItem->parent();

    if (!parentItem) {
        return QModelIndex();
    }

    int row;
    if (parentItem->type() == Item::FileTypeItem) {
        row = m_fileTypeItems.indexOf(parentItem);
    } else {
        row = parentItem->parent()->children().indexOf(parentItem);
    }

    return createIndex(row, 0, parentItem);
}

int CheckerModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const {
    if (parent.column() > 0) {
        return 0;
    }

    if (!parent.isValid()) {
        return m_fileTypeItems.count();
    }

    return static_cast<Item*>(parent.internalPointer())->children().count();
}

int CheckerModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const {
    Q_UNUSED(parent);

    return 1;
}

QVariant CheckerModel::data(const QModelIndex& index, int role /*= Qt::DisplayRole*/) const {
    if (!index.isValid()) {
        return QVariant();
    }

    if (role != Qt::DisplayRole && role != Qt::ToolTipRole) {
        return QVariant();
    }

    Item* item = static_cast<Item*>(index.internalPointer());
    if (role == Qt::ToolTipRole) {
        if (item->type() != Item::CheckerItem) {
            return QVariant();
        }

        if (item->checker()->description().isEmpty()) {
            return i18nc("@info:tooltip The underlying command did not provide a text output", 
                         "Sorry, no description given");
        }

        return "<para>" + item->checker()->description() + "</para>";
    }

    if (item->type() == Item::FileTypeItem) {
        return item->fileType();
    }

    if (item->type() == Item::ExtraHeaderItem) {
        return "[EXTRA]";
    }

    return item->checker()->name();
}

const Checker* CheckerModel::checkerForIndex(const QModelIndex& index) const {
    if (!index.isValid()) {
        return 0;
    }

    Item* item = static_cast<Item*>(index.internalPointer());
    if (item->type() == Item::FileTypeItem || item->type() == Item::ExtraHeaderItem) {
        return 0;
    }

    return item->checker();
}

void CheckerModel::setCheckers(const QList< const Checker* >& checkers) {
    beginResetModel();

    QMultiMap<QString, const Checker*> checkersByFileType;
    foreach (const Checker* checker, checkers) {
        checkersByFileType.insert(checker->fileType(), checker);
    }

    qDeleteAll(m_fileTypeItems);
    m_fileTypeItems.clear();

    //Keys are returned in ascending order
    foreach (const QString& fileType, checkersByFileType.uniqueKeys()) {
        Item* fileTypeItem = new Item();
        fileTypeItem->setType(Item::FileTypeItem);
        fileTypeItem->setFileType(fileType);
        m_fileTypeItems.append(fileTypeItem);

        QMap<QString, Item*> checkerItems;
        QMap<QString, Item*> extraCheckerItems;
        foreach (const Checker* checker, checkersByFileType.values(fileType)) {
            Item* checkerItem = new Item();
            checkerItem->setType(Item::CheckerItem);
            checkerItem->setChecker(checker);

            if (!checker->isExtra()) {
                checkerItems.insert(checker->name(), checkerItem);
            } else {
                extraCheckerItems.insert(checker->name(), checkerItem);
            }
        }

        //Values are returned in ascending order of their keys
        foreach (Item* checkerItem, checkerItems.values()) {
            fileTypeItem->addChild(checkerItem);
        }

        if (!extraCheckerItems.values().isEmpty()) {
            Item* extraHeaderItem = new Item();
            extraHeaderItem->setType(Item::ExtraHeaderItem);
            extraHeaderItem->setFileType(fileTypeItem->fileType());
            fileTypeItem->addChild(extraHeaderItem);

            //Values are returned in ascending order of their keys
            foreach (Item* checkerItem, extraCheckerItems.values()) {
                extraHeaderItem->addChild(checkerItem);
            }
        }
    }

    endResetModel();
}
