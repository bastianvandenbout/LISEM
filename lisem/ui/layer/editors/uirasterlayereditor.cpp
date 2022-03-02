#include "uirasterlayereditor.h"
#include "gl/openglcldatamanager.h"

UIRasterLayerEditor::UIRasterLayerEditor(UILayer * rl) : UILayerEditor(rl)
{

    m_RasterLayer =dynamic_cast<UIStreamRasterLayer*>( rl);

    m_FilePath = m_RasterLayer->GetFilePath();

    QList<cTMap *> bandmaps = m_RasterLayer->GetMaps();

    for(int i = 0; i < bandmaps.size(); i++)
    {
        m_OriginalMap.append(bandmaps.at(i)->GetCopy());
        m_Map.append(bandmaps.at(i));
        m_TMap.append(bandmaps.at(i)->GetCopy0());

    }
    m_RMap = bandmaps.at(0)->GetCopy0();
    m_CMap = bandmaps.at(0)->GetCopy0();

    for(int r = 0; r < m_Map.at(0)->nrRows(); r++)
    {
        for(int c = 0; c < m_Map.at(0)->nrCols(); c++)
        {
            m_RMap->data[r][c] = r;
            m_CMap->data[r][c] = c;

            for(int i = 0; i < m_TMap.length(); i++)
            {
                m_TMap.at(i)->data[r][c] = m_Map.at(i)->data[r][c];
            }
        }
    }

    AddParameter(UI_PARAMETER_TYPE_GROUP,"Draw Tool","","");
    AddParameter(UI_PARAMETER_TYPE_RADIO,"Type","Paint|Add|Subtract|Multiply|Converge|Blur|Liquify|Drag","Paint");
    AddParameter(UI_PARAMETER_TYPE_RADIO,"Shape","Rectangle|Ellips|Rectangle(drag)|Lines|Polygon","Rectangle");
    AddParameter(UI_PARAMETER_TYPE_FLOAT,"Shape Size X","Horizontal extent of the brush shape",QString::number(m_Map.at(0)->cellSize() * 10.0),0,1000000.0);
    AddParameter(UI_PARAMETER_TYPE_FLOAT,"Shape Size Y","Vertical extent of the brush shape",QString::number(m_Map.at(0)->cellSize() * 10.0),0,1000000.0);
    AddParameter(UI_PARAMETER_TYPE_FLOAT,"Shape Feather","Softness of the brush edge","0",0.0,1.0);
    AddParameter(UI_PARAMETER_TYPE_FLOAT,"Value","Value to either set or add/subtract/multiply per second","1",-1e31,1e31);
    AddParameter(UI_PARAMETER_TYPE_BOOL,"Missing Value Painting","Paint Missing Value","false");
    AddParameter(UI_PARAMETER_TYPE_BOOL,"Missing Value Overwrite","Overwrite Missing Value","false");

}

