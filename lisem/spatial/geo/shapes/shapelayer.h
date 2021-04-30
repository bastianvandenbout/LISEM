#ifndef SHAPELAYER_H
#define SHAPELAYER_H

#include "interval.h"
#include "geometry/shape.h"
#include "geometry/feature.h"
#include "QString"
#include "QList"
#include "error.h"
#include <iostream>
#include "ogr_api.h"
#include "ogrsf_frmts.h"
#include "gdal.h"
#include "glgeometrylist.h"
#include "matrixtable.h"
#include "geometry/geometrybase.h"

#define ATTRIBUTE_TYPE_UNKNOWN 0
#define ATTRIBUTE_TYPE_INT32 1
#define ATTRIBUTE_TYPE_INT64 2
#define ATTRIBUTE_TYPE_FLOAT64 3
#define ATTRIBUTE_TYPE_BOOL 4
#define ATTRIBUTE_TYPE_STRING 5

#define LISEM_FIELD_PRECISION 50

//matches the Table Type definitions
class  AttributeList
{
public:
    QString m_Name;
    int m_type;

    QList<int64_t> m_ListInt64;
    QList<int32_t> m_ListInt32;
    QList<double> m_ListFloat64;
    QList<bool> m_ListBool;
    QList<QString> m_ListString;

    inline void Clear()
    {
        m_ListInt64.clear();
        m_ListInt32.clear();
        m_ListFloat64.clear();
        m_ListBool.clear();
        m_ListString.clear();
    }
    QString GetTypeAsString()
    {
        if(m_type == ATTRIBUTE_TYPE_INT32)
        {
            return "Integer (32-bit)";
        }else if(m_type == ATTRIBUTE_TYPE_INT64)
        {
           return "Integer (64-bit)";
       }else if(m_type == ATTRIBUTE_TYPE_FLOAT64)
        {
           return "Float (64-bit)";
       }else if(m_type == ATTRIBUTE_TYPE_BOOL)
        {
            return "Boolean";
        }else if(m_type == ATTRIBUTE_TYPE_STRING)
        {
           return "text";
       }else
        {
           return "Unkown (text assumed)";
        }

    }
    QString GetValueAsString(int i)
    {
        if(m_type == ATTRIBUTE_TYPE_INT32)
        {
             if(m_ListInt32.length() > i)
             {
                 return QString::number(m_ListInt32.at(i));
             }else {
                return "";
            }
        }else if(m_type == ATTRIBUTE_TYPE_INT64)
        {
            if(m_ListInt64.length() > i)
            {
                return QString::number(m_ListInt64.at(i));
            }else {
               return "";
           }
       }else if(m_type == ATTRIBUTE_TYPE_FLOAT64)
        {
            if(m_ListFloat64.length() > i)
            {
                return QString::number(m_ListFloat64.at(i));
            }else {
               return "";
           }
       }else if(m_type == ATTRIBUTE_TYPE_BOOL)
        {
            if(m_ListBool.length() > i)
            {
                return m_ListBool.at(i)? "true":  "false";
            }else {
               return "";
            }
        }else if(m_type == ATTRIBUTE_TYPE_STRING)
        {
            if(m_ListString.length() > i)
            {
                return m_ListString.at(i);
            }else {
               return "";
            }

       }else
        {
            if(m_ListString.length() > i)
            {
                return m_ListString.at(i);
            }else {
               return "";
            }
        }

    }

    inline bool isNumberType()
    {
        if(m_type == ATTRIBUTE_TYPE_INT32)
        {
                return true;
        }else if(m_type == ATTRIBUTE_TYPE_INT64)
        {
                return true;
       }else if(m_type == ATTRIBUTE_TYPE_FLOAT64)
        {
                return true;
       }else if(m_type == ATTRIBUTE_TYPE_BOOL)
        {
                return true;
        }else if(m_type == ATTRIBUTE_TYPE_STRING)
        {
                return false;
       }else
        {
                return false;
        }
    }

    LSMInterval GetValueInterval()
    {
        double min = 0.0;
        double max = 0.0;
        bool first = true;


        if(m_type == ATTRIBUTE_TYPE_INT32)
        {
            for(int i = 0; i < m_ListInt32.length(); i++)
            {
                if(first)
                {
                    first = false;
                    min = m_ListInt64.at(i);
                    max = m_ListInt64.at(i);
                }else {
                    min = std::min((double)(m_ListInt32.at(i)),min);
                    max = std::max((double)(m_ListInt32.at(i)),max);
                }
            }
        }else if(m_type == ATTRIBUTE_TYPE_INT64)
        {
            for(int i = 0; i < m_ListInt64.length(); i++)
            {
                if(first)
                {
                    first = false;
                    min = m_ListInt64.at(i);
                    max = m_ListInt64.at(i);
                }else {
                    min = std::min((double)(m_ListInt64.at(i)),min);
                    max = std::max((double)(m_ListInt64.at(i)),max);
                }
            }
       }else if(m_type == ATTRIBUTE_TYPE_FLOAT64)
        {
            for(int i = 0; i < m_ListFloat64.length(); i++)
            {
                if(first)
                {
                    first = false;
                    min = m_ListFloat64.at(i);
                    max = m_ListFloat64.at(i);
                }else {
                    min = std::min((double)(m_ListFloat64.at(i)),min);
                    max = std::max((double)(m_ListFloat64.at(i)),max);
                }
            }

       }else if(m_type == ATTRIBUTE_TYPE_BOOL)
        {
            min = 0;
            max = 1;
        }else if(m_type == ATTRIBUTE_TYPE_STRING)
        {
            for(int i = 0; i < m_ListString.length(); i++)
            {
                if(first)
                {
                    first = false;
                    min = m_ListString.at(i).toDouble();
                    max = m_ListString.at(i).toDouble();
                }else {
                    min = std::min((double)(m_ListString.at(i).toDouble()),min);
                    max = std::max((double)(m_ListString.at(i).toDouble()),max);
                }
            }

       }else
        {
            for(int i = 0; i < m_ListString.length(); i++)
            {
                if(first)
                {
                    first = false;
                    min = m_ListString.at(i).toDouble();
                    max = m_ListString.at(i).toDouble();
                }else {
                    min = std::min((double)(m_ListString.at(i).toDouble()),min);
                    max = std::max((double)(m_ListString.at(i).toDouble()),max);
                }
            }
        }

        return LSMInterval(min,max);
    }
};


class ShapeLayer
{

private:
    QString m_Name;
    int m_ShapeType;
    QList<Feature*> m_FeatureList;

    QList<AttributeList *> m_AttributeList;

    BoundingBox m_BoundingBox;

    float xmin;
    float ymin;
    float zmin;
    float xmax;
    float ymax;
    float zmax;

public:

    inline ShapeLayer()
    {

    }

    inline ShapeLayer(const ShapeLayer &s)
    {
        CopyFrom(&s);
    }

    inline ShapeLayer * Copy()
    {

        ShapeLayer * layer = new ShapeLayer();

        layer->m_Name = m_Name;
        layer->m_ShapeType = m_ShapeType;

        layer->xmin = xmin;
        layer->xmax = xmax;
        layer->ymin = ymin;
        layer->ymax = ymax;
        layer->zmin = zmin;
        layer->zmax = zmax;

        for(int i = 0; i < m_FeatureList.length(); i++)
        {
            layer->AddFeature(m_FeatureList.at(i)->Copy());
        }

        for(int i = 0; i < m_AttributeList.length(); i++)
        {
            layer->AddAttribute(m_AttributeList.at(i));
        }

        return layer;

    }


    inline void CopyFrom(const ShapeLayer *layer )
    {

        Destroy();

        m_Name = layer->m_Name;
        m_ShapeType = layer->m_ShapeType;

        xmin = layer->xmin;
        xmax = layer->xmax;
        ymin = layer->ymin;
        ymax = layer->ymax;
        zmin = layer->zmin;
        zmax = layer->zmax;

        for(int i = 0; i < layer->m_FeatureList.length(); i++)
        {
            AddFeature(layer->m_FeatureList.at(i)->Copy());
        }

        for(int i = 0; i < layer->m_AttributeList.length(); i++)
        {
            AddAttribute(layer->m_AttributeList.at(i));
        }

    }



