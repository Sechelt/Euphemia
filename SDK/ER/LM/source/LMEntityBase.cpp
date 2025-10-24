#include "LibInfo.h"
#include "LMEntityBase.h"

#include <WTabDialog.h>
#include <ADTransaction.h>
// #include <AWDataTable.h>
#include <AWObjectSelectWidget.h>
#include <AWObjectNameWidget.h>
#include <AWObjectUsesWidget.h>
#include <AWObjectUsedByWidget.h>
#include <AWObjectChildrenWidget.h>

#include "LMModel.h" 
#include "LMAttribute.h"
#include "LMIdentifier.h"
#include "LMRelationship.h"
#include "LMAssociativeLink.h"
#include "LMDataItem.h"
#include "LMDomain.h"
#include "LMRule.h"
#include "LMAttributesWidget.h"
#include "LMEntityOptionsWidget.h"
#include "LMNotesWidget.h"
#include "LMRulesUsedWidget.h"

LMEntityBase::LMEntityBase( LMModel *pmodel )
    : LMRectangle( pmodel )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    bGenerate       = true;

    mapMeta.clear();
    mapMeta.insert( "LMAttribute", ADMeta( QIcon(), "LMAttribute", "Attribute" ) );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

LMEntityBase::~LMEntityBase()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

AWDataWidget *LMEntityBase::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = LMRectangle::getObjectWidget( pWidgetParent );
    
    p->insertTab( 1, new LMRulesUsedWidget( this, p ), tr("Rules") );
    {
        LMNotesWidget *pWidget = new LMNotesWidget( p, getDescription(), getAnnotation() );
        connect( pWidget, SIGNAL(signalDescriptionChanged(const QString&)), SLOT(slotDescription(const QString &)) );
        connect( pWidget, SIGNAL(signalAnnotationChanged(const QString&)), SLOT(slotAnnotation(const QString &)) );
        p->insertTab( 1, pWidget, tr("Notes") );
    }

    p->insertTab( 1, new LMEntityOptionsWidget( this, p ), tr("Options") );
    p->insertTab( 1, new LMAttributesWidget( this, p ), tr("Attributes") );

    p->setCurrentIndex( 0 );

    return p;
}

AWPropWidget *LMEntityBase::getPropWidget( QWidget *pWidgetParent )
{
    // skip the drawing stuff that DRectangle puts in...
    AWPropWidget *pPropWidget = AWObject::getPropWidget( pWidgetParent );

    // replace 'Text' added by DRectangle
    // - justify and alignment are not needed 
    pPropWidget->removeWidget( tr("Text") );                                                                                 
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

        pPropWidget->addWidget( tr("Text"), pWidget );
    }

    return pPropWidget;

}

/*!
    getAttributeNames
    
    Returns a list containing the name of each child LMAttribute.
*/
QStringList LMEntityBase::getAttributeNames()
{
    QStringList listAttributeNames;

    QList<ADObject*>    listAttributes = getObjects( QString(), QString(), "LMAttribute" );
    ADObject *          pObject;

    foreach( pObject, listAttributes )
    {
        listAttributeNames.append( pObject->getName() );
    }

    return listAttributeNames;
}

/*!
    Returns a list containing the name of each child LMAttribute which has been
    flagged as an Identifier.
*/
QStringList LMEntityBase::getIdentifierNames()
{
    QStringList listAttributeNames;

    QList<ADObject*>    listAttributes = getObjects( QString(), QString(), "LMAttribute" );
    ADObject *          pObject;

    foreach( pObject, listAttributes )
    {
        LMAttribute *pAttribute = (LMAttribute*)pObject;
        if ( pAttribute->getIdentifier() )
            listAttributeNames.append( pAttribute->getName() );
    }

    return listAttributeNames;
}

/*!
    Returns the LMAttribute object which is a child of 'this' and has stringName.
*/
LMAttribute * LMEntityBase::getAttribute( const QString &stringName )
{
    QList<ADObject*>  listAttributes = getObjects( stringName, QString(), "LMAttribute" );
    LMAttribute *       pAttribute     = (LMAttribute*)listAttributes.first();

    return pAttribute;
}

ADObject *LMEntityBase::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "LMAttribute" )
        p = new LMAttribute( this );

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

QDomElement LMEntityBase::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = DRectangle::doSave( pdomDoc, pdomElemParent );
    doSaveLMObject( pdomDoc, &domElemThis );

    if ( getGenerate() ) domElemThis.setAttribute( "Generate", 1 );
    if ( !getRecords().isEmpty() ) domElemThis.setAttribute( "Records", getRecords() );

    return domElemThis;
}

/*!
    Load ourself from the XML.
*/
bool LMEntityBase::doLoad( QDomElement *pdomElemThis )
{
    DRectangle::doLoad( pdomElemThis );
    LMObject::doLoadLMObject( pdomElemThis );

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
            if ( domElem.tagName() == "LMAttribute" )
            {
                LMAttribute *pAttribute = (LMAttribute*)getObject( "LMAttribute" );
                pAttribute->doLoad( &domElem );
            }
        }
        domNode = domNode.nextSibling();
    } // while more children

    return true;
}

