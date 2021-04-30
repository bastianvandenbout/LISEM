#ifndef TABLEALGEBRA_H
#define TABLEALGEBRA_H

#include "iostream"
#include "matrixtable.h"


static inline MatrixTable * ApplyOperatorToMatrixTables(MatrixTable * M1, MatrixTable * M2, std::function<int(int,int)> fi,std::function<double(double,double)> ff,std::function<QString(QString,QString)> fs, bool write_to_one = false)
{

    if( M1 == nullptr ||  M2 == nullptr )
    {
        LISEM_ERROR("One of the tables provided to the operator is zero")
        throw 1;
        return new MatrixTable();
    }

    if(!(M1->GetNumberOfCols() == M2->GetNumberOfCols() && M1->GetNumberOfRows() == M2->GetNumberOfRows()))
    {
        LISEMS_ERROR("Could not do operation on tables of different size");
        throw 1;
        return new MatrixTable();
    }

    MatrixTable * Target;
    if(write_to_one)
    {
        Target = M1;
    }else {
        Target = M1->Copy();
    }

    //get size of table
    int rows = M1->GetNumberOfRows();
    int cols = M1->GetNumberOfCols();

    //now do the actual calculations
    for(int c = 0; c < cols; c++)
    {
        //get datatype for this column
        int type = M1->GetColumnType(c);

        if(type == TABLE_TYPE_UNKNOWN || type == TABLE_TYPE_STRING)
        {//as string

            #pragma omp parallel for
            for(int r = 0; r < rows; r++)
            {
                QString val1 = M1->GetValueStringQ(r,c);
                QString val2 = M2->GetValueStringQ(r,c);

                Target->SetValue(r,c,fs(val1,val2));
            }
        }else if(type == TABLE_TYPE_FLOAT64)
        {//as float
            #pragma omp parallel for
            for(int r = 0; r < rows; r++)
            {
                double val1 = M1->GetValueDouble(r,c);
                double val2 = M2->GetValueDouble(r,c);

                Target->SetValue(r,c,ff(val1,val2));
            }
        }else
        {//as int
            #pragma omp parallel for
            for(int r = 0; r < rows; r++)
            {

                int val1 = M1->GetValueInt(r,c);
                int val2 = M2->GetValueInt(r,c);

                Target->SetValue(r,c,fi(val1,val2));
            }
        }
    }

    return Target;
}


static inline MatrixTable * ApplyOperatorToMatrixTables(MatrixTable * M1, double val, std::function<int(int,int)> fi,std::function<double(double,double)> ff,std::function<QString(QString,QString)> fs, bool write_to_one = false)
{
    if( M1 == nullptr )
    {
        LISEM_ERROR("One of the tables provided to the operator is zero")
        throw 1;
        return new MatrixTable();
    }

    MatrixTable * Target;
    if(write_to_one)
    {
        Target = M1;
    }else {
        Target = M1->Copy();
    }

    //get size of table
    int rows = M1->GetNumberOfRows();
    int cols = M1->GetNumberOfCols();

    //now do the actual calculations
    for(int c = 0; c < cols; c++)
    {
        //get datatype for this column
        int type = M1->GetColumnType(c);

        if(type == TABLE_TYPE_UNKNOWN || type == TABLE_TYPE_STRING)
        {//as string

            #pragma omp parallel for
            for(int r = 0; r < rows; r++)
            {
                QString val1 = M1->GetValueStringQ(r,c);
                QString val2 = QString::number(val);

                Target->SetValue(r,c,fs(val1,val2));
            }
        }else if(type == TABLE_TYPE_FLOAT64)
        {//as float

            #pragma omp parallel for
            for(int r = 0; r < rows; r++)
            {
                double val1 = M1->GetValueDouble(r,c);
                double val2 = val;

                Target->SetValue(r,c,ff(val1,val2));

            }
        }else
        {//as int

            #pragma omp parallel for
            for(int r = 0; r < rows; r++)
            {
                int val1 = M1->GetValueInt(r,c);
                int val2 = val;
                Target->SetValue(r,c,fi(val1,val2));
            }
        }
    }

    return Target;


}