    inline BoundingBox GetBoundingBox()
    {
        return m_BoundingBox;
    }

    inline BoundingBox GetAndCalcBoundingBox()
    {
        BoundingBox b;
        if(m_FeatureList.length() > 0)
        {
            b = m_FeatureList.at(0)->GetBoundingBox();

            for(int i = 1; i < m_FeatureList.length(); i++)
            {
                BoundingBox b2 = m_FeatureList.at(i)->GetBoundingBox();
                if(!(b2.GetMaxX() == 0.0 && b2.GetMinX() == 0.0 && b2.GetMaxY() == 0.0 && b2.GetMinY() == 0.0))
                {
                    b.MergeWith(b2);
                }


            }
        }
        m_BoundingBox = b;

        return b;
    }

    //virtual
    inline int GetShapeCount()
    {
        int count = 0;
        for(int i =0; i < m_FeatureList.length(); i++)
        {
            count += m_FeatureList.at(i)->GetShapeCount();
        }
        return count;
    }
    inline void SetType(int x)
    {
        m_ShapeType = x;
    }
    inline int GetType()
    {
        return m_ShapeType;
    }
    inline QString GetName()
    {
        return m_Name;
    }
    inline void SetName(QString m)
    {
        m_Name = m;
    }
    inline int GetFeatureCount()
    {
        return m_FeatureList.length();
    }
    inline Feature * GetFeature(int i)
    {
        return m_FeatureList.at(i);
    }
    inline int GetIndexOfFeature(Feature * f)
    {
        return m_FeatureList.indexOf(f);
    }
    inline QList<QString> GetAttributeValuesForFeature(int i)
    {
        QList<QString> vals;
        if( i > -1 && i < m_FeatureList.length())
        {
            for(int j = 0; j < m_AttributeList.size(); j++)
            {
                vals.push_back(m_AttributeList.at(j)->GetValueAsString(i));
            }
        }else {

            for(int j = 0; j < m_AttributeList.size(); j++)
            {
                vals.push_back("");
            }
        }

        return vals;
    }

    inline void RemoveFeature(int i)
    {
        m_FeatureList.removeAt(i);
        for(int j = 0; j < m_AttributeList.length(); j++)
        {
            AttributeList *AL =m_AttributeList.at(j);
            int type = m_AttributeList.at(j)->m_type;
            if(type == ATTRIBUTE_TYPE_INT32)
            {
                    AL->m_ListInt32.removeAt(i);
            }else if(type == ATTRIBUTE_TYPE_INT64)
            {
                    AL->m_ListInt64.removeAt(i);
           }else if(type == ATTRIBUTE_TYPE_FLOAT64)
            {
                    AL->m_ListFloat64.removeAt(i);
           }else if(type == ATTRIBUTE_TYPE_BOOL)
            {
                    AL->m_ListBool.removeAt(i);
           }else if(type == ATTRIBUTE_TYPE_STRING)
            {
                    AL->m_ListString.removeAt(i);
           }else
            {
                    AL->m_ListString.removeAt(i);
            }
        }
        //GetAndCalcBoundingBox();
    }
    inline void RemoveFeature(Feature* f)
    {
        RemoveFeature(m_FeatureList.indexOf(f));

        //GetAndCalcBoundingBox();
    }
    inline void ReplaceFeature(Feature* f,Feature* f2)
    {
        int i = m_FeatureList.indexOf(f);
        if( i > -1)
        {
            m_FeatureList.replace(i,f2);
            GetBoundingBox();
        }
    }

    inline void ReplaceAttributes(int findex, QList<QString> vals)
    {

    }
    inline int AddFeature(Feature* f)
    {
        m_FeatureList.append(f);

        for(int i = 0; i < m_AttributeList.length(); i++)
        {
            AttributeList *AL =m_AttributeList.at(i);
            int type = m_AttributeList.at(i)->m_type;
            if(type == ATTRIBUTE_TYPE_INT32)
            {
                    AL->m_ListInt32.append(0);
            }else if(type == ATTRIBUTE_TYPE_INT64)
            {
                    AL->m_ListInt64.append(0);
           }else if(type == ATTRIBUTE_TYPE_FLOAT64)
            {
                    AL->m_ListFloat64.append(0);
           }else if(type == ATTRIBUTE_TYPE_BOOL)
            {
                    AL->m_ListBool.append(0);
           }else if(type == ATTRIBUTE_TYPE_STRING)
            {
                    AL->m_ListString.append(QString(" "));
           }else
            {
                    AL->m_ListString.append(QString(" "));
            }
        }

        //set the type of this layer to the type of the latest added feature


        this->SetType(f->GetType());




        return m_FeatureList.length() -1;
    }

    inline int HasFeature(Feature * f)
    {
        int index =m_FeatureList.indexOf(f);
        return index;
    }

    inline int AddAttribute(AttributeList * tocopy)
    {

        AttributeList * AL = new AttributeList();
        AL->m_ListBool = tocopy->m_ListBool;
        AL->m_ListInt32 = tocopy->m_ListInt32;
        AL->m_ListInt64 = tocopy->m_ListInt64;
        AL->m_ListFloat64 = tocopy->m_ListFloat64;
        AL->m_ListString = tocopy->m_ListString;
        AL->m_Name = tocopy->m_Name;
        AL->m_type = tocopy->m_type;

        m_AttributeList.append(AL);

        return m_AttributeList.length() -1;
    }

    inline int AddAttribute(QString name, int type)
    {
        int count = m_FeatureList.length();

        AttributeList * AL = new AttributeList();
        AL->m_ListBool.clear();
        AL->m_ListInt32.clear();
        AL->m_ListInt64.clear();
        AL->m_ListFloat64.clear();
        AL->m_ListString.clear();
        AL->m_Name = name;
        AL->m_type = type;

        if(type == ATTRIBUTE_TYPE_INT32)
            {
                for(int i =0; i < count; i++)
                {
                    AL->m_ListInt32.append(0);
                }
                this->m_AttributeList.append(AL);
            }else
        if(type ==  ATTRIBUTE_TYPE_INT64)
            {
                for(int i =0; i < count; i++)
                {
                    AL->m_ListInt64.append(0);
                }
                this->m_AttributeList.append(AL);
            }else
        if(type == ATTRIBUTE_TYPE_FLOAT64)
            {
                for(int i =0; i < count; i++)
                {
                    AL->m_ListFloat64.append(0);
                }
                this->m_AttributeList.append(AL);
            }else
       if(type ==  ATTRIBUTE_TYPE_BOOL)
            {
                for(int i =0; i < count; i++)
                {
                    AL->m_ListBool.append(0);
                }
                this->m_AttributeList.append(AL);
            }else
       if(type == ATTRIBUTE_TYPE_STRING)
        {
            for(int i =0; i < count; i++)
            {
                AL->m_ListString.append(QString(""));
            }
            this->m_AttributeList.append(AL);
        }else
        {
            LISEM_DEBUG("Could not load attribute type for: " + name + " Loaded instead as string");
            AL->m_type = ATTRIBUTE_TYPE_STRING;
            for(int i =0; i < count; i++)
            {
                AL->m_ListString.append(QString(""));
            }
            this->m_AttributeList.append(AL);
        }

        return m_AttributeList.length()-1;
    }

