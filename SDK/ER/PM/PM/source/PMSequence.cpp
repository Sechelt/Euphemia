#include "LibInfo.h"
#include "PMSequence.h"

#include <ADTransaction.h>
// #include <AWObjectNameWidget.h>
// #include <AWObjectUsesWidget.h>
// #include <AWObjectUsedByWidget.h>
// #include <AWObjectChildrenWidget.h>

#include "PMModel.h" 

#include "PMDDLWidget.h"
#include "PMNotesWidget.h"
#include "PMSequenceWidget.h"

PMSequence::PMSequence( PMModel *pParent )
    : AWObject( pParent ), PMObject( this ), PMDDLManager( this )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

PMSequence::~PMSequence()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

void PMSequence::setInc( const QString &s )
{
    if ( s == stringInc ) return;
    stringInc = s;
    setModified();
}

void PMSequence::setMin( const QString &s )
{
    if ( s == stringMin ) return;
    stringMin = s;
    setModified();
}

void PMSequence::setMax( const QString &s )
{
    if ( s == stringMax ) return;
    stringMax = s;
    setModified();
}

void PMSequence::setStart( const QString &s )
{
    if ( s == stringStart ) return;
    stringStart = s;
    setModified();
}

void PMSequence::setCache( const QString &s )
{
    if ( s == stringCache ) return;
    stringCache = s;
    setModified();
}

void PMSequence::setCycle( const QString &s )
{
    if ( s == stringCycle ) return;
    stringCycle = s;
    setModified();
}

QPixmap PMSequence::getIcon()
{
    return QPixmap( ":PM/Sequence" );
}

AWDataWidget *PMSequence::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = AWObject::getObjectWidget( pWidgetParent );

    p->removeByClass( "AWFactoryWidget" );

    p->insertTab( 1, new PMNotesWidget( this, p ), tr("Notes") );
    p->insertTab( 1, new PMDDLWidget( this, getCreateAuto(), getDropAuto(), p ), tr("DDL") );
    p->insertTab( 1, new PMSequenceWidget( this, p ), tr("Options") );

    p->setCurrentIndex( 0 );

    return p;
}

ADValidationDatabase PMSequence::getValidationConfiguration( ADValidationDatabase &mapClasses )
{
    return mapClasses;
}

QString PMSequence::getCreateAuto()
{
    PMModel *pModel = (PMModel*)getParent( "PMModel" );
    if ( !isIncludeCreate() || !pModel->hasFeature( PMModel::FeatureSequence, PMModel::FeatureSupportCreate ) ) return "";

    QString stringDDL = "CREATE SEQUENCE " + getName();
    if ( !stringInc.isEmpty() ) stringDDL += " INCREMENT " + stringInc;
    if ( !stringMin.isEmpty() ) stringDDL += " MINVALUE " + stringMin;
    if ( !stringMax.isEmpty() ) stringDDL += " MAXVALUE " + stringMax;
    if ( !stringStart.isEmpty() ) stringDDL += " START " + stringStart;
    if ( !stringCache.isEmpty() ) stringDDL += " CACHE " + stringCache;
    if ( !stringCycle.isEmpty() ) stringDDL += " CYCLE";

    return stringDDL + ";\n";
}

QString PMSequence::getDropAuto()
{
    PMModel *pModel = (PMModel*)getParent( "PMModel" );
    if ( !isIncludeDrop() || !pModel->hasFeature( PMModel::FeatureSequence, PMModel::FeatureSupportDrop ) ) return "";

    return "DROP SEQUENCE " + getName() + ";\n";
}

QDomElement PMSequence::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = AWObject::doSave( pdomDoc, pdomElemParent );
    doSavePMObject( pdomDoc, &domElemThis );
    doSavePMDDLManager( pdomDoc, &domElemThis );

    if ( !stringInc.isEmpty() ) domElemThis.setAttribute( "Inc", stringInc );
    if ( !stringMin.isEmpty() ) domElemThis.setAttribute( "Min", stringMin );
    if ( !stringMax.isEmpty() ) domElemThis.setAttribute( "Max", stringMax );
    if ( !stringStart.isEmpty() ) domElemThis.setAttribute( "Start", stringStart );
    if ( !stringCache.isEmpty() ) domElemThis.setAttribute( "Cache", stringCache );
    if ( !stringCycle.isEmpty() ) domElemThis.setAttribute( "Cycle", stringCycle );

    return domElemThis;
}

bool PMSequence::doLoad( QDomElement *pdomElemThis )
{
    AWObject::doLoad( pdomElemThis );
    doLoadPMObject( pdomElemThis );
    doLoadPMDDLManager( pdomElemThis );

    stringInc   = pdomElemThis->attribute( "Inc" );
    stringMin   = pdomElemThis->attribute( "Min" );
    stringMax   = pdomElemThis->attribute( "Max" );
    stringStart = pdomElemThis->attribute( "Start" );
    stringCache = pdomElemThis->attribute( "Cache" );
    stringCycle = pdomElemThis->attribute( "Cycle" );

    return true;
}

void PMSequence::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    Q_UNUSED(nStrictness);
    Q_UNUSED(nValueType);
    Q_UNUSED(listObjectPathItems);
}

void PMSequence::doGoToView( ADObjectPathList, uint )
{
    slotObjectDialog();
}


