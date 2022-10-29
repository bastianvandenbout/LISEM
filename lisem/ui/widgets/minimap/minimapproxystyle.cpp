#include "minimapproxystyle.h"

#include "uihelper.h"
#include "minimapproxystylehelper.h"
#include "QTextBlock"
#include "QTextFragment"
#include "settings/generalsettingsmanager.h"

#include <QPainter>
#include <QScrollBar>
#include <QStyleOption>

#include <algorithm>

namespace Minimap::Internal {

static QColor blendColors(const QColor &a, const QColor &b) noexcept {
    int c = qMin(255, a.cyan() + b.cyan());
    int m = qMin(255, a.magenta() + b.magenta());
    int y = qMin(255, a.yellow() + b.yellow());
    int k = qMin(255, a.black() + b.black());
    return QColor::fromCmyk(c, m, y, k);
}

static bool updatePixel(QRgb *scanLine,
                               bool blend,
                               const QChar &c,
                               int &x,
                               int w,
                               int tab,
                               const QColor &bg,
                               const QColor &fg) noexcept {
    if (c == QChar::Tabulation) {
        for (int i = 0; i < tab; ++i) {
            if (!blend) {
                scanLine[x++] = bg.rgb();
            }
            if (x >= w) {
                return false;
            }
        }
    } else {
        bool isSpace = c.isSpace();
        if (blend && !isSpace) {
            QColor result =
                blendColors(fg.toCmyk(), QColor(scanLine[x]).toCmyk()).toRgb();
            scanLine[x++] = result.rgb();
        } else {
            scanLine[x++] = isSpace ? bg.rgb() : fg.rgb();
        }
        if (x >= w) {
            return false;
        }
    }
    return true;
}

static void merge(QColor &bg, QColor &fg, const QTextCharFormat &f) noexcept {
    if (f.background().style() != Qt::NoBrush) {
        bg = f.background().color();
    }
    if (f.foreground().style() != Qt::NoBrush) {
        fg = f.foreground().color();
    }
}

MinimapProxyStyle::MinimapProxyStyle(QStyle *style) noexcept
    : QProxyStyle(style) {}

void MinimapProxyStyle::drawComplexControl(ComplexControl control,
                                           const QStyleOptionComplex *option,
                                           QPainter *painter,
                                           const QWidget *widget) const {

    int valmini = GetSettingsManager()->GetSettingInt("UseMinimap",0);

    if (widget && control == QStyle::CC_ScrollBar  && valmini == 1) {

        QVariant v =
            widget->property(Constants::MINIMAP_STYLE_OBJECT_PROPERTY);
        if (v.isValid()) {
            auto *helper =
                static_cast<MinimapProxyStyleHelper *>(v.value<QObject *>());


            int lineCount = helper->lineCount();
            if (lineCount > 0 && lineCount <= 8000/*Settings::lineCountThreshold()*/) {
                if (this->drawMinimap(option, painter, widget, helper)) {
                    return;
                }
            }
        }
    }
    QProxyStyle::drawComplexControl(control, option, painter, widget);


    //get highlight locations for scroll bar

    //error
    //search
    //warning


    //draw these on top
}

QStyle::SubControl
MinimapProxyStyle::hitTestComplexControl(ComplexControl control,
                                         const QStyleOptionComplex *option,
                                         const QPoint &pos,
                                         const QWidget *widget) const {
    int valmini = GetSettingsManager()->GetSettingInt("UseMinimap",0);

    if (widget && control == QStyle::CC_ScrollBar && valmini == 1) {
        QVariant v =
            widget->property(Constants::MINIMAP_STYLE_OBJECT_PROPERTY);
        if (v.isValid()) {
            auto *helper =
                static_cast<MinimapProxyStyleHelper *>(v.value<QObject *>());
            int lineCount = helper->lineCount();
            if (lineCount > 0 && lineCount <= 8000/*Settings::lineCountThreshold()*/) {
                SubControl sc = SC_None;
                if (const QStyleOptionSlider *scrollbar =
                        qstyleoption_cast<const QStyleOptionSlider *>(
                            option)) {
                    QRect r;
                    uint ctrl = SC_ScrollBarAddLine;
                    while (ctrl <= SC_ScrollBarGroove) {
                        r = this->subControlRect(control,
                                                 scrollbar,
                                                 QStyle::SubControl(ctrl),
                                                 widget);
                        if (r.isValid() && r.contains(pos)) {
                            sc = QStyle::SubControl(ctrl);
                            break;
                        }
                        ctrl <<= 1;
                    }
                }
                return sc;
            }
        }
    }
    return QProxyStyle::hitTestComplexControl(control, option, pos, widget);
}

int MinimapProxyStyle::pixelMetric(PixelMetric metric,
                                   const QStyleOption *option,
                                   const QWidget *widget) const {
    int valmini = GetSettingsManager()->GetSettingInt("UseMinimap",0);

        if (widget && metric == QStyle::PM_ScrollBarExtent &&
        valmini == 1) {
        int w = QProxyStyle::pixelMetric(metric, option, widget);
        QVariant v =
            widget->property(Constants::MINIMAP_STYLE_OBJECT_PROPERTY);
        if (v.isValid()) {
            auto *helper =
                static_cast<MinimapProxyStyleHelper *>(v.value<QObject *>());
            int lineCount = helper->lineCount();
            if (lineCount > 0 && lineCount <= 8000/*Settings::lineCountThreshold()*/) {
                w += helper->width();
            }
        }
        return w;
    }
    return QProxyStyle::pixelMetric(metric, option, widget);
}

QRect MinimapProxyStyle::subControlRect(ComplexControl cc,
                                        const QStyleOptionComplex *opt,
                                        SubControl sc,
                                        const QWidget *widget) const {

    int valmini = GetSettingsManager()->GetSettingInt("UseMinimap",0);

    if (widget && cc == QStyle::CC_ScrollBar && valmini == 1) {
        QVariant v =
            widget->property(Constants::MINIMAP_STYLE_OBJECT_PROPERTY);
        if (v.isValid()) {
            auto *helper =
                static_cast<MinimapProxyStyleHelper *>(v.value<QObject *>());
            int lineCount = helper->lineCount();
            if (lineCount > 0 && lineCount <= 8000/*Settings::lineCountThreshold()*/) {
                if (sc == QStyle::SC_ScrollBarGroove) {
                    return helper->groove();
                }
                if (sc == QStyle::SC_ScrollBarAddPage) {
                    return helper->addPage();
                }
                if (sc == QStyle::SC_ScrollBarSubPage) {
                    return helper->subPage();
                }
                if (sc == QStyle::SC_ScrollBarSlider) {
                    return helper->slider();
                }
                return QRect();
            }
        }
    }
    return QProxyStyle::subControlRect(cc, opt, sc, widget);
}

bool MinimapProxyStyle::drawMinimap(const QStyleOptionComplex *option,
                                    QPainter *painter,
                                    const QWidget *widget,
                                    MinimapProxyStyleHelper *helper) const
    noexcept {



    /*if (TextEditor::TextEditorSettings::displaySettings().m_textWrapping) {
        return false;
    }*/
    const QScrollBar *scrollbar = qobject_cast<const QScrollBar *>(widget);
    if (!scrollbar) {
        return false;
    }
    int h = helper->editor()->size().height();

    CodeEditor* editor = helper->editor();

    qreal factor = helper->factor();
    if (factor < 1.0) {
        h = helper->lineCount();
    }
    qreal step = 1 / factor;
    QColor baseBg = helper->background();
    QColor baseFg = helper->foreground();
    int w = helper->width() - Constants::MINIMAP_EXTRA_AREA_WIDTH;
    QImage image(helper->width(), h, QImage::Format_RGB32);
    image.fill(baseBg);
    QTextDocument *doc = helper->editor()->document();

    QAbstractTextDocumentLayout *documentLayout =(doc->documentLayout());
    int tab = 4;//helper->editor()->textDocument()->tabSettings().m_tabSize;
    int y = 0;
    int i = 0;
    qreal r = 0.0;
    bool codeFoldingVisible = false;//helper->editor()->codeFoldingVisible();
    bool revisionsVisible = false;//helper->editor()->revisionsVisible();
    bool folded = false;
    int revision = 0;

    std::vector<int> ErrorLines = helper->editor()->GetHighlightErrorLines();
    std::vector<int> SearchLines = helper->editor()->GetHighlightSearchLines();

    QColor Errorcolor = QColor(255,0,0);
    QColor Searchcolor = QColor(0,255,0);
    QColor hlcolor;
    int blockn = 0;


    bool is_error = false;
    bool is_search = false;
    int index_last_error = 0;
    int index_last_search = 0;

    int last_error = -10;
    int last_search = -10;

    for (QTextBlock b = doc->begin(); b.isValid() && y < h; b = b.next()) {
        bool updateY = true;

        if(ErrorLines.size() > index_last_error)
        {
            while(ErrorLines.at(index_last_error) <= blockn)
            {
                last_error = blockn;
                index_last_error ++;
                if(!(ErrorLines.size() > index_last_error))
                {
                    break;
                }
            }
        }

        if(SearchLines.size() > index_last_search)
        {
            while(SearchLines.at(index_last_search) <= blockn)
            {
                last_search = blockn;
                index_last_search ++;
                if(!(SearchLines.size() > index_last_search))
                {
                    break;
                }
            }
        }


        is_search = (blockn -last_search) < 3;
        is_error = (blockn -last_error) < 3;
        hlcolor = is_error? Errorcolor : baseBg;
        if(is_search)
        {
            if(is_error)
            {
                hlcolor = QColor(255,255,0);

            }else
            {
                hlcolor = Searchcolor;
            }
        }



        blockn ++;

        if (b.isVisible()) {
            if (qRound(r) != i++) {
                updateY = false;
            } else {
                r += step;
            }
        } else {
            continue;
        }
        if (codeFoldingVisible && !folded) {
            folded = false;//isFolded(b);
        }
        /*if (revisionsVisible) {
            if (b.revision() != documentLayout->lastSaveRevision) {
                if (revision < 1 && b.revision() < 0) {
                    revision = 1;
                } else if (revision < 2) {
                    revision = 2;
                }
            }
        }*/

        //does this line have a search result?

        //does this line have an error?


        int x = 0;
        bool cont = true;
        QRgb *scanLine = reinterpret_cast<QRgb *>(image.scanLine(y));
        QVector<QTextLayout::FormatRange> formats = b.layout()->formats();
        std::sort(formats.begin(),
                  formats.end(),
                  [](const QTextLayout::FormatRange &r1,
                     const QTextLayout::FormatRange &r2) {
                      if (r1.start < r2.start) {
                          return true;
                      } else if (r1.start > r2.start) {
                          return false;
                      }
                      return r1.length < r2.length;
                  });
        QColor bBg = (is_error || is_search)? hlcolor: baseBg;
        QColor bFg = (is_error  || is_search)? QColor((baseFg.red() + hlcolor.red())/2,(baseFg.green() + hlcolor.green())/2,(baseFg.blue()+ hlcolor.blue())/2) : baseFg;
        merge(bBg, bFg, b.charFormat());
        auto it2 = formats.begin();
        for (QTextBlock::iterator it = b.begin(); !(it.atEnd()); ++it) {
            QTextFragment f = it.fragment();
            if (f.isValid()) {
                QColor fBg = bBg;
                QColor fFg = bFg;
                merge(fBg, fFg, f.charFormat());
                for (const QChar &c : f.text()) {
                    QColor bg = fBg;
                    QColor fg = fFg;
                    it2 = std::find_if(
                        it2,
                        formats.end(),
                        [&x](const QTextLayout::FormatRange &range) {
                            return x >= range.start &&
                                   x < range.start + range.length;
                        });
                    if (it2 != formats.end()) {
                        merge(bg, fg, it2->format);
                    }
                    cont = updatePixel(
                        &scanLine[Constants::MINIMAP_EXTRA_AREA_WIDTH],
                        !updateY,
                        c,
                        x,
                        w,
                        tab,
                        bg,
                        fg);
                    if (!cont) {
                        break;
                    }
                }


                if (!cont) {
                    break;
                }
            } else {
                cont = false;
                break;
            }

            if(is_error || is_search)
            {
                for(int l = 0; l < 200; l++)
                {

                    QColor bg = bBg;
                    QColor fg = bFg;
                    cont = updatePixel(
                        &scanLine[Constants::MINIMAP_EXTRA_AREA_WIDTH],
                        !updateY,
                        QChar(' '),
                        x,
                        w,
                        tab,
                        bg,
                        fg);
                    if (!cont) {
                        break;
                    }
                }


            }
        }
        if (updateY) {
            ++y;
            if (revision == 1) {
                scanLine[1] = QColor(Qt::darkGreen).rgb();
                scanLine[2] = QColor(Qt::darkGreen).rgb();
            } else if (revision == 2) {
                scanLine[1] = QColor(Qt::red).rgb();
                scanLine[2] = QColor(Qt::red).rgb();
            }
            if (folded) {
                scanLine[4] = QColor(Qt::black).rgb();
                scanLine[5] = QColor(Qt::black).rgb();
            }
            folded = false;
            revision = 0;
        }
    }
    painter->save();
    painter->fillRect(option->rect, baseBg);
    painter->drawImage(option->rect, image, option->rect);
    painter->setPen(Qt::NoPen);
    painter->setBrush(helper->overlay());
    QRect rect = this->subControlRect(QStyle::CC_ScrollBar,
                                      option,
                                      QStyle::SC_ScrollBarSlider,
                                      widget)
                     .intersected(option->rect);
    painter->drawRect(rect);
    painter->restore();
    return true;
}

void MinimapProxyStyle::baseEditorCreated(
    CodeEditor *editor) noexcept {
    // Qt parent-child relationship will cover deletion
    new MinimapProxyStyleHelper(editor);
}

} // namespace Minimap::Internal