    inline void SetFeatureAttribute(int indexf,QString name, int64_t attribute)
    {
        bool founda = 0;

        AttributeList * AL;
        for(int i = m_AttributeList.length()-1; i > -1; i--)
        {
            AL =m_AttributeList.at(i);
            if(name.compare(m_AttributeList.at(i)->m_Name) == 0)
            {
                founda = true;
                break;
            }
        }

        if(!founda)
        {
            return;
        }

        int type = AL->m_type;

        if(type == ATTRIBUTE_TYPE_INT32)
            {
                if(AL->m_ListInt32.length() > indexf)
                {
                    AL->m_ListInt32.replace(indexf,attribute);
                }
            }
        else if(type == ATTRIBUTE_TYPE_INT64)
            {
                if(AL->m_ListInt64.length() > indexf)
                {
                    AL->m_ListInt64.replace(indexf,attribute);
                }
            }
       else if(type == ATTRIBUTE_TYPE_FLOAT64)
        {
                if(AL->m_ListFloat64.length() > indexf)
                {
                    AL->m_ListFloat64.replace(indexf,attribute);
                }
        }
       else if(type == ATTRIBUTE_TYPE_BOOL)
        {
                if(AL->m_ListBool.length() > indexf)
                {
                    AL->m_ListBool.replace(indexf,(bool)attribute == 0);
                }
        }
        else if(type == ATTRIBUTE_TYPE_STRING)
        {
                if(AL->m_ListString.length() > indexf)
                {
                    AL->m_ListString.replace(indexf,QString::number(attribute));
                }
        }else if(type == ATTRIBUTE_TYPE_UNKNOWN)
        {
                if(AL->m_ListString.length() > indexf)
                {
                    AL->m_ListString.replace(indexf,QString::number(attribute));
                }
        }

    }

    inline void SetFeatureAttribute(int indexf,int attribindex, int64_t attribute)
    {
        bool founda = 0;

        AttributeList * AL =m_AttributeList.at(attribindex);

        int type = AL->m_type;

        if(type == ATTRIBUTE_TYPE_INT32)
            {
                if(AL->m_ListInt32.length() > indexf)
                {
                    AL->m_ListInt32.replace(indexf,attribute);
                }
            }
        else if(type == ATTRIBUTE_TYPE_INT64)
            {
                if(AL->m_ListInt64.length() > indexf)
                {
                    AL->m_ListInt64.replace(indexf,attribute);
                }
            }
       else if(type == ATTRIBUTE_TYPE_FLOAT64)
        {
                if(AL->m_ListFloat64.length() > indexf)
                {
                    AL->m_ListFloat64.replace(indexf,attribute);
                }
        }
       else if(type == ATTRIBUTE_TYPE_BOOL)
        {
                if(AL->m_ListBool.length() > indexf)
                {
                    AL->m_ListBool.replace(indexf,(bool)attribute == 0);
                }
        }
        else if(type == ATTRIBUTE_TYPE_STRING)
        {
                if(AL->m_ListString.length() > indexf)
                {
                    AL->m_ListString.replace(indexf,QString::number(attribute));
                }
        }else if(type == ATTRIBUTE_TYPE_UNKNOWN)
        {
                if(AL->m_ListString.length() > indexf)
                {
                    AL->m_ListString.replace(indexf,QString::number(attribute));
                }
        }

    }
    inline void SetFeatureAttribute(Feature*f,QString name, int64_t attribute)
    {
        //get index of f
        //because of regular usage, it is faster to start at the end

        bool found = 0;

        int indexf;
        for(int i = m_FeatureList.length()-1; i > -1; i--)
        {
            if(f == m_FeatureList.at(i))
            {
                indexf = i;
                found = true;
                break;
            }
        }

        if(!found)
        {
            return;
        }
        SetFeatureAttribute(indexf,name,attribute);

    }

    inline void SetFeatureAttribute(int indexf,QString name, double attribute)
    {
        bool founda = 0;

        AttributeList * AL;
        for(int i = m_AttributeList.length()-1; i > -1; i--)
        {
            AL =m_AttributeList.at(i);
            if(name.compare(m_AttributeList.at(i)->m_Name) == 0)
            {
                founda = true;
                break;
            }
        }

        if(!founda)
        {
            return;
        }

        int type = AL->m_type;
        if(type == ATTRIBUTE_TYPE_INT32)
        {
                if(AL->m_ListInt32.length() > indexf)
                {
                    AL->m_ListInt32.replace(indexf,attribute);
                }
        }else if(type == ATTRIBUTE_TYPE_INT64)
        {
                if(AL->m_ListInt64.length() > indexf)
                {
                    AL->m_ListInt64.replace(indexf,attribute);
                }
        }else if(type == ATTRIBUTE_TYPE_FLOAT64)
        {
                if(AL->m_ListFloat64.length() > indexf)
                {
                    AL->m_ListFloat64.replace(indexf,attribute);
                }
        }else if(type == ATTRIBUTE_TYPE_BOOL)
        {
                if(AL->m_ListBool.length() > indexf)
                {
                    AL->m_ListBool.replace(indexf,(bool)attribute == 0);
                }
        }else if(type ==  ATTRIBUTE_TYPE_STRING)
        {
                if(AL->m_ListString.length() > indexf)
                {
                    AL->m_ListString.replace(indexf,QString::number(attribute,'g',15));
                }
        }else if(type ==  ATTRIBUTE_TYPE_UNKNOWN)
        {
                if(AL->m_ListString.length() > indexf)
                {
                    AL->m_ListString.replace(indexf,QString::number(attribute,'g',15));
                }
        }
    }

    inline void SetFeatureAttribute(Feature*f,QString name, double attribute)
    {
        //get index of f
        //because of regular usage, it is faster to start at the end

        bool found = 0;

        int indexf;
        for(int i = m_FeatureList.length()-1; i > -1; i--)
        {
            if(f == m_FeatureList.at(i))
            {
                indexf = i;
                found = true;
                break;
            }
        }

        if(!found)
        {
            return;
        }

        SetFeatureAttribute(indexf,name,attribute);


    }

    inline void SetFeatureAttribute(int indexf,QString name, QString attribute)
    {
        bool founda = 0;

        AttributeList * AL;
        for(int i = m_AttributeList.length()-1; i > -1; i--)
        {
            AL =m_AttributeList.at(i);
            if(name.compare(m_AttributeList.at(i)->m_Name) == 0)
            {
                founda = true;
                break;
            }
        }

        if(!founda)
        {
            return;
        }

        int type = AL->m_type;

            if(type == ATTRIBUTE_TYPE_INT32)
            {
                if(AL->m_ListInt32.length() > indexf)
                {
                    AL->m_ListInt32.replace(indexf,attribute.toInt());
                }
            }
            else if(type == ATTRIBUTE_TYPE_INT64)
            {
                if(AL->m_ListInt64.length() > indexf)
                {
                    AL->m_ListInt64.replace(indexf,attribute.toInt());
                }
            }
             else if(type ==  ATTRIBUTE_TYPE_FLOAT64)
            {
                if(AL->m_ListFloat64.length() > indexf)
                {
                    AL->m_ListFloat64.replace(indexf,attribute.toFloat());
                }
            }
             else if(type == ATTRIBUTE_TYPE_BOOL)
            {
                if(AL->m_ListBool.length() > indexf)
                {
                    AL->m_ListBool.replace(indexf,attribute.toInt()==1);
                }
            }
             else if(type ==  ATTRIBUTE_TYPE_STRING)
            {
                if(AL->m_ListString.length() > indexf)
                {
                    AL->m_ListString.replace(indexf,attribute);
                }
            }else if(type == ATTRIBUTE_TYPE_UNKNOWN)
            {
                if(AL->m_ListString.length() > indexf)
                {
                    AL->m_ListString.replace(indexf,attribute);
                }
            }
    }

