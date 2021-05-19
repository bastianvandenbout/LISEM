#ifndef DOWNLOADDIALOG_H
#define DOWNLOADDIALOG_H

#include "model.h"
#include "QWidget"
#include "QDialog"
#include "QString"
#include "QHBoxLayout"
#include "QVBoxLayout"
#include "QLabel"
#include "QCheckBox"
#include "QToolButton"
#include "widgets/labeledwidget.h"
#include "QLineEdit"
#include "QSpinBox"
#include "QPushButton"
#include "lsmio.h"
#include "QFileDialog"
#include "site.h"


class DownloadDialog: public QDialog
{
        Q_OBJECT;

public:

    QString m_Url;
    QString m_File;
    QString m_Password;
    QString m_Username;
    bool do_time = false;
    int time = 0;
    QLineEdit *m_UrlEdit;
    QLineEdit *m_FileEdit;

    QToolButton * m_FileButton;


    QCheckBox * m_timeout;
    QSpinBox * m_time;

    QCheckBox * m_UseCredentials;
    QLineEdit * m_UserNameEdit;
    QLineEdit * m_PasswordEdit;

    QPushButton * okb;
    QPushButton * cancelb;

    QString m_Dir;
    inline DownloadDialog(QString url, QWidget * parent = 0, Qt::WindowFlags f = 0) : QDialog(parent,f)
    {
        m_Dir = GetSite();


        this->setWindowTitle("Add Download");

        this->setMinimumSize(500,300);
        m_Url = url;

        QVBoxLayout* mainGrid = new QVBoxLayout;


        m_UrlEdit = new QLineEdit();
        m_UrlEdit->setText(url);
        m_FileEdit = new QLineEdit();

        m_FileButton = new QToolButton();
        QIcon *icon_open = new QIcon();
        icon_open->addFile((m_Dir + LISEM_FOLDER_ASSETS + "fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);
        m_FileButton->setIcon(*icon_open);
        connect(m_FileButton,SIGNAL(pressed()),this,SLOT(OnFileOpen()));

        m_timeout = new QCheckBox();
        m_timeout->setText("Timeout");
        m_time = new QSpinBox();

        m_UseCredentials = new QCheckBox();
        m_UseCredentials->setText("Use credentials");
        m_UserNameEdit = new QLineEdit();
        m_PasswordEdit = new QLineEdit();

        QWidget * buttons = new QWidget();

        QHBoxLayout *buttonsl = new QHBoxLayout();
        buttons->setLayout(buttonsl);

        QPushButton * okb = new QPushButton("Ok");
        QPushButton * cancelb = new QPushButton("Cancel");

        connect(okb, SIGNAL(clicked(bool)),this,SLOT(onOkClicked()));
        connect(cancelb, SIGNAL(clicked(bool)),this,SLOT(onCancelClicked()));

        buttonsl->addWidget(okb);
        buttonsl->addWidget(cancelb);

        mainGrid->addWidget(new QLabel("URL"));
        mainGrid->addWidget(m_UrlEdit);
        mainGrid->addWidget(new QLabel("File"));
        mainGrid->addWidget(new QWidgetHDuo(m_FileEdit,m_FileButton));
        mainGrid->addWidget(new QWidgetHDuo(m_timeout,m_time));
        mainGrid->addWidget(m_UseCredentials);
        mainGrid->addWidget(new QWidgetHDuo(new QLabel("Username"),m_UserNameEdit));
        mainGrid->addWidget(new QWidgetHDuo(new QLabel("Password"),m_PasswordEdit));
        mainGrid->addWidget(buttons);

        this->setLayout(mainGrid);

    }

    virtual ~DownloadDialog()
    {

    }
public slots:

    inline void onOkClicked()
    {
        m_Url = m_UrlEdit->text();
        m_File = m_FileEdit->text();
        m_Username = m_UserNameEdit->text();
        m_Password = m_PasswordEdit->text();
        time = m_time->value();
        do_time = m_timeout->isChecked();


        emit accept();
    }
    inline void onCancelClicked()
    {
        m_Url = "";
        emit reject();
    }

    inline void OnFileOpen()
    {
        QString dir = m_Dir;

        if(!m_FileEdit->text().isEmpty())
        {
            dir = QFileInfo(m_FileEdit->text()).dir().path();
        }

        QString path = QFileDialog::getSaveFileName(this,
                                            QString("Select file"),
                                            dir,
                                            QString("*.*"));

        if(!path.isEmpty())
        {
            m_FileEdit->setText(path);
        }

    }
};




#endif // DOWNLOADDIALOG_H
