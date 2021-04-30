#pragma once

#include "QList"
#include "geometry/shape.h"
#include "geometry/clipper.hpp"
#include "geometry/point.h"
#include "geometry/polygon.h"
#include "geometry/line.h"
#include "geometry/multipolygon.h"
#include "geometry/multiline.h"
#include "geometry/multipoint.h"
#include "geo/shapes/shapefile.h"
#include "iostream"

#define LISEM_OFFSET_ROUND 0
#define LISEM_OFFSET_SQUARE 1
#define LISEM_OFFSET_MITER 2

#define LISEM_SHAPE_NONE 0
#define LISEM_SHAPE_INTERSECT 1   //A && B
#define LISEM_SHAPE_UNION 2       //A || B
#define LISEM_SHAPE_DIFFERENCE 3  //A && !B
#define LISEM_SHAPE_XOR 4         //A || B  && !(A&&B)
#define LISEM_SHAPE_ADD 5         //A + B






//returns a list of shapes that are given by the operator applied to shape1 and shape2. Thus result = s1 *s2 (with * some operator)
//this function can do a large variety of gis vector operations for two shapes.
//for example, combining two lines with intersect finds the interscect location for points
//or, using the union operator on two polygons creates a single unified polygon
//note: there are 9 unique combinations for the shapes we support
//then for each combination, 5 operators.
//we only support 6 of them, with partial support for operators.
//in the function defined below, numerous combinations are not defined and simply return an empty list
//Taking a point out of a polygon doesnt seem to usefull (a polygon with a infinitely small hole might be better represented as a polygon and a seperate point anyway)
//similarly, point and point comparisons show no direct use, except to check wether points exists on identical locations.

static void ShapeAlgebra(LSMShape * s1, LSMShape * s2, QList<LSMShape *> * ret_A,  QList<LSMShape *> * ret_B, QList<LSMShape *> * ret_C);
static void ShapeAlgebra(QList<LSMShape *> s1, QList<LSMShape *> s2, QList<LSMShape *> * ret_A,  QList<LSMShape *> * ret_B, QList<LSMShape *> * ret_C);
static void PolygonPolygonDisect(QList<LSMPolygon *> p1, QList<LSMPolygon *> p2,  QList<LSMShape*> * ret_pol_A,QList<LSMShape*> * ret_pol_B,QList<LSMShape*> * ret_pol_C);
static void LinePolygonDisect(QList<LSMLine *> p1, QList<LSMPolygon *> p2, QList<LSMShape*> * ret_pol_A,QList<LSMShape*> * ret_pol_B,QList<LSMShape*> * ret_pol_C);
static void PointPolygonDisect(QList<LSMPoint *> p1, QList<LSMPolygon *> p2, QList<LSMShape*> * ret_pol_A,QList<LSMShape*> * ret_pol_B,QList<LSMShape*> * ret_pol_C);
static void LineLineDisect(QList<LSMLine *> p1, QList<LSMLine *> p2, QList<LSMShape*> * ret_pol_A,QList<LSMShape*> * ret_pol_B,QList<LSMShape*> * ret_pol_C);

