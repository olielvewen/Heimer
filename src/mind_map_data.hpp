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

#ifndef MIND_MAP_DATA_HPP
#define MIND_MAP_DATA_HPP

#include <QString>

#include "constants.hpp"
#include "graph.hpp"
#include "image_manager.hpp"
#include "mind_map_data_base.hpp"

class ObjectModelLoader;

class MindMapData : public MindMapDataBase
{
public:
    MindMapData(QString name = "");

    MindMapData(const MindMapData & other);

    virtual ~MindMapData();

    QColor backgroundColor() const;

    void setBackgroundColor(const QColor & backgroundColor);

    int cornerRadius() const;

    void setCornerRadius(int cornerRadius);

    QColor edgeColor() const;

    void setEdgeColor(const QColor & edgeColor);

<<<<<<< HEAD:src/mind_map_data.hpp
=======
    QColor gridColor() const;

    void setGridColor(const QColor & edgeColor);

>>>>>>> upstream/master:src/mindmapdata.hpp
    double edgeWidth() const;

    void setEdgeWidth(double width);

    QString fileName() const;

    void setFileName(QString fileName);

    Graph & graph();

    const Graph & graph() const;

    int textSize() const;

    void setTextSize(int textSize);

    QString version() const;

    void setVersion(const QString & version);

    ImageManager & imageManager();
<<<<<<< HEAD:src/mind_map_data.hpp

    const ImageManager & imageManager() const;

=======

    const ImageManager & imageManager() const;

>>>>>>> upstream/master:src/mindmapdata.hpp
private:
    void copyGraph(const MindMapData & other);

    QString m_fileName;

    QString m_version;

    QColor m_backgroundColor = Constants::MindMap::DEFAULT_BACKGROUND_COLOR;

    QColor m_edgeColor = Constants::MindMap::DEFAULT_EDGE_COLOR;

<<<<<<< HEAD:src/mind_map_data.hpp
=======
    QColor m_gridColor = Constants::MindMap::DEFAULT_GRID_COLOR;

>>>>>>> upstream/master:src/mindmapdata.hpp
    double m_edgeWidth = Constants::MindMap::DEFAULT_EDGE_WIDTH;

    int m_textSize = Constants::MindMap::DEFAULT_TEXT_SIZE;

    int m_cornerRadius = Constants::Node::DEFAULT_CORNER_RADIUS;

    Graph m_graph;

    static ImageManager m_imageManager;
};

typedef std::shared_ptr<MindMapData> MindMapDataPtr;

#endif // MIND_MAP_DATA_HPP
