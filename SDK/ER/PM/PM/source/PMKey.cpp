#include "LibInfo.h"
#include "PMKey.h"

#include "PMModel.h"
#include "PMTableNormal.h"

// #include <AWObjectNameWidget.h>
// #include <AWObjectUsesWidget.h>
// #include <AWObjectUsedByWidget.h>
// #include <AWObjectChildrenWidget.h>

#include "PMKeyOptionsWidget.h"
#include "PMColumnsUsedWidget.h"
#include "PMNotesWidget.h"
#include "PMRulesUsedWidget.h"
#include "PMDDLWidget.h"

#include "PMColumn.h"

PMKey::PMKey( PMTableNormal *pParent )
    : AWObject( pParent ), PMObject( this )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

    bPrimary = false;

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

PMKey::~PMKey()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

void PMKey::setPrimary( bool b ) 
{
    if ( b == bPrimary ) return;
    bPrimary = b;
    setModified();
}

QPixmap PMKey::getIcon()
{
    if ( bPrimary ) return QPixmap( ":PM/PrimaryKey" );
    return QPixmap( ":PM/AlternateKey" );
}

/*!
 * \brief Creates a tab widget with a number of tabs for editing/viewing object properties.
 * 
 * \author pharvey (5/17/20)
 * 
 * \param pWidgetParent 
 * 
 * \return AWDataWidget* 
 */
AWDataWidget *PMKey::getObjectWidget( QWidget *pWidgetParent )
{
    // caller must delete and this is done nicely if used in a AWDataDialog with proper parent
    AWDataWidget *p = AWObject::getObjectWidget( pWidgetParent );

    p->removeByClass( "AWFactoryWidget" );

    p->insertTab( 1, new PMRulesUsedWidget( this, p ), tr( "Rules" ) );
    p->insertTab( 1, new PMNotesWidget( this, p ), tr("Notes") );
    p->insertTab( 1, new PMColumnsUsedWidget( this, p ), tr( "Columns" ) );
    p->insertTab( 1, new PMKeyOptionsWidget( this, p ), tr("Options") );

    p->setCurrentIndex( 0 );

    return p;
}

QList<PMColumn*> PMKey::getColumns()
{
    QList<PMColumn*> l;
    QList<ADObjectReferenceKey> listColumns = getUsesKeys( nullptr, QString(), QString(), "PMColumn" );
    ADObjectReferenceKey key;
    foreach( key, listColumns )
    {
        l.append( (PMColumn*)(key.pObject) );
    }

    return l;
}

ADValidationDatabase PMKey::getValidationConfiguration( ADValidationDatabase &mapClasses )
{
    return mapClasses;
}

void PMKey::doAddColumn( PMColumn *pColumn )
{
    doConnect( pColumn );
}

void PMKey::doRemoveColumn( PMColumn *pColumn )
{
    doDisconnect( pColumn );
}

QDomElement PMKey::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = AWObject::doSave( pdomDoc, pdomElemParent );
    
    doSavePMObject( pdomDoc, &domElemThis );

    if ( isPrimary() ) domElemThis.setAttribute( "Primary", isPrimary() );

    return domElemThis;
}

bool PMKey::doLoad( QDomElement *pdomElemThis )
{
    AWObject::doLoad( pdomElemThis );
    doLoadPMObject( pdomElemThis );

    bPrimary    = pdomElemThis->attribute( "Primary", "0" ).toInt();

    return true;
}

void PMKey::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    Q_UNUSED(nStrictness);
    Q_UNUSED(nValueType);
    Q_UNUSED(listObjectPathItems);
}


