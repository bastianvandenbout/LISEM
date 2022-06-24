#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "defines.h"
#include "lsmio.h"
#include "qicon.h"
#include "QList"
#include "site.h"

#define LISEM_ICON_SAVE 0
#define LISEM_ICON_LOAD 1
#define LISEM_ICON_SAVEAS 2
#define LISEM_ICON_SEARCH 3
#define LISEM_ICON_NEW 4
#define LISEM_ICON_ADDENV 5
#define LISEM_ICON_ADDPC 6
#define LISEM_ICON_DELETE 7
#define LISEM_ICON_BIN 8
#define LISEM_ICON_ADDRASTER 9
#define LISEM_ICON_ADDVECTOR 10
#define LISEM_ICON_ADDWEB 11
#define LISEM_ICON_ADVANCED 12
#define LISEM_ICON_RASTER 13
#define LISEM_ICON_NEWDIR 14
#define LISEM_ICON_DISPLAY 15
#define LISEM_ICON_FONTBIGGER 16
#define LISEM_ICON_FONTSMALLER 17
#define LISEM_ICON_FONTSELECT 18
#define LISEM_ICON_DOWNLOAD 19
#define LISEM_ICON_CANCEL 20
#define LISEM_ICON_WARNING 21
#define LISEM_ICON_COPY 22
#define LISEM_ICON_LOADER1 23
#define LISEM_ICON_LOADER2 24
#define LISEM_ICON_REPORT 25
#define LISEM_ICON_INFO 26
#define LISEM_ICON_TEXTFILE 27
#define LISEM_ICON_START 28
#define LISEM_ICON_PAUSE 29
#define LISEM_ICON_STOP 30
#define LISEM_ICON_PROFILE 31
#define LISEM_ICON_RASTERNEW 32
#define LISEM_ICON_REFRESH 33
#define LISEM_ICON_SCREENSHOT 34
#define LISEM_ICON_SIMPLE 35
#define LISEM_ICON_VECTOR 36
#define LISEM_ICON_UNDO 37
#define LISEM_ICON_ZOOMBEST 38
#define LISEM_ICON_ZOOMFIT 39
#define LISEM_ICON_TIMEPLOT 40
#define LISEM_ICON_POINTCLOUD 41
#define LISEM_ICON_TABLE 42
#define LISEM_ICON_TABLEADD 43
#define LISEM_ICON_TABLEADDROW 44
#define LISEM_ICON_TABLEDATATYPE 45
#define LISEM_ICON_TABLEREMOVE 46
#define LISEM_ICON_TABLEREMOVEROW 47
#define LISEM_ICON_TABLERESTORE 48
#define LISEM_ICON_TABLESEARCH 49
#define LISEM_ICON_TABLESIZE 50
#define LISEM_ICON_TABLESORTASC 51
#define LISEM_ICON_TABLESORTDESC 52
#define LISEM_ICON_TABLETITLE 53
#define LISEM_ICON_TABLETRANSPOSE 54
#define LISEM_ICON_TABLEUNLINK 55
#define LISEM_ICON_DEBUG 56
#define LISEM_ICON_RELOAD 57
#define LISEM_ICON_MESH 58
#define LISEM_ICON_MESHADD 59
#define LISEM_ICON_BOXEXTEND 60
#define LISEM_ICON_BOXCOLLAPSE 61
#define LISEM_ICON_RELOADLIGHT 62
#define LISEM_ICON_FIELD 63
#define LISEM_ICON_ADDFIELD 64
#define LISEM_ICON_CLEAR 65
#define LISEM_ICON_TEXTINDENT 66
#define LISEM_ICON_TEXTINDENTR 67
#define LISEM_ICON_TEXTSTYLE 68
#define LISEM_ICON_TEXTCOMMENT 69
#define LISEM_ICON_TEXTREGULAR 70
#define LISEM_ICON_COMPILE 71
#define LISEM_ICON_ADD 72
#define LISEM_ICON_REMOVE 73
#define LISEM_ICON_COUNT 74

class SPHResourceManager
{

    QList<int> iconids;
    QList<QString> iconname;
    QList<QIcon *> icons;

    QString m_Dir;
public:

    inline SPHResourceManager()
    {
        m_Dir = GetSite();

        for(int i = 0; i < LISEM_ICON_COUNT; i++)
        {
            GetIcon(GetDefaultIconName(i),i);
        }
    }

    inline void Destroy()
    {

        for(int i = 0; i < icons.length(); i++)
        {
            delete icons.at(i);
        }

        icons.clear();
        iconids.clear();
        iconname.clear();
    }
private:
public:

