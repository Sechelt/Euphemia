#include "AppInfo.h"
#include "SAMainWindow.h"

#include <QSqlDatabase>

#include <WIconLayout.h>
#include <WSplash.h>

#include <AWOutputSearchWidget.h>

#include <DATAWConnection.h>

#include <DDiagram.h>
#include <DDiagramEditorWidget.h>
#include <DObject.h>
#include <DRectangle.h>
#include <DStencilsWidget.h>

#include "SAFile.h"
#include "SANewDialog.h"
#include "SAPreferencesDialog.h"
          
#define DMAX_RECENT_FILES 5

SAMainWindow::SAMainWindow( QWidget *pWidget ) 
    : QMainWindow( pWidget )
{
    g_Root          = nullptr;
    pFile           = nullptr;
    pEditorWidget   = nullptr;
    pObject         = nullptr;

    // application icon
    setWindowIcon( QIcon( ":DA/DataArchitectSmall" ) );
    setWindowTitle( APP_NAME );

    doInitActions();
    doInitMenus();
    doInitCentralArea();
    doInitDocks();
    doInitStatusBar();

    doLoadRecentFiles();
    doLoadState();

    // NOTE
    // Qt does not fully support Wayland proper at this time. One could run XWayland instead.
    // This means that moving dock windows around in Wayland proper will be problematic.
    // May not want to restore a messed up state.
    if ( QSettings().value( "RestoreState", true ).toInt() ) doLoadState();

    show();

    if ( QSettings().value( "Splash", true ).toBool() ) doInitSplash();

// qInfo() << QSqlDatabase::drivers();
}

SAMainWindow::~SAMainWindow()
{
    if ( pFile ) doDelete();
    pFile = nullptr;
}

void SAMainWindow::closeEvent( QCloseEvent *pEvent )
{
    if ( pFile && !slotClose() )
    {
        pEvent->ignore();
        return;
    }

    //
    doSaveRecentFiles();
    doSaveState();
    pEvent->accept();
}

void SAMainWindow::slotPropertiesCurrentChanged( const QString &stringName )
{
    AWPropWidget *pPropWidget = (AWPropWidget *)pDockProperties->widget();             
    Q_ASSERT( pPropWidget );                                                           
    mapPropertiesCurrent[pPropWidget->getObject()->metaObject()->className()] = stringName;
}

AWFile *SAMainWindow::getDocument()
{
    AWEditorWidget *pEditorWidget = (AWEditorWidget*)pCentralWidget->currentWidget();
    if ( !pEditorWidget ) return nullptr;
    return (AWFile*)pEditorWidget->getObject();
}

/*!
 * \brief Get a file name (including path) in User friendly format. 
 *  
 * The format is [base name] - [path] where a long path is reduced as requested. 
 *  
 * \author pharvey (2/11/23)
 * 
 * \param stringFileName 
 * \param nMaxPath       max path length 0 = no limit otherwise must be >= 10 default:50
 * 
 * \return QString 
 */
QString SAMainWindow::getFileNameUserFriendly( const QString &stringFileName, int nMaxPath )
{
    return getFileBaseName( stringFileName ) + " - " + getFilePathShort( stringFileName, nMaxPath );
}

QString SAMainWindow::getFileBaseName( const QString &stringFileName )
{
    QFileInfo fileInfo( stringFileName );
    return fileInfo.fileName();
}

/*!
 * \brief Cut left end of path if path too long. 
 *  
 * The path will start with "... " if reduced. 
 * 
 * \author pharvey (2/11/23)
 * 
 * \param stringFileName 
 * \param nMaxPath       
 * 
 * \return QString 
 */
QString SAMainWindow::getFilePathShort( const QString &stringFileName, int nMaxPath )
{
    if ( nMaxPath == 0 ) return getFilePath( stringFileName );

    if ( nMaxPath < 10 )
    {
        qWarning( "Path can only be reduced to 10 or more." );
        nMaxPath = 10;
    }

    QString s = getFilePath( stringFileName );
    if ( s.length() <= nMaxPath ) return s;

    return "... " + s.right( nMaxPath - 4 );
}

QString SAMainWindow::getFilePath( const QString &stringFileName )
{
    // return QDir::cleanPath( stringFileName );
    return QFileInfo( stringFileName ).absolutePath();
}