void LMEntityBase::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{
qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << "Got here. So I guess we need it. Better fix it. See PMEntityNormal.";
    Q_UNUSED( nOption );
    Q_UNUSED( pWidget );

    LMModel *pModel = (LMModel*)getParent( "LMModel" );

    pPainter->setPen( pen );
    pPainter->setBrush( brush );

    int nLineHeight     = pPainter->fontMetrics().height() + 2;
//    int nMarginWidth    = 0; // pPainter->fontMetrics().width( "pk " );
    QRectF rectBounding  = boundingRect();
    QRectF rectName;
    QRectF rectDataType;
    QRectF rectKey;
    QString stringKey;
    // QFont font = pPainter->font();
    
    int nNameW;
    int nDataTypeW;
    int nKeyW;

    // TABLE
    pPainter->drawRect( rectBounding );
    pPainter->drawLine( rectBounding.left(), rectBounding.top() + nLineHeight, rectBounding.right(), rectBounding.top() + nLineHeight );

    // scan data to find these values so we can optimize display space
    getAttributeWidths( pPainter, &nNameW, &nDataTypeW, &nKeyW );

    // TABLE NAME
    font.font.setBold( true );
    pPainter->setPen( font.color );
    pPainter->setFont( font.font );
    rectName.setRect( rectBounding.left() + 2, rectBounding.top(), rectBounding.width() - 2, nLineHeight );
    pPainter->drawText( rectName, getTextFlags(), getName() );
    font.font.setBold( false );
    pPainter->setFont( font.font );

    rectName.setY( rectName.y() + 3 );

    // COLUMN NAMES
 
    // display priority; Key, Name, DataType

    // always enough space for key!
    rectKey.setRect( rectBounding.right() - nKeyW, rectName.top(), nKeyW, rectName.height() );

    // is there enough space for Name?
    if ( rectKey.left() - rectName.left() < nNameW )
    {
        if ( rectKey.left() - rectName.left() < 1 )
            rectName.setWidth( 0 );
        else
            rectName.setWidth( rectKey.left() - rectName.left() );
        rectDataType.setRect( rectName.right(), rectName.top(), 0, rectName.height() );
    }
    else
        rectName.setWidth( nNameW );

    // is there enough space for DataType?
    if ( rectKey.left() - rectName.right() < nDataTypeW )
    {
        if ( rectKey.left() - rectName.right() < 1 )
            rectDataType.setRect( rectName.right(), rectName.top(), 0, rectName.height() );
        else
            rectDataType.setRect( rectName.right(), rectName.top(), rectKey.left() - rectName.right(), rectName.height() );
    }
    else
        rectDataType.setRect( rectName.right(), rectName.top(), nDataTypeW, rectName.height() );

    // COLUMNS
    QList<ADObject*>    listAttributes = getObjects( QString(), QString(), "LMAttribute" );
    ADObject *          pObject;
    int n = 0;
    foreach( pObject, listAttributes )
    {
        n++;

        LMAttribute *pAttribute = (LMAttribute*)pObject;
        if ( pAttribute->getIdentifier() )
            font.font.setUnderline( true );
        else
            font.font.setUnderline( false );

        pPainter->setFont( font.font );

        rectName.moveTo( rectName.x(), nLineHeight * n );
        rectDataType.moveTo( rectDataType.x(), nLineHeight * n );
        rectKey.moveTo( rectKey.x(), nLineHeight * n );

        // are we completely past the bottom of the table object?
        if ( rectName.bottom() >= rectBounding.bottom() )
        {
            rectName.setRect( rectBounding.left(), rectBounding.bottom(), rectBounding.width(), nLineHeight );
            pPainter->drawText( rectName, Qt::AlignCenter, "+" );
            break;
        }

        // are we partially past the bottom of the table object?
        if ( rectName.bottom() > rectBounding.bottom() )
        {
            rectName.setBottom( rectBounding.bottom() );
            rectDataType.setBottom( rectBounding.bottom() );
            rectKey.setBottom( rectBounding.bottom() );
        }

        pPainter->drawText( rectName, Qt::AlignLeft, pAttribute->getName() );
        pPainter->drawText( rectDataType, Qt::AlignLeft, pAttribute->getDataTypeToDisplay( pModel->getDisplayFlags() ) );
        stringKey = "";
        if ( pAttribute->getIdentifier() && pAttribute->getMandatory() )
            stringKey = "pi,M";
        else if ( pAttribute->getIdentifier() )
            stringKey = "pi";
        else if ( pAttribute->getMandatory() )
            stringKey = "M";
        pPainter->drawText( rectKey, Qt::AlignRight, stringKey );
    } // while
}

