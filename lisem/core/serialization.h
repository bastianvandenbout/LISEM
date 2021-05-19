#ifndef SERIALIZATION_H
#define SERIALIZATION_H
#include "QJsonValue" 
#include "QJsonObject"
#include "QJsonArray"
#include "QList"
//helperFunctions

namespace LSMSerialize
{

    class LSMStringable
    {
    public:
        virtual QString toString() const = 0;
        virtual void fromString(QString) = 0;

    };



    static inline void FromString(QJsonObject * obj,  QString name, LSMStringable &x)
    {
        obj->insert(name,x.toString());
    }
    static inline bool ToString(QJsonObject * obj,  QString name, LSMStringable &x)
    {
        QJsonValue v = (*obj)[name];
        if(!v.isUndefined())
        {
            x.fromString(v.toString());
            return true;
        }else {
            return false;
        }

    }

    template<typename A>
    static inline void FromString(QJsonObject * obj,  QString name, A &x)
    {
        obj->insert(name,x.toString());
    }
    template<typename A>
    static inline bool ToString(QJsonObject * obj,  QString name, A &x)
    {
        QJsonValue v = (*obj)[name];
        if(!v.isUndefined())
        {
            x.fromString(v.toString());
            return true;
        }else {
            return false;
        }
    }

    static inline void FromBool(QJsonObject * obj,  QString name, bool x)
    {
        obj->insert(name,x);
    }
    static inline bool ToBool(QJsonObject * obj,  QString name, bool &x, bool defaultval = false)
    {
        QJsonValue v = (*obj)[name];
        bool ret;
        if(!v.isUndefined())
        {
            ret = v.toBool(defaultval);

        }else {
            ret = defaultval;
        }
        x = ret;
        return true;
    }

    static inline void FromString(QJsonObject * obj,  QString name, QString x)
    {
        obj->insert(name,x);
    }
    static inline bool ToString(QJsonObject * obj,  QString name, QString &x, QString defaultval ="")
    {
        QJsonValue v = (*obj)[name];
        QString ret;
        if(!v.isUndefined())
        {
            ret = v.toString(defaultval);

        }else {
            ret = defaultval;
        }
        x = ret;
        return true;
    }

    static inline void FromFloat(QJsonObject * obj,  QString name, float x)
    {
        obj->insert(name,x);
    }
    static inline void ToFloat(QJsonObject * obj,  QString name, float &x, float defaultval =0)
    {
        QJsonValue v = (*obj)[name];
        double ret;
        if(!v.isUndefined())
        {
            ret = v.toDouble(defaultval);
        }else {
            ret = defaultval;
        }
        x = ret;
    }

    static inline void FromDouble(QJsonObject * obj,  QString name, double x)
    {
        obj->insert(name,x);
    }
    static inline void ToDouble(QJsonObject * obj,  QString name, double &x, double defaultval =0)
    {
        QJsonValue v = (*obj)[name];
        double ret;
        if(!v.isUndefined())
        {
            ret = v.toDouble(defaultval);
        }else {
            ret = defaultval;
        }
        x = ret;
    }

    static inline void FromInt(QJsonObject * obj,  QString name, int x)
    {
        obj->insert(name,x);
    }
    static inline bool ToInt(QJsonObject * obj,  QString name, int &x, int defaultval = 0)
    {
        QJsonValue v = (*obj)[name];
        int ret;
        if(!v.isUndefined())
        {
            ret = v.toInt(defaultval);
        }else {
            ret = defaultval;
        }
        x = ret;
        return true;
    }

    template<typename A>
    static inline void FromQListString(QJsonObject * obj,  QString name, QList<A> l)
    {
        for(int i = 0; i < l.length(); i++)
        {
            obj->insert(name+"_internalarray_"+QString::number(i),l.at(i).toString());

        }

    }

    template<typename A>
    static inline bool ToQListString(QJsonObject * obj,  QString name,  QList<A> &l)
    {
        l.clear();
        int i = 0;
        while(true)
        {
            QString s;
            if(obj->contains(name + "_internalarray_" + QString::number(i)))
            {
                FromString(obj,name + "_internalarray_" + QString::number(i),s);
                A x;
                l.append(x.fromString(s));
            }else {
                break;
            }


            i++;

        }

        if(i == 0)
        {
            return false;
        }
        return true;

    }