inline static ShapeFile * ShapeLayerAlgebra(ShapeLayer * sf1, ShapeLayer * sf2,int Operator)
{

    std::cout << "doing shape algebra " << std::endl;

    if(sf1 == nullptr || sf2 == nullptr)
    {
        return new ShapeFile();
    }

    BoundingBox Extent1 = sf1->GetAndCalcBoundingBox();
    BoundingBox Extent2 = sf2->GetAndCalcBoundingBox();

    ShapeFile * ret_sf = new ShapeFile();
    ShapeLayer * ret_layer = new ShapeLayer();
    ret_sf->AddLayer(ret_layer);

    //make a copy and flatten in (this means we put all the shapes in induvidual features
    //this way, later on, we can make things easier when calling the functions to provide the new shapes
    sf1 = sf1->Copy();
    sf2 = sf2->Copy();

    sf1->Flatten();
    sf2->Flatten();


    std::cout << "shape counts " << sf1->GetShapeCount() << "  " << sf2->GetShapeCount() << std::endl;
    std::cout << "attribute coutns " << sf1->GetNumberOfAttributes() << " " << sf2->GetNumberOfAttributes() << std::endl;
    //while we distribute the actual work to a shape-shape based operating function
    //we must apply the logic here also!

    //the shape operator function returns three parts: A, B and C
    //A: the difference A NOT B
    //B: the difference B NOT A
    //C: the intersection of both A AND B
    //together these three make up the union (but fragmented into regions for which we can set attribute values

    //these are the operator types
    //#define LISEM_SHAPE_UNKNOWN 0
    //#define LISEM_SHAPE_POINT 1
    //#define LISEM_SHAPE_LINE 2
    //#define LISEM_SHAPE_POLYGON 3
    //#define LISEM_SHAPE_MULTIPOINT 4
    //#define LISEM_SHAPE_MULTILINE 5
    //#define LISEM_SHAPE_MULTIPOLYGON 6

    QMap<Feature*,QMap<QString,QString>> AtribValues;
    QMap<Feature*,QMap<QString,int>> AtribTypes;

    if(Operator == LISEM_SHAPE_INTERSECT)
    {
        //Brute force n2, add all intersections betwee sf1 and sf2
        for(int i = 0; i < sf1->GetFeatureCount(); i++)
        {
            for(int j = 0; j < sf2->GetFeatureCount(); j++)
            {
                //prepare input
                Feature * f1 = sf1->GetFeature(i);
                Feature * f2 = sf2->GetFeature(j);
                if(!(f1->GetShapeCount() > 0 && f2->GetShapeCount() > 0))
                {
                    continue;
                }else {

                    QList<Feature *> features;

                    QList<LSMShape *> ret_A;
                    QList<LSMShape *> ret_B;
                    QList<LSMShape *> ret_C;

                    //do calculation
                    ShapeAlgebra(f1->GetShape(0),f2->GetShape(0),&ret_A,&ret_B,&ret_C);

                    //now add the relevant part

                    if(ret_B.length() > 0)
                    {

                        for(int k = 0; k < ret_B.length(); k++)
                        {
                            Feature * fnew = new Feature();
                            fnew->AddShape(ret_B.at(k));
                            ret_layer->AddFeature(fnew);
                            features.append(fnew);
                        }
                    }

                    for(int k = 0; k < ret_A.length(); k++)
                    {
                        ret_A.at(k)->Destroy();
                        delete ret_A.at(k);
                    }
                    for(int k = 0; k < ret_C.length(); k++)
                    {
                        ret_C.at(k)->Destroy();
                        delete ret_C.at(k);
                    }

                    //set the attributes of this part
                    for(int k = 0; k < features.length(); k++)
                    {
                        QMap<QString,QString> mval;
                        QMap<QString,int> mtyp;

                        if(AtribValues.contains(features.at(k)))
                        {
                            mval = AtribValues[features.at(k)];
                            mtyp = AtribTypes[features.at(k)];
                        }

                        //attributes coming from first layer
                        for(int l = 0; l < sf1->GetNumberOfAttributes(); l++)
                        {
                            QString atname = sf1->GetAttributeName(l);
                            int attype = sf1->GetAttributeType(l);
                            QString atval = sf1->GetAttributeValue(l,f1);

                            QString tempname = atname;

                            if(mval.contains(atname))
                            {
                                int name_add = 0;
                                while(mval.contains(atname + "_" +QString::number(name_add)))
                                {
                                    name_add ++;
                                    if(name_add > 25)
                                    {
                                        break;
                                    }
                                }
                                tempname = atname + "_" +QString::number(name_add);
                            }

                            mval[atname] = atval;
                            mtyp[atname] = attype;
                        }

                        //attributes coming from second layer
                        for(int l = 0; l < sf2->GetNumberOfAttributes(); l++)
                        {
                            QString atname = sf2->GetAttributeName(l);
                            int attype = sf2->GetAttributeType(l);
                            QString atval = sf2->GetAttributeValue(l,f2);

                            QString tempname = atname;

                            if(mval.contains(atname))
                            {
                                int name_add = 0;
                                while(mval.contains(atname + "_" +QString::number(name_add)))
                                {
                                    name_add ++;
                                    if(name_add > 25)
                                    {
                                        break;
                                    }
                                }
                                tempname = atname + "_" +QString::number(name_add);
                            }

                            mval[atname] = atval;
                            mtyp[atname] = attype;
                        }

                        AtribValues[features.at(k)] = mval;
                        AtribTypes[features.at(k)] = mtyp;
                    }
                }
            }
        }



    }else if(Operator == LISEM_SHAPE_UNION)
    {

        //Bruter force, n3??
        //first, add all sf2 shapes to the new shapefile
        //then keep adding sf1 shapes to the new shapefile
            //union with all shapes in results
            //only add new parts

        while((sf1->GetFeatureCount() + sf2->GetFeatureCount()) > 0)
        {
            bool take_l2 = false;
            Feature * f1;

            ShapeLayer * sfcurrent;

            if(sf1->GetFeatureCount() != 0)
            {
                sfcurrent = sf1;
                f1 = sf1->GetFeature(0);
                sf1->RemoveFeature(0);
            }else {
                sfcurrent = sf2;
                f1 = sf2->GetFeature(0);
                sf2->RemoveFeature(0);
            }

            QList<LSMShape *> shape_parts;
            QList<int> shape_parts_startindex;
            if(!(f1->GetShapeCount() > 0))
            {
                f1->Destroy();
                delete f1;
                continue;
            }else
            {
                shape_parts.append(f1->GetShape(0));
                shape_parts_startindex.append(0);
            }


            QMap<QString,QString> mval_f1;
            QMap<QString,int> mtyp_f1;

            if(AtribValues.contains(f1))
            {
                mval_f1 = AtribValues[f1];
                mtyp_f1 = AtribTypes[f1];
            }

            //attributes coming from first layer
            for(int l = 0; l < sfcurrent->GetNumberOfAttributes(); l++)
            {
                QString atname = sfcurrent->GetAttributeName(l);
                int attype = sfcurrent->GetAttributeType(l);
                QString atval = sfcurrent->GetAttributeValue(l,f1);

                QString tempname = atname;

                if(mval_f1.contains(atname))
                {
                    int name_add = 0;
                    while(mval_f1.contains(atname + "_" +QString::number(name_add)))
                    {
                        name_add ++;
                        if(name_add > 25)
                        {
                            break;
                        }
                    }
                    tempname = atname + "_" +QString::number(name_add);
                }

                mval_f1[atname] = atval;
                mtyp_f1[atname] = attype;
            }

            while(shape_parts.length() > 0)
            {

                QList<Feature *> f_replace_existing;
                QList<Feature *> f_replaced_existing;
                QList<Feature *> f_remove_existing;
                QList<Feature *> f_add_existing;

                LSMShape * shape_current = shape_parts.at(0);
                int startindex =shape_parts_startindex.at(0);
                shape_parts_startindex.removeAt(0);
                shape_parts.removeAt(0);


                bool did_interact = false;

                //do all the interactions
                for(int i = startindex; i < ret_layer->GetFeatureCount(); i++)
                {

                    Feature * f2 = ret_layer->GetFeature(i);

                    if(!((f1->GetShapeCount() > 0) && (f2->GetShapeCount() > 0)))
                    {
                        continue;
                    }else {

                        QList<LSMShape *> ret_A;
                        QList<LSMShape *> ret_B;
                        QList<LSMShape *> ret_C;

                        //do calculation
                        ShapeAlgebra(shape_current,f2->GetShape(0),&ret_A,&ret_B,&ret_C);

                        //now add the intersections
                        if(ret_B.length() > 0)
                        {
                            for(int k = 0; k < ret_B.length(); k++)
                            {
                                Feature * fnew = new Feature();
                                fnew->AddShape(ret_B.at(k));
                                f_add_existing.append(fnew);

                                //set attribute info as mixture of f2 (part of the return layer) and f1 (either part of shapefile1 or shapefile2)

                                QMap<QString,QString> mval;
                                QMap<QString,int> mtyp;

                                if(AtribValues.contains(f2))
                                {
                                    mval = AtribValues[f2];
                                    mtyp = AtribTypes[f2];
                                }

                                //attributes coming from first layer
                                for(int l = 0; l < sfcurrent->GetNumberOfAttributes(); l++)
                                {
                                    QString atname = sfcurrent->GetAttributeName(l);
                                    int attype = sfcurrent->GetAttributeType(l);
                                    QString atval = sfcurrent->GetAttributeValue(l,f1);

                                    QString tempname = atname;

                                    if(mval.contains(atname))
                                    {
                                        int name_add = 0;
                                        while(mval.contains(atname + "_" +QString::number(name_add)))
                                        {
                                            name_add ++;
                                            if(name_add > 25)
                                            {
                                                break;
                                            }
                                        }
                                        tempname = atname + "_" +QString::number(name_add);
                                    }

                                    mval[atname] = atval;
                                    mtyp[atname] = attype;
                                }

                                AtribValues[fnew] = mval;
                                AtribTypes[fnew] = mtyp;

                            }
                        }
                        bool do_break_nothingleft = false;

                        //this is the part we continue with
                        if(ret_A.length() > 0)
                        {
                            did_interact = true;
                            shape_current = ret_A.at(0);

                            for(int k = 1; k < ret_A.length(); k++)
                            {
                                shape_parts.prepend(ret_A.at(k));
                                shape_parts_startindex.append(i+1);
                            }
                        }else { //if there is nothing left of the original shape, continue
                            do_break_nothingleft = true;
                        }

                        //replace the one that was already in
                        if(ret_C.length() > 0)
                        {

                            if(!f_remove_existing.contains(f2))
                            {
                                f_remove_existing.append(f2);
                                for(int k = 0; k < ret_C.length(); k++)
                                {
                                    Feature * fnew = new Feature();
                                    fnew->AddShape(ret_C.at(k));
                                    f_replace_existing.append(fnew);
                                    f_replaced_existing.append(f2);
                                }
                            }
                        }

                        if(do_break_nothingleft)
                        {
                            shape_current = nullptr;
                            break;
                        }
                    }

                }

                if(shape_current != nullptr)
                {
                    //add the remainder
                    Feature * fnew = new Feature();
                    fnew->AddShape(shape_current->Copy());
                    f_add_existing.append(fnew);


                    //set attribute info as  f1 (either part of shapefile1 or shapefile2)

                    AtribValues[fnew] = mval_f1;
                    AtribTypes[fnew] = mtyp_f1;

                }

                for(int i= 0; i < f_replace_existing.length(); i++)
                {
                    Feature * fnew = f_replace_existing.at(i);
                    Feature * fold = f_replaced_existing.at(i);
                    ret_layer->AddFeature(fnew);
                    //set attribute info as  f1 (either part of shapefile1 or shapefile2)

                    QMap<QString,QString> mval;
                    QMap<QString,int> mtyp;

                    if(AtribValues.contains(fold))
                    {
                        mval = AtribValues[fold];
                        mtyp = AtribTypes[fold];

                        AtribValues[fnew] = mval;
                        AtribTypes[fnew] = mtyp;

                    }
                }

                for(int i = 0; i < f_remove_existing.length(); i++)
                {
                    Feature * fremove= f_remove_existing.at(i);

                    ret_layer->RemoveFeature(fremove);

                    if(AtribValues.contains(fremove))
                    {
                        AtribValues.remove(fremove);
                        AtribTypes.remove(fremove);
                    }


                }


                for(int i= 0; i < f_add_existing.length(); i++)
                {
                    ret_layer->AddFeature(f_add_existing.at(i));
                    //we added attrute info before already
                }

            }


            f1->Destroy();
            delete f1;
        }



    }else if(Operator == LISEM_SHAPE_DIFFERENCE)
    {
        //brute force, n2
        //for each shape in sf1, do a difference with all the shapes in sf2
        //then add to results

        //clip4.shp = clip.shp - Dominica_landslide_part2.shp

        for(int j = 0; j < sf1->GetFeatureCount(); j++)
        {
            Feature * f1;

            f1 = sf1->GetFeature(j);

            QList<LSMShape *> shape_parts;
            QList<int> shape_parts_startindex;
            if(!(f1->GetShapeCount() > 0))
            {
                continue;
            }else
            {
                for(int k = 0; k < f1->GetShapeCount(); k++)
                {
                    shape_parts.append(f1->GetShape(k));
                    shape_parts_startindex.append(0);
                }
            }

            while(shape_parts.length() > 0)
            {

                LSMShape * shape_current = shape_parts.at(0);
                int startindex =shape_parts_startindex.at(0);
                shape_parts_startindex.removeAt(0);
                shape_parts.removeAt(0);


                bool did_interact = false;

                //do all the interactions
                for(int i = startindex; i < sf2->GetFeatureCount(); i++)
                {

                    bool do_break_nothingleft = false;

                    Feature * f2 = sf2->GetFeature(i);

                    if(!((f2->GetShapeCount() > 0)))
                    {
                        continue;
                    }else {


                        //do calculation
                        for(int l = 0; l < f2->GetShapeCount(); l++)
                        {
                            QList<LSMShape *> ret_A;
                            QList<LSMShape *> ret_B;
                            QList<LSMShape *> ret_C;


                            ShapeAlgebra(shape_current,f2->GetShape(l),&ret_A,&ret_B,&ret_C);

                            for(int i = 0; i < ret_A.length(); i++)
                            {
                                std::cout << i << " " << ret_A.at(i)->Area() << std::endl;
                            }

                            //this is the part we continue with
                            if(ret_A.length() > 0)
                            {
                                did_interact = true;
                                shape_current = ret_A.at(0);

                                for(int k = 1; k < ret_A.length(); k++)
                                {
                                    shape_parts.prepend(ret_A.at(k));
                                    shape_parts_startindex.prepend(i+1);
                                }
                            }else { //if there is nothing left of the original shape, continue
                                do_break_nothingleft = true;
                            }

                            for(int k = 0; k < ret_B.length(); k++)
                            {
                                ret_B.at(k)->Destroy();
                                delete ret_B.at(k);
                            }
                            for(int k = 0; k < ret_C.length(); k++)
                            {
                                ret_C.at(k)->Destroy();
                                delete ret_C.at(k);
                            }

                            if(do_break_nothingleft)
                            {
                                shape_current = nullptr;
                                break;
                            }
                        }

                        if(do_break_nothingleft)
                        {
                            break;
                        }
                    }

                }

                if(shape_current != nullptr)
                {
                    //add the remainder
                    Feature * fnew = new Feature();
                    fnew->AddShape(shape_current->Copy());
                    ret_layer->AddFeature(fnew);



                    QMap<QString,QString> mval;
                    QMap<QString,int> mtyp;

                    if(AtribValues.contains(fnew))
                    {
                        mval = AtribValues[fnew];
                        mtyp = AtribTypes[fnew];
                    }

                    //attributes coming from first layer
                    for(int l = 0; l < sf1->GetNumberOfAttributes(); l++)
                    {
                        QString atname = sf1->GetAttributeName(l);
                        int attype = sf1->GetAttributeType(l);
                        QString atval = sf1->GetAttributeValue(l,f1);

                        QString tempname = atname;

                        if(mval.contains(atname))
                        {
                            int name_add = 0;
                            while(mval.contains(atname + "_" +QString::number(name_add)))
                            {
                                name_add ++;
                                if(name_add > 25)
                                {
                                    break;
                                }
                            }
                            tempname = atname + "_" +QString::number(name_add);
                        }

                        mval[atname] = atval;
                        mtyp[atname] = attype;
                    }

                    AtribValues[fnew] = mval;
                    AtribTypes[fnew] = mtyp;
                }
            }
        }

    }else if(Operator == LISEM_SHAPE_XOR)
    {
        //same as in difference, but for each difference calculation, add the C part
        for(int j = 0; j < sf1->GetFeatureCount(); j++)
        {
            Feature * f1;

            f1 = sf1->GetFeature(j);

            QList<LSMShape *> shape_parts;
            QList<int> shape_parts_startindex;
            if(!(f1->GetShapeCount() > 0))
            {
                continue;
            }else
            {
                shape_parts.append(f1->GetShape(0));
                shape_parts_startindex.append(0);
            }

            while(shape_parts.length() > 0)
            {

                LSMShape * shape_current = shape_parts.at(0);
                int startindex =shape_parts_startindex.at(0);
                shape_parts_startindex.removeAt(0);
                shape_parts.removeAt(0);


                bool did_interact = false;

                //do all the interactions
                for(int i = startindex; i < sf2->GetFeatureCount(); i++)
                {

                    Feature * f2 = sf2->GetFeature(i);

                    if(!((f1->GetShapeCount() > 0) && (f2->GetShapeCount() > 0)))
                    {
                        continue;
                    }else {

                        QList<LSMShape *> ret_A;
                        QList<LSMShape *> ret_B;
                        QList<LSMShape *> ret_C;

                        //do calculation
                        ShapeAlgebra(shape_current,f2->GetShape(0),&ret_A,&ret_B,&ret_C);

                        bool do_break_nothingleft = false;

                        //this is the part we continue with
                        if(ret_A.length() > 0)
                        {
                            did_interact = true;
                            shape_current = ret_A.at(0);

                            for(int k = 1; k < ret_A.length(); k++)
                            {
                                shape_parts.append(ret_A.at(k));
                                shape_parts_startindex.append(i+1);
                            }

                        }else { //if there is nothing left of the original shape, continue
                            do_break_nothingleft = true;
                        }

                        for(int k = 0; k < ret_B.length(); k++)
                        {
                            ret_B.at(k)->Destroy();
                            delete ret_B.at(k);
                        }
                        for(int k = 0; k < ret_C.length(); k++)
                        {
                            ret_C.at(k)->Destroy();
                            delete ret_C.at(k);
                        }

                        if(do_break_nothingleft)
                        {
                            shape_current = nullptr;
                            break;
                        }
                    }

                }

                if(shape_current != nullptr)
                {

                    //add the remainder
                    Feature * fnew = new Feature();
                    fnew->AddShape(shape_current->Copy());
                    ret_layer->AddFeature(fnew);



                    QMap<QString,QString> mval;
                    QMap<QString,int> mtyp;

                    if(AtribValues.contains(fnew))
                    {
                        mval = AtribValues[fnew];
                        mtyp = AtribTypes[fnew];
                    }

                    //attributes coming from first layer
                    for(int l = 0; l < sf1->GetNumberOfAttributes(); l++)
                    {
                        QString atname = sf1->GetAttributeName(l);
                        int attype = sf1->GetAttributeType(l);
                        QString atval = sf1->GetAttributeValue(l,f1);

                        QString tempname = atname;

                        if(mval.contains(atname))
                        {
                            int name_add = 0;
                            while(mval.contains(atname + "_" +QString::number(name_add)))
                            {
                                name_add ++;
                                if(name_add > 25)
                                {
                                    break;
                                }
                            }
                            tempname = atname + "_" +QString::number(name_add);
                        }

                        mval[atname] = atval;
                        mtyp[atname] = attype;
                    }

                    AtribValues[fnew] = mval;
                    AtribTypes[fnew] = mtyp;

                }

            }

        }


        for(int j = 0; j < sf2->GetFeatureCount(); j++)
        {
            Feature * f1;

            f1 = sf2->GetFeature(j);

            QList<LSMShape *> shape_parts;
            QList<int> shape_parts_startindex;
            if(!(f1->GetShapeCount() > 0))
            {
                continue;
            }else
            {
                shape_parts.append(f1->GetShape(0));
                shape_parts_startindex.append(0);
            }

            while(shape_parts.length() > 0)
            {

                LSMShape * shape_current = shape_parts.at(0);
                int startindex =shape_parts_startindex.at(0);
                shape_parts_startindex.removeAt(0);
                shape_parts.removeAt(0);


                bool did_interact = false;

                //do all the interactions
                for(int i = startindex; i < sf1->GetFeatureCount(); i++)
                {

                    Feature * f2 = sf1->GetFeature(i);

                    if(!((f1->GetShapeCount() > 0) && (f2->GetShapeCount() > 0)))
                    {
                        continue;
                    }else {

                        QList<LSMShape *> ret_A;
                        QList<LSMShape *> ret_B;
                        QList<LSMShape *> ret_C;

                        //do calculation
                        ShapeAlgebra(shape_current,f2->GetShape(0),&ret_A,&ret_B,&ret_C);

                        bool do_break_nothingleft = false;

                        //this is the part we continue with
                        if(ret_A.length() > 0)
                        {
                            did_interact = true;
                            shape_current = ret_A.at(0);

                            for(int k = 1; k < ret_A.length(); k++)
                            {
                                shape_parts.append(ret_A.at(k));
                                shape_parts_startindex.append(i+1);
                            }
                        }else { //if there is nothing left of the original shape, continue
                            do_break_nothingleft = true;
                        }

                        for(int k = 0; k < ret_B.length(); k++)
                        {
                            ret_B.at(k)->Destroy();
                            delete ret_B.at(k);
                        }
                        for(int k = 0; k < ret_C.length(); k++)
                        {
                            ret_C.at(k)->Destroy();
                            delete ret_C.at(k);
                        }

                        if(do_break_nothingleft)
                        {
                            shape_current = nullptr;
                            break;
                        }
                    }

                }

                if(shape_current != nullptr)
                {
                    //add the remainder
                    Feature * fnew = new Feature();
                    fnew->AddShape(shape_current->Copy());
                    ret_layer->AddFeature(fnew);


                    //set attributes

                    QMap<QString,QString> mval;
                    QMap<QString,int> mtyp;

                    if(AtribValues.contains(fnew))
                    {
                        mval = AtribValues[fnew];
                        mtyp = AtribTypes[fnew];
                    }

                    //attributes coming from first layer
                    for(int l = 0; l < sf2->GetNumberOfAttributes(); l++)
                    {
                        QString atname = sf2->GetAttributeName(l);
                        int attype = sf2->GetAttributeType(l);
                        QString atval = sf2->GetAttributeValue(l,f1);

                        QString tempname = atname;

                        if(mval.contains(atname))
                        {
                            int name_add = 0;
                            while(mval.contains(atname + "_" +QString::number(name_add)))
                            {
                                name_add ++;
                                if(name_add > 25)
                                {
                                    break;
                                }
                            }
                            tempname = atname + "_" +QString::number(name_add);
                        }

                        mval[atname] = atval;
                        mtyp[atname] = attype;
                    }

                    AtribValues[fnew] = mval;
                    AtribTypes[fnew] = mtyp;



                }
            }

        }

    }else if(Operator == LISEM_SHAPE_ADD)
    {
        //simply copy and add all of the features to a single new shapefile

        for(int i = 0; i < sf1->GetFeatureCount(); i++)
        {
            Feature * f1 = sf1->GetFeature(i);
            Feature * fnew = sf1->GetFeature(i)->Copy();
            ret_layer->AddFeature(fnew);

            QMap<QString,QString> mval;
            QMap<QString,int> mtyp;

            if(AtribValues.contains(fnew))
            {
                mval = AtribValues[fnew];
                mtyp = AtribTypes[fnew];
            }

            //attributes coming from first layer
            for(int l = 0; l < sf1->GetNumberOfAttributes(); l++)
            {
                QString atname = sf1->GetAttributeName(l);
                int attype = sf1->GetAttributeType(l);
                QString atval = sf1->GetAttributeValue(l,f1);

                QString tempname = atname;

                if(mval.contains(atname))
                {
                    int name_add = 0;
                    while(mval.contains(atname + "_" +QString::number(name_add)))
                    {
                        name_add ++;
                        if(name_add > 25)
                        {
                            break;
                        }
                    }
                    tempname = atname + "_" +QString::number(name_add);
                }

                mval[atname] = atval;
                mtyp[atname] = attype;
            }

            AtribValues[fnew] = mval;
            AtribTypes[fnew] = mtyp;
        }
        for(int i = 0; i < sf2->GetFeatureCount(); i++)
        {
            Feature * f1 = sf2->GetFeature(i);
            Feature * fnew = sf2->GetFeature(i)->Copy();
            ret_layer->AddFeature(fnew);

            QMap<QString,QString> mval;
            QMap<QString,int> mtyp;

            if(AtribValues.contains(fnew))
            {
                mval = AtribValues[fnew];
                mtyp = AtribTypes[fnew];
            }

            //attributes coming from first layer
            for(int l = 0; l < sf2->GetNumberOfAttributes(); l++)
            {
                QString atname = sf2->GetAttributeName(l);
                int attype = sf2->GetAttributeType(l);
                QString atval = sf2->GetAttributeValue(l,f1);

                QString tempname = atname;

                if(mval.contains(atname))
                {
                    int name_add = 0;
                    while(mval.contains(atname + "_" +QString::number(name_add)))
                    {
                        name_add ++;
                        if(name_add > 25)
                        {
                            break;
                        }
                    }
                    tempname = atname + "_" +QString::number(name_add);
                }

                mval[atname] = atval;
                mtyp[atname] = attype;
            }

            AtribValues[fnew] = mval;
            AtribTypes[fnew] = mtyp;
        }


    }else//assume that if no other valid operation is given, we do noting: if(Operator == LISEM_SHAPE_NONE)
    {
        //return nothing
        return new ShapeFile();
    }

    //
    //ret_sf contains all new features
    //the following neste maps contain all attributes
    //we must streamline this
    //QMap<Feature*,QMap<QString,QString>> AtribValues;
    //QMap<Feature*,QMap<QString,int>> AtribTypes;


    //get all unique attribute names and types
    QList<QString> unique_attrib_names;
    QList<int> unique_attrib_types;

    QMap<Feature*,QMap<QString,QString>>::iterator i;
    for (i =  AtribValues.begin(); i !=  AtribValues.end(); ++i)
    {
        QMap<QString,QString> m = i.value();
        QMap<QString,QString>::iterator j;
        for(j = m.begin(); j != m.end(); ++j)
        {
            QString name = j.key();
            if(!unique_attrib_names.contains(name))
            {
                int type = AtribTypes[i.key()][name];
                unique_attrib_names.append(name);
                unique_attrib_types.append(type);
            }
        }
    }

    //now create these attributes and get all the values
    ShapeLayer * layer = ret_sf->GetLayer(0);
    for(int i = 0; i < unique_attrib_names.length(); i++)
    {
        layer->AddAttribute(unique_attrib_names.at(i),unique_attrib_types.at(i));

        for(int j = 0; j < layer->GetFeatureCount(); j++)
        {
            Feature * f = layer->GetFeature(j);
            QString val = "";
            if(AtribValues.contains(f))
            {
                if(AtribValues[f].contains(unique_attrib_names.at(i)))
                {
                    val = AtribValues[f][unique_attrib_names.at(i)];
                }
            }

            layer->SetFeatureAttribute(j,unique_attrib_names.at(i),val);
        }
    }



    //since we made a copy of the shapefiles, delete these copies
    sf1->Destroy();
    delete sf1;
    sf2->Destroy();
    delete sf2;

    return ret_sf;
}

