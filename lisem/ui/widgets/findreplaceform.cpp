/*
 * Copyright (C) 2009  Lorenzo Bettini <http://www.lorenzobettini.it>
 * See COPYING file that comes with this distribution
 */

#include <QtGui>
#include "QPlainTextEdit"
#include <QRegExp>
#include <QSettings>
#include <QTableView>

#include "findreplaceform.h"
#include "ui_findreplaceform.h"

#define TEXT_TO_FIND "textToFind"
#define TEXT_TO_REPLACE "textToReplace"
#define DOWN_RADIO "downRadio"
#define UP_RADIO "upRadio"
#define CASE_CHECK "caseCheck"
#define WHOLE_CHECK "wholeCheck"
#define REGEXP_CHECK "regexpCheck"

FindReplaceForm::FindReplaceForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FindReplaceForm), textEdit(nullptr), tableEdit(nullptr)
{
    ui->setupUi(this);

    ui->errorLabel->setText("");

    connect(ui->textToFind, SIGNAL(textChanged(QString)), this, SLOT(textToFindChanged()));
    connect(ui->textToFind, SIGNAL(textChanged(QString)), this, SLOT(validateRegExp(QString)));

    connect(ui->regexCheckBox, SIGNAL(toggled(bool)), this, SLOT(regexpSelected(bool)));

    connect(ui->findButton, SIGNAL(clicked()), this, SLOT(find()));
    connect(ui->closeButton, SIGNAL(clicked()), parent, SLOT(close()));

    connect(ui->replaceButton, SIGNAL(clicked()), this, SLOT(replace()));
    connect(ui->replaceAllButton, SIGNAL(clicked()), this, SLOT(replaceAll()));
}

FindReplaceForm::~FindReplaceForm()
{
    delete ui;
}

void FindReplaceForm::hideReplaceWidgets() {
    ui->replaceLabel->setVisible(false);
    ui->textToReplace->setVisible(false);
    ui->replaceButton->setVisible(false);
    ui->replaceAllButton->setVisible(false);
}

void FindReplaceForm::setTextEdit(QPlainTextEdit *textEdit_) {
    textEdit = textEdit_;
    connect(textEdit, SIGNAL(copyAvailable(bool)), ui->replaceButton, SLOT(setEnabled(bool)));
    connect(textEdit, SIGNAL(copyAvailable(bool)), ui->replaceAllButton, SLOT(setEnabled(bool)));
}

void FindReplaceForm::setTableEdit(QTableView *textEdit_) {
    tableEdit = textEdit_;
    //connect(textEdit, SIGNAL(copyAvailable(bool)), ui->replaceButton, SLOT(setEnabled(bool)));
    //connect(textEdit, SIGNAL(copyAvailable(bool)), ui->replaceAllButton, SLOT(setEnabled(bool)));
}
void FindReplaceForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void FindReplaceForm::textToFindChanged() {
    ui->findButton->setEnabled(ui->textToFind->text().size() > 0);
}

void FindReplaceForm::regexpSelected(bool sel) {
    if (sel)
        validateRegExp(ui->textToFind->text());
    else
        validateRegExp("");
}

void FindReplaceForm::validateRegExp(const QString &text) {
    if (!ui->regexCheckBox->isChecked() || text.size() == 0) {
        ui->errorLabel->setText("");
        return; // nothing to validate
    }

    QRegExp reg(text,
                (ui->caseCheckBox->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive));

    if (reg.isValid()) {
        showError("");
    } else {
        showError(reg.errorString());
    }
}

void FindReplaceForm::showError(const QString &error) {
    if (error == "") {
        ui->errorLabel->setText("");
    } else {
        ui->errorLabel->setText("<span style=\" font-weight:600; color:#ff0000;\">" +
                                error +
                                "</spam>");
    }
}

void FindReplaceForm::showMessage(const QString &message) {
    if (message == "") {
        ui->errorLabel->setText("");
    } else {
        ui->errorLabel->setText("<span style=\" font-weight:600; color:green;\">" +
                                message +
                                "</spam>");
    }
}

void FindReplaceForm::find() {
    find(ui->downRadioButton->isChecked());
}

