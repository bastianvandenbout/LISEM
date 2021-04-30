#ifndef SHAPEEDITING_H
#define SHAPEEDITING_H

#include "QString"
#include "geometry/shape.h"
#include "QList"
#include "geometry/feature.h"
#include "geo/shapes/shapelayer.h"
#include "geo/shapes/shapefile.h"
#include "matrixtable.h"

#define SHAPELAYER_EDIT_NONE -1
#define SHAPELAYER_EDIT_REMOVE 0
#define SHAPELAYER_EDIT_ADD 1
#define SHAPELAYER_EDIT_REPLACE 2
#define SHAPELAYER_EDIT_REPLACEATTRIBUTE 3
#define SHAPELAYER_EDIT_REPLACEATTRIBUTE 3

typedef struct ShapeLayerChange
{

    int Type;
    ShapeLayer * layer_old = nullptr;
    Feature * feature_old = nullptr;
    LSMShape * shape_old = nullptr;
    ShapeLayer * layer_new = nullptr;
    Feature * feature_new = nullptr;
    LSMShape * shape_new = nullptr;
    MatrixTable * table_new = nullptr;
    MatrixTable * table_old = nullptr;
    QList<QString> attributevalues;
    int ShapeIndex;
    int FeatureIndex;
    int LayerIndex;
    QString AttributeName;
    int AttributeLayer = 0;
    QList<QString> AttributeValues;
    QList<QString> AttributeValuesOld;


} ShapeLayerChange;


static inline ShapeLayerChange ShapeLayerChangeRemoveShape(ShapeFile * shapefile, LSMShape * s)
{
    ShapeLayerChange c;

    bool found = false;
    for(int i = 0; i < shapefile->GetLayerCount(); i++)
    {
        ShapeLayer * sl = shapefile->GetLayer(i);
        for(int j = 0; j < sl->GetFeatureCount(); j++)
        {
            Feature * f = sl->GetFeature(j);
            for(int k = 0; k < f->GetShapeCount(); k++)
            {
                if(f->GetShape(k) == s)
                {
                    found = true;
                    c.ShapeIndex = k;
                    c.FeatureIndex = j;
                    c.LayerIndex = i;
                    break;
                }

            }
        }
    }
    if(found)
    {

        c.Type = SHAPELAYER_EDIT_REMOVE;
        c.shape_old = s;
        return c;
    }else {
        c.Type = SHAPELAYER_EDIT_NONE;
        return c;
    }
}
static inline ShapeLayerChange ShapeLayerChangeAddShape(ShapeFile * shapefile, Feature * f, LSMShape * s)
{
    ShapeLayerChange c;

    if(s != nullptr)
    {
        bool found = false;
        for(int i = 0; i < shapefile->GetLayerCount(); i++)
        {
            ShapeLayer * sl = shapefile->GetLayer(i);
            for(int j = 0; j < sl->GetFeatureCount(); j++)
            {
                Feature * fc = sl->GetFeature(j);
                if(fc == f)
                {
                    found = true;
                    c.FeatureIndex = j;
                    c.LayerIndex = i;
                }
            }
        }

        if(found)
        {
            c.Type = SHAPELAYER_EDIT_ADD;
            c.shape_new = s;
            return c;
        }else {
            c.Type = SHAPELAYER_EDIT_NONE;
            return c;
        }
    }else {
        c.Type = SHAPELAYER_EDIT_NONE;
        return c;
    }
}
static inline ShapeLayerChange ShapeLayerChangeReplaceShape(ShapeFile * shapefile,LSMShape * s,LSMShape * s2)
{
    ShapeLayerChange c;

    bool found = false;
    for(int i = 0; i < shapefile->GetLayerCount(); i++)
    {
        ShapeLayer * sl = shapefile->GetLayer(i);
        for(int j = 0; j < sl->GetFeatureCount(); j++)
        {
            Feature * f = sl->GetFeature(j);
            for(int k = 0; k < f->GetShapeCount(); k++)
            {
                if(f->GetShape(k) == s)
                {
                    found = true;
                    c.ShapeIndex = k;
                    c.FeatureIndex = j;
                    c.LayerIndex = i;
                    break;
                }

            }
        }
    }
    if(found)
    {

        c.Type = SHAPELAYER_EDIT_REPLACE;
        c.shape_old = s;
        c.shape_new= s2;
        return c;
    }else {
        c.Type = SHAPELAYER_EDIT_NONE;
        return c;
    }
}
static inline ShapeLayerChange ShapeLayerChangeRemoveFeature(ShapeFile * shapefile, Feature * f)
{
    ShapeLayerChange c;

    bool found = false;
    for(int i = 0; i < shapefile->GetLayerCount(); i++)
    {
        ShapeLayer * sl = shapefile->GetLayer(i);
        for(int j = 0; j < sl->GetFeatureCount(); j++)
        {
            if(sl->GetFeature(j) == f)
            {

                found = true;
                c.ShapeIndex = 0;
                c.FeatureIndex = j;
                c.LayerIndex = i;
                break;
            }
        }
    }
    if(found)
    {

        c.Type = SHAPELAYER_EDIT_REMOVE;
        c.feature_old = f;
        return c;
    }else {
        c.Type = SHAPELAYER_EDIT_NONE;
        return c;
    }
}