inline static void ShapeAlgebra(LSMShape * s1, LSMShape * s2, QList<LSMShape *> * ret_A,  QList<LSMShape *> * ret_B, QList<LSMShape *> * ret_C)
{


    //first do a simple bound check to see if there is potential interaction
    bool interaction_possible = false;

    //check for null pointer
    if(s1 == nullptr || s2 == nullptr)
    {
        return;
    }


    if(!(s1->GetBoundingBox().Overlaps(s2->GetBoundingBox())))
    {
        ret_A->append(s1->Copy());
        ret_C->append(s2->Copy());
        return;
    }

    int type1 = s1->GetType();
    int type2 = s2->GetType();

    QList<LSMShape*> s1_shapelist;
    QList<LSMShape*> s2_shapelist;

    //check if multi-shape
    //if so, do recursive call and add restuls to return list
    if(type1 == LISEM_SHAPE_UNKNOWN || type2 == LISEM_SHAPE_UNKNOWN)
    {
        return;
    }else if(type1 == LISEM_SHAPE_MULTIPOINT)
    {
        LSMMultiPoint *msp = (LSMMultiPoint*)s1;
        for(int i = 0; i < msp->GetPointCount(); i++)
        {
            s1_shapelist.append((msp->GetPoint(i)));
        }

    }else if(type1 == LISEM_SHAPE_MULTILINE)
    {
        LSMMultiLine *msp = (LSMMultiLine*)s1;
        for(int i = 0; i < msp->GetLineCount(); i++)
        {
            s1_shapelist.append((msp->GetLine(i)));
        }

    }else if(type1 == LISEM_SHAPE_MULTIPOLYGON)
    {
        LSMMultiPolygon *msp = (LSMMultiPolygon*)s1;
        for(int i = 0; i < msp->GetPolygonCount(); i++)
        {
            s1_shapelist.append((msp->GetPolygon(i)));
        }
    }else
    {
        s1_shapelist.append(s1);
    }

    if(type2 == LISEM_SHAPE_MULTIPOINT)
    {
        LSMMultiPoint *msp = (LSMMultiPoint*)s2;
        for(int i = 0; i < msp->GetPointCount(); i++)
        {
            s2_shapelist.append((s1,msp->GetPoint(i)));
        }
    }else if(type2 == LISEM_SHAPE_MULTILINE)
    {
        LSMMultiLine *msp = (LSMMultiLine*)s2;
        for(int i = 0; i < msp->GetLineCount(); i++)
        {
            s2_shapelist.append((s1,msp->GetLine(i)));
        }
    }else if(type2 == LISEM_SHAPE_MULTIPOLYGON)
    {
        LSMMultiPolygon *msp = (LSMMultiPolygon*)s2;
        for(int i = 0; i < msp->GetPolygonCount(); i++)
        {
            s2_shapelist.append((s1,msp->GetPolygon(i)));
        }
    }else
    {
        s2_shapelist.append(s2);
    }

    ShapeAlgebra(s1_shapelist,s2_shapelist,ret_A,ret_B,ret_C);

    return;
}