void FindReplaceForm::find(bool next) {
    if (textEdit != nullptr)
    {

        // backward search
        bool back = !next;

        const QString &toSearch = ui->textToFind->text();

        bool result = false;

        QTextDocument::FindFlags flags;

        if (back)
            flags |= QTextDocument::FindBackward;
        if (ui->caseCheckBox->isChecked())
            flags |= QTextDocument::FindCaseSensitively;
        if (ui->wholeCheckBox->isChecked())
            flags |= QTextDocument::FindWholeWords;

        if (ui->regexCheckBox->isChecked()) {
            QRegExp reg(toSearch,
                        (ui->caseCheckBox->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive));

            qDebug() << "searching for regexp: " << reg.pattern();

            textCursor = textEdit->document()->find(reg, textCursor, flags);
            textEdit->setTextCursor(textCursor);
            result = (!textCursor.isNull());
        } else {
            qDebug() << "searching for: " << toSearch;

            result = textEdit->find(toSearch, flags);
        }

        if (result) {
            showError("");
        } else {
            showError(tr("no match found"));
            // move to the beginning of the document for the next find
            textCursor.setPosition(0);
            textEdit->setTextCursor(textCursor);
        }
    }else if(tableEdit != nullptr)
    {


        const QString &toSearch = ui->textToFind->text();

        //get current selected index (which is starting location)
        QModelIndex current_i = tableEdit->selectionModel()->currentIndex();
        if(!current_i.isValid())
        {
            current_i = tableEdit->model()->index(0,0);
        }
        int columns = tableEdit->model()->columnCount();
        int rows = tableEdit->model()->rowCount();
        QModelIndex current_in = (current_i.column() != columns -1)? current_i.sibling(current_i.row(),current_i.column()): ( (current_i.row() == rows -1) ? current_i.sibling(0,0) : current_i.sibling(current_i.row()+1,0));

        //QModelIndexList ilist = tableEdit->model()->match(current_in,Qt::DisplayRole,toSearch,2,Qt::MatchContains | Qt::MatchWrap | Qt::MatchRecursive);

        //custom search (the QAbstractItemModel Match function is very very buggy!!)

        int current_r = current_in.row();
        int current_c = current_in.column();


        QAbstractItemModel * model = tableEdit->model();

        QModelIndexList ilist;

        bool do_regexp = false;
        QRegExp reg(toSearch,
                    (ui->caseCheckBox->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive));
        if (ui->regexCheckBox->isChecked()) {
            do_regexp = true;

        }

        bool first = true;
        if(next)
        {
            for(int c= current_c; c < columns; c++)
            {
                int r0 = 0;
                if(c == current_c)
                {
                    r0 = current_r;
                }
                for(int r= r0; r < rows; r++)
                {
                    if(!first)
                    {
                        //get data
                        QVariant data = model->data(model->index(r,c),Qt::DisplayRole);
                        //compare
                        if(do_regexp)
                        {
                            if(data.toString().contains(reg))
                            {
                                //add to list
                                ilist.append(model->index(r,c));
                                //one is enough so break;
                                break;
                            }
                        }else
                        {
                            if(data.toString().contains(toSearch,(ui->caseCheckBox->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive)))
                            {
                                //add to list
                                ilist.append(model->index(r,c));
                                //one is enough so break;
                                break;
                            }
                        }
                    }
                    first = false;
                }
            }
        }else
        {
            for(int c= current_c; c > -1; c--)
            {
                int r0 = rows;
                if(c == current_c)
                {
                    r0 = current_r;
                }
                for(int r= r0; r > -1; r--)
                {
                    if(!first)
                    {
                        //get data
                        QVariant data = model->data(model->index(r,c),Qt::DisplayRole);
                        //compare
                        if(data.toString().contains(toSearch,(ui->caseCheckBox->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive)))
                        {
                            //add to list
                            ilist.append(model->index(r,c));
                            //one is enough so break;
                            break;
                        }
                    }
                    first = false;
                }
            }
        }

        if(ilist.length() == 0)
        {
            ui->replaceButton->setEnabled(false);
            ui->replaceAllButton->setEnabled(false);

        }else
        {
            ui->replaceButton->setEnabled(true);
            ui->replaceAllButton->setEnabled(true);
        }

        if(ilist.length() > 0)
        {
            showError("");
            tableEdit->setCurrentIndex(ilist.at(0));
        }else
        {
            showError(tr("no match found"));
            tableEdit->setCurrentIndex(tableEdit->model()->index(0,0));
        }

    }

}

