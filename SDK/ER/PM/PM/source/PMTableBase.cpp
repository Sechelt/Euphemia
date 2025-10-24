#include "LibInfo.h"

#include <WTabDialog.h>
#include <ADTransaction.h>
#include <AWObjectSelectWidget.h>
#include <AWObjectNameWidget.h>
#include <AWObjectUsesWidget.h>
#include <AWObjectUsedByWidget.h>
#include <AWObjectChildrenWidget.h>

#include "PMTableBase.h"
#include "PMModel.h" 
#include "PMColumn.h"
#include "PMKey.h"
#include "PMReference.h"
#include "PMInherit.h"
#include "PMDomain.h"
#include "PMRule.h"

#include "PMDDLWidget.h"
#include "PMNotesWidget.h"
#include "PMAdditionalChecksWidget.h"
#include "PMColumnsWidget.h"


PMTableBase::PMTableBase( PMModel *pmodel )
    : PMRectangle( pmodel ), PMDDLManager( this )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    bGenerate       = true;

    mapMeta.clear(); 

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

PMTableBase::~PMTableBase()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

QList<PMColumn*> PMTableBase::getColumns()
{
    QList<PMColumn*> l;

    QList<ADObject*>    listColumns = getObjects( QString(), QString(), "PMColumn" );
    ADObject *          pObject;

    foreach( pObject, listColumns )
    {
        l.append( (PMColumn*)pObject );
    }

    return l;
}

QList<PMKey*> PMTableBase::getKeys()
{
    QList<PMKey*>       l;
    QList<ADObject*>    listKeys = getObjects( QString(), QString(), "PMKey" );
    ADObject *          pObject;
    foreach( pObject, listKeys )
    {
        l.append( (PMKey*)pObject );
    }

    return l;
}

QList<PMKey*> PMTableBase::getAlternateKeys()
{
    QList<PMKey*>       l;
    QList<ADObject*>    listKeys = getObjects( QString(), QString(), "PMKey" );
    ADObject *          pObject;
    foreach( pObject, listKeys )
    {
        PMKey *pKey = (PMKey*)pObject;
        if ( !pKey->isPrimary() ) l.append( pKey );
    }

    return l;
}

PMKey *PMTableBase::getPrimaryKey()
{
    QList<ADObject*> listKeys = getObjects( QString(), QString(), "PMKey" );
    ADObject *pObject;

    foreach( pObject, listKeys )
    {
        PMKey *pKey = (PMKey*)pObject;
        if ( pKey->isPrimary() ) return pKey;
    }

    return nullptr;
}

/*!
 * \brief Get foreign key columns. 
 *  
 * A column is a Foreign Key when it is flagged as such. 
 * Primary and Alternate key columns are obtained via PMKey. 
 * 
 * \author pharvey (5/20/20)
 * 
 * \return QList&lt;PMColumn*&gt; 
 */
QList<PMColumn*> PMTableBase::getForeignKeyColumns()
{
    QList<PMColumn*> l;

    QList<ADObject*>    listColumns = getObjects( QString(), QString(), "PMColumn" );
    ADObject *          pObject;

    foreach( pObject, listColumns )
    {
        PMColumn *pColumn = (PMColumn*)pObject;
        if ( pColumn->isForeignKey() ) l.append( pColumn );
    }

    return l;
}

/*!
    Returns the PMColumn object which is a child of 'this' and has stringName.
*/
PMColumn * PMTableBase::getColumn( const QString &stringName )
{
    QList<ADObject*>  listColumns = getObjects( stringName, QString(), "PMColumn" );
    if ( listColumns.isEmpty() ) return nullptr;
    return (PMColumn *)listColumns.first();
}

AWPropWidget *PMTableBase::getPropWidget( QWidget *pWidgetParent )
{
    // skip the drawing stuff that DRectangle puts in...
    AWPropWidget *pPropWidget = AWObject::getPropWidget( pWidgetParent );

    // replace 'Text' added by DRectangle
    // - justify and alignment are not needed 
/*
    pPropWidget->doRemoveItem( tr("Text") );                                              
    {                                                                                     
        QWidget *pWidget = new QWidget( pPropWidget );                                    
        QVBoxLayout *pLayoutTop = new QVBoxLayout( pWidget );                             
        QFormLayout *pLayout = new QFormLayout();                                         
                                                                                          
        QComboBox *pDisplay = new QComboBox( pWidget );                                   
        pDisplay->addItem( "Name", (int)TextDisplayName );                                
        pDisplay->addItem( "Code", (int)TextDisplayCode );                                
        pDisplay->addItem( "Comment", (int)TextDisplayComment );                          
        pDisplay->addItem( "None", (int)TextDisplayNone );                                
        WSetComboBox( pDisplay, nTextDisplay );                                           
        pLayout->addRow( tr( "Display:" ), pDisplay );                                    
        connect( pDisplay, SIGNAL(currentIndexChanged(int)), SLOT(slotTextDisplay(int)) );
                                                                                          
        pLayoutTop->addLayout( pLayout );                                                 
        pLayoutTop->addStretch( 10 );                                                     
                                                                                          
        pPropWidget->addItem( pWidget, tr("Text") );                                      
    }                                                                                     
*/

    return pPropWidget;

}

