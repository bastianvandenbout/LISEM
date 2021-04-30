#ifndef RASTERTABLE_H
#define RASTERTABLE_H

#include "matrixtable.h"
#include "geo/raster/map.h"

inline MatrixTable * AS_RasterTableClasses(cTMap * Other)
{

    MatrixTable * ret = new MatrixTable();

    std::vector<int> classes;
    std::vector<float> vals;

    float cellarea = Other->cellSize() * Other->cellSize();

    //we need to find averages for each class
    //first, we find all the classes
    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(!pcr::isMV(Other->data[r][c]))
            {
                bool found = false;
                int class_current = (int)Other->data[r][c];
                int i;
                for(i= 0; i < classes.size(); i++)
                {
                    if(class_current == classes.at(i))
                    {
                        found = true;
                        break;
                    }
                }

                if(found)
                {
                }else
                {
                    i = classes.size();
                    classes.push_back(class_current);
                }
            }

        }
    }

    ret->SetSize(classes.size(),1);
    for(int i = 0; i < classes.size(); i++)
    {
        ret->SetValue(i,0,classes.at(i));
    }

    return ret;
}


inline MatrixTable * AS_RasterTableMinimum(cTMap * Other,cTMap * Other2)
{
    if(Other->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for RasterTableMinimum (Class) can not be non-spatial");
        throw 1;
    }
    if(Other2->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 2 for RasterTableMinimum (Values) can not be non-spatial");
        throw 1;
    }

    if(!(Other->data.nr_rows() ==  Other2->data.nr_rows() && Other->data.nr_cols() == Other2->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }

    MatrixTable * ret = new MatrixTable();

    std::vector<int> classes;
    std::vector<float> vals;

    float cellarea = Other->cellSize() * Other->cellSize();

    //we need to find averages for each class
    //first, we find all the classes
    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(!pcr::isMV(Other->data[r][c]))
            {
                bool found = false;
                int class_current = (int)Other->data[r][c];
                int i;
                for(i= 0; i < classes.size(); i++)
                {
                    if(class_current == classes.at(i))
                    {
                        found = true;
                        break;
                    }

                }

                if(found)
                {
                    vals.at(i)  = std::min(vals.at(i),Other2->data[r][c]);
                }else
                {
                    i = classes.size();
                    classes.push_back(class_current);
                    vals.push_back(1e30);
                    vals.at(i)  = std::min(vals.at(i),Other2->data[r][c]);
                }
            }

        }
    }

    ret->SetSize(classes.size()+1,2);
    ret->SetValue(0,0,QString(""));
    ret->SetValue(0,1,1);
    for(int i = 0; i < classes.size(); i++)
    {
        ret->SetValue(i+1,0,classes.at(i));
        ret->SetValue(i+1,1,vals.at(i));
    }

    return ret;
}

inline MatrixTable * AS_RasterTableAverage(cTMap * Other,cTMap * Other2)
{
    if(Other->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for RasterTableAverage (RowValue) can not be non-spatial");
        throw 1;
    }
    if(Other2->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 2 for RasterTableAverage (Values) can not be non-spatial");
        throw 1;
    }

    if(!(Other->data.nr_rows() ==  Other2->data.nr_rows() && Other->data.nr_cols() == Other2->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }

    MatrixTable * ret = new MatrixTable();

    std::vector<int> classes;
    std::vector<float> vals;
    std::vector<float> ns;

    float cellarea = Other->cellSize() * Other->cellSize();

    //we need to find averages for each class
    //first, we find all the classes
    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(!pcr::isMV(Other->data[r][c]))
            {
                bool found = false;
                int class_current = (int)Other->data[r][c];
                int i;
                for(i= 0; i < classes.size(); i++)
                {
                    if(class_current == classes.at(i))
                    {
                        found = true;
                        break;
                    }

                }

                if(found)
                {
                    vals.at(i) += Other2->data[r][c];
                    ns.at(i) += 1.0f;
                }else
                {
                    i = classes.size();
                    classes.push_back(class_current);
                    vals.push_back(0.0);
                    ns.push_back(0.0);
                    vals.at(i) += Other2->data[r][c];
                    ns.at(i) += 1.0f;
                }
            }

        }
    }

    for(int i= 0; i < classes.size(); i++)
    {
            vals.at(i) = vals.at(i)/std::max(1.0f,ns.at(i));
    }

    std::cout << classes.size() << std::endl;

    ret->SetSize(classes.size()+1,2);
    ret->SetValue(0,0,QString(""));
    ret->SetValue(0,1,1);
    for(int i = 0; i < classes.size(); i++)
    {
        ret->SetValue(i+1,0,classes.at(i));
        ret->SetValue(i+1,1,vals.at(i));
    }

    return ret;
}