void FindReplaceForm::replace() {

    if (textEdit != nullptr)
    {
        if (!textEdit->textCursor().hasSelection()) {
            find();
        } else {
            textEdit->textCursor().insertText(ui->textToReplace->text());
            find();
        }
    }else if(tableEdit != nullptr)
    {

        const QString &toSearch = ui->textToFind->text();
        QModelIndex current_i = tableEdit->selectionModel()->currentIndex();
        QAbstractItemModel * model = tableEdit->model();
        bool do_regexp = false;
        QRegExp reg(toSearch,
                    (ui->caseCheckBox->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive));
        if (ui->regexCheckBox->isChecked()) {
            do_regexp = true;

        }

        if(current_i.isValid())
        {
            QVariant data = model->data(current_i,Qt::DisplayRole);
            if(do_regexp)
            {
                if(data.toString().contains(reg))
                {
                    model->setData(current_i,QVariant(data.toString().replace(reg,ui->textToReplace->text())));
                }else
                {
                }
            }else
            {
                if(data.toString().contains(toSearch,(ui->caseCheckBox->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive)))
                {
                    model->setData(current_i,QVariant(data.toString().replace(toSearch,ui->textToReplace->text())));
                }else
                {
                }
            }
        }else
        {
        }

        find();
    }
}

void FindReplaceForm::replaceAll() {

    if (textEdit != nullptr)
    {
        int i=0;

        while (textEdit->textCursor().hasSelection()){
            textEdit->textCursor().insertText(ui->textToReplace->text());
            find();
            i++;
        }
        showMessage(tr("Replaced %1 occurrence(s)").arg(i));

    }else if(tableEdit != nullptr)
    {
        QModelIndex current_i = tableEdit->selectionModel()->currentIndex();
        const QString &toSearch = ui->textToFind->text();
        bool do_regexp = false;
        QRegExp reg(toSearch,
                    (ui->caseCheckBox->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive));
        if (ui->regexCheckBox->isChecked()) {
            do_regexp = true;

        }
        QAbstractItemModel * model = tableEdit->model();

        int i = 0;


        //if we have no match, try to find one
        current_i = tableEdit->selectionModel()->currentIndex();
        if(current_i.isValid())
        {
            QVariant data = model->data(current_i,Qt::DisplayRole);
            if(do_regexp)
            {
                if(!data.toString().contains(reg))
                {
                    find();
                }
            }else
            {
                if(!data.toString().contains(toSearch,(ui->caseCheckBox->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive)))
                {
                    find();
                }
            }
        }else
        {
            find();
        }


        bool found_none = false;
        while(!found_none)
        {
            //get current index
            current_i = tableEdit->selectionModel()->currentIndex();

            //check if there is a match
            if(current_i.isValid())
            {
                QVariant data = model->data(current_i,Qt::DisplayRole);
                if(do_regexp)
                {
                    if(data.toString().contains(reg))
                    {
                        model->setData(current_i,QVariant(data.toString().replace(reg,ui->textToReplace->text())));
                    }else
                    {
                        found_none = true;
                        break;
                    }
                }else
                {
                    if(data.toString().contains(toSearch,(ui->caseCheckBox->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive)))
                    {
                        model->setData(current_i,QVariant(data.toString().replace(toSearch,ui->textToReplace->text())));
                    }else
                    {
                        found_none = true;
                        break;
                    }
                }
            }else
            {
                found_none = true;
                break;
            }

            //we have replaced the current match
            //find the next
            find();

            //increment the count
            i++;
        }

        showMessage(tr("Replaced %1 occurrence(s)").arg(i));

        ui->replaceButton->setEnabled(false);
        ui->replaceAllButton->setEnabled(false);
    }
}

void FindReplaceForm::writeSettings(QSettings &settings, const QString &prefix) {
    settings.beginGroup(prefix);
    settings.setValue(TEXT_TO_FIND, ui->textToFind->text());
    settings.setValue(TEXT_TO_REPLACE, ui->textToReplace->text());
    settings.setValue(DOWN_RADIO, ui->downRadioButton->isChecked());
    settings.setValue(UP_RADIO, ui->upRadioButton->isChecked());
    settings.setValue(CASE_CHECK, ui->caseCheckBox->isChecked());
    settings.setValue(WHOLE_CHECK, ui->wholeCheckBox->isChecked());
    settings.setValue(REGEXP_CHECK, ui->regexCheckBox->isChecked());
    settings.endGroup();
}

void FindReplaceForm::readSettings(QSettings &settings, const QString &prefix) {
    settings.beginGroup(prefix);
    ui->textToFind->setText(settings.value(TEXT_TO_FIND, "").toString());
    ui->textToReplace->setText(settings.value(TEXT_TO_REPLACE, "").toString());
    ui->downRadioButton->setChecked(settings.value(DOWN_RADIO, true).toBool());
    ui->upRadioButton->setChecked(settings.value(UP_RADIO, false).toBool());
    ui->caseCheckBox->setChecked(settings.value(CASE_CHECK, false).toBool());
    ui->wholeCheckBox->setChecked(settings.value(WHOLE_CHECK, false).toBool());
    ui->regexCheckBox->setChecked(settings.value(REGEXP_CHECK, false).toBool());
    settings.endGroup();
}
