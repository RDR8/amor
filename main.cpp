//---------------------------------------------------------------------------
//
// main.cpp
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//
#include <stdlib.h>
#include <time.h>
#include <kconfig.h>
#include "amor.h"
#include <kapp.h>
#include <kwinmodule.h>

int main(int argc, char *argv[])
{
    KApplication app(argc, argv, "amor");

    srandom(time(0));

    // session management
    AmorSessionWidget *sessionWidget = new AmorSessionWidget;
    app.setTopWidget(sessionWidget);

#warning fix session management    
#if 0    
    app.enableSessionManagement(true);
    app.setWmCommand(argv[0]);
#endif    

    KWinModule *winModule = new KWinModule;

    Amor *amor = new Amor();
    QObject::connect(winModule, SIGNAL(windowActivate(WId)),
                    amor,     SLOT(slotWindowActivate(WId)));
    QObject::connect(winModule, SIGNAL(windowRemove(WId)),
                    amor,     SLOT(slotWindowRemove(WId)));
    QObject::connect(winModule, SIGNAL(stackingOrderChanged()),
                    amor,     SLOT(slotStackingChanged()));
    QObject::connect(winModule, SIGNAL(windowChange(WId)),
                    amor,     SLOT(slotWindowChange(WId)));

	  return app.exec();
}

