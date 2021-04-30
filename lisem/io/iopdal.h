#ifndef IOPDAL_H
#define IOPDAL_H


#include "geo/pointcloud/pointcloud.h"
#include "QFileInfo"

#include "pdal/PointTable.hpp"
#include "pdal/PointView.hpp"
#include "pdal/io/LasHeader.hpp"
#include "pdal/io/LasReader.hpp"
#include "pdal/Options.hpp"
#include "pdal/pdal.hpp"
#include "pdal/filters/DEMFilter.hpp"
#include "error.h"
#include "pdal/io/BufferReader.hpp"

using namespace pdal::Dimension;

inline static PointCloud * ReadPointCloud(QString filename)
{


    PointCloud * ret = new PointCloud();


    //set options and use a las-reader for now
    pdal::Option las_opt("filename", filename.toStdString());
    pdal::Options las_opts;
    las_opts.add(las_opt);
    pdal::PointTable table;
    pdal::LasReader las_reader;

    //do the actual work within a catch block (to catch the pdal_error)
    try {
        las_reader.setOptions(las_opts);
        las_reader.prepare(table);
        pdal::PointViewSet point_view_set = las_reader.execute(table);
        pdal::PointViewPtr point_view = *point_view_set.begin();
        pdal::Dimension::IdList dims = point_view->dims();
        pdal::LasHeader las_header = las_reader.header();


        std::string projstr = table.spatialReference().getWKT();

        double scale_x = las_header.scaleX();
        double scale_y = las_header.scaleY();
        double scale_z = las_header.scaleZ();

        double offset_x = las_header.offsetX();
        double offset_y = las_header.offsetY();
        double offset_z = las_header.offsetZ();

        double xmin = las_header.minX();
        double xmax = las_header.maxX();
        double ymin = las_header.minY();
        double ymax = las_header.maxY();

        unsigned int n_features = las_header.pointCount();


        bool have_time = las_header.hasTime();
        bool have_color = las_header.hasColor();

        ret->Resize(n_features);
        ret->SetHasColor(have_color);
        ret->SetHasTime(have_time);

        std::cout << "num_points " << n_features << std::endl;
        int i = 0;
        for (pdal::PointId idx = 0; idx < point_view->size(); ++idx) {


            double x = point_view->getFieldAs<double>(Id::X, idx);
            double y = point_view->getFieldAs<double>(Id::Y, idx);
            double z = point_view->getFieldAs<double>(Id::Z, idx);
            int return_no = point_view->getFieldAs<int>(Id::ReturnNumber, idx);
            int n_returns = point_view->getFieldAs<int>(Id::NumberOfReturns, idx);
            int point_class = point_view->getFieldAs<int>(Id::Classification, idx);
            float red = 0;
            float green = 0;
            float blue = 0;
            double time = 0.0;
            if(have_time)
            {
                time = point_view->getFieldAs<double>(Id::GpsTime, idx);
            }
            if(have_color)
            {
                red = ((float)point_view->getFieldAs<int>(Id::Red, idx))/255.0;
                green = ((float)point_view->getFieldAs<int>(Id::Green, idx))/255.0;
                blue = ((float)point_view->getFieldAs<int>(Id::Blue, idx))/255.0;

            }

            ret->SetPoint(i,x,y,z,time,red/255.0,green/255.0,blue/255.0,point_class,n_returns, return_no);
            i++;
        }

        ret->CalculateBoundingBox();

        ret->SetProjection(GeoProjection::FromString(QString(projstr.c_str())));

        return ret;

    } catch(pdal::pdal_error err)
    {
        LISEM_ERROR("error reading point cloud from file " + QString(err.what()));
        throw 1;
        return nullptr;
    }

}

inline pdal::PointTable * GetPdalTable(PointCloud * pc)
{
    pdal::PointTable *table = new pdal::PointTable();

    table->layout()->registerDim(pdal::Dimension::Id::X);
    table->layout()->registerDim(pdal::Dimension::Id::Y);
    table->layout()->registerDim(pdal::Dimension::Id::Z);
    if(pc->HasColor())
    {
        table->layout()->registerDim(pdal::Dimension::Id::Red);
            table->layout()->registerDim(pdal::Dimension::Id::Green);
            table->layout()->registerDim(pdal::Dimension::Id::Blue);
    }

    if(pc->HasTime())
    {
        table->layout()->registerDim(Id::GpsTime);
    }

    table->layout()->registerDim(Id::ReturnNumber);
    table->layout()->registerDim(Id::NumberOfReturns);
    table->layout()->registerDim(Id::Classification);


    return table;

}

inline std::shared_ptr<pdal::PointView> FillPdalView(pdal::PointTable * table, PointCloud * pc)
{


    std::shared_ptr<pdal::PointView> view(new pdal::PointView(*table));



    for(int i = 0; i < pc->GetPointCount(); i++)
    {
        PointCloudPoint p = pc->GetPoint(i);

        view->setField(pdal::Dimension::Id::X, i, p.m_x);
        view->setField(pdal::Dimension::Id::Y, i, p.m_y);
        view->setField(pdal::Dimension::Id::Z, i, p.m_z);

        if(pc->HasColor())
        {
            view->setField(Id::Red,pdal::Dimension::Type::Signed32,i,&p.m_r);
            view->setField(Id::Green,pdal::Dimension::Type::Signed32,i,&p.m_g);
            view->setField(Id::Blue,pdal::Dimension::Type::Signed32,i,&p.m_b);
        }

        if(pc->HasTime())
        {
            view->setField(Id::GpsTime,pdal::Dimension::Type::Double,i,&p.m_t);
        }

        view->setField(Id::ReturnNumber,pdal::Dimension::Type::Signed32,i,&p.m_returnn);
        view->setField(Id::NumberOfReturns,pdal::Dimension::Type::Signed32,i,&p.m_nreturns);
        view->setField(Id::Classification,pdal::Dimension::Type::Signed32,i,&p.m_class);
    }
    return view;

}
inline void WritePointCloud(PointCloud * pc, QString filename)
{

    QFileInfo fi(filename);
    if(!fi.dir().exists())
    {
        fi.dir().mkdir(fi.dir().path());
    }

    using namespace pdal;
    Options options;
    options.add("filename", filename.toStdString());

    pdal::PointTable * table = GetPdalTable(pc);
    std::shared_ptr<pdal::PointView> view = FillPdalView(table,pc);



    BufferReader reader;
    reader.addView(view);
    StageFactory factory;
    // Set second argument to 'true' to let factory take ownership of
    // stage and facilitate clean up.
    Stage *writer = factory.createStage("writers.las");

    writer->setInput(reader);
    writer->setOptions(options);
    writer->prepare(*table);
    writer->execute(*table);

    delete table;

}


#endif // IOPDAL_H