//this functions must get as arguments two lists of single-instance shapes (so no multi-point, multi-polygon or multi-line)
//additionaly, the shapes in the list must be all from the same type
inline static void ShapeAlgebra(QList<LSMShape *> s1, QList<LSMShape *> s2, QList<LSMShape *> * ret_A,  QList<LSMShape *> * ret_B, QList<LSMShape *> * ret_C)
{

    if(!(s1.length() > 0 && s2.length() > 0))
    {
        return;
    }

    int type1 = s1.at(0)->GetType();
    int type2 = s2.at(0)->GetType();

    if(type1 == LISEM_SHAPE_POINT && type2 == LISEM_SHAPE_POLYGON)
    {
        QList<LSMPoint *> sl1;
        QList<LSMPolygon *> sl2;
        for(int i = 0; i < s1.length(); i++)
        {
            sl1.append((LSMPoint*)s1.at(i));
        }
        for(int i = 0; i < s2.length(); i++)
        {
            sl2.append((LSMPolygon*)s2.at(i));
        }
        PointPolygonDisect(sl1,sl2,ret_A,ret_B,ret_C);

    }else if(type1 == LISEM_SHAPE_LINE && type2 == LISEM_SHAPE_POLYGON)
    {
        QList<LSMLine *> sl1;
        QList<LSMPolygon *> sl2;
        for(int i = 0; i < s1.length(); i++)
        {
            sl1.append((LSMLine*)s1.at(i));
        }
        for(int i = 0; i < s2.length(); i++)
        {
            sl2.append((LSMPolygon*)s2.at(i));
        }
        LinePolygonDisect(sl1,sl2,ret_A,ret_B,ret_C);
    }else if(type1 == LISEM_SHAPE_POLYGON && type2 == LISEM_SHAPE_POINT)
    {
        QList<LSMPoint *> sl1;
        QList<LSMPolygon *> sl2;
        for(int i = 0; i < s1.length(); i++)
        {
            sl1.append((LSMPoint*)s1.at(i));
        }
        for(int i = 0; i < s2.length(); i++)
        {
            sl2.append((LSMPolygon*)s2.at(i));
        }
        PointPolygonDisect(sl1,sl2,ret_C,ret_B,ret_A);

    }else if(type1 == LISEM_SHAPE_POLYGON && type2 == LISEM_SHAPE_LINE)
    {
        QList<LSMLine *> sl1;
        QList<LSMPolygon *> sl2;
        for(int i = 0; i < s1.length(); i++)
        {
            sl1.append((LSMLine*)s2.at(i));
        }
        for(int i = 0; i < s2.length(); i++)
        {
            sl2.append((LSMPolygon*)s1.at(i));
        }
        LinePolygonDisect(sl1,sl2,ret_C,ret_B,ret_A);
    }else if(type1 == LISEM_SHAPE_LINE && type2 == LISEM_SHAPE_LINE)
    {
        QList<LSMLine *> sl1;
        QList<LSMLine *> sl2;
        for(int i = 0; i < s1.length(); i++)
        {
            sl1.append((LSMLine*)s1.at(i));
        }
        for(int i = 0; i < s2.length(); i++)
        {
            sl2.append((LSMLine*)s2.at(i));
        }
        LineLineDisect(sl1,sl2,ret_A,ret_B,ret_C);
    }else if(type1 == LISEM_SHAPE_POLYGON && type2 == LISEM_SHAPE_POLYGON)
    {
        QList<LSMPolygon *> sl1;
        QList<LSMPolygon *> sl2;
        for(int i = 0; i < s1.length(); i++)
        {
            sl1.append((LSMPolygon*)s1.at(i));
        }
        for(int i = 0; i < s2.length(); i++)
        {
            sl2.append((LSMPolygon*)s2.at(i));
        }
        PolygonPolygonDisect(sl1,sl2,ret_A,ret_B,ret_C);

    }else if(type1 == LISEM_SHAPE_POINT && type2 == LISEM_SHAPE_LINE)
    {
        //we dont do point-line interactions
        return;

    }else if(type1 == LISEM_SHAPE_POINT && type2 == LISEM_SHAPE_LINE)
    {
        //we dont do line-point interactions
        return;

    }else if(type1 == LISEM_SHAPE_POINT && type2 == LISEM_SHAPE_POINT)
    {
        //we dont do point-point interactions
        return;

    }else {
        //else not supported

        return;
    }
}



