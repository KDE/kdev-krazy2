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

#ifndef CHECKERMODEL_H
#define CHECKERMODEL_H

#include <QAbstractItemModel>

class Checker;
class Item;

/**
 * A tree model for a list of checkers.
 * The model is a read only model. It provides three levels of items: the first
 * level are the file types, the second level are the checkers, and the third
 * level are the extra checkers. The last item in each second level group is a
 * header for the extra items. That is the only item out of order; all the other
 * items with the same parent are sorted alphabetically. Only one column is
 * shown for each item in all levels.
 *
 * An example of the structure of this model:
 * -cpp
 *   |-foreach
 *   |-includes
 *   |-strings
 *   |-[EXTRA]
 *     |-contractions
 * -desktop
 *   |-validate
 */
class CheckerModel: public QAbstractItemModel {
Q_OBJECT
public:

    /**
     * Creates a new CheckerModel.
     *
     *  @param parent The parent object.
     */
    explicit CheckerModel(QObject* parent = nullptr);

    /**
     * Destroys all the items (but not the checkers).
     */
    ~CheckerModel() override;

    //<QAbstractItemModel>

    /**
     * Returns the index of the item in the model specified by the given row,
     * column and parent index.
     *
     * @return The index of the item.
     */
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

    /**
     * Returns the parent of the model item with the given index.
     *
     * @return The parent of the model item with the given index.
     */
    QModelIndex parent(const QModelIndex& index) const override;

    /**
     * Returns the number of rows under the given parent.
     * Depending on the parent, it will be the number of file types, the number
     * of checkers for a file type (plus one) or the number of extra checkers
     * for a file type.
     *
     * @param parent The parent index.
     * @return The number of rows under the given parent.
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * Returns the number of columns, which is one.
     *
     * @param parent Unused.
     * @return One column.
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * Returns the data stored under the given role for the item referred to by
     * the index.
     * Display role shows the file type, the "extra" header or the name of the
     * checker, and ToolTip role shows the checker description.
     *
     * Invalid indexes or roles other than Display and ToolTip are ignored (an
     * empty variant is returned).
     *
     * @param index The index of the item.
     * @param role The role for the data.
     * @return The data for the given role and index.
     */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    //</QAbstractItemModel>

    /**
     * Returns the checker associated to the given index.
     * Indexes for file types or extra headers have no associated checker, so a
     * null pointer is returned in those cases.
     *
     * @param index The index to get its associated Checker.
     * @return The Checker associated to the given index.
     */
    const Checker* checkerForIndex(const QModelIndex& index) const;

    /**
     * Sets the checkers to use.
     *
     * @param checkers The checkers to use.
     */
    void setCheckers(const QList<const Checker*>& checkers);

private:

    /**
     * The internal items that represent the file type entries.
     */
    QList<Item*> m_fileTypeItems;

};

#endif
