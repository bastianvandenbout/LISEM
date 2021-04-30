#ifndef TABLEANALYSIS_H
#define TABLEANALYSIS_H

#include "qpair.h"
#include "qlist.h"

#include "matrixtable.h"

static inline MatrixTable * SortByColumn(MatrixTable * in, int column, bool descending)
{
    MatrixTable * ret = in->SortByColumn(column,descending);
    return ret;
}

static inline MatrixTable * SortColumn(MatrixTable * in, int column, bool descending)
{
    MatrixTable * ret = in->Copy();
    ret->SortColumn(column,descending);
    return ret;
}

static inline MatrixTable * SortByColumn(MatrixTable * in, QString name)
{


    return nullptr;
}

static inline MatrixTable * SortColumn(MatrixTable * in, QString name)
{


    return nullptr;
}



#endif // TABLEANALYSIS_H