//specific functions


inline static void PolygonTreeToPolygonList(QList<LSMShape*> * ret_pols,ClipperLib::PolyNode * node, BoundingBox &extent,LSMPolygon * parent)
{

    if(node != nullptr)
    {
        if(node->IsOpen() == true)
        {
            for(int i = 0; i < node->Childs.size();i++)
            {
                PolygonTreeToPolygonList(ret_pols,node->Childs.at(i),extent,nullptr);
            }

        }else {
            LSMPolygon * current_parent = nullptr;
            if(!(node->IsHole()))
            {
                LSMPolygon * p = new LSMPolygon();
                Ring * r = new Ring();
                std::cout << "add polygon ring " << node->Contour.size() << std::endl;
                for(int i = 0; i < node->Contour.size(); i++)
                {
                    double x = (((double)(node->Contour.at(i).X))+(9.0e18))/(2.0*9e18);
                    double y = (((double)(node->Contour.at(i).Y))+(9.0e18))/(2.0*9e18);
                    LSMVector2 point = extent.GetCoordinateAbsoluteFromRelative(LSMVector2(x,y));
                    r->AddVertex(point.x,point.y);
                }
                if(r->GetVertexCount() > 0)
                {
                    r->AddVertex(r->GetVertex(0).x,r->GetVertex(0).y);
                }
                p->SetExtRing(r);
                ret_pols->append((LSMShape*)p);

                for(int i = 0; i < node->Childs.size();i++)
                {
                    PolygonTreeToPolygonList(ret_pols,node->Childs.at(i),extent,p);
                }
                current_parent = p;
            }else if(node->IsHole())
            {

                if(parent != nullptr)
                {
                    Ring * r = new Ring();
                    for(int i = 0; i < node->Contour.size(); i++)
                    {
                        double x = (((double)(node->Contour.at(i).X))+(9.0e18))/(2.0*9e18);
                        double y = (((double)(node->Contour.at(i).Y))+(9.0e18))/(2.0*9e18);
                        LSMVector2 point = extent.GetCoordinateAbsoluteFromRelative(LSMVector2(x,y));
                        r->AddVertex(point.x,point.y);

                    }
                    if(r->GetVertexCount() > 0)
                    {
                        r->AddVertex(r->GetVertex(0).x,r->GetVertex(0).y);
                    }
                    parent->AddIntRing(r);
                }
                for(int i = 0; i < node->Childs.size();i++)
                {
                    PolygonTreeToPolygonList(ret_pols,node->Childs.at(i),extent,nullptr);
                }

            }
        }
    }

}

inline static int PointToClipperPath(LSMPoint * p1,ClipperLib::Paths *Polygon1, BoundingBox &extent, int k)
{
    LSMVector2 p1r_p_i = extent.GetCoordinateRelativeFromAbsolute(LSMVector2(p1->GetX(),p1->GetY()));

    ClipperLib::cInt x = (ClipperLib::cInt)((-9.0e18) + (double)(p1r_p_i.x) * (2.0*9e18));
    ClipperLib::cInt y = (ClipperLib::cInt)((-9.0e18) + (double)(p1r_p_i.y) * (2.0*9e18));
    Polygon1->at(k) << ClipperLib::IntPoint(x,y);
    Polygon1->at(k) << ClipperLib::IntPoint(x+1,y);
    return k + 1;

}

