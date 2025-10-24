#include "LibInfo.h"
#include "LMAttribute.h"

#include <ADTransaction.h>
#include <AWObjectNameWidget.h>
#include <AWObjectUsesWidget.h>
#include <AWObjectUsedByWidget.h>
#include <AWObjectChildrenWidget.h>

#include "LMModel.h"
#include "LMObject.h"
#include "LMEntityBase.h"
#include "LMRule.h"

#include "LMDataTypeWidget.h"
#include "LMAttributeChecksWidget.h" 
#include "LMAdditionalChecksWidget.h"
#include "LMNotesWidget.h"
#include "LMRulesUsedWidget.h"

LMAttribute::LMAttribute( LMEntityBase *pentity )
    : AWObject( pentity ), LMObject( this )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    // LMAttribute should always reference a LMDataItem but we allow NULL here
    // in anticipation of a load to get the LMDataItem from file (or wherever)
    // or it being set externally using setDataItem.
    pDataItem       = nullptr;

    bIdentifier     = false;
    bMandatory      = false;
    bHidden         = false;

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

LMAttribute::~LMAttribute()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif


#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

void LMAttribute::setName( const QString &s )            
{ 
   if ( pDataItem ) pDataItem->setName( s );             
}

void LMAttribute::setComment( const QString &s )         
{ 
    if ( pDataItem ) pDataItem->setComment( s );          
}

void LMAttribute::setCode( const QString &s )            
{ 
    if ( pDataItem ) pDataItem->setCode( s );             
}

void LMAttribute::setDomain( LMDomain *pdomain )
{
    if ( pDataItem ) pDataItem->setDomain( pdomain );        
}

void LMAttribute::setDataTypeObject( const DATADataType &DataType )
{
    if ( pDataItem ) pDataItem->setDataTypeObject( DataType );         
}

void LMAttribute::setDataType( const QString &s )        
{ 
    if ( pDataItem ) pDataItem->setDataType( s );         
}

void LMAttribute::setLength( const QString &s )          
{ 
    if ( pDataItem ) pDataItem->setLength( s );           
}

void LMAttribute::setPrecision1( const QString &s )
{
    if ( pDataItem ) pDataItem->setPrecision1( s );           
}

void LMAttribute::setPrecision2( const QString &s )
{
    if ( pDataItem ) pDataItem->setPrecision2( s );           
}

void LMAttribute::setScale( const QString &s )
{
    if ( pDataItem ) pDataItem->setScale( s );           
}

void LMAttribute::setDescription( const QString &s )     
{ 
    if ( pDataItem ) pDataItem->setDescription( s );      
}

void LMAttribute::setAnnotation( const QString &s )      
{ 
    if ( pDataItem ) pDataItem->setAnnotation( s );       
}

/*!
    Set the  Identifier flag.
    
    We want to ensure we update browser.
*/
void LMAttribute::setIdentifier( bool b ) 
{ 
    if ( bIdentifier == b ) return;
    bIdentifier = b; 
    setModified();
}

/*!
    Set the Mandatory flag.
    
    We want to ensure we update browser.
*/
void LMAttribute::setMandatory( bool b ) 
{ 
    if ( bMandatory == b ) return;
    bMandatory = b; 
    setModified();
}

void LMAttribute::setHidden( bool b )                    
{
    if ( bHidden == b ) return;
    bHidden = b;
    setModified();
}

void LMAttribute::setMinimum( const QString &s )         
{ 
    if ( pDataItem ) pDataItem->setMinimum( s );          
}

void LMAttribute::setMaximum( const QString &s )         
{ 
    if ( pDataItem ) pDataItem->setMaximum( s );          
}

void LMAttribute::setDefault( const QString &s )         
{ 
    if ( pDataItem ) pDataItem->setDefault( s );          
}

void LMAttribute::setFormat( const QString &s )          
{ 
    if ( pDataItem ) pDataItem->setFormat( s );           
}

void LMAttribute::setUnit( const QString &s )            
{ 
    if ( pDataItem ) pDataItem->setUnit( s );             
}

void LMAttribute::setForceUpperCase( bool b )            
{ 
    if ( pDataItem ) pDataItem->setForceUpperCase( b );   
}

void LMAttribute::setForceLowerCase( bool b )            
{ 
    if ( pDataItem ) pDataItem->setForceLowerCase( b );   
}

void LMAttribute::setRestrictModify( bool b )            
{ 
    if ( pDataItem ) pDataItem->setRestrictModify( b );   
}

void LMAttribute::setValues( const QStringList &s )      
{ 
    if ( pDataItem ) pDataItem->setValues( s );           
}

void LMAttribute::setServerCheck( const QString &s )
{
    if ( pDataItem ) pDataItem->setServerCheck( s );           
}

