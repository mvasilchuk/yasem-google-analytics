#ifndef GAPLUGIN_H
#define GAPLUGIN_H

#include "ga_global.h"
#include "plugin.h"

#include <QObject>

namespace yasem {

class GASHARED_EXPORT GoogleAnalytics: public SDK::Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.mvas.yasem.GoogleAnalyticsPlugin/1.0" FILE "metadata.json")
    Q_INTERFACES(yasem::SDK::Plugin)

    Q_CLASSINFO("author", "Maxim Vasilchuk")
    Q_CLASSINFO("description", "Google Analytics")
    Q_CLASSINFO("version", MODULE_VERSION)
    Q_CLASSINFO("revision", GIT_VERSION)
public:
    GoogleAnalytics(QObject* parent = 0);
    virtual ~GoogleAnalytics();

    // Plugin interface
public:
    void register_dependencies();
    void register_roles();
};

}

#endif // GAPLUGIN_H
