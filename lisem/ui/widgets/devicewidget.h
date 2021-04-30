#ifndef DEVICEWIDGET_H
#define DEVICEWIDGET_H

#define __CL_ENABLE_EXCEPTIONS
#include "cl.hpp"
#include "iostream"
#include "QWidget"
#include "QGroupBox"
#include "QHBoxLayout"
#include "QDialog"
#include "styles/sphstyle.h"
#include "gradientcombobox.h"
#include "layer/uilayer.h"
#include "QToolButton"
#include "QCheckBox"
#include "linestylecombobox.h"
#include "fillstylecombobox.h"
#include "colorwheel.h"
#include "QScrollArea"
#include "QToolBox"
#include "QSpacerItem"
#include "widgets/labeledwidget.h"
#include "QComboBox"
#include "openglclmanager.h"
#include "OpenCLUtil.h"
#include "QToolButton"
#include "settings/generalsettingsmanager.h"
#include "site.h"

typedef std::vector<cl::Platform>::iterator PlatformIter;


class DeviceWidget : public QDialog
{
    Q_OBJECT;

    QComboBox * m_DeviceCB;
    QGroupBox * m_MainWidget;
    QVBoxLayout * m_MainLayout;

    QList<cl::Platform> m_Platforms;
    QList<cl::Device> m_Devices;



    QGroupBox *m_DeviceInfoBox;
    QVBoxLayout *m_DeviceInfoLayout;
    QLabel *m_DeviceInfoText;
    QToolButton * m_ApplyButton;

    OpenGLCLManager * m_OpenGLCLManager;
public:
    inline DeviceWidget(OpenGLCLManager * m) : QDialog()
    {
        m_OpenGLCLManager = m;

        QString m_Dir = GetSite();

        QString preferred = GetSettingsManager()->GetSetting("PreferredGPUDevice");

        m_MainWidget = new QGroupBox("Compute Device Selection");
        m_MainLayout = new QVBoxLayout();

        m_MainWidget->setLayout(m_MainLayout);

        m_MainWidget->setMinimumSize(QSize(600,400));
        QVBoxLayout * vl = new QVBoxLayout();
        this->setLayout(vl);
        QScrollArea * sa = new QScrollArea();
        sa->setWidget(m_MainWidget);

        vl->addWidget(sa);

        m_DeviceCB = new QComboBox();
        m_DeviceInfoText = new QLabel();

        bool first = true;


        int index_cb = 0;
        int index_selected = 0;
        QString name_selected = "";

        //get list of all available devices and platforms
        cl::Platform ret_val;
        cl_int error = 0;
        try {
            // Get available platforms
            std::vector<cl::Platform> platforms;
            cl::Platform::get(&platforms);

            for(PlatformIter it=platforms.begin(); it<platforms.end(); ++it) {

                cl::Platform lPlatform = *it;

                std::string platformname = it->getInfo<CL_PLATFORM_NAME>();

                std::vector<cl::Device> devices;
                lPlatform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
                for (unsigned d=0; d<devices.size(); ++d) {

                    QString devicename = QString(devices[d].getInfo<CL_DEVICE_NAME>().c_str());
                    int computeunits = devices[d].getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
                    int computeclock = devices[d].getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>();
                    int computepars = devices[d].getInfo<CL_DEVICE_MAX_PARAMETER_SIZE>();
                    int computetype = devices[d].getInfo<CL_DEVICE_TYPE>();
                    QString clversion = QString(devices[d].getInfo<CL_DEVICE_VERSION>().c_str());
                    unsigned long long computemem = devices[d].getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>();
                    QString clcversion = QString(devices[d].getInfo<CL_DEVICE_OPENCL_C_VERSION>().c_str());

                    size_t maxwidth = devices[d].getInfo<CL_DEVICE_IMAGE2D_MAX_WIDTH>();
                    size_t maxheight = devices[d].getInfo<CL_DEVICE_IMAGE2D_MAX_HEIGHT>();

                    int unified = devices[d].getInfo<CL_DEVICE_HOST_UNIFIED_MEMORY>();

                    const char *typestr;

                    switch(computetype) {
                    case CL_DEVICE_TYPE_CPU:
                        typestr = "CPU";
                        break;
                    case CL_DEVICE_TYPE_ACCELERATOR:
                        typestr = "accelerator";
                        break;
                    case CL_DEVICE_TYPE_GPU:
                        typestr = "GPU";
                        break;
                    default:
                        typestr = "<unknown>";
                    }
                    bool interop = checkExtnAvailability(devices[d],CL_GL_SHARING_EXT);

                    m_Platforms.append(lPlatform);
                    m_Devices.append(devices[d]);
                    m_DeviceCB->addItem(QString(platformname.c_str()) + "  " + devicename);

                    if(first || (QString(platformname.c_str()) + "  " + devicename).compare(preferred) == 0)
                    {
                        SetInfoText(QString(platformname.c_str()) + "  " + devicename);
                        first = false;
                        index_selected = index_cb;
                        name_selected =QString(platformname.c_str()) + "  " + devicename;
                    }

                    index_cb ++;
                }
            }
        } catch(cl::Error err) {
            LISEM_ERROR(QString(err.what()) + "(" + QString(err.err()) + ")");
            error = err.err();
            LISEM_ERROR("Error during search for OpenCL platform");
            throw 1;
        }

        m_DeviceCB->setCurrentIndex(index_selected);

        m_MainLayout->addWidget(m_DeviceCB);

        connect(m_DeviceCB, SIGNAL(currentIndexChanged(int)),this, SLOT(OnIndexChanged(int)));

        m_DeviceInfoBox = new QGroupBox("Device Info");
        m_DeviceInfoLayout = new QVBoxLayout();
        m_DeviceInfoBox->setLayout(m_DeviceInfoLayout);
        m_MainLayout->addWidget(m_DeviceInfoBox);


        m_DeviceInfoLayout->addWidget(m_DeviceInfoText);

        m_ApplyButton = new QToolButton();
        m_ApplyButton->setText("Apply and Restart Application!");

        connect(m_ApplyButton, &QToolButton::clicked,this, &DeviceWidget::OnApplyClicked);
        m_MainLayout->addWidget(m_ApplyButton);

    }