inline MatrixTable * AS_RasterTableTotal(cTMap * Other,cTMap * Other2)
{
    if(Other->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for AreaArea (Class) can not be non-spatial");
        throw 1;
    }
    if(Other2->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 2 for AreaArea (Values) can not be non-spatial");
        throw 1;
    }

    if(!(Other->data.nr_rows() ==  Other2->data.nr_rows() && Other->data.nr_cols() == Other2->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }

    MatrixTable * ret = new MatrixTable();

    std::vector<int> classes;
    std::vector<float> vals;

    float cellarea = Other->cellSize() * Other->cellSize();

    //we need to find averages for each class
    //first, we find all the classes
    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(!pcr::isMV(Other->data[r][c]))
            {
                bool found = false;
                int class_current = (int)Other->data[r][c];
                int i;
                for(i= 0; i < classes.size(); i++)
                {
                    if(class_current == classes.at(i))
                    {
                        found = true;
                        break;
                    }

                }

                if(found)
                {
                    vals.at(i) += Other2->data[r][c];
                }else
                {
                    i = classes.size();
                    classes.push_back(class_current);
                    vals.push_back(0.0);
                    vals.at(i) += Other2->data[r][c];
                }
            }

        }
    }


    ret->SetSize(classes.size()+1,2);
    ret->SetValue(0,0,QString(""));
    ret->SetValue(0,1,1);
    for(int i = 0; i < classes.size(); i++)
    {
        ret->SetValue(i+1,0,classes.at(i));
        ret->SetValue(i+1,1,vals.at(i));
    }

    return ret;
}
inline MatrixTable * AS_RasterTableMaximum(cTMap * Other,cTMap * Other2)
{
    if(Other->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for AreaArea (Class) can not be non-spatial");
        throw 1;
    }
    if(Other2->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 2 for AreaArea (Values) can not be non-spatial");
        throw 1;
    }

    if(!(Other->data.nr_rows() ==  Other2->data.nr_rows() && Other->data.nr_cols() == Other2->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }

    MatrixTable * ret = new MatrixTable();

    std::vector<int> classes;
    std::vector<float> vals;

    float cellarea = Other->cellSize() * Other->cellSize();

    //we need to find averages for each class
    //first, we find all the classes
    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(!pcr::isMV(Other->data[r][c]))
            {
                bool found = false;
                int class_current = (int)Other->data[r][c];
                int i;
                for(i= 0; i < classes.size(); i++)
                {
                    if(class_current == classes.at(i))
                    {
                        found = true;
                        break;
                    }

                }

                if(found)
                {
                    vals.at(i)  = std::max(vals.at(i),Other2->data[r][c]);
                }else
                {
                    i = classes.size();
                    classes.push_back(class_current);
                    vals.push_back(-1e30);
                    vals.at(i)  = std::max(vals.at(i),Other2->data[r][c]);
                }
            }

        }
    }

    ret->SetSize(classes.size()+1,2);
    ret->SetValue(0,0,QString(""));
    ret->SetValue(0,1,1);
    for(int i = 0; i < classes.size(); i++)
    {
        ret->SetValue(i+1,0,classes.at(i));
        ret->SetValue(i+1,1,vals.at(i));
    }

    return ret;
}

