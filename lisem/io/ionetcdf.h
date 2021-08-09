#ifndef IONETCDF_H
#define IONETCDF_H

#include "netcdf.h"
#include <iostream>
#include <string>
#include "geo/raster/field.h"
#include <vector>
#include "QString"
#include "error.h"

inline bool WriteFieldList(std::vector<Field*> fields, std::vector<float> times, QString path)
{
    std::cout << "write field to path " << path.toStdString() << std::endl;
    if(fields.size() == 0)
    {
        LISEMS_ERROR("Could not write file due to 0 fields " + path );
        return false;
    }
    if(fields.at(0)->GetMapList().size() == 0)
    {
        LISEMS_ERROR("Could not write file due to 0 levels " + path);
        return false;
    }

    int x_dimid,y_dimid,z_dimid,t_dimid;

    int ncid, varid;

    int retval = nc_create(path.toStdString().c_str(), NC_CLOBBER, &ncid);
    if(retval != NC_NOERR)
    {

        LISEMS_ERROR("Could not write file (error opening) " + path + " " + QString::number(retval));
        return false;
    }

    int NDIMS  = 4;
    int NX = fields.at(0)->GetMapList().at(0)->nrCols();
    int NY = fields.at(0)->GetMapList().at(0)->nrRows();
    int NZ = fields.at(0)->GetMapList().size();
    int NT = fields.size();



    retval = nc_def_dim(ncid, "dim_x", NX, &x_dimid);
    if(retval != NC_NOERR)
    {
        LISEMS_ERROR("Could not write file (can not create dimension)" + path+ " " + QString::number(retval));
        return false;
    }

    retval = nc_def_dim(ncid, "dim_y", NY, &y_dimid);
    if(retval != NC_NOERR)
    {
        LISEMS_ERROR("Could not write file (can not create dimension)" + path+ " " + QString::number(retval));
        return false;
    }

    retval = nc_def_dim(ncid, "dim_z", NZ, &z_dimid);
    if(retval != NC_NOERR)
    {
        LISEMS_ERROR("Could not write file (can not create dimension)" + path+ " " + QString::number(retval));
        return false;
    }

    retval = nc_def_dim(ncid, "dim_t", NT, &t_dimid);
    if(retval != NC_NOERR)
    {
        LISEMS_ERROR("Could not write file " + path+ " " + QString::number(retval));
        return false;
    }


    int dimids[NDIMS] = {t_dimid,z_dimid,y_dimid,x_dimid};

    retval = nc_def_var(ncid, "data", NC_FLOAT, NDIMS,dimids, &varid);

    if(retval != NC_NOERR)
    {
        LISEMS_ERROR("Could not write file (cannot create variable)  " + path+ " " + QString::number(retval));
        return false;
    }

    int x_varid,y_varid,z_varid,t_varid;

    retval = nc_def_var(ncid, "dim_x", NC_FLOAT, 1, &x_dimid,
                                &x_varid);
    if(retval != NC_NOERR)
    {
        LISEMS_ERROR("Could not write file (can not create dimension variable)" + path+ " " + QString::number(retval));
        return false;
    }
    retval = nc_def_var(ncid, "dim_y", NC_FLOAT, 1, &y_dimid,
                                &y_varid);
    if(retval != NC_NOERR)
    {
        LISEMS_ERROR("Could not write file (can not create dimension variable)" + path+ " " + QString::number(retval));
        return false;
    }
    retval = nc_def_var(ncid, "dim_z", NC_FLOAT, 1, &z_dimid,
                                &z_varid);
    if(retval != NC_NOERR)
    {
        LISEMS_ERROR("Could not write file (can not create dimension variable)" + path+ " " + QString::number(retval));
        return false;
    }
    retval = nc_def_var(ncid, "dim_t", NC_FLOAT, 1, &t_dimid,
                                &t_varid);
    if(retval != NC_NOERR)
    {
        LISEMS_ERROR("Could not write file (can not create dimension variable)" + path+ " " + QString::number(retval));
        return false;
    }

    /* End define mode. */
   retval = nc_enddef(ncid);
   if(retval != NC_NOERR)
   {
       LISEMS_ERROR("Could not write file (not defined well)  " + path+ " " + QString::number(retval));
       return false;
   }

   std::cout << "end define " << std::endl;

   size_t start[ 4], count[ 4];
   count[0] = 1;
   count[1] = 1;
   count[2] = NY;
   count[3] = NX;

   for(int i = 0; i < NT; i++)
   {
       std::vector<cTMap*> maps = fields.at(i)->GetMapList();

       if(maps.size() < NZ)
       {
           LISEMS_ERROR("Could not write file due to wrong number of vertical layers" + path+ " " + QString::number(retval));
           return false;
       }

       for(int j = 0; j < NZ; j++)
       {
           start[0] = i;
           start[1] = j;
           start[2] = 0;
           start[3] = 0;

           retval = nc_put_vara_float(ncid,varid,start,count,maps.at(j)->data[0]);
           if(retval != NC_NOERR)
           {
               LISEMS_ERROR("Could not write file (can not put data)" + path+ " " + QString::number(retval));
               return false;
           }
       }
   }

   std::cout << "done writing data " << std::endl;


   /*retval = nc_def_dim(ncid, "x", NX, &x_dimid);
   retval = nc_def_dim(ncid, "y", NY, &y_dimid);
   retval = nc_def_dim(ncid, "z", NZ, &z_dimid);
   retval = nc_def_dim(ncid, "t", NT, &t_dimid);*/



   std::vector<float> x_vals;
   std::vector<float> y_vals;
   std::vector<float> z_vals;
   std::vector<float> t_vals;


   for(int i = 0; i < NX; i++)
   {
        x_vals.push_back(fields.at(0)->GetMapList().at(0)->west() + fields.at(0)->GetMapList().at(0)->cellSizeX() * ((float)(i)));
   }
   for(int i = 0; i < NY; i++)
   {
        y_vals.push_back(fields.at(0)->GetMapList().at(0)->north() + fields.at(0)->GetMapList().at(0)->cellSizeY() * ((float)(i)));
   }
   for(int i = 0; i < NZ; i++)
   {
        z_vals.push_back(fields.at(0)->m_ZStart + fields.at(0)->m_dz * ((float)(i)));
   }
   for(int i = 0; i < NT; i++)
   {
        t_vals.push_back(i);
   }

   std::cout << "write field dims data " << fields.at(0)->GetMapList().at(0)->cellSizeX() << "  " << fields.at(0)->GetMapList().at(0)->cellSizeY() << std::endl;


   //nc_put_att_text(ncid, lat_varid, UNITS, strlen(DEGREES_NORTH), DEGREES_NORTH)

   retval = nc_put_var_float(ncid, x_varid, &x_vals[0]);
   if(retval != NC_NOERR)
   {
       LISEMS_ERROR("Could not write file (can not put dimension variable)" + path+ " " + QString::number(retval));
       return false;
   }
   retval = nc_put_var_float(ncid, y_varid, &y_vals[0]);
   if(retval != NC_NOERR)
   {
       LISEMS_ERROR("Could not write file (can not put dimension variable)" + path+ " " + QString::number(retval));
       return false;
   }
   retval = nc_put_var_float(ncid, z_varid, &z_vals[0]);
   if(retval != NC_NOERR)
   {
       LISEMS_ERROR("Could not write file (can not put dimension variable)" + path+ " " + QString::number(retval));
       return false;
   }
   retval = nc_put_var_float(ncid, t_varid, &t_vals[0]);
   if(retval != NC_NOERR)
   {
       LISEMS_ERROR("Could not write file (can not put dimension variable)" + path+ " " + QString::number(retval));
       return false;
   }
   //let us put some additional attributes, for CRS and such


   std::cout << "done writing dim var data " << std::endl;



   //now close the file
   retval = nc_close(ncid);
   if(retval != NC_NOERR)
   {
       LISEMS_ERROR("Could not write file (can not close)" + path+ " " + QString::number(retval));
       return false;
   }

   return true;
}