    inline QString GetDefaultIconName(int icon)
    {
        switch(icon)
        {
            case LISEM_ICON_SAVE: return "filesave.png";break;
            case LISEM_ICON_LOAD: return "fileopen.png";break;
            case LISEM_ICON_SAVEAS: return "filesaveas.png";break;
            case LISEM_ICON_SEARCH: return "search.png";break;
            case LISEM_ICON_NEW: return "new.png";break;
            case LISEM_ICON_ADDENV: return "addenvironment.png";break;
            case LISEM_ICON_ADDPC: return "addpointcloud.png";break;
            case LISEM_ICON_DELETE: return "delete.png";break;
            case LISEM_ICON_BIN: return "bin.png";break;
            case LISEM_ICON_ADDRASTER: return "addraster.png";break;
            case LISEM_ICON_ADDVECTOR: return "addvector.png";break;
            case LISEM_ICON_ADDWEB: return "addwev.png";break;
            case LISEM_ICON_ADVANCED: return "advanced.png";break;
            case LISEM_ICON_RASTER: return "aguila2d.png";break;
            case LISEM_ICON_NEWDIR: return "dirnew.png";break;
            case LISEM_ICON_DISPLAY: return "display.png";break;
            case LISEM_ICON_FONTBIGGER: return "fontsmaller.png";break;
            case LISEM_ICON_FONTSMALLER: return "fontbigger.png";break;
            case LISEM_ICON_FONTSELECT: return "fontselect.png";break;
            case LISEM_ICON_DOWNLOAD: return "download.png";break;
            case LISEM_ICON_CANCEL: return "cancel.png";break;
            case LISEM_ICON_WARNING: return "warning.png";break;
            case LISEM_ICON_COPY: return "copy.png";break;
            case LISEM_ICON_LOADER1: return "loader_1.png";break;
            case LISEM_ICON_LOADER2: return "loader_2.png";break;
            case LISEM_ICON_REPORT: return "report.png";break;
            case LISEM_ICON_INFO: return "Info.png";break;
            case LISEM_ICON_TEXTFILE: return "onewfile.png";break;
            case LISEM_ICON_START: return "start1.png";break;
            case LISEM_ICON_PAUSE: return "pause2.png";break;
            case LISEM_ICON_STOP: return "stop1.png";break;
            case LISEM_ICON_PROFILE: return "profile.png";break;
            case LISEM_ICON_RASTERNEW: return "rasternew.png";break;
            case LISEM_ICON_REFRESH: return "refresh.png";break;
            case LISEM_ICON_SCREENSHOT: return "screenshots.png";break;
            case LISEM_ICON_SIMPLE: return "simple.png";break;
            case LISEM_ICON_VECTOR: return "vector.png";break;
            case LISEM_ICON_UNDO: return "Undo-icon.png";break;
            case LISEM_ICON_ZOOMBEST: return "zoom-fit-best.png";break;
            case LISEM_ICON_ZOOMFIT: return "zoom-original.png";break;
            case LISEM_ICON_TIMEPLOT: return "timeplot.png";break;
            case LISEM_ICON_POINTCLOUD: return "pointcloud.png";break;
            case LISEM_ICON_TABLE: return "table.png";break;
            case LISEM_ICON_TABLEADD: return "table_add.png";break;
            case LISEM_ICON_TABLEADDROW: return "table_addrow.png";break;
            case LISEM_ICON_TABLEDATATYPE: return "table_datatype.png";break;
            case LISEM_ICON_TABLEREMOVE: return "table_remove.png";break;
            case LISEM_ICON_TABLEREMOVEROW: return "table_removerow.png";break;
            case LISEM_ICON_TABLERESTORE: return "table_restoresort.png";break;
            case LISEM_ICON_TABLESEARCH: return "table_search.png";break;
            case LISEM_ICON_TABLESIZE: return "table_setsize.png";break;
            case LISEM_ICON_TABLESORTASC: return "table_sortasc.png";break;
            case LISEM_ICON_TABLESORTDESC: return "table_sortdesc.png";break;
            case LISEM_ICON_TABLETITLE: return "table_title.png";break;
            case LISEM_ICON_TABLETRANSPOSE: return "table_transpose.png";break;
            case LISEM_ICON_TABLEUNLINK: return "table_unlink.png";break;
            case LISEM_ICON_DEBUG: return "debug.png";break;
            case LISEM_ICON_RELOAD: return "reload.png";break;
            case LISEM_ICON_MESH: return "mesh.png";break;
            case LISEM_ICON_MESHADD: return "addmesh.png";break;
            case LISEM_ICON_BOXEXTEND: return "extend.png";break;
            case LISEM_ICON_BOXCOLLAPSE: return "collapse.png";break;
            case LISEM_ICON_RELOADLIGHT: return "reload_light.png";break;
            case LISEM_ICON_FIELD: return "field.png";break;
            case LISEM_ICON_ADDFIELD: return "addfield.png";break;
            case LISEM_ICON_CLEAR: return "clear.png";break;
            case LISEM_ICON_TEXTINDENT: return "script_indent.png";break;
            case LISEM_ICON_TEXTINDENTR: return "script_indent_r.png";break;
            case LISEM_ICON_TEXTSTYLE: return "script_style.png";break;
            case LISEM_ICON_TEXTCOMMENT: return "script_comment.png";break;
            case LISEM_ICON_TEXTREGULAR: return "script_normal.png";break;
            case LISEM_ICON_COMPILE: return "compile.png";break;
            case LISEM_ICON_ADD: return "add.png";break;
            case LISEM_ICON_REMOVE: return "remove.png";break;
        }

        return QString();
    }

    inline QIcon * GetIcon(QString filename, int defaultid)
    {
        QIcon * i = nullptr;

        if(defaultid > -1)
        {
            i = new QIcon();
            i->addFile(m_Dir + LISEM_FOLDER_ASSETS + filename);

            iconids.append(defaultid);
            iconname.append(filename);
            icons.append(i);
        }else
        {
            //check if already present
            int index = iconname.indexOf(filename);
            if(index > -1)
            {
                i = icons.at(index);
            }else
            {
                i = new QIcon();
                i->addFile(m_Dir + LISEM_FOLDER_ASSETS + filename);

                iconids.append(-1);
                iconname.append(filename);
                icons.append(i);
            }
        }

        return i;

    }
public:
    inline QIcon * GetIcon(QString filename)
    {
        return GetIcon(filename,-1);
    }
};


LISEM_API extern SPHResourceManager * CGlobalResourceManager;


inline static void InitializeResourceManager()
{
    CGlobalResourceManager = new SPHResourceManager();
}


inline static SPHResourceManager *GetResourceManager()
{
    return CGlobalResourceManager;
}

inline static void CloseResourceManager()
{
    CGlobalResourceManager->Destroy();
    delete CGlobalResourceManager;
}




#endif // RESOURCEMANAGER_H