void SAMainWindow::doInitActions()
{
    // create actions
    pActionNew          = new QAction( QIcon( ":W/new" ), tr("&New"), this );
    pActionOpen         = new QAction( QIcon( ":W/open" ), tr("&Open..."), this );
    pActionSave         = new QAction( QIcon( ":W/save" ), tr("&Save"), this );
    pActionSaveAs       = new QAction( tr("Save &As..."), this );
    pActionClose        = new QAction( QIcon( ":W/close" ), tr("Close"), this );

    pActionPreferences  = new QAction( QIcon( ":W/Properties16x16" ), tr("Preferences..."), this );
    pActionExit         = new QAction( QIcon( ":W/exit" ), tr("E&xit"), this );

    pActionFind         = new QAction( QIcon( ":AWFind" ), tr("Find..."), this );
    pActionReplace      = new QAction( QIcon( ":AW/Replace" ), tr("Replace..."), this );
    pActionFind->setToolTip( tr( "find" ) );
    pActionReplace->setToolTip( tr( "find and replace" ) );

    pActionAbout            = new QAction( QIcon( ":DA/DataArchitectSmall" ), tr("&About..."), this );
    pActionAboutCBD         = new QAction( QIcon( ":W/CodeByDesign32x32" ), tr("About CodeByDesign..."), this );
    pActionFeedback         = new QAction( QIcon( ":DA/Feedback" ), tr("Feedback..."), this );
    pActionFeedback->setToolTip( tr("report an issue or request a feature") );

    pActionPreferences->setShortcut( QKeySequence::Preferences );

    pActionSave->setEnabled( false ); 
    pActionSaveAs->setEnabled( false );
    pActionClose->setEnabled( false );

    pActionFind->setEnabled( false );
    pActionReplace->setEnabled( false );

    // connect actions to self (we will redirect, as needed, via these)
    connect( pActionNew, &QAction::triggered, this, &SAMainWindow::slotNew );
    connect( pActionOpen, &QAction::triggered, this, &SAMainWindow::slotOpen );
    connect( pActionSave, &QAction::triggered, this, &SAMainWindow::slotSave );
    connect( pActionSaveAs, &QAction::triggered, this, &SAMainWindow::slotSaveAs );
    connect( pActionClose, &QAction::triggered, this, &SAMainWindow::slotClose );

    connect( pActionFind,        SIGNAL(triggered()),                                this, SLOT(slotFind()) );              
    connect( pActionReplace,     SIGNAL(triggered()),                                this, SLOT(slotReplace()) );              

    connect( pActionPreferences, &QAction::triggered, this, &SAMainWindow::slotPreferences );
    connect( pActionExit, &QAction::triggered, this, &SAMainWindow::slotExit );

    connect( pActionAbout, &QAction::triggered, this, &SAMainWindow::slotAbout );
    connect( pActionAboutCBD, &QAction::triggered, this, &SAMainWindow::slotAboutCBD );
    connect( pActionFeedback, SIGNAL(triggered()), SLOT(slotFeedback()) );
}

void SAMainWindow::doInitMenus()
{
    pMenuWorkspace = menuBar()->addMenu( tr("&Workspace") );

    pMenuWorkspace->addAction( pActionNew );
    pMenuWorkspace->addAction( pActionOpen );
    pMenuWorkspace->addAction( pActionSave );
    pMenuWorkspace->addAction( pActionSaveAs );
    pMenuWorkspace->addAction( pActionClose );
    pMenuWorkspace->addSeparator();
    pMenuWorkspace->addAction( pActionFind );
    pMenuWorkspace->addAction( pActionReplace );
    pMenuWorkspace->addSeparator();
    pMenuWorkspace->addAction( pActionPreferences );
    pMenuWorkspace->addSeparator();
    pMenuWorkspace->addAction( pActionExit );
    pMenuWorkspace->addSeparator();

    // recent files - just blank for now
    connect( pMenuWorkspace, SIGNAL(triggered(QAction*)), this, SLOT(slotRecent(QAction*)) );

    pMenuWindow = new QMenu( tr("&Window") );
    menuBar()->addMenu( pMenuWindow );

    menuBar()->addSeparator();

    pMenuHelp = new QMenu( tr("&Help") );
    pMenuHelp->addAction( pActionAbout );
    pMenuHelp->addAction( pActionAboutCBD );
    pMenuHelp->addAction( pActionFeedback );
    pActionHelpMenu = menuBar()->addMenu( pMenuHelp );
}