inline std::vector<Field*> ReadFieldList(QString path, QString variable = "", bool only_do_first = false)
{
    std::vector<Field*> ret;

    if(variable.isEmpty())
    {
        variable = "data";
    }

    int ncid = 0;
    int retval = nc_open(path.toStdString().c_str(), NC_NOWRITE, &ncid);
    if(retval != NC_NOERR)
    {
        LISEMS_ERROR("Could not read file (can not open)" + path);
        return ret;
    }

    GeoProjection p;



    int n_dims;
    int n_vars;
    int n_atts;
    int n_unlimited;

    retval = nc_inq(ncid,&n_dims,&n_vars, &n_atts, &n_unlimited);
    if(retval != NC_NOERR)
    {
        LISEMS_ERROR("Could not read file (can not inquire) " + path);
        return ret;
    }

    //get primary variable
    int xtype;
    int var_ndims;
    int var_natts;
    int rh_dimids[NC_MAX_VAR_DIMS];
    size_t rh_dimlength[NC_MAX_VAR_DIMS];

    int ndims_max = 0;
    int var_ndimsmax = -1;
    char var_name[ NC_MAX_NAME + 1];
    bool found = false;
    int varid = 0;
    for(int i = 0; i < n_vars; i++)
    {
        varid = i;
        //get properties of this variable
        nc_inq_var(ncid,varid,var_name,&xtype,&var_ndims,rh_dimids,&var_natts);

        if(var_ndims > 1 && var_ndims < 5 && var_ndims > ndims_max)
        {
            var_ndimsmax = 1;
        }
        if(QString(var_name).compare(variable) == 0)
        {
            found = true;

            break;
        }
    }

    //if not found, use first one with greatest number of dimensions (although less than 5)
    if(!found && ndims_max > 1)
    {
        found = true;
        varid = var_ndimsmax;
        nc_inq_var(ncid,varid,var_name,&xtype,&var_ndims,rh_dimids,&var_natts);
    }

    if(!found)
    {
        LISEMS_ERROR("Could not read file, no suitable variable found" + path + "variable name: " + variable);
        return ret;
    }

    //get variables with one dimension, and that dimension is equal to the nth dimension of the primary variable
    std::vector<int> dim_var(var_ndims);
    for(int i = 0; i < var_ndims; i++)
    {
        dim_var[i] = -999;
    }

    for(int i = 0; i < n_vars; i++)
    {
        int temp_xtype;
        int temp_var_ndims;
        int temp_var_natts;
        int temp_rh_dimids[NC_MAX_VAR_DIMS];
        char temp_var_name[ NC_MAX_NAME + 1];
        int temp_varid = i;
        //get properties of this variable
        nc_inq_var(ncid,temp_varid,temp_var_name,&temp_xtype,&temp_var_ndims,temp_rh_dimids,&temp_var_natts);

        std::cout << "found var " << temp_var_name << " " << temp_var_ndims << std::endl;

        for(int j = 0; j < var_ndims; j++)
        {
            if(temp_var_ndims == 1 && temp_rh_dimids[0] == rh_dimids[j])
            {
                std::cout << "set var as dim var "<< std::endl;
                dim_var[j] = temp_varid;
                break;
            }
        }
    }

    //get primary dimensions sizes


    for(int i = 0; i < var_ndims; i++)
    {
        int dim_id_i = rh_dimids[i];
        char dim_name[ NC_MAX_NAME + 1];
        size_t dim_length = 0;
        nc_inq_dim(ncid,dim_id_i,dim_name,&dim_length);
        rh_dimlength[i] = dim_length;
    }

    if(!found)
    {
        LISEMS_ERROR("Could not read file (variable not found) " + path);
        return ret;
    }

    if(var_ndims == 2)
    {

        int dimid_y = rh_dimids[0];
        int dimid_x = rh_dimids[1];

        char dim_name[ NC_MAX_NAME + 1];

        size_t length_x;
        size_t length_y;

        nc_inq_dim(ncid,dimid_x,dim_name,&length_x);
        nc_inq_dim(ncid,dimid_y,dim_name,&length_y);


        float x0 = 0;
        float y0 = 0;
        float dx = 1.0;
        float dy = 1.0;

        //get the ulc and cellsize from the x and y coordinate data
        if(dim_var[0] > -1 && dim_var[1] > -1)
        {
            std::vector<float> xdata(length_x);
            std::vector<float> ydata(length_y);

            nc_get_var_float(ncid,dim_var[1],&xdata[0]);
            nc_get_var_float(ncid,dim_var[0],&ydata[0]);

            x0 = xdata[0];
            y0 = ydata[0];
            if(length_x > 0)
            {
                dx = (xdata[length_x-1] - x0)/((float)(length_x-1));
            }
            if(length_y > 0)
            {
                dy = (ydata[length_y-1] - y0)/((float)(length_y-1));
            }

        }

        std::cout << "read field2 " << dx << "  " << dy << std::endl;


        MaskedRaster<float> raster_data(length_y, length_x,y0, x0, dx,dy);
        cTMap *m = new cTMap(std::move(raster_data),p.GetWKT(),"");


        //get the actual data
        size_t start[2];
        size_t count[2];
        count[0] = length_y;
        count[1] = length_x;
        start[0] = 0;
        start[1] = 0;
        nc_get_vara(ncid,varid,start,count,m->data[0]);

        Field * f = new Field();
        f->SetFromMapList({m},0,1,false);

        ret.push_back(f);

    }else if(var_ndims == 3)
    {
        int dimid_z = rh_dimids[0];
        int dimid_y = rh_dimids[1];
        int dimid_x = rh_dimids[2];

        char dim_name[ NC_MAX_NAME + 1];

        size_t length_x;
        size_t length_y;
        size_t length_z;

        nc_inq_dim(ncid,dimid_x,dim_name,&length_x);
        nc_inq_dim(ncid,dimid_y,dim_name,&length_y);
        nc_inq_dim(ncid,dimid_z,dim_name,&length_z);

        float x0 = 0;
        float y0 = 0;
        float z0 = 0;
        float dx = 1.0;
        float dy = 1.0;
        float dz = 1.0;

        //get the ulc and cellsize from the x and y coordinate data
        if(dim_var[0] > -1 && dim_var[1] > -1 && dim_var[2] > -1)
        {
            std::vector<float> xdata(length_x);
            std::vector<float> ydata(length_y);
            std::vector<float> zdata(length_z);

            nc_get_var_float(ncid,dim_var[0],&zdata[0]);
            nc_get_var_float(ncid,dim_var[1],&ydata[0]);
            nc_get_var_float(ncid,dim_var[2],&xdata[0]);

            x0 = xdata[0];
            y0 = ydata[0];
            z0 = zdata[0];
            if(length_x > 0)
            {
                dx = (xdata[length_x-1] - x0)/((float)(length_x-1));
            }
            if(length_y > 0)
            {
                dy = (ydata[length_y-1] - y0)/((float)(length_y-1));
            }
            if(length_z > 0)
            {
                dz = (zdata[length_z-1] - z0)/((float)(length_z-1));
            }
        }

        std::cout << "read field " << dx << "  " << dy << std::endl;

        std::vector<cTMap*> levels;
        for(int i = 0; i < length_z; i++)
        {
            MaskedRaster<float> raster_data(length_y, length_x,y0, x0, dx,dy);
            cTMap *m = new cTMap(std::move(raster_data),p.GetWKT(),"");

            //get the actual data
            size_t start[3];
            size_t count[3];
            count[0] = 1;
            count[1] = length_y;
            count[2] = length_x;
            start[0] = i;
            start[1] = 0;
            start[2] = 0;
            nc_get_vara(ncid,varid,start,count,m->data[0]);

            levels.push_back(m);
        }

        Field * f = new Field();
        f->SetFromMapList(levels,z0,dz,false);

        ret.push_back(f);


    }else if(var_ndims == 4)
    {
        int dimid_t = rh_dimids[0];
        int dimid_z = rh_dimids[1];
        int dimid_y = rh_dimids[2];
        int dimid_x = rh_dimids[3];

        char dim_name[ NC_MAX_NAME + 1];

        size_t length_x;
        size_t length_y;
        size_t length_z;
        size_t length_t;

        nc_inq_dim(ncid,dimid_x,dim_name,&length_x);
        nc_inq_dim(ncid,dimid_y,dim_name,&length_y);
        nc_inq_dim(ncid,dimid_z,dim_name,&length_z);
        nc_inq_dim(ncid,dimid_t,dim_name,&length_t);

        float x0 = 0;
        float y0 = 0;
        float z0 = 0;
        float t0 = 0;
        float dx = 1.0;
        float dy = 1.0;
        float dz = 1.0;
        float dt = 1.0;

        std::cout << "dim vars " << dim_var[0] << " " << dim_var[2] << " " << dim_var[3] << std::endl;
        //get the ulc and cellsize from the x and y coordinate data
        if(dim_var[1] > -1 && dim_var[2] > -1 && dim_var[3] > -1 )
        {
            std::vector<float> xdata(length_x);
            std::vector<float> ydata(length_y);
            std::vector<float> zdata(length_z);

            nc_get_var_float(ncid,dim_var[1],&zdata[0]);
            nc_get_var_float(ncid,dim_var[2],&ydata[0]);
            nc_get_var_float(ncid,dim_var[3],&xdata[0]);

            x0 = xdata[0];
            y0 = ydata[0];
            z0 = zdata[0];
            if(length_x > 0)
            {
                dx = (xdata[length_x-1] - x0)/((float)(length_x-1));
            }
            if(length_y > 0)
            {
                dy = (ydata[length_y-1] - y0)/((float)(length_y-1));
            }
            if(length_z > 0)
            {
                dz = (zdata[length_z-1] - z0)/((float)(length_z-1));
            }
        }
        if(dim_var[0] > -1)
        {
            std::vector<float> tdata(length_t);
            nc_get_var_float(ncid,dim_var[0],&tdata[0]);
            t0 = tdata[0];
            if(length_t > 0)
            {
                dt = (tdata[length_t-1] - t0)/((float)(length_t-1));
            }
        }
        std::cout << "read field4 " << dx << "  " << dy << std::endl;


        for(int j = 0; j < length_t; j++)
        {
            std::vector<cTMap*> levels;
            for(int i = 0; i < length_z; i++)
            {
                MaskedRaster<float> raster_data(length_y, length_x,y0, x0, dx,dy);
                cTMap *m = new cTMap(std::move(raster_data),p.GetWKT(),"");

                //get the actual data
                size_t start[4];
                size_t count[4];
                count[0] = 1;
                count[1] = 1;
                count[2] = length_y;
                count[3] = length_x;
                start[0] = j;
                start[1] = i;
                start[2] = 0;
                start[3] = 0;
                nc_get_vara(ncid,varid,start,count,m->data[0]);

                levels.push_back(m);
            }

            Field * f = new Field();
            f->SetFromMapList(levels,z0,dz,false);

            ret.push_back(f);
            if(only_do_first)
            {
                break;
            }
        }

    }else
    {
        LISEMS_ERROR("Could not read file (either 1 or more than 4 dimensions)" + path);


    }
    retval = nc_close(ncid);
    if(retval != NC_NOERR)
    {
        LISEMS_ERROR("Could not write file " + path);
    }
    return ret;
}

