#include "LibInfo.h"
#include "DATAWSql.h"

#include <ADTransaction.h>

#include <AWTreeWidget.h>
#include <AWDataWidget.h>

#include "DATAWConnection.h"
#include "DATAWSqlEditorWidget.h"

DATAWSql::DATAWSql( ADObject *pParent, const QString &stringName )
    : AWObject( pParent, stringName )
{
    bCanCreateEditor = true;

    mapMeta.clear();
}

DATAWSql::~DATAWSql()
{
}

void DATAWSql::setSQL( const QString &s )
{
    if ( s == stringSQL ) return;

    stringSQL = s;
    setModified();
}

void DATAWSql::setModified( bool b )
{
    bool bChanged = bModified != b;
    ADObject::setModified( b );
    if ( bChanged ) // prevent possible looping
    {
        if ( isEditor() )
        { 
            DATAWSqlEditorWidget *p = (DATAWSqlEditorWidget*)getEditorWidget();
            p->setModified( b );
        }
    }
}

QPixmap DATAWSql::getIcon()
{
    return QPixmap( ":DATAW/SQLEditor" );
}

AWDataWidget *DATAWSql::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = AWObject::getObjectWidget( pWidgetParent );
    return p;
}

AWPropWidget *DATAWSql::getPropWidget( QWidget *pWidgetParent )
{
    AWPropWidget *pPropWidget = new AWPropWidget( this, pWidgetParent );
    return pPropWidget;
}

QMenu *DATAWSql::getContextMenu( QWidget *pParent )
{
    QMenu *pMenu = AWObject::getContextMenu( pParent );
    QAction *   pAction;

    // add our stuff at the top of the menu

    pAction = pMenu->insertSeparator( pMenu->actions().first() );
    pAction = new QAction( QPixmap(), tr( "Export..." ), this );
    connect( pAction, SIGNAL(triggered()), SLOT(slotExport()) );
    pMenu->insertAction( pMenu->actions().first(), pAction );

    pAction = pMenu->insertSeparator( pMenu->actions().first() );
    pAction = new QAction( QPixmap(), tr( "Import..." ), this );
    connect( pAction, SIGNAL(triggered()), SLOT(slotImport()) );
    pMenu->insertAction( pMenu->actions().first(), pAction );

    pAction = pMenu->insertSeparator( pMenu->actions().first() );
    if ( isEditor() )
    {
        pAction = new QAction( QPixmap(), tr("Close Editor"), this );
        connect( pAction, SIGNAL(triggered()), SLOT(slotCloseEditor()) );
        pMenu->insertAction( pMenu->actions().first(), pAction );
    }
    else
    {
        pAction = new QAction( QPixmap(), tr( "Open Editor" ), this );
        connect( pAction, SIGNAL(triggered()), SLOT(slotOpenEditor()) );
        pMenu->insertAction( pMenu->actions().first(), pAction );
    }

    return pMenu;
}

QString DATAWSql::getSQL()
{
    return stringSQL;
}

DATAWConnection *DATAWSql::getConnection()
{
    return (DATAWConnection*)getParent( "DATAWConnection" );
}

QDomElement DATAWSql::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    if ( isEditor() ) doSyncWithEditor();

    QDomElement domElemThis = AWObject::doSave( pdomDoc, pdomElemParent );

    if ( !getSQL().isEmpty() )
    {
        ulong nLen = 0;
        domElemThis.setAttribute( "SQL", CBDPersistUtility::getEncodedText( getSQL(), &nLen ) );
        domElemThis.setAttribute( "SQLSize", QString::number( nLen ) );
    }

    return domElemThis;
}

bool DATAWSql::doLoad( QDomElement *pdomElemThis )
{
    AWObject::doLoad( pdomElemThis );
    QString s;

    QString stringAttribute = pdomElemThis->attribute( "SQLSize" );
    if ( !stringAttribute.isNull() )
    {
        ulong nLen = stringAttribute.toULong();
        stringAttribute = pdomElemThis->attribute( "SQL" );
        if ( !stringAttribute.isNull() )
            s = CBDPersistUtility::getDecodedText( stringAttribute, nLen );
    }

/*
for ( int nChar = 0; nChar < s.size(); nChar++ )
{
    QChar cIn = s.at( nChar );
    QChar cOut;
    if ( cIn == QChar( ' ' ) )
        cOut = '.';
    else if ( cIn == QChar( '\t' ) )
        cOut = 't';
    else if ( cIn == QChar( '\n' ) )
        cOut = 'n';
    else if ( cIn == QChar( '\r' ) )
        cOut = 'r';
    else
        cOut = cIn;
    printf( "[PAH][%s][%s][%d] %d: %c\n", __FILE__, __FUNCTION__, __LINE__, nChar, cOut.toLatin1() );
}
*/
    // in some cases (not sure why) we get a lot of white space added to end
    setSQL( s.trimmed() );

    return true;
}

void DATAWSql::doGoTo( ADObjectPathList listObjectPathItems, uint nDepth )
{
    if ( !isEditor() )
    { 
        if ( !slotOpenEditor() ) return;
    }
    getEditorWidget()->doGoTo();

    ADObject::doGoTo( listObjectPathItems, nDepth );
}

void DATAWSql::doGoToView( ADObjectPathList listObjectPathItems, uint nDepth )
{
    if ( !isEditor() )
    { 
        if ( !slotOpenEditor() ) return;
    }
    getEditorWidget()->doGoTo();

    if ( int(nDepth) >= listObjectPathItems.count() )
    {
        slotObjectDialog();
        return;
    }

    //
    ADObject::doGoToView( listObjectPathItems, nDepth );
}

