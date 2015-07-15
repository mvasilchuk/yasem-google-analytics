#include "gaobject.h"
#include "macros.h"
#include "core.h"
#include "profilemanager.h"
#include "stbprofile.h"

#include <QSettings>
#include <QCoreApplication>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QUrlQuery>
#include <QLocale>
#include <QScreen>
#include <QDesktopWidget>
#include <QApplication>
#include <QtWebKit/QtWebKitVersion>
#include <QSysInfo>

using namespace yasem;

GAObject::GAObject(SDK::Plugin* plugin):
    SDK::AbstractPluginObject(plugin),
    m_settings          (SDK::Core::instance()->settings()),
    m_net_acc_manager   (new QNetworkAccessManager(this)),
    m_client_id         (m_settings->value("installation_id", "-undefined-").toString()),
    m_user_id           (getHash(m_settings->value("registration_data/user_name", "anonymous").toString())),
    m_tracking_id       ("UA-65033972-1"),
    m_ga_collect_url    ("http://www.google-analytics.com/collect"),
    m_user_locale       (QLocale::system().bcp47Name()),
    m_core_version      (SDK::Core::instance()->version()),
    m_screen_size       (QApplication::desktop()->screenGeometry().size()),
    m_user_agent        (getUserAgent())
{
    connect(m_net_acc_manager, &QNetworkAccessManager::finished, this, &GAObject::onGaFinished);
    connect(SDK::ProfileManager::instance(), &SDK::ProfileManager::profileChanged, this, &GAObject::onProfileLoaded);
    DEBUG() << "[UA]" << m_user_agent;
}

GAObject::~GAObject()
{

}

SDK::PluginObjectResult GAObject::init()
{
    startSession();
    return SDK::PLUGIN_OBJECT_RESULT_OK;
}

SDK::PluginObjectResult GAObject::deinit()
{
    //stopSession();
    return SDK::PLUGIN_OBJECT_RESULT_OK;
}

void GAObject::startSession()
{
    QUrlQuery query = makeBaseQuery();
    query.addQueryItem("sc", "start");

    // Events
    query.addQueryItem("t", "event");
    query.addQueryItem("ec", "App");
    query.addQueryItem("ea", "Start");
    query.addQueryItem("el", "App started");
    sendData(query);
}

/**
 * @brief GAObject::stopSession
 * TODO: Fix this method, because it doesn't work correctly. When this method is called the app is closing so connection may be terninated.
 */
void GAObject::stopSession()
{
    QUrlQuery query = makeBaseQuery();
    query.addQueryItem("sc", "end");

    query.addQueryItem("t", "event");
    query.addQueryItem("ec", "App");
    query.addQueryItem("ea", "Stop");
    query.addQueryItem("el", "App stopped");

    sendData(query);
}

QUrlQuery GAObject::makeBaseQuery()
{
    QUrlQuery query;
    query.addQueryItem("v", "1"); // Version
    query.addQueryItem("tid", m_tracking_id); // Tracking ID - use value assigned to you by Google Analytics
    query.addQueryItem("cid", m_client_id); // Client ID
    query.addQueryItem("uid", m_user_id);
    query.addQueryItem("ds", "app");

    //Locale
    query.addQueryItem("ul", m_user_locale);

    //App info
    query.addQueryItem("an", "yasem");
    query.addQueryItem("av", m_core_version);

    //Screen info
    query.addQueryItem("sr", QString("%1x%2").arg(m_screen_size.width()).arg(m_screen_size.height()));

    // User agent
    query.addQueryItem("ua", m_user_agent);

    return query;
}

