#pragma once


#include "error.h"
#include "model.h"
#include <utility>

#include <QtCore/QString>

#include "pybind11/pybind11.h"
#include "pybind11/operators.h"

#include "error.h"
#include "ogr_api.h"
#include "ogrsf_frmts.h"
#include "gdal.h"
#include "downloadmanager.h"
#include "QPixmap"
#include "QSplashScreen"
#include "QMessageBox"
#include "resourcemanager.h"
#include "gl/openglcldatamanager.h"

#include "error.h"
#include "ogr_api.h"
#include "ogrsf_frmts.h"
#include "gdal.h"
#include "downloadmanager.h"
#include "QPixmap"
#include "QSplashScreen"
#include "QMessageBox"
#include "resourcemanager.h"
#include "settings/generalsettingsmanager.h"
#include "site.h"
#include "scriptmanager.h"
#include "glplot/worldwindow.h"
#include "linear/lsm_vector.h"
#include "linear/lsm_vector2.h"
#include "linear/lsm_vector3.h"
#include "linear/lsm_vector4.h"
#include "linear/lsm_matrix4x4.h"
#include "map"
namespace py = pybind11;

inline int add(int i, int j) {
    return i + j;
}

namespace py = pybind11;

PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

namespace pybind11
{
namespace detail
{
/* Create a TypeCaster for auto python string <--> QString conversion */
template <> struct type_caster<QString>
{
public:
  /**
* This macro establishes the name 'QString' in
* function signatures and declares a local variable
* 'value' of type QString
*/
  PYBIND11_TYPE_CASTER(QString, _("QString"));

  /**
*  @brief Conversion part 1 (Python->C++): convert a PyObject into a QString
* instance or return false upon failure. The second argument
* indicates whether implicit conversions should be applied.
* @param src
* @return boolean
*/
  bool load(handle src, bool)
  {
    if(!src)
    {
      return false;
    }
    object temp;
    handle load_src = src;
    if(PyUnicode_Check(load_src.ptr()))
    {
      temp = reinterpret_steal<object>(PyUnicode_AsUTF8String(load_src.ptr()));
      if(!temp) /* A UnicodeEncodeError occured */
      {
        PyErr_Clear();
        return false;
      }
      load_src = temp;
    }
    char* buffer = nullptr;
    ssize_t length = 0;
    int err = PYBIND11_BYTES_AS_STRING_AND_SIZE(load_src.ptr(), &buffer, &length);
    if(err == -1) /* A TypeError occured */
    {
      PyErr_Clear();
      return false;
    }
    value = QString::fromUtf8(buffer, static_cast<int>(length));
    return true;
  }

  /**
   * @brief Conversion part 2 (C++ -> Python): convert an QString instance into
   * a Python object. The second and third arguments are used to
   * indicate the return value policy and parent object (for
   * ``return_value_policy::reference_internal``) and are generally
   * ignored by implicit casters.
   *
   * @param src
   * @return
   */
  static handle cast(const QString& src, return_value_policy /* policy */, handle /* parent */)
  {
#if PY_VERSION_HEX >= 0x03030000 // Python 3.3
    assert(sizeof(QChar) == 2);
    return PyUnicode_FromKindAndData(PyUnicode_2BYTE_KIND, src.constData(), src.length());
#else
    QByteArray a = src.toUtf8();
    return PyUnicode_FromStringAndSize(a.data(), (ssize_t)a.length());
#endif
  }
};
}
}

static struct Data {
  int argc = 1;
  char *argv[2] = {strdup("dummy"), {}};
  QApplication app{argc, argv};
  bool has_opengl = false;
  OpenGLCLManager *m_OpenGLCLManager;
  ScriptManager * m_ScriptManager;
  WorldWindow * m_WorldWindow;
  LISEMModel * m_Model;
  ParameterManager * m_ParameterManager;

  std::map<int,std::function<void(bool, BoundingBox)>> m_ChannelCallbacks;

  bool m_ModelIsRunning = false;
  QMutex m_ModelIsRunningMutex;
} *d;

#ifdef OS_WIN
#include "windows.h"
#define MAX_PATH_LSM_DLL 16000
#endif

#include <stdio.h>
#include <stdlib.h>

static void init_pylisem_global(py::module &m)
{
    InitMessages();
    InitMessagesS();

    SetPrintUseStdOut(true);


    std::string DirS;

#ifdef OS_WIN
    char path[MAX_PATH_LSM_DLL];
    HMODULE hm = NULL;

    if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPCSTR) &init_pylisem_global, &hm) == 0)
    {
        int ret = GetLastError();
        fprintf(stderr, "GetModuleHandle failed, error = %d\n", ret);
        // Return or however you want to handle an error.
    }
    if (GetModuleFileName(hm, path, sizeof(path)) == 0)
    {
        int ret = GetLastError();
        fprintf(stderr, "GetModuleFileName failed, error = %d\n", ret);
        // Return or however you want to handle an error.
    }
    DirS = std::string(path);
    DirS.erase((DirS.size()-21),21);//32

#endif
#ifdef OS_LNX

    DirS = std::string("");

#endif


    QString Dir = QString(DirS.c_str());
    //std::ofstream out("C:/Data/output.txt");
    //out << Dir.toStdString();
    //out.close();

    std::cout << "dir = "  << Dir.toStdString()<< std::endl;
    SetSite(Dir);

    QCoreApplication::addLibraryPath(Dir);

    const char *buf = getenv("QT_PLUGIN_PATH");
    std::string env_path;

    env_path = Dir.toStdString();
    env_path += ";";
    if(buf != nullptr)
    {
        env_path = buf;

    }


#ifdef OS_WIN

    int e = _putenv_s("QT_PLUGIN_PATH",env_path.c_str());
    if (e) {
        exit(EXIT_FAILURE);
    }

#endif
#ifdef OS_LNX

#endif
    d = new Data;

    d->app.addLibraryPath(Dir);
    d->app.processEvents();



    InitializeGeneralSettingsManager(Dir);
    InitializeResourceManager();
    InitDownloadManager();

    PreInitializeGDAL(Dir);

    InitializeGDAL();


    QIcon icon(QIcon(Dir + "/assets/" + "logo.png"));

    QPixmap iconp = icon.pixmap(48,48);
    d->m_OpenGLCLManager = new OpenGLCLManager();
    int suc =  d->m_OpenGLCLManager->CreateGLWindow(iconp,false);

    if(suc > 0)
    {
        if(suc == 254)
        {
            std::cout << "Could not create OpenGL 4.0 context" << std::endl;
        }
    }else
    {
        suc =  d->m_OpenGLCLManager->InitGLCL();

        if(suc > 0)
        {
            d->has_opengl =true;
        }
    }
    InitUIShaders(d->m_OpenGLCLManager);

    d->m_ParameterManager = new ParameterManager();
    d->m_ParameterManager->InitParameters();
    d->m_Model = new LISEMModel(d->m_OpenGLCLManager,d->m_ParameterManager);
    d->m_Model->InitModel();
    SetGlobalModel(d->m_Model);

    d->m_WorldWindow  = new WorldWindow(d->m_OpenGLCLManager);


    //register core stuff

    py::register_exception<Exception>(m,"LisemException");
    m.doc() = "Lisem Integrated Spatial Earth Modeller"; // optional module docstring

    m.def("GetLastError", &GetLastErrorMessage, "A function returns a string containing the last produced error message by the LISEM library");



}