void UIRasterLayerEditor::OnDraw(OpenGLCLManager * m,GeoWindowState state)
{

    m_ScriptMutex.lock();

    QString edit_shapetype_string = GetParameterValue("Shape");


    GeoCoordTransformer * transformer = GeoCoordTransformer::GetCoordTransformer(state.projection,m_RasterLayer->GetProjection());

    LSMVector2 MousePosLoc = transformer->Transform(LSMVector2(state.MouseGeoPosX,state.MouseGeoPosZ));


    float edit_cx = MousePosLoc.x;
    float edit_cy = m_Map.at(0)->cellSizeY() > 0? (m_Map.at(0)->north() + (m_Map.at(0)->nrRows() * m_Map.at(0)->cellSizeY()) - MousePosLoc.y) : MousePosLoc.y;
    float edit_sx = GetParameterValueDouble("Shape Size X");
    float edit_sy = GetParameterValueDouble("Shape Size Y");
    float edit_ff = GetParameterValueDouble("Shape Feather");
    float edit_value = GetParameterValueDouble("Value");
    float edit_value2 = 0.95;

    bool paint_mv = GetParameterValue("Missing Value Painting").toInt();
    bool paint_mv_overwrite = GetParameterValue("Missing Value Overwrite").toInt();

    RasterAlterFunc f;

    QString PaintFunction = GetParameterValue("Type");

    bool mult_f = false;

    cTMap * Target2 = nullptr;
    cTMap * Target3 = nullptr;
    if(PaintFunction.compare("Paint") == 0)
    {
        f = PaintToRasterCell;
    }else if(PaintFunction.compare("Add") == 0)
    {
        f = AddToRasterCell;
    }else if(PaintFunction.compare("Subtract") == 0)
    {
        f = AddToRasterCell;
        edit_value = edit_value * -1.0;
    }else if(PaintFunction.compare("Multiply") == 0)
    {
        f = MutiplyToRasterCell;
    }else if(PaintFunction.compare("Converge") == 0)
    {
        f = ConvergeRasterCell;
    }else if(PaintFunction.compare("Blur") == 0)
    {
        f = BlurRasterCell;
    }else if(PaintFunction.compare("Liquify") == 0)
    {
        LSMVector2 tr = transformer->Transform(LSMVector2(state.MouseGeoPosX ,state.MouseGeoPosZ));

        float tlX = tr.x;
        float tlY = tr.y;

        LSMVector2 tr2 = transformer->Transform(LSMVector2(m_DragOrigin.x,m_DragOrigin.z));

        m_DragOrigin = LSMVector3(state.MouseGeoPosX ,state.MouseGeoPosY, state.MouseGeoPosZ);

        LSMVector2 trdif = tr2 - tr;

        //move r
        edit_value = (m_Map.at(0)->cellSizeY() > 0? -1.0:1.0)*(-trdif.y/m_Map.at(0)->cellSizeY());
        //move c
        edit_value2 = -trdif.x/m_Map.at(0)->cellSizeX();


        std::cout << edit_value << " " << edit_value2 << std::endl;
        f = LiquifyRasterCell;
        mult_f = true;
    }else if(PaintFunction.compare("Drag") == 0)
    {
        LSMVector2 tr = transformer->Transform(LSMVector2(state.MouseGeoPosX ,state.MouseGeoPosZ));

        float tlX = tr.x;
        float tlY = tr.y;

        LSMVector2 tr2 = transformer->Transform(LSMVector2(m_DragOrigin.x,m_DragOrigin.z));

        m_DragOrigin = LSMVector3(state.MouseGeoPosX ,state.MouseGeoPosY, state.MouseGeoPosZ);

        LSMVector2 trdif = tr2 - tr;

        //move r
        edit_value = (m_Map.at(0)->cellSizeY() > 0? -1.0:1.0)*(-trdif.y/m_Map.at(0)->cellSizeY());
        //move c
        edit_value2 = -trdif.x/m_Map.at(0)->cellSizeX();

        std::cout << edit_value << " " << edit_value2 << std::endl;
        f = LiquifyRasterCell;
        mult_f = true;

        Target2 = m_RMap;
        Target3 = m_CMap;
    }


    bool dont_ctrlz = false;
    bool edited = false;

    if(PaintFunction.compare("Drag") != 0)
    {

        if(edit_shapetype_string.compare("Rectangle") == 0 || edit_shapetype_string.compare("Ellips") == 0)
        {
            static auto before = std::chrono::system_clock::now();
            auto now = std::chrono::system_clock::now();
            double diff_ms = std::chrono::duration <double, std::milli> (now - before).count();
            if((m_Dragging && diff_ms > 30)|| m_HasDoneMouseClick )
            {
                before = now;
                if(!m_HasDoneCtrlZ)
                {
                    if(edit_shapetype_string.compare("Rectangle") == 0)
                    {

                        for(int i = 0; i < m_Map.length(); i++)
                        {
                            cTMap * Target = m_Map.at(i);

                            RasterCoordinateList rl = RasterizeRectangle(Target,edit_cx,edit_cy,edit_sx,edit_sy,edit_ff);
                            m_Edits.append(RasterPaint(Target,rl,f,edit_value,edit_value2,paint_mv_overwrite,paint_mv, mult_f));

                            if(i == 0)
                            {
                                m_RasterLayer->NotifyDataChange(rl.GetBoundingBox());
                            }
                            //signal the raster layer to update the opengl texture from the map
                            //and re-calculate the minimum and maximum value
                        }

                        m_EditSimultaneity.append(m_Map.length());
                        edited = true;
                    }else {

                        for(int i = 0; i < m_Map.length(); i++)
                        {
                            cTMap * Target = m_Map.at(i);

                            RasterCoordinateList rl = RasterizeEllipsoid(Target,edit_cx,edit_cy,edit_sx,edit_sy,edit_ff);

                            m_Edits.append(RasterPaint(Target,rl,f,edit_value,edit_value2,paint_mv_overwrite,paint_mv, mult_f));

                            if(i == 0)
                            {
                                m_RasterLayer->NotifyDataChange(rl.GetBoundingBox());
                            }
                        }

                        m_EditSimultaneity.append(m_Map.length());
                        edited = true;
                        //signal the raster layer to update the opengl texture from the map
                        //and re-calculate the minimum and maximum value
                    }

                }else
                {
                    dont_ctrlz = true;
                }

            }
        }else if(edit_shapetype_string.compare("Rectangle(drag)") == 0 )
            {
                if(m_HasSquare)
                {
                    if(m_HasDoneEnter)
                    {

                        if(!m_HasDoneCtrlZ)
                        {


                            //draw square from origin position to current position



                            LSMVector2 tr = transformer->Transform(LSMVector2(state.MouseGeoPosX ,state.MouseGeoPosZ));

                            float tlX = tr.x;
                            float tlY = tr.y;

                            LSMVector2 tr2 = transformer->Transform(LSMVector2(m_DragOrigin.x,m_DragOrigin.z));

                            float brX = tr2.x;
                            float brY = tr2.y;

                            if(tlX > brX)
                            {
                                float temp = brX;
                                brX = tlX;
                                tlX = temp;
                            }
                            if(tlY > brY)
                            {
                                float temp = brY;
                                brY = tlY;
                                tlY = temp;
                            }

                            for(int i = 0; i < m_Map.length(); i++)
                            {
                                cTMap * Target = m_Map.at(i);

                                RasterCoordinateList rl = RasterizeRectangle(Target,(tlX +  brX)*0.5,(tlY +  brY)*0.5,(brX -  tlX),(brY - tlY),edit_ff);
                                m_Edits.append(RasterPaint(Target,rl,f,edit_value,edit_value2,paint_mv_overwrite,paint_mv, mult_f));

                                if(i == 0)
                                {
                                    m_RasterLayer->NotifyDataChange(rl.GetBoundingBox());
                                }
                            }

                            m_EditSimultaneity.append(m_Map.length());
                            edited = true;
                            //signal the raster layer to update the opengl texture from the map
                            //and re-calculate the minimum and maximum value

                            m_HasSquare = false;
                        }else
                        {
                            dont_ctrlz = true;
                        }
                    }


                }

        }else if(edit_shapetype_string.compare("Lines") == 0)
        {

            if(Polygonpx.size() > 0)
            {

                if(m_HasDoneEnter)
                {
                    if(!m_HasDoneCtrlZ)
                    {

                        std::vector<float> plx;
                        std::vector<float> ply;

                        for(int i = 0; i < Polygonpx.size(); i++)
                        {
                            float tlX = Polygonpx.at(i);
                            float tlY = Polygonpy.at(i);

                            LSMVector2 tr = transformer->Transform(LSMVector2(tlX,tlY));

                            plx.push_back(tr.x);
                            ply.push_back(tr.y);
                        }

                        for(int i = 0; i < m_Map.length(); i++)
                        {
                            cTMap * Target = m_Map.at(i);
                            RasterCoordinateList rl = RasterizeLines(Target,plx.data(),ply.data(),plx.size(),edit_sx,edit_ff);
                            m_Edits.append(RasterPaint(Target,rl,f,edit_value,edit_value2,paint_mv_overwrite,paint_mv, mult_f));

                            if(i == 0)
                            {
                                m_RasterLayer->NotifyDataChange(rl.GetBoundingBox());
                            }
                        }
                        m_EditSimultaneity.append(m_Map.length());
                        edited = true;
                        //signal the raster layer to update the opengl texture from the map
                        //and re-calculate the minimum and maximum value

                        m_PolygonPoints.clear();
                        m_PolygonFillX.clear();
                        m_PolygonFillY.clear();
                        Polygonpx.clear();
                        Polygonpy.clear();

                        if(m_PolygonFill != nullptr)
                        {
                            m_PolygonFill->Destroy();
                            delete m_PolygonFill;
                            m_PolygonFill = nullptr;
                        }

                    }else
                    {
                        dont_ctrlz = true;
                    }

                }
            }

        }else if(edit_shapetype_string.compare("Polygon") == 0)
        {

            if(m_PolygonFillX.size() > 0)
            {
               if(m_HasDoneEnter)
               {
                    if(!m_HasDoneCtrlZ)
                    {

                        {
                            std::vector<float> plx;
                            std::vector<float> ply;

                            for(int i = 0; i < m_PolygonFillX.size(); i++)
                            {
                                float tlX = m_PolygonFillX.at(i);
                                float tlY = m_PolygonFillY.at(i);

                                LSMVector2 tr = transformer->Transform(LSMVector2(tlX,tlY));

                                plx.push_back(tr.x);
                                ply.push_back(tr.y);

                                for(int i = 0; i < m_Map.length(); i++)
                                {
                                    cTMap * Target = m_Map.at(i);
                                    RasterCoordinateList rl = RasterizeTriangles(Target,plx.data(),ply.data(),plx.size());
                                    m_Edits.append(RasterPaint(Target,rl,f,edit_value,edit_value2,paint_mv_overwrite,paint_mv, mult_f));

                                    if(i == 0)
                                    {
                                        m_RasterLayer->NotifyDataChange(rl.GetBoundingBox());
                                    }
                                }
                                m_EditSimultaneity.append(m_Map.length());
                                edited = true;
                                //signal the raster layer to update the opengl texture from the map
                                //and re-calculate the minimum and maximum value
                            }
                            m_PolygonPoints.clear();
                            m_PolygonFillX.clear();
                            m_PolygonFillY.clear();
                            Polygonpx.clear();
                            Polygonpy.clear();
                            if(m_PolygonFill != nullptr)
                            {
                                m_PolygonFill->Destroy();
                                delete m_PolygonFill;
                                m_PolygonFill = nullptr;
                            }

                        }

                    }else
                    {
                        dont_ctrlz = true;
                    }
                }

            }
        }

    }else
    {
        if(edit_shapetype_string.compare("Rectangle") == 0 || edit_shapetype_string.compare("Ellips") == 0)
        {
            static auto before = std::chrono::system_clock::now();
            auto now = std::chrono::system_clock::now();
            double diff_ms = std::chrono::duration <double, std::milli> (now - before).count();
            if((m_Dragging && diff_ms > 30)|| m_HasDoneMouseClick )
            {
                before = now;
                if(!m_HasDoneCtrlZ)
                {
                    RasterChangeList rclr;
                    RasterChangeList rclc;


                    for(int l = 0; l < m_Map.length(); l++)
                    {
                        cTMap * Target = m_Map.at(l);

                        RasterCoordinateList rl;
                        if(edit_shapetype_string.compare("Rectangle") == 0)
                        {
                            rl = RasterizeRectangle(Target,edit_cx,edit_cy,edit_sx,edit_sy,edit_ff);

                        }else {
                            rl = RasterizeEllipsoid(Target,edit_cx,edit_cy,edit_sx,edit_sy,edit_ff);
                        }

                        if( l == 0)
                        {
                            rclr = RasterPaint(Target2,rl,f,edit_value,edit_value2,paint_mv_overwrite,paint_mv, mult_f);
                            rclc = RasterPaint(Target3,rl,f,edit_value,edit_value2,paint_mv_overwrite,paint_mv, mult_f);

                            m_Edits.append(rclr);
                            m_Edits.append(rclc);
                        }
                        RasterChangeList rcl;
                        rcl.m_R = rclr.m_R;
                        rcl.m_C = rclr.m_C;
                        for(int i = 0; i < rclr.m_C.size(); i++)
                        {
                            int r = rclr.m_R.at(i);
                            int c = rclr.m_C.at(i);
                            float r_real = rclr.m_Post.at(i);
                            float c_real = rclc.m_Post.at(i);
                            float r_real_old = rclr.m_Pre.at(i);
                            float c_real_old = rclc.m_Pre.at(i);

                            int r_real_i = r_real > 0 ? floor(r_real) : ceil(r_real);
                            int c_real_i = c_real > 0 ? floor(c_real) : ceil(c_real);
                            int r_real_old_i = r_real_old> 0 ? floor(r_real_old) : ceil(r_real_old);
                            int c_real_old_i = c_real_old > 0 ? floor(c_real_old) : ceil(c_real_old);

                            int r_floordif =(r_real_i-r_real_old_i);
                            int c_floordif =(c_real_i-c_real_old_i);

                            int r2 = r_real_i;// +r_floordif;
                            int c2 = c_real_i;// +c_floordif;
                            if(r2 > -1 && r2 < Target->nrRows() && c2 > -1 && c2 < Target->nrCols())
                            {
                                rcl.m_Pre.push_back(Target->data[r][c]);


                                double x = Target->west() + (((float)(c_real))) * Target->cellSizeX();
                                double y = Target->north() + (((float)(r_real))) * Target->cellSizeY();

                                if((x > Target->west()) && (x < Target->west() + ((float)(Target->nrCols()))*  Target->cellSizeX()))
                                {
                                    if(Target->cellSizeX() > 0)
                                    {
                                        x= std::max(Target->west(),std::min(Target->west() + ((double(Target->nrCols()))*  Target->cellSizeX()),x));
                                    }else {
                                        x= std::min(Target->west(),std::max(Target->west() + ((double(Target->nrCols()))*  Target->cellSizeX()),x));
                                    }
                                }
                                if((y > Target->north()) && (y < Target->north() + ((float)(Target->nrRows()))*  Target->cellSizeY()))
                                {
                                    if(Target->cellSizeX() > 0)
                                    {
                                        y= std::max(Target->north(),std::min(Target->north() + ((double(Target->nrRows()))*  Target->cellSizeY()),y));
                                    }else {
                                        y= std::min(Target->north(),std::max(Target->north() + ((double(Target->nrRows()))*  Target->cellSizeY()),y));
                                    }
                                }

                                rcl.m_Post.push_back(m_TMap.at(l)->SampleLinear(x,y));

                            }else
                            {
                                rcl.m_Pre.push_back(Target->data[r][c]);
                                rcl.m_Post.push_back(Target->data[r][c]);
                            }
                        }

                        for(int k = 0; k < rclr.m_C.size(); k++)
                        {
                            int r = rclr.m_R.at(k);
                            int c = rclr.m_C.at(k);

                            Target->data[r][c] = rcl.m_Post.at(k);
                        }

                        if(l == 0)
                        {

                            m_RasterLayer->NotifyDataChange(rl.GetBoundingBox());
                        }

                        m_Edits.append(rcl);
                    }

                    m_EditSimultaneity.append(m_Map.length() + 2);
                    edited = true;
                    //signal the raster layer to update the opengl texture from the map
                    //and re-calculate the minimum and maximum value

                }else
                {
                    dont_ctrlz = true;
                }

            }
        }
    }


    bool undid = false;
    if(m_HasDoneCtrlZ && !dont_ctrlz )
    {

        if(m_Edits.length() > 0 && m_EditSimultaneity.size() > 0)
        {

            undid = true;

            for(int i = 0; i < m_EditSimultaneity.at(m_EditSimultaneity.length()-1); i++)
            {
                RasterUndo(m_Edits.at(m_Edits.length()-1).target,m_Edits.at(m_Edits.length()-1));
                //signal the raster layer to update the opengl texture from the map
                //and re-calculate the minimum and maximum value
                m_RasterLayer->NotifyDataChange(m_Edits.at(m_Edits.length()-1).GetBoundingBox());
                m_Edits.removeAt(m_Edits.length()-1);
            }

            m_EditSimultaneity.removeAt(m_EditSimultaneity.length() -1);
        }
    }

    if(edited || undid)
    {
        m_IsChanged = true;
        m_HasChangedSinceLastScriptCheck = true;

    }



    m_HasDoneEnter = false;
    m_HasDoneCtrlZ = false;
    m_HasDoneMouseClick = false;
    m_HasDoneEscape = false;
    m_HasDoneMouseDoubleClick = false;

    delete transformer;
    m_ScriptMutex.unlock();

}