inline static int LineToClipperPolyPath(LSMLine * p1,ClipperLib::Paths *Polygon1, BoundingBox &extent, int k)
{
    std::cout << "line to clipper " << p1->GetVertexCount() << std::endl;
    //line to polygon conversion (miter with delta = 3 and distance = 5)
    double dist = 5.0;
    {
        for(int i = 0; i < p1->GetVertexCount();i++)
        {

            LSMVector2 p1r_p = p1->GetVertex(i);

            //convert to clipper coordinates
            LSMVector2 p1r_p_i = extent.GetCoordinateRelativeFromAbsolute(p1r_p);

            ClipperLib::cInt x = (ClipperLib::cInt)((-9.0e18) + (double)(p1r_p_i.x) * (2.0*9e18));
            ClipperLib::cInt y = (ClipperLib::cInt)((-9.0e18) + (double)(p1r_p_i.y) * (2.0*9e18));

            if(i < p1->GetVertexCount()-1 && i > 0)
            {
                LSMVector2 p1r_pn = p1->GetVertex(i+1);
                LSMVector2 dir = (p1r_pn -p1r_p).Normalize();
                LSMVector2 dirp = (p1r_pn -p1r_p).Normalize().Perpendicular();

                LSMVector2 p1r_pm = p1->GetVertex(i-1);
                LSMVector2 dirm = (p1r_p -p1r_pm).Normalize();
                LSMVector2 dirpm = (p1r_p -p1r_pm).Normalize().Perpendicular();

                /*if(dir.dot(dirpm) > 0)
                {
                    //previous was moving clockwise, so the starting point was already done
                    //this is moving clockwise,
                    ClipperLib::cInt xp = x - (ClipperLib::cInt)(dirpm.x*dist);
                    ClipperLib::cInt yp = y - (ClipperLib::cInt)(dirpm.y*dist);

                    Polygon1->at(k) << ClipperLib::IntPoint(xp,yp);

                    xp = x - (ClipperLib::cInt)(dirp.x*dist);
                    yp = y - (ClipperLib::cInt)(dirp.y*dist);

                    Polygon1->at(k) << ClipperLib::IntPoint(xp,yp);

                }else {
                    LSMVector2 dirmmiddle = dirp.RotatedClockWise(dirpm.AngleWith(dirp)/2.0);
                    ClipperLib::cInt xp = x - (ClipperLib::cInt)(dirmmiddle.x*dist);
                    ClipperLib::cInt yp = y - (ClipperLib::cInt)(dirmmiddle.y*dist);
                }*/

                LSMVector2 dirmmiddle = dirp.RotatedClockWise(dirpm.AngleWith(dirp)/2.0);
                ClipperLib::cInt xp = x - (ClipperLib::cInt)(dirmmiddle.x*dist);
                ClipperLib::cInt yp = y - (ClipperLib::cInt)(dirmmiddle.y*dist);
                Polygon1->at(k) << ClipperLib::IntPoint(xp,yp);
            }else {
                Polygon1->at(k) << ClipperLib::IntPoint(x,y);
            }
        }

        for(int i = p1->GetVertexCount()-2; i >0 ;i--)
        {
            LSMVector2 p1r_p = p1->GetVertex(i);

            //convert to clipper coordinates
            LSMVector2 p1r_p_i = extent.GetCoordinateRelativeFromAbsolute(p1r_p);

            ClipperLib::cInt x = (ClipperLib::cInt)((-9.0e18) + (double)(p1r_p_i.x) * (2.0*9e18));
            ClipperLib::cInt y = (ClipperLib::cInt)((-9.0e18) + (double)(p1r_p_i.y) * (2.0*9e18));

            if(i < p1->GetVertexCount()-1 && i > 0)
            {
                LSMVector2 p1r_pn = p1->GetVertex(i-1);
                LSMVector2 dir = (p1r_pn -p1r_p).Normalize();
                LSMVector2 dirp = (p1r_pn -p1r_p).Normalize().Perpendicular();

                LSMVector2 p1r_pm = p1->GetVertex(i+1);
                LSMVector2 dirm = (p1r_p -p1r_pm).Normalize();
                LSMVector2 dirpm = (p1r_p -p1r_pm).Normalize().Perpendicular();

                /*if(dir.dot(dirpm) > 0)
                {
                    //previous was moving clockwise, so the starting point was already done
                    //this is moving clockwise,
                    ClipperLib::cInt xp = x - (ClipperLib::cInt)(dirpm.x*dist);
                    ClipperLib::cInt yp = y - (ClipperLib::cInt)(dirpm.y*dist);

                    Polygon1->at(k) << ClipperLib::IntPoint(xp,yp);

                    xp = x - (ClipperLib::cInt)(dirp.x*dist);
                    yp = y - (ClipperLib::cInt)(dirp.y*dist);

                    Polygon1->at(k) << ClipperLib::IntPoint(xp,yp);

                }else {
                    LSMVector2 dirmmiddle = dirp.RotatedClockWise(dirpm.AngleWith(dirp)/2.0);
                    ClipperLib::cInt xp = x - (ClipperLib::cInt)(dirmmiddle.x*dist);
                    ClipperLib::cInt yp = y - (ClipperLib::cInt)(dirmmiddle.y*dist);
                }*/
                LSMVector2 dirmmiddle = dirp.RotatedClockWise(dirpm.AngleWith(dirp)/2.0);
                ClipperLib::cInt xp = x - (ClipperLib::cInt)(dirmmiddle.x*dist);
                ClipperLib::cInt yp = y - (ClipperLib::cInt)(dirmmiddle.y*dist);
                Polygon1->at(k) << ClipperLib::IntPoint(xp,yp);
            }else {
                if(i < p1->GetVertexCount()-1)
                {
                    Polygon1->at(k) << ClipperLib::IntPoint(x,y);
                }
            }
        }
    }

    std::cout<< "line to clipper end " << Polygon1->at(k).size() << std::endl;
    return k + 1;


}

inline static void ClipperPolyPathToLineList(QList<LSMShape*> *list,ClipperLib::PolyNode * node, BoundingBox &extent)
{
    //gather all unique points within range of 40 (should be non-visible for any practical use-case)
    //we made the polygon with distance 5 and delta of 3, so distances could be 5*3 on both sides = 30 total

    ClipperLib::cInt xp;
    ClipperLib::cInt yp;
    ClipperLib::cInt xpp;
    ClipperLib::cInt ypp;
    LSMLine * l = new LSMLine();

    std::cout << "Clipper poly to line " << node->Contour.size() << std::endl;
    Ring * r = new Ring();
    for(int i = 0; i < node->Contour.size(); i++)
    {
        if(i == 0)
        {
            xp = node->Contour.at(i).X;
            yp = node->Contour.at(i).Y;

            double x = (((double)(xp))+(9.0e18))/(2.0*9e18);
            double y = (((double)(yp))+(9.0e18))/(2.0*9e18);
            LSMVector2 point = extent.GetCoordinateAbsoluteFromRelative(LSMVector2(x,y));
            l->AddVertex(point.x,point.y);
        }else
        {
            //do we consider this vertex part of the same point?
            if(std::abs(node->Contour.at(i).X - xp) < 40 && std::abs(node->Contour.at(i).Y - yp) < 40)
            {

            }else {

                if(l->GetVertexCount() > 1)
                {
                    if(std::abs(node->Contour.at(i).X - xpp) < 40 && std::abs(node->Contour.at(i).Y - ypp) < 40)
                    {
                        //we have seen this point, we are done
                        break;

                    }
                    if(l->GetVertexCount() > node->Contour.size()-i)
                    {
                        //something wrong? break
                        break;
                    }
                }

                xpp = xp;
                ypp = yp;
                xp = node->Contour.at(i).X;
                yp = node->Contour.at(i).Y;
                double x = (((double)(xp))+(9.0e18))/(2.0*9e18);
                double y = (((double)(yp))+(9.0e18))/(2.0*9e18);
                LSMVector2 point = extent.GetCoordinateAbsoluteFromRelative(LSMVector2(x,y));
                l->AddVertex(point.x,point.y);
            }
        }
    }

    list->append(l);


}


inline static int LineToClipperPath(LSMLine * p1,ClipperLib::Paths *Polygon1, BoundingBox &extent, int k)
{
    {
        for(int i = 0; i < p1->GetVertexCount();i++)
        {
            LSMVector2 p1r_p = p1->GetVertex(i);
            //convert to clipper coordinates
            LSMVector2 p1r_p_i = extent.GetCoordinateRelativeFromAbsolute(p1r_p);

            ClipperLib::cInt x = (ClipperLib::cInt)((-9.0e18) + (double)(p1r_p_i.x) * (2.0*9e18));
            ClipperLib::cInt y = (ClipperLib::cInt)((-9.0e18) + (double)(p1r_p_i.y) * (2.0*9e18));
            Polygon1->at(k) << ClipperLib::IntPoint(x,y);
        }
    }

    return k + 1;


}


inline static int PolygonToClipperPath(LSMPolygon * p1,ClipperLib::Paths *Polygon1, BoundingBox &extent, int k)
{
    {
        Ring * p1r = p1->GetExtRing();
        for(int i = p1r->GetVertexCount()-1; i >-1;i--)
        {
            LSMVector2 p1r_p = p1r->GetVertex(i);
            //convert to clipper coordinates
            LSMVector2 p1r_p_i = extent.GetCoordinateRelativeFromAbsolute(p1r_p);

            ClipperLib::cInt x = (ClipperLib::cInt)((-9.0e18) + (double)(p1r_p_i.x) * (2.0*9e18));
            ClipperLib::cInt y = (ClipperLib::cInt)((-9.0e18) + (double)(p1r_p_i.y) * (2.0*9e18));
            Polygon1->at(k) << ClipperLib::IntPoint(x,y);
        }
    }

    for(int j = 0; j < p1->GetIntRingCount(); j++)
    {
        Ring * p1r = p1->GetIntRing(j);
        for(int i = 0; i < p1r->GetVertexCount();i++)
        {
            LSMVector2 p1r_p = p1r->GetVertex(i);
            //convert to clipper coordinates
            LSMVector2 p1r_p_i = extent.GetCoordinateRelativeFromAbsolute(p1r_p);

            ClipperLib::cInt x = (ClipperLib::cInt)((-9e18) + (double)(p1r_p_i.x) * (2.0*9e18));
            ClipperLib::cInt y = (ClipperLib::cInt)((-9e18) + (double)(p1r_p_i.y) * (2.0*9e18));
            Polygon1->at(k+1+j) << ClipperLib::IntPoint(x,y);
        }
    }

    return k + 1 + p1->GetIntRingCount();


}

