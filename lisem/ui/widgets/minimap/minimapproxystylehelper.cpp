#include "minimapproxystylehelper.h"


#include "uihelper.h"

#include "scripting/codeeditor.h"


#include <QScrollBar>
#include <QTimer>

namespace Minimap::Internal {

MinimapProxyStyleHelper::MinimapProxyStyleHelper(
    CodeEditor *editor) noexcept
    : QObject(editor),
      m_editor(editor) {
    this->m_editor->installEventFilter(this);
    if (!editor->document()->isEmpty()) {
        this->init();
    } else {
        QObject::connect(editor->document(),
                         &QTextDocument::contentsChanged,
                         this,
                         &MinimapProxyStyleHelper::contentsChanged);
    }
}

MinimapProxyStyleHelper::~MinimapProxyStyleHelper() noexcept {
    this->m_editor->removeEventFilter(this);
}

bool MinimapProxyStyleHelper::eventFilter(QObject *watched, QEvent *event) {
    if (watched == this->m_editor && event->type() == QEvent::Resize) {
        this->deferedUpdate();
    }
    return false;
}

int MinimapProxyStyleHelper::width() const noexcept {
    int w =
        0;//this->m_editor->extraArea() ? this->m_editor->extraArea()->width() : 0;
    return qMin(this->m_editor->width() - w,
                /*Settings::width()*/80 + Constants::MINIMAP_EXTRA_AREA_WIDTH);
}

const QRect &MinimapProxyStyleHelper::groove() const noexcept {
    return this->m_groove;
}

const QRect &MinimapProxyStyleHelper::addPage() const noexcept {
    return this->m_addPage;
}

const QRect &MinimapProxyStyleHelper::subPage() const noexcept {
    return this->m_subPage;
}

const QRect &MinimapProxyStyleHelper::slider() const noexcept {
    return this->m_slider;
}

int MinimapProxyStyleHelper::lineCount() const noexcept {
    return this->m_lineCount;
}

qreal MinimapProxyStyleHelper::factor() const noexcept {
    return this->m_factor;
}

const QColor &MinimapProxyStyleHelper::background() const noexcept {
    return this->m_backgroundColor;
}

const QColor &MinimapProxyStyleHelper::foreground() const noexcept {
    return this->m_foregroundColor;
}

const QColor &MinimapProxyStyleHelper::overlay() const noexcept {
    return this->m_overlayColor;
}

CodeEditor *MinimapProxyStyleHelper::editor() const
    noexcept {
    return this->m_editor;
}

void MinimapProxyStyleHelper::init() noexcept {
    QScrollBar *scrollbar = this->m_editor->verticalScrollBar();
    scrollbar->setProperty(Constants::MINIMAP_STYLE_OBJECT_PROPERTY,
                           QVariant::fromValue<QObject *>(this));
    /*QObject::connect(this->m_editor->textDocument(),
                     &TextEditor::TextDocument::fontSettingsChanged,
                     this,
                     &MinimapProxyStyleHelper::fontSettingsChanged);*/
    QObject::connect(this->m_editor->document()->documentLayout(),
                     &QAbstractTextDocumentLayout::documentSizeChanged,
                     this,
                     &MinimapProxyStyleHelper::deferedUpdate);
    QObject::connect(this->m_editor->document()->documentLayout(),
                     &QAbstractTextDocumentLayout::update,
                     this,
                     &MinimapProxyStyleHelper::deferedUpdate);
    /*QObject::connect(Settings::instance(),
                     &Settings::enabledChanged,
                     this,
                     &MinimapProxyStyleHelper::deferedUpdate);
    QObject::connect(Settings::instance(),
                     &Settings::widthChanged,
                     this,
                     &MinimapProxyStyleHelper::deferedUpdate);
    QObject::connect(Settings::instance(),
                     &Settings::lineCountThresholdChanged,
                     this,
                     &MinimapProxyStyleHelper::deferedUpdate);
    QObject::connect(Settings::instance(),
                     &Settings::alphaChanged,
                     this,
                     &MinimapProxyStyleHelper::fontSettingsChanged);*/
    QObject::connect(scrollbar,
                     &QAbstractSlider::valueChanged,
                     this,
                     &MinimapProxyStyleHelper::updateSubControlRects);
    this->fontSettingsChanged();
}

void MinimapProxyStyleHelper::contentsChanged() noexcept {
    QObject::disconnect(this->m_editor->document(),
                        &QTextDocument::contentsChanged,
                        this,
                        &MinimapProxyStyleHelper::contentsChanged);
    this->init();
}

void MinimapProxyStyleHelper::fontSettingsChanged() noexcept {
    QPalette palette = m_editor->palette();
    QColor tcolor = palette.color( QPalette::WindowText );;
    QColor bcolor = palette.color(QPalette::Window);
    this->m_backgroundColor = bcolor;
    this->m_foregroundColor = tcolor;
    this->m_overlayColor = QColor(Qt::black);
    this->m_overlayColor.setAlpha(32);
    /*const TextEditor::FontSettings &settings =
        this->m_editor->textDocument()->fontSettings();
    this->m_backgroundColor =
        settings.formatFor(TextEditor::C_TEXT).background();
    if (!this->m_backgroundColor.isValid()) {
        this->m_backgroundColor =
            this->m_theme->color(Utils::Theme::BackgroundColorNormal);
    }
    this->m_foregroundColor =
        settings.formatFor(TextEditor::C_TEXT).foreground();
    if (!this->m_foregroundColor.isValid()) {
        this->m_foregroundColor =
            this->m_theme->color(Utils::Theme::TextColorNormal);
    }
    if (this->m_backgroundColor.value() < 128) {
        this->m_overlayColor = QColor(Qt::white);
    } else {
        this->m_overlayColor = QColor(Qt::black);
    }
    this->m_overlayColor.setAlpha(Settings::alpha());*/
    this->deferedUpdate();
}

void MinimapProxyStyleHelper::deferedUpdate() noexcept {
    if (this->m_update) {
        return;
    }
    this->m_update = true;
    QTimer::singleShot(0, this, &MinimapProxyStyleHelper::update);
}

void MinimapProxyStyleHelper::update() noexcept {
    QScrollBar *scrollbar = this->m_editor->verticalScrollBar();
    QSizeF sz = this->m_editor->document()->size();
    //Use non-wrapped line count?
    this->m_lineCount = this->m_editor->document()->blockCount() + 1;//sz.toSize().height() + 1;
    int w = scrollbar->width();
    int h = scrollbar->height();
    this->m_factor = this->m_lineCount <= h
                         ? 1.0
                         : h / static_cast<qreal>(this->m_lineCount);
    int width = this->width();
    this->m_groove = QRect(width, 0, w - width, qMin(this->m_lineCount, h));
    this->updateSubControlRects();
    scrollbar->updateGeometry();
    this->m_update = false;
}

void MinimapProxyStyleHelper::updateSubControlRects() noexcept {
    QScrollBar *scrollbar = this->m_editor->verticalScrollBar();

    QSizeF sz = this->m_editor->document()->size();

    int viewPortLineCount =
        qRound(this->m_factor * (sz.toSize().height() + 1/*this->m_lineCount*/ - scrollbar->maximum()));
    std::cout << "viewport linecount " << viewPortLineCount << std::endl;
    int w = scrollbar->width();
    int h = scrollbar->height();
    int value = scrollbar->value();

    //linecount divided by blockcount
    float ratio_wrapping = ((float)(std::max(1,sz.toSize().height() + 1)))/((float)(std::max(1,this->m_lineCount)));

    int realValue = qRound( this->m_factor * value/ratio_wrapping);
    int min = scrollbar->minimum();
    int max = scrollbar->maximum();
    this->m_addPage = value < max ? QRect(0,
                                          realValue + viewPortLineCount,
                                          w,
                                          h - viewPortLineCount - realValue)
                                  : QRect();
    this->m_subPage = value > min ? QRect(0, 0, w, realValue) : QRect();
    this->m_slider = QRect(0, realValue, w, viewPortLineCount);
    scrollbar->update();
}

} // namespace Minimap::Internal
