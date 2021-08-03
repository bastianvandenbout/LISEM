#ifndef MATRIXTABLE_H
#define MATRIXTABLE_H

#include "QString"
#include "QVector"

#include <QList>
#include "QFile"
#include "QFileInfo"
#include "QDir"
#include "error.h"

#include <QTextStream>
#include <iostream>
#include <vector>

//matches the Attribute Type definitions
#define TABLE_TYPE_UNKNOWN 0
#define TABLE_TYPE_INT32 1
#define TABLE_TYPE_INT64 2
#define TABLE_TYPE_FLOAT64 3
#define TABLE_TYPE_BOOL 4
#define TABLE_TYPE_STRING 5


struct QComparer
{
    template<typename T1>
    bool operator()(const T1 & a, const T1 & b) const
    {
        return a < b;
    }
};

struct QPairFirstComparer
{
    template<typename T1, typename T2>
    bool operator()(const QPair<T1,T2> & a, const QPair<T1,T2> & b) const
    {
        return a.first < b.first;
    }
};

struct QPairSecondComparer
{
    template<typename T1, typename T2>
    bool operator()(const QPair<T1,T2> & a, const QPair<T1,T2> & b) const
    {
        return a.second < b.second;
    }
};

struct QComparerDesc
{
    template<typename T1>
    bool operator()(const T1 & a, const T1 & b) const
    {
        return a > b;
    }
};

struct QPairFirstComparerDesc
{
    template<typename T1, typename T2>
    bool operator()(const QPair<T1,T2> & a, const QPair<T1,T2> & b) const
    {
        return a.first > b.first;
    }
};

struct QPairSecondComparerDesc
{
    template<typename T1, typename T2>
    bool operator()(const QPair<T1,T2> & a, const QPair<T1,T2> & b) const
    {
        return a.second > b.second;
    }
};

class MatrixTableParent;

class MatrixTable{

private:

    QVector<QVector<QString*>*> m_Data;


    QVector<std::string> m_ColumnTitles;
    QVector<std::string> m_RowTitles;

    //the data types for each row or column
    //can be either set for rows, or for columns
    QVector<int> m_ColumnTypes;
    QVector<int> m_RowTypes;


    bool m_HasColumnTypes = false;
    bool m_HasRowTypes = false;

public:

    inline MatrixTable()
    {

    }

    inline ~MatrixTable()
    {
        Empty();
    }

    inline MatrixTable(MatrixTable const& other)
    {

        this->CopyFrom(&other);


    }

