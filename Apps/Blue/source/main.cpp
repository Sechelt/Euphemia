/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "AppInfo.h"
#include "EMainWindow.h"

#include <QApplication>
#include <QStyleFactory>

// Globals...
#include <SColors.h>
#include <SSettings.h>

// SSetting classes we know about...
#include <SPen.h>       
#include <SRuler.h>       
#include <SBrush.h>     
#include <SFont.h>      

#include <LSGeneral.h>     
#include <LSBackground.h>     
#include <LSPage.h>     
#include <LSGrid.h>      
#include <LSPointer.h>      
#include <LSLine.h>     
#include <LSEllipse.h>     
#include <LSRender.h>     
#include <LSImage.h>     
#include <LSText.h>      
#include <LSRectangle.h> 

#include <PASErase.h>     
#include <PASFreeHand.h>  
#include <PASGradient.h>  
#include <PASPaste.h>     
#include <PASPolygonFilled.h>
#include <PASSpray.h>     

#include <DRSImage.h>     

int main( int argc, char *argv[] )
{
    QApplication a( argc, argv );

    // apps own resource file 'E' is included but bring in others...
    Q_INIT_RESOURCE( W );
    Q_INIT_RESOURCE( AW );
    Q_INIT_RESOURCE( L );
    Q_INIT_RESOURCE( PA );
    Q_INIT_RESOURCE( DR );

#ifdef W_FAKE_DARK
#ifdef Q_OS_WIN
    // Qt does not support the MS Windows dark-theme so we fake it here. - thanks to QtForums->Carl_P 
    QSettings settings( "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat );
    if ( settings.value( "AppsUseLightTheme" ) == 0 )
    {
        qApp->setStyle( QStyleFactory::create( "Fusion" ) );

        QPalette darkPalette;
        QColor darkColor = QColor( 45, 45, 45 );
        QColor disabledColor = QColor( 127, 127, 127 );

        darkPalette.setColor( QPalette::Window, darkColor );
        darkPalette.setColor( QPalette::WindowText, Qt::white );
        darkPalette.setColor( QPalette::Base, QColor( 18, 18, 18 ) );
        darkPalette.setColor( QPalette::AlternateBase, darkColor );
        darkPalette.setColor( QPalette::ToolTipBase, Qt::white );
        darkPalette.setColor( QPalette::ToolTipText, Qt::white );
        darkPalette.setColor( QPalette::Text, Qt::white );
        darkPalette.setColor( QPalette::Disabled, QPalette::Text, disabledColor );
        darkPalette.setColor( QPalette::Button, darkColor );
        darkPalette.setColor( QPalette::ButtonText, Qt::white );
        darkPalette.setColor( QPalette::Disabled, QPalette::ButtonText, disabledColor );
        darkPalette.setColor( QPalette::BrightText, Qt::red );
        darkPalette.setColor( QPalette::Link, QColor( 42, 130, 218 ) );
        darkPalette.setColor( QPalette::Highlight, QColor( 42, 130, 218 ) );
        darkPalette.setColor( QPalette::HighlightedText, Qt::black );
        darkPalette.setColor( QPalette::Disabled, QPalette::HighlightedText, disabledColor );

        qApp->setPalette( darkPalette );
        qApp->setStyleSheet( "QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }" );
    }
#else
    // temp solution to bring in dark theme in Linux+Motif
    // Fusion will be the default but it is light so we modify it
    qApp->setStyle( QStyleFactory::create( "Fusion" ) );
    QPalette darkPalette;                                                                                     
    QColor darkColor = QColor( 45, 45, 45 );                                                                  
    QColor disabledColor = QColor( 127, 127, 127 );                                                           
                                                                                                              
    darkPalette.setColor( QPalette::Window, darkColor );                                                      
    darkPalette.setColor( QPalette::WindowText, Qt::white );                                                  
    darkPalette.setColor( QPalette::Base, QColor( 18, 18, 18 ) );                                             
    darkPalette.setColor( QPalette::AlternateBase, darkColor );
    darkPalette.setColor( QPalette::ToolTipBase, Qt::white );                                                 
    darkPalette.setColor( QPalette::ToolTipText, Qt::white );                                                 
    darkPalette.setColor( QPalette::Text, Qt::white );                                                        
    darkPalette.setColor( QPalette::Disabled, QPalette::Text, disabledColor );                                
    darkPalette.setColor( QPalette::Button, darkColor );                                                      
    darkPalette.setColor( QPalette::ButtonText, Qt::white );                                                  
    darkPalette.setColor( QPalette::Disabled, QPalette::ButtonText, disabledColor );                          
    darkPalette.setColor( QPalette::BrightText, Qt::red );                                                    
    darkPalette.setColor( QPalette::Link, QColor( 42, 130, 218 ) );                                           
    darkPalette.setColor( QPalette::Highlight, QColor( 42, 130, 218 ) );                                      
    darkPalette.setColor( QPalette::HighlightedText, Qt::black );                                             
    darkPalette.setColor( QPalette::Disabled, QPalette::HighlightedText, disabledColor );                     
                                                                                                              
    qApp->setPalette( darkPalette );                                                                          
    qApp->setStyleSheet( "QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }" );
#endif
#endif
    /* Set some app meta information.
     * 
     * These will be used by QSettings when QSettings created with empty constructor.
     *
     */
    QCoreApplication::setOrganizationName( CBD_COMPANY );
    QCoreApplication::setOrganizationDomain( CBD_DOMAIN );
    QCoreApplication::setApplicationName( APP_NAME );
    QCoreApplication::setApplicationVersion( APP_VER );

    /*
     * Init globals. 
     * - some globals auto initialize but others, like those below, need to be initialized 
     *   explicitly in order to ensure appropriate timing for init and fini 
     */
    g_SColors = new SColors();
    g_SColors->doInit();

    /*
     * Create settings for use as 'context'. Another one of these could be used to support preferences but 
     * this one will exist as long as the main window is working. 
     * Settings such as whether or not to show a splash screen can be in QSetting. 
     */
    g_SSettings = new SSettings();
    g_SSettings->doInsert( "SPen", new SPen() );       
    g_SSettings->doInsert( "SRuler", new SRuler() );       
    g_SSettings->doInsert( "SBrush", new SBrush() );     
    g_SSettings->doInsert( "SFont", new SFont() );      

    g_SSettings->doInsert( "LSGeneral", new LSGeneral() );     
    g_SSettings->doInsert( "LSBackground", new LSBackground() );     
    g_SSettings->doInsert( "LSPage", new LSPage() );      
    g_SSettings->doInsert( "LSGrid", new LSGrid() );      
    g_SSettings->doInsert( "LSPointer", new LSPointer() );      
    g_SSettings->doInsert( "LSLine", new LSLine() );     
    g_SSettings->doInsert( "LSEllipse", new LSEllipse() );     
    g_SSettings->doInsert( "LSText", new LSText() );      
    g_SSettings->doInsert( "LSRender", new LSRender() );     
    g_SSettings->doInsert( "LSImage", new LSImage() );     
    g_SSettings->doInsert( "LSRectangle", new LSRectangle() ); 

    g_SSettings->doInsert( "PASPaste", new PASPaste() );     
    g_SSettings->doInsert( "PASPolygonFilled", new PASPolygonFilled() );
    g_SSettings->doInsert( "PASSpray", new PASSpray() );     
    g_SSettings->doInsert( "PASErase", new PASErase() );     
    g_SSettings->doInsert( "PASFreeHand", new PASFreeHand() );  
    g_SSettings->doInsert( "PASGradient", new PASGradient() );  

    g_SSettings->doInsert( "DRSImage", new DRSImage() );

    g_SSettings->doLoad();

    /* Create the main window and start the app event loop.
     * 
     * We use 'new' so we can use 'delete' before we get rid 
     * of the globals. 
     */
    EMainWindow *pMainWindow = new EMainWindow();
    pMainWindow->show();
    int n = a.exec();

    /* Delete main window
     * 
     * This will ensure that the UI is no longer using the globals - as it 
     * is, in theory, gone. 
     */
    delete pMainWindow;

    /* Fini globals.
     * 
     * The doFini() calls can probably go to the destructors. They were added during
     * some debugging.
     *
     */
    g_SColors->doFini();
    delete g_SColors;

    delete g_SSettings;

    return n;
}

