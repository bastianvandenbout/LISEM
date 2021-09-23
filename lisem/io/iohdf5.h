#ifndef IOHDF5_H
#define IOHDF5_H

#include "geo/raster/map.h"
#include "geo/raster/field.h"
#include "H5Cpp.h"
#include "error.h"


inline void PrintAttribs(H5::H5Object &g)
{


    int attributecount = g.getNumAttrs();
    for(int i = 0; i < attributecount; i++)
    {
        H5::Attribute a = g.openAttribute(i);
        std::string aname = a.getName();
        H5T_class_t type_class = a.getTypeClass();

        LISEMS_STATUS("Attribute: " + QString(aname.c_str()) );
        if( type_class == H5T_INTEGER )
        {
           LISEMS_STATUS("Integer");
        }else if( type_class == H5T_FLOAT )
        {
           LISEMS_STATUS("Float");
        }else if( type_class == H5T_STRING )
        {
           LISEMS_STATUS("String");
        }else if(type_class == H5T_TIME)
        {
            LISEMS_STATUS("Time");

        }else
        {
           LISEMS_STATUS("Unsupported Type");
        }


    }

}


inline void PrintGroup(H5::Group &g, QString cumname = "")
{
    std::cout << "print group " << std::endl;

    std::vector<QString> names;

    PrintAttribs(g);

    std::cout << "attribs done " << std::endl;

    int ocount = g.getNumObjs();

     for(int i = 0; i < ocount; i++)
     {

         std::string name = g.getObjnameByIdx(i);
         H5G_obj_t type = g.getObjTypeByIdx(i);

         std::cout << "object " << i << " " << name <<std::endl;

         if(type == H5G_GROUP)		        /* Object is a group		*/
         {
             std::cout << "open group " << name << std::endl;
             H5::Group g2 = g.openGroup(name);//QString(cumname + "/"+ QString(name.c_str())).toStdString());
             std::cout << "opened group " << name << std::endl;
             LISEMS_STATUS("HDF-Group: " + QString(name.c_str()));
             PrintGroup(g2,cumname + "/" + QString(name.c_str()));
             std::cout << "printed group group " << name << std::endl;

         }else if(type == H5G_DATASET)         /* Object is a dataset		*/
         {
             H5::DataSet d = g.openDataSet(name);
             LISEMS_STATUS("HDF-Dataset: " + cumname + "/" + QString(name.c_str()));
             int rank = d.getSpace().getSimpleExtentNdims();
             std::vector<hsize_t> dims;dims.resize(rank);
             int ndims = d.getSpace().getSimpleExtentDims( dims.data(), NULL);

             LISEMS_STATUS("Rank: " + QString::number(rank));
             QString dimstring = "";
             for(int j = 0; j < ndims; j++)
             {
                dimstring += QString::number(dims[j]) ;
                if(j != ndims-1)
                {
                    dimstring +=  ",";
                }
             }
             LISEMS_STATUS("dims: " + dimstring);

             H5T_class_t type_class = d.getTypeClass();

             //H5T_FLOAT            = 1,   /*floating-point types                       */
             //H5T_TIME             = 2,   /*date and time types                        */
             //H5T_STRING           = 3,   /*character string types                     */
             //H5T_BITFIELD         = 4,   /*bit field types                            */
             //H5T_OPAQUE           = 5,   /*opaque types                               */
             //H5T_COMPOUND         = 6,   /*compound types                             */
             //H5T_REFERENCE        = 7,   /*reference types                            */
             //H5T_ENUM		 = 8,	/*enumeration types                          */
             //H5T_VLEN		 = 9,	/*Variable-Length types                      */
             //H5T_ARRAY	         = 10,

             if( type_class == H5T_INTEGER )
             {
                LISEMS_STATUS("Integer");
             }else if( type_class == H5T_FLOAT )
             {
                LISEMS_STATUS("Float");
             }else if( type_class == H5T_STRING )
             {
                LISEMS_STATUS("String");
             }else if(type_class == H5T_TIME)
             {
                 LISEMS_STATUS("Time");

             }else
             {
                LISEMS_STATUS("Unsupported Type");
             }

             PrintAttribs(d);

         }else if(type == H5G_TYPE)         /* Object is a dataset		*/
         {
             H5::DataType d = g.openDataType(name);
             LISEMS_STATUS("HDF-Datatype: " + QString(name.c_str()));


         }else if (type == H5G_LINK)
         {
             LISEMS_STATUS("HDF-Link: " + QString(name.c_str()));


             //dont do this yet
         }

     }
}

inline std::vector<QString> GetHDF5Datasets(QString filename)
{
    std::vector<QString> ret;

    return ret;

}

inline std::vector<int> GetHDF5VariableDims(QString filename, QString variable)
{

    std::vector<int> ret;

    return ret;

}

inline std::vector<QString> GetHDF5Attributes(QString filename, QString variable)
{
    std::vector<QString> ret;

    return ret;

}
inline std::vector<QString> GetHDF5Attribute(QString filename, QString variable, QString Attribute)
{
    std::vector<QString> ret;

    return ret;

}


