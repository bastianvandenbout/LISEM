#ifndef CLABEL_H
#define CLABEL_H
#include "qlabel.h"
#include "QStackedWidget"
#include "QLineEdit"

class CEditLabel : public QStackedWidget
{
public:
    QLabel * m_Label;
    QLineEdit * m_LineEdit;

    inline CEditLabel(QString name = "",QWidget* parent = Q_NULLPTR) : QStackedWidget(parent)
    {

        m_Label = new QLabel(name);
        m_LineEdit = new QLineEdit(name);

        addWidget(m_Label);
        addWidget(m_LineEdit);
        setCurrentWidget(m_Label);

    }




};

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
