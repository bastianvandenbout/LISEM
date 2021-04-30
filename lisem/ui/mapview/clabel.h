#ifndef CLABEL_H
#define CLABEL_H
#include "qlabel.h"

class CLabel : public QLabel {
    Q_OBJECT

    friend class QWidget;
    int m_arg;
public:
    inline explicit CLabel(QString name = "",QWidget* parent = Q_NULLPTR, int arg = 0) : QLabel(name, parent)
    {

        setFocusPolicy(Qt::NoFocus);

        m_arg=arg;
    }
    inline ~CLabel()
    {

    }

signals:
    void clicked();
    void clicked(int);
protected:
    void mousePressEvent(QMouseEvent* event) override;
};


#endif // CLABEL_H