    inline void CopyFeatureAttributesFrom(int from, int to)
    {
        int indexf = to;
        AttributeList * AL;
        for(int i = m_AttributeList.length()-1; i > -1; i--)
        {
            AL =m_AttributeList.at(i);
            int type = AL->m_type;

                if(type == ATTRIBUTE_TYPE_INT32)
                {
                    if(AL->m_ListInt32.length() > indexf && AL->m_ListInt32.length() >from)
                    {
                        AL->m_ListInt32.replace(indexf,AL->m_ListInt32.at(from));
                    }
                }
                else if(type == ATTRIBUTE_TYPE_INT64)
                {
                    if(AL->m_ListInt64.length() > indexf && AL->m_ListInt64.length() > from)
                    {
                        AL->m_ListInt64.replace(indexf,AL->m_ListInt64.at(from));
                    }
                }
                 else if(type ==  ATTRIBUTE_TYPE_FLOAT64)
                {
                    if(AL->m_ListFloat64.length() > indexf && AL->m_ListFloat64.length() > from)
                    {
                        AL->m_ListFloat64.replace(indexf,AL->m_ListFloat64.at(from));
                    }
                }
                 else if(type == ATTRIBUTE_TYPE_BOOL)
                {
                    if(AL->m_ListBool.length() > indexf && AL->m_ListBool.length() > from)
                    {
                        AL->m_ListBool.replace(indexf,AL->m_ListBool.at(from));
                    }
                }
                 else if(type ==  ATTRIBUTE_TYPE_STRING)
                {
                    if(AL->m_ListString.length() > indexf && AL->m_ListString.length() > from)
                    {
                        AL->m_ListString.replace(indexf,AL->m_ListString.at(from));
                    }
                }else if(type == ATTRIBUTE_TYPE_UNKNOWN)
                {
                    if(AL->m_ListString.length() > indexf && AL->m_ListString.length() > from)
                    {
                        AL->m_ListString.replace(indexf,AL->m_ListString.at(from));
                    }
                }
        }


    }

    inline void SetFeatureAttribute(Feature*f,QString name, QString attribute)
    {
        //get index of f
        //because of regular usage, it is faster to start at the end

        bool found = 0;

        int indexf;
        for(int i = m_FeatureList.length()-1; i > -1; i--)
        {
            if(f == m_FeatureList.at(i))
            {
                indexf = i;
                found = true;
                break;
            }
        }

        if(!found)
        {
            return;
        }
        SetFeatureAttribute(indexf,name,attribute);

    }

    //attributes are done by this parent class
    inline int GetNumberOfAttributes()
    {
        return m_AttributeList.length();
    }
    inline QString GetAttributeName(int i)
    {
        return m_AttributeList.at(i)->m_Name;
    }

    inline int GetAttributeType(int i)
    {
        return m_AttributeList.at(i)->m_type;
    }

    inline QString GetAttributeValue(int i,int j)
    {
        if(i > -1 && j > -1 && i < m_AttributeList.length() && j < m_FeatureList.length())
        {
            return m_AttributeList.at(i)->GetValueAsString(j);
        }else {
            return "";
        }
    }

    inline QString GetAttributeValue(int i,Feature * f)
    {
        int index = m_FeatureList.indexOf(f);

        if(index > -1)
        {
            return GetAttributeValue(i,index);
        }
        return "";
    }

    inline AttributeList * GetAttributeList(int i)
    {
        return m_AttributeList.at(i);
    }

    inline int GetAttributeListIndex(QString name)
    {
        AttributeList * AL;
        for(int i = m_AttributeList.length()-1; i > -1; i--)
        {
            AL =m_AttributeList.at(i);
            if(name.compare(m_AttributeList.at(i)->m_Name) == 0)
            {
                return i;
            }
        }

        return -1;
    }




    LSMVector2 GetClosestEdgeLocation(LSMVector2 pos,Feature **f_ret,LSMShape ** s_ret = nullptr)
    {
        LSMVector2 closest = LSMVector2(1e31,1e31);
        float mindist = 1e31;
        if(s_ret != nullptr)
        {
            *s_ret = nullptr;
        }if(f_ret != nullptr)
        {
            *f_ret = nullptr;
        }


        for(int j = 0; j < m_FeatureList.length(); j++)
        {
            LSMShape ** s_retj = new (LSMShape *)();
            LSMVector2 p = m_FeatureList.at(j)->GetClosestEdgeLocation(pos,s_retj);
            float distance = LSMVector2(pos - p).length();
            if(std::isfinite(distance))
            {
                if(j == 0)
                {
                    mindist = distance;
                    closest = p;
                    if(s_ret != nullptr &&  s_retj != nullptr)
                    {
                        *s_ret = *s_retj;
                    }if(f_ret != nullptr)
                    {
                        *f_ret = m_FeatureList.at(j);
                    }
                }else if(distance < mindist)
                {
                    mindist = distance;
                    closest = p;
                    if(s_ret != nullptr &&  s_retj != nullptr)
                    {
                        *s_ret = *s_retj;
                    }if(f_ret != nullptr)
                    {
                        *f_ret = m_FeatureList.at(j);
                    }
                }
            }
            delete s_retj;
        }

        return closest;
    }

    LSMVector2 GetClosestVertex(LSMVector2 pos,Feature **f_ret,LSMShape ** s_ret = nullptr)
    {
        LSMVector2 closest = LSMVector2(1e31,1e31);
        float mindist = 1e31;
        if(s_ret != nullptr)
        {
            *s_ret = nullptr;
        }if(f_ret != nullptr)
        {
            *f_ret = nullptr;
        }


        for(int j = 0; j < m_FeatureList.length(); j++)
        {

            LSMShape ** s_retj = new (LSMShape *)();
            LSMVector2 p = m_FeatureList.at(j)->GetClosestVertex(pos,s_retj);
            float distance = LSMVector2(pos - p).length();
            if(std::isfinite(distance))
            {
                if(j == 0)
                {
                    mindist = distance;
                    closest = p;
                    if(s_ret != nullptr &&  s_retj != nullptr)
                    {
                        *s_ret = *s_retj;
                    }if(f_ret != nullptr)
                    {
                        *f_ret = m_FeatureList.at(j);
                    }
                }else if(distance < mindist)
                {
                    mindist = distance;
                    closest = p;
                    if(s_ret != nullptr &&  s_retj != nullptr)
                    {
                        *s_ret = *s_retj;
                    }if(f_ret != nullptr)
                    {
                        *f_ret = m_FeatureList.at(j);
                    }
                }
            }
            delete s_retj;
        }

        return closest;
    }
    inline void Destroy()
    {
        //create attribute fields
        for(int i = 0; i < m_AttributeList.length(); i++)
        {
            AttributeList * al = m_AttributeList.at(i);
            if(al != nullptr)
            {
                delete al;
            }
        }
        m_AttributeList.clear();

        for(int i =0; i < m_FeatureList.length(); i++)
        {
            Feature* f = m_FeatureList.at(i);
            if(f!= nullptr)
            {
                f->Destroy();
                delete f;
            }
        }
        m_FeatureList.clear();

    }

    inline QList<Feature *> GetFeaturesAt(LSMVector2 pos, double tolerence)
    {
        QList<Feature *> result;

        for(int i = 0; i < m_FeatureList.length(); i++)
        {
            Feature *f = m_FeatureList.at(i);

            if(f->GetBoundingBox().Contains(pos.x,pos.y,tolerence))
            {
                if(f->Contains(pos, tolerence))
                {
                    result.append(f);
                }
            }

        }

        return result;

    }

    inline void Flatten()
    {

        for(int i =0; i < GetFeatureCount(); i++)
        {

            Feature *f = GetFeature(i);

            for(int j = f->GetShapeCount()-1; j >0; j--)
            {
                LSMShape * s = f->GetShape(j);
                f->RemoveShape(s);
                Feature* fnew = new Feature();
                fnew->AddShape(s);
                int index = this->AddFeature(fnew);
                this->CopyFeatureAttributesFrom(i, index);
            }
        }


    }

    inline GLGeometryList * GetGLGeometryList()
    {
        struct GLGeometryList * gl_list = new GLGeometryList();
        //we will fil this struct with the geometry used by the display code for drawing with opengl
        //this means creating triangulations of points with a radius and polygons
        //borders of points and polygons and lines are given as line geometry

        for(int i =0; i < GetFeatureCount(); i++)
        {

            Feature *f = GetFeature(i);

            f->AddToGlGeometryList(gl_list,i);
        }

        return gl_list;

    }

    inline int GetAttributeIndex(QString attb_name)
    {
        int index = -1;

        if(attb_name.isEmpty())
        {
            return index;
        }
        int h = GetNumberOfAttributes();
        for(int i = 0; i < h; i++)
        {
            if(GetAttributeList(i)->m_Name.compare(attb_name)==0)
            {
                index = i;
                break;
            }
        }

        return index;
    }