void LMAttribute::setClientCheck( const QString &s )
{
    if ( pDataItem ) pDataItem->setClientCheck( s );           
}

/*!
    Return 16x16 icon for such things as browser (list view).
*/
QPixmap LMAttribute::getIcon()
{
    return QPixmap( ":LM/Attribute16x16" );
}

AWDataWidget *LMAttribute::getObjectWidget( QWidget *pWidgetParent )
{
    LMModel *pModel = (LMModel*)getParent( "LMModel" );

    // caller must delete and this is done nicely if used in a AWDataDialog with proper parent
    AWDataWidget *p = new AWDataWidget( this, pWidgetParent );
    p->setWindowIcon( getIcon() );
    p->setWindowTitle( tr("Attribute...") );
    // Name
    p->addTab( new AWObjectNameWidget( this, p ), tr("Name") );
    // Data type
    {
        LMDataTypeWidget *pWidget = new LMDataTypeWidget( p, pModel, getDomain(), getDataType(), getLength(), getPrecision1(), getPrecision2(), getScale() ); 
        connect( pWidget, SIGNAL(signalDomainChanged(LMDomain *)), SLOT(slotDomain(LMDomain *)) );
        connect( pWidget, SIGNAL(signalDataTypeChanged(const QString&)), SLOT(slotDataType(const QString &)) );
        connect( pWidget, SIGNAL(signalLengthChanged(const QString&)), SLOT(slotLength(const QString &)) );
        connect( pWidget, SIGNAL(signalPrecision1Changed(const QString&)), SLOT(slotPrecision1(const QString &)) );
        connect( pWidget, SIGNAL(signalPrecision2Changed(const QString&)), SLOT(slotPrecision2(const QString &)) );
        connect( pWidget, SIGNAL(signalScaleChanged(const QString&)), SLOT(slotScale(const QString &)) );
        p->addTab( pWidget, tr("Data Type") );
    }
    // Checks
    p->addTab( new LMAttributeChecksWidget( this, p ), tr("Checks") );
    // Value list
    {
        WValueListWidget *pWidget = new WValueListWidget( getValues(), p );
        connect( pWidget, SIGNAL(signalChanged(const QStringList &)), SLOT(slotValues(const QStringList &)) );
        p->addTab( pWidget, tr("Values") );
    }
    // Additional Checks
    {
        LMAdditionalChecksWidget *pWidget = new LMAdditionalChecksWidget( p, getClientCheck(), getServerCheck() );
        connect( pWidget, SIGNAL(signalClient(const QString&)), SLOT(slotClientCheck(const QString &)) );
        connect( pWidget, SIGNAL(signalServer(const QString&)), SLOT(slotServerCheck(const QString &)) );
        p->addTab( pWidget, tr("Additional Checks") );
    }
    // notes
    {
        LMNotesWidget *pWidget = new LMNotesWidget( p, getDescription(), getAnnotation() );
        connect( pWidget, SIGNAL(signalDescriptionChanged(const QString&)), SLOT(slotDescription(const QString &)) );
        connect( pWidget, SIGNAL(signalAnnotationChanged(const QString&)), SLOT(slotAnnotation(const QString &)) );
        p->addTab( pWidget, tr("Notes") );
    }
    p->addTab( new LMRulesUsedWidget( this, p ), tr("Rules") );
    p->addTab( new AWObjectUsesWidget( this, p ), tr("Uses") );
    p->addTab( new AWObjectUsedByWidget( this, p ), tr("Used By") );
    p->addTab( new AWObjectChildrenWidget( this, p ), tr("Children") );

    p->setCurrentIndex( 0 );

    return p;
}

LMEntityBase *LMAttribute::getEntity()
{
    return (LMEntityBase*)getParent( "LMEntityBase" );
}

LMDataItem *LMAttribute::getDataItem()               
{ 
    return pDataItem;                                             
}

QString LMAttribute::getName()
{ 
    if ( !pDataItem ) return QString(); 
    return pDataItem->getName();             
}

QString LMAttribute::getComment()                    
{ 
    if ( !pDataItem ) return QString(); 
    return pDataItem->getComment();          
}
QString LMAttribute::getCode()                       
{ 
    if ( !pDataItem ) return QString(); 
    return pDataItem->getCode();             
}

QString LMAttribute::getDescription()                
{ 
    if ( !pDataItem ) return QString(); 
    return pDataItem->getDescription();      
}

QString LMAttribute::getAnnotation()                 
{ 
    if ( !pDataItem ) return QString(); 
    return pDataItem->getAnnotation();       
}

DATADataType LMAttribute::getDataTypeObject()
{
    if ( !pDataItem ) return DATADataType(); 
    return pDataItem->getDataTypeObject();         
}

QString LMAttribute::getDataType()                   
{ 
    if ( !pDataItem ) return QString(); 
    return pDataItem->getDataType();         
}

