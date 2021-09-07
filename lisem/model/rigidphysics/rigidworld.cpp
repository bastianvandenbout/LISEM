#include "rigidworld.h"
#include "geometry/triangleintersect.h"
#include "defines.h"

void RigidPhysicsWorld::CopyFrom(RigidPhysicsWorld * w)
{
    std::cout << "copy from start "<< std::endl;
    //create a new system

    m_system.Set_G_acc(w->m_system.Get_G_acc());


    //copy the objects

    for(int i = 0; i < w->m_Objects.length(); i++)
    {
        std::cout << "object i " << i << std::endl;
        RigidPhysicsObject * obj =w->m_Objects.at(i)->GetCopy();
        std::cout << "copied object, adding"<< std::endl;
        this->AddObject(obj);
        if(w->m_Objects.at(i)->m_IsTerrain && m_HasTerrain == false)
        {
            std::cout << "copy dem" <<std::endl;
            //m_DEM = w->m_DEM->GetCopy();
            m_HasTerrain = true;
        }else if(w->m_Objects.at(i)->m_IsTerrain)
        {
            m_DEMObjects.append(obj);
        }


    }

                 std::cout <<" copy from end "<< std::endl;
}



void RigidPhysicsWorld::SetTerrain(cTMap * DEM)
{
    //if(!m_HasChanges)
    {
        m_DEM = DEM;
        m_DEMObjects =RigidPhysicsObject::RigidPhysicsObject_AsHeightField(DEM);
        for(int i = 0; i < m_DEMObjects.length(); i++)
        {
            AddObject(m_DEMObjects.at(i));
        }

        m_HasTerrain = true;
    }

}

int RigidPhysicsWorld::GetObjectCount( bool has_mutex )
{
    if(!has_mutex)
    {
        m_Mutex.lock();
        m_Mutex_internal.lock();
    }
    int objects = m_Objects.length();

    if(!has_mutex)
    {
        m_Mutex_internal.unlock();
        m_Mutex.unlock();
    }

    return objects;

}

void RigidPhysicsWorld::AddObject(RigidPhysicsObject*obj , bool has_mutex, bool add_geo )
{
    if(!has_mutex)
    {
        m_Mutex.lock();
        m_Mutex_internal.lock();
    }
    if(obj->AS_IsScript())
    {
        obj->AS_AddRef();
    }
    LSMVector2 geo = this->GetSimpleGeoOrigin();
    if(add_geo && !obj->IsTerrain())
    {
        LSMVector3 pos = obj->GetPosition();
        LSMVector3 posn = LSMVector3(pos.x - geo.x,pos.y,pos.z -geo.y);
        obj->m_chBody->SetPos(ChVector<double>(posn.x,posn.y,posn.z));

    }
    m_Objects.append(obj);
    m_system.AddBody(obj->GetChBody());

    m_HasChanges = true;
    if(!has_mutex)
    {
        m_Mutex_internal.unlock();
        m_Mutex.unlock();
    }

}

void RigidPhysicsWorld::RemoveObject(RigidPhysicsObject*obj, bool has_mutex)
{
    if(!has_mutex)
    {
        m_Mutex.lock();
        m_Mutex_internal.lock();
    }
    if(obj->AS_IsScript())
    {
        obj->AS_ReleaseRef();
    }
    m_system.RemoveBody(obj->GetChBody());
    m_HasChanges = true;
    if(!has_mutex)
    {
        m_Mutex_internal.unlock();
        m_Mutex.unlock();
    }

}