static inline MatrixTable * ApplyOperatorToMatrixTables(MatrixTable * M1, int val, std::function<int(int,int)> fi,std::function<double(double,double)> ff,std::function<QString(QString,QString)> fs, bool write_to_one = false)
{
    if( M1 == nullptr )
    {
        LISEM_ERROR("One of the tables provided to the operator is zero")
        throw 1;
        return new MatrixTable();
    }

    MatrixTable * Target;
    if(write_to_one)
    {
        Target = M1;
    }else {
        Target = M1->Copy();
    }

    //get size of table
    int rows = M1->GetNumberOfRows();
    int cols = M1->GetNumberOfCols();

    //now do the actual calculations
    for(int c = 0; c < cols; c++)
    {
        //get datatype for this column
        int type = M1->GetColumnType(c);

        if(type == TABLE_TYPE_UNKNOWN || type == TABLE_TYPE_STRING)
        {//as string

            #pragma omp parallel for
            for(int r = 0; r < rows; r++)
            {
                QString val1 = M1->GetValueStringQ(r,c);
                QString val2 = QString::number(val);

                Target->SetValue(r,c,fs(val1,val2));
            }
        }else if(type == TABLE_TYPE_FLOAT64)
        {//as float

            #pragma omp parallel for
            for(int r = 0; r < rows; r++)
            {
                double val1 = M1->GetValueDouble(r,c);
                double val2 = val;

                Target->SetValue(r,c,ff(val1,val2));

            }
        }else
        {//as int

            #pragma omp parallel for
            for(int r = 0; r < rows; r++)
            {
                int val1 = M1->GetValueInt(r,c);
                int val2 = val;
                Target->SetValue(r,c,fi(val1,val2));
            }
        }
    }

    return Target;
}


static inline MatrixTable * ApplyOperatorToMatrixTables(MatrixTable * M1, QString val, std::function<int(int,int)> fi,std::function<double(double,double)> ff,std::function<QString(QString,QString)> fs, bool write_to_one = false)
{
    if( M1 == nullptr )
    {
        LISEM_ERROR("One of the tables provided to the operator is zero")
        throw 1;
        return new MatrixTable();
    }

    MatrixTable * Target;
    if(write_to_one)
    {
        Target = M1;
    }else {
        Target = M1->Copy();
    }

    //get size of table
    int rows = M1->GetNumberOfRows();
    int cols = M1->GetNumberOfCols();

    //now do the actual calculations
    for(int c = 0; c < cols; c++)
    {
        //get datatype for this column
        int type = M1->GetColumnType(c);

        if(type == TABLE_TYPE_UNKNOWN || type == TABLE_TYPE_STRING)
        {//as string

            #pragma omp parallel for
            for(int r = 0; r < rows; r++)
            {
                QString val1 = M1->GetValueStringQ(r,c);
                QString val2 = val;

                Target->SetValue(r,c,fs(val1,val2));
            }
        }else if(type == TABLE_TYPE_FLOAT64)
        {//as float

            #pragma omp parallel for
            for(int r = 0; r < rows; r++)
            {
                double val1 = M1->GetValueDouble(r,c);
                bool ok =false;
                double val2 = val.toDouble(&ok);
                if(ok)
                {
                    Target->SetValue(r,c,ff(val1,val2));
                }

            }
        }else
        {//as int

            #pragma omp parallel for
            for(int r = 0; r < rows; r++)
            {
                int val1 = M1->GetValueInt(r,c);
                bool ok =false;
                int val2 = val.toInt(&ok);
                if(ok)
                {
                    Target->SetValue(r,c,fi(val1,val2));
                }
            }
        }
    }

    return Target;




}



#endif // TABLEALGEBRA_H