void SAMainWindow::doInitStatusBar()
{
    QStatusBar *pstatusbar = statusBar();

    plabelMessage= new QLabel( pstatusbar );
    plabelMessage->setToolTip( tr("general messages") );
    pstatusbar->addPermanentWidget( plabelMessage, 10 );

    plabelModified  = new QLabel( pstatusbar );
    plabelModified->setToolTip( tr("edit status") );
    pstatusbar->addPermanentWidget( plabelModified, 0 );

}

void SAMainWindow::doInitCentralArea()
{
    pCentralWidget = new QTabWidget( this );
    pCentralWidget->setObjectName( "CentralWidget" );
    pCentralWidget->resize( 800, 600 ); // just use up some space for case when we are 1st executed
    setCentralWidget( pCentralWidget ); 
    connect( pCentralWidget, SIGNAL(currentChanged(int)), SLOT(slotEditorFocused(int)) );

    pTabCloseButton = new QToolButton();
    pTabCloseButton->setIcon( QPixmap( ":W/close" ) );
    pCentralWidget->setCornerWidget( pTabCloseButton, Qt::TopRightCorner );

    // close the editor but leave the object alone
    connect( pTabCloseButton, SIGNAL(clicked()), SLOT(slotCloseEditor()) );
}

void SAMainWindow::doInitDocks()
{
    doInitDockBrowser();
    doInitDockClasses(); 
    doInitDockProperties();
    doInitDockPanner();
    doInitDockScratchPad();
    doInitDockOutput();
}

void SAMainWindow::doInitDockBrowser()
{
    pDockBrowser = new QDockWidget( tr( "Objects" ), this );
    pDockBrowser->setObjectName( "Objects" );

    pTreeWidgetBrowser = new AWTreeWidget( pDockBrowser );

    pDockBrowser->setWidget( pTreeWidgetBrowser );
    addDockWidget( Qt::LeftDockWidgetArea, pDockBrowser );

    pMenuWindow->addAction( pDockBrowser->toggleViewAction() );
}

void SAMainWindow::doInitDockClasses()
{
    pDockClasses = new QDockWidget( tr( "Classes" ), this );
    pDockClasses->setObjectName( "Classes" );

    pWidgetStackPalette = new WStackedWidget( pDockClasses );
    pWidgetStackClasses = new QStackedWidget( pWidgetStackPalette );
    pWidgetStackPalette->addWidget( tr( "Classes" ), pWidgetStackClasses );

    pDockClasses->setWidget( pWidgetStackPalette );
    addDockWidget( Qt::LeftDockWidgetArea, pDockClasses );

    pMenuWindow->addAction( pDockClasses->toggleViewAction() );
}

void SAMainWindow::doInitDockProperties()
{
    pDockProperties = new QDockWidget( tr("Properties"), this );
    pDockProperties->setObjectName( "Properties" );
    addDockWidget( Qt::RightDockWidgetArea, pDockProperties );

    pMenuWindow->addAction( pDockProperties->toggleViewAction() );
}

void SAMainWindow::doInitDockPanner()
{
    pDockPanner = new QDockWidget( tr( "Panner" ), this );
    pDockPanner->setObjectName( "Panner" );
    pWidgetStackPanners = new QStackedWidget( pDockPanner );
    pDockPanner->setWidget( pWidgetStackPanners );
    addDockWidget( Qt::RightDockWidgetArea, pDockPanner );

    pMenuWindow->addAction( pDockPanner->toggleViewAction() );
}

void SAMainWindow::doInitDockScratchPad()
{
    pDockScratchPad = new QDockWidget( tr( "ScratchPad" ), this );
    pDockScratchPad->setObjectName( "ScratchPad" );
    pScratchTabWidget = new AWScratchTabWidget( pDockScratchPad );
    pDockScratchPad->setWidget( pScratchTabWidget );
    addDockWidget( Qt::RightDockWidgetArea, pDockScratchPad );

    pMenuWindow->addAction( pDockScratchPad->toggleViewAction() );
}

void SAMainWindow::doInitDockOutput()
{
    pDockOutput = new QDockWidget( tr( "Output" ), this );
    pDockOutput->setObjectName( "Output" );

    pOutputWidget = new SAOutputWidget( pDockOutput );
    pDockOutput->setWidget( pOutputWidget );

    addDockWidget( Qt::BottomDockWidgetArea, pDockOutput );

    pMenuWindow->addAction( pDockOutput->toggleViewAction() );

    pOutputWidget->getSearch()->setEnabled( false );
}

