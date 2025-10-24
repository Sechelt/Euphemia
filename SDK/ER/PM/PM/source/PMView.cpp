#include "LibInfo.h"
#include "PMView.h"

#include <ADTransaction.h>
#include <AWObjectNameWidget.h>
#include <AWObjectUsesWidget.h>
#include <AWObjectUsedByWidget.h>
#include <AWObjectChildrenWidget.h>

#include "PMViewColumn.h"
#include "PMViewWidget.h"
#include "PMDDLWidget.h"
#include "PMNotesWidget.h"
#include "PMRulesUsedWidget.h"

PMView::PMView( PMModel *pParent )
    : PMRectangle( pParent ), PMDDLManager( this )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    mapMeta.clear();
    mapMeta.insert( "PMViewColumn", ADMeta( QIcon(), "PMViewColumn", "Column" ) );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

PMView::~PMView()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

void PMView::setDef( const QString &s )
{
    if ( s == stringDef ) return;
    stringDef = s;
    doParse();
    setModified();
}

QPixmap PMView::getIcon()
{
    return QPixmap( ":PM/View" );
}

AWDataWidget *PMView::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = PMRectangle::getObjectWidget( pWidgetParent );
    
    p->insertTab( 1, new PMRulesUsedWidget( this, p ), tr("Rules") );
    p->insertTab( 1, new PMNotesWidget( this, p ), tr("Notes") );
    p->insertTab( 1, new PMDDLWidget( this, getCreateAuto(), getDropAuto(), p ), tr("DDL") );
    p->insertTab( 1, new PMViewWidget( this, p ), tr("Definition") );

    p->setCurrentIndex( 0 );

    return p;
}

ADObject *PMView::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "PMViewColumn" )
        p = new PMViewColumn( this );

    if ( !p )
    {
        doMessageBox( "WARNING", "Create Object...", "Unknown class requested " + s );
        return nullptr;
    }

    // announce the birth of a new child
    emit signalCreated( p );    
    emit signalCreated( this, p );
    // set parent modified by default (can be set back if this is a load)           
    setModified();              

    return p;
}

QString PMView::getCreateAuto()
{
    PMModel *pModel = (PMModel*)getParent( "PMModel" );
    if ( !isIncludeCreate() || !pModel->hasFeature( PMModel::FeatureView, PMModel::FeatureSupportCreate ) ) return "";

    return "CREATE VIEW " + getName() + " AS \n" + getDef() + ";\n";
}

QString PMView::getDropAuto()
{
    PMModel *pModel = (PMModel*)getParent( "PMModel" );
    if ( !isIncludeDrop() || !pModel->hasFeature( PMModel::FeatureView, PMModel::FeatureSupportDrop ) ) return "";

    return "DROP VIEW " + getName() + ";\n";
}

ADValidationDatabase PMView::getValidationConfiguration( ADValidationDatabase &mapClasses )
{
    // add ourself
    ADValidationStrictness mapStrictness;
    // Low
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Name missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Code missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Comment missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Description missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Name duplicate" ), ADValidation::Information );
        mapValidations.insert( tr( "Code duplicate" ), ADValidation::Information );
        mapStrictness.insert( ADValidation::Low, mapValidations );
    }

    // Medium:
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Name missing" ), ADValidation::Warning );
        mapValidations.insert( tr( "Code missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Comment missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Description missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Name duplicate" ), ADValidation::Warning );
        mapValidations.insert( tr( "Code duplicate" ), ADValidation::Warning );
        mapStrictness.insert( ADValidation::Medium, mapValidations );
    }

    // High:
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Name missing" ), ADValidation::Error );
        mapValidations.insert( tr( "Code missing" ), ADValidation::Warning );
        mapValidations.insert( tr( "Comment missing" ), ADValidation::Warning );
        mapValidations.insert( tr( "Description missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Name duplicate" ), ADValidation::Error );
        mapValidations.insert( tr( "Code duplicate" ), ADValidation::Error );
        mapStrictness.insert( ADValidation::High, mapValidations );
    }
    mapClasses.insert( "PMView", mapStrictness );

    return mapClasses;
}

QDomElement PMView::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = PMRectangle::doSave( pdomDoc, pdomElemParent );
    doSavePMDDLManager( pdomDoc, &domElemThis );

    if ( !stringDef.isEmpty() )
    { 
        ulong nLen = 0;
        domElemThis.setAttribute( "Def", CBDPersistUtility::getEncodedText( stringDef, &nLen ) );
        domElemThis.setAttribute( "DefSize", QString::number( nLen ) );
    }

    return domElemThis;
}

bool PMView::doLoad( QDomElement *pdomElemThis )
{
    PMRectangle::doLoad( pdomElemThis );
    doLoadPMDDLManager( pdomElemThis );

    QString s = pdomElemThis->attribute( "Def" );
    ulong   n = pdomElemThis->attribute( "DefSize", "0" ).toULong();
    if ( !s.isEmpty() && n > 0 ) stringDef = CBDPersistUtility::getDecodedText( s, n ); 

    return true;
}