void RigidPhysicsWorld::RunSingleStep(double dt, double tmodel)
{
    dt = dt;

    double t = 0.0;
    int iter = 0;
    while(t < dt && iter < 1620)
    {
        //std::cout <<" run rigid step " << std::endl;

        m_Mutex.lock();

        //std::cout <<" 1" << std::endl;

        float dtreal = 0.01;

         //do interactions with fluid
         if(m_HasTerrain && m_HF != nullptr)
         {
             std::cout << "make maps, there is flow "<< std::endl;
             //make sure we have all relevant maps in the right dimensionality

             if(m_BlockX == nullptr || !(m_BlockX->nrCols() == m_DEM->nrCols() && m_BlockX->nrRows() == m_DEM->nrRows()))
             {
                 if(m_BlockX != nullptr)
                 {
                     delete m_BlockX;
                 }
                 m_BlockX = new cTMap();
                 m_BlockX->MakeMap(m_DEM,0.0);
             }
             if(m_BlockY == nullptr || !(m_BlockY->nrCols() == m_DEM->nrCols() && m_BlockY->nrRows() == m_DEM->nrRows()))
             {
                 if(m_BlockY != nullptr)
                 {
                     delete m_BlockY;
                 }
                 m_BlockY = new cTMap();
                 m_BlockY->MakeMap(m_DEM,0.0);
             }
             if(m_BlockFX == nullptr || !(m_BlockFX->nrCols() == m_DEM->nrCols() && m_BlockFX->nrRows() == m_DEM->nrRows()))
             {
                 if(m_BlockFX != nullptr)
                 {
                     delete m_BlockFX;
                 }
                 m_BlockFX = new cTMap();
                 m_BlockFX->MakeMap(m_DEM,0.0);
             }
             if(m_BlockFY == nullptr || !(m_BlockFY->nrCols() == m_DEM->nrCols() && m_BlockFY->nrRows() == m_DEM->nrRows()))
             {
                 if(m_BlockFY != nullptr)
                 {
                     delete m_BlockFY;
                 }
                 m_BlockFY = new cTMap();
                 m_BlockFY->MakeMap(m_DEM,0.0);
             }
             if(m_BlockCaptureX == nullptr || !(m_BlockCaptureX->nrCols() == m_DEM->nrCols() && m_BlockCaptureX->nrRows() == m_DEM->nrRows()))
             {
                 if(m_BlockCaptureX != nullptr)
                 {
                     delete m_BlockCaptureX;
                 }
                 m_BlockCaptureX = new cTMap();
                 m_BlockCaptureX->MakeMap(m_DEM,0.0);
             }
             if(m_BlockCaptureY == nullptr || !(m_BlockCaptureY->nrCols() == m_DEM->nrCols() && m_BlockCaptureY->nrRows() == m_DEM->nrRows()))
             {
                 if(m_BlockCaptureY != nullptr)
                 {
                     delete m_BlockCaptureY;
                 }
                 m_BlockCaptureY = new cTMap();
                 m_BlockCaptureY->MakeMap(m_DEM,0.0);
             }


             if(m_BlockLDD == nullptr || !(m_BlockLDD->nrCols() == m_DEM->nrCols() && m_BlockLDD->nrRows() == m_DEM->nrRows()))
             {
                 if(m_BlockLDD != nullptr)
                 {
                     delete m_BlockLDD;
                 }
                 m_BlockLDD = new cTMap();
                 m_BlockLDD->MakeMap(m_DEM,0.0);
             }
             if(m_HCorrected == nullptr || !(m_HCorrected->nrCols() == m_DEM->nrCols() && m_HCorrected->nrRows() == m_DEM->nrRows()))
             {
                 if(m_HCorrected != nullptr)
                 {
                     delete m_HCorrected;
                 }
                 m_HCorrected = new cTMap();
                 m_HCorrected->MakeMap(m_DEM,0.0);
             }

             if(m_BlockLX1 == nullptr || !(m_BlockLX1->nrCols() == m_DEM->nrCols() && m_BlockLX1->nrRows() == m_DEM->nrRows()))
             {
                 if(m_BlockLX1 != nullptr)
                 {
                     delete m_BlockLX1;
                 }
                 m_BlockLX1 = new cTMap();
                 m_BlockLX1->MakeMap(m_DEM,0.0);
             }
             if(m_BlockLY1 == nullptr || !(m_BlockLY1->nrCols() == m_DEM->nrCols() && m_BlockLY1->nrRows() == m_DEM->nrRows()))
             {
                 if(m_BlockLY1 != nullptr)
                 {
                     delete m_BlockLY1;
                 }
                 m_BlockLY1 = new cTMap();
                 m_BlockLY1->MakeMap(m_DEM,0.0);
             }
             if(m_BlockLX2 == nullptr || !(m_BlockLX2->nrCols() == m_DEM->nrCols() && m_BlockLX2->nrRows() == m_DEM->nrRows()))
             {
                 if(m_BlockLX2 != nullptr)
                 {
                     delete m_BlockLX2;
                 }
                 m_BlockLX2 = new cTMap();
                 m_BlockLX2->MakeMap(m_DEM,0.0);
             }
             if(m_BlockLY2 == nullptr || !(m_BlockLY2->nrCols() == m_DEM->nrCols() && m_BlockLY2->nrRows() == m_DEM->nrRows()))
             {
                 if(m_BlockLY2 != nullptr)
                 {
                     delete m_BlockLY2;
                 }
                 m_BlockLY2 = new cTMap();
                 m_BlockLY2->MakeMap(m_DEM,0.0);
             }

             //velocity-projected surface area
        //std::cout <<" 3" << std::endl;

             //submerged volume
             FOR_ROW_COL_MV(m_DEM)
             {
                 m_BlockFX->data[r][c] = 0.0;
                 m_BlockFY->data[r][c] = 0.0;

                 double capture_x_total = 0.0;
                 double capture_y_total = 0.0;
                 double capture_x_wtotal = 0.0;
                 double capture_y_wtotal = 0.0;


                 float h = m_DEM->data[r][c];
                 float hf = m_HF->data[r][c];

                 double csx = m_DEM->cellSizeX();
                 double csy = m_DEM->cellSizeY();

                 float hs = ((m_HS == nullptr)? 0.0:m_HS->data[r][c]);

                 float dhdx = cTMap::DER_X(m_DEM,r,c)/csx;
                 float dhdy = cTMap::DER_Y(m_DEM,r,c)/csy;
                 float dhfdx = cTMap::DER_X(m_HF,r,c)/csx;
                 float dhfdy = cTMap::DER_Y(m_HF,r,c)/csy;
                 float dhsdx = cTMap::DER_X(m_HS,r,c)/csx;
                 float dhsdy = cTMap::DER_Y(m_HS,r,c)/csy;
                 float surfacetop = h+ hf +hs
                         + 0.5 * std::fabs(csy) * dhdy
                         +  0.5 * std::fabs(csx) * dhdx
                         + 0.5 * std::fabs(csy) * dhsdy
                         +  0.5 * std::fabs(csx) * dhsdx
                         + 0.5 * std::fabs(csy) * dhfdy
                         +  0.5 * std::fabs(csx) * dhfdx;


                 BoundingBox extenthor((float)(c) * csx, (float)(c+1) * csx, (float)(r) * csy, (float)(r+1) * csy) ;
                 //get a list of all objects for this pixel

                 double ymax = 0.0;
                 double ymin = 0.0;
                 bool first = false;
                 bool first2 = false;
                 std::vector<ObjAABB> objs;
                 for(int i = 0; i < m_Objects.length(); i++)
                 {
                     RigidPhysicsObject * obj = m_Objects.at(i);
                     if(!obj->IsTerrain())
                     {
                         BoundingBox3D extent = obj->GetAABB();
                         double csx = m_DEM->cellSizeX();
                         double csy = m_DEM->cellSizeY();

                         BoundingBox extenthor((float)(c) * csx, (float)(c+1) * csx, (float)(r) * csy, (float)(r+1) * csy) ;
                         if(extenthor.Overlaps(extent))
                         {
                            double my = obj->GetAABB().GetMaxY();
                            double sy = obj->GetAABB().GetMinY();
                            if(my > ymax || first)
                            {
                                first = false;
                                ymax = my;
                            }
                            if(sy < ymin || first2)
                            {
                                first2 = false;
                                ymin = sy;
                            }
                            objs.push_back(ObjAABB(obj,obj->GetAABB()));

                            std::vector<double> ymax;
                         }
                     }

                 }
                 if(objs.size()> 0)
                 {
                     //std::cout << "found 1" << std::endl;

                     //get total objs volumes

                     double area_proj_x_total = 0.0;
                     double area_proj_z_total = 0.0;

                     double concavity = 0.0;

                     for(int i = 0; i < objs.size(); i++)
                     {

                         bool firstpoint = true;
                         BoundingBox3D bb3dconfined;


                         BoundingBox3D extent3d((float)(c) * csx, (float)(c+1) * csx,ymin, ymax, (float)(r) * csy, (float)(r+1) * csy) ;
                         objs.at(i).aabb_confined = objs.at(i).aabb;
                         objs.at(i).aabb_confined.And(extent3d);

                         double vol_total = 0.0;

                         RigidPhysicsObject * obj = objs.at(i).obj;

                         ModelGeometry * model = obj->GetModel();
                         QList<LSMMesh *> meshes = model->GetMeshes();

                         LSMVector3 objpos = obj->GetPosition();

                         LSMVector2 offset = GetSimpleGeoOrigin();
                         LSMMatrix4x4 objtrans;
                         LSMMatrix4x4 objrotm = obj->GetRotationMatrix();
                         objtrans.Translate(LSMVector3(objpos.x,objpos.y,objpos.z));
                         objrotm = objtrans*objrotm;

                         for(int k = 0; k < meshes.length(); k++)
                         {
                             LSMMesh * mesh = meshes.at(k);
                             int triangleCount = mesh->indices.size()/3;
                             for (unsigned long l = 0; l <  triangleCount; l++)
                             {
                                 LSMVector3 relpos_A = mesh->vertices[mesh->indices[l * 3 + 0]].position();
                                 LSMVector3 relpos_B = mesh->vertices[mesh->indices[l * 3 + 1]].position();
                                 LSMVector3 relpos_C = mesh->vertices[mesh->indices[l * 3 + 2]].position();

                                 //we have to convert this to global world space
                                 LSMVector3 Atmp = objrotm *relpos_A;
                                 LSMVector3 Btmp = objrotm *relpos_B;
                                 LSMVector3 Ctmp = objrotm *relpos_C;
                                 //finally, get the sub-surface properties
                                 double vol;
                                 double area_proj;
                                 double proj_area_vel;
                                 LSMVector3 com;
                                 LSMVector3 hydro_cof;
                                 double hydro_total;
                                 std::vector<LSMVector3> points;
                                 double area_this =TriangleWaterFlowIntersect(Btmp,Atmp,Ctmp,extenthor,ymax - h,0.0,0.0,h,0.0,0.0,vol,area_proj,com,LSMVector3(0.0,0.0,0.0),proj_area_vel, hydro_cof,hydro_total,points);//);
                                 vol_total += vol;

                                 for(int o = 0; o < points.size(); o++)
                                 {
                                     LSMVector3 po = points.at(o);
                                     if(po.y < h)
                                     {
                                         po.y = h;
                                     }
                                     if(firstpoint)
                                     {
                                        firstpoint = false;
                                        bb3dconfined.Set(po);
                                     }else
                                     {
                                        bb3dconfined.Merge(po);
                                     }
                                 }
                             }
                         }

                         //get obj to AABB ratio
                         objs.at(i).aabb_confined = bb3dconfined;
                         objs.at(i).volume_per_unit_height = vol_total / std::max(1e-10,objs.at(i).aabb_confined.GetVolume());

                     }


                     //std::cout << "found 2 " << std::endl;

                     //sort based on minimum y and maximum y

                     std::vector<ObjAABB> objsrev = objs;

                     std::sort(objs.begin(),objs.end(),[](ObjAABB a,ObjAABB b){return a.aabb.GetMinY() < b.aabb.GetMinY();});
                     std::sort(objsrev.begin(),objsrev.end(),[](ObjAABB a,ObjAABB b){return a.aabb.GetMaxY() < b.aabb.GetMaxY();});

                     //run vertically increasing algorithm to distribute water

                     //total water volume
                     double vol_left = std::fabs(csx) * std::fabs(csy) * hf;
                     double area_cell = std::fabs(csx) * std::fabs(csy);

                     double h_final = 0.0;
                     int index_min = 0;
                     int index_max = 0;
                     double y_last = h;
                     double block_av_y = 0.0;
                     double fraction_space = 1.0;
                     double fraction_space_vert_average;
                     int n_obj_vert = 0;
                     for(int i = 0; i < objs.size() + objsrev.size(); i++)
                     {
                         bool minavailable = false;
                         if(index_min < objs.size())
                         {
                            minavailable = true;
                         }
                         bool maxavailable = false;
                         if(index_max < objsrev.size())
                         {
                            maxavailable = true;
                         }

                         bool min_is_smaller = false;
                         if(minavailable && maxavailable)
                         {
                            min_is_smaller = objs.at(index_min).aabb_confined.GetMinY() < objsrev.at(index_max).aabb_confined.GetMaxY();
                         }

                         if(!minavailable && !maxavailable)
                         {
                            break;
                         }


                         double y_new;
                         double fraction_space_new;
                         int n_obj_vert_new = n_obj_vert;
                         if(
                                 (minavailable && maxavailable && min_is_smaller) ||
                                 (minavailable && !maxavailable)
                           )
                         {
                             //closest object boundary in vertical space is the beginning of a new object
                                fraction_space_new = fraction_space - objs.at(index_min).volume_per_unit_height * objs.at(index_min).aabb_confined.GetSizeX() * objs.at(index_min).aabb_confined.GetSizeZ()/area_cell;
                                y_new =objs.at(index_min).aabb_confined.GetMinY();
                                index_min ++;
                                n_obj_vert_new++;
                         }else
                         {
                             //closest object boundary in vertical space is the ending of an object
                             fraction_space_new = fraction_space + objsrev.at(index_max).volume_per_unit_height * objsrev.at(index_max).aabb_confined.GetSizeX() * objsrev.at(index_max).aabb_confined.GetSizeZ()/area_cell;
                             y_new =objsrev.at(index_max).aabb_confined.GetMaxY();
                             index_max ++;
                             n_obj_vert_new--;
                         }

                         double v_dif = std::max(0.0,y_new - y_last);
                         double vol_remove = v_dif *  area_cell * std::max(0.0,fraction_space);
                         fraction_space_vert_average += fraction_space * v_dif;
                         if(n_obj_vert == 0)
                         {
                             block_av_y += v_dif;
                         }
                         n_obj_vert = n_obj_vert_new;
                         if(!(vol_left > 0.0))
                         {
                             h_final = y_last;
                             break;
                         }
                         if(vol_left > vol_remove)
                         {
                            //this means we can calculate the final water height
                            h_final = y_last + v_dif;
                            vol_left -= vol_remove;
                            y_last = y_new;

                         }else
                         {
                            h_final = y_last + v_dif * vol_left/std::max(0.000001,vol_remove);
                            y_last = y_last +  v_dif * vol_left/std::max(0.000001,vol_remove);
                            vol_left =0.0;
                            break;
                         }
                         fraction_space = fraction_space_new;
                     }


                     //add remainder
                     if(vol_left > 0.0)
                     {
                        double h_add = vol_left/area_cell;
                        h_final = objsrev.at(objsrev.size()-1).aabb.GetMaxY() + h_add;
                        fraction_space_vert_average  += h_add * 1.0;
                        block_av_y +=h_add;
                     }
                     h_final = h_final - h;

                     if(h_final > 1e-7)
                     {
                         fraction_space_vert_average = fraction_space_vert_average /h_final;
                         block_av_y = 1.0 - block_av_y/h_final;
                     }else
                     {
                         fraction_space_vert_average = 1.0;
                         block_av_y = 0.0;
                     }


                     double area_proj_x = 0.0;
                     double area_proj_z = 0.0;
                     double area_total = 0.0;


                     double lpx = 0.0;
                     double lpz = 0.0;
                     double lpx_last = 0.0;
                     double lpz_last = 0.0;
                     double weight = 0.0;
                     double lmx = 0.0;
                     double lmz = 0.0;

                     //std::cout << "found 3" << std::endl;

                     for(int i = 0; i < objs.size(); i++)
                     {
                         BoundingBox3D extent3d((float)(c) * csx, (float)(c+1) * csx,ymin, ymax, (float)(r) * csy, (float)(r+1) * csy) ;

                         double vol_total = 0.0;
                         double capture_x = 0.0;
                         double capture_y = 0.0;
                         double capture_wx = 0.0;
                         double capture_wy = 0.0;
                         RigidPhysicsObject * obj = objs.at(i).obj;

                         ModelGeometry * model = obj->GetModel();
                         QList<LSMMesh *> meshes = model->GetMeshes();

                         LSMVector3 objpos = obj->GetPosition();

                         LSMVector2 offset = GetSimpleGeoOrigin();
                         LSMMatrix4x4 objtrans;
                         LSMMatrix4x4 objrotm = obj->GetRotationMatrix();
                         objtrans.Translate(LSMVector3(objpos.x,objpos.y,objpos.z));
                         objrotm = objtrans*objrotm;

                         LSMVector2 objline_total;
                         LSMVector3 objline_ctotal;

                         LSMVector3 center_bound_x1;
                         LSMVector3 center_bound_x2;
                         LSMVector3 center_bound_y1;
                         LSMVector3 center_bound_y2;
                         double area_bound_x1 = 0.0;
                         double area_bound_x2 = 0.0;
                         double area_bound_y1 = 0.0;
                         double area_bound_y2 = 0.0;


                         for(int k = 0; k < meshes.length(); k++)
                         {
                             LSMMesh * mesh = meshes.at(k);
                             int triangleCount = mesh->indices.size()/3;
                             for (unsigned long l = 0; l <  triangleCount; l++)
                             {
                                 LSMVector3 relpos_A = mesh->vertices[mesh->indices[l * 3 + 0]].position();
                                 LSMVector3 relpos_B = mesh->vertices[mesh->indices[l * 3 + 1]].position();
                                 LSMVector3 relpos_C = mesh->vertices[mesh->indices[l * 3 + 2]].position();

                                 //we have to convert this to global world space
                                 LSMVector3 Atmp = objrotm *relpos_A;
                                 LSMVector3 Btmp = objrotm *relpos_B;
                                 LSMVector3 Ctmp = objrotm *relpos_C;

                                 LSMVector3 FLoc = (Atmp + Btmp + Ctmp)*(1.0/3.0);
                                 LSMVector3 FLoc_loc= (relpos_A + relpos_B + relpos_C)*(1.0/3.0);

                                 //get velocity and angular velocity
                                 ChVector<double> rot = obj->m_chBody->GetWvel_par();
                                 ChVector<double> vel = obj->m_chBody->GetPos_dt();


                                 LSMVector3 axis = LSMVector3(rot[0],rot[1],rot[2]);
                                 LSMVector3 velo = LSMVector3(vel[0],vel[1],vel[2]);


                                 ChVector<double> vel_abs_A = obj->m_chBody->PointSpeedLocalToParent(ChVector<double>(relpos_A.x,relpos_A.y,relpos_A.z));
                                 ChVector<double> vel_abs_B = obj->m_chBody->PointSpeedLocalToParent(ChVector<double>(relpos_B.x,relpos_B.y,relpos_B.z));
                                 ChVector<double> vel_abs_C = obj->m_chBody->PointSpeedLocalToParent(ChVector<double>(relpos_C.x,relpos_C.y,relpos_C.z));
                                 ChVector<double> vel_abs_F = obj->m_chBody->PointSpeedLocalToParent(ChVector<double>(FLoc_loc.x,FLoc_loc.y,FLoc_loc.z));

                                 LSMVector3 vel_FLoc = velo + LSMVector3::CrossProduct(axis,FLoc - objpos);//*DistancePointLine(mesh->vertices[mesh->indices[i * 3 + 2]].position(),LSMVector3(0.0,0.0,0.0),axis);
                                 LSMVector3 vel_FLoc_NoRot = velo;//*DistancePointLine(mesh->vertices[mesh->indices[i * 3 + 2]].position(),LSMVector3(0.0,0.0,0.0),axis);


                                 //finally, get the sub-surface properties
                                 double vol;
                                 double area_proj;
                                 double proj_area_vel;
                                 LSMVector3 com;
                                 LSMVector3 hydro_cof;
                                 double hydro_total;
                                 std::vector<LSMVector3> points;
                                 double area_this =TriangleWaterFlowIntersect(Btmp,Atmp,Ctmp,extenthor,h_final,0.0,0.0,h,0.0,0.0,vol,area_proj,com,LSMVector3(0.0,0.0,0.0),proj_area_vel, hydro_cof,hydro_total,points);//);
                                 vol_total += vol;
                                 area_total += area_this;

                                 LSMVector3 Normal = LSMVector3::CrossProduct(Atmp-Btmp,Ctmp-Btmp).Normalize();
                                 bool facing_up = Normal.dot(LSMVector3(0.0,1.0,0.0)) > 0;
                                 area_proj_x += 0.5 * area_this*std::fabs(Normal.dot(LSMVector3(1.0,0.0,0.0)));
                                 area_proj_z += 0.5 * area_this*std::fabs(Normal.dot(LSMVector3(0.0,0.0,1.0)));

                                 capture_x += 0.5 * area_this*std::max(0.0,Normal.dot(LSMVector3(1.0,0.0,0.0))) * vel_FLoc.x;
                                 capture_y += 0.5 * area_this*std::max(0.0,Normal.dot(LSMVector3(0.0,0.0,1.0))) * vel_FLoc.z;
                                 capture_wx += 0.5 * area_this*std::max(0.0,Normal.dot(LSMVector3(1.0,0.0,0.0)));
                                 capture_wy += 0.5 * area_this*std::max(0.0,Normal.dot(LSMVector3(0.0,0.0,1.0)));

                                 //std::cout << "found 6" << std::endl;
                                 for(int o = 0; o < points.size(); o++)
                                 {
                                     //std::cout << o<< "  " << points.size() << std::endl;
                                     LSMVector3 po1 = points.at(o);
                                     LSMVector3 po2 = (o == (points.size() -1))?points.at(0):points.at(o+1);

                                     //check wether these points lie on a boundary
                                     if(std::fabs(po1.x - extenthor.GetMinX()) < 1e-6 && std::fabs(po2.x - extenthor.GetMinX()) < 1e-6)
                                     {
                                         double po1y = h + dhdx *(extenthor.GetMinX()-extenthor.GetCenterX()) + dhdy *((po1.z)-extenthor.GetCenterY());
                                         double po2y = h + dhdx *(extenthor.GetMinX()-extenthor.GetCenterX()) + dhdy *((po2.z)-extenthor.GetCenterY());
                                         double area_bound_this = (facing_up?-1.0:1.0)*(0.5 *(po1y + po2y) - 0.5*(po1.y + po2.y)) *std::fabs(po1.z - po2.z);
                                         area_bound_x1 += area_bound_this;
                                         area_total += area_bound_this;
                                         area_proj_z += std::fabs(0.5*area_bound_this);
                                         center_bound_x1 = center_bound_x1 + LSMVector3(extenthor.GetMinX(),0.25 *(po1y + po2y) + 0.25*(po1.y + po2.y),(po1.z * std::fabs(po1y- po1.y)+ po2.z * std::fabs(po2y- po2.y))/(std::max(1e-6,std::fabs(po1y- po1.y)+std::fabs(po2y- po2.y))) )*area_bound_this;
                                     }
                                     if(std::fabs(po1.x - extenthor.GetMaxX()) < 1e-6 && std::fabs(po2.x - extenthor.GetMaxX()) < 1e-6)
                                     {
                                         double po1y = h + dhdx *(extenthor.GetMaxX()-extenthor.GetCenterX()) + dhdy *((po1.z)-extenthor.GetCenterY());
                                         double po2y = h + dhdx *(extenthor.GetMaxX()-extenthor.GetCenterX()) + dhdy *((po2.z)-extenthor.GetCenterY());
                                         double area_bound_this = (facing_up?-1.0:1.0)*(0.5 *(po1y + po2y) - 0.5*(po1.y + po2.y)) *std::fabs(po1.z - po2.z);
                                         area_bound_x2 += area_bound_this;
                                         area_total += area_bound_this;
                                         area_proj_z += std::fabs(0.5*area_bound_this);
                                         center_bound_x2 = center_bound_x2 + LSMVector3(extenthor.GetMaxX(),0.25 *(po1y + po2y) + 0.25*(po1.y + po2.y),(po1.z * std::fabs(po1y- po1.y)+ po2.z * std::fabs(po2y- po2.y))/(std::max(1e-6,std::fabs(po1y- po1.y)+std::fabs(po2y- po2.y))) )*area_bound_this;
                                     }
                                     if(std::fabs(po1.z - extenthor.GetMinY()) < 1e-6 && std::fabs(po2.z - extenthor.GetMinY()) < 1e-6)
                                     {
                                         double po1y = h + dhdy *(extenthor.GetMinY()-extenthor.GetCenterY()) + dhdx *((po1.x)-extenthor.GetCenterX());
                                         double po2y = h + dhdy *(extenthor.GetMinY()-extenthor.GetCenterY()) + dhdx *((po2.x)-extenthor.GetCenterX());
                                         double area_bound_this = (facing_up?-1.0:1.0)*(0.5 *(po1y + po2y) - 0.5*(po1.y + po2.y)) *std::fabs(po1.x - po2.x);
                                         area_bound_y1 += area_bound_this;
                                         area_total += area_bound_this;
                                         area_proj_x += std::fabs(0.5*area_bound_this);
                                         center_bound_y1 = center_bound_y1 + LSMVector3((po1.x * std::fabs(po1y- po1.y)+ po2.x * std::fabs(po2y- po2.y))/(std::max(1e-6,std::fabs(po1y- po1.y)+std::fabs(po2y- po2.y))) ,0.25 *(po1y + po2y) + 0.25*(po1.y + po2.y),extenthor.GetMinY())*area_bound_this;
                                     }
                                     if(std::fabs(po1.z - extenthor.GetMaxY()) < 1e-6 && std::fabs(po2.z - extenthor.GetMaxY()) < 1e-6)
                                     {
                                         double po1y = h + dhdy *(extenthor.GetMaxY()-extenthor.GetCenterY()) + dhdx *((po1.x)-extenthor.GetCenterX());
                                         double po2y = h + dhdy *(extenthor.GetMaxY()-extenthor.GetCenterY()) + dhdx *((po2.x)-extenthor.GetCenterX());
                                         double area_bound_this = (facing_up?-1.0:1.0)*(0.5 *(po1y + po2y) - 0.5*(po1.y + po2.y)) *std::fabs(po1.x - po2.x);
                                         area_bound_y2 += area_bound_this;
                                         area_total += area_bound_this;
                                         area_proj_x += std::fabs(0.5*area_bound_this);
                                         center_bound_y2 = center_bound_y2 + LSMVector3((po1.x * std::fabs(po1y- po1.y)+ po2.x * std::fabs(po2y- po2.y))/(std::max(1e-6,std::fabs(po1y- po1.y)+std::fabs(po2y- po2.y))) ,0.25 *(po1y + po2y) + 0.25*(po1.y + po2.y),extenthor.GetMaxY())*area_bound_this;
                                     }
                                 }
                                 //std::cout << "found 7" << std::endl;

                                 //add weighted line properties

                                 LSMVector2 lnorm = LSMVector2(Normal.x,Normal.z).Normalize();
                                 lnorm = lnorm.Perpendicular();

                                 objline_total = objline_total + lnorm *area_this;
                                 objline_ctotal = objline_ctotal + hydro_cof *area_this;


                             }
                         }

//std::cout << "found 8" << std::endl;

                         objline_total = objline_total + LSMVector2(0.0,1.0)*area_bound_x1+LSMVector2(0.0,1.0)*area_bound_x2+LSMVector2(1.0,0.0)*area_bound_y1 +LSMVector2(1.0,0.0)*area_bound_y2;
                         objline_ctotal = objline_ctotal + center_bound_x1+center_bound_x2+center_bound_y1 +center_bound_y2;

                         //estimate convexness
                         //by ratio of area to equivalent LSMere area
                         //this holds some relation to the fraction of uniquely projected area vs total projected area
                         //something like this is required for concave shapes, but can work very badly for very thin convex shapes
                         //since it averages things independent of the orientation of the object


                         if(obj->IsConvex())
                         {

                             double equivalent_LSMere_area = 4.0 * LISEM_PI * std::pow(vol_total * 3.0/(LISEM_PI*4.0),2.0/3.0);
                             concavity = area_total/equivalent_LSMere_area;
                             area_proj_x_total += area_proj_x / std::max(1.0,concavity);
                            area_proj_z_total += area_proj_z / std::max(1.0,concavity);
                            objs.at(i).area_proj_x = area_proj_x / std::max(1.0,concavity);
                            objs.at(i).area_proj_z = area_proj_z / std::max(1.0,concavity);


                            capture_x_total += capture_x/ std::max(1.0,concavity);
                            capture_y_total += capture_y/ std::max(1.0,concavity);
                            capture_x_wtotal += capture_wx / std::max(1.0,concavity);
                            capture_y_wtotal += capture_wy / std::max(1.0,concavity);

                         }else
                         {
                             area_proj_x_total += area_proj_x;
                             area_proj_z_total += area_proj_z;
                             objs.at(i).area_proj_x = area_proj_x;
                             objs.at(i).area_proj_z = area_proj_z;
                             capture_x_total += capture_x;
                             capture_y_total += capture_y;
                             capture_x_wtotal += area_proj_x;
                             capture_y_wtotal += area_proj_z;

                         }

                         //set mass direction line for this object
                         objs.at(i).linedir = objline_total/std::max(1e-6,area_total + area_bound_x1 + area_bound_x2 + area_bound_y1 + area_bound_y2);
                         objs.at(i).linecenter = LSMVector2(objline_ctotal.x,objline_ctotal.z)/std::max(1e-6,area_total + area_bound_x1 + area_bound_x2 + area_bound_y1 + area_bound_y2);
                         //compensate for missing intersected surface area



                         objs.at(i).volume = vol_total;

                         lpx += objs.at(i).linecenter.x*area_total;
                         lpz += objs.at(i).linecenter.y*area_total;
                         weight += area_total*area_total;
                         lmx += objs.at(i).linedir.x*area_total;
                         lmz += objs.at(i).linedir.y*area_total;


                     }

                     //remove any objects that are without projected surface

                     for(int i = objs.size() -1; i > -1; i--)
                     {
                         if(objs.at(i).area_proj_x < 1e-4 && objs.at(i).area_proj_x < 1e-4)
                         {
                             objs.erase(objs.begin() + i);

                         }
                     }

                     //std::cout << "found 9" << std::endl;

                     if(objs.size() > 1)
                     {

                         double fraction_space_lat_averagex;
                         double fraction_space_lat_averagez;
                         double block_av = 1.0;
                         double block_av_z = 0.0;


                         double z_wsum = 0.0;
                         double z_w = 0.0;
                         double z_areasum = 0.0;
                         LSMVector2 z_dirsum ;
                         LSMVector2 z_dirsum_old;

                         //get line, and x-y plane consistency of projected area
                         {
                             objsrev = objs;

                             std::sort(objs.begin(),objs.end(),[](ObjAABB a,ObjAABB b){return a.aabb_confined.GetMinX() < b.aabb_confined.GetMinX();});
                             std::sort(objsrev.begin(),objsrev.end(),[](ObjAABB a,ObjAABB b){return a.aabb_confined.GetMaxX() < b.aabb_confined.GetMaxX();});

                             index_min = 0;
                             index_max = 0;
                             double x_last = extenthor.GetMinX();
                             double fraction_space = 1.0;

                             bool first = true;

                             for(int i = 0; i < objs.size() + objsrev.size(); i++)
                             {
                                 bool minavailable = false;
                                 if(index_min < objs.size())
                                 {
                                    minavailable = true;
                                 }
                                 bool maxavailable = false;
                                 if(index_max < objsrev.size())
                                 {
                                    maxavailable = true;
                                 }

                                 bool min_is_smaller = false;
                                 if(minavailable && maxavailable)
                                 {
                                    min_is_smaller = objs.at(index_min).aabb_confined.GetMinX() < objsrev.at(index_max).aabb_confined.GetMaxX();
                                 }
                                 if(!minavailable && !maxavailable)
                                 {
                                    break;
                                 }

                                 double x_new;
                                 double fraction_space_new;
                                 double block_av_new;
                                 if(
                                         (minavailable && maxavailable && min_is_smaller) ||
                                         (minavailable && !maxavailable)
                                   )
                                 {
                                     //closest object boundary in vertical space is the beginning of a new object
                                        fraction_space_new = fraction_space -objs.at(index_min).volume_per_unit_height * objs.at(index_min).aabb_confined.GetSizeZ()/extenthor.GetSizeY();
                                        block_av_new = block_av - objs.at(index_min).area_proj_z/(std::max(0.001,objs.at(index_min).aabb_confined.GetSizeY() * objs.at(index_min).aabb_confined.GetSizeZ()))* objs.at(index_min).aabb_confined.GetSizeZ()/extenthor.GetSizeY();
                                        x_new =objs.at(index_min).aabb_confined.GetMinX();
                                        z_wsum += objs.at(index_min).aabb_confined.GetCenterZ() * (std::fabs(objs.at(index_min).area_proj_x) + std::fabs(objs.at(index_min).area_proj_z));
                                        z_w += (std::fabs(objs.at(index_min).area_proj_x) + std::fabs(objs.at(index_min).area_proj_z));
                                        z_dirsum = z_dirsum + objs.at(index_min).linedir * (std::fabs(objs.at(index_min).area_proj_x) + std::fabs(objs.at(index_min).area_proj_z));
                                        index_min ++;
                                 }else
                                 {
                                     //closest object boundary in vertical space is the ending of an object
                                     fraction_space_new = fraction_space + objsrev.at(index_max).volume_per_unit_height * objsrev.at(index_max).aabb_confined.GetSizeZ()/extenthor.GetSizeY();
                                     block_av_new = block_av + objsrev.at(index_max).area_proj_z/(std::max(0.001,objsrev.at(index_max).aabb_confined.GetSizeY() * objsrev.at(index_max).aabb_confined.GetSizeZ()))* objsrev.at(index_max).aabb_confined.GetSizeZ()/extenthor.GetSizeY();
                                     x_new =objsrev.at(index_max).aabb_confined.GetMaxX();
                                     z_wsum -= objsrev.at(index_max).aabb_confined.GetCenterZ() * (std::fabs(objsrev.at(index_max).area_proj_x) + std::fabs(objsrev.at(index_max).area_proj_z));
                                     z_w -= (std::fabs(objsrev.at(index_max).area_proj_x) + std::fabs(objsrev.at(index_max).area_proj_z));
                                     index_max ++;
                                 }
                                 bool dobreak = false;
                                 if(!(x_new < extenthor.GetMaxX()))
                                 {

                                     x_new = extenthor.GetMaxX();
                                     dobreak = true;
                                 }
                                 double x_dif = std::max(0.0,x_new - x_last);
                                 fraction_space_lat_averagex += fraction_space * x_dif/extenthor.GetSizeX();
                                 block_av_z += std::min(1.0,std::max(0.0,1.0 -2.0 *(1.0- block_av)))* x_dif/extenthor.GetSizeX();
                                 fraction_space = fraction_space_new;
                                 block_av = block_av_new;

                                 //get z center at xnew
                                 double z_real;
                                 if(z_w > 1e-6)
                                 {
                                     z_real = z_wsum/z_w;
                                 }else
                                 {
                                     pcr::setMV(z_real);
                                 }


                                 if(first == false)
                                 {
                                    if(!pcr::isMV(z_real)&& !pcr::isMV(lpz_last))
                                    {
                                        //we have a line segment
                                        double l = 0.5 *(z_w)*(z_dirsum.dot(LSMVector2((x_new - lpx_last),(z_real -lpz_last)))*z_dirsum_old.dot(LSMVector2((x_new - lpx_last),(z_real -lpz_last)))) * LSMVector2((x_new - lpx_last),(z_real -lpz_last)).length()/csx;
                                        lpx += 0.5*(x_new + lpx_last) * l;
                                        lpz += 0.5*(z_real+ lpz_last) * l;
                                        weight += l;
                                        if(l > 1e-7)
                                        {
                                            double dx = (x_new - lpx_last)/l;
                                            double dz = (z_real- lpz_last)/l;
                                            lmx += dx * l;
                                            lmz += dz * l;
                                        }
                                    }
                                 }
                                 if(!pcr::isMV(z_real))
                                 {


                                     lpx_last = x_new;
                                     lpz_last = z_real;

                                     z_dirsum_old = z_dirsum;

                                     first = false;

                                 }

                                 x_last = x_new;
                                 if(dobreak)
                                 {
                                     break;
                                 }
                             }

                             if(x_last < extenthor.GetMaxX())
                             {
                                    fraction_space_lat_averagex  += (extenthor.GetMaxX() - x_last)  * 1.0/extenthor.GetSizeX();
                                    block_av_z += (extenthor.GetMaxX() - x_last)  * 1.0/extenthor.GetSizeX();
                             }
                        }


                         double block_av_x = 0.0;

                         //get line, and x-y plane consistency of projected area
                         //get line, and x-y plane consistency of projected area
                         {
                             objsrev = objs;

                             std::sort(objs.begin(),objs.end(),[](ObjAABB a,ObjAABB b){return a.aabb_confined.GetMinZ() < b.aabb_confined.GetMinZ();});
                             std::sort(objsrev.begin(),objsrev.end(),[](ObjAABB a,ObjAABB b){return a.aabb_confined.GetMaxZ() < b.aabb_confined.GetMaxZ();});

                             index_min = 0;
                             index_max = 0;
                             double x_last = extenthor.GetMinY();
                             double fraction_space = 1.0;

                             bool first = true;

                             for(int i = 0; i < objs.size() + objsrev.size(); i++)
                             {
                                 bool minavailable = false;
                                 if(index_min < objs.size())
                                 {
                                    minavailable = true;
                                 }
                                 bool maxavailable = false;
                                 if(index_max < objsrev.size())
                                 {
                                    maxavailable = true;
                                 }

                                 bool min_is_smaller = false;
                                 if(minavailable && maxavailable)
                                 {
                                    min_is_smaller = objs.at(index_min).aabb.GetMinZ() < objsrev.at(index_max).aabb.GetMaxZ();
                                 }
                                 if(!minavailable && !maxavailable)
                                 {
                                    break;
                                 }

                                 double x_new;
                                 double fraction_space_new;
                                 double block_av_new;
                                 if(
                                         (minavailable && maxavailable && min_is_smaller) ||
                                         (minavailable && !maxavailable)
                                   )
                                 {
                                     //closest object boundary in vertical space is the beginning of a new object
                                        fraction_space_new = fraction_space -objs.at(index_min).volume_per_unit_height * objs.at(index_min).aabb_confined.GetSizeZ()/extenthor.GetSizeY();
                                        block_av_new = block_av - objs.at(index_min).area_proj_x/(std::max(0.001,objs.at(index_min).aabb_confined.GetSizeY() * objs.at(index_min).aabb_confined.GetSizeX()))* objs.at(index_min).aabb_confined.GetSizeX()/extenthor.GetSizeX();
                                        x_new =objs.at(index_min).aabb_confined.GetMinZ();
                                        index_min ++;
                                 }else
                                 {
                                     //closest object boundary in vertical space is the ending of an object
                                     fraction_space_new = fraction_space + objsrev.at(index_max).volume_per_unit_height * objsrev.at(index_max).aabb_confined.GetSizeZ()/extenthor.GetSizeY();
                                     block_av_new = block_av + objsrev.at(index_max).area_proj_x/(std::max(0.001,objsrev.at(index_max).aabb_confined.GetSizeY() * objsrev.at(index_max).aabb_confined.GetSizeX()))* objsrev.at(index_max).aabb_confined.GetSizeX()/extenthor.GetSizeX();
                                     x_new =objsrev.at(index_max).aabb_confined.GetMaxZ();
                                     index_max ++;
                                 }
                                 bool dobreak = false;
                                 if(!(x_new < extenthor.GetMaxY()))
                                 {

                                     x_new = extenthor.GetMaxY();
                                     dobreak = true;
                                 }
                                 double x_dif = std::max(0.0,x_new - x_last);
                                 fraction_space_lat_averagez += fraction_space * x_dif/extenthor.GetSizeY();
                                 block_av_x += std::min(1.0,std::max(0.0,1.0 -2.0 *(1.0- block_av))) * x_dif/extenthor.GetSizeY();
                                 fraction_space = fraction_space_new;
                                 block_av = block_av_new;



                                 first = false;

                                 x_last = x_new;
                                 if(dobreak)
                                 {
                                     break;
                                 }
                             }

                             if(x_last < extenthor.GetMaxY())
                             {
                                    fraction_space_lat_averagez  += (extenthor.GetMaxY() - x_last)  * 1.0/extenthor.GetSizeY();
                                    block_av_x += (extenthor.GetMaxY() - x_last)  * 1.0/extenthor.GetSizeY();
                             }
                        }

                         //we can get the best fitted line through the objects
                         if(weight > 0.0)
                         {


                             lpx = lpx/weight;
                             lpz = lpz/weight;

                             //check with which directions it disects the two cell centers

                             lmx = lmx/weight;
                             lmz = lmz/weight;

                             LSMVector2 CenterThis = LSMVector2((float)(c + 0.5) * csx,(float)(r + 0.5) * csy);
                             LSMVector2 CenterX1 = LSMVector2((float)(c - 0.5) * csx,(float)(r + 0.5) * csy);
                             LSMVector2 CenterX2 = LSMVector2((float)(c + 1.5) * csx,(float)(r + 0.5) * csy);
                             LSMVector2 CenterY1 = LSMVector2((float)(c + 0.5) * csx,(float)(r - 0.5) * csy);
                             LSMVector2 CenterY2 = LSMVector2((float)(c + 0.5) * csx,(float)(r + 1.5) * csy);

                             bool disectsx1 = Geometry_PointsOnSameSideLine(CenterThis,CenterX1,LSMVector2(lpx,lpz),LSMVector2(lpx + lmx,lpz + lmz));
                             bool disectsx2 = Geometry_PointsOnSameSideLine(CenterThis,CenterX2,LSMVector2(lpx,lpz),LSMVector2(lpx + lmx,lpz + lmz));
                             bool disectsy1 = Geometry_PointsOnSameSideLine(CenterThis,CenterY1,LSMVector2(lpx,lpz),LSMVector2(lpx + lmx,lpz + lmz));
                             bool disectsy2 = Geometry_PointsOnSameSideLine(CenterThis,CenterY2,LSMVector2(lpx,lpz),LSMVector2(lpx + lmx,lpz + lmz));

                             //calculate effective blocking in those directions

                            int ldd = 5;
                            if(disectsx1)
                            {
                                if(disectsy1)
                                {
                                    ldd = 1;
                                }else if(disectsy2)
                                {
                                    ldd = 3;
                                }else
                                {
                                    ldd = 2;
                                }
                            }else if(disectsx2)
                            {
                                if(disectsy1)
                                {
                                    ldd = 7;
                                }else if(disectsy2)
                                {
                                    ldd = 9;
                                }else
                                {
                                    ldd = 8;
                                }
                            }else if(disectsy1)
                            {
                                ldd = 4;
                            }else if(disectsy2)
                            {
                                ldd = 6;
                            }

                            m_BlockLDD->data[r][c] = ldd;
                            m_BlockX->data[r][c] = std::max(0.0,1.0-block_av_x) * std::max(0.0,block_av_y);
                            m_BlockY->data[r][c] = std::max(0.0,1.0-block_av_z) * std::max(0.0,block_av_y);
                            m_HCorrected->data[r][c] = h_final - hf;

                            std::cout  << r << " " << c << " " << block_av_y << " " << std::max(0.0,1.0-block_av_x) << " " << std::max(0.0,1.0-block_av_z) << std::endl;

                            LSMVector2 is1;
                            LSMVector2 is2;
                            lineRect(lpx,lpz,lpx + lmx, lpz + lmz,(float)(c) * csx,(float)(r) * csy,csx,csy,is1,is2);

                            m_BlockLX1->data[r][c] = is1.x;
                            m_BlockLY1->data[r][c] = is1.y;
                            m_BlockLX2->data[r][c] = is2.x;
                            m_BlockLY2->data[r][c] = is2.y;

                         }else //if line weight > 0.0
                         {
                             m_BlockLDD->data[r][c] = 5;

                             m_BlockX->data[r][c] = std::max(0.0,1.0-block_av_x) * std::max(0.0,block_av_y);
                             m_BlockY->data[r][c] = std::max(0.0,1.0-block_av_z) * std::max(0.0,block_av_y);
                             m_HCorrected->data[r][c] = h_final - hf;
                             m_BlockLX1->data[r][c] = 0.0;
                             m_BlockLY1->data[r][c] = 0.0;
                             m_BlockLX2->data[r][c] = 0.0;
                             m_BlockLY2->data[r][c] = 0.0;

                             m_BlockCaptureX->data[r][c] = capture_x_total/std::max(1e-6,capture_x_wtotal);
                             m_BlockCaptureY->data[r][c] = capture_y_total/std::max(1e-6,capture_y_wtotal);



                         }

                    }else // if nrobjects > 1
                     {

                             if(weight > 0.0)
                             {


                                 lpx = lpx/weight;
                                 lpz = lpz/weight;

                                 //check with which directions it disects the two cell centers

                                 lmx = lmx/weight;
                                 lmz = lmz/weight;

                                 LSMVector2 CenterThis = LSMVector2((float)(c + 0.5) * csx,(float)(r + 0.5) * csy);
                                 LSMVector2 CenterX1 = LSMVector2((float)(c - 0.5) * csx,(float)(r + 0.5) * csy);
                                 LSMVector2 CenterX2 = LSMVector2((float)(c + 1.5) * csx,(float)(r + 0.5) * csy);
                                 LSMVector2 CenterY1 = LSMVector2((float)(c + 0.5) * csx,(float)(r - 0.5) * csy);
                                 LSMVector2 CenterY2 = LSMVector2((float)(c + 0.5) * csx,(float)(r + 1.5) * csy);

                                 bool disectsx1 = Geometry_PointsOnSameSideLine(CenterThis,CenterX1,LSMVector2(lpx,lpz),LSMVector2(lpx + lmx,lpz + lmz));
                                 bool disectsx2 = Geometry_PointsOnSameSideLine(CenterThis,CenterX2,LSMVector2(lpx,lpz),LSMVector2(lpx + lmx,lpz + lmz));
                                 bool disectsy1 = Geometry_PointsOnSameSideLine(CenterThis,CenterY1,LSMVector2(lpx,lpz),LSMVector2(lpx + lmx,lpz + lmz));
                                 bool disectsy2 = Geometry_PointsOnSameSideLine(CenterThis,CenterY2,LSMVector2(lpx,lpz),LSMVector2(lpx + lmx,lpz + lmz));

                                 //calculate effective blocking in those directions

                                int ldd = 5;
                                if(disectsx1)
                                {
                                    if(disectsy1)
                                    {
                                        ldd = 1;
                                    }else if(disectsy2)
                                    {
                                        ldd = 3;
                                    }else
                                    {
                                        ldd = 2;
                                    }
                                }else if(disectsx2)
                                {
                                    if(disectsy1)
                                    {
                                        ldd = 7;
                                    }else if(disectsy2)
                                    {
                                        ldd = 9;
                                    }else
                                    {
                                        ldd = 8;
                                    }
                                }else if(disectsy1)
                                {
                                    ldd = 4;
                                }else if(disectsy2)
                                {
                                    ldd = 6;
                                }
                                m_BlockLDD->data[r][c] = ldd;


                                LSMVector2 is1;
                                LSMVector2 is2;
                                lineRect(lpx,lpz,lpx + lmx, lpz + lmz,(float)(c) * csx,(float)(r) * csy,csx,csy,is1,is2);

                                m_BlockLX1->data[r][c] = is1.x;
                                m_BlockLY1->data[r][c] = is1.y;
                                m_BlockLX2->data[r][c] = is2.x;
                                m_BlockLY2->data[r][c] = is2.y;

                         }else
                         {
                        m_BlockLDD->data[r][c] = 5;
                        m_BlockLX1->data[r][c] = 0.0;
                        m_BlockLY1->data[r][c] = 0.0;
                        m_BlockLX2->data[r][c] = 0.0;
                        m_BlockLY2->data[r][c] = 0.0;


                         }


                         m_BlockX->data[r][c] = std::min(1.0,2.0*area_proj_x_total /(std::max(0.00001,h_final) * std::fabs(csy))) * std::max(0.0,block_av_y);
                         m_BlockY->data[r][c] = std::min(1.0,2.0*area_proj_z_total /(std::max(0.00001,h_final) * std::fabs(csx))) * std::max(0.0,block_av_y);
                         m_HCorrected->data[r][c] = (h_final)-hf;

                         m_BlockCaptureX->data[r][c] = capture_x_total/std::max(1e-6,capture_x_wtotal);
                         m_BlockCaptureY->data[r][c] = capture_y_total/std::max(1e-6,capture_y_wtotal);

                     }

                 }else //if nrobjects > 0
                 {
                     m_BlockLDD->data[r][c] = 5;
                     m_BlockX->data[r][c] = 0.0;
                     m_BlockY->data[r][c] = 0.0;
                     m_HCorrected->data[r][c] = 0.0;
                     m_BlockLX1->data[r][c] = 0.0;
                     m_BlockLY1->data[r][c] = 0.0;
                     m_BlockLX2->data[r][c] = 0.0;
                     m_BlockLY2->data[r][c] = 0.0;

                     m_BlockCaptureX->data[r][c] = 0.0;//capture_x_total;
                     m_BlockCaptureY->data[r][c] =0.0;// capture_y_total;

                 }

             }

        //std::cout <<" 4"  << std::endl;

             //now do fluid to object interactions
             for(int i = 0; i < m_Objects.length(); i++)
             {
                 RigidPhysicsObject * obj = m_Objects.at(i);
                 if(!obj->IsTerrain())
                 {
                     //obj->m_chBody->SetPos_dt(ChVector<double>(-15.0 *cos(tmodel/5.0),0.0,0.0));
                     //obj->m_chBody->SetRot(ChQuaternion<double>(1.0,0.0,0.0,0.0));
                     //obj->m_chBody->SetWvel_loc(obj->m_chBody->GetWvel_loc()*0.98);

                     obj->m_chBody->Empty_forces_accumulators();

                     BoundingBox3D extent = obj->GetAABB();
                      double csx = m_DEM->cellSizeX();
                      double csy = m_DEM->cellSizeY();
                      int rmin = extent.GetMinZ()/csy;
                      int cmin = extent.GetMinX()/csx;
                      int rmax = extent.GetMaxZ()/csy;
                      int cmax = extent.GetMaxX()/csx;

                      if(rmin > rmax)
                      {
                          int temp = rmax;
                          rmax = rmin;
                          rmin = temp;
                      }
                      if(cmin > cmax)
                      {
                          int temp = cmax;
                          cmax = cmin;
                          cmin = temp;
                      }
                      rmin = std::max(rmin-1,0);
                      cmin = std::max(cmin-1,0);
                      rmax = std::min(rmax+1,m_DEM->nrRows()-1);
                      cmax = std::min(cmax+1,m_DEM->nrCols()-1);

                      double area = 0.0;
                      double vol_total = 0.0;
                      LSMVector3 com_total;
                      LSMVector3 FFricTotal;

                      LSMVector3 objpos = obj->GetPosition();

                      {
                          ModelGeometry * model = obj->GetModel();
                          QList<LSMMesh *> meshes = model->GetMeshes();

                          for(int k = 0; k < meshes.length(); k++)
                          {
                              ////std::cout <<"total surface is" << meshes.at(k)->CalculateSurface() << std::endl;
                          }
                      }

                      for(int r = rmin; r < rmax+1; r++)
                      {
                          for(int c = cmin; c < cmax+1; c++)
                          {
                                //get top elevation of flow surface
                                float density = 1000.0;
                                float viscosity = 0.001;
                                float length = 10.0;
                                float CDrag = 1.0;

                                float h = m_DEM->data[r][c];
                                float hf = ((m_HF == nullptr)? 0.0:m_HF->data[r][c]);
                                float hs = ((m_HS == nullptr)? 0.0:m_HS->data[r][c]);
                                float dhdx = cTMap::DER_X(m_DEM,r,c)/csx;
                                float dhdy = cTMap::DER_Y(m_DEM,r,c)/csy;
                                float dhfdx = cTMap::DER_X(m_HF,r,c)/csx;
                                float dhfdy = cTMap::DER_Y(m_HF,r,c)/csy;
                                float dhsdx = cTMap::DER_X(m_HS,r,c)/csx;
                                float dhsdy = cTMap::DER_Y(m_HS,r,c)/csy;
                                float surfacetop = h+ hf +hs
                                        + 0.5 * std::fabs(csy) * dhdy
                                        +  0.5 * std::fabs(csx) * dhdx
                                        + 0.5 * std::fabs(csy) * dhsdy
                                        +  0.5 * std::fabs(csx) * dhsdx
                                        + 0.5 * std::fabs(csy) * dhfdy
                                        +  0.5 * std::fabs(csx) * dhfdx;

                                float fvelx = ((m_UFX == nullptr)? 0.0:m_UFX->data[r][c]);
                                float fvely = ((m_UFY == nullptr)? 0.0:m_UFY->data[r][c]);
                                if(fabs(fvelx) > 1e3)
                                {
                                    fvelx = 0.0;
                                }
                                if(fabs(fvely) > 1e3)
                                {
                                    fvely = 0.0;
                                }

                                //if(surfacetop > extent.GetMinY())
                                {
                                    ModelGeometry * model = obj->GetModel();
                                    QList<LSMMesh *> meshes = model->GetMeshes();


                                    LSMVector2 offset = GetSimpleGeoOrigin();
                                    LSMMatrix4x4 objtrans;
                                    LSMMatrix4x4 objrotm = obj->GetRotationMatrix();
                                    objtrans.Translate(LSMVector3(objpos.x,objpos.y,objpos.z));
                                    objrotm = objtrans*objrotm;

                                    for(int k = 0; k < meshes.length(); k++)
                                    {
                                        LSMMesh * mesh = meshes.at(k);
                                        int triangleCount = mesh->indices.size()/3;
                                        for (unsigned long i = 0; i <  triangleCount; i++)
                                        {
                                            LSMVector3 relpos_A = mesh->vertices[mesh->indices[i * 3 + 0]].position();
                                            LSMVector3 relpos_B = mesh->vertices[mesh->indices[i * 3 + 1]].position();
                                            LSMVector3 relpos_C = mesh->vertices[mesh->indices[i * 3 + 2]].position();

                                            //we have to convert this to global world space
                                            LSMVector3 Atmp = objrotm *relpos_A;
                                            LSMVector3 Btmp = objrotm *relpos_B;
                                            LSMVector3 Ctmp = objrotm *relpos_C;

                                            double area_fulltotal = TriangleArea(Atmp,Btmp,Ctmp);

                                            LSMVector3 FLoc = (Atmp + Btmp + Ctmp)*(1.0/3.0);
                                            LSMVector3 FLoc_loc= (relpos_A + relpos_B + relpos_C)*(1.0/3.0);

                                            //get velocity and angular velocity
                                            ChVector<double> rot = obj->m_chBody->GetWvel_par();
                                            ChVector<double> vel = obj->m_chBody->GetPos_dt();


                                            LSMVector3 axis = LSMVector3(rot[0],rot[1],rot[2]);
                                            LSMVector3 velo = LSMVector3(vel[0],vel[1],vel[2]);


                                            ChVector<double> vel_abs_A = obj->m_chBody->PointSpeedLocalToParent(ChVector<double>(relpos_A.x,relpos_A.y,relpos_A.z));
                                            ChVector<double> vel_abs_B = obj->m_chBody->PointSpeedLocalToParent(ChVector<double>(relpos_B.x,relpos_B.y,relpos_B.z));
                                            ChVector<double> vel_abs_C = obj->m_chBody->PointSpeedLocalToParent(ChVector<double>(relpos_C.x,relpos_C.y,relpos_C.z));
                                            ChVector<double> vel_abs_F = obj->m_chBody->PointSpeedLocalToParent(ChVector<double>(FLoc_loc.x,FLoc_loc.y,FLoc_loc.z));

                                            LSMVector3 vel_FLoc = velo + LSMVector3::CrossProduct(axis,FLoc - objpos);//*DistancePointLine(mesh->vertices[mesh->indices[i * 3 + 2]].position(),LSMVector3(0.0,0.0,0.0),axis);
                                            LSMVector3 vel_FLoc_NoRot = velo;//*DistancePointLine(mesh->vertices[mesh->indices[i * 3 + 2]].position(),LSMVector3(0.0,0.0,0.0),axis);

                                            LSMVector3 Normal = LSMVector3::CrossProduct(Atmp-Btmp,Ctmp-Btmp).Normalize();

                                            //finally, get the sub-surface properties
                                            double vol;
                                            double area_proj;
                                            double proj_area_vel;
                                            LSMVector3 com;
                                            LSMVector3 hydro_cof;
                                            double hydro_total;
                                            BoundingBox extenthor((float)(c) * csx, (float)(c+1) * csx, (float)(r) * csy, (float)(r+1) * csy) ;
                                            std::vector<LSMVector3> points;
                                            double area_this =TriangleWaterFlowIntersect(Btmp,Atmp,Ctmp,extenthor,hf,dhfdx,dhfdy,h,dhdx,dhdy,vol,area_proj,com,LSMVector3(10.0,0.0,10.0),proj_area_vel, hydro_cof,hydro_total,points);//);
                                            area += area_this;
                                            vol_total += vol;
                                            com_total = com_total + com * vol;

                                            if(area_this > 0.0001 )
                                            {
                                                if(points.size() > 0)
                                                {
                                                    LSMVector3 ref = points[0];
                                                    for(int l = 0; l < points.size()-1; l++)
                                                    {
                                                        LSMVector3 v0 = ref;
                                                        LSMVector3 v1 = points.at(l);
                                                        LSMVector3 v2 = points.at(l+1);


                                                        //these are all the sub-triangles that come from the intersection algorithm
                                                        //calculate all the necessary forces for each triangle

                                                    }
                                                }

                                                LSMVector3 vel_difav = vel_FLoc * 1.0+LSMVector3(fvelx,0.0,fvely)*-1.0;
                                                LSMVector3 vel_tangent = LSMVector3::CrossProduct(Normal,LSMVector3::CrossProduct(vel_difav,Normal)).Normalize() * (-vel_difav.length());//

                                                //skin drag(parrallel to triangle surface)
                                                LSMVector3 F = vel_tangent * 1000.0 * vel_tangent.length() * area_this* std::min(10.0,0.075/std::max(0.001f,std::log10(density * length * vel_tangent.length()/viscosity)));
                                                FFricTotal = FFricTotal + F;

                                                //form drag(tangent to triangle surface)
                                                LSMVector3 F2 = ((Normal.dot(vel_difav) > 0.0) ? Normal : Normal * -1.0) * -1000.0 * CDrag *  std::fabs(Normal.dot(vel_difav)) * area_this;

                                                LSMVector3 NormalWater = LSMVector3(-dhdx,1.0,-dhdy).Normalize();

                                                //entry or slamming forces
                                                LSMVector3 F3 = NormalWater * 1000.0 * (area_this/std::max(0.001,area_fulltotal)) *(1.0-(area_this/std::max(0.001,area_fulltotal)))* area_this * std::max(0.0,-NormalWater.dot(Normal)) * std::max(0.0,-NormalWater.dot(vel_FLoc));

                                                float depht_floc = std::max(0.0f,h + hf - FLoc.y);// + (dhfdx + dhdx)*(FLoc.x - extenthor.GetCenterX()) + (dhfdy + dhdy)*(FLoc.z - extenthor.GetCenterY())  - FLoc.y);
                                                float FHydro = -Normal.y*depht_floc *-1000.0 * 9.81 * area_this;
                                                ////std::cout << " drag force " << F.length() << "  " << vel_tangent.length() << std::endl;
                                                if(std::isfinite(F.x) && std::isfinite(F.y) && std::isfinite(F.z) )
                                                {
                                                    obj->m_chBody->Accumulate_force(ChVector<double>(F.x,F.y,F.z),ChVector<double>(FLoc.x,FLoc.y,FLoc.z),false);
                                                    m_BlockFX->data[r][c] = m_BlockFX->data[r][c] - F.x * area_this/(csx * csx );
                                                    m_BlockFY->data[r][c] = m_BlockFY->data[r][c] - F.z * area_this/(csx  * csx );
                                                }

                                                //std::cout << "area this " << area_this << " " << vel_tangent.length() << "  " << F.length() << " " << F2.length() << " " << F3.length() << "          " << velo.length() << "  " << axis.length()<< "   " << (FLoc - objpos).length() <<"  " << LSMVector3(fvelx,0.0,fvely).length() << "  " << vel_FLoc.length() << std::endl;

                                                if(std::isfinite(F2.x) && std::isfinite(F2.y) && std::isfinite(F2.z) )
                                                {
                                                    obj->m_chBody->Accumulate_force(ChVector<double>(F2.x,F2.y,F2.z),ChVector<double>(FLoc.x,FLoc.y,FLoc.z),false);
                                                    m_BlockFX->data[r][c] -= F2.x* area_this/(csx  * csx );
                                                    m_BlockFY->data[r][c] -= F2.z* area_this/(csx  * csx );
                                                }

                                                if(std::isfinite(F3.x) && std::isfinite(F3.y) && std::isfinite(F3.z) )
                                                {
                                                    obj->m_chBody->Accumulate_force(ChVector<double>(F3.x,F3.y,F3.z),ChVector<double>(FLoc.x,FLoc.y,FLoc.z),false);
                                                    m_BlockFX->data[r][c] -= F3.x* area_this/(csx  * csx );
                                                    m_BlockFY->data[r][c] -= F3.z* area_this/(csx  * csx );
                                                }

                                            }
                                        }
                                    }
                                }
                          }
                      }

                      if(vol_total > 1e-6)
                      {
                          com_total = com_total/vol_total;
                      }

                      Quaternion rot = obj->m_chBody->GetFrame_REF_to_abs().GetRot();
                      double angle;
                      Vector axisc;
                      rot.Q_to_AngAxis(angle, axisc);

                      Quaternion rot2 = obj->m_chBody->GetFrame_REF_to_abs().GetRot_dt();
                      double angle2;
                      Vector axisc2;
                      rot2.Q_to_AngAxis(angle2, axisc2);

                      ChVector<double> rot3 = obj->m_chBody->GetWvel_par();


                      /*//std::cout << "total area and volume : "  << vol_total << "  " << area << std::endl;
                      //std::cout<< "TOTAL DRAG FORCE:  "<< obj->m_chBody->Get_accumulated_force().x() << " " << obj->m_chBody->Get_accumulated_force().y() << " " << obj->m_chBody->Get_accumulated_force().z() << std::endl;
                      //std::cout<< "TOTAL DRAG FORCE TORQUE:  "<< obj->m_chBody->Get_accumulated_torque().x() << " " << obj->m_chBody->Get_accumulated_torque().y() << " " << obj->m_chBody->Get_accumulated_torque().z() << std::endl;
                      //std::cout<< "TOTAL Rotation:  "<< angle << " " << axisc.x() << " " << axisc.y() << "  " << axisc.z()<<std::endl;
                      //std::cout<< "TOTAL Rotation vel:  "<< rot3.x()<<" " << rot3.y() << " " << rot3.z() << std::endl;//" angle2 << " " << axisc2.x() << " " << axisc2.y() << "  " << axisc2.z()<<std::endl;
                      //std::cout<< "TOTAL Velocity:  "<< obj->m_chBody->GetPos_dt().x() << " " << obj->m_chBody->GetPos_dt().y() << " " << obj->m_chBody->GetPos_dt().z() <<std::endl;
                      //std::cout<< "TOTAL Position:  "<< obj->m_chBody->GetPos().x() << " " << obj->m_chBody->GetPos().y() << " " << obj->m_chBody->GetPos().z() <<std::endl;

                      //std::cout<< "com:  "<< com_total.x << " " << objpos.x << "  "  << com_total.y << " " << objpos.y << "  " << com_total.z << " " << objpos.z << "  "  <<std::endl;
                      */

                    if(vol_total > 1e-6)
                      {
                           //std::cout << " vol total " << vol_total << std::endl;
                          //buoyoncy force
                          obj->m_chBody->Accumulate_force(ChVector<double>(0.0,vol_total * 1000.0 * 9.81,0.0),ChVector<double>(com_total.x,com_total.y,com_total.z),false);


                      }

                    //TEST, IS THERE SOME ERROR WITH THE TORQUE FRAME OF REFERENCE IN PROJECT CHRONO?????
                    //APPARENTLY THERE IS, the Accumulate_force function with local = false sets torque in global frame
                    //later, it is used as local frame value.
                    //the Accumulate_torque does transform correctly.
                    //temporary solution now: set the torque manually to be transformed
                    obj->m_chBody->Accumulate_torque(obj->m_chBody->Dir_World2Body(obj->m_chBody->Get_accumulated_torque())-obj->m_chBody->Get_accumulated_torque(),true);

                    //now do fluid to object interactions



                 }



             }
         }

         std::cout << "step model " << std::endl;
         m_system.DoStepDynamics(dtreal);
         t = t + dtreal;
         iter ++;

         m_Mutex_internal.lock();
         m_HasChanges = true;
         m_Mutex_internal.unlock();
    //std::cout <<" run rigid step done " << t <<  std::endl;

         m_Mutex.unlock();

    }

    //std::cout <<" run rigid step done all" << std::endl;


}