QString LMAttribute::getDataTypeToDisplay( int nFlags )
{
    if ( !pDataItem ) return QString();
    return pDataItem->getDataTypeToDisplay( nFlags );
}

QString LMAttribute::getDataTypeWithArgs()
{ 
    if ( !pDataItem ) return QString(); 
    return pDataItem->getDataTypeWithArgs();           
}

QString LMAttribute::getLength()                     
{ 
    if ( !pDataItem ) return QString(); 
    return pDataItem->getLength();           
}

QString LMAttribute::getPrecision1()
{ 
    if ( !pDataItem ) return QString(); 
    return pDataItem->getPrecision1();           
}

QString LMAttribute::getPrecision2()
{ 
    if ( !pDataItem ) return QString(); 
    return pDataItem->getPrecision2();           
}

QString LMAttribute::getScale()
{ 
    if ( !pDataItem ) return QString(); 
    return pDataItem->getScale();           
}

LMDomain *LMAttribute::getDomain()                  
{ 
    if ( !pDataItem ) return nullptr; 
    return pDataItem->getDomain();
}

bool LMAttribute::getIdentifier()                    
{ 
    return bIdentifier;                                           
}

bool LMAttribute::getMandatory()                     
{ 
    return bMandatory;                                            
} 

bool LMAttribute::getHidden()                        
{ 
    return bHidden;                                               
}                       

QString LMAttribute::getMinimum()                    
{ 
    if ( !pDataItem ) return QString(); 
    return pDataItem->getMinimum();          
}

QString LMAttribute::getMaximum()                    
{ 
    if ( !pDataItem ) return QString(); 
    return pDataItem->getMaximum();          
}

QString LMAttribute::getDefault()                    
{ 
    if ( !pDataItem ) return QString(); 
    return pDataItem->getDefault();          
}

QString LMAttribute::getFormat()                     
{ 
    if ( !pDataItem ) return QString(); 
    return pDataItem->getFormat();           
}

QString LMAttribute::getUnit()                       
{ 
    if ( !pDataItem ) 
        return QString(); 
    
    return pDataItem->getUnit();             
}

bool LMAttribute::getForceUpperCase()                
{ 
    if ( !pDataItem ) 
        return false; 
    
    return pDataItem->getForceUpperCase();   
}

bool LMAttribute::getForceLowerCase()                
{ 
    if ( !pDataItem ) 
        return false; 
    
    return pDataItem->getForceLowerCase();   
}

bool LMAttribute::getRestrictModify()                
{ 
    if ( !pDataItem ) 
        return false; 
    
    return pDataItem->getRestrictModify();   
}

QStringList LMAttribute::getValues()                 
{ 
    if ( !pDataItem ) 
        return QStringList(); 
    
    return pDataItem->getValues();           
}

QString LMAttribute::getServerCheck()
{
    if ( !pDataItem ) 
        return QString(); 
    
    return pDataItem->getServerCheck();             
}

QString LMAttribute::getClientCheck()
{
    if ( !pDataItem ) 
        return QString(); 
    
    return pDataItem->getClientCheck();             
}

ADValidationDatabase LMAttribute::getValidationConfiguration( ADValidationDatabase &mapClasses )
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
        mapValidations.insert( tr( "Name duplicate" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Code duplicate" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Attribute is indentifier but not mandatory" ), ADValidation::Warning );
        mapStrictness.insert( ADValidation::Low, mapValidations );
    }

    // Medium:
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Name missing" ), ADValidation::Warning );
        mapValidations.insert( tr( "Code missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Comment missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Description missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Name duplicate" ), ADValidation::Information );
        mapValidations.insert( tr( "Code duplicate" ), ADValidation::Information );
        mapValidations.insert( tr( "Attribute is indentifier but not mandatory" ), ADValidation::Error );
        mapStrictness.insert( ADValidation::Medium, mapValidations );
    }

    // High:
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Name missing" ), ADValidation::Error );
        mapValidations.insert( tr( "Code missing" ), ADValidation::Warning );
        mapValidations.insert( tr( "Comment missing" ), ADValidation::Warning );
        mapValidations.insert( tr( "Description missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Name duplicate" ), ADValidation::Warning );
        mapValidations.insert( tr( "Code duplicate" ), ADValidation::Warning );
        mapValidations.insert( tr( "Attribute is indentifier but not mandatory" ), ADValidation::Error );
        mapStrictness.insert( ADValidation::High, mapValidations );
    }
    mapClasses.insert( "LMAttribute", mapStrictness );

    // no child classes to add here so we are done

    return mapClasses;
}

