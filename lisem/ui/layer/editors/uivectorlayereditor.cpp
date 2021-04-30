#include "layer/geo/uivectorlayer.h"
#include "geo/shapeediting.h"
#include "widgets/layerinfowidget.h"

UIVectorLayerEditor::UIVectorLayerEditor(UILayer * rl) : UILayerEditor(rl)
{

    m_VectorLayer =dynamic_cast<UIVectorLayer*>( rl);
    m_VectorLayer->SetDrawPoints(true);
    m_FilePath = m_VectorLayer->GetFilePath();

    m_Shapes = m_VectorLayer->GetShapeFile();
    m_OriginalShapes = new ShapeFile();
    m_OriginalShapes->CopyFrom(m_Shapes);

    AddParameter(UI_PARAMETER_TYPE_GROUP,"Vector Tool","","");
    AddParameter(UI_PARAMETER_TYPE_RADIO,"Selection","Shape|Feature|Vertices|Shapes|Features","Shape");
    AddParameter(UI_PARAMETER_TYPE_RADIO,"Type","Select (alt)|Add/Remove Shape|Edit|Move|Attributes|Split","Select (alt)");
    AddParameter(UI_PARAMETER_TYPE_RADIO,"Shape","Polygon|Rectangle|Circle|Lines|Rectangle (Outline)|Circle (Outline)|Point","Rectangle");
    AddParameter(UI_PARAMETER_TYPE_FLOAT,"Value","Value used as parameter to the chosen function","1",-1e31,1e31);
    AddParameter(UI_PARAMETER_TYPE_FLOAT,"Circle Points","Number of points used to build a polygon approximating a circle","25",3,10000);
    AddParameter(UI_PARAMETER_TYPE_BOOL,"Ask For Attributes","Show dialog to enter attributes when adding a feature","true");
    AddParameter(UI_PARAMETER_TYPE_BOOL,"Topological editing","When moving vertices, move sufficiently close vertices with it","true");
    AddParameter(UI_PARAMETER_TYPE_FLOAT,"Topological distance","Maximum distance (in geo-spatial units) for topological editing","0.");

    std::cout << "edit feature count " << m_Shapes->GetLayer(0)->GetFeatureCount() << std::endl;
}

MatrixTable * UIVectorLayerEditor::DoTableDialog(OpenGLCLManager * m, MatrixTable * t)
{



    std::function<void()> f = [t, this]()
    {

        LayerInfoWidget *w = new LayerInfoWidget(t,true,false,false,true,false,false,false);
        if(QDialog::Accepted == w->exec())
        {
           this->m_DialogReturn = 1;
        }else
        {
           this->m_DialogReturn = 0;
        }
        this->m_DialogResult = w->GetMatrixTable();

    ;
    };

    m->DoDialog(f);

    return m_DialogResult;
}


