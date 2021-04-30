#pragma once
#include <nodes/NodeDataModel>
#include "scriptmanager.h"
#include "geo/raster/map.h"
#include <QtCore/QJsonValue>

#include "QList"
#include "QString"


typedef struct PreviewArgument
{
    bool xb;
    int xi;
    double xd;
    QString xs;

    bool isvalid=false;

    //reference based types
    QString type;
    std::shared_ptr<void> xm = nullptr;

} PreviewArgument;



class FunctionalNodeModel : public QtNodes::NodeDataModel
{

    Q_OBJECT;

public:

    inline FunctionalNodeModel()
    {

    }

    inline ~FunctionalNodeModel()
    {

    }

    inline QJsonObject
    save() const
    {
      QJsonObject modelJson = QtNodes::NodeDataModel::save();



      return modelJson;
    }

    inline void
    restore(QJsonObject const &p)
    {


    }

    bool m_CallBackFuncSet = false;
    std::function<void(FunctionalNodeModel* n)> m_CallBackFunc;
    template<typename _Callable1, typename... _Args1>
    inline void RegisterDataChangedCallBack(_Callable1&& __f1, _Args1&&... __args1)
    {
        m_CallBackFuncSet = true;
        m_CallBackFunc = std::bind(std::forward<_Callable1>(__f1),std::forward<_Args1>(__args1)...,std::placeholders::_1);
    }

    inline void CallBackDataChanged()
    {
        if(m_CallBackFuncSet)
        {
            m_CallBackFunc(this);
        }

    }
    inline virtual bool FirstOutputIsFirstInput()
    {
        return false;
    }

    inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, LSMScriptEngine * e)
    {
        QList<QString> l;

        return l;
    }

    inline void SetValidationState(QtNodes::NodeValidationState s)
    {
        modelValidationState = s;
    }

    inline void SetValidationMessage(QString message)
    {
        modelValidationError = message;
    }
    inline QtNodes::NodeValidationState validationState() const override
    {
       return modelValidationState;
    }

    inline QString  validationMessage() const override
    {
      return modelValidationError;
    }

    //for the real-time preview stuff

    inline virtual bool IsReturner()
    {

        return false;
    }

    inline virtual void ClearPreview()
    {
        return;
    }

    inline virtual void SetPreviewData(PreviewArgument data)
    {

        return;
    }
    inline virtual void ClearError()
    {

    }

    inline virtual bool IsInternalDataForPreviewValid()
    {
        return true;

    }

    inline virtual bool IsPreviewError()
    {
        return false;
    }

    inline virtual bool IsPreviewAvailable()
    {
        return false;
    }

    inline virtual PreviewArgument GetPreviewData()
    {
        return PreviewArgument();
    }


    bool internal_datachangeflag = false;
    bool external_datachangeflag = false;
    inline void SetInternalDataChangeFlag(bool x)
    {
        internal_datachangeflag = x;
    }

    inline bool GetInternalDataChangeFlag()
    {
        return internal_datachangeflag;
    }

    inline void SetExternalDataChangeFlag(bool x)
    {
        external_datachangeflag = x;
    }

    inline bool GetExternalDataChangeFlag()
    {
        return external_datachangeflag;
    }

    inline virtual void SetPreviewIsLoading()
    {

    }




    QtNodes::NodeValidationState modelValidationState = QtNodes::NodeValidationState::Valid;
    QString modelValidationError = QString("");

    signals:

    void OnInternalDataChanged();


};