    static inline void FromQListString(QJsonObject * obj,  QString name, QList<QString> l)
    {
        for(int i = 0; i < l.length(); i++)
        {
            obj->insert(name+"_internalarray_"+QString::number(i),l.at(i));

        }

    }
    static inline bool ToQListString(QJsonObject * obj,  QString name,  QList<QString> &l)
    {
        l.clear();
        int i = 0;
        while(true)
        {
            QString s;
            if(obj->contains(name + "_internalarray_" + QString::number(i)))
            {
                ToString(obj,name + "_internalarray_" + QString::number(i),s);

                l.append(s);
            }else {
                break;
            }


            i++;

        }

        if(i == 0)
        {
            return false;
        }
        return true;

    }



    static inline void FromQListListString(QJsonObject * obj,  QString name, QList<QList<QString>*> l)
    {
        for(int i = 0; i < l.length(); i++)
        {
            QList<QString> l2;
            l2 = *l.at(i);
            FromQListString(obj,name+ "_internalarray_"+QString::number(i),l2);
        }
    }
    static inline bool ToQListListString(QJsonObject * obj, QString name, QList<QList<QString>*> &l)
    {
        l.clear();
        int i = 0;
        while(true)
        {
            QList<QString> *l2 = new QList<QString>();
            if(obj->contains(QString("name")+"_internalarray_"+QString::number(i)))
            {
                    ToQListString(obj,QString("name")+"_internalarray_"+QString::number(i),*l2);
            }else {
                break;
            }
            l.append(l2);
            i++;
        }

        if(i == 0)
        {
            return false;
        }
        return true;
    }


    static inline void FromQListDouble(QJsonObject * obj,  QString name, QList<double> l)
    {
        QList<QString> l2;
        for(int i = 0; i < l.length(); i++)
        {
            l2.append(QString::number(l.at(i)));
        }
        FromQListString(obj,name,l2);
    }
    static inline bool ToQListDouble(QJsonObject * obj,  QString name, QList<double> &l)
    {
        QList<QString> l2;
        bool has = ToQListString(obj,name,l2);
        l.clear();
        for(int i = 0; i < l2.length(); i++)
        {
            l.append(l2.at(i).toDouble());
        }

        return has;
    }

    static inline void FromQListFloat(QJsonObject * obj,  QString name, QList<float> l)
    {
        QList<QString> l2;
        for(int i = 0; i < l.length(); i++)
        {
            l2.append(QString::number(l.at(i)));
        }
        FromQListString(obj,name,l2);
    }
    static inline bool ToQListFloat(QJsonObject * obj,  QString name, QList<float> &l)
    {
        QList<QString> l2;
        bool has = ToQListString(obj,name,l2);
        l.clear();
        for(int i = 0; i < l2.length(); i++)
        {
            l.append(l2.at(i).toDouble());
        }

        return has;
    }

    static inline void FromQListInt(QJsonObject * obj,  QString name, QList<int> l)
    {
        QList<QString> l2;
        for(int i = 0; i < l.length(); i++)
        {
            l2.append(QString::number(l.at(i)));
        }
        FromQListString(obj,name,l2);
    }
    static inline bool ToQListInt(QJsonObject * obj,  QString name, QList<int>&l)
    {
        QList<QString> l2;
        bool has = ToQListString(obj,name,l2);
        l.clear();
        for(int i = 0; i < l2.length(); i++)
        {
            l.append(l2.at(i).toInt());
        }

        return has;
    }

    static inline void FromQListBool(QJsonObject * obj,  QString name, QList<bool> l)
    {
        QList<QString> l2;
        for(int i = 0; i < l.length(); i++)
        {
            l2.append(QString::number(l.at(i)?1:0));
        }
        FromQListString(obj,name,l2);
    }
    static inline bool ToQListBool(QJsonObject * obj,  QString name, QList<bool> &l)
    {
        QList<QString> l2;
        bool has = ToQListString(obj,name,l2);
        l.clear();
        for(int i = 0; i < l2.length(); i++)
        {
            l.append(l2.at(i).toInt() == 1);
        }

        return has;

    }


