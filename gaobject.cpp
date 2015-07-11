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

using namespace yasem;

GAObject::GAObject(SDK::Plugin* plugin):
    SDK::AbstractPluginObject(plugin),
    m_settings(SDK::Core::instance()->settings()),
    m_net_acc_manager(new QNetworkAccessManager(this)),
    m_client_id(m_settings->value("installation_id", "-undefined-").toString()),
    m_user_id(m_settings->value("registration_data/user_name", "anonymous").toString()),
    m_tracking_id("UA-65033972-1")
{
    connect(m_net_acc_manager, &QNetworkAccessManager::finished, this, &GAObject::onGaFinished);
    connect(SDK::ProfileManager::instance(), &SDK::ProfileManager::profileChanged, this, &GAObject::onProfileLoaded);
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
    stopSession();
    return SDK::PLUGIN_OBJECT_RESULT_OK;
}

void GAObject::startSession()
{
    QUrlQuery query = makeBaseQuery();
    query.addQueryItem("sc", "start");

    // Events
    query.addQueryItem("t", "event");
    query.addQueryItem("ec", "Unspecified");
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
    query.addQueryItem("ec", "Unspecified");
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
    query.addQueryItem("an", "yasem");
    query.addQueryItem("av", SDK::Core::instance()->version());

    query.addQueryItem("ul", QLocale::system().bcp47Name());
    query.addQueryItem("ds", "app");
    return query;
}

void GAObject::sendData(const QUrlQuery &query, bool close)
{
    DEBUG() << "Sending statistics report to Google Analytics";

    QNetworkRequest req(QUrl("http://www.google-analytics.com/collect"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QByteArray data;
    data.append(query.query());
    DEBUG() << data; // Output for debug purposes.
    m_net_acc_manager->post(req, data);
    DEBUG() << "Report has been sent";
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