void UIRasterLayerEditor::OnDrawGeo(OpenGLCLManager * m, GeoWindowState state, WorldGLTransformManager * tm)
{
    cTMap * m_mapd =m_Map.at(0);
    std::cout << "editor draw geo " << m_mapd << std::endl;

    if(IsDraw() &&  m_mapd != nullptr)
    {
        QString edit_shapetype_string = GetParameterValue("Shape");

        if(edit_shapetype_string.compare("Rectangle") == 0 || edit_shapetype_string.compare("Ellips") == 0)
        {
            WorldGLTransform * gltransform = tm->Get(state.projection,m_RasterLayer->GetProjection());


            //Get style
            LSMStyle s = m_RasterLayer->GetStyle();
            BoundingBox bb = m_RasterLayer->GetBoundingBox();

            float hmax = -1e31f;
            float hmin = 1e31f;
            float havg = 0.0f;
            float n = 0.0;

            GeoCoordTransformer * transformer = GeoCoordTransformer::GetCoordTransformer(state.projection,m_RasterLayer->GetProjection());

            LSMVector2 MousePosLoc = transformer->Transform(LSMVector2(state.MouseGeoPosX,state.MouseGeoPosZ));

            delete transformer;

            float edit_cx = MousePosLoc.x;
            float edit_cy = m_Map.at(0)->cellSizeY() > 0? (m_Map.at(0)->north() + (m_Map.at(0)->nrRows() * m_Map.at(0)->cellSizeY()) - MousePosLoc.y) : MousePosLoc.y;
            float edit_sx = GetParameterValueDouble("Shape Size X");
            float edit_sy = GetParameterValueDouble("Shape Size Y");
            float edit_ff = GetParameterValueDouble("Shape Feather");
            float edit_value = GetParameterValueDouble("Value");

            int edit_shapetype = 0;
            if(edit_shapetype_string.compare("Rectangle") == 0)
            {
                edit_shapetype = 0;
            }else if(edit_shapetype_string.compare("Ellips") == 0)
            {
                edit_shapetype = 1;
            }

            if(!(s.m_Intervalb1.GetMax()  == 0.0f && s.m_Intervalb1.GetMin()  == 0.0f) && s.m_Intervalb1.GetMax()  > s.m_Intervalb1.GetMin() )
            {
                hmax = s.m_Intervalb1.GetMax() ;
                hmin = s.m_Intervalb1.GetMin() ;
            }

            //layer geometry
            float l_width = ((float)( m_mapd->nrCols()))* m_mapd->data.cell_sizeX();
            float l_height = ((float)( m_mapd->nrRows()))* m_mapd->data.cell_sizeY();
            float l_cx =  m_mapd->data.west() + 0.5f * l_width;
            float l_cy =  m_mapd->data.north()+ 0.5f * l_height;

            //set shader uniform values
            OpenGLProgram * program = GLProgram_uimapshape;

            // bind shader
            glad_glUseProgram(program->m_program);
            // get uniform locations

            int h_max_loc = glad_glGetUniformLocation(program->m_program,"h_max");
            int h_min_loc = glad_glGetUniformLocation(program->m_program,"h_min");
            int mat_loc = glad_glGetUniformLocation(program->m_program,"matrix");
            int tex_loc = glad_glGetUniformLocation(program->m_program,"tex");
            int tex_x_loc = glad_glGetUniformLocation(program->m_program,"texX");
            int tex_y_loc = glad_glGetUniformLocation(program->m_program,"texY");
            int islegend_loc = glad_glGetUniformLocation(program->m_program,"is_legend");
            int alpha_loc = glad_glGetUniformLocation(program->m_program,"alpha");
            int istransformed_loc = glad_glGetUniformLocation(program->m_program,"is_transformed");
            int istransformedf_loc = glad_glGetUniformLocation(program->m_program,"is_transformedf");
            int israw_loc = glad_glGetUniformLocation(program->m_program,"is_raw");

            std::cout << 1 <<std::endl;
            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"edit_shapetype"),edit_shapetype);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"edit_cx"),edit_cx);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"edit_cy"),edit_cy);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"edit_sx"),edit_sx);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"edit_sy"),edit_sy);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"edit_ff"),edit_ff);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"edit_value"),edit_value);


            std::cout << 1 <<std::endl;
            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_ldd"), m_mapd->AS_IsLDD? 1:0);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizex"),l_width);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizey"),l_height);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixx"),(float)( m_mapd->nrCols()));
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixy"),(float)( m_mapd->nrRows()));
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdx"),(float)( m_mapd->cellSizeX()));
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdy"),(float)( m_mapd->cellSizeY()));
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_transx"),l_cx);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_transy"),l_cy);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowpixsizex"),state.scr_pixwidth);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowpixsizey"),state.scr_pixheight);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowsizex"),state.width);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowsizey"),state.height);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowhwration"),state.scr_ratio);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowtransx"),state.translation_x);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowtransy"),state.translation_y);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_scrwidth"),state.scr_width);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_scrheight"),state.scr_height);

            std::cout << 1 <<std::endl;
            for(int i = 0; i <LISEM_GRADIENT_NCOLORS; i++)
            {
                QString is = QString::number(i);
                int colorstop_i_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop_"+ is).toStdString().c_str());
                int colorstop_ci_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop_c"+ is).toStdString().c_str());

                if(i < s.m_ColorGradientb1.m_Gradient_Stops.length())
                {
                    glad_glUniform1f(colorstop_i_loc,s.m_ColorGradientb1.m_Gradient_Stops.at(i));
                    ColorF c = s.m_ColorGradientb1.m_Gradient_Colors.at(i);
                    glad_glUniform4f(colorstop_ci_loc,c.r,c.g,c.b,c.a);
                }else {
                    glad_glUniform1f(colorstop_i_loc,1e30f);
                    glad_glUniform4f(colorstop_ci_loc,1.0,1.0,1.0,1.0);
                }
            }

            std::cout << 1 <<std::endl;

            // bind texture
            glad_glUniform1i(tex_loc,0);
            glad_glActiveTexture(GL_TEXTURE0);
            glad_glBindTexture(GL_TEXTURE_2D,m_RasterLayer->GetMainTextures().at(0)->m_texgl);


            std::cout << 1 <<std::endl;

            if(gltransform != nullptr)
            {
                if(!gltransform->IsGeneric())
                {
                    BoundingBox b = gltransform->GetBoundingBox();

                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizex"),b.GetSizeX());
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizey"),b.GetSizeY());
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transx"),b.GetCenterX());
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transy"),b.GetCenterY());

                    glad_glUniform1i(istransformed_loc,1);
                    glad_glUniform1i(istransformedf_loc,1);

                    glad_glUniform1i(tex_x_loc,1);
                    glad_glActiveTexture(GL_TEXTURE1);
                    glad_glBindTexture(GL_TEXTURE_2D,gltransform->GetTextureX()->m_texgl);

                    glad_glUniform1i(tex_y_loc,2);
                    glad_glActiveTexture(GL_TEXTURE2);
                    glad_glBindTexture(GL_TEXTURE_2D,gltransform->GetTextureY()->m_texgl);
                }else {

                    glad_glUniform1i(istransformed_loc,0);
                    glad_glUniform1i(istransformedf_loc,0);
                }

            }else
            {
                glad_glUniform1i(istransformed_loc,0);
                glad_glUniform1i(istransformedf_loc,0);
            }


            std::cout << 1 <<std::endl;

            glad_glUniform1i(israw_loc,0);



            glad_glUniform1f(h_max_loc,hmax);
            glad_glUniform1f(h_min_loc,hmin);

            // set project matrix
            glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
            glad_glUniform1i(islegend_loc,0);
            glad_glUniform1f(alpha_loc,1.0f-s.GetTransparancy());

            // now render stuff
            glad_glBindVertexArray(m->m_Quad->m_vao);
            glad_glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
            glad_glBindVertexArray(0);
        }

        if(edit_shapetype_string.compare("Rectangle(drag)") == 0 )
        {
            if(m_HasSquare)
            {
                //draw square from origin position to current position

                float tlX = state.scr_width * (state.MouseGeoPosX - state.tlx)/state.width;
                float tlY = (state.scr_height * (state.MouseGeoPosZ- state.tly)/state.height) ;

                float brX = state.scr_width * (m_DragOrigin.x - state.tlx)/state.width;
                float brY = (state.scr_height * (m_DragOrigin.z- state.tly)/state.height) ;

                if(tlX > brX)
                {
                    float temp = brX;
                    brX = tlX;
                    tlX = temp;
                }
                if(tlY > brY)
                {
                    float temp = brY;
                    brY = tlY;
                    tlY = temp;
                }

                m->m_ShapePainter->DrawSquare(tlX,tlY,brX-tlX,brY-tlY,LSMVector4(1.0,1.0,1.0,0.75));

            }

        }else if(edit_shapetype_string.compare("Lines") == 0)
        {

            if(Polygonpx.size() > 0)
            {
                std::vector<float> plx;
                std::vector<float> ply;

                for(int i = 0; i < Polygonpx.size(); i++)
                {
                    float tlX = state.scr_width * (Polygonpx.at(i) - state.tlx)/state.width;
                    float tlY = (state.scr_height * (Polygonpy.at(i)- state.tly)/state.height) ;

                    plx.push_back(tlX);
                    ply.push_back(tlY);
                }

                float mtlX = state.scr_width * (state.MouseGeoPosX - state.tlx)/state.width;
                float mtlY = (state.scr_height * (state.MouseGeoPosZ- state.tly)/state.height) ;

                plx.push_back(mtlX);
                ply.push_back(mtlY);

                m->m_ShapePainter->DrawLines(plx.data(),ply.data(),std::max(1.0,5.0 * state.ui_scale2d3d),Polygonpx.size()+1,LSMVector4(1.0,1.0,1.0,0.75));
            }

        }else if(edit_shapetype_string.compare("Polygon") == 0)
        {

            {
                if(Polygonpx.size() > 0)
                {
                    std::vector<float> plx;
                    std::vector<float> ply;

                    for(int i = 0; i < Polygonpx.size(); i++)
                    {
                        float tlX = state.scr_width * (Polygonpx.at(i) - state.tlx)/state.width;
                        float tlY = (state.scr_height * (Polygonpy.at(i)- state.tly)/state.height) ;

                        plx.push_back(tlX);
                        ply.push_back(tlY);
                    }

                    float mtlX = state.scr_width * (state.MouseGeoPosX - state.tlx)/state.width;
                    float mtlY = (state.scr_height * (state.MouseGeoPosZ- state.tly)/state.height) ;

                    plx.push_back(mtlX);
                    ply.push_back(mtlY);

                    m->m_ShapePainter->DrawLines(plx.data(),ply.data(),std::max(1.0,5.0 * state.ui_scale2d3d),Polygonpx.size()+1,LSMVector4(0.0,0.0,0.0,0.5));
                }

                if(m_PolygonFillX.size() > 0)
                {
                    {
                        std::vector<float> plx;
                        std::vector<float> ply;

                        for(int i = 0; i < m_PolygonFillX.size(); i++)
                        {
                            float tlX = state.scr_width * (m_PolygonFillX.at(i) - state.tlx)/state.width;
                            float tlY = (state.scr_height * (m_PolygonFillY.at(i)- state.tly)/state.height) ;

                            plx.push_back(tlX);
                            ply.push_back(tlY);
                        }
                        m->m_ShapePainter->DrawPolygon(plx.data(),ply.data(),m_PolygonFillX.size(),LSMVector4(1.0,1.0,1.0,0.75));
                    }
                }
            }
        }

    }

    std::cout << "end draw geo editor raster " << std::endl;

}

inline void UIRasterLayerEditor::OnClose()
{
    m_ScriptMutex.lock();
    for(int r = 0; r < m_Map.at(0)->nrRows(); r++)
    {
        for(int c = 0; c < m_Map.at(0)->nrCols(); c++)
        {
            for(int i = 0; i < m_Map.length(); i++)
            {
                m_Map.at(i)->Drc = m_OriginalMap.at(i)->Drc;
            }
        }
    }
    m_RasterLayer->NotifyDataChange(BoundingBox(0,m_Map.at(0)->nrRows()-1,0,m_Map.at(0)->nrCols()-1));

    m_ScriptMutex.unlock();
}