    inline void SetGeometryLayerAttributes(GLGeometryList * geom, QString attb_name_pf,QString attb_name_pl,QString attb_name_ll,QString attb_name_pof,QString attb_name_pol)
    {
        //find attribute


        int index_pf = GetAttributeIndex(attb_name_pf);
        int index_pl = GetAttributeIndex(attb_name_pl);
        int index_ll = GetAttributeIndex(attb_name_ll);
        int index_pof = GetAttributeIndex(attb_name_pof);
        int index_pol = GetAttributeIndex(attb_name_pol);



        if(index_pf > -1)
        {
            for(int i = 0; i <geom->Vertex_Point_attr.size(); i++)
            {
                int findex = geom->Vertex_Point_index.at(i);
                QString val =GetAttributeValue(index_pf,findex);
                double vald = val.toDouble();
                geom->Vertex_Point_attr[i] = vald;
            }
        }

        for(int i = 0; i <geom->Polygon_Lines_attr.size(); i++)
        {
            int findex = geom->Polygon_Lines_index.at(i);
            QString val =GetAttributeValue(index_pol,findex);
            double vald = val.toDouble();
            geom->Polygon_Lines_attr[i] = vald;
        }

        for(int i = 0; i <geom->Polygon_Triangles_attr.size(); i++)
        {
            int findex = geom->Polygon_Triangles_index.at(i);
            QString val =GetAttributeValue(index_pof,findex);
            double vald = val.toDouble();
            geom->Polygon_Triangles_attr[i] = vald;
        }

        for(int i = 0; i <geom->Line_Lines_attr.size(); i++)
        {
            int findex = geom->Line_Lines_index.at(i);
            QString val =GetAttributeValue(index_ll,findex);
            double vald = val.toDouble();
            geom->Line_Lines_attr[i] = vald;
        }

        for(int i = 0; i <geom->Point_Lines_attr.size(); i++)
        {
            int findex = geom->Point_Lines_index.at(i);
            QString val =GetAttributeValue(index_pl,findex);
            double vald = val.toDouble();
            geom->Point_Lines_attr[i] = vald;
        }

        for(int i = 0; i <geom->Point_Triangles_attr.size(); i++)
        {
            int findex = geom->Point_Triangles_index.at(i);
            QString val =GetAttributeValue(index_pf,findex);
            double vald = val.toDouble();
            geom->Point_Triangles_attr[i] = vald;
        }


    }

    inline LSMInterval GetAttributeInterval(QString attb_name)
    {
        int index = GetAttributeIndex(attb_name);

        if(index < 0)
        {
            return LSMInterval(0.0,0.0);
        }
        AttributeList * l = GetAttributeList(index);
        return l->GetValueInterval();

    }

    inline QList<Feature *>  GetAttributeTableReturnRemovedFeatures(MatrixTable * m)
    {

        QList<Feature *> fret;


        std::vector<bool> found;
        found.reserve(m_FeatureList.size());
        for(int i = 0; i < m_FeatureList.size(); i++)
        {
            found.push_back(false);
        }
        for(int i = 0; i < m->GetNumberOfRows(); i++)
        {
            QString title = m->GetRowTitleQ(i);
            bool ok = false;
            int number = title.toInt(&ok);
            if(ok)
            {
                if(number > -1 && number < m_FeatureList.length())
                {
                    found.at(i) = true;
                }
            }
        }

        int n_out = 0;
        for(int i = m_FeatureList.size()-1; i > -1 ; i--)
        {
            if(!found.at(i))
            {
                n_out++;
                fret.append(m_FeatureList.at(i));
            }
        }
        std::cout << "removing features " << m_FeatureList.size() << " " << n_out << std::endl;

        return fret;
    }

    inline void SetAttributeTable(MatrixTable * m, bool do_remove = false)
    {

        for(int i = 0; i < m_AttributeList.length(); i++)
        {
            AttributeList *AL =m_AttributeList.at(i);
            delete AL;
        }
        m_AttributeList.clear();

        if(do_remove)
        {
            std::vector<bool> found;
            found.reserve(m_FeatureList.size());
            for(int i = 0; i < m_FeatureList.size(); i++)
            {
                found.push_back(false);
            }
            for(int i = 0; i < m->GetNumberOfRows(); i++)
            {
                QString title = m->GetRowTitleQ(i);
                bool ok = false;
                int number = title.toInt(&ok);
                if(ok)
                {
                    if(number > -1 && number < m_FeatureList.length())
                    {
                        found.at(i) = true;
                    }
                }
            }

            int n_out = 0;
            for(int i = m_FeatureList.size()-1; i >-1 ; i--)
            {
                if(!found.at(i))
                {
                    m_FeatureList.removeAt(i);
                    n_out++;
                }
            }
            std::cout << "removing features " << m_FeatureList.size() << " " << n_out << std::endl;


        }

        for(int i = 0; i < m->GetNumberOfCols(); i++)
        {
            QString name = m->GetColumnTitleQ(i);
            int type = m->GetColumnType(i);
            this->AddAttribute(name,type);

        }

        for(int i = 0; i < m->GetNumberOfCols(); i++)
        {
            QString name = m->GetColumnTitleQ(i);
            for(int j = 0; j < m->GetNumberOfRows(); j++)
            {
                if(j < GetFeatureCount())
                {
                    //set attribute value
                    SetFeatureAttribute(j,name,m->GetValueStringQ(j,i));
                }
            }
        }


    }

    inline MatrixTable * GetAttributeTable()
    {

        int h = GetFeatureCount();
        int w = m_AttributeList.length();

        MatrixTable * t = new MatrixTable();
        t->SetSize(h,w);

        for(int i = 0; i < h; i++)
        {
            t->SetRowTitle(i,QString::number(i+1));
        }

        for(int j = 0; j < w; j++)
        {
            t->SetColumnTitle(j,m_AttributeList.at(j)->m_Name);
        }

        for(int j = 0; j < w; j++)
        {
            t->SetColumnType(j,m_AttributeList.at(j)->m_type);
        }
        t->SetColumnTypesActive(true);
        t->SetRowTypesActive(false);
        for(int i = 0; i < h; i++)
        {
            for(int j = 0; j < w; j++)
            {
                t->SetValue(i,j,m_AttributeList.at(j)->GetValueAsString(i));
            }
        }

        return t;

    }

    inline MatrixTable * GetAttributeTable(QString name)
    {

        int h = GetFeatureCount();
        int w = m_AttributeList.length();

        int j = 0;
        bool found = false;
        for(j = 0; j < w; j++)
        {
            std::cout << " compare " << m_AttributeList.at(j)->m_Name.toStdString() << " " << name.toStdString() << std::endl;
            if(m_AttributeList.at(j)->m_Name.compare(name) == 0)
            {
                found = true;
                break;
            }
        }


        std::cout << "index of attribute " << name.toStdString() << " " << j << std::endl;

        if(found)
        {
            MatrixTable * t = new MatrixTable();
            t->SetSize(h,1);

            for(int i = 0; i < h; i++)
            {
                t->SetRowTitle(i,QString::number(i+1));
            }

            t->SetColumnTitle(0,m_AttributeList.at(j)->m_Name);

            for(int i = 0; i < h; i++)
            {
                //for(int j = 0; j < w; j++)
                {
                    t->SetValue(i,0,m_AttributeList.at(j)->GetValueAsString(i));
                }
            }

            return t;
        }else {
            MatrixTable * t = new MatrixTable();
            t->SetSize(h,1);
            t->SetColumnTitle(0,name);
            return t;
        }
    }



    inline bool Overlaps(std::vector<double> & x,std::vector<double> & y)
    {

        for(int i = 0; i < m_FeatureList.length(); i++)
        {
            if(m_FeatureList.at(i)->Overlaps(x,y))
            {
                return true;
            }
        }

        return false;
    }

    inline QList<Feature*> GetOverlappingFeatures(std::vector<double> & x,std::vector<double> & y)
    {
        QList<Feature*> l;
        for(int i = 0; i < m_FeatureList.length(); i++)
        {
            if(m_FeatureList.at(i)->Overlaps(x,y))
            {

             l.append(m_FeatureList.at(i));
            }

        }
        return l;
    }

