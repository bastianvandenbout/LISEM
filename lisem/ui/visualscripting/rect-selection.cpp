#include <QPen>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "rect-selection.h"

namespace pal {

RectHandle::RectHandle(Qt::Orientation orientation, QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
    , m_orientation(orientation)
    , m_size(30.0)
    , m_pen(QPen(Qt::black, 2))
    , m_dragging(false)
    , m_min_pos(0)
    , m_max_pos(1)
{
    setAcceptHoverEvents(true);
    setPen(QPen(Qt::transparent));
    m_pen.setCosmetic(true);
    setRect(-m_size/2, -m_size/2, m_size, m_size);
}

double RectHandle::position() const {
    if (m_orientation == Qt::Horizontal)
        return this->pos().y();
    else
        return this->pos().x();
}

void RectHandle::setPosition(double p) {
    p = qBound(m_min_pos, p, m_max_pos);
    if (qFuzzyCompare(p, position()))
        return;

    if (m_orientation == Qt::Horizontal)
        moveBy(0, p - position());
    else
        moveBy(p - position(), 0);

    emit moved(p);
}

double RectHandle::minPosition() const {
    return m_min_pos;
}

void RectHandle::setMinPosition(double m) {
    m_min_pos = m;
    setPosition(position());
}

double RectHandle::maxPosition() const {
    return m_max_pos;
}

void RectHandle::setMaxPosition(double m) {
    m_max_pos = m;
    setPosition(position());
}

void RectHandle::setExtents(double min, double max) {
    if (m_orientation == Qt::Horizontal)
        setRect(min, rect().top(), max - min, rect().height());
    else
        setRect(rect().left(), min, rect().width(), max - min);
}

double RectHandle::size() const {
    return m_size;
}

void RectHandle::setSize(double w) {
    m_size = w;
    if (m_orientation == Qt::Horizontal)
        setRect(rect().left(), position()-w/2, rect().width(), w);
    else
        setRect(position()-w/2, rect().top(), w, rect().height());
}

void RectHandle::hoverEnterEvent(QGraphicsSceneHoverEvent *) {
    setPen(m_pen);
    update();
}

void RectHandle::hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
    setPen(QPen(Qt::transparent));
    update();
}

void RectHandle::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() & Qt::LeftButton)
        m_dragging = true;
}

void RectHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (! m_dragging)
        return;

    if (m_orientation == Qt::Horizontal)
        setPosition(position() + event->pos().y() - event->lastPos().y());
    else
        setPosition(position() + event->pos().x() - event->lastPos().x());
}

void RectHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() & Qt::LeftButton) {
        m_dragging = false;
    }
}

enum {
    NONE   = 0,
    LEFT   = 1,
    RIGHT  = 2,
    TOP    = 4,
    BOTTOM = 8,
    ALL    = 15
};

SelectionItem::SelectionItem(QGraphicsItem *parent) :
    QGraphicsObject(parent)
{
    setFlag(ItemHasNoContents);

    m_pen = QPen(QColor(15, 105, 200), 5);
    m_pen.setJoinStyle(Qt::RoundJoin);
    m_pen.setStyle(Qt::DashLine);
    m_pen.setCapStyle(Qt::RoundCap);
    m_pen.setCosmetic(true);

    m_rect = new QGraphicsRectItem(parent);
    m_rect->setPen(m_pen);

    m_hl = new RectHandle(Qt::Vertical, parent);
    m_hr = new RectHandle(Qt::Vertical, parent);
    m_ht = new RectHandle(Qt::Horizontal, parent);
    m_hb = new RectHandle(Qt::Horizontal, parent);

    connect(m_hl, &RectHandle::moved, this, &SelectionItem::leftMoved);
    connect(m_hr, &RectHandle::moved, this, &SelectionItem::rightMoved);
    connect(m_ht, &RectHandle::moved, this, &SelectionItem::topMoved);
    connect(m_hb, &RectHandle::moved, this, &SelectionItem::bottomMoved);
}

double SelectionItem::left() const {
    return m_l;
}

void SelectionItem::setLeft(double p) {
    m_l = p;
    updateSelection(ALL);
}

double SelectionItem::right() const {
    return m_r;
}

void SelectionItem::setRight(double p) {
    m_r = p;
    updateSelection(ALL);
}

double SelectionItem::top() const {
    return m_t;
}

void SelectionItem::setTop(double p) {
    m_t = p;
    updateSelection(ALL);
}

double SelectionItem::bottom() const {
    return m_b;
}

