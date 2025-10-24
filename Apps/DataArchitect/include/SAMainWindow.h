#ifndef H_SAMainWindow
#define H_SAMainWindow

#include <AWScratchWidget.h>
#include <AWTreeWidget.h>
#include <AWFile.h>

#include <DATAWConnectionToolBar.h>
#include <DATAWSqlEditorWidget.h>
#include <DATAWEditorToolBar.h>

#include <SAOutputWidget.h>


class SAFile;
class DDiagram;
class DDiagramEditorWidget;
class DObject;

class SAMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    SAMainWindow( QWidget *pWidget = nullptr );
    ~SAMainWindow();

protected:
    void closeEvent( QCloseEvent *pEvent );

protected slots:
    void slotPropertiesCurrentChanged( const QString &stringName );

private:
    // central widget fills main client area
    QToolButton *pTabCloseButton;
    QTabWidget *pCentralWidget; 

    // current 'document'
    // - this is the root object and there can be 0-1
    // - determined in the usual way - file; new/open, and close
    SAFile *pFile;

    // current editor
    // - has focus 
    // - can have selected objects
    // - some controls may connect to this
    // - determined in slotEditorFocused
    AWEditorWidget *pEditorWidget;

    // current object
    // - selected object 
    // - there may be more than one but we just use/store last
    // - some object controls are connected to this (others via the diagram)
    // - determined in slotSelected
    DObject *pObject;        

    QToolBar *pToolBar;     // most menus, all toolbars, and all status bars are provided by editor \sa AWEditorWidget

    // Dock Browser
    QDockWidget *   pDockBrowser;
    AWTreeWidget *  pTreeWidgetBrowser;         
    // Dock Pallette
    QDockWidget *   pDockClasses;
    WStackedWidget *pWidgetStackPalette;        // stack containing a DStencilWidget + pWidgetStackClasses and a combobox to choose which to show
    QStackedWidget *pWidgetStackClasses;        // a stack of AWPalWidget's - one per open document - widget shown is for the document with focus
    // Dock Properties
    QDockWidget *   pDockProperties;            // we get a \sa AWPropWidget from selected editor object \sa ADObject::getPropWidget()
    QMap<QString,QString> mapPropertiesCurrent; // class,propname - [class=selected object class propname=AWPropWidget::propname] [used to maintain state]
    // Dock Panner
    QDockWidget *   pDockPanner;  
    QStackedWidget *pWidgetStackPanners;        // editor provides a widget we can stick in here (or nullptr) \sa AWEditorWidget::getPanWidget()
    // Dock ScratchPad
    QDockWidget *       pDockScratchPad;  
    AWScratchTabWidget *pScratchTabWidget;
    // Dock Output
    QDockWidget *   pDockOutput;                // primarily for output captured from pFile
    SAOutputWidget *pOutputWidget;

    // status bar
    QLabel *plabelMessage;
    QLabel *plabelModified;

    // menu
    QMenu * pMenuWorkspace;
    QAction *pActionNew; 
    QAction *pActionOpen;
    QAction *pActionSave;
    QAction *pActionSaveAs;
    QAction *pActionClose;
    QVector<QString>    vectorRecentStrings;
    QVector<QAction*>   vectorRecentActions;

    QAction *pActionPreferences;// app
    QAction *pActionExit;       

    QMenu *pMenuEdit;
    QAction *pActionFind;
    QAction *pActionReplace;

    QMenu *pMenuWindow;

    QMenu *pMenuHelp;
    QAction *pActionAbout;      
    QAction *pActionAboutCBD;
    QAction *pActionFeedback;

    QAction *pActionHelpMenu;

    AWFile *getDocument();
    QString getFileNameUserFriendly( const QString &stringFileName, int nMaxPath = 50 );
    QString getFileBaseName( const QString &stringFileName );
    QString getFilePathShort( const QString &stringFileName, int nMaxPath );
    QString getFilePath( const QString &stringFileName );

    // init
    void doInitActions();
    void doInitMenus();
    void doInitCentralArea();
    void doInitDocks();
    void doInitStatusBar();

    // docks
    void doInitDockBrowser();
    void doInitDockClasses();
    void doInitDockProperties();
    void doInitDockPanner();
    void doInitDockScratchPad();
    void doInitDockOutput();
    void doInitSplash();

    bool doNew();
    bool doClear();
    bool doDelete();
    bool doPreDeleteDocument( int n );

    // recent file names
    void doSaveRecentFiles();
    void doLoadRecentFiles();
    void doAddRecentFile( const QString &s );

    // state
    void doSaveState();
    void doLoadState();

    void doConnectEditor( AWEditorWidget *p );
    void doConnectEditorDiagram( AWEditorWidget *p );
    void doDisconnectEditor();
    void doDisconnectEditorDiagram();

    void doConnectObject( DObject *p );
    void doDisconnectObject();
    void doConnectProperties( AWPropWidget *pPropWidget );
    void doDisconnectProperties();

private slots:
    // workspace
    bool slotNew();             
    bool slotOpen();            
    bool slotSave();            
    bool slotSaveAs();          
    bool slotClose();  
    bool slotRecent( QAction *p ); 

    // for app
    void slotFind();
    void slotReplace();
    void slotPreferences();
    void slotExit();

    void slotAbout();
    void slotAboutCBD();
    void slotFeedback();

    void slotCloseEditor();
    void slotEditorFocused( int n );            // for when tab changed in tab widget
    void slotGoTo( AWEditorWidget *p );         // to set focus on tab as request by DDiagram etc

    void slotAppTitle();
    void slotEditorOpened( AWObject *, AWEditorWidget * );
    void slotEditorClosing( AWObject *, AWEditorWidget * );
    void slotModified( AWEditorWidget *p );
    void slotModified();
    void slotSelected( ADObject *p, bool );
};

#endif