void RigidPhysicsWorld::LockMutex()
{
   m_Mutex.lock();


}
void RigidPhysicsWorld::UnLockMutex()
{

    m_Mutex.unlock();
}

QList<RigidPhysicsObject * > RigidPhysicsWorld::GetObjectList()
{
    return m_Objects;
}



void RigidPhysicsWorld::AS_Step(float dt)
{
    this->RunSingleStep(dt,0.0);
}
void RigidPhysicsWorld::AS_SetElevation(cTMap * dem)
{
    m_DEM = dem->GetCopy();
    m_DEMObjects =RigidPhysicsObject::RigidPhysicsObject_AsHeightField(m_DEM,false);


    LSMVector2 geo_old = GetSimpleGeoOrigin();

    for(int i = 0; i < m_DEMObjects.length(); i++)
    {
        AddObject(m_DEMObjects.at(i));
    }


    SetSimpleGeoOrigin(LSMVector2(dem->west(),dem->north()));

    LSMVector2 geo = GetSimpleGeoOrigin();

    for(int i = 0; i < m_Objects.size(); i++)
    {
        if(!(m_Objects.at(i)->IsTerrain()))
        {
            LSMVector3 pos = m_Objects.at(i)->GetPosition();
            LSMVector3 posn = LSMVector3(pos.x + geo_old.x - geo.x,pos.y,pos.z + geo_old.y -geo.y);
            m_Objects.at(i)->m_chBody->SetPos(ChVector<double>(posn.x,posn.y,posn.z));
        }
    }

    m_HasTerrain = true;
}