inline std::vector<QString> GetVarNamesFromNetCDF(QString path)
{
    std::vector<QString> names;

    int ncid = 0;
    int retval = nc_open(path.toStdString().c_str(), NC_NOWRITE, &ncid);
    if(retval != NC_NOERR)
    {
        LISEMS_ERROR("Could not read file " + path);
        return names;
    }

    int n_dims;
    int n_vars;
    int n_atts;
    int n_unlimited;

    retval = nc_inq(ncid,&n_dims,&n_vars, &n_atts, &n_unlimited);
    if(retval != NC_NOERR)
    {
        LISEMS_ERROR("Could not read file " + path);
        retval = nc_close(ncid);
        if(retval != NC_NOERR)
        {
            LISEMS_ERROR("Could not close file " + path);
        }
        return names;
    }

    //get primary variable
    int xtype;
    int var_ndims;
    int var_natts;
    int rh_dimids[NC_MAX_VAR_DIMS];
    size_t rh_dimlength[NC_MAX_VAR_DIMS];

    int ndims_max = 0;
    int var_ndimsmax = -1;
    char var_name[ NC_MAX_NAME + 1];
    bool found = false;
    int varid = 0;
    for(int i = 0; i < n_vars; i++)
    {
        varid = i;
        //get properties of this variable
        nc_inq_var(ncid,varid,var_name,&xtype,&var_ndims,rh_dimids,&var_natts);

        if(var_ndims > 1 && var_ndims < 5 && var_ndims > ndims_max)
        {
            var_ndimsmax = 1;
        }
        names.push_back(QString(var_name));
    }

    retval = nc_close(ncid);
    if(retval != NC_NOERR)
    {
        LISEMS_ERROR("Could not write file " + path);
    }

    return names;


}