    inline QList<std::pair<LSMShape*,Feature*>> GetOverlappingShapes(std::vector<double> & x,std::vector<double> & y)
    {
        QList<std::pair<LSMShape*,Feature*>> l;
        for(int i = 0; i < m_FeatureList.length(); i++)
        {
             l.append(m_FeatureList.at(i)->GetOverlappingShapes(x,y));

        }
        return l;
    }
    inline QList<std::tuple<double*,double*,LSMShape*,Feature*>> GetOverlappingVertices(std::vector<double> & x,std::vector<double> & y)
    {
        QList<std::tuple<double*,double*,LSMShape*,Feature*>> l;
        for(int i = 0; i < m_FeatureList.length(); i++)
        {
             l.append(m_FeatureList.at(i)->GetOverlappingVertices(x,y));

        }
        return l;
    }





    //Angelscript related functions
    public:

        int            AS_refcount          = 1;
        void           AS_AddRef            ();
        void           AS_ReleaseRef        ();
        ShapeLayer* AS_Assign            (ShapeLayer*);


        //feature stuff

        Feature * AS_GetFeature(int index);
        int AS_GetFeatureCount();
        int AS_GetShapeCount();
        int AS_GetAtrributeCount();
        MatrixTable *AS_GetAttributes();

        void AS_AddFeature(Feature *f);
        void AS_RemoveFeature(int);
        QString AS_GetAttributeName(int i);
        bool AS_IsAttributeNumber(int i);
        bool AS_IsAttributeText(int i);
        void AS_SetAttributeAsNumber(int i);
        void AS_SetAttributeAsText(int i);

        void AS_SetFeatureAttribute(int i,QString name, QString val);
        void AS_SetFeatureAttribute(int i,QString name, double val);
        void AS_AddAttributeNumber(QString name, double default_val);
        void AS_AddAttributeString(QString name, QString default_val);

        LSMVector2 AS_GetCentroid();
        bool AS_Contains(LSMVector2 v);
        double AS_GetLength();
        double AS_GetArea();
        void AS_Move(LSMVector2 m);
        LSMVector2 AS_GetClosestVertex(LSMVector2 v);
        LSMVector2 AS_GetClosestEdgePoint(LSMVector2 v);
        BoundingBox AS_GetBoundingBox();
        QString AS_GetAttributeValueAt( QString name, LSMVector2 v);


};

inline QString ShapeLayer::AS_GetAttributeValueAt(QString name, LSMVector2 v)
{
    int  j = GetAttributeIndex(name);

    if(j < 0)
    {
        LISEMS_WARNING("Attribute not found");
        return "";
    }

    for(int i = 0; i < m_FeatureList.length(); i++)
    {
        if(m_FeatureList.at(i)->Contains(v,0.0))
        {

            return GetAttributeValue(j,i);
        }
    }
    return "";
}

inline LSMVector2 ShapeLayer::AS_GetClosestEdgePoint(LSMVector2 v)
{
    Feature ** f_retj = new (Feature *)();
    LSMShape ** s_retj  = new (LSMShape *)();

    LSMVector2 closest =  GetClosestEdgeLocation(v,f_retj,s_retj);
    delete f_retj;
    delete s_retj;
    return closest;
}

inline LSMVector2 ShapeLayer::AS_GetClosestVertex(LSMVector2 v)
{
    Feature ** f_retj = new (Feature *)();
    LSMShape ** s_retj  = new (LSMShape *)();

    LSMVector2 closest =  GetClosestVertex(v,f_retj,s_retj);
    delete f_retj;
    delete s_retj;
    return closest;
}
inline BoundingBox ShapeLayer::AS_GetBoundingBox()
{
    return this->GetBoundingBox();
}
inline void ShapeLayer::AS_Move(LSMVector2 m)
{
    for(int i = 0; i < m_FeatureList.length(); i++)
    {
        m_FeatureList.at(i)->Move(m);
    }
}

inline double ShapeLayer::AS_GetArea()
{
    double area_t = 0.0;
    for(int i = 0; i < m_FeatureList.length(); i++)
    {
        area_t += m_FeatureList.at(i)->Area();
    }
    return area_t;
}

inline double ShapeLayer::AS_GetLength()
{
    double area_t = 0.0;
    for(int i = 0; i < m_FeatureList.length(); i++)
    {
        area_t += m_FeatureList.at(i)->Length();
    }
    return area_t;
}

inline int ShapeLayer::AS_GetAtrributeCount()
{
    return m_AttributeList.length();

}
inline bool ShapeLayer::AS_Contains(LSMVector2 pos)
{
    for(int i = 0; i < m_FeatureList.length(); i++)
    {
        if(m_FeatureList.at(i)->Contains(pos,0.0))
        {
            return true;
        }
    }
    return false;
}

inline LSMVector2 ShapeLayer::AS_GetCentroid()
{
    double area_t = 0.0;
    LSMVector2 c_t = LSMVector2(0.0,0.0);
    for(int i = 0; i < m_FeatureList.length(); i++)
    {
        LSMVector2 c = m_FeatureList.at(i)->GetCentroid();
        double area = m_FeatureList.at(i)->Area();
        c_t = c_t + c* area;
        area_t += area;
    }

    if(area_t > 0.0)
    {
        c_t = c_t/area_t;
    }
    return c_t;
}

inline void ShapeLayer::AS_SetFeatureAttribute(int i,QString name, QString val)
{
    bool founda = false;
    AttributeList * AL;
    for(int i = m_AttributeList.length()-1; i > -1; i--)
    {
        AL =m_AttributeList.at(i);
        if(name.compare(m_AttributeList.at(i)->m_Name) == 0)
        {
            founda = true;
            break;
        }
    }

    if(!founda)
    {
        LISEMS_WARNING("Can not alter attribute, name not found");
    }

    if(i > -1 && i < m_FeatureList.length())
    {
        this->SetFeatureAttribute(i,name,val);
    }else {
        LISEMS_WARNING("Can not alter attribute, index out of bounds");
    }
}
inline void ShapeLayer::AS_SetFeatureAttribute(int i,QString name, double val)
{
    bool founda = false;
    AttributeList * AL;
    for(int i = m_AttributeList.length()-1; i > -1; i--)
    {
        AL =m_AttributeList.at(i);
        if(name.compare(m_AttributeList.at(i)->m_Name) == 0)
        {
            founda = true;
            break;
        }
    }

    if(!founda)
    {
        LISEMS_WARNING("Can not alter attribute, name not found");
    }

    if(i > -1 && i < m_FeatureList.length())
    {
        this->SetFeatureAttribute(i,name,val);
    }else {
        LISEMS_WARNING("Can not alter attribute, index out of bounds");
    }
}
inline void ShapeLayer::AS_AddAttributeNumber(QString name, double default_val)
{
    bool founda = false;
    AttributeList * AL;
    for(int i = m_AttributeList.length()-1; i > -1; i--)
    {
        AL =m_AttributeList.at(i);
        if(name.compare(m_AttributeList.at(i)->m_Name) == 0)
        {
            founda = true;
            break;
        }
    }

    if(founda)
    {
        LISEMS_WARNING("Can not add attribute, name already found");
    }


    this->AddAttribute(name,ATTRIBUTE_TYPE_FLOAT64);

    for(int i = 0; i < m_FeatureList.length(); i++)
    {
        SetFeatureAttribute(i,name,default_val);
    }
}
inline void ShapeLayer::AS_AddAttributeString(QString name, QString default_val)
{
    bool founda = false;
    AttributeList * AL;
    for(int i = m_AttributeList.length()-1; i > -1; i--)
    {
        AL =m_AttributeList.at(i);
        if(name.compare(m_AttributeList.at(i)->m_Name) == 0)
        {
            founda = true;
            break;
        }
    }

    if(founda)
    {
        LISEMS_WARNING("Can not add attribute, name already found");
    }

    this->AddAttribute(name,ATTRIBUTE_TYPE_STRING);
    for(int i = 0; i < m_FeatureList.length(); i++)
    {
        SetFeatureAttribute(i,name,default_val);
    }
}