QDomElement PMTableBase::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = PMRectangle::doSave( pdomDoc, pdomElemParent );
    doSavePMDDLManager( pdomDoc, &domElemThis );

    if ( getGenerate() ) domElemThis.setAttribute( "Generate", 1 );
    if ( !getRecords().isEmpty() ) domElemThis.setAttribute( "Records", getRecords() );

    return domElemThis;
}

/*!
    Load ourself from the XML.
*/
bool PMTableBase::doLoad( QDomElement *pdomElemThis )
{
    PMRectangle::doLoad( pdomElemThis );
    doLoadPMDDLManager( pdomElemThis );

    bGenerate = pdomElemThis->attribute( "Generate", "0" ).toInt();
    stringRecords = pdomElemThis->attribute( "Records" );

    // load child nodes
    QDomElement         domElem;
    QDomNode            domNode;
    domNode = pdomElemThis->firstChild();
    while( !domNode.isNull() ) 
    {
        domElem = domNode.toElement();
        if( !domElem.isNull() ) 
        {
            if ( domElem.tagName() == "PMColumn" )
            {
                PMColumn *pColumn = (PMColumn*)getObject( "PMColumn" );
                pColumn->doLoad( &domElem );
            }
        }
        domNode = domNode.nextSibling();
    } // while more children

    return true;
}

void PMTableBase::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )                                
{                                                                                                                                       
    Q_UNUSED( nOption );
    Q_UNUSED( pWidget );

    PMModel *pModel = (PMModel*)getParent( "PMModel" );

    pPainter->setPen( pen );
    pPainter->setBrush( brush );
    pPainter->setFont( font.font );

    int nLineHeight = pPainter->fontMetrics().height() + 2;

    // a font we can play with...
    // QFont font = pPainter->font();

    //
    // ENTITY
    // Draw the rectangle of the entity.
    //
    QRectF rectBounding  = boundingRect();
    pPainter->drawRect( rectBounding );
    pPainter->drawLine( rectBounding.left(), rectBounding.top() + nLineHeight, rectBounding.right(), rectBounding.top() + nLineHeight );

    //
    // ENTITY NAME
    // Draw the entity name.
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
    int nDataTypeW;
    int nKeyW;
    getColumnWidths( pPainter, &nNameW, &nDataTypeW, &nKeyW );

    rectName.setWidth( nNameW );
    QRectF rectDataType( rectName.x() + nNameW, rectName.y(), nDataTypeW, rectName.height() );
    QRectF rectKey( rectDataType.x() + nDataTypeW, rectName.y(), nKeyW, rectName.height() );

    //
    // SHRINK WIDTHS
    // Work right to left.
    // Shrink widths when not enough space.
    // Widths are set to 0 when the field has to be 'sacraficed' due to lack of horizontal space. 
    //

    // key
    if ( rectKey.left() > rectBounding.right() ) rectKey.setWidth( 0 );
    else if ( rectKey.right() > rectBounding.right() ) rectKey.setWidth( rectKey.width() - (rectKey.right() - rectBounding.right()) );

    // data type
    if ( rectDataType.left() > rectBounding.right() ) rectDataType.setWidth( 0 );
    else if ( rectDataType.right() > rectBounding.right() ) rectDataType.setWidth( rectDataType.width() - (rectDataType.right() - rectBounding.right()) );

    // name
    if ( rectName.left() > rectBounding.right() ) rectName.setWidth( 0 );
    else if ( rectName.right() > rectBounding.right() ) rectName.setWidth( rectName.width() - (rectName.right() - rectBounding.right()) );

    //
    // COLUMNS
    // Draw the Columns. Each contains up to 3 fields/rectangles; name, type and key.
    // Move the boxs down as we go.
    // Stop early if we run out space. In such a case - draw a '+' to give indication.
    //
    {
        QList<ADObject*>    listColumns = getObjects( QString(), QString(), "PMColumn" );
        ADObject *          pObject;
        foreach( pObject, listColumns )
        {
            PMColumn *pColumn = (PMColumn*)pObject;

            pPainter->setFont( font.font );

            // move down 1 row
            rectName.moveTo( rectName.x(), rectName.y() + nLineHeight );
            rectDataType.moveTo( rectDataType.x(), rectDataType.y() + nLineHeight );
            rectKey.moveTo( rectKey.x(), rectKey.y() + nLineHeight );

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
                rectDataType.setBottom( rectBounding.bottom() );
                rectKey.setBottom( rectBounding.bottom() );
            }

            // form key
            QString stringKey( "" );
            if ( pColumn->isPrimaryKey() ) stringKey = "pk";
            if ( pColumn->isAlternateKey() ) stringKey += ( stringKey.isEmpty() ? "ak" : ",ak" );
            if ( pColumn->isForeignKey() ) stringKey += ( stringKey.isEmpty() ? "fk" : ",fk" );
            if ( pColumn->isMandatory() )  stringKey += ( stringKey.isEmpty() ? "M" : ",M" );

            // draw text
            pPainter->drawText( rectName, Qt::AlignLeft, pColumn->getName() );
            pPainter->drawText( rectDataType, Qt::AlignLeft, pColumn->getDataTypeToDisplay( pModel->getDisplayFlags() ) );
            pPainter->drawText( rectKey, Qt::AlignRight, stringKey );

// this is great for debugging
/*
            pPainter->setPen( Qt::red );       
            pPainter->drawRect( rectName );    
            pPainter->setPen( Qt::green );     
            pPainter->drawRect( rectDataType );
            pPainter->setPen( Qt::blue );      
            pPainter->drawRect( rectKey );     
*/

        } // while
    }

    // this is where we left off
    qreal nY = rectName.bottom();

    //
    // KEYS
    // Draw the key names.
    // Do in reverse order starting at bottom of entity.
    // Stop early if we run out space. In such a case - draw a '+' to give indication.
    //
    {
        rectName.moveTo( rectName.x(), rectBounding.bottom() );
        rectKey.moveTo( rectKey.x(), rectBounding.bottom() );

        QList<ADObject*>    listKeys = getObjects( QString(), QString(), "PMKey" );
        QListIterator<ADObject*> i( listKeys );
        i.toBack();
        while ( i.hasPrevious() )
        {
            PMKey *pKey = (PMKey*)i.previous();

            pPainter->setFont( font.font );
            rectName.moveTo( rectName.x(), rectName.y() - nLineHeight );
            rectKey.moveTo( rectKey.x(), rectKey.y() - nLineHeight );

            if ( rectName.top() < nY )
            {
                pPainter->drawText( QRectF( rectBounding.left(), rectBounding.bottom() - nLineHeight, rectBounding.width(), nLineHeight ), Qt::AlignCenter, "+" );
                pPainter->setPen( pen );
                pPainter->drawLine( rectBounding.left(), rectName.bottom(), rectBounding.right(), rectName.bottom() );
                pPainter->setPen( font.color );
                return;
            }

            pPainter->drawText( rectName, Qt::AlignLeft, pKey->getName() );
            if ( pKey->isPrimary() ) pPainter->drawText( rectKey, Qt::AlignRight, "pk" );
            else pPainter->drawText( rectKey, Qt::AlignRight, "ak" );

        } // while

        pPainter->setPen( pen );
        pPainter->drawLine( rectBounding.left(), rectName.top(), rectBounding.right(), rectName.top() );
        pPainter->setPen( font.color );
    }
}

