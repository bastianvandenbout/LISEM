#include "geo/raster/map.h"
#include "iopdal.h"
#include "pdal/filters/DEMFilter.hpp"
#include "pdal/filters/DelaunayFilter.hpp"
#include "boundingbox.h"
#include "geometry/sphtriangle.h"
#include "raster/rastercommon.h"

#include "geometry/delaunay.h"


using namespace pdal;


void Barycentric(LSMVector2 p, LSMVector2 a, LSMVector2 b, LSMVector2 c, float &u, float &v, float &w)
{
    LSMVector2 v0 = b - a, v1 = c - a, v2 = p - a;
    float d00 = v0.dot( v0);
    float d01 = v0.dot( v1);
    float d11 = v1.dot( v1);
    float d20 = v2.dot( v0);
    float d21 = v2.dot( v1);
    float denom = d00 * d11 - d01 * d01;
    v = (d11 * d20 - d01 * d21) / denom;
    w = (d00 * d21 - d01 * d20) / denom;
    u = 1.0f - v - w;
}

static cTMap * PointCloudToDEM(PointCloud * pc, float resolution)
{

    pdal::DelaunayFilter filter;

    pc->CalculateBoundingBox();
    BoundingBox b = pc->GetBoundingBox();

    if(resolution == 0)
    {
        LISEMS_ERROR("Can not rasterize to DEM with resolution of 0");
        throw 1;
    }
    int cols = b.GetSizeX()/resolution;
    int rows = b.GetSizeY()/resolution;

    MaskedRaster<float> raster_data(rows,cols, b.GetMaxY(), b.GetMinX(), resolution, -resolution);
    cTMap *map = new cTMap(std::move(raster_data),pc->GetProjection().GetWKT(),"");

    cTMap * m = map;
    MaskedRaster<float> raster_datan(rows,cols, b.GetMaxY(), b.GetMinX(), resolution, -resolution);
    cTMap *mapn = new cTMap(std::move(raster_datan),pc->GetProjection().GetWKT(),"");

    //check how many unique points we would have

    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            pcr::setMV(map->data[r][c]);
        }
    }
    int n3 = 0;
    for(int i = 0; i < pc->GetPointCount(); i++)
    {
        PointCloudPoint p = pc->GetPoint(i);

        int r = (p.m_y  - map->north())/map->cellSizeY();
        int c = (p.m_x - map->west())/map->cellSizeX();

        if(n3 ++ < 10000)
        {
        }

        if(pcr::isMV(map->data[r][c]))
        {
            if(n3  < 10000)
            {
            }
            map->data[r][c] = 0.0;
        }

        map->data[r][c] += p.m_z;
        mapn->data[r][c] += 1.0;

        if(n3  < 10000)
        {

        }
    }

    int n = 0;

    for(int r = 0; r < map->nrRows();r++)
    {
        for(int c = 0; c < map->nrCols();c++)
        {

            if(!pcr::isMV(map->data[r][c]))
            {
                n+= 1;
            }
        }
    }

    std::vector<LSMVector3> vertices;


    PointCloud * pcmin = new PointCloud();
    pcmin->Resize(n);
    int n2 = 0;
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {

            if(!pcr::isMV(map->data[r][c]))
            {
                if(mapn->data[r][c] > 0.0)
                {
                    pcmin->SetPoint(n2,map->west() + c * map->cellSizeX(),map->north() + r * map->cellSizeY(),map->data[r][c]/mapn->data[r][c],0.0,0.0,0.0,0.0,0,0,0);
                    vertices.push_back(LSMVector3(map->west() + c * map->cellSizeX(),map->north() + r * map->cellSizeY(),map->data[r][c]/mapn->data[r][c]));

                                           map->data[r][c] = map->data[r][c]/mapn->data[r][c];

                    n2++;
                }else {



                }
            }

        }
    }

    /*
    pdal::PointTable * table = GetPdalTable(pcmin);
    std::shared_ptr<pdal::PointView> view = FillPdalView(table,pcmin);
    BufferReader reader;
    reader.addView(view);
    filter.setInput(reader);
    pdal::PointTable table2;
    filter.prepare(table2);
    PointViewSet viewSet = filter.execute(table2);
    PointViewPtr view2 = *viewSet.begin();
    pdal::TriangularMesh *mesh = view2->mesh("delaunay2d");*/
    //put all the traingles on the actual DEM


    IDelaBella* idb = IDelaBella::Create();

    LSMVector3 *vdata = vertices.data();
    int verts = idb->Triangulate(vertices.size(), &vdata->x, &vdata->y, sizeof(LSMVector3));

    int tris = verts / 3;

        int n5 = 0;

        const DelaBella_Triangle* dela = idb->GetFirstDelaunayTriangle();


        for(std::size_t i = 0; i < tris; i++)
        {


        /*Triangle t = (*mesh)[i];
        double x1 = view->getFieldAs<double>(Id::X, t.m_a);
        double y1 = view->getFieldAs<double>(Id::Y, t.m_a);
        double z1 = view->getFieldAs<double>(Id::Z, t.m_a);
        double x2 = view->getFieldAs<double>(Id::X, t.m_b);
        double y2 = view->getFieldAs<double>(Id::Y, t.m_b);
        double z2 = view->getFieldAs<double>(Id::Z, t.m_b);
        double x3 = view->getFieldAs<double>(Id::X, t.m_c);
        double y3 = view->getFieldAs<double>(Id::Y, t.m_c);
        double z3 = view->getFieldAs<double>(Id::Z, t.m_c);*/


        double x1 = vertices[dela->v[0]->i].x;
        double y1 = vertices[dela->v[0]->i].y;
        double z1 = vertices[dela->v[0]->i].z;
        double x2 = vertices[dela->v[1]->i].x;
        double y2 = vertices[dela->v[1]->i].y;
        double z2 = vertices[dela->v[1]->i].z;
        double x3 = vertices[dela->v[2]->i].x;
        double y3 = vertices[dela->v[2]->i].y;
        double z3 = vertices[dela->v[2]->i].z;

        double xmin = std::min({x1,x2,x3});
        double xmax = std::max({x1,x2,x3});
        double ymin = std::min({y1,y2,y3});
        double ymax = std::max({y1,y2,y3});

        int cmin = static_cast<int>((xmin - m->west())/m->cellSizeX());
        int cmax = static_cast<int>((xmax - m->west())/m->cellSizeX());
        int rmin = static_cast<int>((ymin - m->north())/m->cellSizeY());
        int rmax = static_cast<int>((ymax - m->north())/m->cellSizeY());

        if(rmax < rmin)
        {
            int temp = rmin;
            rmin = rmax;
            rmax = temp;
        }

        if(cmax < cmin)
        {
            int temp = cmin;
            cmin = cmax;
            cmax = temp;
        }

        SPHTriangle *triangle = new SPHTriangle(x1,y1,x2,y2,x3,y3);
        SPHTriangle *triangle2 = new SPHTriangle(x2,y2,x1,y1,x3,y3);

        for(int r = rmin-1; r < rmax + 2; r++)
        {
            for(int c = cmin-1; c < cmax + 2; c++)
            {
                if(INSIDE(map,r,c))
                {
                    float celly = ((m->north() + float(r) * m->cellSizeY()));
                    float cellx = ((m->west() + float(c) * m->cellSizeX()));

                    if(triangle->Contains(LSMVector2(cellx,celly),m->cellSize()))
                    {
                        //get location in trangle coordinates

                        float u,v,w;
                        Barycentric(LSMVector2(cellx,celly),LSMVector2(x1,y1),LSMVector2(x2,y2),LSMVector2(x3,y3),u,v,w);
                        float z = u * z1 + v * z2 + w * z3;

                        if(pcr::isMV(z))
                        {
                            std::cout << "NAN " << std::endl;
                        }
                        map->data[r][c] = z;
                        n5++;
                    }else if(triangle2->Contains(LSMVector2(cellx,celly),m->cellSize()))
                    {
                        //get location in trangle coordinates

                        float u,v,w;
                        Barycentric(LSMVector2(cellx,celly),LSMVector2(x2,y2),LSMVector2(x1,y1),LSMVector2(x3,y3),u,v,w);
                        float z = u * z1 + v * z2 + w * z3;

                        if(pcr::isMV(z))
                        {
                            std::cout << "NAN2 " << std::endl;
                        }
                        map->data[r][c] = z;
                        n5++;
                    }
                }
            }
        }

        dela = dela->next;
    }

    std::cout << "done "<< n5 << std::endl;

    delete mapn;
    return map;

}


static cTMap * PointCloudRasterize(cTMap * m, PointCloud * p)
{








    return nullptr;

}