inline void ShapeLayer::AS_SetAttributeAsText(int i)
{
    if(i > -1 && i < m_AttributeList.length())
    {
        if(!m_AttributeList.at(i)->isNumberType())
        {
            AttributeList * l = m_AttributeList.at(i);
            QList<QString> vals;
            for(int k = 0; k < m_FeatureList.length(); k++)
            {
                vals.push_back(l->GetValueAsString(k));
            }

            l->Clear();
            l->m_type = ATTRIBUTE_TYPE_STRING;
            for(int k = 0; k < m_FeatureList.length(); k++)
            {
                l->m_ListString.push_back(vals.at(k));
            }
        }
    }else {
        LISEMS_WARNING("Can not get attribute table from shape layer, index out of bounds");
    }
    return;

}


inline void ShapeLayer::AS_SetAttributeAsNumber(int i)
{
    if(i > -1 && i < m_AttributeList.length())
    {
        if(!m_AttributeList.at(i)->isNumberType())
        {
            AttributeList * l = m_AttributeList.at(i);
            QList<QString> vals;
            for(int k = 0; k < m_FeatureList.length(); k++)
            {
                vals.push_back(l->GetValueAsString(k));
            }

            l->Clear();
            l->m_type = ATTRIBUTE_TYPE_FLOAT64;
            for(int k = 0; k < m_FeatureList.length(); k++)
            {
                l->m_ListFloat64.push_back(vals.at(k).toDouble());
            }
        }
    }else {
        LISEMS_WARNING("Can not get attribute table from shape layer, index out of bounds");
    }
    return;

}

inline bool ShapeLayer::AS_IsAttributeText(int i)
{
    if(i > -1 && i < m_AttributeList.length())
    {
        return !m_AttributeList.at(i)->isNumberType();

    }else {
        LISEMS_WARNING("Can not get attribute table from shape layer, index out of bounds");
    }
    return false;
}


inline bool ShapeLayer::AS_IsAttributeNumber(int i)
{
    if(i > -1 && i < m_AttributeList.length())
    {
        return m_AttributeList.at(i)->isNumberType();

    }else {
        LISEMS_WARNING("Can not get attribute table from shape layer, index out of bounds");
    }
    return false;
}


inline QString ShapeLayer::AS_GetAttributeName(int i)
{
    if(i > -1 && i < m_AttributeList.length())
    {
        return m_AttributeList.at(i)->m_Name;

    }else {
        LISEMS_WARNING("Can not get attribute table from shape layer, index out of bounds");
    }
    return "";
}

inline void ShapeLayer::AS_AddFeature(Feature * f)
{
    AddFeature(f);
}
inline void ShapeLayer::AS_RemoveFeature(int i)
{
    if(i > -1 && i < m_FeatureList.length())
    {
        RemoveFeature(i);
    }else {
        LISEMS_WARNING("Can not remove object from shape layer, index out of bounds");
    }


}


inline Feature * ShapeLayer::AS_GetFeature(int index)
{
    if(index < 0 || index >= m_FeatureList.size())
    {
        LISEMS_ERROR("ShapeLayer::GetFeature(int) out of bounds, index does not represent a valid layer");
        throw 1;
    }
    return m_FeatureList.at(index);
}
inline int ShapeLayer::AS_GetFeatureCount()
{

    return m_FeatureList.size();

}
inline int ShapeLayer::AS_GetShapeCount()
{
    return GetShapeCount();
}

inline MatrixTable *ShapeLayer::AS_GetAttributes()
{
    return GetAttributeTable();
}


//Angelscript related functionality

inline void ShapeLayer::AS_AddRef()
{
    AS_refcount = AS_refcount + 1;

}

inline void ShapeLayer::AS_ReleaseRef()
{
    AS_refcount = AS_refcount - 1;
    if(AS_refcount == 0)
    {
        Destroy();
        delete this;
    }

}

inline ShapeLayer* ShapeLayer::AS_Assign(ShapeLayer* sh)
{
    this->CopyFrom(sh);

    return this;
}

inline ShapeLayer* AS_ShapeLayerFactory()
{
    return new ShapeLayer();

}

inline ShapeLayer* FromGDALLayer(OGRLayer * d)
{
    ShapeLayer * l = new ShapeLayer();
    const char * name = d->GetName();l->SetName(d->GetName());
    if(name != NULL)
    {
        l->SetName(name);
    }

    OGRwkbGeometryType layertype = wkbFlatten(d->GetGeomType());

    std::cout << "Layertype " <<  layertype << std::endl;

    if(layertype == wkbUnknown)
    {
        l->SetType(LISEM_SHAPE_UNKNOWN);
    }else if(layertype == wkbPoint)
    {
        l->SetType(LISEM_SHAPE_POINT);
    }else if(layertype == wkbLineString)
    {
        l->SetType(LISEM_SHAPE_LINE);
    }else if(layertype == wkbPolygon)
    {
        l->SetType(LISEM_SHAPE_POLYGON);
    }else if(layertype == wkbMultiPoint)
    {
        l->SetType(LISEM_SHAPE_MULTIPOINT);
    }else if(layertype == wkbMultiLineString)
    {
        l->SetType(LISEM_SHAPE_MULTILINE);
    }else if(layertype == wkbMultiPolygon)
    {
        l->SetType(LISEM_SHAPE_MULTIPOLYGON);
    }else {
        LISEM_ERROR("Unsupported layer type (shape not implemented, can not be loaded)")
        return l;
    }

    OGREnvelope Envelope;
    int error = d->GetExtent(&Envelope,true);

    OGRFeatureDefn *poFDefn = d->GetLayerDefn();

    std::cout << "field count " << poFDefn->GetFieldCount() << std::endl;

    //get attribute names and types
    for( int iField = 0; iField < poFDefn->GetFieldCount(); iField++ )
    {
        OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn( iField );

        if(!QString(poFieldDefn->GetNameRef()).startsWith("OBJECTID"))
        {
            switch( poFieldDefn->GetType() )
            {
                case OFTInteger:
                    l->AddAttribute(poFieldDefn->GetNameRef(),ATTRIBUTE_TYPE_INT32);
                    break;
                case OFTInteger64:
                    l->AddAttribute(poFieldDefn->GetNameRef(),ATTRIBUTE_TYPE_INT64);
                    break;
                case OFTReal:
                    l->AddAttribute(poFieldDefn->GetNameRef(),ATTRIBUTE_TYPE_FLOAT64);
                    break;
                case OFTString:
                    l->AddAttribute(poFieldDefn->GetNameRef(),ATTRIBUTE_TYPE_STRING);
                    break;
                default:
                    l->AddAttribute(poFieldDefn->GetNameRef(),ATTRIBUTE_TYPE_UNKNOWN);
                    break;
            }
        }
    }

    int count = d->GetFeatureCount(true);

    std::cout << "feature count " << count << std::endl;
    d->ResetReading();
    OGRFeature *poFeature;
    for(int j = 0; j < count; j++)
    {
        poFeature = d->GetNextFeature();


        Feature *f = FromGDALFeature(poFeature);
        int indexf = l->AddFeature(f);

        //for each feature we must also get the attributes
        for( int iField = 0; iField < poFDefn->GetFieldCount(); iField++ )
        {
            OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn( iField );

            if(!QString(poFieldDefn->GetNameRef()).startsWith("OBJECTID"))
            {
                switch( poFieldDefn->GetType() )
                {
                    case OFTInteger:
                        l->SetFeatureAttribute(indexf, poFieldDefn->GetNameRef(), (int64_t) poFeature->GetFieldAsInteger( iField ) );
                        break;
                    case OFTInteger64:
                        l->SetFeatureAttribute(indexf, poFieldDefn->GetNameRef(), (int64_t) poFeature->GetFieldAsInteger64( iField ) );
                        break;
                    case OFTReal:
                        l->SetFeatureAttribute(indexf, poFieldDefn->GetNameRef(), (double) poFeature->GetFieldAsDouble(iField) );
                        break;
                    case OFTString:
                        l->SetFeatureAttribute(indexf, poFieldDefn->GetNameRef(), QString(poFeature->GetFieldAsString(iField)) );
                        break;
                    default:
                        l->SetFeatureAttribute(indexf, poFieldDefn->GetNameRef(), QString(poFeature->GetFieldAsString(iField) ));
                        break;
                }
            }
        }


    }

    return l;
}

