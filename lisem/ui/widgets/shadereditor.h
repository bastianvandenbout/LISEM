#ifndef SHADEREDITOR_H
#define SHADEREDITOR_H


#include "QWidget"

#include "
#include "QVBoxLayout"
#include "QHBoxLayout"
#include "QIcon"
#include "lsmio.h"
#include "QCoreApplication"
#include "QToolButton"
#include <QTabWidget>
#include <QLabel>
#include "QFileDialog"
#include "QMessageBox"
#include "QMovie"
#include "QSplitter"
#include "widgets/labeledwidget.h"
#include "resourcemanager.h"
#include "QTextDocument"
#include "QTextDocumentFragment"
#include "scripting/codeeditor.h"
#include "qpixmap.h"
#include "layer/geo/uishaderlayer.h"
#include "layer/editors/uishadereditor.h"
#include "widgets/spoilerwidget.h"

#include "atomic"
#include "site.h"

inline QImage * GetImageFromFile(QString file)
{
    QFile f(file);
    if(file.isEmpty() || !f.exists())
    {
        QImage *i = new QImage();
        i->load(GetSite() + LISEM_FOLDER_ASSETS + "nodata.png");
        return i;
    }
    //returns nullptr if the file can not be read as an image by LISEM

    //is it a regular image
    if(file.endsWith(".jpg")|| file.endsWith(".jpeg")|| file.endsWith(".png")||file.endsWith(".bmp"))
    {
        QImage *i = new QImage();
        bool suc = i->load(file);
        if(suc)
        {
            return i;
        }else
        {
            delete i;
            return nullptr;
        }

        // or a geo-image
    //is it a geo-file
    }else
    {
        //get the first three bands only
        try
        {
            std::vector<cTMap *> maps = LoadMapBandList(file);
            if(maps.size() == 0)
            {
                return nullptr;
            }


            std::vector<cTMap *> mapsnorm;

            {
                for(int i = 0; i < std::min(4,(int)maps.size()); i++)
                {
                    double min;
                    double max;
                    NormalizeMapInPlace(maps.at(i),min,max);
                    mapsnorm.push_back(maps.at(i));
                }
            }

            //convert to QImage
            QImage* img = new QImage(maps.at(0)->nrCols(), maps.at(0)->nrRows(), QImage::Format_RGBA8888);
            if(maps.size() >3)
            {
                for (int r = 0; r < maps.at(0)->nrRows(); r++)
                {
                    for (int c = 0; c < maps.at(0)->nrCols(); c++)
                    {
                        int val_r = 255.0 * (!pcr::isMV(mapsnorm.at(0)->data[r][c])? mapsnorm.at(0)->data[r][c] : 0.0);
                        int val_g = 255.0 * (!pcr::isMV(mapsnorm.at(1)->data[r][c])? mapsnorm.at(0)->data[r][c] : 0.0);
                        int val_b = 255.0 * (!pcr::isMV(mapsnorm.at(2)->data[r][c])? mapsnorm.at(0)->data[r][c] : 0.0);
                        int val_a = 255.0 * (!pcr::isMV(mapsnorm.at(3)->data[r][c])? mapsnorm.at(0)->data[r][c] : 0.0);
                        img->setPixelColor(c,r,QColor(val_r,val_g,val_b,val_a));
                    }
                }

            }else if(maps.size() >2)
            {
                for (int r = 0; r < maps.at(0)->nrRows(); r++)
                {
                    for (int c = 0; c < maps.at(0)->nrCols(); c++)
                    {
                        int val_r = 255.0 * (!pcr::isMV(mapsnorm.at(0)->data[r][c])? mapsnorm.at(0)->data[r][c] : 0.0);
                        int val_g = 255.0 * (!pcr::isMV(mapsnorm.at(1)->data[r][c])? mapsnorm.at(1)->data[r][c] : 0.0);
                        int val_b = 255.0 * (!pcr::isMV(mapsnorm.at(2)->data[r][c])? mapsnorm.at(2)->data[r][c] : 0.0);
                        int val_a = 1.0;
                        img->setPixelColor(c,r,QColor(val_r,val_g,val_b,val_a));
                    }
                }
            }else if(maps.size() >1)
            {
                for (int r = 0; r < maps.at(0)->nrRows(); r++)
                {
                    for (int c = 0; c < maps.at(0)->nrCols(); c++)
                    {
                        int val_r = 255.0 * (!pcr::isMV(mapsnorm.at(0)->data[r][c])? mapsnorm.at(0)->data[r][c] : 0.0);
                        int val_g = 255.0 * (!pcr::isMV(mapsnorm.at(1)->data[r][c])? mapsnorm.at(1)->data[r][c] : 0.0);
                        int val_b = 0;
                        int val_a = 1.0;
                        img->setPixelColor(c,r,QColor(val_r,val_g,val_b,val_a));
                    }
                }

            }else if(maps.size() >1)
            {
                for (int r = 0; r < maps.at(0)->nrRows(); r++)
                {
                    for (int c = 0; c < maps.at(0)->nrCols(); c++)
                    {
                        int val_r = 255.0 * (!pcr::isMV(mapsnorm.at(0)->data[r][c])? mapsnorm.at(0)->data[r][c] : 0.0);
                        int val_g = 255.0 * (!pcr::isMV(mapsnorm.at(0)->data[r][c])? mapsnorm.at(0)->data[r][c] : 0.0);
                        int val_b = 255.0 * (!pcr::isMV(mapsnorm.at(0)->data[r][c])? mapsnorm.at(0)->data[r][c] : 0.0);
                        int val_a = 1.0;
                        img->setPixelColor(c,r,QColor(val_r,val_g,val_b,val_a));
                    }
                }

            }

            for(int i = 0; i < maps.size(); i++)
            {
                delete maps.at(i);
            }
            return img;
        }catch(...)
        {
            return nullptr;
        }
    }

    return nullptr;
}