inline cTMap * AS_RasterFromTableHeader(MatrixTable * table, cTMap * Other, cTMap * Other2)
{

    if(!(table->GetNumberOfCols() > 1 && table->GetNumberOfCols() > 1))
    {
        LISEMS_ERROR("Table does not contain at least 2 rows and 2 columns (top row = row class) (left column = column class");
        throw -1;
    }
    if(!Other->AS_IsSingleValue && !Other2->AS_IsSingleValue)
    {
        if(!(Other->data.nr_rows() ==  Other2->data.nr_rows() && Other->data.nr_cols() == Other2->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }

    cTMap *map;

      int nr_cols;
      int nr_rows;

    if(Other->AS_IsSingleValue)
    {
        nr_cols = Other2->data.nr_cols();
        nr_rows = Other2->data.nr_rows();
        MaskedRaster<float> raster_data(Other2->data.nr_rows(), Other2->data.nr_cols(), Other2->data.north(), Other2->data.west(), Other2->data.cell_size(),Other2->data.cell_sizeY());
        map = new cTMap(std::move(raster_data),Other2->projection(),"");

    }else {
        nr_cols = Other->data.nr_cols();
        nr_rows = Other->data.nr_rows();
        MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
        map = new cTMap(std::move(raster_data),Other2->projection(),"");
    }


    QList<int> rs;
    QList<int> cs;

    for(int i = 1; i < table->GetNumberOfCols(); i++)
    {
        cs.append(table->GetValueInt(0,i));
    }
    for(int i = 1; i < table->GetNumberOfRows(); i++)
    {
        rs.append(table->GetValueInt(i,0));
    }
    int tnrcols=  table->GetNumberOfCols();
    int tnrrows = table->GetNumberOfRows();


    std::cout << "get raster from table values " << rs.size() << " " << cs.size() << " " << rs.at(0) << std::endl;


    for(int r = 0; r < nr_rows;r++)
    {
        for(int c = 0; c < nr_cols;c++)
        {

            {
                int class_r = 0;
                int class_c = 0;

                if(Other->AS_IsSingleValue)
                {
                    if(pcr::isMV(Other->data[0][0]))
                    {

                    }else {
                        class_r = (int) (0.5f +Other->data[0][0]);
                    }
                }else {
                    class_r = (int) (0.5f +Other->data[r][c]);
                }

                if(Other2->AS_IsSingleValue)
                {
                    if(pcr::isMV(Other2->data[0][0]))
                    {

                    }else {
                        class_c = (int) (0.5f +Other2->data[0][0]);
                    }
                }else {
                    class_c = (int) (0.5f +Other2->data[r][c]);
                }


                if(class_r == 0 || class_c == 0)
                {
                    pcr::setMV(map->data[r][c]);
                }else {
                    int ri = rs.indexOf(class_r);
                    int ci = cs.indexOf(class_c);
                    if(ri > -1 && ci > -1 && ri < tnrrows && ci < tnrcols)
                    {
                        map->data[r][c] = table->GetValueDouble(ri+1,ci+1);
                    }else {
                        pcr::setMV(map->data[r][c]);
                    }
                }

            }
        }
    }


    return map;
}

inline cTMap * AS_RasterFromTable(MatrixTable * table, cTMap * Other, cTMap * Other2)
{

    if(!(table->GetNumberOfCols() > 1 && table->GetNumberOfCols() > 1))
    {
        LISEMS_ERROR("Table does not contain at least 2 rows and 2 columns (top row = row class) (left column = column class");
        throw -1;
    }
    if(!Other->AS_IsSingleValue && !Other2->AS_IsSingleValue)
    {
        if(!(Other->data.nr_rows() ==  Other2->data.nr_rows() && Other->data.nr_cols() == Other2->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }

    cTMap *map;

    if(Other->AS_IsSingleValue)
    {
        MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
        map = new cTMap(std::move(raster_data),Other2->projection(),"");
    }else {
        MaskedRaster<float> raster_data(Other2->data.nr_rows(), Other2->data.nr_cols(), Other2->data.north(), Other2->data.west(), Other2->data.cell_size(),Other2->data.cell_sizeY());
        map = new cTMap(std::move(raster_data),Other2->projection(),"");
    }

    int tnrcols=  table->GetNumberOfCols();
    int tnrrows = table->GetNumberOfRows();

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {

            {
                int class_r = 0;
                int class_c = 0;

                if(Other->AS_IsSingleValue)
                {
                    if(pcr::isMV(Other->data[0][0]))
                    {

                    }else {
                        class_r = (int) (0.5f +Other->data[0][0]);
                    }
                }else {
                    class_r = (int) (0.5f +Other->data[r][c]);
                }

                if(Other2->AS_IsSingleValue)
                {
                    if(pcr::isMV(Other2->data[0][0]))
                    {

                    }else {
                        class_c = (int) (0.5f +Other2->data[0][0]);
                    }
                }else {
                    class_c = (int) (0.5f +Other2->data[r][c]);
                }


                if(class_r == 0 || class_c == 0)
                {
                    pcr::setMV(map->data[r][c]);
                }else {
                    int ri = class_r;
                    int ci = class_c;
                    if(ri > -1 && ci > -1 && ri < tnrrows && ci < tnrcols)
                    {
                        map->data[r][c] = table->GetValueDouble(ri,ci);
                    }else {
                        pcr::setMV(map->data[r][c]);
                    }
                }

            }
        }
    }


    return map;
}



#endif // RASTERTABLE_H
