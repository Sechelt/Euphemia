#include "LibInfo.h"
#include "PMProcedure.h"

#include <ADTransaction.h>
// #include <AWObjectNameWidget.h>
// #include <AWObjectUsesWidget.h>
// #include <AWObjectUsedByWidget.h>
// #include <AWObjectChildrenWidget.h>

#include "PMModel.h" 

#include "PMDDLWidget.h"
#include "PMNotesWidget.h"
#include "PMRulesUsedWidget.h"
#include "PMProcedureWidget.h"

PMProcedure::PMProcedure( PMModel *pmodel )
    : AWObject( pmodel ), PMObject( this ), PMDDLManager( this )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

PMProcedure::~PMProcedure()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

void PMProcedure::setDef( const QString &s )
{
    if ( s == stringDef ) return;
    stringDef = s;
    setModified();
}

QPixmap PMProcedure::getIcon()
{
    return QPixmap( ":PM/Procedure" );
}

AWDataWidget *PMProcedure::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = AWObject::getObjectWidget( pWidgetParent );

    p->removeByClass( "AWFactoryWidget" );

    p->insertTab( 1, new PMRulesUsedWidget( this, p ), tr("Rules") );
    p->insertTab( 1, new PMNotesWidget( this, p ), tr("Notes") );
    p->insertTab( 1, new PMDDLWidget( this, getCreateAuto(), getDropAuto(), p ), tr("DDL") );
    p->insertTab( 1, new PMProcedureWidget( this, p ), tr("Definition") );

    p->setCurrentIndex( 0 );

    return p;
}

ADValidationDatabase PMProcedure::getValidationConfiguration( ADValidationDatabase &mapClasses )
{
    return mapClasses;
}

QString PMProcedure::getCreateAuto()
{
    PMModel *pModel = (PMModel*)getParent( "PMModel" );
    if ( !isIncludeCreate() || !pModel->hasFeature( PMModel::FeatureProcedure, PMModel::FeatureSupportCreate ) ) return "";

    QString stringSQL = getDef() + "\n";

    return stringSQL;
}

QString PMProcedure::getDropAuto()
{
    PMModel *pModel = (PMModel*)getParent( "PMModel" );
    if ( !isIncludeDrop() || !pModel->hasFeature( PMModel::FeatureProcedure, PMModel::FeatureSupportDrop ) ) return "";

    return "DROP PROCEDURE " + getName() + ";\n";
}

QDomElement PMProcedure::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = AWObject::doSave( pdomDoc, pdomElemParent );
    doSavePMObject( pdomDoc, &domElemThis );
    doSavePMDDLManager( pdomDoc, &domElemThis );

    if ( !stringDef.isEmpty() )
    {
        unsigned long nLen = 0;
        domElemThis.setAttribute( "Def", CBDPersistUtility::getEncodedText( stringDef, &nLen ) );
        domElemThis.setAttribute( "DefSize", QString::number( nLen ) );
    }

    return domElemThis;
}

bool PMProcedure::doLoad( QDomElement *pdomElemThis )
{
    AWObject::doLoad( pdomElemThis );
    doLoadPMObject( pdomElemThis );
    doLoadPMDDLManager( pdomElemThis );

    QString stringAttribute = pdomElemThis->attribute( "DefSize" );
    if ( !stringAttribute.isNull() )
    {
        ulong nLen = stringAttribute.toULong();

        stringAttribute = pdomElemThis->attribute( "Def" );
        if ( !stringAttribute.isNull() ) stringDef = CBDPersistUtility::getDecodedText( stringAttribute, nLen );
    }

    return true;
}

void PMProcedure::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    Q_UNUSED(nStrictness);
    Q_UNUSED(nValueType);
    Q_UNUSED(listObjectPathItems);
}

void PMProcedure::doGoToView( ADObjectPathList, uint )
{
    slotObjectDialog();
}


