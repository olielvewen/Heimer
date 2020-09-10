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

#include "node.hpp"

#include "constants.hpp"
#include "edge.hpp"
#include "graphics_factory.hpp"
#include "image.hpp"
#include "layers.hpp"
#include "node_handle.hpp"
#include "text_edit.hpp"

#include "simple_logger.hpp"

#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneHoverEvent>
#include <QImage>
#include <QPainter>
#include <QPen>
#include <QVector2D>

#include <algorithm>
#include <cmath>

Node::Node()
  : m_textEdit(new TextEdit(this))
{
    setAcceptHoverEvents(true);

    m_size = QSize(Constants::Node::MIN_WIDTH, Constants::Node::MIN_HEIGHT);

    setZValue(static_cast<int>(Layers::Node));

    createEdgePoints();

    createHandles();

    initTextField();

    setSelected(false);

    connect(m_textEdit, &TextEdit::textChanged, [=](const QString & text) {
        setText(text);
        adjustSize();
    });

    connect(m_textEdit, &TextEdit::undoPointRequested, this, &Node::undoPointRequested);

    m_handleVisibilityTimer.setSingleShot(true);
    m_handleVisibilityTimer.setInterval(2000);

    connect(&m_handleVisibilityTimer, &QTimer::timeout, [=]() {
        setHandlesVisible(false, false);
    });

    // Set the background transparent as the TextEdit background will be rendered in Node::paint().
    // The reason for this is that TextEdit's background affects only the area that includes letters
    // and we want to render a larger area.
    m_textEdit->setBackgroundColor({ 0, 0, 0, 0 });
}

Node::Node(const Node & other)
  : Node()
{
    setColor(other.m_color);

    setCornerRadius(other.m_cornerRadius);

    setImageRef(other.m_imageRef);

    setIndex(other.m_index);

    setLocation(other.m_location);

    m_size = other.m_size;

    setText(other.text());

    setTextColor(other.m_textColor);

    setTextSize(other.m_textSize);
}

void Node::addGraphicsEdge(Edge & edge)
{
#ifndef HEIMER_UNIT_TEST
    m_graphicsEdges.push_back(&edge);
#else
    Q_UNUSED(edge)
#endif
}

void Node::removeGraphicsEdge(Edge & edge)
{
#ifndef HEIMER_UNIT_TEST
    const auto iter = std::find(m_graphicsEdges.begin(), m_graphicsEdges.end(), &edge);
    if (iter != m_graphicsEdges.end()) {
        m_graphicsEdges.erase(iter);
    }
#else
    Q_UNUSED(edge)
#endif
}

void Node::adjustSize()
{
    prepareGeometryChange();

    const auto margin = Constants::Node::MARGIN * 2;
    const auto newSize = QSize {
        std::max(Constants::Node::MIN_WIDTH, static_cast<int>(m_textEdit->boundingRect().width() + margin)),
        std::max(Constants::Node::MIN_HEIGHT, static_cast<int>(m_textEdit->boundingRect().height() + margin))
    };

    m_size = newSize;

    createHandles();

    createEdgePoints();

    updateEdgeLines();

    initTextField();

    update();
}

QRectF Node::boundingRect() const
{
    // Take children into account
    QRectF nodeBox { -m_size.width() / 2, -m_size.height() / 2, m_size.width(), m_size.height() };
    for (auto && handle : m_handles) {
        auto handleBox = handle->boundingRect();
        handleBox.translate(handle->pos());
        nodeBox = nodeBox.united(handleBox);
    }
    return nodeBox;
}

EdgePtr Node::createAndAddGraphicsEdge(NodePtr targetNode)
{
    const auto edge = std::make_shared<Edge>(*this, *targetNode);
    edge->updateLine();
    m_graphicsEdges.push_back(edge.get());
    return edge;
}

void Node::createEdgePoints()
{
    const double w2 = m_size.width() * 0.5;
    const double h2 = m_size.height() * 0.5;
    const double bias = 0.1;

    m_edgePoints = {
        { { -w2, h2 }, true },
        { { 0, h2 + bias }, false },
        { { w2, h2 }, true },
        { { w2 + bias, 0 }, false },
        { { w2, -h2 }, true },
        { { 0, -h2 - bias }, false },
        { { -w2, -h2 }, true },
        { { -w2 - bias, 0 }, false }
    };
}

