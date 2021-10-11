#include "elidedlabel.h"

#include <QPaintEvent>
#include <QResizeEvent>
#include "elidedlabel.h"

#include <QResizeEvent>
#include <QTimer>

ElidedLabel::ElidedLabel(QWidget *parent, Qt::WindowFlags f) :
    QLabel(parent, f)
{
    defaultType = Qt::ElideMiddle;
    eliding = false;
    original = "";
}

ElidedLabel::ElidedLabel(const QString &text, QWidget *parent, Qt::WindowFlags f) :
    QLabel(text, parent, f)
{
    defaultType = Qt::ElideMiddle;

    //Usado para verificar se a string está ou não sendo atualizado
    eliding = false;

    //Guarda o texto original
    setText(text);
}

void ElidedLabel::setType(const Qt::TextElideMode type)
{
    /*
       Altera o tipo de elide, podendo ser:
       Esquerda: "... bar baz"
       Meido: "Foo ... baz"
       Direita: "Foo bar ..."
    */
    defaultType = type;
    elide();
}

//Atualiza o texto se o Label for redimensionado
void ElidedLabel::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    //O delay é necessário para evitar conflitos
    QTimer::singleShot(50, this, SLOT(elide()));
}

//Atualiza o texto
void ElidedLabel::setText(const QString &text)
{
    original = text;
    QLabel::setText(text);

    //Executa no momento que é atualizado
    elide();
}

void ElidedLabel::elide()
{
    if (eliding == false) {
        eliding = true;

        QFontMetrics metrics(font());
        QLabel::setText(metrics.elidedText(original, defaultType, width()));

        eliding = false;
    }
}
