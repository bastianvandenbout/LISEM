#pragma once
#include "QString"
#include "QMap"
#include "QMutex"
#include "QImage"
#include "color/colorf.h"
#include "linear/lsm_vector3.h"
#include "QFileInfo"

typedef struct LayerParameters
{

public:

    inline LayerParameters()
    {

    }
    inline ~LayerParameters()
    {

    }

    static const int LISEM_LAYERPARAMETER_TYPE_BOOL = 0; //bool
    static const int LISEM_LAYERPARAMETER_TYPE_COUNT = 1; //int
    static const int LISEM_LAYERPARAMETER_TYPE_NUMBER = 2; //double
    static const int LISEM_LAYERPARAMETER_TYPE_COLOR = 3; //SPHColor (4 x float)
    static const int LISEM_LAYERPARAMETER_TYPE_VEC3 = 4; //SPHVec3 (3 x float)
    static const int LISEM_LAYERPARAMETER_TYPE_TEXTURE = 5; //QImage
    static const int LISEM_LAYERPARAMETER_TYPE_3DOBJECT = 6; //OpenGLCLObject
    static const int LISEM_LAYERPARAMETER_TYPE_STRING = 7; //OpenGLCLObject
    QList<QString> ParameterNames;
    QMap<QString, QString> ParameterDefaultValues;
    QMap<QString, QString> ParameterValue;
    QMap<QString, void*> ParameterMemory;
    QMap<QString, void*> ParameterTempMemory;
    QMap<QString, int> ParameterTypes;
    QMap<QString, double> ParameterMaxVal;
    QMap<QString, double> ParameterMinVal;
    QMap<QString, bool> ParameterHasLimit;
    QMap<QString, bool> ParameterHasSlider;
    QMap<QString, std::function<void(QString, void*)>> ParameterCallback;

    QList<QString> m_ParameterSetRequestNames;
    QList<QString> m_ParameterSetRequestValues;

    std::function<void(QString, void*)> ParameterValueChangeCallback;
    std::function<void(void)> ParametersChangedCallback;

    QMutex ParameterMutex;

    inline void AddParameter(QString name, bool & value, bool defeault = false)
    {
        ParameterMutex.lock();
        ParameterNames.append(name);
        ParameterValue[name] = QString::number(value?1:0);
        ParameterDefaultValues[name] = QString::number(defeault?1:0);
        ParameterMemory[name] = (void*) (&value);
        ParameterTypes[name] = LISEM_LAYERPARAMETER_TYPE_BOOL;
        ParameterMutex.unlock();


        if(ParametersChangedCallback)
        {
            ParametersChangedCallback();
        }
    }

    inline void AddParameter(QString name, int & value, int defeault = 0, bool range = false, int min = 0, int max = 100, bool slider = false)
    {
        ParameterMutex.lock();
        ParameterNames.append(name);
        ParameterDefaultValues[name] = QString::number(defeault);
        ParameterValue[name] = QString::number(value);
        ParameterMemory[name] = (void*) (&value);
        ParameterTypes[name] = LISEM_LAYERPARAMETER_TYPE_COUNT;
        ParameterMaxVal[name] = max;
        ParameterMinVal[name] = min;
        ParameterHasLimit[name] = range;
        ParameterHasSlider[name] = slider;
        ParameterMutex.unlock();

        if(ParametersChangedCallback)
        {
            ParametersChangedCallback();
        }
    }

    inline void AddParameter(QString name, double & value, double defeault = 0.0, bool range = false, double min = 0, double max = 100.0, bool slider = false)
    {
        ParameterMutex.lock();
        ParameterNames.append(name);
        ParameterDefaultValues[name] = QString::number(defeault);
        ParameterValue[name] = QString::number(value);
        ParameterMemory[name] = (void*) (&value);
        ParameterTypes[name] = LISEM_LAYERPARAMETER_TYPE_NUMBER;
        ParameterMaxVal[name] = max;
        ParameterMinVal[name] = min;
        ParameterHasLimit[name] = range;
        ParameterHasSlider[name] = slider;
        ParameterMutex.unlock();

        if(ParametersChangedCallback)
        {
            ParametersChangedCallback();
        }
    }

    inline void AddParameter(QString name, ColorF & value, ColorF defeault = ColorF(1.0,1.0,1.0,1.0))
    {
        ParameterMutex.lock();
        ParameterNames.append(name);
        ParameterDefaultValues[name] = defeault.toString();
        ParameterValue[name] = value.toString();
        ParameterMemory[name] = (void*) (&value);
        ParameterTypes[name] = LISEM_LAYERPARAMETER_TYPE_COLOR;
        ParameterMutex.unlock();

        if(ParametersChangedCallback)
        {
            ParametersChangedCallback();
        }
    }

    inline void AddParameter(QString name, LSMVector3 & value, LSMVector3 defeault = LSMVector3(0.0,1.0,0.0))
    {
        ParameterMutex.lock();
        ParameterNames.append(name);
        ParameterDefaultValues[name] = defeault.toString();
        ParameterValue[name] = value.toString();
        ParameterMemory[name] = (void*) (&value);
        ParameterTypes[name] = LISEM_LAYERPARAMETER_TYPE_VEC3;
        ParameterMutex.unlock();

        if(ParametersChangedCallback)
        {
            ParametersChangedCallback();
        }
    }

    inline void AddParameter(QString name, QImage & value, QString defeault = "")
    {
        ParameterMutex.lock();
        ParameterNames.append(name);
        ParameterDefaultValues[name] = defeault;
        ParameterValue[name] = defeault;
        ParameterMemory[name] = (void*) (&value);
        ParameterTypes[name] = LISEM_LAYERPARAMETER_TYPE_TEXTURE;
        ParameterMutex.unlock();

        if(ParametersChangedCallback)
        {
            ParametersChangedCallback();
        }
    }

    /*inline void AddParameter(QString name, OpenGLCLObject & value, QString defeault = "")
    {
        ParameterMutex.lock();
        ParameterNames.append(name);
        ParameterDefaultValues[name] = defeault;
        ParameterValue[name] = defeault;
        ParameterMemory[name] = (void*) (&value);
        ParameterTypes[name] = LISEM_LAYERPARAMETER_TYPE_3DOBJECT;
        ParameterMutex.unlock();

        if(ParametersChangedCallback)
        {
            ParametersChangedCallback();
        }
    }*/

    inline void AddParameter(QString name, QString & value, QString defeault = "")
    {
        ParameterMutex.lock();
        ParameterNames.append(name);
        ParameterDefaultValues[name] = defeault;
        ParameterValue[name] = value;
        ParameterMemory[name] = (void*) (&value);
        ParameterTypes[name] = LISEM_LAYERPARAMETER_TYPE_STRING;
        ParameterMutex.unlock();

        if(ParametersChangedCallback)
        {
            ParametersChangedCallback();
        }
    }

    inline void SetParameterCallBack(QString name, std::function<void(QString, void*)> f)
    {
        ParameterCallback[name] = f;
    }

    inline bool SetParameter(QString name, QString value)
    {
        ParameterMutex.lock();
        bool found = true;
        if(ParameterNames.contains(name))
        {
            m_ParameterSetRequestNames.append(name);
            m_ParameterSetRequestValues.append(value);
        }else {
            found = false;
        }
        ParameterMutex.unlock();
        return found;
    }

    inline QList<int> GetParameterTypes()
    {
        ParameterMutex.lock();
        QList<int> ret;

        for(int i = 0; i < ParameterNames.length(); i++)
        {
            ret.append(ParameterTypes[ParameterNames.at(i)]);
        }
        ParameterMutex.unlock();
        return ret;

    }

    inline QList<QString> GetParameterNames()
    {
        ParameterMutex.lock();
        QList<QString> ret;
        ret = ParameterNames;
        ParameterMutex.unlock();
        return ParameterNames;
    }
    inline QList<QString> GetParameterValues()
    {
        ParameterMutex.lock();
        QList<QString> ret;

        for(int i = 0; i < ParameterNames.length(); i++)
        {
            ret.append(ParameterValue[ParameterNames.at(i)]);
        }
        ParameterMutex.unlock();
        return ret;
    }


    inline QString GetParameterValueAsString(QString name)
    {
        ParameterMutex.lock();
        QString val;
        if(ParameterValue.contains(name))
        {
            val = ParameterValue[name];
        }else {
            val=  "";
        }

        ParameterMutex.unlock();

        return val;
    }

    inline QString GetParameterValueAsString(int index)
    {
        ParameterMutex.lock();
        QString val = ParameterValue[ParameterNames.at(index)];
        ParameterMutex.unlock();

        return val;
    }

    inline int GetParameterCount()
    {
        ParameterMutex.lock();
        int count = ParameterNames.length();
        ParameterMutex.unlock();

        return count;
    }

    inline void * GetParameterMemory(QString name)
    {
        void * mem;
        if(ParameterMemory.contains(name))
        {
            mem = ParameterMemory[name];
        }else {
            mem = nullptr;
        }
        return mem;
    }

    inline void RestoreDefeault()
    {
        ParameterMutex.lock();
        QList<QString> ret;

        for(int i = 0; i < ParameterNames.length(); i++)
        {
            QString name = ParameterNames.at(i);
            QString val = ParameterDefaultValues[name];

            internal_updateparam(name,val);

        }
        ParameterMutex.unlock();
    }

    inline void UpdateParameters()
    {
        ParameterMutex.lock();
        QList<QString> names;
        QList<void*> vals;

        for(int i = 0; i < m_ParameterSetRequestNames.length(); i++)
        {
            QString name = m_ParameterSetRequestNames.at(i);
            QString val = m_ParameterSetRequestValues.at(i);

            if(ParameterMemory.contains(name))
            {

                void * valm = ParameterMemory[name];

                //set values
                internal_updateparam(name,val);
                names.append(name);
                vals.append(valm);
            }


        }
        ParameterMutex.unlock();

        //callback
        if(ParameterValueChangeCallback)
        {
            for(int i = 0; i < m_ParameterSetRequestNames.length(); i++)
            {

                ParameterValueChangeCallback(names.at(i),vals.at(i));
            }
        }
    }

    inline void internal_updateparam(QString in_name, QString in_value)
    {
        QString name = in_name;
        QString value = in_value;
        if(ParameterTypes.contains(name) && ParameterMemory.contains(name) && ParameterDefaultValues.contains(name))
        {
            int type = ParameterTypes[name];
            QString defeault = ParameterDefaultValues[name];

            if(type == LISEM_LAYERPARAMETER_TYPE_BOOL)
            {
                bool * mem = static_cast<bool*>(ParameterMemory[name]);
                bool ok;
                bool val_convert = !(value.toInt(&ok) == 0);
                if(ok)
                {
                    if(ParameterCallback.contains(name))
                    {
                        ParameterCallback[name](value, mem);
                    }
                }else {
                    val_convert = !(defeault.toInt(&ok) == 0);

                    if(ParameterCallback.contains(name))
                    {
                        ParameterCallback[name](defeault, mem);
                    }
                }
                *mem = val_convert;
                ParameterValue[name] = QString::number(val_convert?1:0);
            }else if(type == LISEM_LAYERPARAMETER_TYPE_COUNT)
            {
                int * mem = static_cast<int*>(ParameterMemory[name]);
                bool ok;
                int val_convert = value.toInt(&ok);
                if(ok)
                {
                    if(ParameterCallback.contains(name))
                    {
                        ParameterCallback[name](value, mem);
                    }
                }else {
                    val_convert = defeault.toInt(&ok);

                    if(ParameterCallback.contains(name))
                    {
                        ParameterCallback[name](defeault, mem);
                    }
                }
                ParameterValue[name] = QString::number(val_convert);
                *mem = val_convert;
            }else if(type == LISEM_LAYERPARAMETER_TYPE_NUMBER)
            {
                double * mem = static_cast<double*>(ParameterMemory[name]);
                bool ok;
                double val_convert = value.toDouble(&ok);
                if(ok)
                {
                    if(ParameterCallback.contains(name))
                    {
                        ParameterCallback[name](value, mem);
                    }
                }else {
                    val_convert = defeault.toDouble(&ok);

                    if(ParameterCallback.contains(name))
                    {
                        ParameterCallback[name](defeault, mem);
                    }
                }
                ParameterValue[name] = QString::number(val_convert);
                *mem = val_convert;
            }else if(type == LISEM_LAYERPARAMETER_TYPE_STRING)
            {
                QString * mem = static_cast<QString*>(ParameterMemory[name]);
                QString val_convert = value;
                if(ParameterCallback.contains(name))
                {
                    ParameterCallback[name](value, mem);
                }
                ParameterValue[name] =val_convert;
                *mem = val_convert;

            }else if(type == LISEM_LAYERPARAMETER_TYPE_COLOR)
            {
                ColorF * mem = static_cast<ColorF*>(ParameterMemory[name]);
                ColorF val_convert;
                val_convert.fromString(value);
                if(ParameterCallback.contains(name))
                {
                    ParameterCallback[name](value, mem);
                }
                *mem = val_convert;
                ParameterValue[name] =val_convert.toString();
            }else if(type == LISEM_LAYERPARAMETER_TYPE_VEC3)
            {
                LSMVector3 * mem = static_cast<LSMVector3*>(ParameterMemory[name]);
                LSMVector3 val_convert;
                val_convert.fromString(value);
                if(ParameterCallback.contains(name))
                {
                    ParameterCallback[name](value, mem);
                }
                *mem = val_convert;
                ParameterValue[name] =val_convert.toString();

            }else if(type == LISEM_LAYERPARAMETER_TYPE_TEXTURE)
            {
                QImage * mem = static_cast<QImage*>(ParameterMemory[name]);

                //check if file can be read
                QString actual_val;

                QFileInfo f(value);
                if(f.exists())
                {
                    actual_val = value;
                }else {
                    actual_val = defeault;
                }

                if(ParameterCallback.contains(name))
                {
                    ParameterCallback[name](actual_val, mem);
                }

                //load the actual image
                mem->load(actual_val);
                ParameterValue[name] =actual_val;


            }else if(LISEM_LAYERPARAMETER_TYPE_3DOBJECT)
            {
                QString * mem = static_cast<QString*>(ParameterMemory[name]);
                QString val_convert = value;
                if(ParameterCallback.contains(name))
                {
                    ParameterCallback[name](value, mem);
                }
                *mem = val_convert;
                ParameterValue[name] =val_convert;
            }else {

                //do nothing
            }


        }


    }



    inline void SetGeneralCallBacks(std::function<void(void)> f_altered, std::function<void(QString, void*)> f_value)
    {

        ParameterValueChangeCallback = f_value;
        ParametersChangedCallback = f_altered;

    }

    inline void RemoveGeneralCallBacks()
    {
        ParameterValueChangeCallback = [](QString, void*){};
        ParametersChangedCallback = [](){};

    }



    inline void Save(QJsonObject * obj)
    {


        ParameterMutex.lock();

        for(int i = 0; i < ParameterNames.length(); i++)
        {
            QString name = ParameterNames.at(i);
            if(ParameterValue.contains(name))
            {
                QString value = ParameterValue[name];
                int type = ParameterTypes[name];
                QString defeault = ParameterDefaultValues[name];

                LSMSerialize::From(obj,name,value);

            }
        }

        ParameterMutex.unlock();

    }

    //
    inline void Restore(QJsonObject * obj)
    {
        QList<QString> names;
        QList<void*> vals;

        ParameterMutex.lock();

        for(int i = 0; i < ParameterNames.length(); i++)
        {
            QString name = ParameterNames.at(i);
            if(obj->contains(name))
            {
                QString val;
                LSMSerialize::To(obj,name,val);

                if(ParameterMemory.contains(name))
                {

                    void * valm = ParameterMemory[name];

                    //set values
                    internal_updateparam(name,val);

                    names.append(name);
                    vals.append(valm);
                }

            }
        }
        ParameterMutex.unlock();

        if(ParameterValueChangeCallback)
        {
            //callback
            for(int i = 0; i < m_ParameterSetRequestNames.length(); i++)
            {
                ParameterValueChangeCallback(names.at(i),vals.at(i));
            }
        }
    }

} LayerParameters;