QSizeF LMEntityBase::getOptimalSize()
{
qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__;
    QFontMetrics FontMetrics( font.font );
    QSizeF       size;
    QSizeF       sizeCaption;
    QSizeF       sizeAttributes;
    int          nMargin         = 6;

    // CAPTION
    {
        font.font.setBold( true );
        QFontMetrics FontMetrics( font.font );
#if QT_VERSION < 0x060000
        sizeCaption.setWidth( FontMetrics.boundingRect( getName() ).width() );
#else
        sizeCaption.setWidth( FontMetrics.boundingRect( getName() ).width() );
#endif

        sizeCaption.setHeight( FontMetrics.height() + 2 );
        font.font.setBold( false );
    }

    // Attributes
    {
        int nWidthName     = 1;
        int nWidthDataType = 1;
        int nWidthKey      = 1;

        QList<ADObject*>    listAttributes = getObjects( QString(), QString(), "LMAttribute" );
        ADObject *          pObject;

        foreach( pObject, listAttributes )
        {
            LMAttribute *pAttribute = (LMAttribute*)pObject;

            if ( FontMetrics.boundingRect( pAttribute->getName() ).width() > nWidthName )
                nWidthName = FontMetrics.boundingRect( pAttribute->getName() ).width();

            if ( FontMetrics.boundingRect( pAttribute->getDataType() ).width() > nWidthDataType )
                nWidthDataType = FontMetrics.boundingRect( pAttribute->getDataType() ).width();

            if ( pAttribute->getIdentifier() && pAttribute->getMandatory() )
                nWidthKey = FontMetrics.boundingRect( "pi,M" ).width();
            else if ( pAttribute->getIdentifier() && FontMetrics.boundingRect( "pi" ).width() > nWidthKey )
                nWidthKey = FontMetrics.boundingRect( "pi" ).width();
            else if ( pAttribute->getMandatory() && FontMetrics.boundingRect( "M" ).width() > nWidthKey )
                nWidthKey = FontMetrics.boundingRect( "M" ).width();

        }

        nWidthName     += 4;
        nWidthDataType += 4;
        nWidthKey      += 4;

        sizeAttributes.setWidth( nWidthName + nWidthDataType + nWidthKey );
        sizeAttributes.setHeight( listAttributes.count() * (FontMetrics.height() + 2) );
    }

    // OPTIMAL SIZE
    size.setWidth( qMax( sizeCaption.width(), sizeAttributes.width() ) + nMargin );
    size.setHeight( sizeCaption.height() + sizeAttributes.height() + nMargin );

    return size;
}

void LMEntityBase::getAttributeWidths( QPainter *pPainter, int *pnName, int *pnDataType, int *pnKey )
{
    LMModel *pModel    = (LMModel*)getParent( "LMModel" );

    QFontMetrics  fontmetrics = pPainter->fontMetrics();

    *pnName     = 1;
    *pnDataType = 1;
    *pnKey      = 1;

    // attributes
    {
        QList<ADObject*> listAttributes = getObjects( QString(), QString(), "LMAttribute" );
        ADObject *      pObject;

        foreach( pObject, listAttributes )
        {
            LMAttribute *pAttribute = (LMAttribute*)pObject;

            if ( fontmetrics.boundingRect( pAttribute->getName() ).width() > *pnName )
                *pnName = fontmetrics.boundingRect( pAttribute->getName() ).width();

            if ( fontmetrics.boundingRect( pAttribute->getDataTypeToDisplay( pModel->getDisplayFlags() ) ).width() > *pnDataType )
                *pnDataType = fontmetrics.boundingRect( pAttribute->getDataTypeToDisplay( pModel->getDisplayFlags() ) ).width();

            if ( pAttribute->getIdentifier() && pAttribute->getMandatory() )
                *pnKey = fontmetrics.boundingRect( "pi,M" ).width();
            else if ( pAttribute->getIdentifier() && fontmetrics.boundingRect( "pi" ).width() > *pnKey )
                *pnKey = fontmetrics.boundingRect( "pi" ).width();
            else if ( pAttribute->getMandatory() && fontmetrics.boundingRect( "M" ).width() > *pnKey )
                *pnKey = fontmetrics.boundingRect( "M" ).width();

        }
    }
    
    // Identifiers (save LMEntityNormal the trouble?)
    {
        QList<ADObject*>    listIdentifiers = getObjects( QString(), QString(), "LMIdentifier" );
        ADObject *          pObject;

        foreach( pObject, listIdentifiers )
        {
            LMIdentifier *pidentifier = (LMIdentifier*)pObject;

            if ( fontmetrics.boundingRect( pidentifier->getName() ).width() > *pnName )
                *pnName = fontmetrics.boundingRect( pidentifier->getName() ).width();

            if ( pidentifier->isPrimary() && fontmetrics.boundingRect( "pi" ).width() > *pnKey )
                *pnKey = fontmetrics.boundingRect( "pi" ).width();
            else if ( !pidentifier->isPrimary() && fontmetrics.boundingRect( "ai" ).width() > *pnKey )
                *pnKey = fontmetrics.boundingRect( "ai" ).width();
        }
    }
    
    *pnName     += 4;
    *pnDataType += 4;
    *pnKey      += 4;
}

/*!
    doGoToView
    
    Handle a request to 'go to view'. In this case we invoke our Properties dialog
    unless the request is not for us - in which case we pass it on to our child objects.
*/
void LMEntityBase::doGoToView( ADObjectPathList listObjectPathItems, uint nDepth )
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

