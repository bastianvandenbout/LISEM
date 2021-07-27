#ifndef MARCHINGCUBES_H
#define MARCHINGCUBES_H

#include "defines.h"
#include "models/3dmodel.h"
#include "geo/raster/map.h"
#include "linear/lsm_vector3.h"
#include "error.h"

class ModelGeometry;
extern int edgeTable[256];
extern char triTable[256][16];

/*
   Linearly interpolate the position where an isosurface cuts
   an edge between two vertices, each with their own scalar value
*/
inline LSMVector3 VertexInterp(double isolevel,LSMVector3 p1,LSMVector3 p2,double valp1,double valp2)
{
   double mu;
   LSMVector3 p;

   if (ABS(isolevel-valp1) < 0.00001)
      return(p1);
   if (ABS(isolevel-valp2) < 0.00001)
      return(p2);
   if (ABS(valp1-valp2) < 0.00001)
      return(p1);
   mu = (isolevel - valp1) / (valp2 - valp1);
   p.x = p1.x + mu * (p2.x - p1.x);
   p.y = p1.y + mu * (p2.y - p1.y);
   p.z = p1.z + mu * (p2.z - p1.z);

   return(p);
}

typedef struct {
   LSMVector3 p[3];
} TRIANGLE;

typedef struct {
   LSMVector3 p[8];
   double val[8];
} GRIDCELL;



inline LISEM_API int Polygonise(GRIDCELL grid,double isolevel,TRIANGLE *triangles)
{
   int i,ntriang;
   int cubeindex;
   LSMVector3 vertlist[12];

   cubeindex = 0;
   if (grid.val[0] < isolevel) cubeindex |= 1;
   if (grid.val[1] < isolevel) cubeindex |= 2;
   if (grid.val[2] < isolevel) cubeindex |= 4;
   if (grid.val[3] < isolevel) cubeindex |= 8;
   if (grid.val[4] < isolevel) cubeindex |= 16;
   if (grid.val[5] < isolevel) cubeindex |= 32;
   if (grid.val[6] < isolevel) cubeindex |= 64;
   if (grid.val[7] < isolevel) cubeindex |= 128;


   if (edgeTable[cubeindex] == 0)
         return(0);

      /* Find the vertices where the surface intersects the cube */
      if (edgeTable[cubeindex] & 1)
         vertlist[0] =
            VertexInterp(isolevel,grid.p[0],grid.p[1],grid.val[0],grid.val[1]);
      if (edgeTable[cubeindex] & 2)
         vertlist[1] =
            VertexInterp(isolevel,grid.p[1],grid.p[2],grid.val[1],grid.val[2]);
      if (edgeTable[cubeindex] & 4)
         vertlist[2] =
            VertexInterp(isolevel,grid.p[2],grid.p[3],grid.val[2],grid.val[3]);
      if (edgeTable[cubeindex] & 8)
         vertlist[3] =
            VertexInterp(isolevel,grid.p[3],grid.p[0],grid.val[3],grid.val[0]);
      if (edgeTable[cubeindex] & 16)
         vertlist[4] =
            VertexInterp(isolevel,grid.p[4],grid.p[5],grid.val[4],grid.val[5]);
      if (edgeTable[cubeindex] & 32)
         vertlist[5] =
            VertexInterp(isolevel,grid.p[5],grid.p[6],grid.val[5],grid.val[6]);
      if (edgeTable[cubeindex] & 64)
         vertlist[6] =
            VertexInterp(isolevel,grid.p[6],grid.p[7],grid.val[6],grid.val[7]);
      if (edgeTable[cubeindex] & 128)
         vertlist[7] =
            VertexInterp(isolevel,grid.p[7],grid.p[4],grid.val[7],grid.val[4]);
      if (edgeTable[cubeindex] & 256)
         vertlist[8] =
            VertexInterp(isolevel,grid.p[0],grid.p[4],grid.val[0],grid.val[4]);
      if (edgeTable[cubeindex] & 512)
         vertlist[9] =
            VertexInterp(isolevel,grid.p[1],grid.p[5],grid.val[1],grid.val[5]);
      if (edgeTable[cubeindex] & 1024)
         vertlist[10] =
            VertexInterp(isolevel,grid.p[2],grid.p[6],grid.val[2],grid.val[6]);
      if (edgeTable[cubeindex] & 2048)
         vertlist[11] =
            VertexInterp(isolevel,grid.p[3],grid.p[7],grid.val[3],grid.val[7]);

      /* Create the triangle */
      ntriang = 0;
      for (i=0;triTable[cubeindex][i]!=-1;i+=3) {
         triangles[ntriang].p[0] = vertlist[triTable[cubeindex][i  ]];
         triangles[ntriang].p[1] = vertlist[triTable[cubeindex][i+1]];
         triangles[ntriang].p[2] = vertlist[triTable[cubeindex][i+2]];
         ntriang++;
      }

      return(ntriang);



}

