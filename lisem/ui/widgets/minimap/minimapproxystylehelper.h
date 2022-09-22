#pragma once

#include <QColor>
#include <QObject>
#include <QRect>
#include "scripting/codeeditor.h"

namespace Utils {

class Theme;

}

namespace Minimap::Internal {

class MinimapProxyStyleHelper final : public QObject {
    Q_OBJECT

public:
    MinimapProxyStyleHelper(CodeEditor *editor) noexcept;
    ~MinimapProxyStyleHelper() noexcept override;

    bool eventFilter(QObject *watched, QEvent *event) override;

    int width() const noexcept;
    const QRect &groove() const noexcept;
    const QRect &addPage() const noexcept;
    const QRect &subPage() const noexcept;
    const QRect &slider() const noexcept;
    int lineCount() const noexcept;
    qreal factor() const noexcept;
    const QColor &background() const noexcept;
    const QColor &foreground() const noexcept;
    const QColor &overlay() const noexcept;
    CodeEditor *editor() const noexcept;

private:
    void init() noexcept;
    void contentsChanged() noexcept;
    void fontSettingsChanged() noexcept;
    void deferedUpdate() noexcept;
    void update() noexcept;
    void updateSubControlRects() noexcept;

    CodeEditor *m_editor = nullptr;
    qreal m_factor = 0.0;
    int m_lineCount = 0;
    QRect m_groove;
    QRect m_addPage;
    QRect m_subPage;
    QRect m_slider;
    QColor m_backgroundColor;
    QColor m_foregroundColor;
    QColor m_overlayColor;
    bool m_update = false;
};

} // namespace Minimap::Internal