/*!
  Bring up a splash dialog; let it time out. Also create our time-out timer if we are a demo.
*/
void SAMainWindow::doInitSplash()
{
    WSplash splash( this, QPixmap( ":DA/DataArchitectSplash" ), true, true, 10 );
    splash.exec();
}


/*!
 * \brief Create a new file. 
 *  
 * \author pharvey (4/22/20)
 * 
 * \return bool 
 */
bool SAMainWindow::doNew()
{
    Q_ASSERT( !pFile );

    // create new root object
    pFile = new SAFile();
    g_Root = pFile;
    pFile->setEditorWidgetParent( this );
    pTreeWidgetBrowser->setRoot( pFile );

    pWidgetStackPalette->addWidget( tr( "Stencils" ), pFile->getStencilsWidget( pWidgetStackPalette ) );

    connect( pFile, SIGNAL(signalChangedFileName()), this, SLOT(slotAppTitle()) );
    connect( pFile, SIGNAL(signalEditorOpened(AWObject*,AWEditorWidget*)), this, SLOT(slotEditorOpened(AWObject*,AWEditorWidget*)) );
    connect( pFile, SIGNAL(signalEditorClosing(AWObject*,AWEditorWidget*)), this, SLOT(slotEditorClosing(AWObject*,AWEditorWidget*)) );
    connect( pFile, SIGNAL(signalModified()), this, SLOT(slotModified()) );
    connect( pFile, SIGNAL(signalSelected(ADObject*,bool)), this, SLOT(slotSelected(ADObject*,bool)) );
    connect( pFile, SIGNAL(signalOutput(ADEventOutput *)), pOutputWidget, SLOT(slotOutput(ADEventOutput *)) );
    connect( pOutputWidget, SIGNAL(signalFind(const ADFindCriteria &)), pFile, SLOT(slotFind(const ADFindCriteria &)) );
    connect( pOutputWidget, SIGNAL(signalGoTo(ADObjectPathList)), pFile, SLOT(slotGoTo(ADObjectPathList)) );
    connect( pOutputWidget, SIGNAL(signalGoToView(ADObjectPathList)), pFile, SLOT(slotGoToView(ADObjectPathList)) );

    slotAppTitle();

    pActionSaveAs->setEnabled( true );
    pActionClose->setEnabled( true );
    pActionFind->setEnabled( true );
    pActionReplace->setEnabled( true );
    pOutputWidget->getSearch()->setEnabled( true );

    // ensure that this is visible else User may get lost as to whats next
    pDockBrowser->show(); 
    pDockBrowser->raise(); 
    pDockBrowser->setFocus(); 

    return true;
}

// supports doDeleteFile()
bool SAMainWindow::doClear()
{
    // get rid of editors cleanly
    while ( pCentralWidget->count() )
    {
        doPreDeleteDocument( 0 );
    }

    // pFile->setFileName( QString() );
    pFile->doClear();

    return true;
}

bool SAMainWindow::doDelete()
{
    Q_ASSERT( pFile );

    doClear();

    // 
    pTreeWidgetBrowser->setRoot( nullptr );
    g_Root = nullptr;
    delete pFile;
    pFile = nullptr;

    slotAppTitle();

    //
    pActionSave->setEnabled( false );
    pActionSaveAs->setEnabled( false );
    pActionClose->setEnabled( false );
    pActionFind->setEnabled( false );
    pActionReplace->setEnabled( false );
    pOutputWidget->getSearch()->setEnabled( false );

    return true;
}

/*!
 * \brief Close editor without prompting to save any changes.
 *  
 * This supports \sa doClearFile. 
 * The UI will be disconnected by \sa slotEditorClosing. 
 * The Document is not actually deleted here... this happens in doClearFile.
 *  
 * \author pharvey (4/22/20)
 * 
 * \return bool 
 */
bool SAMainWindow::doPreDeleteDocument( int n )
{
    AWEditorWidget *pEditorWidget = (AWEditorWidget*)pCentralWidget->widget( n );
    AWFile *pDocument = (AWFile*)pEditorWidget->getObject();
    pDocument->setModified( false ); // ensure there is no prompting
    pDocument->slotCloseEditor();

    return true;
}