//line-polygon intersection (returns multiple polygons and a list of lines that are inside or outside the polygon but end when they meet a polygon boundary)
inline static void PolygonPolygonDisect(QList<LSMPolygon *> p1l, QList<LSMPolygon *> p2l, QList<LSMShape*> * ret_pol_A,QList<LSMShape*> * ret_pol_B,QList<LSMShape*> * ret_pol_C)
{
    //std::cout << "poly poly clip " << std::endl;

    int n_path_1 = 0;
    int n_path_2 = 0;

    if(p2l.length() == 0)
    {
        for(int i = 0; i < p1l.length(); i++)
        {
            ret_pol_A->append(p1l.at(i)->Copy());
        }
        return;
    }

    if(p1l.length() == 0)
    {
        for(int i = 0; i < p2l.length(); i++)
        {
            ret_pol_C->append(p2l.at(i)->Copy());
        }
        return;
    }

    BoundingBox extent;

    for(int i = 0; i < p1l.length(); i++)
    {
        LSMPolygon * p1 = p1l.at(i);
        if(i== 0)
        {
            extent = p1->GetBoundingBox();
        }else {
            extent.MergeWith(p1->GetBoundingBox());
        }
        if(!(p1->GetExtRing() == nullptr))
        {
            n_path_1 += 1;
            n_path_1 += p1->GetIntRingCount();
        }
    }
    for(int i = 0; i < p2l.length(); i++)
    {
        LSMPolygon * p2 = p2l.at(i);

        extent.MergeWith(p2->GetBoundingBox());

        if(!(p2->GetExtRing() == nullptr))
        {
            n_path_2 += 1;
            n_path_2 += p2->GetIntRingCount();
        }
    }

    extent.Scale(3.0);

    //to use the clipper library, we have to rescale the coordinates to 64 bit integers within a certain bounding box
    //to do this, we define the bounding box as the total bounding box of both shapes with a 100 percent margin on both sides
    double area_total_1 = 0.0;
    for(int i = 0; i < p1l.length(); i++)
    {
        LSMPolygon * p1 = p1l.at(i);
        area_total_1 += p1->Area();
    }
    double area_total_2 = 0.0;
    for(int i = 0; i < p2l.length(); i++)
    {
        LSMPolygon * p2 = p2l.at(i);
        area_total_2 += p2->Area();
    }

    //std::cout << "polygons1 :  " << p1l.length() << " " <<  n_path_1 << " " <<  "polygons2 :  " << p2l.length() << " " <<  n_path_2 << std::endl;

    //now run the desired operators

    int solution_count = 3;
    ClipperLib::ClipType OperatorClipLib[3] = {ClipperLib::ctIntersection,ClipperLib::ctDifference,ClipperLib::ctDifference}; // default value
    QList<LSMShape*> * Path_Ret[3] = {ret_pol_B,ret_pol_A,ret_pol_C};

    double area_total_res[3] = {0.0,0.0,0.0};
    for(int s = 0; s < solution_count; s++)
    {


        ClipperLib::Paths Polygon1(n_path_1);
        ClipperLib::Paths Polygon2(n_path_2);

        int k = 0;
        for(int i = 0; i < p1l.length(); i++)
        {
            LSMPolygon * p1 = p1l.at(i);
            k = PolygonToClipperPath(p1,&Polygon1,extent,k);
        }
        k = 0;
        for(int i = 0; i < p2l.length(); i++)
        {
            LSMPolygon * p2 = p2l.at(i);
            k = PolygonToClipperPath(p2,&Polygon2,extent,k);
        }

        ClipperLib::Paths * Path_A[3] = {&Polygon1,&Polygon1,&Polygon2};
        ClipperLib::Paths * Path_B[3] = {&Polygon2,&Polygon2,&Polygon1};


        ClipperLib::PolyTree Solution;
        ClipperLib::Clipper C;

        C.AddPaths(*Path_A[s],ClipperLib::ptSubject,true);
        C.AddPaths(*Path_B[s],ClipperLib::ptClip,true);

        C.Execute(OperatorClipLib[s],Solution,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd);

        //now convert back the Solution path to a list of polygons

        for(int i = 0; i < Solution.ChildCount(); i++)
        {
            PolygonTreeToPolygonList(Path_Ret[s],Solution.Childs.at(i),extent,nullptr);
        }
        for(int i = 0; i < Path_Ret[s]->size(); i++)
        {
            area_total_res[s] += ((LSMPolygon*)Path_Ret[s]->at(i))->Area();
        }
    }

    double error = 100.0*((area_total_1 + area_total_2)-(area_total_res[1] + area_total_res[2] + 2.0 * area_total_res[0]))/(area_total_1 + area_total_2);

    //std::cout << "polygon polygon clip :  " << area_total_1 << " " <<  area_total_2 << " A:" << area_total_res[1] << " B: " <<  2.0 * area_total_res[0] << " C:" << area_total_res[2] << std::endl;
    //std::cout << "polygon polygon clip compare :  " << area_total_1 + area_total_2 << " " << area_total_res[1] + area_total_res[2] + 2.0 * area_total_res[0]<< "      error: "  << error << std::endl;
    //std::cout << ret_pol_A->length() << (error > 1? "LARGE ERRRROOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOR!!!!!!!!!!!!!!!!!!!!" : " ")<< std::endl;

}


//line-polygon intersection (returns multiple polygons and a list of lines that are inside or outside the polygon but end when they meet a polygon boundary)
inline static void LinePolygonDisect(QList<LSMLine *> p1l, QList<LSMPolygon *> p2l, QList<LSMShape*> * ret_pol_A,QList<LSMShape*> * ret_pol_B,QList<LSMShape*> * ret_pol_C)
{
    //std::cout << "poly poly clip " << std::endl;

    int n_path_1 = 0;
    int n_path_2 = 0;

    if(p2l.length() == 0)
    {
        for(int i = 0; i < p1l.length(); i++)
        {
            ret_pol_A->append(p1l.at(i)->Copy());
        }
        return;
    }

    if(p1l.length() == 0)
    {
        for(int i = 0; i < p2l.length(); i++)
        {
            ret_pol_C->append(p2l.at(i)->Copy());
        }
        return;
    }

    BoundingBox extent;

    for(int i = 0; i < p1l.length(); i++)
    {
        LSMLine * p1 = p1l.at(i);
        if(i== 0)
        {
            extent = p1->GetBoundingBox();
        }else {
            extent.MergeWith(p1->GetBoundingBox());
        }
        {
            n_path_1 += 1;
        }
    }
    for(int i = 0; i < p2l.length(); i++)
    {
        LSMPolygon * p2 = p2l.at(i);

        extent.MergeWith(p2->GetBoundingBox());

        if(!(p2->GetExtRing() == nullptr))
        {
            n_path_2 += 1;
            n_path_2 += p2->GetIntRingCount();
        }
    }

    extent.Scale(3.0);

    //to use the clipper library, we have to rescale the coordinates to 64 bit integers within a certain bounding box
    //to do this, we define the bounding box as the total bounding box of both shapes with a 100 percent margin on both sides
    double area_total_1 = 0.0;
    for(int i = 0; i < p1l.length(); i++)
    {
        LSMLine * p1 = p1l.at(i);
        area_total_1 += p1->Length();
    }
    double area_total_2 = 0.0;
    for(int i = 0; i < p2l.length(); i++)
    {
        LSMPolygon * p2 = p2l.at(i);
        area_total_2 += p2->Area();
    }

    //std::cout << "polygons1 :  " << p1l.length() << " " <<  n_path_1 << " " <<  "polygons2 :  " << p2l.length() << " " <<  n_path_2 << std::endl;

    //now run the desired operators

    std::cout << "got input "<< std::endl;

    int solution_count = 3;
    ClipperLib::ClipType OperatorClipLib[3] = {ClipperLib::ctIntersection,ClipperLib::ctDifference,ClipperLib::ctDifference}; // default value
    QList<LSMShape*> * Path_Ret[3] = {ret_pol_B,ret_pol_A,ret_pol_C};

    double area_total_res[3] = {0.0,0.0,0.0};
    for(int s = 0; s < solution_count; s++)
    {


        ClipperLib::Paths Polygon1(n_path_1);
        ClipperLib::Paths Polygon2(n_path_2);

        std::cout << "do line convert "<< std::endl;
        int k = 0;
        for(int i = 0; i < p1l.length(); i++)
        {
            LSMLine * p1 = p1l.at(i);
            k = LineToClipperPolyPath(p1,&Polygon1,extent,k);
        }
        std::cout << "do polygon convert " << std::endl;
        k = 0;
        for(int i = 0; i < p2l.length(); i++)
        {
            LSMPolygon * p2 = p2l.at(i);
            k = PolygonToClipperPath(p2,&Polygon2,extent,k);
        }

        std::cout << "create paths from stuff " << std::endl;

        ClipperLib::Paths * Path_A[3] = {&Polygon1,&Polygon1,&Polygon2};
        ClipperLib::Paths * Path_B[3] = {&Polygon2,&Polygon2,&Polygon1};
        bool isline[3] = {true,true,false};

        ClipperLib::PolyTree Solution;
        ClipperLib::Clipper C;

        C.AddPaths(*Path_A[s],ClipperLib::ptSubject,true);
        C.AddPaths(*Path_B[s],ClipperLib::ptClip,true);

        std::cout << "execute "<< std::endl;

        C.Execute(OperatorClipLib[s],Solution,ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd);

        //now convert back the Solution path to a list of polygons

        for(int i = 0; i < Solution.ChildCount(); i++)
        {
            if(!isline[s])
            {
                PolygonTreeToPolygonList(Path_Ret[s],Solution.Childs.at(i),extent,nullptr);
            }else {
                ClipperPolyPathToLineList(Path_Ret[s],Solution.Childs.at(i),extent);
            }
        }
    }

}