inline LISEM_API ModelGeometry * MarchingCubes(std::vector<cTMap*> data, float z_top, float dz, float value)
{

    if(data.size() < 2)
    {
        LISEMS_ERROR("Can not do marchingcubes on field with only 1 or fewer vertical layers");
        throw 1;
    }
    std::vector<Vertex> g_vertex;
    std::vector<unsigned int> g_indices;

    ModelGeometry * m = new ModelGeometry();

    TRIANGLE triangles[9];
    float dx = data.at(0)->cellSizeX();
    float dy = data.at(0)->cellSizeY();

    int index = 0;
    static int cubeIndex;
    for(int i = 0; i < data.size()-1; i++)
    {
        for(int r = 0; r < data.at(i)->nrRows()-1; r++)
        {
            for(int c = 0; c < data.at(i)->nrCols()-1; c++)
            {

                float x = data.at(0)->west() +c * dx;
                float y = data.at(0)->north() + r * dy;
                float z = z_top + i * dz;

                GRIDCELL g;
                //create gridcell
                g.p[0] = LSMVector3(x,y,z);
                g.val[0] = data.at(i)->data[r][c];

                g.p[1] = LSMVector3(x+dx,y,z);
                g.val[1] = data.at(i)->data[r][c+1];

                g.p[2] = LSMVector3(x+dx,y+dy,z);
                g.val[2] = data.at(i)->data[r+1][c+1];

                g.p[3] = LSMVector3(x,y+dy,z);
                g.val[3] = data.at(i)->data[r+1][c];

                g.p[4] = LSMVector3(x,y,z+dz);
                g.val[4] = data.at(i+1)->data[r][c];

                g.p[5] = LSMVector3(x+dx,y,z+dz);
                g.val[5] = data.at(i+1)->data[r][c+1];

                g.p[6] = LSMVector3(x+dx,y+dy,z+dz);
                g.val[6] = data.at(i+1)->data[r+1][c+1];

                g.p[7] = LSMVector3(x,y+dy,z+dz);
                g.val[7] = data.at(i+1)->data[r+1][c];


                //get triangles

                int n_triangle = Polygonise(g,value,triangles);

                //push to mesh
                for(int k = 0; k < n_triangle; k++)
                {

                    LSMVector3 v0 = triangles[k].p[0];
                    LSMVector3 v1 = triangles[k].p[1];
                    LSMVector3 v2 = triangles[k].p[2];

                    LSMVector3 normal = LSMVector3::CrossProduct(v1-v0,v2-v0).Normalize();

                    Vertex ve0; ve0.setPosition(v0);ve0.setNormal(normal);
                    Vertex ve1; ve1.setPosition(v1);ve1.setNormal(normal);
                    Vertex ve2; ve2.setPosition(v2);ve2.setNormal(normal);

                    g_vertex.push_back(ve0);
                    g_vertex.push_back(ve1);
                    g_vertex.push_back(ve1);
                    g_indices.push_back(index +0);
                    g_indices.push_back(index +1);
                    g_indices.push_back(index +2);
                    index = index + 3;
                }
            }
        }
    }

    LSMMesh mesh = LSMMesh(g_vertex,g_indices);
    m->AddMesh(mesh);

    return m;
}





#endif // MARCHINGCUBES_H