void PMTableBase::slotDelete()
{
    {
        // delete all keys - this can trigger a lot of cleanup with PMReference objects etc
        // and this is good - we want it done sooner rather than later
        QList<ADObject*> l = getObjects( QString(), QString(), "PMKey" );
        ADObject *p;
        foreach( p, l )
        {
            PMKey *pKey = (PMKey*)p;
            pKey->slotDelete();
        }
    }

/*
    {                                                                       
        QList<ADObject*> l = getObjects( QString(), QString(), "PMColumn" );
        ADObject *p;                                                        
        foreach( p, l )                                                     
        {                                                                   
            PMColumn *pColumn = (PMColumn*)p;                               
            pColumn->slotDelete();                                          
        }                                                                   
    }                                                                       
*/

    PMRectangle::slotDelete();
}

QSizeF PMTableBase::getOptimalSize()
{
    PMModel *    pModel = (PMModel*)getParent( "PMModel" );
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
        int nWidthDataType = 1;
        int nWidthKey      = 1;

        QList<ADObject*>    listColumns = getObjects( QString(), QString(), "PMColumn" );
        ADObject *          pObject;

        foreach( pObject, listColumns )
        {
            PMColumn *pColumn = (PMColumn*)pObject;

            int n;

            n = FontMetrics.boundingRect( pColumn->getName() ).width();
            if (  n > nWidthName ) nWidthName = n;
            n = FontMetrics.boundingRect( pColumn->getDataTypeToDisplay( pModel->getDisplayFlags() ) ).width();
            if ( n > nWidthDataType ) nWidthDataType = n;

            QString stringKey( "" );
            if ( pColumn->isPrimaryKey() ) stringKey = "pk";
            if ( pColumn->isAlternateKey() ) stringKey += ( stringKey.isEmpty() ? "ak" : ",ak" );
            if ( pColumn->isForeignKey() ) stringKey += ( stringKey.isEmpty() ? "fk" : ",fk" );
            if ( pColumn->isMandatory() )  stringKey += ( stringKey.isEmpty() ? "M" : ",M" );
            n = FontMetrics.boundingRect( stringKey ).width();
            if ( n > nWidthKey ) nWidthKey = n;
        }

        nWidthName     += 4;
        nWidthDataType += 4;
        nWidthKey      += 4;

        sizeColumns.setWidth( nWidthName + nWidthDataType + nWidthKey );
        sizeColumns.setHeight( listColumns.count() * (FontMetrics.height() + 2) );
    }

    // Keys
    {
        int nWidthName     = 1;
        int nWidthKey      = 1;

        QList<ADObject*>    listKeys = getObjects( QString(), QString(), "PMKey" );
        ADObject *          pObject;

        foreach( pObject, listKeys )
        {
            PMKey *pKey = (PMKey*)pObject;

            if ( FontMetrics.boundingRect( pKey->getName() ).width() > nWidthName )
                nWidthName = FontMetrics.boundingRect( pKey->getName() ).width();
        }

        nWidthName     += 4;
        nWidthKey      += 4;

        if ( sizeColumns.width() < (nWidthName + nWidthKey) )
            sizeColumns.setWidth( nWidthName + nWidthKey );
        sizeColumns.setHeight( sizeColumns.height() + listKeys.count() * (FontMetrics.height() + 2) );
    }

    // OPTIMAL SIZE
    size.setWidth( qMax( sizeCaption.width(), sizeColumns.width() ) + nMargin );
    size.setHeight( sizeCaption.height() + sizeColumns.height() + nMargin );

    return size;
}

