#include "PMIndex.h"

#include "PMModel.h"
#include "PMTableBase.h"
#include "PMIndexColumn.h"
#include "PMDDLWidget.h"
#include "PMIndexColumnsWidget.h"
#include "PMRulesUsedWidget.h"
#include "PMNotesWidget.h"

PMIndex::PMIndex( PMTableBase *pParent )
    : AWObject( pParent ), PMObject( this ), PMDDLManager( this )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

    bUnique = false;

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

PMIndex::~PMIndex()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

QPixmap PMIndex::getIcon()
{
    return QPixmap( ":PM/Index" );
}

AWDataWidget *PMIndex::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = AWObject::getObjectWidget( pWidgetParent );

    p->removeByClass( "AWFactoryWidget" );
    p->insertTab( 1, new PMNotesWidget( this, p ), tr("Notes") );
    p->insertTab( 1, new PMRulesUsedWidget( this, p ), tr("Rules") );
    p->insertTab( 1, new PMDDLWidget( this, getCreateAuto(), getDropAuto(), p ), tr("DDL") );
    p->insertTab( 1, new PMIndexColumnsWidget( this, p ), tr("Columns") );

    p->setCurrentIndex( 0 );

    return p;
}

/*!
    getDefaultName
    
    Index name should be unique database object name. Use the table name as the basis for
    the name.
    
    Currently assumes all indexs are being added sequentially i.e. as when creating
    PM from LM.
*/    
QString PMIndex::getDefaultName()
{
    PMTableBase *pTable = (PMTableBase*)getParent( "PMTableBase" );

    QList<ADObject*> l = pTable->getObjects( QString(), QString(), "PMIndex" );

    return "idx" + pTable->getName() + QString::number( l.count() );
}

ADValidationDatabase PMIndex::getValidationConfiguration( ADValidationDatabase &mapClasses )
{
    return mapClasses;
}

QString PMIndex::getCreateAuto()
{
    PMModel *pModel = (PMModel*)getParent( "PMModel" );
    if ( !isIncludeCreate() || !pModel->hasFeature( PMModel::FeatureIndex, PMModel::FeatureSupportCreate ) ) return "";

    QString stringSQL;

    if ( bUnique ) stringSQL = "CREATE UNIQUE INDEX";                                          
    else stringSQL = "CREATE INDEX";                                                           
                                                                                               
    PMTableBase *pTable = (PMTableBase*)getParent( "PMTableBase" );                                        
                                                                                               
    stringSQL += " " + getName() + " ON " + pTable->getName();                                 
                                                                                               
    stringSQL += "\n(\n";                                                                      
                                                                                               
    // columns                                                                                 
    QList<ADObject*> l = getObjects( QString(), QString(), "PMIndexColumn" );                  
    ADObject *  p;                                                                             
    foreach( p, l )                                                                            
    {                                                                                          
        PMIndexColumn *pIndexColumn = (PMIndexColumn*)p;                                       
        if ( pIndexColumn != l.first() ) stringSQL += ",\n";                                   
        stringSQL += "  " + pIndexColumn->getName();                                           
        if ( !pIndexColumn->getOrder().isEmpty() ) stringSQL += " " + pIndexColumn->getOrder();
    }                                                                                          
                                                                                               
    stringSQL += "\n);\n";                                                                     
                                                                                               
    return stringSQL;
}

QString PMIndex::getDropAuto()
{
    PMModel *pModel = (PMModel*)getParent( "PMModel" );
    if ( !isIncludeDrop() || !pModel->hasFeature( PMModel::FeatureIndex, PMModel::FeatureSupportDrop ) ) return "";

    return "DROP INDEX " + getName() + ";\n";
}

QDomElement PMIndex::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = AWObject::doSave( pdomDoc, pdomElemParent );
    doSavePMObject( pdomDoc, &domElemThis );
    doSavePMDDLManager( pdomDoc, &domElemThis );

    if ( getUnique() ) domElemThis.setAttribute( "Unique", getUnique() );

    return domElemThis;
}

bool PMIndex::doLoad( QDomElement *pdomElemThis )
{
    AWObject::doLoad( pdomElemThis );
    doLoadPMObject( pdomElemThis );
    doLoadPMDDLManager( pdomElemThis );

    bUnique    = pdomElemThis->attribute( "Unique", "0" ).toInt();

    return true;
}

void PMIndex::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    Q_UNUSED(nStrictness);
    Q_UNUSED(nValueType);
    Q_UNUSED(listObjectPathItems);
}


