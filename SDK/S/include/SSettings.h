/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#ifndef H_SSettings
#define H_SSettings

#include <CBDPersist.h>

#include "SSetting.h"

/*!
 * \brief Serves the dual purpose of being preferences and also context settings.
 *  
 * Preference
 *  
 * - this instantiated as needed (ie to support Preferences dialog) 
 * - default SSetting objects are added using doInsert() (SSettings takes ownership)
 * - supporting libraries are provided an opportunity to add more SSetting based objects
 * - defaults loaded by calling doLoad()
 * - can be edited using panel widgets where provided by SSetting. 
 * - any changes will trigger SSetting::signalChanged() (may not be relevant in this case)
 * - can be saved at anytime by calling doSave()
 * - this object no longer needed after Preferences completed 
 * * SSetting::signalChanged() may or may not be useful in this case 
 *  
 * Context 
 *  
 * - a single global is automatically instantiated (g_SSettings) 
 * - default SSetting objects are added using doInsert() (SSettings takes ownership) 
 * - supporting libraries are provided an opportunity to add more SSetting based objects
 * - defaults loaded by calling doLoad() 
 * - can be edited using; ToolBar, panel and dialogs as provided by SSetting 
 *   For example; a shape tool is selected and the panel for its SSetting (if any) may be
 *   shown in a dock window to allow settings to be edited
 * - any changes will trigger SSetting::signalChanged() which may be used to notify the shape
 * - calling doSave() will result in a NoOp - can not save Context  
 * - the global (g_SSettings) remains available until the app closes  
 *  
 * Key 
 *  
 * The key probvided to doInsert() must be unique. When in doubt use; [orgnization]:[product] 
 *  
 * \note Use QSettings for stuff that needs to be known during app start-up. 
 *       For example; whether to show a splash screen. 
 * 
 * \author pharvey (3/13/23)
 */
class SSettings : public QObject 
{
    Q_OBJECT
public:
     enum Mode
    {
        ModePreference,        /*!< used to save/read default/start-up values (for preferences dialog)         */
        ModeContext            /*!< reads defaults and then gets altered during app usage - but never saved    */
    };

    SSettings( Mode n = ModeContext );

    Mode        getMode() { return nMode; }
    SSetting *  getSetting( const QString & );

    void        doInsert( const QString stringKey, SSetting * );
    void        doRemove( const QString stringKey );

    void doSave();
    void doLoad();

protected:
    QString stringFileName;
    Mode nMode = ModeContext;
    QMap<QString,SSetting*> mapSettings;
};

/*!
 * \brief Single global instance. 
 *  
 * App main.cpp would be a good place to init/fini this. 
 * This is for context - not preferences.
 *  
 * \sa main 
 * \author pharvey (3/13/23)
 */
extern SSettings *g_SSettings;

#endif