void SAMainWindow::doSaveRecentFiles()
{
    QSettings settings;
    settings.beginGroup( "Recent" );
    for ( int n = 0; n < vectorRecentStrings.count(); n++ )
    {
        settings.setValue( QString::number( n ), vectorRecentStrings.at( n ) );
    }
    settings.endGroup();
}

void SAMainWindow::doLoadRecentFiles()
{
    QSettings settings;
    settings.beginGroup( "Recent" );
    for ( int n = 0; n < DMAX_RECENT_FILES; n++ )
    {
        QString s = settings.value( QString::number( n ), "" ).toString();
        if ( s.isEmpty() ) break;

        vectorRecentStrings.append( s );
        vectorRecentActions.append( pMenuWorkspace->addAction( getFileNameUserFriendly( s ) ) );

    }
    settings.endGroup();
}

void SAMainWindow::doAddRecentFile( const QString &s )
{
    if ( vectorRecentStrings.contains( s ) )
    {
        // bring to the front
        return;
    }

    // remove actions from menu (order is changing)
    for ( int n = 0; n < vectorRecentActions.count(); n++ )
    {
        pMenuWorkspace->removeAction( vectorRecentActions[n] );
    }
    
    // insert at front
    vectorRecentStrings.insert( 0, s );
    vectorRecentActions.insert( 0, pMenuWorkspace->addAction( QString("1 %2").arg( getFileNameUserFriendly( s ) ) ) );

    // delete oldest as needed
    if ( vectorRecentStrings.count() > DMAX_RECENT_FILES )
    {
        delete vectorRecentActions.last();
        vectorRecentStrings.removeLast();
        vectorRecentActions.removeLast();
    }

    // update all action texts to indicate new index position
    // add back to menu
    for ( int n = 0; n < vectorRecentActions.count(); n++ )
    {
        vectorRecentActions[n]->setText( QString("%1 %2").arg( n + 1 ).arg( getFileNameUserFriendly( vectorRecentStrings[n] ) ) );
        pMenuWorkspace->addAction( vectorRecentActions[n] );
    }
}

void SAMainWindow::doSaveState()
{
    QSettings settings;
    settings.setValue( "geometry", saveGeometry() );
    settings.setValue( "windowState", saveState() );
}

void SAMainWindow::doLoadState()
{
    QSettings settings;
    restoreGeometry( settings.value( "geometry" ).toByteArray() );
    restoreState( settings.value( "windowState" ).toByteArray() );
}

void SAMainWindow::doConnectEditor( AWEditorWidget *p )
{
    Q_ASSERT(!pEditorWidget);

    if ( p->inherits( "DDiagramEditorWidget" ) ) return doConnectEditorDiagram( p );
    pEditorWidget = p;

}

void SAMainWindow::doConnectEditorDiagram( AWEditorWidget *p )
{
    pEditorWidget = p;
    // DDiagramEditorWidget *pDiagramEditorWidget = (DDiagramEditorWidget*)p;
    DDiagram *pDiagram = (DDiagram *)p->getObject();

    pWidgetStackClasses->setEnabled( true );
    pWidgetStackPanners->setEnabled( true );

    // connect any selected object
    if ( pDiagram->isSelected() )
    {
        QList<ADObject*> l = pDiagram->getSelectionManager()->getSelected();
        Q_ASSERT( l.count() );
        ADObject *p = l.last();
        Q_ASSERT( p->inherits( "DObject" ) );
        doConnectObject( (DObject*)p );
    }
    else
    {
        doConnectProperties( pDiagram->getPropWidget() );
    }
}

void SAMainWindow::doDisconnectEditor()
{
    Q_ASSERT(pEditorWidget);
    if ( pEditorWidget->inherits( "DDiagramEditorWidget" ) ) return doDisconnectEditorDiagram();

    if ( pEditorWidget->isFind() ) pEditorWidget->doCancelFind();

    pEditorWidget = nullptr;
}

void SAMainWindow::doDisconnectEditorDiagram()
{
    Q_ASSERT(pEditorWidget);

    // disconnect any selected object
    if ( pObject ) doDisconnectObject();
    else doDisconnectProperties();

    //
    // DDiagram *pDiagram = (DDiagram *)pEditorWidget->getObject();

    // disbale controls
    pWidgetStackClasses->setEnabled( false );
    pWidgetStackPanners->setEnabled( false );

    pEditorWidget = nullptr;
}

void SAMainWindow::doConnectObject( DObject *p )
{
    pObject = p;
    doConnectProperties( pObject->getPropWidget() );
}

