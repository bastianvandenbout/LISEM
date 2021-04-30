#pragma once
#include <QLabel>

// MIT - Yash : Speedovation.com [ Picked from internet and modified. if owner needs to add or change license do let me know.]

class ElidedLabel : public QLabel
{
    Q_OBJECT
public:
    using QLabel::QLabel;
    // Set the elide mode used for displaying text.
    void setElideMode(Qt::TextElideMode elideMode);
    // Get the elide mode currently used to display text.
    Qt::TextElideMode elideMode() const { return m_elideMode; }

protected:
    void paintEvent(QPaintEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;

private:
    void updateCachedTexts();

private:
    Qt::TextElideMode m_elideMode = Qt::ElideLeft;
    QString m_cachedElidedText;
    QString m_cachedText;
};