void RigidPhysicsWorld::AS_SetFlow(cTMap * h , cTMap * ux, cTMap * uy ,cTMap * dens)
{
    SetInteractTwoPhaseFlow(h->GetCopy(),ux->GetCopy(),uy->GetCopy(),nullptr,nullptr,nullptr,nullptr, true);

}


std::vector<Field*> RigidWorldToField(RigidPhysicsWorld* world, Field * ref)
{

    //
    //It is important to note that the Field extend the Map data structure, and
    //uses x-y directions as horizontal axes.
    //The rigid physics and mesh stuff follow the usual convension
    //of having y in the vertical direction
    //here, we have to convert between the two
    // (x,y,z)->(x,z,y)


    //set all blocking and velocity to 0

    Field * Block = ref->GetCopy0();
    Field * BlockU = ref->GetCopy0();
    Field * BlockV = ref->GetCopy0();
    Field * BlockW = ref->GetCopy0();

    //get all cell centers and check if they are contained in one of the objects of the rigid simulation

    QList<RigidPhysicsObject *> objs = world->GetObjectList();
    for(int i = 0; i < objs.size(); i++)
    {
        RigidPhysicsObject * obj= objs.at(i);

        if(obj->IsTerrain())
        {
            continue;
        }

        BoundingBox3D bb = obj->GetAABB();

        LSMVector2 geo_simple = world->GetSimpleGeoOrigin();

        BoundingBox3D fieldbb = Block->GetAABB();

        fieldbb = BoundingBox3D(fieldbb.GetMinX()- geo_simple.x,fieldbb.GetMaxX()- geo_simple.x,fieldbb.GetMinY(),fieldbb.GetMaxY(),fieldbb.GetMinZ()- geo_simple.y,fieldbb.GetMaxZ()- geo_simple.y);

        //overlaps with bounding box of the Field?
        if(bb.Overlaps(fieldbb))
        {


            //if contained, get local linear velocity of the body

            int lstart = (bb.GetMinY() - Block->GetBottom())/Block->cellSizeZ();
            int cstart = (bb.GetMinX() + geo_simple.x - Block->GetWest())/Block->cellSizeX();
            int rstart = (bb.GetMinZ() + geo_simple.y - Block->GetNorth())/Block->cellSizeY();

            int lend = (bb.GetMaxY() - Block->GetBottom())/Block->cellSizeZ();
            int cend = (bb.GetMaxX() + geo_simple.x - Block->GetWest())/Block->cellSizeX();
            int rend = (bb.GetMaxZ() + geo_simple.y - Block->GetNorth())/Block->cellSizeY();

            lstart = std::max(0,std::min(Block->nrLevels()-1,lstart));
            cstart = std::max(0,std::min(Block->nrCols()-1,cstart));
            rstart = std::max(0,std::min(Block->nrRows()-1,rstart));

            lend = std::max(0,std::min(Block->nrLevels()-1,lend));
            cend = std::max(0,std::min(Block->nrCols()-1,cend));
            rend = std::max(0,std::min(Block->nrRows()-1,rend));

            if(rstart > rend)
            {
                int temp = rstart;
                rstart = rend;
                rend = temp;
            }
            if(cstart > cend)
            {
                int temp = cstart;
                cstart = cend;
                cend = temp;
            }
            if(lstart > lend)
            {
                int temp = lstart;
                lstart = lend;
                lend = temp;
            }

            for(int l = lstart; l < lend +1; l++)
            {
                for(int r = rstart; r < rend +1; r++)
                {
                    for(int c = cstart; c < cend +1; c++)
                    {
                        float x = Block->GetWest() - geo_simple.x + Block->cellSizeX() * ((float)(c));
                        float y = Block->GetBottom() + Block->cellSizeZ() * ((float)(l));
                        float z = Block->GetNorth() - geo_simple.y + Block->cellSizeY() * ((float)(r));

                        if(obj->Contains(LSMVector3(x,y,z)))
                        {
                            LSMVector3 vel = obj->GetLocalLinearVelocity(LSMVector3(x,y,z));

                            Block->at(l)->data[r][c] = 1.0f;
                            BlockU->at(l)->data[r][c] = vel.x;//Block->cellSizeX()> 0.0?vel.x : -vel.x;
                            BlockV->at(l)->data[r][c] = vel.z;
                            BlockW->at(l)->data[r][c] = vel.y;//Block->cellSizeY()> 0.0? vel.y : - vel.y;
                        }
                    }
                }
            }
        }
    }

    return {Block,BlockU,BlockV,BlockW};
}