void Node::createHandles()
{
    // Delete old handles
    for (auto handle : m_handles) {
        handle->setParentItem(nullptr);
        delete handle;
    }
    m_handles.clear();

    const auto addHandle = new NodeHandle(*this, NodeHandle::Role::Add, Constants::Node::HANDLE_RADIUS);
    addHandle->setParentItem(this);
    addHandle->setPos({ 0, m_size.height() * 0.5 });
    m_handles.push_back(addHandle);

    const auto colorHandle = new NodeHandle(*this, NodeHandle::Role::Color, Constants::Node::HANDLE_RADIUS_SMALL);
    colorHandle->setParentItem(this);
    colorHandle->setPos({ m_size.width() * 0.5, m_size.height() * 0.5 - Constants::Node::HANDLE_RADIUS_SMALL * 0.5 });
    m_handles.push_back(colorHandle);

    const auto textColorHandle = new NodeHandle(*this, NodeHandle::Role::TextColor, Constants::Node::HANDLE_RADIUS_SMALL);
    textColorHandle->setParentItem(this);
    textColorHandle->setPos({ m_size.width() * 0.5, -m_size.height() * 0.5 + Constants::Node::HANDLE_RADIUS_SMALL * 0.5 });
    m_handles.push_back(textColorHandle);

    const auto dragHandle = new NodeHandle(*this, NodeHandle::Role::Drag, Constants::Node::HANDLE_RADIUS_MEDIUM);
    dragHandle->setParentItem(this);
    dragHandle->setPos({ -m_size.width() * 0.5 - Constants::Node::HANDLE_RADIUS_SMALL * 0.15, -m_size.height() * 0.5 - Constants::Node::HANDLE_RADIUS_SMALL * 0.15 });
    m_handles.push_back(dragHandle);
}

QRectF Node::expandedTextEditRect() const
{
    auto textEditRect = QRectF {};
    textEditRect.setX(m_textEdit->pos().x());
    textEditRect.setY(m_textEdit->pos().y());
    textEditRect.setWidth(m_size.width() - Constants::Node::MARGIN * 2);
    textEditRect.setHeight(m_textEdit->boundingRect().height());
    return textEditRect;
}

std::pair<EdgePoint, EdgePoint> Node::getNearestEdgePoints(const Node & node1, const Node & node2)
{
    double bestDistance = std::numeric_limits<double>::max();
    std::pair<EdgePoint, EdgePoint> bestPair = { EdgePoint(), EdgePoint() };

    // This is O(n^2) but fine as there are not many points
    for (auto && point1 : node1.m_edgePoints) {
        for (auto && point2 : node2.m_edgePoints) {
            const auto distance = std::pow(node1.pos().x() + point1.location.x() - node2.pos().x() - point2.location.x(), 2) + std::pow(node1.pos().y() + point1.location.y() - node2.pos().y() - point2.location.y(), 2);
            if (distance < bestDistance) {
                bestDistance = distance;
                bestPair = { point1, point2 };
            }
        }
    }

    return bestPair;
}

void Node::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    if (index() != -1) // Prevent right-click on the drag node
    {
        m_currentMousePos = event->pos();
        m_mouseIn = true;

        checkHandleVisibility(event->pos());

        QGraphicsItem::hoverEnterEvent(event);
    }
}

void Node::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    if (index() != -1) // Prevent right-click on the drag node
    {
        m_mouseIn = false;

        setHandlesVisible(false);

        if (event) // EditorView may call this with a null event
        {
            QGraphicsItem::hoverLeaveEvent(event);
        }
    }
}

void Node::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
    if (index() != -1) // Prevent right-click on the drag node
    {
        m_currentMousePos = event->pos();

        checkHandleVisibility(event->pos());

        QGraphicsItem::hoverMoveEvent(event);
    }
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    if (index() != -1) // Prevent left-click on the drag node
    {
        if (expandedTextEditRect().contains(event->pos())) {
            m_textEdit->setFocus();
        }

        QGraphicsItem::mousePressEvent(event);
    }
}

void Node::checkHandleVisibility(QPointF pos)
{
    // Bounding box without children
    const QRectF bbox { -m_size.width() / 2, -m_size.height() / 2, m_size.width(), m_size.height() };
    if (bbox.contains(pos)) {
        if (hitsHandle(pos)) {
            setHandlesVisible(true, false);
        } else {
            setHandlesVisible(true);
        }

        m_handleVisibilityTimer.start();
    }
}

NodeHandle * Node::hitsHandle(QPointF pos)
{
    for (auto && handle : m_handles) {
        if (handle->contains(pos)) {
            return handle;
        }
    }

    return nullptr;
}

void Node::initTextField()
{
#ifndef HEIMER_UNIT_TEST
    m_textEdit->setTextWidth(-1);
    m_textEdit->setPos(-m_size.width() * 0.5 + Constants::Node::MARGIN, -m_size.height() * 0.5 + Constants::Node::MARGIN);
#endif
}

