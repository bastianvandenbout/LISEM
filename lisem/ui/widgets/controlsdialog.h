#ifndef CONTROLSDIALOG_H
#define CONTROLSDIALOG_H

#include <QDialog>
#include <qlayout.h>
#include <QGroupBox>
#include "QToolButton"
#include "QKeyEvent"
#include "QMouseEvent"
#include "settings/generalsettingsmanager.h"
#include "widgets/labeledwidget.h"


//we use the qt codes for buttons, and let the opengl side of things convert this to glfw3 codes later on
class ControlWidget : public QPushButton
{
Q_OBJECT
    bool is_listening = false;
    int value = 0;

    std::function<void(int)> m_onChanged;
public:
    inline ControlWidget(int current, bool must_be_mouse, std::function<void(int)> onChanged)
    {
        m_onChanged =  onChanged;
        value = current;
        if(must_be_mouse)
        {
            int enum_index = qt_getQtMetaObject()->indexOfEnumerator("Key");
              const char* string =
                  qt_getQtMetaObject()->enumerator(enum_index).valueToKey(current);
              if(string == nullptr)
              {
                  this->setText("Unkown");
              }else
              {
                  this->setText(string);
              }
        }else
        {
            int enum_index = qt_getQtMetaObject()->indexOfEnumerator("MouseButton");
              const char* string =
                  qt_getQtMetaObject()->enumerator(enum_index).valueToKey(current);
            if(string == nullptr)
            {
                this->setText("Unkown");
            }else
            {
                this->setText(string);
            }
        }

        connect(this,&QPushButton::pressed,this,&ControlWidget::onClicked);

    }


    void keyPressEvent(QKeyEvent *event) override
    {
        if(is_listening)
        {
            //cancel in case of escape button
            if(event->key() == Qt::Key::Key_Escape)
            {
                int enum_index = qt_getQtMetaObject()->indexOfEnumerator("Key");
                  const char* string =
                      qt_getQtMetaObject()->enumerator(enum_index).valueToKey(value);
                  if(string == nullptr)
                  {
                      this->setText("Unkown");
                  }else
                  {
                      this->setText(string);
                  }
                is_listening = false;
                return;
            }
            value = event->key();
            m_onChanged(value);
            int enum_index = qt_getQtMetaObject()->indexOfEnumerator("Key");
              const char* string =
                  qt_getQtMetaObject()->enumerator(enum_index).valueToKey(value);
              if(string == nullptr)
              {
                  this->setText("Unkown");
              }else
              {
                  this->setText(string);
              }
            is_listening = false;
            return;
        }
        QPushButton::keyPressEvent(event);

    }

    void mousePressEvent(QMouseEvent *event) override
    {
        if(is_listening)
        {
            value = event->button();
            m_onChanged(value);
            int enum_index = qt_getQtMetaObject()->indexOfEnumerator("MouseButton");
              const char* string =
                  qt_getQtMetaObject()->enumerator(enum_index).valueToKey(value);
            if(string == nullptr)
            {
                this->setText("Unkown");
            }else
            {
                this->setText(string);
            }
            is_listening = false;
            return;
        }
        QPushButton::mousePressEvent(event);

    }

public slots:

    void onClicked()
    {
        this->setText("Listening");
        is_listening = true;
    }
};

class ControlDialog : public QDialog
{
Q_OBJECT

public:

    inline ControlDialog() : QDialog()
    {

        QVBoxLayout *l = new QVBoxLayout();
        QGroupBox *gb1 = new QGroupBox("Map Viewer");
        QGroupBox *gb2 = new QGroupBox("Scripting");

        l->addWidget(gb1);
        l->addWidget(gb2);

        this->setLayout(l);

        QVBoxLayout *l1 = new QVBoxLayout();
        gb1->setLayout(l1);
        QVBoxLayout *l2 = new QVBoxLayout();
        gb2->setLayout(l2);

        GeneralSettingsManager  * m = GetSettingsManager();


        //Drag Button
        l1->addWidget(new QWidgetHDuo(new QLabel("Drag Button"),new ControlWidget(m->GetSettingInt("ButtonDrag",Qt::MouseButton::RightButton),true,[this,m](int key){
        m->SetSetting("ButtonDrag",key);
        })));
        //Interact button
        l1->addWidget(new QWidgetHDuo(new QLabel("Interact Button"),new ControlWidget(m->GetSettingInt("ButtonInteract",Qt::MouseButton::LeftButton),true,[this,m](int key){
        m->SetSetting("ButtonInteract",key);
        })));
        //Edit drag button
        l1->addWidget(new QWidgetHDuo(new QLabel("Edit Drag Button"),new ControlWidget(m->GetSettingInt("ButtonEditDrag",Qt::MouseButton::RightButton),true,[this,m](int key){
        m->SetSetting("ButtonEditDrag",key);
        })));
        //Lines button
        l1->addWidget(new QWidgetHDuo(new QLabel("Edit Toggle"),new ControlWidget(m->GetSettingInt("ButtonEdit",Qt::Key::Key_Shift),false,[this,m](int key){
        m->SetSetting("ButtonEdit",key);
        })));
        //Copy button
        l1->addWidget(new QWidgetHDuo(new QLabel("Action Button"),new ControlWidget(m->GetSettingInt("ButtonAction",Qt::MouseButton::LeftButton),true,[this,m](int key){
        m->SetSetting("ButtonAction",key);
        })));
        //Clear Selection button
        l1->addWidget(new QWidgetHDuo(new QLabel("Clear/Cancel Button"),new ControlWidget(m->GetSettingInt("ButtonClear",Qt::MouseButton::RightButton),true,[this,m](int key){
        m->SetSetting("ButtonClear",key);
        })));

        //AutoStyle button
        l2->addWidget(new QWidgetHDuo(new QLabel("Autostyle Button (Ctrl+)"),new ControlWidget(m->GetSettingInt("ButtonStyle",Qt::Key::Key_Q),false,[this,m](int key){
        m->SetSetting("ButtonStyle",key);
        })));
        //AutoComment button
        l2->addWidget(new QWidgetHDuo(new QLabel("Autocomment Button"),new ControlWidget(m->GetSettingInt("ButtonComment",Qt::Key::Key_Tab),false,[this,m](int key){
        m->SetSetting("ButtonStyle",key);
        })));
        //AutoCommentRemove Button
        l2->addWidget(new QWidgetHDuo(new QLabel("Remove comment Button (Shift+)"),new ControlWidget(m->GetSettingInt("ButtonCommentR",Qt::Key::Key_Tab),false,[this,m](int key){
        m->SetSetting("ButtonCommentR",key);
        })));
        //Search and Replace Button
        l2->addWidget(new QWidgetHDuo(new QLabel("Search Button (Ctrl+)"),new ControlWidget(m->GetSettingInt("ButtonSearch",Qt::Key::Key_F),false,[this,m](int key){
        m->SetSetting("ButtonSearch",key);
        })));
        //Show Autocomplete Button
        l2->addWidget(new QWidgetHDuo(new QLabel("Autocomment Button (Ctrl+)"),new ControlWidget(m->GetSettingInt("ButtonStyle",Qt::Key::Key_Q),false,[this,m](int key){
        m->SetSetting("ButtonStyle",key);
        })));
        //Remove Autocomplete Button
        l2->addWidget(new QWidgetHDuo(new QLabel("Autocomment Button (Ctrl+)"),new ControlWidget(m->GetSettingInt("ButtonStyle",Qt::Key::Key_Q),false,[this,m](int key){
        m->SetSetting("ButtonStyle",key);
        })));
        //Run Script Button
        l2->addWidget(new QWidgetHDuo(new QLabel("Autocomment Button (Ctrl+)"),new ControlWidget(m->GetSettingInt("ButtonStyle",Qt::Key::Key_Q),false,[this,m](int key){
        m->SetSetting("ButtonStyle",key);
        })));
        //Stop Script Button
        l2->addWidget(new QWidgetHDuo(new QLabel("Autocomment Button (Ctrl+)"),new ControlWidget(m->GetSettingInt("ButtonStyle",Qt::Key::Key_Q),false,[this,m](int key){
        m->SetSetting("ButtonStyle",key);
        })));
        //Compile Script Button
        l2->addWidget(new QWidgetHDuo(new QLabel("Autocomment Button (Ctrl+)"),new ControlWidget(m->GetSettingInt("ButtonStyle",Qt::Key::Key_Q),false,[this,m](int key){
        m->SetSetting("ButtonStyle",key);
        })));
        //Show Debugger Button
        l2->addWidget(new QWidgetHDuo(new QLabel("Autocomment Button (Ctrl+)"),new ControlWidget(m->GetSettingInt("ButtonStyle",Qt::Key::Key_Q),false,[this,m](int key){
        m->SetSetting("ButtonStyle",key);
        })));
        //Save Button
        l2->addWidget(new QWidgetHDuo(new QLabel("Autocomment Button (Ctrl+)"),new ControlWidget(m->GetSettingInt("ButtonStyle",Qt::Key::Key_Q),false,[this,m](int key){
        m->SetSetting("ButtonStyle",key);
        })));
        //Save As Button
        l2->addWidget(new QWidgetHDuo(new QLabel("Autocomment Button (Ctrl+)"),new ControlWidget(m->GetSettingInt("ButtonStyle",Qt::Key::Key_Q),false,[this,m](int key){
        m->SetSetting("ButtonStyle",key);
        })));
        //Open Button
        l2->addWidget(new QWidgetHDuo(new QLabel("Autocomment Button (Ctrl+)"),new ControlWidget(m->GetSettingInt("ButtonStyle",Qt::Key::Key_Q),false,[this,m](int key){
        m->SetSetting("ButtonStyle",key);
        })));
        //Open File in Script Editor Button
        l2->addWidget(new QWidgetHDuo(new QLabel("Autocomment Button (Ctrl+)"),new ControlWidget(m->GetSettingInt("ButtonStyle",Qt::Key::Key_Q),false,[this,m](int key){
        m->SetSetting("ButtonStyle",key);
        })));
        //Open File in Map Viewer Button
        l2->addWidget(new QWidgetHDuo(new QLabel("Autocomment Button (Ctrl+)"),new ControlWidget(m->GetSettingInt("ButtonStyle",Qt::Key::Key_Q),false,[this,m](int key){
        m->SetSetting("ButtonStyle",key);
        })));


        QPushButton *resetbutton = new QPushButton("reset");
        QPushButton *okebutton = new QPushButton("ok");

        connect(resetbutton,&QPushButton::pressed,[this](){
            reset();
        });
        connect(okebutton,&QPushButton::pressed,[this](){
            this->close();
        });
        l->addWidget(new QWidgetHDuo(resetbutton,okebutton));
        this->setLayout(l);

    }

    inline void reset()
    {

    }


};


#endif // CONTROLSDIALOG_H