void PMView::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    QString stringValidation;
    ADValidation::Severity nSeverity;

    // Name...                                                                                                                        
    stringValidation = tr("Name missing");
    nSeverity = getValidationSeverity( "PMView", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getName().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation );                                         

    // Code...                                                                                                                        
    stringValidation = tr("Code missing");
    nSeverity = getValidationSeverity( "PMView", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getCode().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation );                                         

    // Comment...                                                                                                                     
    stringValidation = tr("Comment missing");
    nSeverity = getValidationSeverity( "PMView", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getComment().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Duplicate...
    ADObject *pObjectParent = (ADObject*)getParent( "ADObject" );
    if ( pObjectParent )
    { 
        stringValidation = tr("Name duplicate");
        nSeverity = getValidationSeverity( "PMView", nStrictness, stringValidation );                                                          
        if ( nSeverity != ADValidation::Ignore && !getName().isEmpty() && pObjectParent->getDuplicate( this, ADObject::FieldName ) )                                                                                                
            eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

        stringValidation = tr("Code duplicate");                                                                         
        nSeverity = getValidationSeverity( "PMView", nStrictness, stringValidation );                                                          
        if ( nSeverity != ADValidation::Ignore && !getCode().isEmpty() && pObjectParent->getDuplicate( this, ADObject::FieldCode ) )                                                                                                
            eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                
    }
                 
    // check our children                                                                                                              
    QList<ADObject*>    listChildren = getObjects( QString(), QString(), QString() );                                            
    ADObject *          pObject;
    int                 nIndex = -1;                                                                                           

    foreach( pObject, listChildren )
    {                                                                                                                                  
        nIndex++;                                                                                                                      

        // append child to path                                                                                                        
        listObjectPathItems += ADObjectPathItem( nValueType, pObject, pObject->metaObject()->className(), pObject->getName(), nIndex );
        // validate child                                                                                                              
        pObject->doValidate( nStrictness, nValueType, listObjectPathItems );                                                                        
        // remove child from path                                                                                                      
        listObjectPathItems.pop_back();                                                                                                
    }                                                                                                                                  
}

void PMView::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )                                    
{                                                                                                                                       
    Q_UNUSED( nOption );
    Q_UNUSED( pWidget );

    pPainter->setPen( pen );
    pPainter->setBrush( brush );
    pPainter->setFont( font.font );

    int nLineHeight = pPainter->fontMetrics().height() + 2;

    // a font we can play with...
    // QFont font = pPainter->font();

    //
    // VIEW
    // Draw the rectangle of the view.
    //
    QRectF rectBounding  = boundingRect();
    pPainter->drawRoundedRect( rectBounding, 5, 5 );
    pPainter->drawLine( rectBounding.left(), rectBounding.top() + nLineHeight, rectBounding.right(), rectBounding.top() + nLineHeight );
                                                                                                                                        
    //
    // VIEW NAME
    // Draw the view name.
    //
    QRectF rectName;
    font.font.setBold( true );
    pPainter->setFont( font.font );
    pPainter->setPen( font.color );
    rectName.setRect( rectBounding.left() + 2, rectBounding.top(), rectBounding.width() - 2, nLineHeight );
    pPainter->drawText( rectName, getTextFlags(), getName() );
    font.font.setBold( false );
    pPainter->setFont( font.font );

    //
    // DESIRED WIDTHS
    // Init the field rectangles to height and max needed widths.
    //

    // get max needed widths...
    int nNameW;
    getColumnWidths( pPainter, &nNameW );

    rectName.setWidth( nNameW );

    //
    // SHRINK WIDTHS
    // Shrink widths when not enough space.
    //

    // name
    if ( rectName.left() > rectBounding.right() ) rectName.setWidth( 0 );
    else if ( rectName.right() > rectBounding.right() ) rectName.setWidth( rectName.width() - (rectName.right() - rectBounding.right()) );

    //
    // COLUMNS
    // Draw the Columns. Each contains up to 2 fields/rectangles; name, and type.
    // Move the boxs down as we go.
    // Stop early if we run out space. In such a case - draw a '+' to give indication.
    //
    {
        QList<ADObject*>    listColumns = getObjects( QString(), QString(), "PMViewColumn" );
        ADObject *          pObject;
        foreach( pObject, listColumns )
        {
            PMViewColumn *pColumn = (PMViewColumn*)pObject;

            pPainter->setFont( font.font );

            // move down 1 row
            rectName.moveTo( rectName.x(), rectName.y() + nLineHeight );

            // are we completely past the bottom of the table object?
            if ( rectName.bottom() >= rectBounding.bottom() )
            {
                pPainter->drawText( QRectF( rectBounding.left(), rectBounding.bottom() - nLineHeight, rectBounding.width(), nLineHeight ), Qt::AlignCenter, "+" );
                return;
            }

            // are we partially past the bottom of the table object?
            if ( rectName.bottom() > rectBounding.bottom() )
            {
                rectName.setBottom( rectBounding.bottom() );
            }

            // draw text
            pPainter->drawText( rectName, Qt::AlignLeft, pColumn->getName() );

        } // while
    }
}                                                                                                                                       