//line-polygon intersection (returns multiple polygons and a list of lines that are inside or outside the polygon but end when they meet a polygon boundary)
inline static void PointPolygonDisect(QList<LSMPoint *> p1l, QList<LSMPolygon *> p2l, QList<LSMShape*> * ret_pol_A,QList<LSMShape*> * ret_pol_B,QList<LSMShape*> * ret_pol_C)
{
    //this is not really relevant i suppose,
    //we can check wether points are inside a polygon, but taking a point out of a polygon would hardly be usefull


    for(int i = 0; i < p1l.length(); i++)
    {
        bool inside = false;
        for(int j = 0; j < p2l.length(); j++)
        {
            if(p2l.at(j)->Contains(LSMVector2(p1l.at(i)->GetX(),p1l.at(i)->GetY()),0.0))
            {
                inside = true;
                break;
            }
        }

        if(inside)
        {
            ret_pol_B->append(p1l.at(i)->Copy());

        }else
        {
            ret_pol_A->append(p1l.at(i)->Copy());
        }
    }


    for(int i = 0; i < p2l.length(); i++)
    {
        ret_pol_C->append(p2l.at(i)->Copy());
    }


}

//line-line intersection (returns two new lists of lines)
inline static void LineLineDisect(QList<LSMLine *> p1, QList<LSMLine *> p2, QList<LSMShape*> * ret_pol_A,QList<LSMShape*> * ret_pol_B,QList<LSMShape*> * ret_pol_C)
{
    //this one we dont use clipperlib, as it doesnt support this well, and is relatively easy to write ourselves

    QList<LSMLine *> pl1 = p1;
    QList<LSMLine *> pl2 = p2;

    for(int k = 0; k < pl1.length(); k++)
    {
        LSMLine * lk  = pl1.at(k);

        //split this line based on all the lines in part b
        int vcount = lk->GetVertexCount();

        bool intersect = false;

        LSMLine * l = new LSMLine();

        std::vector<std::pair<LSMVector2,double>> collides;
        for(int i = 0; i < vcount-1; i++)
        {
            collides.clear();
            LSMVector2 v = lk->GetVertex(i);
            l->AddVertex(v.x,v.y);

            double x1,x2,y1,y2;
            double x3,x4,y3,y4;

            if(i != vcount - 1)
            {
                x1 =  lk->GetVertex(i).x;
                y1 =  lk->GetVertex(i).y;
                x2 =  lk->GetVertex(i+1).x;
                y2 =  lk->GetVertex(i+1).y;

                //not used here, no polygon
            }else {
                x1 =  lk->GetVertex(i).x;
                y1 =  lk->GetVertex(i).y;
                x2 =  lk->GetVertex(0).x;
                y2 =  lk->GetVertex(0).y;
            }

            for(int m = 0; m < pl2.size(); m++)
            {
                for(int j =0; j < pl2.at(m)->GetVertexCount(); j++)
                {
                    if(j != pl2.at(m)->GetVertexCount() - 1)
                    {
                        x3 = pl2.at(m)->GetVertex(j).x;
                        y3 = pl2.at(m)->GetVertex(j).y;
                        x4 = pl2.at(m)->GetVertex(j+1).x;
                        y4 = pl2.at(m)->GetVertex(j+1).y;
                    }else {
                        x3 = pl2.at(m)->GetVertex(j).x;
                        y3 = pl2.at(m)->GetVertex(j).y;
                        x4 = pl2.at(m)->GetVertex(0).x;
                        y4 = pl2.at(m)->GetVertex(0).y;
                    }
                    LSMVector2 collide;
                    bool doescollide = Geometry_lineLine(x1,y1,x2,y2,x3,y3,x4,y4,collide);
                    if(doescollide)
                    {
                        ret_pol_B->append(new LSMPoint(collide.x,collide.y));

                        collides.push_back(std::make_pair<LSMVector2,double>(LSMVector2(collide.x,collide.y),(double)(collide - v).length()));
                    }
                }

            }

            //sort collisions
            std::sort(collides.begin(), collides.end(),[](const std::pair<LSMVector2,double> &a,const std::pair<LSMVector2,double> &b)
            {
                return a.second < b.second;
            });

            //now we do our things
            for(int j = 0; j < collides.size(); j++)
            {
                l->AddVertex(collides.at(j).first.x,collides.at(j).first.y);
                ret_pol_A->append(l);
                l = new LSMLine();
                l->AddVertex(collides.at(j).first.x,collides.at(j).first.y);
            }
        }

        ret_pol_A->append(l);

    }


    pl1 = p1;
    pl2 = p2;

    for(int k = 0; k < pl1.length(); k++)
    {
        LSMLine * lk  = pl1.at(k);

        //split this line based on all the lines in part b
        int vcount = lk->GetVertexCount();

        bool intersect = false;

        LSMLine * l = new LSMLine();

        std::vector<std::pair<LSMVector2,double>> collides;
        for(int i = 0; i < vcount-1; i++)
        {
            collides.clear();
            LSMVector2 v = lk->GetVertex(i);
            l->AddVertex(v.x,v.y);

            double x1,x2,y1,y2;
            double x3,x4,y3,y4;

            if(i != vcount - 1)
            {
                x1 =  lk->GetVertex(i).x;
                y1 =  lk->GetVertex(i).y;
                x2 =  lk->GetVertex(i+1).x;
                y2 =  lk->GetVertex(i+1).y;

                //not used here, no polygon
            }else {
                x1 =  lk->GetVertex(i).x;
                y1 =  lk->GetVertex(i).y;
                x2 =  lk->GetVertex(0).x;
                y2 =  lk->GetVertex(0).y;
            }

            for(int m = 0; m < pl2.size(); m++)
            {
                for(int j =0; j < pl2.at(m)->GetVertexCount(); j++)
                {
                    if(j != pl2.at(m)->GetVertexCount() - 1)
                    {
                        x3 = pl2.at(m)->GetVertex(j).x;
                        y3 = pl2.at(m)->GetVertex(j).y;
                        x4 = pl2.at(m)->GetVertex(j+1).x;
                        y4 = pl2.at(m)->GetVertex(j+1).y;
                    }else {
                        x3 = pl2.at(m)->GetVertex(j).x;
                        y3 = pl2.at(m)->GetVertex(j).y;
                        x4 = pl2.at(m)->GetVertex(0).x;
                        y4 = pl2.at(m)->GetVertex(0).y;
                    }
                    LSMVector2 collide;
                    bool doescollide = Geometry_lineLine(x1,y1,x2,y2,x3,y3,x4,y4,collide);
                    if(doescollide)
                    {
                        ret_pol_B->append(new LSMPoint(collide.x,collide.y));

                        collides.push_back(std::make_pair<LSMVector2,double>(LSMVector2(collide.x,collide.y),(double)(collide - v).length()));
                    }
                }

            }

            //sort collisions
            std::sort(collides.begin(), collides.end(),[](const std::pair<LSMVector2,double> &a,const std::pair<LSMVector2,double> &b)
            {
                return a.second < b.second;
            });

            //now we do our things
            for(int j = 0; j < collides.size(); j++)
            {
                l->AddVertex(collides.at(j).first.x,collides.at(j).first.y);
                ret_pol_C->append(l);
                l = new LSMLine();
                l->AddVertex(collides.at(j).first.x,collides.at(j).first.y);
            }
        }

        ret_pol_C->append(l);

    }

}





