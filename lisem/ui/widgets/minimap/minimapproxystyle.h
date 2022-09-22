#pragma once

#include <QProxyStyle>
#include "scripting/codeeditor.h"
namespace TextEditor {

class BaseTextEditor;

}

namespace Minimap::Internal {

class MinimapProxyStyleHelper;

class MinimapProxyStyle final : public QProxyStyle {
    Q_OBJECT

public:
    explicit MinimapProxyStyle(QStyle *style) noexcept;

    void drawComplexControl(ComplexControl control,
                            const QStyleOptionComplex *option,
                            QPainter *painter,
                            const QWidget *widget = nullptr) const override;

    // need this due to QTBUG-24279
    SubControl
    hitTestComplexControl(ComplexControl control,
                          const QStyleOptionComplex *option,
                          const QPoint &pos,
                          const QWidget *widget = nullptr) const override;

    int pixelMetric(PixelMetric metric,
                    const QStyleOption *option = nullptr,
                    const QWidget *widget = nullptr) const override;

    QRect subControlRect(ComplexControl cc,
                         const QStyleOptionComplex *opt,
                         SubControl sc,
                         const QWidget *widget) const override;

    static void baseEditorCreated(CodeEditor *editor) noexcept;

private:
    bool drawMinimap(const QStyleOptionComplex *option,
                     QPainter *painter,
                     const QWidget *widget,
                     MinimapProxyStyleHelper *helper) const noexcept;
};

} // namespace Minimap::Internal