void Node::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    painter->save();

    // Background

    QPainterPath path;
    const QRectF rect(-m_size.width() / 2, -m_size.height() / 2, m_size.width(), m_size.height());
    path.addRoundedRect(rect, m_cornerRadius, m_cornerRadius);
    painter->setRenderHint(QPainter::Antialiasing);

    if (!m_pixmap.isNull()) {
        QPixmap scaledPixmap(static_cast<int>(m_size.width()), static_cast<int>(m_size.height()));
        scaledPixmap.fill(Qt::transparent);
        QPainter pixmapPainter(&scaledPixmap);
        QPainterPath scaledPath;
        const QRectF scaledRect(0, 0, m_size.width(), m_size.height());
        scaledPath.addRoundedRect(scaledRect, m_cornerRadius, m_cornerRadius);

        const auto pixmapAspect = static_cast<double>(m_pixmap.width()) / m_pixmap.height();
        const auto nodeAspect = m_size.width() / m_size.height();
        if (nodeAspect > 1.0) {
            if (pixmapAspect > nodeAspect) {
                pixmapPainter.fillPath(scaledPath, QBrush(m_pixmap.scaledToHeight(static_cast<int>(m_size.height()))));
            } else {
                pixmapPainter.fillPath(scaledPath, QBrush(m_pixmap.scaledToWidth(static_cast<int>(m_size.width()))));
            }
        } else {
            if (pixmapAspect < nodeAspect) {
                pixmapPainter.fillPath(scaledPath, QBrush(m_pixmap.scaledToWidth(static_cast<int>(m_size.width()))));
            } else {
                pixmapPainter.fillPath(scaledPath, QBrush(m_pixmap.scaledToHeight(static_cast<int>(m_size.height()))));
            }
        }

        painter->drawPixmap(rect, scaledPixmap, scaledRect);
    } else {
        painter->fillPath(path, QBrush(m_color));
    }

    // Patch for TextEdit

    painter->fillRect(expandedTextEditRect(), Constants::Node::TEXT_EDIT_BACKGROUND_COLOR);

    painter->restore();
}

void Node::setColor(const QColor & color)
{
    m_color = color;
#ifndef HEIMER_UNIT_TEST
    update();
#endif
}

int Node::cornerRadius() const
{
    return m_cornerRadius;
}

void Node::setCornerRadius(int value)
{
    m_cornerRadius = value;

    updateEdgeLines();

    // Needed to redraw immediately on e.g. a new design. Otherwise updates start to work only after
    // the first mouse over, which is a bit weird.
    prepareGeometryChange();

    update();
}

void Node::setHandlesVisible(bool visible, bool all)
{
    if (all) {
        for (auto && handle : m_handles) {
            handle->setVisible(visible);
        }
    } else {
        for (auto && handle : m_handles) {
            if (!visible) {
                if (!handle->contains(m_currentMousePos)) {
                    handle->setVisible(visible);
                }
            } else {
                if (handle->contains(m_currentMousePos)) {
                    handle->setVisible(visible);
                }
            }
        }
    }
}

QPointF Node::location() const
{
    return m_location;
}

void Node::setLocation(QPointF newLocation)
{
    m_location = newLocation;
    setPos(newLocation);

    updateEdgeLines();

    setHandlesVisible(false);
}

QRectF Node::placementBoundingRect() const
{
    return { -m_size.width() / 2, -m_size.height() / 2, m_size.width(), m_size.height() };
}

bool Node::selected() const
{
    return m_selected;
}

void Node::setSelected(bool selected)
{
    m_selected = selected;
    setGraphicsEffect(GraphicsFactory::createDropShadowEffect(selected));
    update();
}

void Node::setTextInputActive()
{
    m_textEdit->setActive(true);
    m_textEdit->setFocus();
}

QString Node::text() const
{
#ifndef HEIMER_UNIT_TEST
    return m_textEdit->text();
#else
    return m_text;
#endif
}

void Node::setText(const QString & text)
{
    if (text != m_text) {
        m_text = text;
        m_textEdit->setText(text);
        adjustSize();
    }
}

QColor Node::textColor() const
{
    return m_textColor;
}

void Node::setTextColor(const QColor & color)
{
    m_textColor = color;
#ifndef HEIMER_UNIT_TEST
    m_textEdit->setDefaultTextColor(color);
    m_textEdit->update();
#endif
}

void Node::setTextSize(int textSize)
{
    m_textSize = textSize;
#ifndef HEIMER_UNIT_TEST
    m_textEdit->setTextSize(textSize);
    adjustSize();
#endif
}

void Node::setImageRef(size_t imageRef)
{
    if (imageRef) {
        m_imageRef = imageRef;
        emit imageRequested(imageRef, *this);
    } else if (m_imageRef) {
        m_imageRef = imageRef;
        applyImage({});
    }
}

void Node::applyImage(const Image & image)
{
    m_pixmap = QPixmap::fromImage(image.image());

    update();
}

void Node::updateEdgeLines()
{
    for (auto && edge : m_graphicsEdges) {
        edge->updateLine();
    }
}

QSizeF Node::size() const
{
    return m_size;
}

void Node::setSize(const QSizeF & size)
{
    m_size = size;
}

size_t Node::imageRef() const
{
    return m_imageRef;
}

QColor Node::color() const
{
    return m_color;
}

int Node::index() const
{
    return m_index;
}

void Node::setIndex(int index)
{
    m_index = index;
}

Node::~Node()
{
    juzzlin::L().debug() << "Deleting Node " << index();
}
