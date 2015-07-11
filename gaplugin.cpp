#include "gaplugin.h"
#include "gaobject.h"

using namespace yasem;

GoogleAnalytics::GoogleAnalytics(QObject* parent):
    SDK::Plugin(parent)
{

}

GoogleAnalytics::~GoogleAnalytics()
{

}


void GoogleAnalytics::register_dependencies()
{
}

void GoogleAnalytics::register_roles()
{
    register_role(SDK::ROLE_UNSPECIFIED, new GAObject(this));
}