//class that allows you to select and shows an input texture
class TextureSelector : public QLabel
{


    Q_OBJECT

    QImage * m_Image = nullptr;
    QString m_Texture;

public:


    inline TextureSelector() :QLabel()
    {

        //set empty texture
        SetTexture("");
        this->setFixedWidth(150);
        this->setFixedHeight(150);

        setContextMenuPolicy(Qt::CustomContextMenu);

        connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));

    }

    inline void SetTexture(QString file)
    {

        if(m_Image != nullptr)
        {
            delete m_Image;
        }
        m_Image = GetImageFromFile(file);

        if(m_Image != nullptr)
        {
            m_Texture= file;
            this->setPixmap(QPixmap::fromImage(m_Image->scaled(150,150,Qt::KeepAspectRatio)));
        }else
        {
            m_Image = GetImageFromFile("");
            this->setPixmap(QPixmap::fromImage(m_Image->scaled(150,150,Qt::KeepAspectRatio)));
        }
    }

    inline QString GetTexture()
    {
        return m_Texture;
    }



public slots:
    inline void onCustomContextMenu(const QPoint &p)
    {

        QMenu menu;

        menu.addAction(new QAction("Load File"));
        menu.addAction(new QAction("Set Coordinates to this"));
        menu.addAction(new QAction("Load"));

        QAction* selAct = menu.exec(mapToGlobal(p));
        if(0 != selAct){

            //for(int i = 0; i < item->m_Actions.size(); i++)
            {
                if(selAct->text() == "Load File")
                {
                    QString path = QFileDialog::getOpenFileName(this,
                                                        QString("Select object file"),
                                                        GetFIODir(LISEM_DIR_FIO_GEODATA),
                                                        GetExtensionsFileFilter(GetModelExtensions()));
                    if(!path.isEmpty())
                    {

                        SetTexture(path);
                    }


                }

            }
        }
    }
};


//class that allows you to select up to n input textures
class TextureInputSelector : public QWidget
{

    Q_OBJECT

    QList<TextureSelector*> m_Selectors;
    QHBoxLayout * m_Layout;
public:

    inline TextureInputSelector(int n)
    {
        m_Layout = new QHBoxLayout();
        m_Layout->setMargin(0);
        this->setLayout(m_Layout);

        for(int i = 0; i < n; i++)
        {
            TextureSelector * s = new TextureSelector();
            m_Selectors.push_back(s);
            m_Layout->addWidget(s);
        }



    }
    inline void SetTextures(std::vector<QString> files, std::vector<cTMap *>maps)
    {

    }
    inline void SetTexture(QString file, int n)
    {


    }


};


class ShaderPassEditor : public QWidget
{

    Q_OBJECT

    CodeEditor * m_CodeEditor= nullptr;
    TextureInputSelector *m_TextureSelector = nullptr;

    QVBoxLayout * m_Layout = nullptr;