inline cTMap * ReadHDF5Map(QString filename, QString variable)
{

    try
    {
        /*
          * Open the specified file and the specified dataset in the file.
          */
         H5::H5File file( filename.toStdString(), H5F_ACC_RDONLY );

         QStringList groups = variable.split("/");


         H5::Group g = file;

         for(int i = 0; i < groups.size()-1; i ++)
         {

             g = file.openGroup(groups.at(i).toStdString());

         }

         QString objname = groups.at(groups.size()-1);

         H5::DataSet dataset = g.openDataSet(objname.toStdString());
         H5::DataSpace dataspace = dataset.getSpace();
           /*
            * Get the number of dimensions in the dataspace.
            */
           int rank = dataspace.getSimpleExtentNdims();

           if(rank != 2)
           {
               LISEMS_ERROR("Dataspace is not 2D, could not open as Map");
               throw 1;
           }
           /*
            * Get the dimension size of each dimension in the dataspace and
            * display them.
            */
           hsize_t dims_out[2];
           int ndims = dataspace.getSimpleExtentDims( dims_out, NULL);


    /*
           * Define hyperslab in the dataset; implicitly giving strike and
           * block NULL.
           */
          hsize_t      offset[2];   // hyperslab offset in the file
          hsize_t      count[2];    // size of the hyperslab in the file
          offset[0] = 0;
          offset[1] = 0;
          count[0]  = dims_out[0];
          count[1]  = dims_out[1];
          dataspace.selectHyperslab( H5S_SELECT_SET, count, offset );

          /*
           * Define the memory dataspace.
           */
          hsize_t     dimsm[3];              /* memory space dimensions */
          dimsm[0] = dims_out[0];
          dimsm[1] = dims_out[1];
          dimsm[2] = 1;
          H5::DataSpace memspace( rank, dimsm );
          /*
           * Define memory hyperslab.
           */
          hsize_t      offset_out[3];   // hyperslab offset in memory
          hsize_t      count_out[3];    // size of the hyperslab in memory
          offset_out[0] = 0;
          offset_out[1] = 0;
          offset_out[2] = 0;
          count_out[0]  = dims_out[0];
          count_out[1]  = dims_out[1];
          count_out[2]  = 1;
          memspace.selectHyperslab( H5S_SELECT_SET, count_out, offset_out );

          /*
           * Read data from hyperslab in the file into the hyperslab in
           * memory and display the data.
           */
          MaskedRaster<float> raster_data(dims_out[1],dims_out[0], 0.0,0.0,1.0,1.0);
          cTMap *map = new cTMap(std::move(raster_data),"","");


          dataset.read( (map->data[0]), H5::PredType::NATIVE_FLOAT, memspace, dataspace );

          return map;

    }
    // catch failure caused by the H5File operations
    catch( H5::FileIException error )
    {

        LISEMS_ERROR("File exception from hdf5 " + QString(error.getFuncName().c_str()) + QString(error.getDetailMsg().c_str()));
        throw 1;
    }
    // catch failure caused by the DataSet operations
    catch( H5::DataSetIException error )
    {
        LISEMS_ERROR("DataSet exception from hdf5 " + QString(error.getFuncName().c_str()) + QString(error.getDetailMsg().c_str()));
        throw 1;

    //catch failure caused by group opening
    }catch( H5::GroupIException error )
    {
        LISEMS_ERROR("Group exception from hdf5 " + QString(error.getFuncName().c_str()) + QString(error.getDetailMsg().c_str()));
        throw 1;
    }
    // catch failure caused by the DataSpace operations
    catch( H5::DataSpaceIException error )
    {
        LISEMS_ERROR("DataSpace exception from hdf5 " + QString(error.getFuncName().c_str()) + QString(error.getDetailMsg().c_str()));
        throw 1;
    }
    // catch failure caused by the DataSpace operations
    catch( H5::DataTypeIException error )
    {
        LISEMS_ERROR("DataType exception from hdf5 " + QString(error.getFuncName().c_str()) + QString(error.getDetailMsg().c_str()));
        throw 1;
    }catch( H5::Exception error)
    {
        LISEMS_ERROR("Unknown exception from hdf5 " + QString(error.getFuncName().c_str()) + QString(error.getDetailMsg().c_str()));
        throw 1;

    }



    return nullptr;
}



inline Field * ReadHDF5Field(QString filename, QString variable)
{

    return nullptr;
}

inline cTMap * WriteHDF5Map(QString filename, QString variable)
{

    return nullptr;
}



inline Field * WriteHDF5Field(QString filename, QString variable)
{

    return nullptr;
}




inline void HDF5InfoPrint(QString filename)
{

    try
    {
        /*
          * Open the specified file and the specified dataset in the file.
          */
         H5::H5File file( filename.toStdString(), H5F_ACC_RDONLY );

         LISEM_STATUS("HDF-File: " + filename);
         PrintGroup(file);



    }
    // catch failure caused by the H5File operations
    catch( H5::FileIException error )
    {

       LISEMS_ERROR("File exception from hdf5 " + QString(error.getFuncName().c_str()) + QString(error.getDetailMsg().c_str()));
    }
    // catch failure caused by the DataSet operations
    catch( H5::DataSetIException error )
    {
       LISEMS_ERROR("DataSet exception from hdf5 " + QString(error.getFuncName().c_str()) + QString(error.getDetailMsg().c_str()));

    //catch failure caused by group opening
    }catch( H5::GroupIException error )
    {
        LISEMS_ERROR("Group exception from hdf5 " + QString(error.getFuncName().c_str()) + QString(error.getDetailMsg().c_str()));

    }
    // catch failure caused by the DataSpace operations
    catch( H5::DataSpaceIException error )
    {
       LISEMS_ERROR("DataSpace exception from hdf5 " + QString(error.getFuncName().c_str()) + QString(error.getDetailMsg().c_str()));
    }
    // catch failure caused by the DataSpace operations
    catch( H5::DataTypeIException error )
    {
       LISEMS_ERROR("DataType exception from hdf5 " + QString(error.getFuncName().c_str()) + QString(error.getDetailMsg().c_str()));
    }catch( H5::Exception error)
    {
        LISEMS_ERROR("Unknown exception from hdf5 " + QString(error.getFuncName().c_str()) + QString(error.getDetailMsg().c_str()));

    }


}










#endif // IOHDF5_H