void DATAWSql::doGoTo()
{
    // recurse our way to root
    if ( parent() )
    { 
        AWObject *p = (AWObject*)parent();
        p->doGoTo();
    }

    // do what we can on the way back down
    if ( isEditor() ) getEditorWidget()->doGoTo();
}

void DATAWSql::doFind( ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems, const ADFindCriteria &Criteria )
{
    if ( Criteria.bOther )
    {
        // todo: handle Criteria.bCompleteWord
        Qt::CaseSensitivity nCase = ( Criteria.bCaseSenstive ? Qt::CaseSensitive : Qt::CaseInsensitive );

        // to get line and column search list?
        // QStringList listStrings = QString.split( QRegExp( "[\r\n]" ), QString::SkipEmptyParts );

        // do repeat calls to indexOf to get multiple occurrences
        // qsizetype QString::indexOf(const QString &str, qsizetype from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive)

        // use contains() to keep it simple - for now
        if ( getSQL().contains( Criteria.stringValue, nCase ) ) eventOutputFind( listObjectPathItems, getIcon(), Criteria.stringValue );
    }
    AWObject::doFind( nValueType, listObjectPathItems, Criteria );                                                                 
}

void DATAWSql::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    // todo: validate our SQL
    AWObject::doValidate( nStrictness, nValueType, listObjectPathItems );
}

void DATAWSql::doPreSubmit()
{
    eventOutputRequest( "Data Diagnostics", ADEventRequest::RequestClear );
    eventOutputRequest( "Data Messages", ADEventRequest::RequestClear );
    eventOutputRequest( "Data Messages", ADEventRequest::RequestFocus );
}

bool DATAWSql::slotOpenEditor()
{
    if ( pEditorWidget ) return true;

    // create view
    DATAWSqlEditorWidget *pEditor;
    pEditorWidget = pEditor = new DATAWSqlEditorWidget( this, getEditorWidgetParent() );

    pEditor->setSQL( stringSQL );

    connect( pEditor, SIGNAL(signalModified(bool)), SLOT(slotModified(bool)) );

    emit signalEditorOpened( pEditorWidget );
    emit signalEditorOpened( this, pEditorWidget );

    return true;
}

bool DATAWSql::slotCloseEditor()
{
    if ( !isEditor() ) return true;
    doSyncWithEditor();
    return AWObject::slotCloseEditor();
}

void DATAWSql::doClear()
{
    stringSQL.clear();
    AWObject::doClear(); // some of this is relevant
}

void DATAWSql::doSyncWithEditor()
{
    AWEditorWidget *p = getEditorWidget(); 
    if ( p ) 
    {
        DATAWSqlEditorWidget *p2 = (DATAWSqlEditorWidget*)p;
        stringSQL = p2->getSQL();
    }
}

void DATAWSql::slotModified( bool b )
{
    setModified( b );
}

void DATAWSql::slotExport()                                                                                                                                                                                 
{
    if ( isEditor() ) doSyncWithEditor();
                               
    QString stringExtension = ".sql";
    QString stringFileSpec =  "SQL (*.sql)";

    // get file name                                                                                                                                                                                                                      
    QString stringFileName = QFileDialog::getSaveFileName( 0, tr( "Export..." ), QString(), stringFileSpec );                                                                                                                                      
    if ( stringFileName.isEmpty() ) return;

    // append extension                                                                                                                                                                                                                   
    if ( stringFileName.right( stringExtension.length() ) != stringExtension ) stringFileName += stringExtension;                                                                                                                                                                                             
                                                                                                                                                                                                                                          
    QFileInfo fileinfo( stringFileName );                                                                                                                                                                                                 
    stringFileName = fileinfo.absoluteFilePath();                                                                                                                                                                                         
                                                                                                                                                                                                                                          
    // write data                                                                                                                                                                                                                         
    // create
    QFile file( stringFileName );
    if ( !file.open( QIODevice::WriteOnly ) )
    {
        doMessageBox( "WARNING", LIB_NAME, "Could not open " + file.fileName() );
        return;
    }
    // write
    file.write( stringSQL.toUtf8(), stringSQL.length() );

    // close
    file.close();
}
                                                                                                                                                                                                                                          
void DATAWSql::slotImport()                                                                                                                                                                                 
{
    if ( isEditor() ) doSyncWithEditor();
                               
    QString stringExtension = ".sql";
    QString stringFileSpec =  "SQL (*.sql)";

    // get file name                                                                                                                                                                                                                      
    QString stringFileName = QFileDialog::getOpenFileName( 0, tr( "Import..." ), QString(), stringFileSpec );                                                                                                                                      
    if ( stringFileName.isEmpty() ) return;

    // append extension                                                                                                                                                                                                                   
    if ( stringFileName.right( stringExtension.length() ) != stringExtension ) stringFileName += stringExtension;                                                                                                                                                                                             
                                                                                                                                                                                                                                          
    QFileInfo fileinfo( stringFileName );                                                                                                                                                                                                 
    stringFileName = fileinfo.absoluteFilePath();                                                                                                                                                                                         
                                                                                                                                                                                                                                          
    // open                                                                                                                                                                                                                                          
    QFile file( stringFileName );
    if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        doMessageBox( "WARNING", LIB_NAME, "Could not open " + file.fileName() );
        return;
    }

    // read
    QTextStream streamIn( &file ); 
    stringSQL += streamIn.readAll();

    // close
    file.close();

    if ( isEditor() )
    {
        DATAWSqlEditorWidget *pEditor = (DATAWSqlEditorWidget *)pEditorWidget;
        pEditor->setSQL( stringSQL );
    }

    setModified();
}
                                                                                                                                                                                                                                          