/*!
    doSave
    
    Save ourself to the XML.
*/
QDomElement LMAttribute::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = AWObject::doSave( pdomDoc, pdomElemParent );

    doSaveLMObject( pdomDoc, &domElemThis );

    domElemThis.setAttribute( "Identifier", getIdentifier() );
    domElemThis.setAttribute( "Mandatory", getMandatory() );
    domElemThis.setAttribute( "Hidden", getHidden() );

    return domElemThis;
}

/*!
    doLoad
    
    Load ourself from the given XML.
    Most of the work is done by LMDataItem.
*/
bool LMAttribute::doLoad( QDomElement *pdomElemThis )
{
    AWObject::doLoad( pdomElemThis );

    doLoadLMObject( pdomElemThis );

    bIdentifier = pdomElemThis->attribute( "Identifier", "0" ).toInt();
    bMandatory  = pdomElemThis->attribute( "Mandatory", "0" ).toInt();
    bHidden     = pdomElemThis->attribute( "Hidden", "0" ).toInt();

    return true;
}

void LMAttribute::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes, ADObjectPathList listObjectPathItems )
{
    QString stringValidation;
    ADValidation::Severity nSeverity;

    // Name...                                                                                                                        
    stringValidation = tr("Name missing");
    nSeverity = getValidationSeverity( "LMAttribute", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getName().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation );                                         

    // Code...                                                                                                                        
    stringValidation = tr("Code missing");
    nSeverity = getValidationSeverity( "LMAttribute", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getCode().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Comment...                                                                                                                     
    stringValidation = tr("Comment missing");
    nSeverity = getValidationSeverity( "LMAttribute", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getComment().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Description...                                                                                                                 
    stringValidation = tr("Description missing");                                                                         
    nSeverity = getValidationSeverity( "LMAttribute", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getDescription().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Flags...                                                                                                                       
    stringValidation= tr("Attribute is indentifier but not mandatory");                                                               
    nSeverity       = getValidationSeverity( "LMAttribute", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && ( bIdentifier && !bMandatory ) )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                
                                                                                                                                      
    // Duplicate...
    ADObject *pObjectParent = (ADObject*)getParent( "ADObject" );
    if ( pObjectParent )
    { 
        stringValidation = tr("Name duplicate");
        nSeverity = getValidationSeverity( "LMAttribute", nStrictness, stringValidation );                                                          
        if ( nSeverity != ADValidation::Ignore && !getName().isEmpty() && pObjectParent->getDuplicate( this, ADObject::FieldName ) )                                                                                                
            eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

        stringValidation = tr("Code duplicate");                                                                         
        nSeverity = getValidationSeverity( "LMAttribute", nStrictness, stringValidation );                                                          
        if ( nSeverity != ADValidation::Ignore && !getCode().isEmpty() && pObjectParent->getDuplicate( this, ADObject::FieldCode ) )                                                                                                
            eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                
    }
}

/*!
 * \brief Catch case where we are being connected to LMDataItem.
 *  
 * Sets pDataItem.
 *  
 * \author pharvey (12/21/19)
 * 
 * \param pmodel 
 * \param s 
 */
void LMAttribute::doUsesAdd( ADObject *pObject, const QString &s )
{
    ADObject::doUsesAdd( pObject, s );

    // We should have exactly one of these
    if ( pObject->inherits( "LMDataItem" ) )
    {
        pDataItem = (LMDataItem*)pObject;
        // DGObject's get this signal/slot connected but we are not a DGObject so lets do it here...
        connect( pDataItem, SIGNAL(signalModified(ADObject*)), this, SLOT(slotModifiedReference(ADObject*)) );
        emit signalModified( pObject );
        emit signalModified();
    }
}

/*!
 * \brief Catch case where we are being disconnected from LMDataItem.
 *  
 * Sets pDataItem to nullptr. 
 *  
 * \author pharvey (12/21/19)
 * 
 * \param pmodel 
 * \param s 
 */
void LMAttribute::doUsesRemove( ADObject *pObject, const QString &s  )
{
    ADObject::doUsesRemove( pObject, s );

    //
    if ( pObject->inherits( "LMDataItem" ) )
    {
        disconnect( pDataItem, SIGNAL(signalModified(ADObject*)), this, SLOT(slotModifiedReference(ADObject*)) );
        pDataItem = nullptr;
        emit signalModified( pObject );
        emit signalModified();
    }
}

/*!
    Invoke our standard view.
*/
void LMAttribute::doGoToView( ADObjectPathList, uint )
{
    slotObjectDialog();
}

/*!
 * \brief We use data in another object (LMDataItem) and that data has changed. Let others know.
 *  
 * Mostly to update text in treewidget. 
 *  
 * \author pharvey (12/21/19)
 * 
 * \param pObject 
 * \param b 
 */
void LMAttribute::slotModifiedReference( ADObject *pObject )
{
    emit signalModified( pObject );
    emit signalModified();
}