    static inline void From(QJsonObject * obj,QString name,int x)
    {
        FromInt(obj,name,x);
    }

    static inline void From(QJsonObject * obj,QString name,float x)
    {
        FromFloat(obj,name,x);
    }

    static inline void From(QJsonObject * obj,QString name,double x)
    {
        FromDouble(obj,name,x);
    }

    static inline void From(QJsonObject * obj,QString name,bool x)
    {
        FromBool(obj,name,x);
    }

    static inline void From(QJsonObject * obj,QString name,QString x)
    {
        FromString(obj,name,x);
    }


    template <typename A>
    static inline void From(QJsonObject * obj,QString name,A x)
    {
        FromString(obj,name,x);
    }


    static inline void From(QJsonObject * obj,QString name,QList<int> x)
    {
        FromQListInt(obj,name,x);
    }

    static inline void From(QJsonObject * obj,QString name,QList<float> x)
    {
        FromQListFloat(obj,name,x);
    }

    static inline void From(QJsonObject * obj,QString name,QList<double> x)
    {
        FromQListDouble(obj,name,x);
    }

    static inline void From(QJsonObject * obj,QString name,QList<bool> x)
    {
        FromQListBool(obj,name,x);
    }

    static inline void From(QJsonObject * obj,QString name,QList<QString> x)
    {
        FromQListString(obj,name,x);
    }

    template <typename A>
    static inline void From(QJsonObject * obj,QString name,QList<A> x)
    {
        FromQListString(obj,name,x);
    }

    static inline void To(QJsonObject * obj,QString name,int &x)
    {
        ToInt(obj,name,x);
    }

    static inline void To(QJsonObject * obj,QString name,float &x)
    {
        ToFloat(obj,name,x);
    }

    static inline void To(QJsonObject * obj,QString name,double &x)
    {
        ToDouble(obj,name,x);
    }

    static inline void To(QJsonObject * obj,QString name,bool &x)
    {
        ToBool(obj,name,x);
    }

    static inline void To(QJsonObject * obj,QString name,QString &x)
    {
        ToString(obj,name,x);
    }

    template <typename A>
    static inline void To(QJsonObject * obj,QString name,LSMStringable &x)
    {
        ToString(obj,name,x);
    }

    template <typename A>
    static inline void To(QJsonObject * obj,QString name,A &x)
    {
        ToString<A>(obj,name,x);
    }


    static inline void To(QJsonObject * obj,QString name,QList<int> &x)
    {
        ToQListInt(obj,name,x);
    }

    static inline void To(QJsonObject * obj,QString name,QList<float> &x)
    {
        ToQListFloat(obj,name,x);
    }

    static inline void To(QJsonObject * obj,QString name,QList<double> &x)
    {
        ToQListDouble(obj,name,x);
    }

    static inline void To(QJsonObject * obj,QString name,QList<bool> &x)
    {
        ToQListBool(obj,name,x);
    }

    static inline void To(QJsonObject * obj,QString name,QList<QString> &x)
    {
        ToQListString(obj,name,x);
    }

    template<typename A>
    static inline void FromG(QJsonObject * obj,QString name, A &x)
    {
        From<A>(obj,name,x);
    }

    template<typename A>
    static inline void ToG(QJsonObject * obj,QString name, A &x)
    {
        To<A>(obj,name,x);
    }


    template<typename A>
    static inline void FromObject(QJsonObject * obj,QString name, A x)
    {
        QJsonObject obj2;
        x.Save(&obj2);
        obj->insert(name,obj2);
    }

    template<typename A>
    static inline void ToObject(QJsonObject * obj,QString name, A &x)
    {
        QJsonValue obj2 = (*obj)[name];
        if(obj2.isObject())
        {
            QJsonObject obj3 = obj2.toObject();
            x.Restore(&obj3);
        }

    }


}
#endif // SERIALIZATION_H
