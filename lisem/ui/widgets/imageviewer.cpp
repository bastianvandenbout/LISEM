#include "imageviewer.h"
#include <cmath>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSceneHoverEvent>
#include <QWheelEvent>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>
#include "graphicsview.h"
#include "lsmio.h"
#include "site.h"

namespace pal {


ImageViewer::ImageViewer(QWidget *parent)
    : QFrame(parent)
    , m_zoom_level(0)
    , m_fit(true)
    , m_bar_mode(ToolBarMode::Visible)
{
    auto scene = new QGraphicsScene(this);
    m_view = new GraphicsView(this);
    m_view->setScene(scene);

    // graphic object holding the image buffer
    m_pixmap = new PixmapItem;
    scene->addItem(m_pixmap);
    connect(m_pixmap, SIGNAL(mouseMoved(int,int)), SLOT(mouseAt(int,int)));

    makeToolbar();

    auto box = new QVBoxLayout;
    box->setContentsMargins(5,0,5,0);
    box->addWidget(m_toolbar);
    box->addWidget(m_view, 1);
    setLayout(box);
}

// toolbar with a few quick actions and display information
void ImageViewer::makeToolbar() {
    // text and value at pixel
    m_text_label = new QLabel(this);
    m_text_label->setStyleSheet(QString("QLabel { font-weight: bold; }"));
    m_pixel_value = new QLabel(this);
    m_pixel_value->setStyleSheet("QLabel {  color : black; }");
    QString Dir = GetSite();

    auto fit = new QToolButton(this);
    fit->setToolTip(tr("Fit image to window"));
    fit->setIcon(QIcon(Dir + LISEM_FOLDER_ASSETS +"zoom-fit-best.png"));
    connect(fit, SIGNAL(clicked()), SLOT(zoomFit()));

    auto orig = new QToolButton(this);
    orig->setToolTip(tr("Resize image to its original size"));
    orig->setIcon(QIcon(Dir + LISEM_FOLDER_ASSETS +"zoom-original.png"));
    connect(orig, SIGNAL(clicked()), SLOT(zoomOriginal()));

    m_toolbar = new QWidget;
    auto box = new QHBoxLayout(m_toolbar);
    m_toolbar->setContentsMargins(0,0,0,0);
    box->setContentsMargins(0,0,0,0);
    box->addWidget(m_text_label);
    box->addStretch(1);
    box->addWidget(m_pixel_value);
    box->addWidget(fit);
    box->addWidget(orig);
}

QString ImageViewer::text() const {
    return m_text_label->text();
}

void ImageViewer::setText(const QString &txt) {
    m_text_label->setText(txt);
}

const QImage &ImageViewer::image() const {
    return m_pixmap->image();
}

void ImageViewer::setImage(const QImage &im) {
    m_pixmap->setImage(im);

    if (m_fit)
        zoomFit();

    emit imageChanged();
}

const PixmapItem *ImageViewer::pixmapItem() const {
    return m_pixmap;
}

PixmapItem *ImageViewer::pixmapItem() {
    return m_pixmap;
}

ImageViewer::ToolBarMode ImageViewer::toolBarMode() const {
    return m_bar_mode;
}

void ImageViewer::setToolBarMode(ToolBarMode mode) {
    m_bar_mode = mode;
    if (mode == ToolBarMode::Hidden)
        m_toolbar->hide();
    else if (mode == ToolBarMode::Visible)
        m_toolbar->show();
    else
        m_toolbar->setVisible(underMouse());
}

bool ImageViewer::isAntialiasingEnabled() const {
    return m_view->renderHints() & QPainter::Antialiasing;
}

void ImageViewer::enableAntialiasing(bool on) {
    m_view->setRenderHint(QPainter::Antialiasing, on);
}

void ImageViewer::addTool(QWidget *tool) {
    m_toolbar->layout()->addWidget(tool);
}

void ImageViewer::setMatrix() {
    qreal scale = std::pow(2.0, m_zoom_level / 10.0);

    QMatrix matrix;
    matrix.scale(scale, scale);

    m_view->setMatrix(matrix);
    emit zoomChanged(m_view->matrix().m11());
}

void ImageViewer::zoomFit() {
    m_view->fitInView(m_pixmap, Qt::KeepAspectRatio);
    m_zoom_level = int(10.0 * std::log2(m_view->matrix().m11()));
    m_fit = true;
    emit zoomChanged(m_view->matrix().m11());
}

void ImageViewer::zoomOriginal() {
    m_zoom_level = 0;
    m_fit = false;
    setMatrix();
}

void ImageViewer::zoomIn(int level) {
    m_zoom_level += level;
    m_fit = false;
    setMatrix();
}

void ImageViewer::zoomOut(int level) {
    m_zoom_level -= level;
    m_fit = false;
    setMatrix();
}

void ImageViewer::mouseAt(int x, int y) {
    if (m_pixmap->image().valid(x,y)) {


        QRgb rgb = m_pixmap->image().pixel(x, y);
        QString s;
        if(m_IsSingleBand)
        {
            s = QString("[%1, %2] (%3)").arg(x).arg(y)
                               .arg(m_MinB1 + (m_MaxB1-m_MinB1) * ((float)(qRed(rgb))/(float(255))));
        }else if(m_IsTripleBand)
        {
            s =  QString("[%1, %2] (%3, %4, %5)").arg(x).arg(y)
                    .arg(m_MinB1 + (m_MaxB1-m_MinB1) * ((float)(qRed(rgb))/(float(255)))).arg(m_MinB2 + (m_MaxB2-m_MinB2) * ((float)(qGreen(rgb))/(float(255)))).arg(m_MinB3 + (m_MaxB3-m_MinB3) * ((float)(qBlue(rgb))/(float(255))));
        }else
        {
            s = QString("[%1, %2] (%3, %4, %5)").arg(x).arg(y)
                               .arg(qRed(rgb)).arg(qGreen(rgb)).arg(qBlue(rgb));
        }
        m_pixel_value->setText(s);


    }
    else
        m_pixel_value->setText(QString());
}

void ImageViewer::enterEvent(QEvent *event) {
    QFrame::enterEvent(event);
    if (m_bar_mode == ToolBarMode::AutoHidden) {
        m_toolbar->show();
        if (m_fit)
            zoomFit();
    }
}

void ImageViewer::leaveEvent(QEvent *event) {
    QFrame::leaveEvent(event);
    if (m_bar_mode == ToolBarMode::AutoHidden) {
        m_toolbar->hide();
        if (m_fit)
            zoomFit();
    }
}

void ImageViewer::resizeEvent(QResizeEvent *event) {
    QFrame::resizeEvent(event);
    if (m_fit)
        zoomFit();
}

void ImageViewer::showEvent(QShowEvent *event) {
    QFrame::showEvent(event);
    if (m_fit)
        zoomFit();
}


PixmapItem::PixmapItem(QGraphicsItem *parent) :
    QObject(), QGraphicsPixmapItem(parent)
{
    setAcceptHoverEvents(true);
}

void PixmapItem::setImage(QImage im) {
    if (im.isNull()) {
        m_image.fill(Qt::white);
        im=m_image.copy();
    }
    std::swap(m_image, im);

    setPixmap(QPixmap::fromImage(m_image));

    if (m_image.size() != im.size())
        emit sizeChanged(m_image.width(), m_image.height());

    emit imageChanged(m_image);
}

void PixmapItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mousePressEvent(event);
}

void PixmapItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent(event);
}

void PixmapItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
    auto pos = event->pos();
    emit mouseMoved(int(pos.x()), int(pos.y()));
    QGraphicsItem::hoverMoveEvent(event);
}

} // namespace pal