void SAMainWindow::doDisconnectObject()
{
    Q_ASSERT(pObject);
    doDisconnectProperties();
    pObject = nullptr;
}

/*!
 * \brief Connect properties widget. 
 *  
 * The given properties widget is put into the properties dock. 
 * The objects class name is used to try to set focus on the same property last used by the class. 
 *  
 *      ie If 'Name' properties was last visible for a DRectangle then
 *      make 'Name' properties visible next time a DRectangle is selected.
 *  
 * \sa doDisconnectProperties() 
 * \sa mapPropertiesCurrent 
 *  
 * \author pharvey (8/2/20)
 * 
 * \param pPropWidget 
 */
void SAMainWindow::doConnectProperties( AWPropWidget *pPropWidget )
{
    pDockProperties->setWidget( pPropWidget );
    pDockProperties->setWindowIcon( pPropWidget->getObject()->getIcon() );
    // pDockProperties->setWindowTitle( QString( "%1(%2)" ).arg( pPropWidget->getObject()->metaObject()->className() ).arg( pPropWidget->getObject()->getName() ) );

    QString s = mapPropertiesCurrent[pPropWidget->getObject()->metaObject()->className()];
    if ( !s.isEmpty() ) pPropWidget->setCurrent( s );

    connect( pPropWidget, SIGNAL(signalCurrentChanged(const QString &)), SLOT(slotPropertiesCurrentChanged(const QString &)) );
}

/*!
 * \brief Disconnect properties widget.
 *  
 * The properties widget (in properties dock) is simply deleted. 
 *  
 * \sa doConnectProperties() 
 *  
 * \author pharvey (8/2/20)
 */
void SAMainWindow::doDisconnectProperties()
{
    QWidget *pWidget = pDockProperties->widget();
    if ( pWidget )
    { 
        pWidget->setParent( nullptr );
        delete pWidget;
    }
    pDockProperties->setWindowIcon( QIcon() );
    pDockProperties->setWindowTitle( tr("Properties") );
}

/*!
 * \brief New file.
 *  
 * \author pharvey (4/21/20)
 * 
 * \return bool 
 */
bool SAMainWindow::slotNew()
{
    if ( pFile && !slotClose() ) return false;
    doNew();
    pFile->slotObjectDialog( this );

    return true;
}

/*!
 * \brief Open a file.
 * 
 * \author pharvey (4/21/20)
 * 
 * \return bool 
 */
bool SAMainWindow::slotOpen()
{
    if ( pFile && !slotClose() ) return false;
    doNew();
    if ( pFile->slotOpen() ) 
    {
        doAddRecentFile( pFile->getFileName() );
        // slotAppTitle();
//        QGuiApplication::processEvents();
        pTreeWidgetBrowser->collapseAll();
        return true;
    }
    doDelete();

    return false;
}

/*!
 * \brief Save a workspace.
 * 
 * \author pharvey (4/21/20)
 * 
 * \return bool 
 */
bool SAMainWindow::slotSave()
{ 
    Q_ASSERT( pFile );
    if ( !pFile->isModified() ) return true;
    if ( pFile->getFileName().isEmpty() ) return slotSaveAs();

    if ( pFile->slotSave() != AWObject::SaveSuccess ) return false;

    slotAppTitle();
    pActionSave->setEnabled( false );

    return true;
}
  
/*!
 * \brief Save workspace under a different name.
 * 
 * \author pharvey (4/21/20)
 * 
 * \return bool 
 */
bool SAMainWindow::slotSaveAs()
{
    Q_ASSERT( pFile );
    if ( pFile->slotSaveAs() != AWObject::SaveSuccess ) return false;

    doAddRecentFile( pFile->getFileName() );
    slotAppTitle();
    pActionSave->setEnabled( false );

    return true;
}

/*!
 * \brief Close file.
 *  
 * \author pharvey (4/21/20)
 * 
 * \return bool 
 */
bool SAMainWindow::slotClose()
{
    Q_ASSERT( pFile );

    // clear - save if need be
    AWObject::SaveResults n = pFile->slotClose();
    if ( n == AWObject::SaveCancel ) return false;

    return doDelete();
}