void SelectionItem::setBottom(double p) {
    m_b = p;
    updateSelection(ALL);
}

QRectF SelectionItem::selection() const {
    return m_rect->rect();
}

void SelectionItem::setSelection(const QRectF &sel) {
   m_l = sel.left();
   m_r = sel.right();
   m_t = sel.top();
   m_b = sel.bottom();
   updateSelection(ALL);
}

void SelectionItem::resetSelection() {
    setSelection(m_rect->parentItem()->boundingRect());
}

QPen SelectionItem::pen() const {
    return m_rect->pen();
}

void SelectionItem::setPen(const QPen &p) {
    m_pen = p;
    m_rect->setPen(p);
}

double SelectionItem::handleSize() const {
    return m_hl->size();
}

void SelectionItem::setHandleSize(double w) {
    m_hl->setSize(w);
    m_hr->setSize(w);
    m_ht->setSize(w);
    m_hb->setSize(w);
}

QPen SelectionItem::handlePen() const {
    return m_hl->pen();
}

void SelectionItem::setHandlePen(const QPen &p) {
    m_hl->setPen(p);
    m_hr->setPen(p);
    m_ht->setPen(p);
    m_hb->setPen(p);
}

bool SelectionItem::isVisible() const {
    return m_rect->isVisible();
}

void SelectionItem::setVisible(bool on) {
    m_rect->setVisible(on);
    m_hb->setVisible(on);
    m_hl->setVisible(on);
    m_ht->setVisible(on);
    m_hr->setVisible(on);
}

void SelectionItem::updateSelectionItems(int sides) {
    const QRectF sel_rect = m_rect->rect();
    const QRectF rect = m_rect->parentItem()->boundingRect();
    if (!sel_rect.isValid() || !rect.isValid())
        return;

    // enforce limits
    m_hl->setMinPosition(rect.left());
    m_hr->setMaxPosition(rect.right() - handleSize());
    m_ht->setMinPosition(rect.top());
    m_hb->setMaxPosition(rect.bottom() - handleSize());

    // new positions
    if (sides & LEFT) {
        m_hl->setRect(QRectF(0, 0, handleSize(), sel_rect.height()));
        m_hl->setPos(sel_rect.topLeft());
        emit leftChanged(left());
    }

    if (sides & RIGHT) {
        m_hr->setRect(QRectF(0, 0, handleSize(), sel_rect.height()));
        m_hr->setPos(sel_rect.topRight() - QPointF(handleSize(), 0));
        emit rightChanged(right());
    }

    if (sides & TOP) {
        m_ht->setRect(QRectF(0, 0, sel_rect.width(), handleSize()));
        m_ht->setPos(sel_rect.topLeft());
        emit topChanged(top());
    }

    if (sides & BOTTOM) {
        m_hb->setRect(QRectF(0, 0, sel_rect.width(), handleSize()));
        m_hb->setPos(sel_rect.bottomLeft() + QPointF(0, -handleSize()));
        emit bottomChanged(bottom());
    }

    if (sides != NONE)
        emit selectionChanged(selection());

    // new limits
    if (sides & LEFT)
        m_hr->setMinPosition(m_hl->pos().x() + handleSize());

    if (sides & RIGHT)
        m_hl->setMaxPosition(m_hr->pos().x() - handleSize());

    if (sides & TOP)
        m_hb->setMinPosition(m_ht->pos().y() + handleSize());

    if (sides & BOTTOM)
        m_ht->setMaxPosition(m_hb->pos().y() - handleSize());
}

void SelectionItem::updateSelectionRect() {
    // new selection
    QRectF crop_rect = QRectF(m_l, m_t, m_r - m_l, m_b - m_t);
    m_rect->setRect(crop_rect);
}

void SelectionItem::updateSelection(int sides) {
    updateSelectionRect();
    updateSelectionItems(sides);
}

void SelectionItem::leftMoved(double p) {
    m_l = p;
    updateSelection(ALL &~ LEFT);
}

void SelectionItem::rightMoved(double p) {
    m_r = p + handleSize();
    updateSelection(ALL &~ RIGHT);
}

void SelectionItem::topMoved(double p) {
    m_t = p;
    updateSelection(ALL &~ TOP);
}

void SelectionItem::bottomMoved(double p) {
    m_b = p + handleSize();
    updateSelection(ALL &~ BOTTOM);
}

QRectF SelectionItem::boundingRect() const {
    return QRect();
}

void SelectionItem::paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) {
}

} // namespace cmc