void UIVectorLayerEditor::OnDraw(OpenGLCLManager * m,GeoWindowState state)
{

    if(m_StartedDragging)
    {
        m_DragPrevious = m_DragOrigin;

    }
    if(m_Dragging)
    {
        m_DragDelta = LSMVector3(state.MouseGeoPosX,state.MouseGeoPosY,state.MouseGeoPosZ) - m_DragPrevious;

    }

    QString edit_type_string = GetParameterValue("Type");
    QString edit_shape_string = GetParameterValue("Shape");

    LSMVector2 locn = m_Transformer->Transform(LSMVector2(state.MouseGeoPosX,state.MouseGeoPosZ));

    double tolerence = 0.02 *std::min(state.width,state.height);
    QList<Feature *> features = m_Shapes->GetFeaturesAt(locn,tolerence);

    LSMShape ** s_close = new (LSMShape*)();
    Feature ** f_close = new (Feature*)();

    m_closestVertex = m_Shapes->GetClosestVertex(locn,nullptr,f_close,s_close);

    m_closestEdgeLocation = m_Shapes->GetClosestEdgeLocation(locn);


    LSMVector2 locn1 = m_Transformerinv->Transform(m_closestVertex);
    LSMVector2 locn2 = m_Transformerinv->Transform(m_closestEdgeLocation);

    float tlX_vertex = state.scr_width * (locn1.x - state.tlx)/state.width;
    float tlY_vertex = (state.scr_height * (locn1.y - state.tly)/state.height) ;

    float tlX_edge = state.scr_width * (locn2.x - state.tlx)/state.width;
    float tlY_edge = (state.scr_height * (locn2.y - state.tly)/state.height) ;

    if(features.length() > 0)
    {
        m_VectorLayer->DrawFeature(features.at(0),state,m);
    }

    bool dont_ctrlz = false;
    bool edited = false;

    if(edit_type_string.compare("Add/Remove Shape") == 0)
    {
        if(this->m_HasDoneEnter || (this->m_HasDoneMouseClick &&edit_shape_string.compare("Point") == 0) )
        {

            std::cout << "has done enter "<< std::endl;

            if(!m_HasDoneCtrlZ)
            {
                if(!(m_Shapes == nullptr))
                {
                    if(m_Shapes->GetLayerCount() > 0)
                    {
                        if(edit_shape_string.compare("Polygon") == 0 && Polygonpx.size() > 2)
                        {

                            std::cout << "create polygon" << std::endl;

                            int success;
                            QList<QString> attrvalues;

                            if(m_AskAttributes)
                            {
                                MatrixTable * attroptions = new MatrixTable();
                                attroptions->SetSize(m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2,3);
                                for(int i = 0; i < m_Shapes->GetLayer(0)->GetNumberOfAttributes(); i++)
                                {
                                    attroptions->SetValue(i+2,0,m_Shapes->GetLayer(0)->GetAttributeList(i)->GetTypeAsString());
                                    attroptions->SetValue(i+2,1,m_Shapes->GetLayer(0)->GetAttributeName(i));
                                }

                                MatrixTable * attroptionsn = DoTableDialog(m,attroptions);

                                success = GetResultingMatrixTableCode();
                                attrvalues = attroptionsn->GetVerticalStringList(2,2,m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2-1);

                                std::cout << "add polygon, " << success << std::endl;
                                delete attroptions;
                                delete attroptionsn;

                            }else {
                                success = 1;
                            }

                            if(success)
                            {
                                Feature * fnew = new Feature();
                                LSMPolygon * snew = new LSMPolygon();

                                Ring * ring = new Ring();
                                for(int k = 0; k < Polygonpx.size(); k++)
                                {
                                    LSMVector2 point = m_Transformer->Transform(LSMVector2(Polygonpx.at(k),Polygonpy.at(k)));
                                    ring->AddVertex(point.x,point.y);
                                }
                                snew->SetExtRing(ring);
                                fnew->AddShape(snew);

                                std::cout << "Layer Count " << m_Shapes->GetLayerCount()<< std::endl;
                                if(m_Shapes->GetLayerCount()>0)
                                {
                                    std::cout << "add " << std::endl;

                                    ShapeLayerChange lc = ShapeLayerChangeAddFeature(m_Shapes,m_Shapes->GetLayer(0),fnew,attrvalues);
                                    ApplyShapeLayerChange(m_Shapes,lc);
                                    m_Edits.append(lc);
                                    m_EditsSimultaneityCount.append(1);
                                    edited = true;
                                    m_VectorLayer->NotifyDataChange();

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
                        }else if(m_HasSquare && (edit_shape_string.compare("Rectangle") == 0 || edit_shape_string.compare("Rectangle (Outline)") == 0))
                        {
                            MatrixTable * attroptions = new MatrixTable();
                            attroptions->SetSize(m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2,3);
                            for(int i = 0; i < m_Shapes->GetLayer(0)->GetNumberOfAttributes(); i++)
                            {
                                attroptions->SetValue(i+2,0,QString::number(m_Shapes->GetLayer(0)->GetAttributeList(i)->m_type));
                                attroptions->SetValue(i+2,1,m_Shapes->GetLayer(0)->GetAttributeName(i));
                            }
                            MatrixTable * attroptionsn = DoTableDialog(m,attroptions);

                            int success = GetResultingMatrixTableCode();
                            QList<QString> attrvalues = attroptionsn->GetVerticalStringList(2,2,m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2-1);

                            delete attroptions;
                            delete attroptionsn;
                            if(success)
                            {
                                LSMVector3 endpos = m_Dragging? LSMVector3(state.MouseGeoPosX,0.0,state.MouseGeoPosZ):m_DragEnd;

                                LSMVector2 br = m_Transformer->Transform(LSMVector2(endpos.x,endpos.z));
                                LSMVector2 tl = m_Transformer->Transform(LSMVector2(m_DragOrigin.x,m_DragOrigin.z));

                                if(m_Shapes->GetLayerCount()>0)
                                {

                                    Feature * fnew = new Feature();
                                    if(edit_shape_string.compare("Rectangle") == 0)
                                    {
                                        LSMPolygon * snew = new LSMPolygon();
                                        snew->SetAsSquare(tl,br);
                                        fnew->AddShape(snew);
                                    }else {
                                        LSMLine * snew = new LSMLine();
                                        snew->SetAsSquare(tl,br);
                                        fnew->AddShape(snew);
                                    }

                                    ShapeLayerChange lc = ShapeLayerChangeAddFeature(m_Shapes,m_Shapes->GetLayer(0),fnew,attrvalues);
                                    ApplyShapeLayerChange(m_Shapes,lc);
                                    m_Edits.append(lc);
                                    m_EditsSimultaneityCount.append(1);
                                    edited = true;
                                    m_VectorLayer->NotifyDataChange();

                                }

                                m_HasSquare = false;

                            }

                        }else if(m_HasCircle && (edit_shape_string.compare("Circle") == 0 || edit_shape_string.compare("Circle (Outline)") == 0))
                        {
                            MatrixTable * attroptions = new MatrixTable();
                            attroptions->SetSize(m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2,3);
                            for(int i = 0; i < m_Shapes->GetLayer(0)->GetNumberOfAttributes(); i++)
                            {
                                attroptions->SetValue(i+2,0,QString::number(m_Shapes->GetLayer(0)->GetAttributeList(i)->m_type));
                                attroptions->SetValue(i+2,1,m_Shapes->GetLayer(0)->GetAttributeName(i));
                            }
                            MatrixTable * attroptionsn = DoTableDialog(m,attroptions);

                            int success = GetResultingMatrixTableCode();
                            QList<QString> attrvalues = attroptionsn->GetVerticalStringList(2,2,m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2-1);

                            delete attroptions;
                            delete attroptionsn;
                            if(success)
                            {
                                //draw square from origin position to current position

                                LSMVector3 endpos = m_Dragging? LSMVector3(state.MouseGeoPosX,0.0,state.MouseGeoPosZ):m_DragEnd;

                                LSMVector2 br = m_Transformer->Transform(LSMVector2(endpos.x,endpos.z));
                                LSMVector2 tl = m_Transformer->Transform(LSMVector2(m_DragOrigin.x,m_DragOrigin.z));

                                float r = sqrt((tl.y - br.y)*(tl.y - br.y) + (tl.x - br.x)*(tl.x - br.x));

                                if(m_Shapes->GetLayerCount()>0)
                                {
                                    Feature * fnew = new Feature();
                                    if(edit_shape_string.compare("Circle") == 0)
                                    {
                                        LSMPolygon * snew = new LSMPolygon();
                                        snew->SetAsRegularNGon(tl,r);
                                        fnew->AddShape(snew);
                                    }else {
                                        LSMLine * snew = new LSMLine();
                                        snew->SetAsRegularNGon(tl,r);
                                        fnew->AddShape(snew);
                                    }
                                    ShapeLayerChange lc = ShapeLayerChangeAddFeature(m_Shapes,m_Shapes->GetLayer(0),fnew,attrvalues);
                                    ApplyShapeLayerChange(m_Shapes,lc);
                                    m_Edits.append(lc);
                                    m_EditsSimultaneityCount.append(1);
                                    edited = true;
                                    m_VectorLayer->NotifyDataChange();

                                }
                                m_HasCircle = false;

                            }

                        }else if(edit_shape_string.compare("Lines") == 0 && Polygonpx.size() > 1)
                        {
                            MatrixTable * attroptions = new MatrixTable();
                            attroptions->SetSize(m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2,3);
                            for(int i = 0; i < m_Shapes->GetLayer(0)->GetNumberOfAttributes(); i++)
                            {
                                attroptions->SetValue(i+2,0,QString::number(m_Shapes->GetLayer(0)->GetAttributeList(i)->m_type));
                                attroptions->SetValue(i+2,1,m_Shapes->GetLayer(0)->GetAttributeName(i));
                            }
                            MatrixTable * attroptionsn = DoTableDialog(m,attroptions);

                            int success = GetResultingMatrixTableCode();
                            QList<QString> attrvalues = attroptionsn->GetVerticalStringList(2,2,m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2-1);

                            delete attroptions;
                            delete attroptionsn;
                            if(success)
                            {
                                if(m_Shapes->GetLayerCount()>0)
                                {
                                    Feature * fnew = new Feature();
                                    LSMLine* snew = new LSMLine();


                                    for(int k = 0; k < Polygonpx.size(); k++)
                                    {
                                        LSMVector2 point = m_Transformer->Transform(LSMVector2(Polygonpx.at(k),Polygonpy.at(k)));
                                        snew->AddVertex(point.x,point.y);
                                    }
                                    fnew->AddShape(snew);

                                    ShapeLayerChange lc = ShapeLayerChangeAddFeature(m_Shapes,m_Shapes->GetLayer(0),fnew,attrvalues);
                                    ApplyShapeLayerChange(m_Shapes,lc);
                                    m_Edits.append(lc);
                                    m_EditsSimultaneityCount.append(1);
                                    edited = true;
                                    m_VectorLayer->NotifyDataChange();

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


                        }else if(edit_shape_string.compare("Point") == 0)
                        {
                            MatrixTable * attroptions = new MatrixTable();
                            attroptions->SetSize(m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2,3);
                            for(int i = 0; i < m_Shapes->GetLayer(0)->GetNumberOfAttributes(); i++)
                            {
                                attroptions->SetValue(i+2,0,QString::number(m_Shapes->GetLayer(0)->GetAttributeList(i)->m_type));
                                attroptions->SetValue(i+2,1,m_Shapes->GetLayer(0)->GetAttributeName(i));
                            }
                            MatrixTable * attroptionsn = DoTableDialog(m, attroptions);

                            int success = GetResultingMatrixTableCode();
                            QList<QString> attrvalues = attroptionsn->GetVerticalStringList(2,2,m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2-1);

                            delete attroptions;
                            delete attroptionsn;
                            if(success)
                            {
                                LSMVector2 p = m_Transformer->Transform(LSMVector2(state.MouseGeoPosX,state.MouseGeoPosZ));
                                if(m_Shapes->GetLayerCount()>0)
                                {
                                    Feature * fnew = new Feature();
                                    LSMPoint * point = new LSMPoint();
                                    point->SetPos(p.x,p.y);
                                    ShapeLayerChange lc = ShapeLayerChangeAddFeature(m_Shapes,m_Shapes->GetLayer(0),fnew,attrvalues);
                                    ApplyShapeLayerChange(m_Shapes,lc);
                                    m_Edits.append(lc);
                                    m_EditsSimultaneityCount.append(1);
                                    edited = true;
                                    m_VectorLayer->NotifyDataChange();

                                }
                            }

                        }
                    }
                }
            }else
            {
                dont_ctrlz = true;
            }
        }

    }else if(edit_type_string.compare("Split") == 0)
    {
        if(this->m_HasDoneEnter && Polygonpx.size() > 1)
        {
            std::cout << "split 2 " << std::endl;

            int edits= 0;
            std::vector<double> x;
            std::vector<double> y;
            for(int k = 0; k < Polygonpx.size(); k++)
            {
                LSMVector2 point = m_Transformer->Transform(LSMVector2(Polygonpx.at(k),Polygonpy.at(k)));
                x.push_back(point.x);
                y.push_back(point.y);
            }

            for(int i =m_Shapes->GetLayer(0)->GetFeatureCount()-1; i>-1 ; i--)
            {
                Feature * f = m_Shapes->GetLayer(0)->GetFeature(i);
                QList<QString> attribvalues = m_Shapes->GetLayer(0)->GetAttributeValuesForFeature(i);


                for(int j = 0; j < f->GetShapeCount(); j++)
                {
                    LSMShape * s = f->GetShape(j);
                    QList<LSMShape *> ss = s->Split(x,y);

                    std::cout << "split size " << ss.size() << std::endl;
                    if(ss.size() > 1)
                    {

                        //first delete old
                        if(f->GetShapeCount() > 1){
                            //delete current shape only
                            ShapeLayerChange lc = ShapeLayerChangeRemoveShape(m_Shapes,s);
                            ApplyShapeLayerChange(m_Shapes,lc);
                            m_Edits.append(lc);
                            edits ++;
                        }else
                        {
                            //delete entire feature
                            ShapeLayerChange lc = ShapeLayerChangeRemoveFeature(m_Shapes,f);
                            ApplyShapeLayerChange(m_Shapes,lc);
                            m_Edits.append(lc);
                            edits ++;
                        }


                        //then add all the new ones as seperate features
                        for(int k =0; k < ss.size(); k++)
                        {
                            Feature * fn = new Feature();
                            fn->AddShape(ss.at(k));
                            ShapeLayerChange lc = ShapeLayerChangeAddFeature(m_Shapes,m_Shapes->GetLayer(0),fn,attribvalues);
                            ApplyShapeLayerChange(m_Shapes,lc);
                            m_Edits.append(lc);
                            edits ++;
                        }
                    }
                }
            }

            if(edits > 0)
            {
                std::cout << "split n " << edits << std::endl;
                edited = true;
                m_VectorLayer->NotifyDataChange();
                m_EditsSimultaneityCount.append(edits);

            }
        }
    }
    else if(edit_type_string.compare("Edit") == 0 || edit_type_string.compare("Move") == 0)
    {

        if(m_StartedDragging)
        {
            //starting drag, see if we meet criteria
            if(!((*s_close == nullptr)|| (*f_close == nullptr)))
            {
                LSMShape * shapeclose = *s_close;
                Feature * featureclose = *f_close;

                float pixdist_vertex = sqrt((tlX_vertex - state.MousePosX)*(tlX_vertex - state.MousePosX) + (tlY_vertex - state.MousePosY)*(tlY_vertex - state.MousePosY));

                if(edit_type_string.compare("Edit") == 0)
                {
                    if(pixdist_vertex < 100 )
                    {
                        SetSelected(featureclose,shapeclose);

                        if(s_close != nullptr)
                        {
                                if(*s_close != nullptr)
                                {
                                    (*s_close)->GetClosestVertexP(locn,x1t,y1t);
                                }
                        }

                    }else
                    {
                        SetSelected(nullptr, nullptr);
                    }
                }else if(edit_type_string.compare("Move") == 0)
                {
                    /*if(features.length() > 0)
                    {
                        for(int i = 0; i < features.at(0)->GetShapeCount(); i++)
                        {
                            if(features.at(0)->GetShape(i)->Contains(locn,tolerence))
                            {
                                SetSelected(features.at(0),features.at(0)->GetShape(i));
                                break;
                            }
                        }
                    }else
                    {
                        SetSelected(nullptr, nullptr);
                    }*/
                }
            }else
            {
                SetSelected(nullptr, nullptr);
            }
        }
        if(edit_type_string.compare("Edit") == 0)
        {
            //add vertex
            if(m_HasDoneMouseDoubleClick)
            {

            }
            //remove vertex?
            if(m_HasDoneRightMouseClick)
            {


            }
        }
        if(m_Dragging)
        {
            std::cout << "dragging shape "<< std::endl;
            //during drag, update the selected shape only
            LSMVector2 pos_dragold = LSMVector2(m_DragPrevious.X(),m_DragPrevious.Z());
            LSMVector2 pos_dragcurrent = LSMVector2(state.MouseGeoPosX,state.MouseGeoPosZ);

            LSMVector2 pos_dragoldn = m_Transformer->Transform(pos_dragold);
            LSMVector2 pos_dragcurrentn = m_Transformer->Transform(pos_dragcurrent);

            LSMVector2 dragv = pos_dragcurrentn - pos_dragoldn;
            LSMShape * shapeclose = GetSelectedShapeC();

                if(edit_type_string.compare("Edit") == 0)
                {
                    if(shapeclose != nullptr)
                    {
                        shapeclose->MoveVertex(*x1t,*y1t,dragv);
                    }
                }else if(edit_type_string.compare("Move") == 0)
                {
                    //shapeclose->Move(dragv);
                    m_DragOffset = m_DragOffset+ dragv;
                }

        }

        if(m_StoppedDragging)
        {
            //when stopping the drag, apply the changes to the shapefile

            std::cout << "stop dragging " << std::endl;
            LSMShape * shapeclose = GetSelectedShapeC();
            LSMShape * shapecloseorg = GetSelectedShape();
            Feature * featureclose = GetSelectedFeature();


            if(m_SelectedFeatures.length() > 0)
            {
                QList<Feature*> newselect;
                for(int i = 0; i < m_SelectedFeatures.length(); i++)
                {
                    Feature * news=  m_SelectedFeatures.at(i)->Copy();
                    news->Move(m_DragOffset);
                    ShapeLayerChange lc = ShapeLayerChangeReplaceFeature(m_Shapes,m_SelectedFeatures.at(i),news);
                    ApplyShapeLayerChange(m_Shapes,lc);
                    m_Edits.append(lc);
                    newselect.append(news);
                }

                edited = true;
                m_VectorLayer->NotifyDataChange();
                m_EditsSimultaneityCount.append(m_SelectedFeatures.length());
                ClearSelected();
                m_SelectedFeatures = newselect;

            }else if(m_SelectedShapes.length() > 0)
            {
                QList<std::pair<LSMShape *,Feature*>> newselect;
                for(int i = 0; i < m_SelectedShapes.length(); i++)
                {
                    LSMShape * news= m_SelectedShapes.at(i).first->Copy();
                    news->Move(m_DragOffset);
                    ShapeLayerChange lc = ShapeLayerChangeReplaceShape(m_Shapes,m_SelectedShapes.at(i).first,news);
                    ApplyShapeLayerChange(m_Shapes,lc);
                    m_Edits.append(lc);
                    newselect.append(std::pair<LSMShape *,Feature*>(news,m_SelectedShapes.at(i).second));
                }

                edited = true;
                m_VectorLayer->NotifyDataChange();
                m_EditsSimultaneityCount.append(m_SelectedShapes.length());
                ClearSelected();
                m_SelectedShapes = newselect;


            }else if(m_SelectedVertices.length() > 0)
            {
                int edits = 0;
                QList<std::tuple<double*,double*,LSMShape *,Feature *>> newsselect;

                for(int i = 0; i < m_Shapes->GetLayer(0)->GetFeatureCount(); i++)
                {
                    Feature * f = m_Shapes->GetLayer(0)->GetFeature(i);

                    for(int j = 0; j < f->GetShapeCount(); j++)
                    {
                        LSMShape * s = f->GetShape(j);
                        LSMShape * sc;
                        bool found = false;

                        for(int k = 0; k < s->GetVertexCount(); k++)
                        {
                            bool found_this = false;
                            double * xvp = nullptr;
                            double * yvp = nullptr;
                            s->GetVertexP(k,&xvp,&yvp);
                            if(!(xvp == nullptr || yvp == nullptr))
                            {
                                for(int l = 0; l < m_SelectedVertices.size(); l++)
                                {
                                    std::tuple<double*,double*,LSMShape *,Feature *> t = m_SelectedVertices.at(l);
                                    if(std::get<0>(t) == xvp && std::get<1>(t) == yvp)
                                    {
                                        found_this = true;
                                        break;
                                    }
                                }

                            }
                            if(found_this)
                            {
                                if(!found)
                                {
                                   sc = f->GetShape(j)->Copy();
                                }


                                double * xvp2 = nullptr;
                                double * yvp2 = nullptr;
                                sc->GetVertexP(k,&xvp2,&yvp2);

                                if(!(xvp2 == nullptr || yvp2 == nullptr))
                                {
                                    *xvp2 += m_DragOffset.x;
                                    *yvp2 += m_DragOffset.y;

                                    newsselect.append(std::tuple<double*,double*,LSMShape *,Feature*>(xvp2,yvp2,sc,f));
                                }

                                found = true;
                            }
                        }

                        if(found)
                        {
                            edits++;
                            ShapeLayerChange lc = ShapeLayerChangeReplaceShape(m_Shapes,s,sc);
                            ApplyShapeLayerChange(m_Shapes,lc);
                            m_Edits.append(lc);

                        }
                    }


                }
                if(edits > 0)
                {
                    edited = true;
                    m_VectorLayer->NotifyDataChange();
                    m_EditsSimultaneityCount.append(edits);
                    ClearSelected();
                    m_SelectedVertices = newsselect;

                }



            }else if(m_SelectedFeature != nullptr || m_SelectedShape != nullptr)
            {

                if(m_SelectedShape == nullptr)
                {
                    Feature * news= m_SelectedFeature->Copy();
                    news->Move(m_DragOffset);
                    ShapeLayerChange lc = ShapeLayerChangeReplaceFeature(m_Shapes,m_SelectedFeature,news);
                    ApplyShapeLayerChange(m_Shapes,lc);
                    m_Edits.append(lc);
                    m_EditsSimultaneityCount.append(1);
                    edited = true;
                    m_VectorLayer->NotifyDataChange();
                    SetSelected(news,nullptr);
                }else
                {

                    LSMShape * news= m_SelectedShape->Copy();
                    news->Move(m_DragOffset);
                    ShapeLayerChange lc = ShapeLayerChangeReplaceShape(m_Shapes,m_SelectedShape,news);
                    ApplyShapeLayerChange(m_Shapes,lc);
                    m_Edits.append(lc);
                    m_EditsSimultaneityCount.append(1);
                    edited = true;
                    m_VectorLayer->NotifyDataChange();
                    SetSelected(m_SelectedFeature,news);
                }

            }

            m_DragOffset = LSMVector2(0.0,0.0);

        }

    }else if(edit_type_string.compare("Edit Attributes") == 0)
    {


        if(m_SelectedFeature != nullptr)
        {
            int findex = m_Shapes->GetLayer(0)->GetIndexOfFeature(m_SelectedFeature);
            if(findex > -1)
            {
                QList<QString> attribs =  m_Shapes->GetLayer(0)->GetAttributeValuesForFeature(findex);
                MatrixTable * attroptions = new MatrixTable();
                attroptions->SetSize(m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2,2);
                for(int i = 0; i < m_Shapes->GetLayer(0)->GetNumberOfAttributes(); i++)
                {
                    attroptions->SetValue(i+2,0,QString::number(m_Shapes->GetLayer(0)->GetAttributeList(i)->m_type));
                    attroptions->SetValue(i+2,1,m_Shapes->GetLayer(0)->GetAttributeName(i));
                    attroptions->SetValue(i+2,2,attribs.at(i));
                }
                MatrixTable * attroptionsn = DoTableDialog(m,attroptions);

                int success = GetResultingMatrixTableCode();
                QList<QString> attrvalues = attroptionsn->GetVerticalStringList(2,2,m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2-1);




                delete attroptions;
                delete attroptionsn;

                m_DoAlterAttributeDialog = false;
                m_DoAlterAttributeFeature = nullptr;
            }
        }else if(m_SelectedFeatures.size() > 0)
        {
            MatrixTable * attroptions = new MatrixTable();
            attroptions->SetSize(m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2,2);
            for(int i = 0; i < m_Shapes->GetLayer(0)->GetNumberOfAttributes(); i++)
            {
                attroptions->SetValue(i+2,0,QString::number(m_Shapes->GetLayer(0)->GetAttributeList(i)->m_type));
                attroptions->SetValue(i+2,1,m_Shapes->GetLayer(0)->GetAttributeName(i));
            }
            MatrixTable * attroptionsn = DoTableDialog(m,attroptions);

            int success = GetResultingMatrixTableCode();
            QList<QString> attrvalues = attroptionsn->GetVerticalStringList(2,2,m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2-1);


            //get only the filled-in values and replace those in all selected features




            delete attroptions;
            delete attroptionsn;

            m_DoAlterAttributeDialog = false;
            m_DoAlterAttributeFeature = nullptr;
        }
    }


    if(m_HasDoneDelete && m_Shapes->GetLayerCount() > 0)
    {
        if(edit_type_string.compare("Select (alt)") == 0 || edit_type_string.compare("Edit Attributes") == 0 || edit_type_string.compare("Add/Remove Shape") == 0 || edit_type_string.compare("Edit Shape") == 0 || edit_type_string.compare("Move") == 0)
        {
            if(!m_HasDoneCtrlZ)
            {
                if(m_SelectedFeatures.length() > 0)
                {
                    for(int i = 0; i < m_SelectedFeatures.length(); i++)
                    {
                        ShapeLayerChange lc = ShapeLayerChangeRemoveFeature(m_Shapes,m_SelectedFeatures.at(i));
                        ApplyShapeLayerChange(m_Shapes,lc);
                        m_Edits.append(lc);
                    }

                    edited = true;
                    m_VectorLayer->NotifyDataChange();
                    m_EditsSimultaneityCount.append(m_SelectedFeatures.length());

                }else if(m_SelectedShapes.length() > 0)
                {
                    for(int i = 0; i < m_SelectedShapes.length(); i++)
                    {
                        ShapeLayerChange lc = ShapeLayerChangeRemoveShape(m_Shapes,m_SelectedShapes.at(i).first);
                        ApplyShapeLayerChange(m_Shapes,lc);
                        m_Edits.append(lc);
                    }

                    edited = true;
                    m_VectorLayer->NotifyDataChange();
                    m_EditsSimultaneityCount.append(m_SelectedShapes.length());


                }else if(m_SelectedVertices.length() > 0)
                {

                    int edits = 0;

                    for(int i = 0; i < m_Shapes->GetLayer(0)->GetFeatureCount(); i++)
                    {
                        Feature * f = m_Shapes->GetLayer(0)->GetFeature(i);

                        for(int j = f->GetShapeCount()-1; j < -1; j--)
                        {
                            LSMShape * s = f->GetShape(j);
                            LSMShape * sc;
                            bool found = false;
                            bool do_delet = false;
                            if(s->GetVertexCount() == 1)
                            {
                                do_delet = true;

                            }

                                for(int k = s->GetVertexCount()-1; k > -1 ; k--)
                                {
                                    bool found_this = false;
                                    double * xvp = nullptr;
                                    double * yvp = nullptr;
                                    s->GetVertexP(k,&xvp,&yvp);
                                    if(!(xvp == nullptr || yvp == nullptr))
                                    {
                                        for(int l = 0; l < m_SelectedVertices.size(); l++)
                                        {
                                            std::tuple<double*,double*,LSMShape *,Feature *> t = m_SelectedVertices.at(l);
                                            if(std::get<0>(t) == xvp && std::get<1>(t) == yvp)
                                            {
                                                    found_this = true;
                                                    break;
                                            }
                                        }
                                    }
                                    if(found_this)
                                    {
                                        if(s->GetVertexCount() == 1)
                                        {
                                            found = true;
                                            break;

                                        }else
                                        {
                                            if(!found)
                                            {
                                               sc = f->GetShape(j)->Copy();
                                            }
                                            sc->RemoveVertex(k);
                                            found = true;
                                        }
                                    }
                                }

                                if(found)
                                {
                                    if(do_delet)
                                    {
                                        edits++;
                                        ShapeLayerChange lc = ShapeLayerChangeRemoveShape(m_Shapes,s);
                                        ApplyShapeLayerChange(m_Shapes,lc);
                                        m_Edits.append(lc);
                                    }else
                                    {
                                        edits++;
                                        ShapeLayerChange lc = ShapeLayerChangeReplaceShape(m_Shapes,s,sc);
                                        ApplyShapeLayerChange(m_Shapes,lc);
                                        m_Edits.append(lc);
                                    }
                                }
                        }
                    }
                    if(edits > 0)
                    {
                        edited = true;
                        m_VectorLayer->NotifyDataChange();
                        m_EditsSimultaneityCount.append(edits);
                        ClearSelected();

                    }


                }else if(m_SelectedFeature != nullptr || m_SelectedShape != nullptr)
                {

                    if(m_SelectedShape == nullptr)
                    {

                        ShapeLayerChange lc = ShapeLayerChangeRemoveFeature(m_Shapes,m_SelectedFeature);
                        ApplyShapeLayerChange(m_Shapes,lc);
                        m_Edits.append(lc);
                        m_EditsSimultaneityCount.append(1);
                        edited = true;
                        m_VectorLayer->NotifyDataChange();
                    }else
                    {
                        ShapeLayerChange lc = ShapeLayerChangeRemoveShape(m_Shapes,m_SelectedShape);
                        ApplyShapeLayerChange(m_Shapes,lc);
                        m_Edits.append(lc);
                        m_EditsSimultaneityCount.append(1);
                        edited = true;
                        m_VectorLayer->NotifyDataChange();
                    }

                }


            }else
            {
                dont_ctrlz = true;
            }
        }
        ClearSelected();
    }

    bool undid = false;
    if(m_HasDoneCtrlZ && !dont_ctrlz)
    {
        if(m_Edits.length() > 0)
        {
            int edit_length = m_EditsSimultaneityCount.at(m_EditsSimultaneityCount.length()-1);

            for(int i = 0; i < edit_length; i++)
            {
                UndoShapeLayerChange(m_Shapes,m_Edits.at(m_Edits.length()-1));
                m_Edits.removeAt(m_Edits.length()-1);
            }
            m_EditsSimultaneityCount.removeAt(m_EditsSimultaneityCount.length()-1);

            undid = true;
            edited = true;
            m_VectorLayer->NotifyDataChange();
        }


    }

    if(edited)
    {
        m_IsChanged = true;
    }

    m_HasDoneDelete = false;
    m_HasDoneEnter = false;
    m_HasDoneCtrlZ = false;
    m_HasDoneMouseClick = false;
    m_HasDoneRightMouseClick = false;
    m_HasDoneEscape = false;
    m_HasDoneMouseDoubleClick = false;

    m_StartedDragging = false;
    m_StoppedDragging = false;

    if(m_Dragging)
    {
        m_DragPrevious = LSMVector3(state.MouseGeoPosX,state.MouseGeoPosY,state.MouseGeoPosZ);
    }

    delete s_close;
    delete f_close;

}



void UIVectorLayerEditor::OnDrawGeo(OpenGLCLManager * m, GeoWindowState state, WorldGLTransformManager * tm)
{

    LSMVector2 locn = m_Transformer->Transform(LSMVector2(state.MouseGeoPosX,state.MouseGeoPosZ));

    double tolerence = 0.02 *std::min(state.width,state.height);
    QList<Feature *> features = m_Shapes->GetFeaturesAt(locn,tolerence);

    LSMVector2 locn1 = m_Transformerinv->Transform(m_closestVertex);
    LSMVector2 locn2 = m_Transformerinv->Transform(m_closestEdgeLocation);

    float tlX_vertex = state.scr_width * (locn1.x - state.tlx)/state.width;
    float tlY_vertex = (state.scr_height * (locn1.y - state.tly)/state.height) ;

    m->m_ShapePainter->DrawSquare(tlX_vertex -4,tlY_vertex-4,8,8,LSMVector4(1.0,1.0,1.0,0.75));

    float tlX_edge = state.scr_width * (locn2.x - state.tlx)/state.width;
    float tlY_edge = (state.scr_height * (locn2.y - state.tly)/state.height) ;

    m->m_ShapePainter->DrawSquare(tlX_edge -4,tlY_edge-4,8,8,LSMVector4(1.0,1.0,1.0,0.75));


    if(features.length() > 0)
    {
        m_VectorLayer->DrawFeature(features.at(0),state,m,LSMVector4(1.0,1.0,1.0,0.2),m_DragOffset);
    }

    if(m_SelectedShape != nullptr)
    {
        m_VectorLayer->DrawShape(m_SelectedShapeC,state,m,LSMVector4(1.0,0.6,0.3,0.2),m_DragOffset);

        if(m_SelectedShapeC != nullptr)
        {
             m_VectorLayer->DrawShape(m_SelectedShapeC,state,m,LSMVector4(1.0,0.6,0.3,0.75),m_DragOffset);
        }
    }else if(m_SelectedFeature != nullptr)//&& m_SelectedFeature != features.at(0))
    {
        m_VectorLayer->DrawFeature(m_SelectedFeature,state,m,LSMVector4(1.0,1.0,1.0,0.3),m_DragOffset);
    }else if(m_SelectedFeatures.size() > 0)
    {
        for(int i = 0; i < m_SelectedFeatures.size(); i++)
        {
            m_VectorLayer->DrawFeature(m_SelectedFeatures.at(i),state,m,LSMVector4(1.0,1.0,1.0,0.3),m_DragOffset);
        }

    }else if(m_SelectedShapes.size() > 0)
    {
        for(int i = 0; i < m_SelectedShapes.size(); i++)
        {
            m_VectorLayer->DrawShape(m_SelectedShapes.at(i).first,state,m,LSMVector4(1.0,1.0,1.0,0.3),m_DragOffset);
        }
    }else if(m_SelectedVertices.size() > 0)
    {
        for(int i = 0; i < m_SelectedVertices.size(); i++)
        {
            LSMVector2 locn1 = m_Transformerinv->Transform(LSMVector2(*(std::get<0>(m_SelectedVertices.at(i) ))+ m_DragOffset.x,*(std::get<1>(m_SelectedVertices.at(i) ))+ m_DragOffset.y));

            float tlX_vertex = state.scr_width * (locn1.x - state.tlx)/state.width;
            float tlY_vertex = (state.scr_height * (locn1.y - state.tly)/state.height) ;

            if(!(tlX_vertex < 0.0 || tlY_vertex < 0.0 || tlX_vertex > state.scr_width || tlY_vertex > state.scr_height))
            {
                m->m_ShapePainter->DrawSquare(tlX_vertex -3,tlY_vertex-3,6,6,LSMVector4(1.0,1.0,1.0,0.75));
            }
        }
    }

    if(m_DialogRemovedFeatures.size() > 0)
    {
        for(int i = 0; i < m_DialogRemovedFeatures.size(); i++)
        {
            Feature * f = m_DialogRemovedFeatures.at(i);
            m_VectorLayer->DrawFeature(f,state,m,LSMVector4(0.929,0.26,0.2,0.7),m_DragOffset);

        }
    }



    /*if(m_SelectedFeatures.size() > 0)
    {
        m_VectorLayer->DrawFeature(m_SelectedFeature,state,m,LSMVector4(1.0,1.0,1.0,0.4));
    }*/



    if(IsDraw() && m_Shapes != nullptr)
    {
        QString edit_type_string = GetParameterValue("Type");
        QString edit_shape_string = GetParameterValue("Shape");
        QString edit_selecttype_string = GetParameterValue("Selection");


        if(((edit_type_string == "Select (alt)") && !((edit_selecttype_string=="Shape") || (edit_selecttype_string == "Feature")) ) || edit_type_string.compare("Add/Remove Shape") == 0 || edit_type_string.compare("Add/Remove Shape To Feature") == 0  || (edit_type_string=="Split") )
        {

            if(edit_shape_string.compare("Polygon") == 0)
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

                if(m_HasSquare)
                {
                    //draw square from origin position to current position

                    LSMVector3 endpos = m_Dragging? LSMVector3(state.MouseGeoPosX,0.0,state.MouseGeoPosZ):m_DragEnd;

                    float tlX = state.scr_width * (endpos.x - state.tlx)/state.width;
                    float tlY = (state.scr_height * (endpos.z - state.tly)/state.height) ;

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


                if(m_HasCircle)
                {
                    //draw square from origin position to current position

                    LSMVector3 endpos = m_Dragging? LSMVector3(state.MouseGeoPosX,0.0,state.MouseGeoPosZ):m_DragEnd;

                    float brX = state.scr_width * (endpos.x - state.tlx)/state.width;
                    float brY = (state.scr_height * (endpos.z - state.tly)/state.height) ;

                    float tlX = state.scr_width * (m_DragOrigin.x - state.tlx)/state.width;
                    float tlY = (state.scr_height * (m_DragOrigin.z- state.tly)/state.height) ;


                    float r = sqrt((tlY - brY)*(tlY - brY) + (tlX - brX)*(tlX - brX));

                    m->m_ShapePainter->DrawRegularNGon(tlX,tlY,r,25,LSMVector4(1.0,1.0,1.0,0.75));

                }

            if(edit_shape_string.compare("Lines") == 0)
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
            }


        }



    }
}


void UIVectorLayerEditor::OnSave()
{


    m_OriginalShapes->Destroy();
    delete m_OriginalShapes;
    m_OriginalShapes = new ShapeFile();
    m_OriginalShapes->CopyFrom(m_Shapes);

    AS_SaveVectorToFileAbsolute(m_Shapes,GetCurrentFilePath());


    m_IsChanged = false;
    m_VectorLayer->NotifyDataChange();
}

void UIVectorLayerEditor::OnSaveAs(QString path)
{
    m_OriginalShapes->Destroy();
    delete m_OriginalShapes;
    m_OriginalShapes = new ShapeFile();
    m_OriginalShapes->CopyFrom(m_Shapes);

    m_Layer->SetFilePath(path);
    m_FilePath = m_Layer->GetFilePath();

    AS_SaveVectorToFileAbsolute(m_Shapes,m_FilePath);

    m_IsChanged = false;
    m_VectorLayer->NotifyDataChange();
}


void UIVectorLayerEditor::SetAttributes(MatrixTable * t)
{
    std::cout <<"set attributes " << std::endl;
    if(m_Shapes != nullptr)
    {
        if(m_Shapes->GetLayerCount() > 0)
        {
            QList<Feature *> frem = m_Shapes->GetLayer(0)->GetAttributeTableReturnRemovedFeatures(t);

            //first add all the relevant changes (removing the features)
            //they are in the order in which are occur in the internal list, however, removal order doesnt matter here

            int edits = 0;

            for(int i = 0; i < frem.size(); i++)
            {

                Feature * f = frem.at(i);
                ShapeLayerChange lc = ShapeLayerChangeRemoveFeature(m_Shapes,f);
                ApplyShapeLayerChange(m_Shapes,lc);
                m_Edits.append(lc);
                edits ++;

            }

            edits ++;

            ShapeLayerChange lc = ShapeLayerChangeReplaceAllAttributes(m_Shapes,t,0);
            ApplyShapeLayerChange(m_Shapes,lc);
            m_Edits.append(lc);
            edits ++;

            if(edits > 0)
            {

                m_EditsSimultaneityCount.append(edits);
                m_VectorLayer->NotifyDataChange();
            }

        }
    }

    m_IsChanged = true;
}

void UIVectorLayerEditor::OnClose()
{

    m_Shapes->Destroy();
    delete m_Shapes;
    m_Shapes = nullptr;
    m_VectorLayer->m_Shapes = m_OriginalShapes;
    m_VectorLayer->SetDrawPoints(false);
    m_VectorLayer->NotifyDataChange();
    m_IsPrepared = false;
}