bool SAMainWindow::slotRecent( QAction *p )     
{
    int nIndex = vectorRecentActions.indexOf( p );
    if ( nIndex < 0 ) return false;

    QString s = vectorRecentStrings.at( nIndex );
    if ( s.isEmpty() ) return false;

    if ( pFile && !slotClose() ) return false;
    doNew();

    if ( pFile->doOpen( s ) ) 
    {
        pTreeWidgetBrowser->collapseAll();
        // remove all from menu
        QAction *p;
        foreach( p, vectorRecentActions )
        {
            pMenuWorkspace->removeAction( p );
        }
        // move nIndex to front
        vectorRecentStrings.move( nIndex, 0 );
        vectorRecentActions.move( nIndex, 0 );

        // add them back to the menu
        foreach( p, vectorRecentActions )
        {
            pMenuWorkspace->addAction( p );
        }
        return true;
    }

    doDelete();

    return false;
}

void SAMainWindow::slotFind()
{
    if ( pDockOutput->isHidden() ) pDockOutput->setVisible( true );
    pOutputWidget->setFocusFind();
}

void SAMainWindow::slotReplace()
{
    if ( pDockOutput->isHidden() ) pDockOutput->setVisible( true );
    pOutputWidget->setFocusReplace();
}

void SAMainWindow::slotPreferences()
{
    // new way
    SAPreferencesDialog::doPreferences( this );

    // old way - we need to purge this stuff?
//    AWPrefItem *pItemRoot = new SAPrefWorkspace();
//    AWPrefDialog dialog( this, pItemRoot );
//    dialog.exec();
}

void SAMainWindow::slotExit()
{
    if ( pFile && !slotClose() ) return;
    close();
}

void SAMainWindow::slotAbout()
{
    QMessageBox::about( this, 
                        tr( "About..." ),
                        tr( "<b>" APP_NAME "</b><br><br>" ) +
                        tr( "Description: A system design tool.<br><br>" ) +
                        tr( "Version: v" APP_VER "<br>" ) +
                        tr( "License: " CBD_LICENSE "<br>" ) +
                        tr( "Copyright: " CBD_COMPANY " " CBD_COPYRIGHT )
                      );
}

void SAMainWindow::slotAboutCBD()
{
    // seems app icon gets in there automagically
    QMessageBox::about( this, 
                        tr( "About " CBD_COMPANY "..." ),
                        tr( "<b>" CBD_COMPANY "</b> is a software company which specializes in tools for; System Architects, Developers, and Consultants.<br><br> http://www.codebydesign.com" )
                      );
}

/*!
 * \brief Provide feedback. 
 *  
 * Feedback is done via page on www.codebydesign.com web site. 
 * 
 * \author pharvey (7/16/20)
 */
void SAMainWindow::slotFeedback()
{
    QDesktopServices::openUrl( QUrl( "https://www.codebydesign.com/mantisbt" ) );
}

void SAMainWindow::slotCloseEditor()
{
    if ( !pEditorWidget ) return;
    pEditorWidget->getObject()->slotCloseEditor();
    // pCentralWidget->removeTab( pCentralWidget->currentIndex() );
}

/*!
 * \brief Synchronize the UI with the editor in focus.
 *  
 * The central widget tells us when an editor has come into focus. 
 * We then disconnect UI from any previous editor and connect the UI to the current one. 
 *  
 * Some UI controls are synchronized directly with the DObject which has 'focus'. In this case 'focus' means 
 * a selected DObject (last one if there are multiple). So this is handled in slotSelected.   
 *  
 * \sa slotSelected 
 *  
 * \author pharvey (2/18/20)
 * 
 * \param n 
 */
void SAMainWindow::slotEditorFocused( int n )
{
    if ( pEditorWidget ) doDisconnectEditor();
    if ( n < 0 ) return;

    pWidgetStackClasses->setCurrentIndex( n );
    pWidgetStackPanners->setCurrentIndex( n );

    doConnectEditor( (AWEditorWidget *)pCentralWidget->currentWidget() );
}

void SAMainWindow::slotGoTo( AWEditorWidget *pEditor )
{
    // should trigger slotEditorFocused??
    pCentralWidget->setCurrentWidget( pEditor );
}

void SAMainWindow::slotAppTitle()
{
    if ( pFile ) setWindowTitle( QString("%1 - %2 %3").arg( APP_NAME ).arg( getFileNameUserFriendly( pFile->getFileName() ) ).arg( pFile->isModified() ? "*" : "" ) );
    else setWindowTitle( APP_NAME );
}