    inline bool SaveAsAutoFormat(QString file)
    {

        if(file.isEmpty())
        {
            return false;

        }

        if(file.endsWith(".tbl"))
        {
            return SaveAs(file);
        }else { //comma seperated value

            //should we check the contents for comma's?
            //these split up the columns and rows incorrectly when loaded.

           return SaveAs(file,",",false);
       }
    }
    inline bool SaveAs(QString file, QString sep = QString(QChar('`')), bool header = true)
    {
        QFile fin(file);
        if (fin.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        {
            QTextStream out(&fin);
            if(header)
            {
                //magic code
                out << "<<!TABLEHEADER!>>";out << "\n";

                //one line with all the column names
                for(int i = 0; i < m_ColumnTitles.length(); i++)
                {
                    out << QString(m_ColumnTitles.at(i).c_str()) << sep;
                }
                out << "\n";

                //one line with all the row names
                for(int i = 0; i < m_RowTitles.length(); i++)
                {
                    out << QString(m_RowTitles.at(i).c_str()) << sep;
                }
                out << "\n";
                //one line with all the column types
                for(int i = 0; i < m_ColumnTypes.length(); i++)
                {
                    out << QString::number(m_ColumnTypes.at(i)) << sep;
                }
                out << "\n";
                //one line with all the row types

            }
            for(int i = 0; i < m_Data.length(); i++)
            {
                for(int j = 0; j < m_Data.at(i)->length(); j++)
                {
                    out << QString(this->GetValueStringQ(i,j));
                    if(j != m_Data.at(i)->length() -1)
                    {
                        out << sep;
                    }
                }
                out << "\n";
            }
        }else
        {
            return false;
        }
        fin.close();

        return true;
    }
    inline bool LoadAsAutoFormat(QString file)
    {
        if(file.isEmpty())
        {
            return false;
        }

        if(file.endsWith(".tbl"))
        {
            return LoadAs(file);
        }else { //comma seperated value
            //should we check the contents for comma's?
            //these split up the columns and rows incorrectly when loaded.

           return LoadAs(file,",",false);
       }

    }

    inline bool LoadAs(QString file, QString sep = QString(QChar('`')), bool skipempty = false)
    {
        std::cout << "load table " << file.toStdString() << std::endl;
        QFile fff(file);
        QFileInfo fi(file);
        QString S;
        QStringList Lines;

        fff.open(QIODevice::ReadOnly | QIODevice::Text);

        if (!fi.exists())
        {
            return false;
        }

        QString folder = fi.dir().absolutePath() + "/";


        while (!fff.atEnd())
        {
            S = fff.readLine();
            if (S.contains("\n"))
                S.remove(S.count()-1,1);
            // readLine also reads \n as a character on an empty line!
            if (!S.trimmed().isEmpty())
            {
                Lines << S.trimmed();
            }
            //qDebug() << S;
        }
        fff.close();



        QList<QString> ColumnNames;
        QList<int> ColumnTypes;
        QList<QString> RowNames;
        QList<int> RowTypes;

        bool first = true;
        bool hheader = false;
        QVector<QVector<QString*>*> data;
        for(int i = 0; i < Lines.length(); i++)
        {
            QString line = Lines[i];

            if(line.isEmpty())
            {
                continue;
            }
            bool first_this = false;

            if(first && line.startsWith("<<!TABLEHEADER!>>"))
            {
                first_this = true;

                first = false;

                hheader = true;
            }

            //read header
            if(first_this && hheader  && Lines.length() > i + 3)
            {
                QString linep1 = Lines[i+1]; //column names
                QString linep2 = Lines[i+2]; //row types
                QString linep3 = Lines[i+3]; //column types
                QString linep4 = Lines[i+4]; //row types


                QStringList SL1 = linep1.split(QRegExp(sep));
                QStringList SL2 = linep2.split(QRegExp(sep));
                QStringList SL3 = linep3.split(QRegExp(sep));
                //QStringList SL4 = linep4.split(QRegExp(sep));

                ColumnNames = SL1;
                RowNames = SL2;

                for(int i = 0; i < SL3.length(); i++)
                {
                    ColumnTypes.append(SL3.at(i).toInt());
                }
                /*for(int i = 0; i < SL4.length(); i++)
                {
                    RowTypes.append(SL4.at(i).toInt());
                }*/
                i+=3;
                continue;

            }else if(first_this && (Lines.length() < i + 3))
            {
                break;
            }



            QStringList SL = Lines[i].split(QRegExp(sep));
            QVector<QString*> *row = new QVector<QString*>();
            for(int j = 0; j < SL.length(); j++)
            {
                QString * s = new QString();
                *s = SL.at(j);
                row->push_back(s);
            }
            data.push_back(row);
        }

        CopyFromData(data);

        fill();

        std::cout<< " header "  << hheader << std::endl;
        for(int i = 0; i < ColumnNames.length(); i++)
        {
            std::cout << ColumnNames.at(i).toStdString()<< std::endl;
            this->SetColumnTitle(i,ColumnNames.at(i));
        }

        if(hheader)
        {

            for(int i = 0; i < ColumnNames.length(); i++)
            {
                this->SetColumnTitle(i,ColumnNames.at(i));
            }
            for(int i = 0; i < RowNames.length(); i++)
            {
                this->SetRowTitle(i,RowNames.at(i));
            }
            for(int i = 0; i < ColumnTypes.length(); i++)
            {
                this->SetColumnType(i,ColumnTypes.at(i));
            }
            for(int i = 0; i < RowTypes.length(); i++)
            {
                this->SetRowType(i,RowTypes.at(i));
            }
        }

        FileName = file;

        return true;
    }

    inline bool LoadFromFileCSV(QString file)
    {
        return LoadAs(file,",");

    }
    inline bool LoadFromFileWST(QString file)
    {
        return LoadAs(file,"\t",true);
    }


    inline QString GetDataTypeName(int type)
    {
        if(type == TABLE_TYPE_UNKNOWN)
        {
            return "Unknown";
        }else if(type == TABLE_TYPE_INT32)
        {
            return "Integer(32bit)";
        }else if(type == TABLE_TYPE_INT64)
        {
            return "Integer(64bit)";
        }else if(type == TABLE_TYPE_FLOAT64)
        {
            return "Float(64bit)";
        }else if(type == TABLE_TYPE_BOOL)
        {
            return "Boolean";
        }else if(type == TABLE_TYPE_STRING)
        {
            return "String";
        }else
        {
            return "Unknown";
        }
    }

    inline int GetColumnIndexFromTitle(QString title)
    {
        for(int i = 0; i < m_ColumnTitles.length(); i++)
        {
            if(m_ColumnTitles.at(i).compare(title.toStdString()) == 0)
            {
                return i;
            }
        }
        return -1;
    }

    inline int GetRowIndexFromTitle(QString title)
    {
        for(int i = 0; i < m_RowTitles.length(); i++)
        {
            if(m_RowTitles.at(i).compare(title.toStdString()) == 0)
            {
                return i;
            }
        }
        return -1;
    }

    inline void SetRowTypesActive(bool x)
    {
        m_HasColumnTypes = x;
        if(x)
        {
            m_HasRowTypes = false;
        }
    }
    inline void SetColumnTypesActive(bool x)
    {
        m_HasRowTypes = x;
        if(x)
        {
            m_HasColumnTypes = false;
        }
    }
    inline void SetRowTitle(int i, QString s)
    {
        if(m_RowTitles.length() < GetNumberOfRows())
        {
            for(int i = m_RowTitles.length(); i < GetNumberOfRows(); i++)
            {
                m_RowTitles.append("");
            }

        }
        if(i < m_RowTitles.length())
        {
            m_RowTitles.replace(i,s.toStdString());
        }
    }
    inline void SetColumnTitle(int i,QString s)
    {
        if(m_ColumnTitles.length() < GetNumberOfCols())
        {
            for(int i = m_ColumnTitles.length(); i < GetNumberOfCols(); i++)
            {
                m_ColumnTitles.append("");
            }

        }
        if(i < m_ColumnTitles.length())
        {
            m_ColumnTitles.replace(i,s.toStdString());
        }
    }
    inline void SetRowTitle(int i, std::string s)
    {
        if(m_RowTitles.length() < GetNumberOfRows())
        {
            for(int i = m_RowTitles.length(); i < GetNumberOfRows(); i++)
            {
                m_RowTitles.append("");
            }

        }
        if(i < m_RowTitles.length())
        {
            m_RowTitles.replace(i,s);
        }
    }
    inline void SetColumnTitle(int i,std::string s)
    {
        if(m_ColumnTitles.length() < GetNumberOfCols()&& i > -1)
        {
            for(int i = m_ColumnTitles.length(); i < GetNumberOfCols(); i++)
            {
                m_ColumnTitles.append("");
            }

        }
        if(i < m_ColumnTitles.length()&& i > -1)
        {
            m_ColumnTitles.replace(i,s);
        }
    }

    inline QString GetColumnTitle(int i)
    {
        if(i < m_ColumnTitles.length() && i > -1)
        {
            return QString(m_ColumnTitles.at(i).c_str());
        }else
        {
            return "";
        }
    }

    inline QString GetRowTitle(int i)
    {
        if(i < m_RowTitles.length() && i > -1)
        {
            return QString(m_RowTitles.at(i).c_str());
        }else
        {
            return "";
        }
    }

    inline QString GetColumnTitleQ(int i)
    {
        if(i < m_ColumnTitles.length()&& i > -1)
        {
            return QString(m_ColumnTitles.at(i).c_str());
        }else
        {
            return "";
        }
    }

    inline QString GetRowTitleQ(int i)
    {
        if(i < m_RowTitles.length())
        {
            return QString(m_RowTitles.at(i).c_str());
        }else
        {
            return "";
        }
    }

    inline void SetRowType(int i,int t)
    {
        if(m_RowTypes.length() < GetNumberOfRows())
        {
            for(int i = m_RowTypes.length(); i < GetNumberOfRows(); i++)
            {
                m_RowTypes.append(TABLE_TYPE_UNKNOWN);
            }
        }
        if(i < m_RowTypes.length())
        {
            m_RowTypes.replace(i,t);
        }
    }
    inline void SetColumnType(int i,int t)
    {
        if(m_ColumnTypes.length() < GetNumberOfCols()&& i > -1)
        {
            for(int i = m_ColumnTypes.length(); i < GetNumberOfCols(); i++)
            {
                m_ColumnTypes.append(TABLE_TYPE_UNKNOWN);
            }
        }
        if(i < m_ColumnTypes.length()&& i > -1)
        {
            m_ColumnTypes.replace(i,t);
        }
    }

    inline int GetRowType(int i)
    {

        if(i < m_RowTypes.length()&& i > -1)
        {
            return m_RowTypes.at(i);
        }else
        {
            return TABLE_TYPE_UNKNOWN;
        }
    }

    inline int GetColumnType(int i)
    {
        if(i < m_ColumnTypes.length()&& i > -1)
        {
            return m_ColumnTypes.at(i);
        }else
        {
            return TABLE_TYPE_UNKNOWN;
        }
    }


    inline QString GetRowTypeName(int i)
    {
        int type = GetRowType(i);
        return GetDataTypeName(type);
    }

    inline QString  GetColumnTypeName(int i)
    {
        int type = GetColumnType(i);
        return GetDataTypeName(type);
    }

    inline bool IsCalcInt()
    {
        if(m_HasColumnTypes && m_ColumnTypes.length() > 0)
        {
            if(m_ColumnTypes.at(0) == TABLE_TYPE_BOOL || m_ColumnTypes.at(0) == TABLE_TYPE_INT32 || m_ColumnTypes.at(0) == TABLE_TYPE_INT64)
            {
                return true;
            }
        }
        return false;
    }

    inline bool IsCalcDouble()
    {
        if(m_HasColumnTypes && m_ColumnTypes.length() > 0)
        {
            if(m_ColumnTypes.at(0) == TABLE_TYPE_FLOAT64)
            {
                return true;
            }
        }
        return false;
    }

    inline bool IsCalcString()
    {
        if(m_HasColumnTypes && m_ColumnTypes.length() > 0)
        {
            if(m_ColumnTypes.at(0) == TABLE_TYPE_BOOL || m_ColumnTypes.at(0) == TABLE_TYPE_INT32 || m_ColumnTypes.at(0) == TABLE_TYPE_INT64 || m_ColumnTypes.at(0) == TABLE_TYPE_FLOAT64)
            {
                return false;
            }
        }

        return true;
    }

    inline bool IsColumnCalcInt(int column)
    {
        if(m_HasColumnTypes && m_ColumnTypes.length() > 0)
        {
            if(m_ColumnTypes.at(column) == TABLE_TYPE_BOOL || m_ColumnTypes.at(column) == TABLE_TYPE_INT32 || m_ColumnTypes.at(column) == TABLE_TYPE_INT64)
            {
                return true;
            }
        }
        return false;
    }

    inline bool IsColumnCalcDouble(int column)
    {
        if(m_HasColumnTypes && m_ColumnTypes.length() > 0)
        {
            if(m_ColumnTypes.at(column) == TABLE_TYPE_FLOAT64)
            {
                return true;
            }
        }
        return false;
    }

    inline bool IsColumnCalcString(int column)
    {
        if(m_HasColumnTypes && m_ColumnTypes.length() > 0)
        {
            if(m_ColumnTypes.at(column) == TABLE_TYPE_BOOL || m_ColumnTypes.at(column) == TABLE_TYPE_INT32 || m_ColumnTypes.at(column) == TABLE_TYPE_INT64 || m_ColumnTypes.at(column) == TABLE_TYPE_FLOAT64)
            {
                return false;
            }
        }

        return true;
    }

    inline MatrixTable * GetColumn(int column)
    {
        MatrixTable * t = new MatrixTable();
        int rows = GetNumberOfRows();
        t->SetSize(rows,1);

        if(column > -1 && column < GetNumberOfCols())
        {
            t->SetColumnType(0,this->GetColumnType(column));
            t->SetColumnTitle(0,this->GetColumnTitleQ(column));

            for(int r = 0; r < rows; r++)
            {
                t->SetValue(r,0,QString(*m_Data.at(r)->at(column)));
            }
        }


        return t;

    }

    inline QList<QString> GetColumnString(int column)
    {
        int rows = GetNumberOfRows();

        QList<QString> ret;
        if(column > -1 && column < GetNumberOfCols())
        {
            for(int r = 0; r < rows; r++)
            {
                ret.append( QString(*m_Data.at(r)->at(column)));
            }
        }

        return ret;

    }

    inline QList<QString*> GetColumnStringRef(int column)
    {
        int rows = GetNumberOfRows();

        QList<QString*> ret;
        if(column > -1 && column < GetNumberOfCols())
        {
            for(int r = 0; r < rows; r++)
            {
                ret.append( m_Data.at(r)->at(column));
            }
        }

        return ret;
    }

    //replace inner section
    inline void ReplaceValues(MatrixTable * t, int rs, int cs)
    {


        int rows_rp = t->GetNumberOfRows();
        int cols_rp = t->GetNumberOfCols();
        int rows = GetNumberOfRows();
        int cols = GetNumberOfCols();
        for(int r = rs; r < std::min(rows,rs + rows_rp); r++)
        {
            for(int c = cs; c < std::min(cols,cs + cols_rp); c++)
            {
                SetValue(r,c,t->GetValueStringQ(r - rs,c- cs));
            }
        }

    }

    inline void ReplaceColumn(int column, MatrixTable * t)
    {

        column = std::max(0,column);

        if(column > -1 && column < GetNumberOfCols())
        {

            for(int i = 0; i < t->GetNumberOfCols()-1;i++)
            {
                t->AddColumn(column);
            }

            if( t->GetNumberOfRows() > GetNumberOfRows())
            {
                SetSize(t->GetNumberOfRows(),GetNumberOfCols());
            }

            for(int r = 0; r < t->GetNumberOfRows(); r++)
            {
                for(int c = 0; c < t->GetNumberOfRows(); c++)
                {
                    SetValue(r,column + c,t->GetValueStringQ(r,c));
                }
            }

        }else if(column > GetNumberOfCols()-1)
        {
            this->AppendColumns(t);
        }
    }

    //replace functions
    inline void ReplaceColumn(int column, QList<int> &new_values)
    {
        int rows = GetNumberOfRows();

        if(column > -1 && column < GetNumberOfCols())
        {
            for(int r = 0; r < std::min(new_values.length(),rows); r++)
            {
                 *(m_Data.at(r)->at(column)) = QString::number(new_values.at(rows));
            }
        }
    }

    inline void ReplaceColumn(int column, QList<double> &new_values)
    {
        int rows = GetNumberOfRows();

        if(column > -1 && column < GetNumberOfCols())
        {
            for(int r = 0; r < std::min(new_values.length(),rows); r++)
            {
                 *(m_Data.at(r)->at(column)) = QString::number(new_values.at(rows));
            }
        }
    }

    inline void ReplaceColumn(int column, QList<QString> &new_values)
    {
        int rows = GetNumberOfRows();

        if(column > -1 && column < GetNumberOfCols())
        {
            for(int r = 0; r < std::min(new_values.length(),rows); r++)
            {
                 *(m_Data.at(r)->at(column)) = new_values.at(rows);
            }
        }
    }

    //replace functions used by sorting helpers
    inline void ReplaceColumn(int column, QList<QPair<int,int>> &new_values)
    {
        int rows = GetNumberOfRows();

        if(column > -1 && column < GetNumberOfCols())
        {
            for(int r = 0; r < std::min(new_values.length(),rows); r++)
            {
                 *(m_Data.at(r)->at(column)) = QString::number(new_values.at(rows).first);
            }
        }
    }

    inline void ReplaceColumn(int column, QList<QPair<double,int>> &new_values)
    {
        int rows = GetNumberOfRows();

        if(column > -1 && column < GetNumberOfCols())
        {
            for(int r = 0; r < std::min(new_values.length(),rows); r++)
            {
                 *(m_Data.at(r)->at(column)) = QString::number(new_values.at(rows).first);
            }
        }
    }

    inline void ReplaceColumn(int column, QList<QPair<QString,int>> &new_values)
    {
        int rows = GetNumberOfRows();

        if(column > -1 && column < GetNumberOfCols())
        {
            for(int r = 0; r < std::min(new_values.length(),rows); r++)
            {
                 *(m_Data.at(r)->at(column)) = new_values.at(rows).first;
            }
        }

    }

    //re-ordering the data of columns
    inline void ReOrderColumn(int column, QList<int> &new_indices)
    {
        QList<QString> data_old = GetColumnString(column);
        QList<QString> data_new;

        int rows = this->GetNumberOfRows();
        int cols = this->GetNumberOfCols();

        for(int r = 0; r < rows;r ++)
        {
            int row_old = new_indices.at(r);
            data_new.append(data_old.at(row_old));
        }


        ReplaceColumn(column,data_new);


    }

    template<typename T>
    inline void ReOrderColumn(int column, QList<QPair<T,int>> &new_indices)
    {
        QList<QString> data_old = GetColumnString(column);
        QList<QString> data_new;

        int rows = this->GetNumberOfRows();
        int cols = this->GetNumberOfCols();

        for(int r = 0; r < rows;r ++)
        {
            int row_old = new_indices.at(r).second;
            data_new.append(data_old.at(row_old));
        }

        ReplaceColumn(column,data_new);
    }


    inline MatrixTable * ReOrderAllColumns(QList<int> &new_indices)
    {
        MatrixTable * t = new MatrixTable();
        t->SetSize(this->GetNumberOfRows(),this->GetNumberOfCols());
        t->m_HasColumnTypes = m_HasColumnTypes;
        t->m_ColumnTypes = m_ColumnTypes;
        t->m_ColumnTitles = m_ColumnTitles;

        int rows = this->GetNumberOfRows();
        int cols = this->GetNumberOfCols();

        for(int r = 0; r < rows;r ++)
        {
            int row_old = new_indices.at(r);
            for(int c = 0; c < cols;c ++)
            {
                *(t->m_Data.at(r)->at(c)) = *(m_Data.at(row_old)->at(c));
            }
        }

        return t;

    }

    template<typename T>
    inline MatrixTable * ReOrderAllColumns(QList<QPair<T,int>> &new_indices)
    {
        MatrixTable * t = new MatrixTable();
        t->SetSize(this->GetNumberOfRows(),this->GetNumberOfCols());
        t->m_HasColumnTypes = m_HasColumnTypes;
        t->m_ColumnTypes = m_ColumnTypes;
        t->m_ColumnTitles = m_ColumnTitles;

        int rows = this->GetNumberOfRows();
        int cols = this->GetNumberOfCols();

        for(int r = 0; r < rows;r ++)
        {
            int row_old = new_indices.at(r).second;

            for(int c = 0; c < cols;c ++)
            {
                *(t->m_Data.at(r)->at(c)) = *(m_Data.at(row_old)->at(c));
            }
        }

        return t;
    }


    inline void SortColumn(int column, bool descending = false)
    {
        MatrixTable * in = this;
        if(column > -1 && in->GetNumberOfCols() > column)
        {
            if(in->IsColumnCalcInt(column))
            {
                //compare as ints
                QList<QPair<int,int> > array;
                int rmax = in->GetNumberOfRows();
                for (int r = 0; r < rmax; r++)
                {
                    int x = in->GetValueInt(r,column);
                    array.append(qMakePair(x,r));
                }

                // Ordering ascending
                if(!descending)
                {
                    std::sort(array.begin(), array.end(), QPairFirstComparer());
                }else
                {
                    std::sort(array.begin(), array.end(), QPairFirstComparerDesc());
                }

                ReplaceColumn(column,array);

            }else if(in->IsColumnCalcDouble(column))
            {
                //compare as ints
                QList<QPair<double,int> > array;
                int rmax = in->GetNumberOfRows();
                for (int r = 0; r < rmax; r++)
                {
                    double x = in->GetValueDouble(r,column);
                    array.append(qMakePair(x,r));
                }

                // Ordering ascending
                if(!descending)
                {
                    std::sort(array.begin(), array.end(), QPairFirstComparer());
                }else
                {
                    std::sort(array.begin(), array.end(), QPairFirstComparerDesc());
                }

                ReplaceColumn(column,array);

            }else if(in->IsColumnCalcString(column))
            {
                //compare as ints
                QList<QPair<QString,int> > array;
                int rmax = in->GetNumberOfRows();
                for (int r = 0; r < rmax; r++)
                {
                    QString x = in->GetValueStringQ(r,column);
                    array.append(qMakePair(x,r));
                }

                // Ordering ascending
                if(!descending)
                {
                    std::sort(array.begin(), array.end(), QPairFirstComparer());
                }else
                {
                    std::sort(array.begin(), array.end(), QPairFirstComparerDesc());
                }

                ReplaceColumn(column,array);
            }
        }
    }

    inline MatrixTable * SortByColumn(int column, bool descending = false)
    {
        MatrixTable * in = this;
        if(column > -1 && in->GetNumberOfCols() > column)
        {
            if(in->IsColumnCalcInt(column))
            {
                //compare as ints
                QList<QPair<int,int> > array;
                int rmax = in->GetNumberOfRows();
                for (int r = 0; r < rmax; r++)
                {
                    int x = in->GetValueInt(r,column);
                    array.append(qMakePair(x,r));
                }

                // Ordering ascending
                if(!descending)
                {
                    std::sort(array.begin(), array.end(), QPairFirstComparer());
                }else
                {
                    std::sort(array.begin(), array.end(), QPairFirstComparerDesc());
                }

                return this->ReOrderAllColumns(array);

            }else if(in->IsColumnCalcDouble(column))
            {
                //compare as ints
                QList<QPair<double,int> > array;
                int rmax = in->GetNumberOfRows();
                for (int r = 0; r < rmax; r++)
                {
                    double x = in->GetValueDouble(r,column);
                    array.append(qMakePair(x,r));
                }

                // Ordering ascending
                if(!descending)
                {
                    std::sort(array.begin(), array.end(), QPairFirstComparer());
                }else
                {
                    std::sort(array.begin(), array.end(), QPairFirstComparerDesc());
                }

                return this->ReOrderAllColumns(array);

            }else if(in->IsColumnCalcString(column))
            {
                //compare as ints
                QList<QPair<QString,int> > array;
                int rmax = in->GetNumberOfRows();
                for (int r = 0; r < rmax; r++)
                {
                    QString x = in->GetValueStringQ(r,column);
                    array.append(qMakePair(x,r));
                }

                // Ordering ascending
                if(!descending)
                {
                    std::sort(array.begin(), array.end(), QPairFirstComparer());
                }else
                {
                    std::sort(array.begin(), array.end(), QPairFirstComparerDesc());
                }

                return this->ReOrderAllColumns(array);
            }
        }
        return this->Copy();
    }

    inline MatrixTable * SortByColumn(QString name)
    {

        return nullptr;
    }

    inline void SortColumn(QString name)
    {

    }

    inline void fill()
    {
        int wmax = 0;
        for(int i = 0; i < m_Data.length(); i++)
        {
            wmax= std::max(wmax,m_Data.at(i)->length());
        }
        for(int i = 0; i < m_Data.length(); i++)
        {
            for(int j = m_Data.at(i)->length(); j < wmax; j++)
            {
                m_Data.at(i)->append(new QString(""));
            }
        }

    }



    inline void AddItem(QString s)
    {
        QVector<QString*> * v = new QVector<QString*>();
        v->append(new QString(s));
        int rows = GetNumberOfRows();
        int cols = GetNumberOfCols();
        if(rows > 0 && cols > 0)
        {
            for(int j = v->length()-1; j < cols ; j++)
            {
                v->append(new QString(""));
            }
        }
        m_Data.append(v);

        //

    }

    inline void AddItem(std::string s)
    {
        AddItem(QString(s.c_str()));
    }
    inline void SetSize(int r)
    {
         SetSize(r,1);
    }

    inline QString ColumnTitle(int i)
    {
        return GetColumnTitleQ(i);
    }

    inline int GetSizeR()
    {
         return GetNumberOfRows();
    }
    inline int GetSizeC()
    {
         return GetNumberOfCols();
    }

    inline void SetSize(int r, int c)
    {
        //set the table data to be the correct size
        if(m_Data.length() > r)
        {
            for(int i = m_Data.length()-1; i > r -1; i--)
            {
                int l = m_Data.at(i)->length();
                for(int j = 0; j < l; j++)
                {
                    delete m_Data.at(i)->at(j);
                }
                delete m_Data.at(i);
                m_Data.removeAt(i);
            }
        }else if (m_Data.length() < r)
        {
            for(int i = m_Data.length(); i < r; i++)
            {
                QVector<QString*> * row = new QVector<QString*>();
                m_Data.push_back(row);
            }
        }

        //remove items if a row is too long, or add if too short
        for(int i = 0; i < r; i++)
        {
            int l = m_Data.at(i)->length();
            QVector<QString*> * row = m_Data.at(i);
            if(l > c)
            {
                for(int j = l-1; j > c -1; j--)
                {
                    delete row->at(j);
                    row->removeAt(j);
                }
            }else if (l < c)
            {
                for(int j = l; j < c; j++)
                {
                    row->push_back(new QString(""));
                }
            }
        }

        //now change the size of the titles and data type arrays
        m_RowTypes.resize(GetNumberOfRows());
        m_RowTitles.resize(GetNumberOfRows());
        m_ColumnTypes.resize(GetNumberOfCols());
        m_ColumnTitles.resize(GetNumberOfCols());

    }
    inline void AddRow(int i)
    {
        //now change the size of the titles and data type arrays

        m_ColumnTypes.resize(GetNumberOfCols()+1);
        m_ColumnTitles.resize(GetNumberOfCols()+1);
        if(i > GetNumberOfCols())
        {
            m_RowTypes.resize(GetNumberOfRows()+1);
            m_RowTitles.resize(GetNumberOfRows()+1);
        }else
        {
            m_ColumnTypes.insert(i,TABLE_TYPE_FLOAT64);
            m_ColumnTitles.insert(i,"");
        }


        int cols = GetNumberOfCols();
        QVector<QString*> * row = new QVector<QString*>();
        for(int j = 0; j < cols; j++)
        {
            row->append(new QString(""));
        }
        m_Data.insert(i,row);


    }
    inline void AddRow()
    {
        int cols = GetNumberOfCols();
        QVector<QString*> * row = new QVector<QString*>();
        for(int j = 0; j < cols; j++)
        {
            row->append(new QString(""));
        }
        m_Data.append(row);

        //now change the size of the titles and data type arrays
        m_RowTypes.resize(GetNumberOfRows());
        m_RowTitles.resize(GetNumberOfRows());
        m_ColumnTypes.resize(GetNumberOfCols());
        m_ColumnTitles.resize(GetNumberOfCols());
    }

    inline void AddColumn()
    {
        for(int i = 0; i < m_Data.length(); i++)
        {
            m_Data.at(i)->append(new QString(""));
        }

        //now change the size of the titles and data type arrays
        m_RowTypes.resize(GetNumberOfRows());
        m_RowTitles.resize(GetNumberOfRows());
        m_ColumnTypes.resize(GetNumberOfCols());
        m_ColumnTitles.resize(GetNumberOfCols());

    }
    inline void AddColumn(int index )
    {

        //now change the size of the titles and data type arrays
        m_RowTypes.resize(GetNumberOfRows());
        m_RowTitles.resize(GetNumberOfRows());
        if(index > GetNumberOfCols())
        {
            m_ColumnTypes.resize(GetNumberOfCols()+1);
            m_ColumnTitles.resize(GetNumberOfCols()+1);
        }else
        {
            m_ColumnTypes.insert(index,TABLE_TYPE_FLOAT64);
            m_ColumnTitles.insert(index,"");
        }

        for(int i = 0; i < m_Data.length(); i++)
        {
            if(index > m_Data.at(i)->length())
            {
                m_Data.at(i)->insert(m_Data.at(i)->length(),new QString(""));
            }else {
                m_Data.at(i)->insert(index,new QString(""));
            }
        }


    }

    inline MatrixTable * GetCopyOfColumns(QList<int> columns)
    {
        MatrixTable * ret = new MatrixTable();

        if(columns.length() == 0)
        {
            return ret;
        }else
        {
            int rows = this->GetNumberOfRows();
            int cols =columns.length();

            ret->SetSize(rows,cols);


            for(int c = 0; c < cols; c++)
            {
                ret->m_ColumnTypes.append(GetColumnType(columns.at(c)));
                ret->m_ColumnTitles.append(GetColumnTitleQ(columns.at(c)).toStdString());

                for(int r = 0; r < rows; r++)
                {
                    ret->SetValue(r,c,GetValueStringQ(r,columns.at(c)));
                }
            }
            ret->m_HasColumnTypes = this->m_HasColumnTypes;
            ret->m_RowTitles = this->m_RowTitles;
            ret->m_RowTypes = this->m_RowTypes;

            return ret;
        }

    }


    inline void AppendColumns(MatrixTable * t)
    {
        if(this->GetNumberOfCols() == 0)
        {
            CopyFrom(t);
        }else
        {
            int rows = t->GetNumberOfRows();
            int thisrows = this->GetNumberOfRows();
            if(rows > thisrows)
            {
                for(int i = thisrows; i < rows; i++)
                {
                    this->AddRow();
                }
            }

            for(int i = 0; i < m_Data.length(); i++)
            {
                for(int k = 0; k < t->GetNumberOfCols(); k++)
                {
                    m_Data.at(i)->append(new QString(t->GetValueStringQ(i,k)));
                }
            }

            //now change the size of the titles and data type arrays
            m_RowTypes.resize(GetNumberOfRows());
            m_RowTitles.resize(GetNumberOfRows());
            m_ColumnTypes.resize(GetNumberOfCols());
            m_ColumnTitles.resize(GetNumberOfCols());
        }


    }

    inline void SetValue(int r, QString value)
    {
        SetValue(r, 0,value);
    }

    inline void SetValue(int r, int c, std::string value)
    {
        if(r > -1 && r < m_Data.length())
        {
            if(c > -1 && c < m_Data.at(r)->length())
            {
                m_Data.at(r)->at(c)->clear();
                *m_Data.at(r)->at(c) = QString(value.c_str());

            }
        }
    }
    inline void  SetValue(int r, int c, int value)
    {
        if(r > -1 && r < m_Data.length())
        {
            if(c > -1 && c < m_Data.at(r)->length())
            {
                *m_Data.at(r)->at(c) = QString::number(value);
            }
        }
    }

    inline void  SetValue(int r, int c, double value)
    {
        if(r > -1 && r < m_Data.length())
        {
            if(c > -1 && c < m_Data.at(r)->length())
            {
                *m_Data.at(r)->at(c) = QString::number(value);
            }
        }
    }
    inline void  SetValue(int r, int c, QString value)
    {
        if(r > -1 && r < m_Data.length())
        {
            if(c > -1 && c < m_Data.at(r)->length())
            {
                *m_Data.at(r)->at(c) = QString(value);
            }
        }
    }
    inline int GetNumberOfCols()
    {
        int wmax = 0;
        for(int i = 0; i < m_Data.length(); i++)
        {
            wmax= std::max(wmax,m_Data.at(i)->length());
        }
        return wmax;
    }

    inline int GetNumberOfRows()
    {
        return m_Data.length();
    }

    inline int GetNumberOfRowsAtColumn(int c)
    {
        if(c < 0 || m_Data.length() == 0 )
        {
            return 0;
        }
        int r;
        for(r = m_Data.length()-1; r > -1;r--)
        {
            if(!(m_Data.at(r)->length() > c))
            {
                break;
            }
            if(!(m_Data.at(r)->at(c)->isEmpty()))
            {
                break;
            }
        }
        return r + 1;
    }

    inline std::string *GetValueString(int r, int c)
    {
        if(r > -1 && r < m_Data.length())
        {
            if(c > -1 && c < m_Data.at(r)->length())
            {
                return new std::string(m_Data.at(r)->at(c)->toStdString());
            }
        }

        return nullptr;
    }

    inline QString *GetValueStringQRef(int r, int c) const
    {
        if(r > -1 && r < m_Data.length())
        {
            if(c > -1 && c < m_Data.at(r)->length())
            {
                return m_Data.at(r)->at(c);
            }
        }

        return new QString("");
    }


    inline QString GetValueStringQ(int r, int c) const
    {
        if(r > -1 && r < m_Data.length())
        {
            if(c > -1 && c < m_Data.at(r)->length())
            {
                return QString(*m_Data.at(r)->at(c));
            }
        }

        return "";
    }

    inline double GetValueDouble(int r, int c) const
    {
        if(r > -1 && r < m_Data.length())
        {
            if(c > -1 && c < m_Data.at(r)->length())
            {
                bool ok = true;
                double val =  QString(*m_Data.at(r)->at(c)).toDouble(&ok);
                if(ok)
                {
                    return val;
                }else
                {
                    return 0.0;
                }
            }
        }

        return 0.0;
    }

    inline int GetValueInt(int r, int c)
    {
        if(r > -1 && r < m_Data.length())
        {
            if(c > -1 && c < m_Data.at(r)->length())
            {
                bool ok = true;
                double val =  QString(*m_Data.at(r)->at(c)).toInt(&ok);
                if(ok)
                {
                    return val;
                }else
                {
                    return 0;
                }
            }
        }

        return 0;
    }

    inline void RemoveRow(int i)
    {
        if(i > -1 && i < m_Data.length())
        {

            m_RowTypes.removeAt(i);
            m_RowTitles.removeAt(i);

            for(int j =0; j < m_Data.at(i)->length(); j++)
            {
                delete m_Data.at(i)->at(j);
            }
            m_Data.at(i)->clear();
            delete m_Data.at(i);
            m_Data.removeAt(i);
        }
    }
    inline void RemoveColumn(int i)
    {
        if(m_Data.length() > 0)
        {
            if(i > -1 && i < m_Data.at(0)->length())
            {
                m_ColumnTypes.removeAt(i);
                m_ColumnTitles.removeAt(i);

                for(int j =0; j < m_Data.length(); j++)
                {
                    if(i < m_Data.at(j)->length())
                    {
                        delete m_Data.at(j)->at(i);
                        m_Data.at(j)->removeAt(i);
                    }
                }
            }
        }
    }

    inline void Empty()
    {
        int count = 0;
        for(int i =0; i < m_Data.length(); i++)
        {
            if(m_Data.at(i) != nullptr)
            {
                for(int j =0; j < m_Data.at(i)->length(); j++)
                {
                    if(m_Data.at(i)->at(j) != nullptr)
                    {
                        count ++;
                        delete m_Data.at(i)->at(j);
                    }
                }
                m_Data.at(i)->clear();
                delete m_Data.at(i);
            }
        }
        m_Data.clear();
        m_RowTitles.clear();
        m_ColumnTitles.clear();
        m_RowTypes.clear();
        m_ColumnTypes.clear();
        m_HasRowTypes = false;
        m_HasColumnTypes =false;
    }

    inline void CopyFromData(QVector<QVector<QString*>*> const &data)
    {
        Empty();

        int count  = 0;
        for(int i =0; i < data.length(); i++)
        {
            QVector<QString*> * row = new QVector<QString*>();
            for(int j = 0; j < data.at(i)->length(); j++)
            {
                count++;
                QString * s = new QString();
                *(s) = *(data.at(i)->at(j));
                row->push_back(s);
            }
            m_Data.push_back(row);
        }


    }
    inline MatrixTable * Copy()
    {
        MatrixTable * ret  = new MatrixTable();
        ret->CopyFrom(this);
        return ret;
    }

    inline void CopyFrom(MatrixTable const * t)
    {
        CopyFromData(t->m_Data);
        m_HasColumnTypes = t->m_HasColumnTypes;
        m_HasRowTypes = t->m_HasRowTypes;
        m_ColumnTypes = t->m_ColumnTypes;
        m_RowTypes = t->m_RowTypes;
        m_ColumnTitles = t->m_ColumnTitles;
        m_RowTitles = t->m_RowTitles;

    }

    inline bool ConvertToInt(bool set  = false)
    {
        bool allok = true;
        for(int i =0; i < m_Data.length();i++)
        {
            for(int j =0; j < m_Data.at(i)->length(); j++)
            {
                if(m_Data.at(i)->at(j) != nullptr)
                {
                    QString text = *(m_Data.at(i)->at(j));
                    if(!text.trimmed().isEmpty())
                    {
                        bool ok = true;
                        int n = text.toInt(&ok);
                        allok = allok & ok;
                        if(!ok)
                        {
                            n = 0;
                            *(m_Data.at(i)->at(j)) = QString::number(n);
                        }
                    }else {
                        *(m_Data.at(i)->at(j)) = QString::number(0);
                    }



                }else {
                    m_Data.at(i)->replace(j,new QString("0"));
                }
            }
        }

        if(set)
        {
            if(!m_HasColumnTypes || m_HasRowTypes)
            {
                m_HasColumnTypes = true;
                m_HasRowTypes = false;
            }
            for(int i = 0; i < m_ColumnTypes.length() ; i++)
            {

                m_ColumnTypes.replace(i,TABLE_TYPE_INT64);
            }
            for(int i = 0; i < m_RowTypes.length() ; i++)
            {
                m_RowTypes.replace(i,TABLE_TYPE_INT64);
            }
        }
        return allok;
    }

    inline bool ConvertToString(bool set = false)
    {
        bool allok = true;
        for(int i =0; i < m_Data.length();i++)
        {
            for(int j =0; j < m_Data.at(i)->length(); j++)
            {
                if(m_Data.at(i)->at(j) != nullptr)
                {

                }else {
                    m_Data.at(i)->replace(j,new QString(""));
                }
            }
        }

        if(set)
        {
            for(int i = 0; i < m_ColumnTypes.length() ; i++)
            {
                m_ColumnTypes.replace(i,TABLE_TYPE_STRING);
            }
            for(int i = 0; i < m_RowTypes.length() ; i++)
            {
                m_RowTypes.replace(i,TABLE_TYPE_STRING);
            }

            if(!m_HasColumnTypes || m_HasRowTypes)
            {
                m_HasColumnTypes = true;
                m_HasRowTypes = false;
            }
        }
        return allok;
    }

    inline bool ConvertToDouble(bool set = false)
    {
        bool allok = true;
        for(int i =0; i < m_Data.length();i++)
        {
            for(int j =0; j < m_Data.at(i)->length(); j++)
            {
                if(m_Data.at(i)->at(j) != nullptr)
                {
                    QString text = *(m_Data.at(i)->at(j));
                    if(!text.trimmed().isEmpty())
                    {
                        bool ok = true;
                        double n = text.toDouble(&ok);
                        allok = allok & ok;
                        if(!ok)
                        {
                            n= 0.0f;
                            *(m_Data.at(i)->at(j)) = QString::number(n);
                        }
                    }else {
                        *(m_Data.at(i)->at(j)) = QString::number(0.0);
                    }
                }else {
                    m_Data.at(i)->replace(j,new QString("0.0"));
                }
            }
        }
        if(set)
        {
            for(int i = 0; i < m_ColumnTypes.length() ; i++)
            {
                m_ColumnTypes.replace(i,TABLE_TYPE_FLOAT64);
            }
            for(int i = 0; i < m_RowTypes.length() ; i++)
            {
                m_RowTypes.replace(i,TABLE_TYPE_FLOAT64);
            }

            if(!m_HasColumnTypes || m_HasRowTypes)
            {
                m_HasColumnTypes = true;
                m_HasRowTypes = false;
            }
        }
        return allok;
    }

    inline QList<QString> GetVerticalStringList(int c,int rmin, int rmax)
    {
        QList<QString> list;

        for(int r = rmin; r < rmax +1; r++)
        {
            list.append(this->GetValueStringQ(c,r));
        }

        return list;
    }

    inline void Invert()
    {

        QVector<QVector<QString*>*> DataN;

        QVector<std::string> m_ColumnTitlesN = m_RowTitles;
        QVector<std::string> m_RowTitlesN = m_ColumnTitles;

        QVector<int> m_ColumnTypesN = m_RowTypes;
        QVector<int> m_RowTypesN = m_ColumnTypes;

        int cols = GetNumberOfCols();
        int rows = GetNumberOfRows();

        //create empty first, but switch number of rows and cols
        for(int i = 0; i < cols; i++)
        {
            QVector<QString*>* datarow = new QVector<QString*>();
            for(int j = 0; j < rows; j++)
            {
                datarow->append(new QString(""));
            }
            DataN.append(datarow);
        }

        //fill data
        for(int i = 0; i < m_Data.length(); i++)
        {
            for(int j = 0; j < m_Data.at(i)->length(); j++)
            {
                (*DataN.at(j)->at(i)) = (*m_Data.at(i)->at(j));
            }
        }

        this->Empty();

        m_Data = DataN;

        m_ColumnTitles = m_ColumnTitlesN;
        m_RowTitles = m_RowTitlesN ;

        m_ColumnTypes = m_ColumnTypesN;
        m_RowTypes = m_RowTypesN ;


    }

    //Angelscript related functions
    public:

        std::function<void(QString,int,QString)> SetAttributeFunction;
        bool        DoSetAttribute = false;
        QString     DoSetAttributeName = "";

        bool          HasParent= false;
        MatrixTable * Parent = nullptr;
        MatrixTableParent * MTParent = nullptr;
        int           Parent_sr = 0;
        int           Parent_sc = 0;
        QString       Parent_scname = "";
        inline void   UpdateParent();

        QString        FileName          = "";
        bool           writeonassign     = false;
        std::function<void(MatrixTable *,QString)> writefunc;
        std::function<MatrixTable *(QString)> readfunc;
        bool           m_Created           = false;
        int            m_refcount          = 1;
        void           AddRef            ();
        void           ReleaseRef        ();
        MatrixTable*   Assign            (MatrixTable*);
        MatrixTable*   OpIndex           (int);
        MatrixTable*   OpIndex           (QString);
        QString*       OpIndex           (int,int);
        void OpIndexSet                  (int,int,QString);
        void OpIndexSet                  (int,MatrixTable*);
        MatrixTable*        OpAdd             (MatrixTable*other);
        MatrixTable*        OpMul             (MatrixTable*other);

        MatrixTable*        OpNeg             ();
        MatrixTable*        OpCom             ();
        MatrixTable*        OpSub             (MatrixTable*other);
        MatrixTable*        OpDiv             (MatrixTable*other);
        MatrixTable*        OpPow             (MatrixTable*other);
        MatrixTable*        OpAddAssign       (MatrixTable*other);
        MatrixTable*        OpSubAssign       (MatrixTable*other);
        MatrixTable*        OpMulAssign       (MatrixTable*other);
        MatrixTable*        OpDivAssign       (MatrixTable*other);
        MatrixTable*        OpPowAssign       (MatrixTable*other);

        MatrixTable*        OpModAssign       (MatrixTable*other);
        MatrixTable*        OpMod             (MatrixTable*other);

        MatrixTable*        EqualTo           (MatrixTable*other);
        MatrixTable*        LargerThen        (MatrixTable*other);
        MatrixTable*        SmallerThen       (MatrixTable*other);
        MatrixTable*        NotEqualTo        (MatrixTable*other);
        MatrixTable*        LargerEqualThen   (MatrixTable*other);
        MatrixTable*        SmallerEqualThen  (MatrixTable*other);

        MatrixTable*        And               (MatrixTable*other);
        MatrixTable*        Or                (MatrixTable*other);
        MatrixTable*        Xor               (MatrixTable*other);

        MatrixTable*        AndAssign         (MatrixTable*other);
        MatrixTable*        OrAssign          (MatrixTable*other);
        MatrixTable*        XorAssign         (MatrixTable*other);

        MatrixTable*        Assign            (double other);
        MatrixTable*        OpAdd             (double other);
        MatrixTable*        OpMul             (double other);

        MatrixTable*        OpSub             (double other);
        MatrixTable*        OpDiv             (double other);
        MatrixTable*        OpPow             (double other);
        MatrixTable*        OpAddAssign       (double other);
        MatrixTable*        OpSubAssign       (double other);
        MatrixTable*        OpMulAssign       (double other);
        MatrixTable*        OpDivAssign       (double other);
        MatrixTable*        OpPowAssign       (double other);

        MatrixTable*        OpModAssign       (double other);
        MatrixTable*        OpMod             (double other);

        MatrixTable*        EqualTo           (double other);
        MatrixTable*        LargerThen        (double other);
        MatrixTable*        SmallerThen       (double other);
        MatrixTable*        NotEqualTo        (double other);
        MatrixTable*        LargerEqualThen   (double other);
        MatrixTable*        SmallerEqualThen  (double other);

        MatrixTable*        And               (double other);
        MatrixTable*        Or                (double other);
        MatrixTable*        Xor               (double other);


        MatrixTable*        AndAssign         (double other);
        MatrixTable*        OrAssign          (double other);
        MatrixTable*        XorAssign         (double other);

        MatrixTable*        Assign            (int other);
        MatrixTable*        OpAdd             (int other);
        MatrixTable*        OpMul             (int other);

        MatrixTable*        OpSub             (int other);
        MatrixTable*        OpDiv             (int other);
        MatrixTable*        OpPow             (int other);
        MatrixTable*        OpAddAssign       (int other);
        MatrixTable*        OpSubAssign       (int other);
        MatrixTable*        OpMulAssign       (int other);
        MatrixTable*        OpDivAssign       (int other);
        MatrixTable*        OpPowAssign       (int other);

        MatrixTable*        OpModAssign       (int other);
        MatrixTable*        OpMod             (int other);

        MatrixTable*        EqualTo           (int other);
        MatrixTable*        LargerThen        (int other);
        MatrixTable*        SmallerThen       (int other);
        MatrixTable*        NotEqualTo        (int other);
        MatrixTable*        LargerEqualThen   (int other);
        MatrixTable*        SmallerEqualThen  (int other);

        MatrixTable*        And               (int other);
        MatrixTable*        Or                (int other);
        MatrixTable*        Xor               (int other);


        MatrixTable*        AndAssign         (int other);
        MatrixTable*        OrAssign          (int other);
        MatrixTable*        XorAssign         (int other);

        MatrixTable*        Assign            (QString s);
        MatrixTable*        OpAdd             (QString s);
        MatrixTable*        OpAddAssign       (QString s);
        MatrixTable*        EqualTo           (QString s);
        MatrixTable*        NotEqualTo        (QString s);

        MatrixTable*        OpAdd_r             (QString s);
        MatrixTable*        EqualTo_r           (QString s);
        MatrixTable*        NotEqualTo_r           (QString s);

        MatrixTable *        OpMod_r           (double other);
        MatrixTable *        OpAdd_r           (double other);
        MatrixTable *        OpMul_r           (double other);
        MatrixTable *        OpSub_r           (double other);
        MatrixTable *        OpDiv_r           (double other);
        MatrixTable *        OpPow_r           (double other);
        MatrixTable *        LargerThen_r      (double other);
        MatrixTable *        SmallerThen_r     (double other);
        MatrixTable *        NotEqualTo_r      (double other);
        MatrixTable *        LargerEqualThen_r (double other);
        MatrixTable *        SmallerEqualThen_r(double other);
        MatrixTable *        EqualTo_r         (double other);
        MatrixTable *        And_r             (double other);
        MatrixTable *        Or_r              (double other);
        MatrixTable *        Xor_r             (double other);

        MatrixTable *        OpMod_r           (int other);
        MatrixTable *        OpAdd_r           (int other);
        MatrixTable *        OpMul_r           (int other);
        MatrixTable *        OpSub_r           (int other);
        MatrixTable *        OpDiv_r           (int other);
        MatrixTable *        OpPow_r           (int other);
        MatrixTable *        LargerThen_r      (int other);
        MatrixTable *        SmallerThen_r     (int other);
        MatrixTable *        NotEqualTo_r      (int other);
        MatrixTable *        LargerEqualThen_r (int other);
        MatrixTable *        SmallerEqualThen_r(int other);
        MatrixTable *        EqualTo_r         (int other);
        MatrixTable *        And_r             (int other);
        MatrixTable *        Or_r              (int other);
        MatrixTable *        Xor_r             (int other);

};


class MatrixTableParent{

public:

    inline virtual void ReplaceValues(MatrixTable * t, QString name)
    {

    }

    inline virtual void UpdateParent()
    {

    }
};




//Angelscript related functionality

inline void MatrixTable::AddRef()
{
    m_refcount = m_refcount + 1;
}

inline void MatrixTable::ReleaseRef()
{
    m_refcount = m_refcount - 1;
    if(m_refcount == 0)
    {
        Empty();
        delete this;
    }
}



inline MatrixTable* MatrixTable::Assign(MatrixTable* sh)
{

    CopyFrom(sh);
    UpdateParent();

    return this;
}




inline MatrixTable* TableFactory()
{
    MatrixTable * ret = new MatrixTable();
    return ret;
}

inline MatrixTable *MatrixTable::OpIndex(int column)
{

    if(writeonassign)
    {
        MatrixTable*target = this;
        target = readfunc(FileName);
        this->CopyFrom(target);
    }

    MatrixTable * ret = GetColumn(column);
    ret->HasParent = true;
    ret->Parent = this;
    ret->Parent_sc = column;
    ret->Parent_sr = 0;
    return ret;
}

inline MatrixTable *MatrixTable::OpIndex(QString title)
{

    if(writeonassign)
    {
        MatrixTable*target = this;
        target = readfunc(FileName);
        this->CopyFrom(target);
    }

    int column = GetNumberOfCols();
    for(int i = 0; i < m_ColumnTitles.length(); i++)
    {
        if(QString((m_ColumnTitles.at(i).c_str())).compare(title) == 0)
        {
            column = i;
            break;
        }
    }

    MatrixTable * ret = GetColumn(column);
    ret->SetColumnTitle(0,title);
    ret->HasParent = true;
    ret->Parent = this;
    ret->Parent_sc = column;
    ret->Parent_sr = 0;
    return ret;
}

//return specific value at index (reference string so can be edited)
inline QString *MatrixTable::OpIndex(int r,int c)
{
    return GetValueStringQRef(r,c);
}

inline void MatrixTable::OpIndexSet(int r, int c,QString x )
{
    SetValue(r,c,x);
}

inline void MatrixTable::OpIndexSet(int c, MatrixTable * t)
{
    this->ReplaceColumn(c,t);
}
inline void MatrixTable::UpdateParent()
{

    if(writeonassign)
    {
        writefunc(this,FileName);
    }
    this->writeonassign = false;

    if(HasParent)
    {
        if(Parent != nullptr)
        {

            Parent->ReplaceValues(this,Parent_sr,Parent_sc);
            Parent->UpdateParent();
            HasParent = false;
            Parent = nullptr;
            Parent_sc = 0;
            Parent_sr = 0;
        }
        if(MTParent != nullptr)
        {
            MTParent->ReplaceValues(this,Parent_scname);
            MTParent->UpdateParent();
            HasParent = false;
            MTParent = nullptr;
            Parent_sc = 0;
            Parent_sr = 0;

        }
    }
}

inline static MatrixTable * TToInt(MatrixTable * in)
{
    if(in != nullptr)
    {
        MatrixTable * ret = in->Copy();
        ret->ConvertToInt(true);
        return ret;
    }else {

        return new MatrixTable();
    }

}

inline static MatrixTable * TToDouble(MatrixTable * in)
{

    if(in != nullptr)
    {
        MatrixTable * ret = in->Copy();
        ret->ConvertToDouble(true);
        return ret;
    }else {

        return new MatrixTable();
    }
    throw 1;
}

inline static MatrixTable * TToString(MatrixTable * in)
{
    if(in != nullptr)
    {
        MatrixTable * ret = in->Copy();
        ret->ConvertToString(true);
        return ret;
    }else {

        return new MatrixTable();
    }

}


inline static MatrixTable * TableGetColumn(MatrixTable * in, int column)
{
    MatrixTable * ret = new MatrixTable();
    ret->SetSize(in->GetNumberOfRows(),1);

    for(int i = 0; i < in->GetNumberOfRows();i++)
    {
        ret->SetValue(i,0,in->GetValueStringQ(i,column));
    }

    return ret;
}


inline static MatrixTable * DropColumn(MatrixTable * t, int i)
{
    MatrixTable * ret = t->Copy();
    ret->RemoveColumn(i);
    return ret;
}

inline static MatrixTable * DropColumn(MatrixTable * t, QString i)
{
    int index = t->GetColumnIndexFromTitle(i);
    if(index > -1)
    {
        return DropColumn(t,index);
    }else {
        return t->Copy();
    }
}

inline static MatrixTable * DropRow(MatrixTable * t, int i)
{
    MatrixTable * ret = t->Copy();
    ret->RemoveRow(i);
    return ret;
}

inline static MatrixTable * DropRow(MatrixTable * t, QString i)
{
    int index = t->GetRowIndexFromTitle(i);
    if(index > -1)
    {
        return DropRow(t,index);
    }else {
        return t->Copy();
    }
}

#include "qmetatype.h"

Q_DECLARE_METATYPE(MatrixTable*)


#endif // MATRIXTABLE_H