void RigidWorldApplyPressureField(RigidPhysicsWorld* world, Field * Block, Field * P)
{

    //
    // Currently we approximate the fluid-rigid interface by looking at cell-boundaries.
    // another more accurate approach is used ofte, where raytracing is done along boundary cells to better approximate the local
    // normal vector along the surface of the rigid object
    // this would also allow for a more accurate, sub-cell representation of local volume, although it is not so clear how this
    // can be easily combined with the current incompressible naviers stokes 3d solver.
    // a volume of fluid method might be needed to implement this.






    //for each object, get bounding box

    BoundingBox3D fieldbb = Block->GetAABB();

    //loop over all pixels within the bounding box

    //if a pixel is blocked by the object, but a neighboring pixel contains a pressure value
    //we apply that pressure as a boundary force

    QList<RigidPhysicsObject *> objs = world->GetObjectList();
    for(int i = 0; i < objs.size(); i++)
    {
        RigidPhysicsObject * obj= objs.at(i);

        if(obj->IsTerrain())
        {
            continue;
        }

        obj->m_chBody->Empty_forces_accumulators();

        BoundingBox3D bb = obj->GetAABB();


        LSMVector2 geo_simple = world->GetSimpleGeoOrigin();

        //overlaps with bounding box of the Field?
        if(bb.Overlaps(fieldbb))
        {
            //if contained, get local linear velocity of the body

            int lstart = (bb.GetMinY() - Block->GetBottom())/Block->cellSizeZ();
            int cstart = (bb.GetMinX() + geo_simple.x - Block->GetWest())/Block->cellSizeX();
            int rstart = (bb.GetMinZ() + geo_simple.y - Block->GetNorth())/Block->cellSizeY();

            int lend = (bb.GetMaxY() - Block->GetBottom())/Block->cellSizeZ();
            int cend = (bb.GetMaxX() + geo_simple.x - Block->GetWest())/Block->cellSizeX();
            int rend = (bb.GetMaxZ() + geo_simple.y - Block->GetNorth())/Block->cellSizeY();

            lstart = std::max(0,std::min(Block->nrLevels()-1,lstart));
            cstart = std::max(0,std::min(Block->nrCols()-1,cstart));
            rstart = std::max(0,std::min(Block->nrRows()-1,rstart));

            lend = std::max(0,std::min(Block->nrLevels()-1,lend));
            cend = std::max(0,std::min(Block->nrCols()-1,cend));
            rend = std::max(0,std::min(Block->nrRows()-1,rend));

            if(rstart > rend)
            {
                int temp = rstart;
                rstart = rend;
                rend = temp;
            }
            if(cstart > cend)
            {
                int temp = cstart;
                cstart = cend;
                cend = temp;
            }
            if(lstart > lend)
            {
                int temp = lstart;
                lstart = lend;
                lend = temp;
            }

            for(int l = lstart; l < lend +1; l++)
            {
                for(int r = rstart; r < rend +1; r++)
                {
                    for(int c = cstart; c < cend +1; c++)
                    {
                        float x = Block->GetWest() - geo_simple.x + Block->cellSizeX() * ((float)(c));
                        float y = Block->GetBottom() + Block->cellSizeZ() * ((float)(l));
                        float z = Block->GetNorth() - geo_simple.y + Block->cellSizeY() * ((float)(r));

                        float signx = Block->cellSizeX() > 0.0? 1.0:-1.0;
                        float signy = Block->cellSizeY() > 0.0? 1.0:-1.0;
                        float signz = Block->cellSizeZ() > 0.0? 1.0:-1.0;



                        if(Block->at(l)->data[r][c] > 0.5f)
                        {
                            if(l-1 > 0)
                            {
                                if(Block->at(l-1)->data[r][c] < 0.5f)
                                {
                                    float pressure = signz * P->at(l-1)->data[r][c];
                                    obj->m_chBody->Accumulate_force(ChVector<double>(0.0,pressure,0.0),ChVector<double>(x,y,z),false);
                                }
                            }

                            if(l+1 < Block->nrLevels())
                            {
                                if(Block->at(l+1)->data[r][c] < 0.5f)
                                {
                                    float pressure = signz * P->at(l+1)->data[r][c];
                                    obj->m_chBody->Accumulate_force(ChVector<double>(0.0,-pressure,0.0),ChVector<double>(x,y,z),false);
                                }
                            }

                            if(r-1 > 0)
                            {
                                if(Block->at(l)->data[r-1][c] < 0.5f)
                                {
                                    float pressure = signy * P->at(l)->data[r-1][c];
                                    obj->m_chBody->Accumulate_force(ChVector<double>(0.0,0.0,pressure),ChVector<double>(x,y,z),false);
                                }
                            }

                            if(r+1 < Block->nrRows())
                            {
                                if(Block->at(l)->data[r+1][c] < 0.5f)
                                {
                                    float pressure = signy * P->at(l)->data[r+1][c];
                                    obj->m_chBody->Accumulate_force(ChVector<double>(0.0,0.0,-pressure),ChVector<double>(x,y,z),false);
                                }
                            }


                            if(c-1 > 0)
                            {
                                if(Block->at(l)->data[r][c-1] < 0.5f)
                                {
                                    float pressure = signx * P->at(l)->data[r][c-1];
                                    obj->m_chBody->Accumulate_force(ChVector<double>(pressure,0.0,0.0),ChVector<double>(x,y,z),false);
                                }
                            }

                            if(c+1 < Block->nrCols())
                            {
                                if(Block->at(l)->data[r][c+1] < 0.5f)
                                {
                                    float pressure = signx* P->at(l)->data[r][c+1];
                                    obj->m_chBody->Accumulate_force(ChVector<double>(-pressure,0.0,0.0),ChVector<double>(x,y,z),false);
                                }
                            }
                        }
                    }
                }
            }

            //TEST, IS THERE SOME ERROR WITH THE TORQUE FRAME OF REFERENCE IN PROJECT CHRONO?????
            //APPARENTLY THERE IS, the Accumulate_force function with local = false sets torque in global frame
            //later, it is used as local frame value.
            //the Accumulate_torque does transform correctly.
            //temporary solution now: set the torque manually to be transformed
            obj->m_chBody->Accumulate_torque(obj->m_chBody->Dir_World2Body(obj->m_chBody->Get_accumulated_torque())-obj->m_chBody->Get_accumulated_torque(),true);


        }
    }


}