inline OGRLayer * ToGDALLayer(GDALDataset * to, ShapeLayer* layer, GeoProjection p)
{
    //wkbUnknown = 0          /** unknown non-standard */
    //wkbPoint = 1,           /**< 0-dimensional geometric object, standard WKB */
    //wkbLineString = 2,      /**< 1-dimensional geometric object with linear
    //                         *   interpolation between Points, standard WKB */
    //wkbPolygon = 3,         /**< planar 2-dimensional geometric object defined
    //                         *   by 1 exterior boundary and 0 or more interior
    //                         *   boundaries, standard WKB */
    //wkbMultiPoint = 4,      /**< GeometryCollection of Points, standard WKB */
    //wkbMultiLineString = 5, /**< GeometryCollection of LineStrings, standard WKB */
    //wkbMultiPolygon = 6,    /**< GeometryCollection of Polygons, standard WKB */

    int layertype = layer->GetType();
    OGRwkbGeometryType ogrtype;

    if(layertype == LISEM_SHAPE_UNKNOWN)
    {
        ogrtype = wkbUnknown;
    }else if(layertype == LISEM_SHAPE_POINT)
    {
        ogrtype = wkbPoint;
    }else if(layertype ==LISEM_SHAPE_LINE )
    {
        ogrtype =wkbLineString;
    }else if(layertype == LISEM_SHAPE_POLYGON)
    {
        ogrtype = wkbPolygon;
    }else if(layertype == LISEM_SHAPE_MULTIPOINT)
    {
        ogrtype = wkbMultiPoint;
    }else if(layertype == LISEM_SHAPE_MULTILINE)
    {
        ogrtype = wkbMultiLineString;
    }else if(layertype == LISEM_SHAPE_MULTIPOLYGON)
    {
        ogrtype = wkbMultiPolygon;
    }else {

        LISEM_ERROR("Could not recognize layer type")
        return nullptr;
    }

    QString name = layer->GetName();
    if(name.isEmpty())
    {
        name = QString::number(to->GetLayerCount());
    }

    OGRLayer * poLayer;

    if(p.IsGeneric())
    {
        poLayer = to->CreateLayer( name.toStdString().c_str(),nullptr, ogrtype, NULL );
    }else
    {
        OGRSpatialReference * ref = new OGRSpatialReference(p.ToGDALRef());
        poLayer = to->CreateLayer( name.toStdString().c_str(),ref, ogrtype, NULL );
    }

    if( poLayer == NULL )
    {
        LISEM_ERROR("Could not create layer in gdal dataset");
        return nullptr;
    }

    //create attribute fields
    for(int i = 0; i < layer->GetNumberOfAttributes(); i++)
    {
        AttributeList * al = layer->GetAttributeList(i);
        QString alname = al->m_Name;

        if(alname.length() > 10)
        {
            LISEM_ERROR("Attribute: " + alname +" has a name longer then 10 character, truncation taking place");
            alname.left(10);
        }


        if(al->m_type == ATTRIBUTE_TYPE_STRING)
        {
            OGRFieldDefn oField( alname.toStdString().c_str(), OFTString );
            oField.SetWidth(LISEM_FIELD_PRECISION*2);
            if( poLayer->CreateField( &oField ) != OGRERR_NONE )
            {
                LISEM_ERROR("Could not add attribute of to gdal layer: " + alname);
            }
        }else if(al->m_type == ATTRIBUTE_TYPE_BOOL)
        {
            OGRFieldDefn oField( alname.toStdString().c_str(), OFTInteger);
            oField.SetWidth(LISEM_FIELD_PRECISION);
            oField.SetPrecision(LISEM_FIELD_PRECISION);
            if( poLayer->CreateField( &oField ) != OGRERR_NONE )
            {
                LISEM_ERROR("Could not add attribute of to gdal layer: " + alname);
            }
        }else if(al->m_type == ATTRIBUTE_TYPE_INT32)
        {
            OGRFieldDefn oField( alname.toStdString().c_str(), OFTInteger);
            oField.SetWidth(LISEM_FIELD_PRECISION);
            oField.SetPrecision(LISEM_FIELD_PRECISION);
            if( poLayer->CreateField( &oField ) != OGRERR_NONE )
            {
                LISEM_ERROR("Could not add attribute of to gdal layer: " + alname);
            }
        }else if(al->m_type == ATTRIBUTE_TYPE_INT64)
        {
            OGRFieldDefn oField( alname.toStdString().c_str(), OFTInteger64 );
            oField.SetWidth(LISEM_FIELD_PRECISION);
            oField.SetPrecision(LISEM_FIELD_PRECISION);
            if( poLayer->CreateField( &oField ) != OGRERR_NONE )
            {
                LISEM_ERROR("Could not add attribute of to gdal layer: " + alname);
            }
        }else if(al->m_type == ATTRIBUTE_TYPE_FLOAT64)
        {
            OGRFieldDefn oField( alname.toStdString().c_str(), OFTReal );
            oField.SetWidth(LISEM_FIELD_PRECISION);
            oField.SetPrecision(LISEM_FIELD_PRECISION);
            if( poLayer->CreateField( &oField ) != OGRERR_NONE )
            {
                LISEM_ERROR("Could not add attribute of to gdal layer: " + alname);
            }
        }else if(al->m_type == ATTRIBUTE_TYPE_UNKNOWN)
        {
            OGRFieldDefn oField( alname.toStdString().c_str(), OFTString );
            oField.SetWidth(LISEM_FIELD_PRECISION*2);
            if( poLayer->CreateField( &oField ) != OGRERR_NONE )
            {
                LISEM_ERROR("Could not add attribute of unknown type to gdal layer");
            }
        }
    }

    for(int i =0; i < layer->GetFeatureCount(); i++)
    {
        //add feature
        OGRFeature *poFeature;
        poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );

        for(int j = 0; j < layer->GetNumberOfAttributes(); j++)
        {
            AttributeList * al = layer->GetAttributeList(j);
            QString alname = al->m_Name;
            if(alname.length() > 10)
            {
                alname.left(10);
            }

            //set feature attribute values
            if(al->m_type == ATTRIBUTE_TYPE_STRING)
            {
                if(al->m_ListString.length() > i)
                {
                    poFeature->SetField( alname.toStdString().c_str(),al->m_ListString.at(i).toStdString().c_str());
                }
            }else if(al->m_type == ATTRIBUTE_TYPE_BOOL)
            {
                if(al->m_ListBool.length() > i)
                {
                    poFeature->SetField( alname.toStdString().c_str(),al->m_ListBool.at(i)? 1:0);
                }
            }else if(al->m_type == ATTRIBUTE_TYPE_INT32)
            {
                if(al->m_ListInt32.length() > i)
                {
                    poFeature->SetField( alname.toStdString().c_str(),al->m_ListInt32.at(i));
                }
            }else if(al->m_type == ATTRIBUTE_TYPE_INT64)
            {
                if(al->m_ListInt64.length() > i)
                {
                    poFeature->SetField( alname.toStdString().c_str(),(int32_t)al->m_ListInt64.at(i));
                }
            }else if(al->m_type == ATTRIBUTE_TYPE_FLOAT64)
            {
                if(al->m_ListFloat64.length() > i)
                {
                    poFeature->SetField( alname.toStdString().c_str(),(double)al->m_ListFloat64.at(i));
                }
            }else if(al->m_type == ATTRIBUTE_TYPE_UNKNOWN)
            {
                if(al->m_ListString.length() > i)
                {
                    poFeature->SetField( alname.toStdString().c_str(),al->m_ListString.at(i).toStdString().c_str());
                }
            }
        }

        ToGDALFeature(poFeature,layer->GetFeature(i));


        if(poLayer->CreateFeature(poFeature)!= OGRERR_NONE)
        {
            LISEM_ERROR("Could not add feature");
        };

        OGRFeature::DestroyFeature( poFeature );
    }

    return poLayer;


}

#endif // SHAPELAYER_H