QSizeF PMView::getOptimalSize()
{
    QFontMetrics FontMetrics( font.font );
    QSizeF       size;
    QSizeF       sizeCaption;
    QSizeF       sizeColumns;
    int          nMargin         = 6;

    // CAPTION
    {
        font.font.setBold( true );
        QFontMetrics FontMetrics( font.font );
        sizeCaption.setWidth( FontMetrics.boundingRect( getName() ).width() );
        sizeCaption.setHeight( FontMetrics.height() + 2 );
        font.font.setBold( false );
    }

    // Columns
    {
        int nWidthName     = 1;
 
        QList<ADObject*>    listColumns = getObjects( QString(), QString(), "PMViewColumn" );
        ADObject *          pObject;

        foreach( pObject, listColumns )
        {
            PMViewColumn *pColumn = (PMViewColumn*)pObject;

            int n;
            n = FontMetrics.boundingRect( pColumn->getName() ).width();
            if (  n > nWidthName ) nWidthName = n;

        }

        nWidthName     += 4;

        sizeColumns.setWidth( nWidthName );
        sizeColumns.setHeight( listColumns.count() * (FontMetrics.height() + 2) );
    }

    // OPTIMAL SIZE
    size.setWidth( qMax( sizeCaption.width(), sizeColumns.width() ) + nMargin );
    size.setHeight( sizeCaption.height() + sizeColumns.height() + nMargin );

    return size;
}

void PMView::getColumnWidths( QPainter *pPainter, int *pnName )
{
    QFontMetrics  fontmetrics = pPainter->fontMetrics();

    *pnName     = 1;

    // columns
    {
        QList<ADObject*> listColumns = getObjects( QString(), QString(), "PMViewColumn" );
        ADObject *      pObject;

        foreach( pObject, listColumns )
        {
            PMViewColumn *pColumn = (PMViewColumn*)pObject;

            int n = fontmetrics.boundingRect( pColumn->getName() ).width();
            if ( n > *pnName ) *pnName = n;
        }
    }
    
    *pnName     += 4;
}

void PMView::doParse()
{
    // start with a clean slate
    doObjectDelete( QString(), QString(), "PMViewColumn" );

    //
    stringDef = stringDef.trimmed();
    if ( stringDef.isEmpty() ) return;

    int n1;
    int n2;
    QString string;
    QStringList stringlist;
    PMViewColumn * pcolumn;

    // - try to gather column names
    // - this is simple, 1st pass...
    // - extract text between SELECT and FROM then split on ','
    n1 = stringDef.indexOf( "SELECT ", 0, Qt::CaseInsensitive );
    if ( n1 < 0 ) return;
    n1 += 7;

    n2 = stringDef.indexOf( "FROM ", n1, Qt::CaseInsensitive );
    if ( n2 < 0 )
        string = stringDef.mid( n1 );
    else
        string = stringDef.mid( n1, n2-n1 );

#if QT_VERSION < 0x060000
    stringlist = string.split( ',', QString::SkipEmptyParts );
#else
    stringlist = string.split( ',', Qt::SkipEmptyParts );
#endif
    for ( QStringList::Iterator it = stringlist.begin(); it != stringlist.end(); ++it ) 
    {
        string = (*it).trimmed();
        if ( string.isEmpty() )
            continue;

        pcolumn = (PMViewColumn*)getObject( "PMViewColumn" );
        pcolumn->setName( string );
    }

    // - try to gather table names
    // - this is simple, 2nd pass...
    // - extract text between FROM and WHERE then split on ','
    // - ignore long table names because they are certianly for table join
    n1 = stringDef.indexOf( "FROM ", 0, Qt::CaseInsensitive );
    if ( n1 < 0 )
        return;
    n1 += 5;

    n2 = stringDef.indexOf( "WHERE ", n1, Qt::CaseInsensitive );
    if ( n2 < 0 )
        string = stringDef.mid( n1 );
    else
        string = stringDef.mid( n1, n2-n1 );

#if QT_VERSION < 0x060000
    stringlist = string.split( ',', QString::SkipEmptyParts );
#else
    stringlist = string.split( ',', Qt::SkipEmptyParts );
#endif
    {
        for ( QStringList::Iterator it = stringlist.begin(); it != stringlist.end(); ++it ) 
        {
            string = (*it).trimmed();
            if ( string.isEmpty() )
                continue;
            if ( string.length() > 32 )
                continue;
            // listTables.append( string );
        }
    }
}

void PMView::doGoToView( ADObjectPathList listObjectPathItems, uint nDepth )
{
    // Are we it?
    if ( int(nDepth) >= listObjectPathItems.count() )
    {
        slotObjectDialog();
        return; 
    }

    // Keep drilling...
    ADObject::doGoToView( listObjectPathItems, nDepth );
}

