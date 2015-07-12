#ifndef GAOBJECT_H
#define GAOBJECT_H

#include "abstractpluginobject.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QSettings>
#include <QUrlQuery>
#include <QSize>

namespace yasem {

namespace SDK {
class Profile;
}

class GAObject: public SDK::AbstractPluginObject
{
    Q_OBJECT
public:
    explicit GAObject(SDK::Plugin* plugin);
    virtual ~GAObject();

    // AbstractPluginObject interface
public:
    SDK::PluginObjectResult init();
    SDK::PluginObjectResult deinit();


public slots:
    void startSession();
    void stopSession();

    QUrlQuery makeBaseQuery();
    void sendData(const QUrlQuery& query, bool close = false);
protected:
    QSettings* m_settings;
    QNetworkAccessManager* m_net_acc_manager;
    QString m_client_id;
    QString m_user_id;
    QString m_tracking_id;
    QUrl m_ga_collect_url;
    QString m_user_locale;
    QString m_core_version;
    QSize m_screen_size;

    QString m_user_agent;

    QString getUserAgent() const;
    QString getHash(const QString& str) const;
#ifdef Q_OS_WIN
    QString windowsVersionForUAString();
#endif
protected slots:
    void onGaFinished(QNetworkReply *reply);
    void onProfileLoaded(SDK::Profile* profile);
};

}

#endif // GAOBJECT_H
