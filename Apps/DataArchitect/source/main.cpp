#include "AppInfo.h"
#include "SAMainWindow.h"

#include <QApplication>
#include <QStyleFactory>

// Globals...
#include <SColors.h>
#include <SSettings.h>

// SSetting classes we know about...
#include <SPen.h>       
#include <SBrush.h>     
#include <SFont.h>      
#include <SText.h>      
#include <SRuler.h>       
#include <SGrid.h>       

int main( int argc, char *argv[] )
{
    QApplication a( argc, argv );

    // Apps own resource file (if any) is included. 
    // Bring in the library resources...
    Q_INIT_RESOURCE( W );
    Q_INIT_RESOURCE( AW );
    Q_INIT_RESOURCE( DATA );
    Q_INIT_RESOURCE( DATAW );
    Q_INIT_RESOURCE( D );
    Q_INIT_RESOURCE( ORG );
    Q_INIT_RESOURCE( FL );
    Q_INIT_RESOURCE( FR );
    Q_INIT_RESOURCE( LM );
    Q_INIT_RESOURCE( PM );

#ifdef xxQ_OS_WINxx
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
#endif

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
    g_SSettings->doInsert( "SBrush", new SBrush() );     
    g_SSettings->doInsert( "SFont", new SFont() );      
    g_SSettings->doInsert( "SText", new SText() );      
    g_SSettings->doInsert( "SRuler", new SRuler() );       
    g_SSettings->doInsert( "SGrid", new SGrid() );       
    g_SSettings->doLoad();

    int n;
    {
        SAMainWindow w;
        w.show();
        n = a.exec();
    }

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