static inline ShapeLayerChange ShapeLayerChangeReplaceFeature(ShapeFile * shapefile, Feature * f, Feature * f2)
{
    ShapeLayerChange c;

    bool found = false;
    for(int i = 0; i < shapefile->GetLayerCount(); i++)
    {
        ShapeLayer * sl = shapefile->GetLayer(i);
        for(int j = 0; j < sl->GetFeatureCount(); j++)
        {
            if(sl->GetFeature(j) == f)
            {
                found = true;
                c.ShapeIndex = 0;
                c.FeatureIndex = j;
                c.LayerIndex = i;
                break;
            }
        }
    }
    if(found)
    {

        c.Type = SHAPELAYER_EDIT_REPLACE;
        c.feature_old = f;
        return c;
    }else {
        c.Type = SHAPELAYER_EDIT_NONE;
        return c;
    }
}

static inline ShapeLayerChange ShapeLayerChangeReplaceFeatureAttributes(ShapeFile * shapefile, Feature * f, QList<QString> attributevalues)
{
    ShapeLayerChange c;

    bool found = false;
    for(int i = 0; i < shapefile->GetLayerCount(); i++)
    {
        ShapeLayer * sl = shapefile->GetLayer(i);
        for(int j = 0; j < sl->GetFeatureCount(); j++)
        {
            if(sl->GetFeature(j) == f)
            {
                found = true;
                c.ShapeIndex = 0;
                c.FeatureIndex = j;
                c.LayerIndex = i;
                break;
            }
        }
    }
    if(found)
    {

        c.Type = SHAPELAYER_EDIT_REPLACE;
        c.attributevalues = attributevalues;
        c.AttributeValuesOld = shapefile->GetLayer(c.LayerIndex)->GetAttributeValuesForFeature(c.FeatureIndex);
        return c;
    }else {
        c.Type = SHAPELAYER_EDIT_NONE;
        return c;
    }
}

static inline ShapeLayerChange ShapeLayerChangeAddFeature(ShapeFile * shapefile, ShapeLayer * l,Feature * f,QList<QString> attributevalues)
{
    ShapeLayerChange c;

    if(f != nullptr)
    {
        bool found = false;
        for(int i = 0; i < shapefile->GetLayerCount(); i++)
        {
            ShapeLayer * sl = shapefile->GetLayer(i);
            if(sl == l)
            {
                found = true;
                c.LayerIndex = i;
            }
        }


        if(found)
        {
            c.attributevalues = attributevalues;
            c.Type = SHAPELAYER_EDIT_ADD;
            c.feature_new = f;
            return c;
        }else {
            c.Type = SHAPELAYER_EDIT_NONE;
            return c;
        }
    }else {
        c.Type = SHAPELAYER_EDIT_NONE;
        return c;
    }
}

static inline ShapeLayerChange ShapeLayerChangeAddAttribute(ShapeFile * shapefile, QString name, int type,int LayerIndex = 0)
{
    return ShapeLayerChange();
}
static inline ShapeLayerChange ShapeLayerChangeRemoveAttribute(ShapeFile * shapefile, QString name,int LayerIndex = 0)
{
    return ShapeLayerChange();
}
static inline ShapeLayerChange ShapeLayerChangeReplaceAttribute(ShapeFile * shapefile, QString name, QList<QString> values,int LayerIndex = 0)
{
    return ShapeLayerChange();
}
static inline ShapeLayerChange ShapeLayerChangeReplaceAllAttributes(ShapeFile * shapefile, MatrixTable * attributes,int LayerIndex = 0)
{

    ShapeLayerChange c;
    c.table_new = attributes;
    c.table_old = shapefile->GetLayer(LayerIndex)->GetAttributeTable();
    c.LayerIndex = LayerIndex;
    c.Type ==  SHAPELAYER_EDIT_REPLACE;
    return c;
}

static inline ShapeLayerChange ShapeLayerChangeSetFeatureAttribute(ShapeFile * shapefile,QString name, QString Value,int LayerIndex = 0)
{
    return ShapeLayerChange();
}


