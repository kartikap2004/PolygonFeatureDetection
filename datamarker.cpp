#include "datamarker.h"
#include <QDebug>
#include <QPainter>
#include <cmath>

#define _USE_MATH_DEFINES
#include "math.h"

DataMarker::DataMarker(QPointF aSymbolPosition, const SymbolType &aType, const QColor &aColor)
{
    m_symbolPos = aSymbolPosition;
    m_symbolType = aType;
    m_color = aColor;
    m_symbolSize = 0.5;
}

void DataMarker::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    const QPoint w(m_symbolSize, 0);
    const QPoint h(0, m_symbolSize);
    const QColor &backgroundColor = superlightgray;

    painter->setRenderHint(QPainter::Antialiasing, true);

    switch (m_symbolType) {
    case NoMarker: {
        break;
    }
    case Circle: {
        const double scale = 2.0 / sqrt(M_PI);
        painter->setBrush(m_color);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(m_symbolPos,
                             (int) round(m_symbolSize * scale),
                             (int) round(m_symbolSize * scale));
        break;
    }

    case Square: {
        // const double sqrt2 = sqrt(2.0);
        painter->setBrush(m_color);
        painter->setPen(Qt::NoPen);
        painter->drawRect(QRectF(m_symbolPos.x() - (m_symbolSize / 2),
                                 m_symbolPos.y() - (m_symbolSize / 2),
                                 m_symbolSize,
                                 m_symbolSize));
        break;
    }

    case Diamond: {
        const double sqrt2 = sqrt(2.0);
        const QPointF points[4] = {m_symbolPos + w * sqrt2,
                                   m_symbolPos - h * sqrt2,
                                   m_symbolPos - w * sqrt2,
                                   m_symbolPos + h * sqrt2};
        painter->setBrush(m_color);
        painter->setPen(Qt::NoPen);
        painter->drawConvexPolygon(points, 4);
        break;
    }

    case Plus: {
        // const QPointF pixel = (int(m_symbolSize) % 2 == 1) ? QPointF(0.5, 0.5) :
        // QPointF();
        const double sqrt2 = sqrt(2.0);
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(m_color, m_symbolSize));
        painter->drawLine(QPointF(m_symbolPos.x() - m_symbolSize / sqrt2, m_symbolPos.y()),
                          QPointF(m_symbolPos.x() + m_symbolSize / sqrt2, m_symbolPos.y()));
        painter->drawLine(QPointF(m_symbolPos.x(), m_symbolPos.y() - m_symbolSize / sqrt2),
                          QPointF(m_symbolPos.x(), m_symbolPos.y() + m_symbolSize / sqrt2));
        break;
    }

    case CircleOutline: {
        const double scale = 2.0 / sqrt(M_PI);
        painter->setBrush(m_color);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(m_symbolPos,
                             (int) round(m_symbolSize * scale),
                             (int) round(m_symbolSize * scale));
        painter->setBrush(backgroundColor);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(m_symbolPos,
                             (int) round(m_symbolSize * scale / 2),
                             (int) round(m_symbolSize * scale / 2));
        break;
    }

    case SquareOutline: {
        painter->setBrush(m_color);
        painter->setPen(Qt::NoPen);
        painter->drawRect(m_symbolPos.x() - m_symbolSize,
                          m_symbolPos.y() - m_symbolSize,
                          2 * m_symbolSize,
                          2 * m_symbolSize);
        painter->setBrush(backgroundColor);
        painter->setPen(Qt::NoPen);
        painter->drawRect(m_symbolPos.x() - m_symbolSize / 2,
                          m_symbolPos.y() - m_symbolSize / 2,
                          (int) (m_symbolSize / 2) * 2,
                          (int) (m_symbolSize / 2) * 2);
        break;
    }

    case DiamondOutline: {
        const double sqrt2 = sqrt(2.0);
        const QPointF points[4] = {m_symbolPos + w * sqrt2,
                                   m_symbolPos - h * sqrt2,
                                   m_symbolPos - w * sqrt2,
                                   m_symbolPos + h * sqrt2};
        const QPointF pointsInner[4] = {m_symbolPos + w * sqrt2 / 2,
                                        m_symbolPos - h * sqrt2 / 2,
                                        m_symbolPos - w * sqrt2 / 2,
                                        m_symbolPos + h * sqrt2 / 2};
        painter->setBrush(m_color);
        painter->setPen(Qt::NoPen);
        painter->drawConvexPolygon(points, 4);
        painter->setBrush(backgroundColor);
        painter->setPen(Qt::NoPen);
        painter->drawConvexPolygon(pointsInner, 4);
        break;
    }

    case Cross: {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(m_color, m_symbolSize));
        painter->drawLine(m_symbolPos - w / 2 - h / 2, m_symbolPos + w / 2 + h / 2);
        painter->drawLine(m_symbolPos + w / 2 - h / 2, m_symbolPos - w / 2 + h / 2);
        break;
    }
    }

    painter->setRenderHint(QPainter::Antialiasing, false);
}

QRectF DataMarker::boundingRect() const
{
    return QRectF(m_symbolPos.x() - m_symbolSize / 2,
                  m_symbolPos.y() - m_symbolSize / 2,
                  m_symbolSize,
                  m_symbolSize);
}

const QPointF &DataMarker::position() const
{
    return m_symbolPos;
}

void DataMarker::setPosition(const QPointF &aPosition)
{
    m_symbolPos = aPosition;
}

const SymbolType &DataMarker::symbolType() const
{
    return m_symbolType;
}

const QColor &DataMarker::color() const
{
    return m_color;
}