inline std::vector<QString> GetVarDimNames(QString path, QString variable)
{
    std::vector<QString> names;

    int ncid = 0;
    int retval = nc_open(path.toStdString().c_str(), NC_NOWRITE, &ncid);
    if(retval != NC_NOERR)
    {
        LISEMS_ERROR("Could not read file " + path);
        return names;
    }

    int varid = 0;

    retval = nc_inq_varid(ncid,variable.toStdString().c_str(),&varid);
    if(retval != NC_NOERR)
    {
        LISEMS_ERROR("Could not read file " + path);
        retval = nc_close(ncid);
        if(retval != NC_NOERR)
        {
            LISEMS_ERROR("Could not close file " + path);
        }
        return names;
    }

    int xtype;
    int var_ndims = 0;
    int var_natts;
    int rh_dimids[NC_MAX_VAR_DIMS];
    size_t rh_dimlength[NC_MAX_VAR_DIMS];

    int ndims_max = 0;
    int var_ndimsmax = -1;
    char var_name[ NC_MAX_NAME + 1];
    bool found = false;
    //get properties of this variable
    nc_inq_var(ncid,varid,var_name,&xtype,&var_ndims,rh_dimids,&var_natts);
    if(retval != NC_NOERR)
    {
        LISEMS_ERROR("Could not read file " + path);
        retval = nc_close(ncid);
        if(retval != NC_NOERR)
        {
            LISEMS_ERROR("Could not close file " + path);
        }
        return names;
    }

    for(int i = 0; i < var_ndims; i++)
    {
        char dim_name[ NC_MAX_NAME + 1];
        nc_inq_dimname(ncid,rh_dimids[i],dim_name);
        names.push_back(QString(dim_name));
    }

    retval = nc_close(ncid);
    if(retval != NC_NOERR)
    {
        LISEMS_ERROR("Could not close file " + path);
    }

    return names;
}

