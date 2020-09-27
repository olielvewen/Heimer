// This file is part of Heimer.
// Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
//
// Heimer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Heimer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Heimer. If not, see <http://www.gnu.org/licenses/>.

#ifndef EDITOR_SCENE_HPP
#define EDITOR_SCENE_HPP

#include <memory>

#include <QGraphicsScene>

class Node;

class EditorScene : public QGraphicsScene
{
public:
    EditorScene();

    QRectF zoomToFit(bool isForExport = false) const;

    //! Checks if the graphics scene already has the given edge item added
    bool hasEdge(Node & node0, Node & node1);

    QImage toImage(QSize size, QColor backgroundColor, bool transparentBackground);

    void toSvg(QString fileName, QString title);

    virtual ~EditorScene();

private:
    void removeItems();

    using ItemPtr = std::unique_ptr<QGraphicsItem>;
    std::vector<ItemPtr> m_ownItems;
};

#endif // EDITOR_SCENE_HPP