    Spoiler * spoilerdraw = nullptr;
    QPlainTextEdit * m_ErrorWidget = nullptr;
public:

    inline ShaderPassEditor() : QWidget()
    {
        m_Layout = new QVBoxLayout();
        m_Layout->setMargin(0);
        this->setLayout(m_Layout);

        m_CodeEditor = new CodeEditor();
        m_Layout->addWidget(m_CodeEditor);

        m_TextureSelector = new TextureInputSelector(4);
        m_Layout->addWidget(m_TextureSelector);

       spoilerdraw = new Spoiler("Opengl Output");
       m_ErrorWidget = new QPlainTextEdit();
       QHBoxLayout * l = new QHBoxLayout();
       l->addWidget(m_ErrorWidget);
       l->setSpacing(0);
       l->setMargin(0);

       spoilerdraw->setContentLayout(*l);


       m_Layout->addWidget(spoilerdraw);
    }

    inline void SetShaderText(QString s)
    {
        m_CodeEditor->document()->setPlainText(s);
    }

    inline QString GetShaderText()
    {
        return m_CodeEditor->document()->toPlainText();
    }

    inline std::vector<QString> GetImageFiles()
    {
        return {};
    }

    inline std::vector<cTMap *> GetImageMaps()
    {
        return {};
    }

    inline void SetErrors(std::vector<int> lines, std::vector<QString> messages)
    {
        QString tot;

        if(messages.size() > 0)
        {

            tot = "Errors:";
            for(int i = 0; i < messages.size(); i++)
            {

                tot += "\n";
                tot += messages.at(i);
            }
        }else
        {
            tot = "Succes!";

        }

        m_ErrorWidget->document()->setPlainText(tot);
        m_ErrorWidget->setEnabled(false);

    }

};


class ShaderEditor : public QWidget
{
    Q_OBJECT

    QTabWidget * m_ScriptTabs = nullptr;
    QVBoxLayout * m_MainLayout = nullptr;
    QWidget * m_MenuWidget = nullptr;
    QHBoxLayout * m_MenuLayout = nullptr;

    UIShaderLayerEditor*  m_ShaderLayerEditor = nullptr;
    Spoiler * spoilerdraw = nullptr;
    QPlainTextEdit * m_ErrorWidget = nullptr;
    std::vector<QString> m_PassNames = {"A","B","C","D","E","F","G","H","I","J","K"};
    int m_MaxPasses = 7;
public:

    inline ShaderEditor()
    {
        m_MainLayout = new QVBoxLayout();
        this->setLayout(m_MainLayout);
        m_MainLayout->setMargin(0);

        spoilerdraw = new Spoiler("Opengl Input");
        m_ErrorWidget = new QPlainTextEdit();
        QHBoxLayout * l = new QHBoxLayout();
        l->addWidget(m_ErrorWidget);

        QString inputtext = QString("")
            + "\nuniform vec2 iScreenResolution; //resolution of full opengl screen of LISEM"
            + "\nuniform vec2 iResolution; //resolution of shader buffers"
            + "\nuniform float iTime; //time in seconds since shader was added"
            + "\nuniform int iFrame; //frame number since this shader was added"
            + "\nuniform vec2 iChannelResolution[4]; //resolution of input images"
            + "\nuniform vec4 iMouse; //xy->mouse coords (relative to shader area), zw->left and right mouse button pressed"
            + "\nuniform sampler2D iChannel0; //input images, have resolution iChannelResolution[i]"
            + "\nuniform sampler2D iChannel1;"
            + "\nuniform sampler2D iChannel2;"
            + "\nuniform sampler2D iChannel3;"
            + "\nuniform sampler2D iChannelB0; //input buffers as obtained from previous frame"
            + "\nuniform sampler2D iChannelB1;"
            + "\nuniform sampler2D iChannelB2;"
            + "\nuniform sampler2D iChannelB3;"
            + "\nuniform vec4 iDate; //year,month,day,time in seconds"
            + "\nuniform vec4 iScreenCoords; //the upper left and bottom right coordinates in screen relative numbers (0-1)"
            + "\nuniform vec4 iCRSCoords; //the upper left and bottom right coordinates in CRS coordinates"
            + "\nuniform highp vec3 CameraPosition; //only valid in 3D"
            + "\nuniform highp mat4 CMatrix;//only valid in 3D"
            + "\nuniform highp vec3 iSunDir; //only valid in 3D"
            + "\nuniform highp sampler2D ScreenColor; //from previous full-screen frame, has resolution iScreenResolution"
            + "\nuniform highp sampler2D ScreenPosX; //from previous full-screen frame, has resolution iScreenResolution"
            + "\nuniform highp sampler2D ScreenPosY; //from previous full-screen frame, has resolution iScreenResolution"
            + "\nuniform highp sampler2D ScreenPosZ; //from previous full-screen frame, has resolution iScreenResolution"
            + "\nuniform highp sampler2D ScreenNormal; //from previous full-screen frame, has resolution iScreenResolution";
        m_ErrorWidget->document()->setPlainText(inputtext);
        l->setSpacing(0);
        l->setMargin(0);

        spoilerdraw->setContentLayout(*l);



        m_ScriptTabs = new QTabWidget();
        m_ScriptTabs->setTabsClosable(true);

        //m_ScriptTabs->addTab(new ShaderPassEditor(), "Pass A");

        //shader compile
        //shader play
        //shader pause



        m_MenuWidget = new QWidget(this);
        m_MenuLayout = new QHBoxLayout(m_MenuWidget);

        QIcon *icon_start;
        QIcon *icon_pause;
        QIcon *icon_stop;
        QIcon *icon_open;
        QIcon *icon_save;
        QIcon *icon_saveas;
        QIcon *icon_new;
        QIcon *icon_info;
        QIcon *icon_debug;

        QIcon *icon_compile;
        QIcon *icon_indent;
        QIcon *icon_indentr;
        QIcon *icon_style;
        QIcon *icon_comment;
        QIcon *icon_normal;
        QIcon *icon_add;
        QIcon *icon_remove;
        QIcon *icon_reset;

        icon_start = (GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_START),-1));
        icon_pause = (GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_PAUSE),-1));
        icon_stop = (GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_STOP),-1));
        icon_open = (GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_LOAD),-1));
        icon_save = (GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_SAVE),-1));
        icon_saveas = (GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_SAVEAS),-1));
        icon_new= (GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_NEW),-1));
        icon_info= (GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_INFO),-1));
        icon_debug= (GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_DEBUG),-1));
        icon_add= (GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_ADD),-1));
        icon_remove= (GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_REMOVE),-1));
        icon_reset = (GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_REFRESH),-1));

        icon_compile =(GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_COMPILE),-1));
        icon_indent =(GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_TEXTINDENT),-1));
        icon_indentr =(GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_TEXTINDENTR),-1));
        icon_style =(GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_TEXTSTYLE),-1));
        icon_comment =(GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_TEXTCOMMENT),-1));
        icon_normal =(GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_TEXTREGULAR),-1));


        QToolButton * StartButton = new QToolButton(this);
        StartButton->setIcon(*icon_start);
        StartButton->setIconSize(QSize(22,22));
        StartButton->setMaximumSize(QSize(22,22));
        StartButton->resize(22,22);
        StartButton->setEnabled(true);

        QToolButton * PauseButton = new QToolButton(this);
        PauseButton->setIcon(*icon_pause);
        PauseButton->setIconSize(QSize(22,22));
        PauseButton->setMaximumSize(QSize(22,22));
        PauseButton->resize(22,22);
        PauseButton->setEnabled(true);

        QToolButton * StopButton = new QToolButton(this);
        StopButton->setIcon(*icon_stop);
        StopButton->setIconSize(QSize(22,22));
        StopButton->setMaximumSize(QSize(22,22));
        StopButton->resize(22,22);
        StopButton->setEnabled(true);

        QToolButton * AddButton = new QToolButton(this);
        AddButton->setIcon(*icon_add);
        AddButton->setIconSize(QSize(22,22));
        AddButton->setMaximumSize(QSize(22,22));
        AddButton->resize(22,22);
        AddButton->setEnabled(true);

        QToolButton * RemoveButton = new QToolButton(this);
        RemoveButton->setIcon(*icon_remove);
        RemoveButton->setIconSize(QSize(22,22));
        RemoveButton->setMaximumSize(QSize(22,22));
        RemoveButton->resize(22,22);
        RemoveButton->setEnabled(true);

        QToolButton * RefreshButton = new QToolButton(this);
        RefreshButton->setIcon(*icon_reset);
        RefreshButton->setIconSize(QSize(22,22));
        RefreshButton->setMaximumSize(QSize(22,22));
        RefreshButton->resize(22,22);
        RefreshButton->setEnabled(true);

        QToolButton * SaveButton = new QToolButton(this);
        SaveButton->setIcon(*icon_save);
        SaveButton->setIconSize(QSize(22,22));
        SaveButton->setMaximumSize(QSize(22,22));
        SaveButton->resize(22,22);
        SaveButton->setEnabled(true);

        QToolButton * LoadButton = new QToolButton(this);
        LoadButton->setIcon(*icon_open);
        LoadButton->setIconSize(QSize(22,22));
        LoadButton->setMaximumSize(QSize(22,22));
        LoadButton->resize(22,22);
        LoadButton->setEnabled(true);


        m_MenuLayout->addWidget(StartButton);
        m_MenuLayout->addWidget(PauseButton);
        m_MenuLayout->addWidget(StopButton);
        m_MenuLayout->addWidget(AddButton);
        m_MenuLayout->addWidget(RemoveButton);
        m_MenuLayout->addWidget(RefreshButton);
        m_MenuLayout->addWidget(SaveButton);
        m_MenuLayout->addWidget(LoadButton);

        connect(StartButton,SIGNAL(clicked()),this,SLOT(OnRequestRunCode()));
        connect(PauseButton,SIGNAL(clicked()),this,SLOT(OnRequestPauseCode()));
        connect(StopButton,SIGNAL(clicked()),this,SLOT(OnRequestStopCode()));
        connect(AddButton,SIGNAL(clicked()),this,SLOT(OnRequestAddCode()));
        connect(RemoveButton,SIGNAL(clicked()),this,SLOT(OnRequestRemoveCode()));
        connect(RefreshButton,SIGNAL(clicked()),this,SLOT(OnRequestRefreshCode()));
        connect(LoadButton,SIGNAL(clicked()),this,SLOT(OnRequestLoadCode()));
        connect(SaveButton,SIGNAL(clicked()),this,SLOT(OnRequestSaveCode()));

        connect(this,&ShaderEditor::int_Shader_error,this,&ShaderEditor::int_OnShaderCompileError,Qt::QueuedConnection);

        m_MainLayout->addWidget(m_MenuWidget);
        m_MainLayout->addWidget(m_ScriptTabs);

        m_MainLayout->addWidget(spoilerdraw);

        this->setLayout(m_MainLayout);
    }

    inline void SetEditorLayer(UIShaderLayerEditor * s)
    {
        m_ShaderLayerEditor = s;


        std::vector<QString> shaders = s->m_ShaderLayer->GetShaderTexts();

        if(shaders.size() > m_MaxPasses)
        {
            int i = shaders.size();
            while(i > m_MaxPasses)
            {
                shaders.erase(shaders.begin());
                i--;
            }
        }
        for(int i = 0; i < shaders.size(); i++)
        {
            int n = shaders.size()-1 - i;
            ShaderPassEditor *spe = new ShaderPassEditor();
            m_ScriptTabs->addTab(spe, "Pass " + m_PassNames.at(n));
            spe->SetShaderText(shaders.at(n));

        }

    }



    bool m_CallBackUpdateSet = false;
    std::function<void(std::vector<QString>, std::vector<std::vector<QString>>, std::vector<std::vector<cTMap *>>)> m_CallBackUpdate;
    template<typename _Callable1, typename... _Args1>
    inline void SetShaderUpdateCallback(_Callable1&& __f1, _Args1&&... __args1)
    {
        m_CallBackUpdateSet = true;
        m_CallBackUpdate = std::bind(std::forward<_Callable1>(__f1),std::forward<_Args1>(__args1)...,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
    }

    bool m_CallBackRunSet = false;
    std::function<void()> m_CallBackRun;
    template<typename _Callable1, typename... _Args1>
    inline void SetShaderRunCallback(_Callable1&& __f1, _Args1&&... __args1)
    {
        m_CallBackRunSet = true;
        m_CallBackRun = std::bind(std::forward<_Callable1>(__f1),std::forward<_Args1>(__args1)...);
    }

    bool m_CallBackPauseSet = false;
    std::function<void()> m_CallBackPause;
    template<typename _Callable1, typename... _Args1>
    inline void SetShaderPauseCallback(_Callable1&& __f1, _Args1&&... __args1)
    {
        m_CallBackPauseSet = true;
        m_CallBackPause = std::bind(std::forward<_Callable1>(__f1),std::forward<_Args1>(__args1)...);
    }

    bool m_CallBackCoordinatesSet = false;
    std::function<void(BoundingBox, bool,bool,GeoProjection)> m_CallBackCoordinates;
    template<typename _Callable1, typename... _Args1>
    inline void SetShaderCoordinatesCallback(_Callable1&& __f1, _Args1&&... __args1)
    {
        m_CallBackCoordinatesSet = true;
        m_CallBackCoordinates= std::bind(std::forward<_Callable1>(__f1),std::forward<_Args1>(__args1)...,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4);
    }


    QMutex mut_errors;
    std::vector<int> m_pass;
    std::vector<std::vector<int>> m_lines;
    std::vector<std::vector<QString>> m_errors;

    inline void OnShaderCompileError(std::vector<int> pass, std::vector<std::vector<int>> lines,std::vector<std::vector<QString>> errors)
    {
         mut_errors.lock();
        m_pass = pass;
        m_lines =lines;
        m_errors = errors;
         mut_errors.unlock();
        emit int_Shader_error();
    }

signals:

    inline int_Shader_error();



public slots:

    inline void int_OnShaderCompileError()
    {
         mut_errors.lock();
        for(int i = 0; i < m_pass.size(); i++)
        {
            int currentindex = m_pass.at(i);
            if(currentindex > -1 && currentindex < m_ScriptTabs->count())
            {
                ShaderPassEditor * spe = (ShaderPassEditor *)(m_ScriptTabs->widget(i));
                spe->SetErrors(m_lines.at(i),m_errors.at(i));
            }
        }
         mut_errors.unlock();
    }

    inline void OnRequestRunCode()
    {
        std::vector<QString> shaders;
        for(int i = 0; i < m_ScriptTabs->count(); i++)
        {
            shaders.push_back(((ShaderPassEditor*)(m_ScriptTabs->widget(i)))->GetShaderText());
        }

        std::vector<std::vector<QString>> texts;
        std::vector<std::vector<cTMap *>> maps;

        for(int i = 0; i < m_ScriptTabs->count(); i++)
        {
            texts.push_back(((ShaderPassEditor*)(m_ScriptTabs->widget(i)))->GetImageFiles());
            maps.push_back(((ShaderPassEditor*)(m_ScriptTabs->widget(i)))->GetImageMaps());
        }


        m_CallBackUpdate(shaders,texts,maps);

    }

    inline void OnRequestPauseCode()
    {



    }

    inline void OnRequestStopCode()
    {



    }

    inline void  ResetTabNames()
    {

        for(int i = 0; i < m_ScriptTabs->count(); i++)
        {
            int index = m_ScriptTabs->count()- 1 - i;
            m_ScriptTabs->setTabText(i,QString("Pass ") + m_PassNames[index]);
        }


    }

    inline void OnRequestAddCode()
    {
        if( m_ScriptTabs->count() > m_MaxPasses)
        {
            return;
        }

        QString basicstring = QString("void main(){\n")
        + "\n"
        + "    // Time varying pixel color, shadertoy style\n"
        + "    vec3 col = 0.5 + 0.5*cos(iTime+texcoord.xyx+vec3(0,2,4));\n"
        + "\n"
        + "    fragColor = vec4(col.xyz,1.0);\n"
        + "}\n";

        int currentindex = m_ScriptTabs->currentIndex();
        if(currentindex > -1 && currentindex < m_ScriptTabs->count())
        {
            ShaderPassEditor *spe = new ShaderPassEditor();
            spe->SetShaderText(basicstring);
            m_ScriptTabs->insertTab(currentindex,spe, "Pass ");


        }else
        {
            ShaderPassEditor *spe = new ShaderPassEditor();
            spe->SetShaderText(basicstring);
            m_ScriptTabs->insertTab(0,spe, "Pass ");
        }

        ResetTabNames();


    }

    inline void OnRequestRemoveCode()
    {
        //never remove the last one
        if(m_ScriptTabs->count() == 1)
        {
            return;
        }

        int currentindex = m_ScriptTabs->currentIndex();
        if(currentindex > -1 && currentindex < m_ScriptTabs->count())
        {
            m_ScriptTabs->removeTab(currentindex);
        }

        ResetTabNames();

    }

    inline void OnRequestRefreshCode()
    {

        ResetTabNames();

    }

    inline void OnRequestSaveCode()
    {

        ResetTabNames();

    }

    inline void OnRequestLoadCode()
    {

        ResetTabNames();

    }



};
#endif // SHADEREDITOR_H
