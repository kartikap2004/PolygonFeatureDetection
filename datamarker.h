#ifndef DATAMARKER_H
#define DATAMARKER_H

#include <QColor>
#include <QFont>
#include <QFontMetrics>
#include <QGraphicsItem>
#include <QRect>

// Pre-defined Colors
const QColor darkergrey, darkergray = QColor(46, 52, 54);
const QColor darkgrey, darkgray = QColor(85, 87, 83);
const QColor grey, gray = QColor(136, 138, 133);
const QColor lightgrey, lightgray = QColor(186, 189, 182);
const QColor lightergrey, lightergray = QColor(211, 215, 207);
const QColor superlightgrey, superlightgray = QColor(238, 238, 236);

const QColor darkred = QColor(146, 0, 0);
const QColor red = QColor(204, 0, 0);
const QColor lightred = QColor(239, 41, 41);

const QColor darkbrown = QColor(143, 89, 2);
const QColor brown = QColor(193, 125, 17);
const QColor lightbrown = QColor(233, 185, 110);

const QColor darkpurple = QColor(92, 53, 102);
const QColor purple = QColor(117, 80, 123);
const QColor lightpurple = QColor(173, 127, 168);

const QColor darkblue = QColor(32, 74, 135);
const QColor blue = QColor(52, 101, 164);
const QColor lightblue = QColor(114, 159, 207);

const QColor darkorange = QColor(206, 92, 0);
const QColor orange = QColor(245, 121, 0);
const QColor lightorange = QColor(252, 175, 62);

const QColor darkgreen = QColor(78, 154, 6);
const QColor green = QColor(115, 210, 22);
const QColor lightgreen = QColor(138, 226, 52);

const QColor darkyellow = QColor(169, 160, 0);
const QColor yellow = QColor(237, 212, 0);
const QColor lightyellow = QColor(252, 233, 79);

enum SymbolType {
    NoMarker,
    Circle,
    Square,
    Diamond,
    Plus,
    CircleOutline,
    SquareOutline,
    DiamondOutline,
    Cross
};

class QPainter;

class DataMarker : public QGraphicsItem
{
public:
    DataMarker(QPointF aCenter, const SymbolType &aType, const QColor &aColor);

    const QPointF &position() const;
    void setPosition(const QPointF &aPosition);

    const SymbolType &symbolType() const;
    const QColor &color() const;

    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget = Q_NULLPTR) override;
    virtual QRectF boundingRect() const override;

private:
    QPointF m_symbolPos;
    SymbolType m_symbolType;
    QColor m_color;
    double m_symbolSize;
};

#endif // DATAMARKER_H