inline std::vector<int> GetVarDimSizes(QString path, QString variable)
{
    std::vector<int> sizes;

    int ncid = 0;
    int retval = nc_open(path.toStdString().c_str(), NC_NOWRITE, &ncid);
    if(retval != NC_NOERR)
    {
        LISEMS_ERROR("Could not read file " + path);
        return sizes;
    }

    int varid = 0;

    retval = nc_inq_varid(ncid,variable.toStdString().c_str(),&varid);
    if(retval != NC_NOERR)
    {
        LISEMS_ERROR("Could not read file " + path);
        retval = nc_close(ncid);
        if(retval != NC_NOERR)
        {
            LISEMS_ERROR("Could not close file " + path);
        }
        return sizes;
    }

    int xtype;
    int var_ndims = 0;
    int var_natts;
    int rh_dimids[NC_MAX_VAR_DIMS];
    size_t rh_dimlength[NC_MAX_VAR_DIMS];

    int ndims_max = 0;
    int var_ndimsmax = -1;
    char var_name[ NC_MAX_NAME + 1];
    bool found = false;
    //get properties of this variable
    nc_inq_var(ncid,varid,var_name,&xtype,&var_ndims,rh_dimids,&var_natts);
    if(retval != NC_NOERR)
    {
        LISEMS_ERROR("Could not read file " + path);
        retval = nc_close(ncid);
        if(retval != NC_NOERR)
        {
            LISEMS_ERROR("Could not close file " + path);
        }
        return sizes;
    }

    for(int i = 0; i < var_ndims; i++)
    {
        size_t diml = 0;
        nc_inq_dimlen(ncid,rh_dimids[i],&diml);
        sizes.push_back(diml);
    }

    retval = nc_close(ncid);
    if(retval != NC_NOERR)
    {
        LISEMS_ERROR("Could not close file " + path);
    }

    return sizes;


}

#endif // IONETCDF_H
