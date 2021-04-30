#pragma once
#include <QGraphicsRectItem>
#include <QGraphicsItem>
#include <QPen>

namespace pal {

class RectHandle;

/**
 * An item used to interactively define a rectangle selection over the parent item
 */
class SelectionItem : public QGraphicsObject {
    Q_OBJECT

    public:
        explicit SelectionItem(QGraphicsItem *parent);

        /// current selection
        double left() const;
        double right() const;
        double top() const;
        double bottom() const;
        QRectF selection() const;

        /// Pen used to draw the selection rectangle
        QPen pen() const;
        void setPen(const QPen &p);

        /// size of the handle
        double handleSize() const;
        void setHandleSize(double w);

        /// Pen used to draw the handle rectangles
        QPen handlePen() const;
        void setHandlePen(const QPen &p);

        bool isVisible() const;

    signals:
        void leftChanged(double);
        void rightChanged(double);
        void topChanged(double);
        void bottomChanged(double);
        void selectionChanged(const QRectF &);

    public slots:
        void setLeft(double p);
        void setRight(double p);
        void setTop(double p);
        void setBottom(double p);
        void setSelection(const QRectF &sel);

        /// select the whole parent
        void resetSelection();

        void setVisible(bool);

    public:
        virtual QRectF boundingRect() const;
        virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

    private slots:
        void updateSelection(int);
        void updateSelectionRect();
        void updateSelectionItems(int);

        void leftMoved(double);
        void rightMoved(double);
        void topMoved(double);
        void bottomMoved(double);

    private:
        double m_l = 0, m_r = 0, m_t = 0, m_b = 0;
        QPen m_pen;
        QGraphicsRectItem *m_rect;
        RectHandle *m_hl, *m_hr, *m_ht, *m_hb;
};


/**
 * @brief RectHandle is a rectangular gnadle delimiting a region sensible to
 * mouse drag and drop, making it easy for the user to interact with objets.
 */
class RectHandle : public QObject, public QGraphicsRectItem {
    Q_OBJECT

public:
    RectHandle(Qt::Orientation orientation, QGraphicsItem *parent);

    /// position and position limits of the handle
    double position() const;
    double minPosition() const;
    double maxPosition() const;

    /// size of the handle
    double size() const;
    void setSize(double w);

public slots:
    void setPosition(double);
    void setMinPosition(double);
    void setMaxPosition(double);

    // taille du rectangle
    void setExtents(double min, double max);

signals:
    void moved(double pos);

protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    Qt::Orientation m_orientation;
    double m_size;
    QPen m_pen;
    bool m_dragging;
    double m_min_pos, m_max_pos;
};

} // namespace pal