void PMTableBase::getColumnWidths( QPainter *pPainter, int *pnName, int *pnDataType, int *pnKey )
{
    PMModel *    pDiagram    = (PMModel*)getParent( "PMModel" );

    QFontMetrics  fontmetrics = pPainter->fontMetrics();

    *pnName     = 1;
    *pnDataType = 1;
    *pnKey      = 1;

    // columns
    {
        QList<ADObject*> listColumns = getObjects( QString(), QString(), "PMColumn" );
        ADObject *      pObject;

        foreach( pObject, listColumns )
        {
            PMColumn *pColumn = (PMColumn*)pObject;

            int n = fontmetrics.boundingRect( pColumn->getName() ).width();
            if ( n > *pnName ) *pnName = n;

            n = fontmetrics.boundingRect( pColumn->getDataTypeToDisplay( pDiagram->getDisplayFlags() ) ).width();
            if ( n > *pnDataType ) *pnDataType = n;

            QString stringKey( "" );
            if ( pColumn->isPrimaryKey() ) stringKey = "pk";
            if ( pColumn->isAlternateKey() ) stringKey += ( stringKey.isEmpty() ? "ak" : ",ak" );
            if ( pColumn->isForeignKey() ) stringKey += ( stringKey.isEmpty() ? "fk" : ",fk" );
            if ( pColumn->isMandatory() )  stringKey += ( stringKey.isEmpty() ? "M" : ",M" );
            n = fontmetrics.boundingRect( stringKey ).width();
            if ( n > *pnKey ) *pnKey = n;
        }
    }
    
    // Keys
    {
        QList<ADObject*>    listKeys = getObjects( QString(), QString(), "PMKey" );
        ADObject *          pObject;

        foreach( pObject, listKeys )
        {
            PMKey *pKey = (PMKey*)pObject;

            int n = fontmetrics.boundingRect( pKey->getName() ).width();
            if ( n > *pnName ) *pnName = n;
        }
    }
    
    *pnName     += 4;
    *pnDataType += 4;
    *pnKey      += 4;
}

void PMTableBase::doLoadDefaults()
{
/*
    CBDSettings         settings;                                        
    QString             stringPath = "/CodeByDesign/LM/Defaults/Entity/";
    QPen                pen;                                             
    QBrush              brush;                                           
                                                                         
                                                                         
    pen.setColor( "#0000ff" );                                           
    brush.setColor( "#9932cc" );                                         
    brush.setStyle( Qt::SolidPattern );                                  
    pen = settings.readLinePrefEntry( stringPath, pen );                 
    brush = settings.readFillPrefEntry( stringPath, brush);              
                                                                         
    setPen(pen);                                                         
    setBrush( brush );                                                   
    setDefaultsLoaded( true );                                           
*/
}

/*!
    doGoToView
    
    Handle a request to 'go to view'. In this case we invoke our Properties dialog
    unless the request is not for us - in which case we pass it on to our child objects.
*/
void PMTableBase::doGoToView( ADObjectPathList listObjectPathItems, uint nDepth )
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