void GAObject::sendData(const QUrlQuery &query, bool close)
{
    DEBUG() << "Sending statistics report to Google Analytics";

    QNetworkRequest req(m_ga_collect_url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QByteArray data;
    data.append(query.query());
    DEBUG() << data; // Output for debug purposes.
    m_net_acc_manager->post(req, data);
    DEBUG() << "Report has been sent";
}

/**
 * @brief GAObject::getUserAgent
 * This code was copy/pasted from qtwebkit/Source/WebCore/platform/qt/UserAgentQt.cpp
 * and modified
 *
 * @return
 */
QString GAObject::getUserAgent() const
{
    static QString ua;

    if (ua.isNull()) {

        ua = QLatin1String("Mozilla/5.0 (%1%2%3) AppleWebKit/%4 (KHTML, like Gecko) %99 Safari/%5");

        // Platform.
        ua = ua.arg(QLatin1String(
#ifdef Q_OS_DARWIN
            "Macintosh; "
#elif defined(Q_OS_WIN)
            ""
#else
            (QGuiApplication::platformName() == QLatin1String("xcb")) ? "X11; " : "Unknown; "
#endif
        ));


        // Security strength.
        QString securityStrength;
#if defined(QT_NO_OPENSSL)
        securityStrength = QLatin1String("N; ");
#endif
        ua = ua.arg(securityStrength);

        // Operating system.
        ua = ua.arg(QLatin1String(

#ifdef Q_OS_WIN
        windowsVersionForUAString().toUtf8().constData()
#elif defined(Q_OS_DARWIN)
    #if defined(Q_PROCESSOR_X86_32) || defined(Q_PROCESSOR_X86_64)
                "Intel Mac OS X"
    #else
                "PPC Mac OS X"
    #endif

#elif defined(Q_OS_FREEBSD)
            "FreeBSD"
#elif defined(Q_OS_HURD)
            "GNU Hurd"
#elif defined(Q_OS_LINUX)
    #if defined(Q_PROCESSOR_X86_64)
                "Linux x86_64"
    #elif defined(Q_PROCESSOR_X86_32)
                "Linux i686"
    #else
                "Linux"
    #endif
#elif defined(Q_OS_NETBSD)
            "NetBSD"
#elif defined(Q_OS_OPENBSD)
            "OpenBSD"
#elif defined(Q_OS_QNX)
            "QNX"
#elif defined(Q_OS_SOLARIS)
            "Sun Solaris"
#elif defined(Q_OS_UNIX) // FIXME Looks like all unix variants above are the only cases where OS_UNIX is set.
            "UNIX BSD/SYSV system"
#else
            "Unknown"
#endif
        ));

        // WebKit version.
        ua = ua.arg(QTWEBKIT_VERSION_STR, QTWEBKIT_VERSION_STR);
    }

    QString appName;
    /*
    if (applicationNameForUserAgent.isEmpty())
        appName = QCoreApplication::applicationName();
    else
        appName = applicationNameForUserAgent;

    if (!appName.isEmpty()) {
        QString appVer = QCoreApplication::applicationVersion();
        if (!appVer.isEmpty())
            appName.append(QLatin1Char('/') + appVer);
    } else {
        // Qt version.
        appName = QLatin1String("Qt/") + QLatin1String(qVersion());
    }*/

    return ua.arg(appName);
}

QString GAObject::getHash(const QString &str) const
{
    return QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Sha256).toHex();
}

void GAObject::onGaFinished(QNetworkReply *reply)
{
    Q_UNUSED(reply);
}

void GAObject::onProfileLoaded(SDK::Profile* profile)
{
    Q_UNUSED(profile);
    QUrlQuery query = makeBaseQuery();

    // Events
    query.addQueryItem("t", "event");
    query.addQueryItem("ec", "Profile");
    query.addQueryItem("ea", "Loaded");
    query.addQueryItem("el", "Profile loaded");
    sendData(query);
}

#ifdef Q_OS_WIN
QString GAObject::windowsVersionForUAString()
{
    QString os;
    switch (QSysInfo::windowsVersion())
    {
        case QSysInfo::WV_CE:
        case QSysInfo::WV_CE_5:
        case QSysInfo::WV_CE_6:         os = "Windows CE"; break;
        case QSysInfo::WV_CENET:        os = "Windows CE .NET"; break;
        case QSysInfo::WV_32s:          os = "Windows 3.1";break;
        case QSysInfo::WV_95:           os = "Windows 95";break;
        case QSysInfo::WV_98:           os = "Windows 98"; break;
        case QSysInfo::WV_Me:           os = "Windows 98; Win 9x 4.90"; break;
        case QSysInfo::WV_NT:           os = "WinNT4.0"; break;
        case QSysInfo::WV_2000:         os = "Windows NT 5.0"; break;
        case QSysInfo::WV_XP:           os = "Windows NT 5.1"; break;
        case QSysInfo::WV_2003:         os = "Windows NT 5.2"; break;
        case QSysInfo::WV_VISTA:        os = "Windows NT 6.0"; break;
        case QSysInfo::WV_WINDOWS7:     os = "Windows NT 6.1"; break;
        case QSysInfo::WV_WINDOWS8:     os = "Windows NT 6.2"; break;
#if (QT_VERSION >= 0x05020)
        case QSysInfo::WV_WINDOWS8_1:   os = "Windows NT 6.3"; break;
#endif
#if (QT_VERSION >= 0x05040)
        case QSysInfo::WV_WINDOWS10:    os = "Windows NT 10.0"; break;
#endif
        default: {
            os = QString("Windows NT %1").arg(QSysInfo::kernelVersion());
            break;
        }
    }

    QString cpu_arch = QSysInfo::currentCpuArchitecture();
    QString arch_line = "";
    if(cpu_arch == "x86_64")
    {
        arch_line = "; Win64; x64";
    }

    return os + arch_line;
}
#endif