/*!
 * \brief Catch when an object has created/opened an editor. 
 *  
 * This is connected to g_Root (\sa SAFile::signalEditorOpened). 
 *  
 * Add to the central tab widget and add some dock window widgets. 
 *  
 * Other controls (menu,toolbar etc) are connected/disconnected based upon focus (\sa slotEditorFocused). 
 *  
 * \author pharvey (12/29/20)
 * 
 * \param pObject 
 * \param pEditorWidget 
 */
void SAMainWindow::slotEditorOpened( AWObject *pObject, AWEditorWidget *pEditorWidget )
{
    int nTab = pCentralWidget->addTab( pEditorWidget, pObject->getName() );

    pWidgetStackClasses->addWidget( (QWidget*)pEditorWidget->getPalWidget() );                                                                            
    pWidgetStackPanners->addWidget( (QWidget*)pEditorWidget->getPanWidget() );                                                                              

    pCentralWidget->setCurrentIndex( nTab );

    connect( pEditorWidget, SIGNAL(signalModified(AWEditorWidget*)), this, SLOT(slotModified(AWEditorWidget*)) );
    connect( pEditorWidget, SIGNAL(signalGoTo(AWEditorWidget*)), this, SLOT(slotGoTo(AWEditorWidget*)) );
}

void SAMainWindow::slotEditorClosing( AWObject *pObject, AWEditorWidget *pEditorWidget )
{
    Q_ASSERT(pObject);
    Q_ASSERT(pEditorWidget);

    int n = pCentralWidget->indexOf( pEditorWidget );
    Q_ASSERT( n >= 0 );

    // The following is normally done in slotEditorFocused but in this case it would fail
    // as the objects in question will be deleted by then. So we do it here.
    if ( pEditorWidget == this->pEditorWidget )
    { 
        if ( pEditorWidget ) doDisconnectEditor();
    }

    // Just remove these - they will get deleted when the editor is deleted.
    pWidgetStackClasses->removeWidget( pWidgetStackClasses->widget( n ) );
    pWidgetStackPanners->removeWidget( pWidgetStackPanners->widget( n ) );  
}

/*!
 * \brief Catch when an editor has changed to a modified state. 
 *  
 * This is connected to the current editor (\sa AWEditorWidget::signalModified). 
 * 
 * \author pharvey (12/29/20)
 * 
 * \param pEditor 
 */
void SAMainWindow::slotModified( AWEditorWidget *pEditor )
{
    AWObject *pObject = pEditor->getObject();
    int nTab = pCentralWidget->indexOf( pEditor );
    pCentralWidget->setTabText( nTab, pObject->getName() + ( pObject->isModified() ? "*" : "" ) ); 
}

/*!
 * \brief Catch when an object in the app data is modified.
 *  
 * This is connected to g_Root (\sa SAFile::signalModified).  
 * 
 * We catch this so we can synchronize some UI controls.
 *  
 * 
 * \author pharvey (12/29/20)
 */
void SAMainWindow::slotModified()
{
    Q_ASSERT( pFile );

    pActionSave->setEnabled( pFile->isModified() ); 
    slotAppTitle();
}

/*!
 * \brief Catch when an object in the app data is selected.
 *  
 * This is connected to g_Root (\sa SAFile::signalSelected).  
 * 
 * We catch this so we can synchronize some UI controls with the DObject. 
 *  
 * Some controls are partly synchronized to the current editor and partly to the current object. 
 * For example pen color; 
 *  
 * - connected to editor to change color on *all* selected objects 
 * - connected to last selected object to get notification, from object, that the pen color was changed 
 *  
 * This is important as it allows multiple controls to change an object while keeping all synchronized.
 *  
 * \note It is possible to do this just using signalModified but doing so will lead to a significant hit on efficiency. 
 *  
 * \author pharvey (2/6/20)
 * 
 * \param p 
 * \param b 
 */
void SAMainWindow::slotSelected( ADObject *p, bool b )
{
    Q_ASSERT( pFile );

    // disconnect object (or diagram)
    if ( pObject ) doDisconnectObject();
    else doDisconnectProperties();

    // connect object (or diagram)
    if ( b && p->inherits( "DObject" ) ) 
    {
        doConnectObject( (DObject*)p );
    }
    else
    {
        AWEditorWidget *pEditorWidget = (AWEditorWidget*)pCentralWidget->currentWidget();
        if ( !pEditorWidget ) return;
        AWObject *pObject = pEditorWidget->getObject();
        doConnectProperties( pObject->getPropWidget() );
    }
}