    inline void SetInfoText(QString device)
    {
        m_DeviceInfoText->clear();
        QString infotext = "";

        cl::Platform ret_val;
        cl_int error = 0;
        try {
            // Get available platforms
            std::vector<cl::Platform> platforms;
            cl::Platform::get(&platforms);

            for(PlatformIter it=platforms.begin(); it<platforms.end(); ++it) {

                cl::Platform lPlatform = *it;

                std::string platformname = it->getInfo<CL_PLATFORM_NAME>();

                std::vector<cl::Device> devices;
                lPlatform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
                for (unsigned d=0; d<devices.size(); ++d) {

                    QString devicename = QString(devices[d].getInfo<CL_DEVICE_NAME>().c_str());

                    if((QString(platformname.c_str()) + "  " + devicename).compare(device) == 0)
                    {
                        int computeunits = devices[d].getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
                        int computeclock = devices[d].getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>();
                        int computepars = devices[d].getInfo<CL_DEVICE_MAX_PARAMETER_SIZE>();
                        int computetype = devices[d].getInfo<CL_DEVICE_TYPE>();
                        QString clversion = QString(devices[d].getInfo<CL_DEVICE_VERSION>().c_str());
                        unsigned long long computemem = devices[d].getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>();
                        QString clcversion = QString(devices[d].getInfo<CL_DEVICE_OPENCL_C_VERSION>().c_str());

                        size_t maxwidth = devices[d].getInfo<CL_DEVICE_IMAGE2D_MAX_WIDTH>();
                        size_t maxheight = devices[d].getInfo<CL_DEVICE_IMAGE2D_MAX_HEIGHT>();
                        int unified = devices[d].getInfo<CL_DEVICE_HOST_UNIFIED_MEMORY>();

                        const char *typestr;

                        switch(computetype) {
                        case CL_DEVICE_TYPE_CPU:
                            typestr = "CPU";
                            break;
                        case CL_DEVICE_TYPE_ACCELERATOR:
                            typestr = "accelerator";
                            break;
                        case CL_DEVICE_TYPE_GPU:
                            typestr = "GPU";
                            break;
                        default:
                            typestr = "<unknown>";
                        }
                        bool interop = checkExtnAvailability(devices[d],CL_GL_SHARING_EXT);

                        infotext.append(QString("Platform Name : ") + QString(platformname.c_str()));
                        infotext.append("\n");
                        infotext.append(QString("Device Name : ") + QString(devicename));
                        infotext.append("\n");
                        infotext.append(QString("Device Type : ") + QString(typestr));
                        infotext.append("\n");
                        infotext.append(QString("Compute Units : ") + QString::number(computeunits));
                        infotext.append("\n");
                        infotext.append(QString("Compute Clock : ") + QString::number(computeclock));
                        infotext.append("\n");
                        infotext.append(QString("Compute Mem : ") + QString::number(computemem/(1024*1024)) + " MB");
                        infotext.append("\n");
                        infotext.append(QString("OpenCL Version : ") + clversion);
                        infotext.append("\n");
                        infotext.append(QString("OpenCL C Version : ") + clcversion);
                        infotext.append("\n");
                        infotext.append(QString("Device Type : ") + QString(typestr));
                        infotext.append("\n");
                        infotext.append(QString("Dedicated : ") + QString(unified?"false":"true"));
                        infotext.append("\n");
                        infotext.append(QString("Map Max Width : ") + QString::number(maxwidth));
                        infotext.append("\n");
                        infotext.append(QString("Map Max Height : ") + QString::number(maxheight));
                        infotext.append("\n");

                        break;
                    }


                }
            }
        } catch(cl::Error err) {
        }

        m_DeviceInfoText->setText(infotext);

    }

    inline void GetOpenGLSupportVersion()
    {
        //glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        //GLFWwindow* offscreen_context = glfwCreateWindow(640, 480, "", NULL, NULL);
    }

    public slots:

    inline void OnIndexChanged(int i)
    {
        std::cout << "selected device " << m_DeviceCB->currentText().toStdString() << std::endl;
        SetInfoText(m_DeviceCB->currentText());
    }

    inline void OnApplyClicked(bool x)
    {
        GeneralSettingsManager * sm = GetSettingsManager();

        sm->SetSetting("PreferredGPUDevice",m_DeviceCB->currentText());

        m_OpenGLCLManager->Restart();

    }
};

#endif // DEVICEWIDGET_H