static inline void ApplyShapeLayerChange(ShapeFile * sf,ShapeLayerChange change)
{
    std::cout << "change " << sf << " " <<change.feature_new << std::endl;
    if(change.Type ==  SHAPELAYER_EDIT_ADD)
    {

        if(change.shape_new != nullptr)
        {
            sf->GetLayer(change.LayerIndex)->GetFeature(change.FeatureIndex)->AddShape(change.shape_new);
        }else if(change.feature_new != nullptr)
        {

            std::cout << "add_to_feature " << std::endl;

            int indexf = sf->GetLayer(change.LayerIndex)->AddFeature(change.feature_new);
            for(int i = 0; i < change.attributevalues.length(); i++)
            {
                if(!(i < sf->GetLayer(change.LayerIndex)->GetNumberOfAttributes()))
                {
                    break;
                }
                sf->GetLayer(change.LayerIndex)->SetFeatureAttribute(indexf, sf->GetLayer(change.LayerIndex)->GetAttributeName(i), change.attributevalues.at(i) );
            }

        }else if(!change.AttributeName.isEmpty())
        {

        }


    }else if(change.Type ==  SHAPELAYER_EDIT_REMOVE)
    {
        if(change.shape_old != nullptr)
        {
            sf->GetLayer(change.LayerIndex)->GetFeature(change.FeatureIndex)->RemoveShape(change.shape_old);

        }else if(change.feature_old != nullptr)
        {
            sf->GetLayer(change.LayerIndex)->RemoveFeature(change.feature_old);

        }else if(!change.AttributeName.isEmpty())
        {

        }

    }else if(change.Type ==  SHAPELAYER_EDIT_REPLACE)
    {
        if(change.shape_old != nullptr && change.shape_new != nullptr)
        {
            sf->GetLayer(change.LayerIndex)->GetFeature(change.FeatureIndex)->ReplaceShape(change.shape_old, change.shape_new);

        }else if(change.feature_old != nullptr && change.feature_new != nullptr)
        {
            sf->GetLayer(change.LayerIndex)->ReplaceFeature(change.feature_old, change.feature_new);

        }else if(!change.AttributeValues.isEmpty())
        {
            sf->GetLayer(change.LayerIndex)->ReplaceAttributes(change.FeatureIndex,change.AttributeValuesOld);

        }else if(change.table_new != nullptr && change.table_old != nullptr)
        {
            sf->GetLayer(change.LayerIndex)->SetAttributeTable(change.table_new);
        }
    }

}

static inline void UndoShapeLayerChange(ShapeFile * sf,ShapeLayerChange change)
{

    if(change.Type ==  SHAPELAYER_EDIT_ADD)
    {

        if(change.shape_new != nullptr)
        {
            sf->GetLayer(change.LayerIndex)->GetFeature(change.FeatureIndex)->RemoveShape(change.shape_new);
        }else if(change.feature_new != nullptr)
        {
            sf->GetLayer(change.LayerIndex)->RemoveFeature(change.feature_new);
        }else if(!change.AttributeName.isEmpty())
        {

        }


    }else if(change.Type ==  SHAPELAYER_EDIT_REMOVE)
    {
        if(change.shape_old != nullptr)
        {
            sf->GetLayer(change.LayerIndex)->GetFeature(change.FeatureIndex)->AddShape(change.shape_old);

        }else if(change.feature_old != nullptr)
        {
            sf->GetLayer(change.LayerIndex)->AddFeature(change.feature_old);

        }else if(!change.AttributeName.isEmpty())
        {

        }

    }else if(change.Type ==  SHAPELAYER_EDIT_REPLACE)
    {
        if(change.shape_old != nullptr && change.shape_new != nullptr)
        {
            sf->GetLayer(change.LayerIndex)->GetFeature(change.FeatureIndex)->ReplaceShape(change.shape_new, change.shape_old);

        }else if(change.feature_old != nullptr && change.feature_new != nullptr)
        {
            sf->GetLayer(change.LayerIndex)->ReplaceFeature(change.feature_new, change.feature_old);

        }else if(!change.AttributeValues.isEmpty())
        {
            sf->GetLayer(change.LayerIndex)->ReplaceAttributes(change.FeatureIndex,change.AttributeValuesOld);
        }else if(change.table_new != nullptr && change.table_old != nullptr)
        {
            sf->GetLayer(change.LayerIndex)->SetAttributeTable(change.table_old);
        }
    }


}


static inline void DestroyShapeLayerChange(ShapeLayerChange change)
{



}


#endif // SHAPEEDITING_H
