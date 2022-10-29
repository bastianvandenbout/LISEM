#ifndef UIHELPER_H
#define UIHELPER_H

#include <qwidget.h>
#include "QString"
#include "QApplication"
#include "QMessageBox"
#include "QFileInfo"
#include "QProcess"
#include "QTextStream"
#include "QPlainTextEdit"
#include "QSettings"
#include "QDir"
#include "iostream"

namespace Minimap::Internal::Constants {

constexpr const char MINIMAP_SETTINGS[] = "MinimapSettings";
constexpr const char MINIMAP_STYLE_OBJECT_PROPERTY[] = "Minimap.StyleObject";
constexpr const int MINIMAP_WIDTH_DEFAULT = 80;
constexpr const int MINIMAP_EXTRA_AREA_WIDTH = 7;
constexpr const int MINIMAP_MAX_LINE_COUNT_DEFAULT = 8000;
constexpr const int MINIMAP_ALPHA_DEFAULT = 32;

}

inline static void ShowTextfile(QString name, QWidget* parent = nullptr)
{

    std::cout << "open file for view/edit " << name.toStdString() << std::endl;
    QFile file(name);
    if (!file.open(QFile::ReadWrite | QFile::Text))
    {
        QMessageBox::warning(parent, QString("LISEM"),
                             QString("Cannot read file %1:\n%2.")
                             .arg(name)
                             .arg(file.errorString()));
        return;
    }

    QString modifiedContents;
    QTextStream in(&file);

    QString all = in.readAll();

    std::cout << "opened: " << all.toStdString() << std::endl;

    QPlainTextEdit *view = new QPlainTextEdit(all);
    view->setWindowTitle(name);
    view->setMinimumWidth(400);
    view->setMinimumHeight(500);
    view->setAttribute(Qt::WA_DeleteOnClose);
    view->show();
    if (view->document()->isModified())
    {
        int ret =
                QMessageBox::question(parent, QString("LISEM"),
                                      QString("You have modified the contents of this file.\n"
                                              "Do you want to save it?"),
                                      QMessageBox::Ok |QMessageBox::Cancel,QMessageBox::Cancel);
        if (ret == QMessageBox::Ok)
        {
            // Don't take the address of a temporary!
            // in.setString(&view->toPlainText());
            modifiedContents = view->toPlainText();
            in.setString(&modifiedContents);
        }
    }

    file.close();
}

#ifdef Q_OS_WIN
#define QTC_HOST_EXE_SUFFIX QTC_WIN_EXE_SUFFIX
#else
#define QTC_HOST_EXE_SUFFIX ""
#endif // Q_OS_WIN

class UnixUtils
{
public:
    static QString defaultFileBrowser()
    {
        return QLatin1String("xdg-open %d");
    }
    static QString fileBrowser(const QSettings *settings = nullptr)
    {
        const QString dflt = defaultFileBrowser();
                return dflt;
    }
    static QString substituteFileBrowserParameters(const QString &pre,
                                                   const QString &file)
    {

        QFileInfo fileInfo(file);
        QString filename(fileInfo.fileName());
        {
            QString cmd;
            for (int i = 0; i < pre.size(); ++i) {
                QChar c = pre.at(i);
                if (c == QLatin1Char('%') && i < pre.size()-1) {
                    c = pre.at(++i);
                    QString s;
                    if (c == QLatin1Char('d')) {
                        s = QLatin1Char('"') + QFileInfo(file).path() + QLatin1Char('"');
                    } else if (c == QLatin1Char('f')) {
                        s = QLatin1Char('"') + file + QLatin1Char('"');
                    } else if (c == QLatin1Char('n')) {
                        s = QLatin1Char('"') + filename + QLatin1Char('"');
                    } else if (c == QLatin1Char('%')) {
                        s = c;
                    } else {
                        s = QLatin1Char('%');
                        s += c;
                    }
                    cmd += s;
                    continue;

                }
                cmd += c;
            }

            return cmd;
        }
    }
};

#define LSM_OsTypeWindows 1
#define LSM_OsTypeLinux 2
#define LSM_OsTypeMac 3
#define LSM_OsTypeOtherUnix 4
#define LSM_OsTypeOther 5

static constexpr int hostOs()
{
#if defined(Q_OS_WIN)
    return LSM_OsTypeWindows;
#elif defined(Q_OS_LINUX)
    return LSM_OsTypeLinux;
#elif defined(Q_OS_MAC)
    return LSM_OsTypeMac;
#elif defined(Q_OS_UNIX)
    return LSM_OsTypeOtherUnix;
#else
    return LSM_OsTypeOther;
#endif
}

enum HostArchitecture { HostArchitectureX86, HostArchitectureAMD64, HostArchitectureItanium,
                        HostArchitectureArm, HostArchitectureArm64, HostArchitectureUnknown };
static HostArchitecture hostArchitecture();

static constexpr bool isWindowsHost() { return hostOs() == LSM_OsTypeWindows; }
static constexpr bool isLinuxHost() { return hostOs() == LSM_OsTypeLinux; }
static constexpr bool isMacHost() { return hostOs() == LSM_OsTypeMac; }
static constexpr bool isAnyUnixHost()
{
#ifdef Q_OS_UNIX
        return true;
#else
        return false;
#endif
    }


inline static void showInGraphicalShell(QWidget *parent, const QString &pathIn)
{
    const QFileInfo fileInfo(pathIn);
    // Mac, Windows support folder or file.
    if (isWindowsHost()) {
        /*const FileName explorer = Environment::systemEnvironment().searchInPath(QLatin1String("explorer.exe"));
        if (explorer.isEmpty()) {
            QMessageBox::warning(parent,
                                 QApplication::translate("Core::Internal",
                                                         "Launching Windows Explorer Failed"),
                                 QApplication::translate("Core::Internal",
                                                         "Could not find explorer.exe in path to launch Windows Explorer."));
            return;
        }*/
        QStringList param;
        if (!fileInfo.isDir())
            param += QLatin1String("/select,");
        param += QDir::toNativeSeparators(fileInfo.canonicalFilePath());
        QProcess::startDetached("explorer", param);
    } else if (isMacHost()) {
        QStringList scriptArgs;
        scriptArgs << QLatin1String("-e")
                   << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"")
                                         .arg(fileInfo.canonicalFilePath());
        QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
        scriptArgs.clear();
        scriptArgs << QLatin1String("-e")
                   << QLatin1String("tell application \"Finder\" to activate");
        QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
    } else {
        // we cannot select a file here, because no file browser really supports it...
        const QString folder = fileInfo.isDir() ? fileInfo.absoluteFilePath() : fileInfo.filePath();
        const QString app = UnixUtils::fileBrowser(nullptr);
        QProcess browserProc;
        const QString browserArgs = UnixUtils::substituteFileBrowserParameters(app, folder);
        bool success = browserProc.startDetached(browserArgs);
        const QString error = QString::fromLocal8Bit(browserProc.readAllStandardError());
        success = success && error.isEmpty();
        //if (!success)
        //    showGraphicalShellError(parent, app, error);
    }
}

























#endif // UIHELPER_H
