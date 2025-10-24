#include "LibInfo.h"
#include "PMRule.h"

#include <ADTransaction.h>
#include <AWObjectNameWidget.h>
#include <AWObjectUsesWidget.h>
#include <AWObjectUsedByWidget.h>
#include <AWObjectChildrenWidget.h>

#include "PMModel.h" 
#include "PMRuleWidget.h"
#include "PMNotesWidget.h"

PMRule::PMRule( PMModel *pParent )
    : AWObject( pParent ), PMObject( this )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    nRuleType = RuleTypeDefinition;

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

PMRule::~PMRule()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

void PMRule::setRuleType( enumRuleTypes n )
{
    if ( n == nRuleType ) return;
    nRuleType = n;
    setModified();
}

void PMRule::setExpressionClient( const QString &s )
{
    if ( s == stringExpressionClient ) return;
    stringExpressionClient = s;
    setModified();
}

void PMRule::setExpressionServer( const QString &s )
{
    if ( s == stringExpressionServer ) return;
    stringExpressionServer = s;
    setModified();
}

QPixmap PMRule::getIcon()
{
    return QPixmap( ":PM/Rule" );
}

AWDataWidget *PMRule::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = AWObject::getObjectWidget( pWidgetParent );

    p->removeByClass( "AWFactoryWidget" );
    p->insertTab( 1, new PMNotesWidget( this, p ), tr("Notes") );
    p->insertTab( 1, new PMRuleWidget( this, p ), tr("Rule") );

    p->setCurrentIndex( 0 );

    return p;
}

ADValidationDatabase PMRule::getValidationConfiguration( ADValidationDatabase &mapClasses )
{
    return mapClasses;
}

QString PMRule::getCreateAuto()
{
    return QString();
}

QString PMRule::getDropAuto()
{
    return QString();
}

QDomElement PMRule::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = AWObject::doSave( pdomDoc, pdomElemParent );
    doSavePMObject( pdomDoc, &domElemThis );

    return domElemThis;
}

bool PMRule::doLoad( QDomElement *pdomElemThis )
{
    AWObject::doLoad( pdomElemThis );
    doLoadPMObject( pdomElemThis );

    return true;
}

void PMRule::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    Q_UNUSED(nStrictness);
    Q_UNUSED(nValueType);
    Q_UNUSED(listObjectPathItems);
}

void PMRule::doGoToView( ADObjectPathList, uint )
{
    slotObjectDialog();
}


