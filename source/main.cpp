#include "EMainWindow.h"

#include <QApplication>
#include <QStyleFactory>

#include <WPaletteColor.h>

#include "AppInfo.h"

int main( int argc, char *argv[] )
{
    QApplication a( argc, argv );

    // apps own resource file 'E' is included but bring in others...
    Q_INIT_RESOURCE( W );
    Q_INIT_RESOURCE( P );

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
    g_PaletteColors = new WPaletteColor();
    g_PaletteColors->doInit();

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
    g_PaletteColors->doFini();
    delete g_PaletteColors;

    return n;
}

