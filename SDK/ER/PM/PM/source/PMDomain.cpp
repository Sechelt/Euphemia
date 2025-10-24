#include "LibInfo.h"
#include "PMDomain.h"

#include <WValueListWidget.h>
#include <ADTransaction.h>
#include <AWObjectNameWidget.h>
#include <AWObjectUsesWidget.h>
#include <AWObjectUsedByWidget.h>
#include <AWObjectChildrenWidget.h>

#include "PMModel.h" 
#include "PMRule.h" 

#include "PMNotesWidget.h"
#include "PMDataTypeWidget.h"
#include "PMDomainChecksWidget.h"
#include "PMAdditionalChecksWidget.h"
#include "PMRulesUsedWidget.h"
#include "PMDDLWidget.h"
#include "PMNotesWidget.h"

PMDomain::PMDomain( PMModel *pmodel )
    : AWObject( pmodel ), PMObject( this ), PMDDLManager( this )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    bForceUpperCase     = false;
    bForceLowerCase     = false;
    bRestrictModify     = false;

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

PMDomain::~PMDomain()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    // Remove all UsedBy                                        
    {                                                           
        QList<ADObjectReferenceKey> listKeys = mapUsedBy.keys();
        ADObjectReferenceKey Key;                               
        foreach( Key, listKeys )                                
        {                                                       
            Key.pObject->doUsesRemove( this, Key.stringName );  
        }                                                       
    }                                                           
    mapUsedBy.clear();                                          
                                                                
    // Remove all Uses                                          
    {                                                           
        QList<ADObjectReferenceKey> listKeys = mapUses.keys();  
        ADObjectReferenceKey Key;                               
        foreach( Key, listKeys )                                
        {                                                       
            Key.pObject->doUsedByRemove( this, Key.stringName );
        }                                                       
    }                                                           
    mapUses.clear();                                            

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

void PMDomain::setDataTypeObject( const DATADataType &DataType )
{
    this->DataType = DataType;
    setModified();
}

/*!
    setDataType
    
    Sets the atringDataType. The thing to note here is that we also set
    stringDataTypeWithArgs. Technically we should also set stringDataTypeWithArgs
    in whenever any of the data type args change but in the name of optimization
    we only set it here and rely setDataType() being called after any setLength()
    etc.
*/    
void PMDomain::setDataType( const QString &s )    
{ 
    if ( DataType.stringDataType == s ) return;

    DataType.stringDataType = s;
    setModified();
}

void PMDomain::setLength( const QString &s )      
{ 
    if ( DataType.stringLength == s ) return;
    DataType.stringLength = s;
    setModified();
}

void PMDomain::setPrecision1( const QString &s )  
{ 
    if ( DataType.stringPrecision1 == s ) return;
    DataType.stringPrecision1 = s;
    setModified();
}

void PMDomain::setPrecision2( const QString &s )  
{ 
    if ( DataType.stringPrecision2 == s ) return;
    DataType.stringPrecision2 = s;
    setModified();
}

void PMDomain::setScale( const QString &s )       
{ 
    if ( DataType.stringScale == s ) return;
    DataType.stringScale = s;
    setModified();
}

void PMDomain::setMinimum( const QString &s )     
{ 
    if ( stringMinimum == s ) return;
    stringMinimum = s;
    setModified();
}

void PMDomain::setMaximum( const QString &s )     
{ 
    if ( stringMaximum == s ) return;
    stringMaximum = s;
    setModified();
}

void PMDomain::setDefault( const QString &s )     
{ 
    if ( stringDefault == s ) return;
    stringDefault = s;
    setModified();
}

void PMDomain::setFormat( const QString &s )      
{ 
    if ( stringFormat == s ) return;
    stringFormat = s;
    setModified();
}

void PMDomain::setUnit( const QString &s )        
{ 
    if ( stringUnit == s ) return;
    stringUnit = s;
    setModified();
}

void PMDomain::setForceUpperCase( bool b )        
{
    if ( bForceUpperCase == b ) return;
    bForceUpperCase = b;
    setModified();
}

void PMDomain::setForceLowerCase( bool b )        
{ 
    if ( bForceLowerCase == b ) return;
    bForceLowerCase = b;
    setModified();
}

void PMDomain::setRestrictModify( bool b )        
{ 
    if ( bRestrictModify == b ) return;
    bRestrictModify = b;
    setModified();
}

void PMDomain::setValues( const QStringList &s )  
{ 
    if ( stringlistValues == s ) return;
    stringlistValues = s;
    setModified();
}

void PMDomain::setServerCheck( const QString &s ) 
{ 
    if ( stringServerCheck == s ) return;
    stringServerCheck = s;
    setModified();
}

void PMDomain::setClientCheck( const QString &s ) 
{ 
    if ( stringClientCheck == s ) return;
    stringClientCheck = s;
    setModified();
}

/*!
    getIcon
    
    Return 16x16 icon for such things as browser (list view).
*/
QPixmap PMDomain::getIcon()
{
    return QPixmap( ":PM/Domain" );
}

AWDataWidget *PMDomain::getObjectWidget( QWidget *pWidgetParent )
{
    // PMModel *pModel = (PMModel*)getParent( "PMModel" );

    // caller must delete and this is done nicely if used in a AWDataDialog with proper parent
    AWDataWidget *p = AWObject::getObjectWidget( pWidgetParent );

    p->removeByClass( "AWFactoryWidget" );

    p->insertTab( 1, new PMNotesWidget( this, p ), tr("Notes") );
    p->insertTab( 1, new PMDDLWidget( this, getCreateAuto(), getDropAuto(), p ), tr("DDL") );
    p->insertTab( 1, new PMRulesUsedWidget( this, p ), tr("Rules") );
    {
        PMAdditionalChecksWidget *pWidget = new PMAdditionalChecksWidget( p, getClientCheck(), getServerCheck() );
        connect( pWidget, SIGNAL(signalClient(const QString&)), SLOT(slotClientCheck(const QString &)) );
        connect( pWidget, SIGNAL(signalServer(const QString&)), SLOT(slotServerCheck(const QString &)) );
        p->insertTab( 1, pWidget, tr("Additional Checks") );
    }
    {
        WValueListWidget *pWidget = new WValueListWidget( getValues(), p );
        connect( pWidget, SIGNAL(signalChanged(const QStringList &)), SLOT(slotValues(const QStringList &)) );
        p->insertTab( 1, pWidget, tr("Values") );
    }
    p->insertTab( 1, new PMDomainChecksWidget( this, p ), tr("Checks") );
    p->insertTab( 1, new PMDataTypeWidget( this, p ), tr("Data Type") );

    p->setCurrentIndex( 0 );

    return p;
}

DATADataTypeSpec *PMDomain::getDataTypeSpec()
{
    PMModel *pModel = (PMModel*)getParent( "PMModel" );
    return pModel->getDataTypeSpec( DataType.stringDataType );
}

QString PMDomain::getDataTypeWithArgs()
{
    return DATADataTypeSpec::getDataTypeWithArgs( getDataTypeSpec(), getDataTypeObject() );
}

ADValidationDatabase PMDomain::getValidationConfiguration( ADValidationDatabase &mapClasses )
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
        mapValidations.insert( tr( "Type unknown" ), ADValidation::Information );
        mapValidations.insert( tr( "Type missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Not used" ), ADValidation::Ignore );
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
        mapValidations.insert( tr( "Type unknown" ), ADValidation::Warning );
        mapValidations.insert( tr( "Type missing" ), ADValidation::Warning );
        mapValidations.insert( tr( "Not used" ), ADValidation::Information );
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
        mapValidations.insert( tr( "Type unknown" ), ADValidation::Error );
        mapValidations.insert( tr( "Type missing" ), ADValidation::Error );
        mapValidations.insert( tr( "Not used" ), ADValidation::Warning );
        mapStrictness.insert( ADValidation::High, mapValidations );
    }
    mapClasses.insert( "PMDomain", mapStrictness );

    // no child classes to add here so we are done

    return mapClasses;
}

QString PMDomain::getCreateAuto()
{
    PMModel *pModel = (PMModel*)getParent( "PMModel" );
    if ( !pModel->hasFeature( PMModel::FeatureDomain, PMModel::FeatureSupportCreate ) ) return "";

    // DATA TYPE
    QString     stringSQL   = "";
    stringSQL += "CREATE DOMAIN ";
    stringSQL += getName();
    stringSQL += " AS ";
    stringSQL += getDataTypeWithArgs();

    // DEFAULT
    if ( !getDefault().isEmpty() ) 
    {
        stringSQL += " ";
        stringSQL += getDefault();
    }

    stringSQL += ";\n";

    return stringSQL;
}

/*!
    getSQLDrop
*/    
QString PMDomain::getDropAuto()
{
    PMModel *pModel = (PMModel*)getParent( "PMModel" );
    if ( !isIncludeDrop() || !pModel->hasFeature( PMModel::FeatureDomain, PMModel::FeatureSupportDrop ) ) return "";

    return "DROP DOMAIN " + getName() + ";\n";
}


/*!
    doSave
    
    Save ourself to the XML.
*/
QDomElement PMDomain::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = AWObject::doSave( pdomDoc, pdomElemParent );
    doSavePMObject( pdomDoc, &domElemThis );
    doSavePMDDLManager( pdomDoc, &domElemThis );

    if ( !getDataType().isEmpty() ) domElemThis.setAttribute( "DataType", getDataType() );
    if ( !getLength().isEmpty() ) domElemThis.setAttribute( "Length", getLength() );
    if ( !getPrecision1().isEmpty() ) domElemThis.setAttribute( "Precision1", getPrecision1() );
    if ( !getPrecision2().isEmpty() ) domElemThis.setAttribute( "Precision2", getPrecision2() );
    if ( !getScale().isEmpty() ) domElemThis.setAttribute( "Scale", getScale() );
    if ( !getMinimum().isEmpty() ) domElemThis.setAttribute( "Minimum", getMinimum() );
    if ( !getMaximum().isEmpty() ) domElemThis.setAttribute( "Maximum", getMaximum() );
    if ( !getDefault().isEmpty() ) domElemThis.setAttribute( "Default", getDefault() );
    if ( !getFormat().isEmpty() ) domElemThis.setAttribute( "Format", getFormat() );
    if ( !getUnit().isEmpty() ) domElemThis.setAttribute( "Unit", getUnit() );
    if ( getForceUpperCase() ) domElemThis.setAttribute( "ForceUpperCase", 1 );
    if ( getForceLowerCase() ) domElemThis.setAttribute( "ForceLowerCase", 1 );
    if ( getRestrictModify() ) domElemThis.setAttribute( "RestrictModify", 1 );

    // Server Check
    if ( !getServerCheck().isEmpty() )
    {
        unsigned long nLen = 0;
        domElemThis.setAttribute( "ServerCheck", CBDPersistUtility::getEncodedText( getServerCheck(), &nLen ) );
        domElemThis.setAttribute( "ServerCheckSize", QString::number( nLen ) );
    }

    // Client Check
    if ( !getClientCheck().isEmpty() )
    {
        unsigned long nLen = 0;
        domElemThis.setAttribute( "ClientCheck", CBDPersistUtility::getEncodedText( getClientCheck(), &nLen ) );
        domElemThis.setAttribute( "ClientCheckSize", QString::number( nLen ) );
    }

    // save value list
    if ( stringlistValues.count() )
    {
        QDomElement domElemValueList = pdomDoc->createElement( "ValueList" );
        QDomElement domElemValue;
        for ( QStringList::Iterator it = stringlistValues.begin(); it != stringlistValues.end(); ++it ) 
        {
            domElemValue = pdomDoc->createElement( "Value" );
            domElemValue.setAttribute( "Val", *it );
            domElemValueList.appendChild( domElemValue );
        }
    
        //
        domElemThis.appendChild( domElemValueList );
    }

    return domElemThis;
}

/*!
    doLoad
    
    Load ourself from the given XML.
*/
bool PMDomain::doLoad( QDomElement *pdomElemThis )
{
    AWObject::doLoad( pdomElemThis );
    doLoadPMObject( pdomElemThis );
    doLoadPMDDLManager( pdomElemThis );

    // load attributes
    QString stringAttribute;

    // data-type
    DataType.stringDataType  = pdomElemThis->attribute( "DataType" );
    DataType.stringLength    = pdomElemThis->attribute( "Length" );
    DataType.stringPrecision1= pdomElemThis->attribute( "Precision1" );
    DataType.stringPrecision2= pdomElemThis->attribute( "Precision2" );
    DataType.stringScale     = pdomElemThis->attribute( "Scale" );
    stringMinimum   = pdomElemThis->attribute( "Minimum" );
    stringMaximum   = pdomElemThis->attribute( "Maximum" );
    stringDefault   = pdomElemThis->attribute( "Default" );
    stringFormat    = pdomElemThis->attribute( "Format" );
    stringUnit      = pdomElemThis->attribute( "Unit" );
    bForceUpperCase = pdomElemThis->attribute( "ForceUpperCase", "0" ).toInt();
    bForceLowerCase = pdomElemThis->attribute( "ForceLowerCase", "0" ).toInt();
    bRestrictModify = pdomElemThis->attribute( "RestrictModify", "0" ).toInt();

    // ServerCheck
    stringAttribute = pdomElemThis->attribute( "ServerCheckSize" );
    if ( !stringAttribute.isNull() )
    {
        ulong nLen = stringAttribute.toULong();

        stringAttribute = pdomElemThis->attribute( "ServerCheck" );
        if ( !stringAttribute.isNull() )
            stringServerCheck = CBDPersistUtility::getDecodedText( stringAttribute, nLen );
    }

    // ClientCheck
    stringAttribute = pdomElemThis->attribute( "ClientCheckSize" );
    if ( !stringAttribute.isNull() )
    {
        ulong nLen = stringAttribute.toULong();

        stringAttribute = pdomElemThis->attribute( "ClientCheck" );
        if ( !stringAttribute.isNull() )
            stringClientCheck = CBDPersistUtility::getDecodedText( stringAttribute, nLen );
    }

    // load child nodes
    QDomElement         domElem;
    QDomNode            domNode;

    domNode = pdomElemThis->firstChild();
    while( !domNode.isNull() ) 
    {
        if ( domNode.nodeType() != QDomNode::ElementNode )
        {
             domNode = domNode.nextSibling(); 
             continue;
        }
        domElem = domNode.toElement();
        if( domElem.isNull() ) 
        {
            domNode = domNode.nextSibling(); 
            continue;
        }
        // valuelist
        if ( domElem.tagName() == "ValueList" )
        {
            doLoadValueList( &domElem );
        }
        domNode = domNode.nextSibling();
    }

    return true;
}

/*!
    doValidate
    
    Validate ourself.
*/
void PMDomain::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes, ADObjectPathList listObjectPathItems )
{
    QString stringValidation;
    ADValidation::Severity nSeverity;

    // Name...                                                                                                                        
    stringValidation = tr("Name missing");
    nSeverity = getValidationSeverity( "PMDomain", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getName().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation );                                         

    // Code...                                                                                                                        
    stringValidation = tr("Code missing");
    nSeverity = getValidationSeverity( "PMDomain", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getCode().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Comment...                                                                                                                     
    stringValidation = tr("Comment missing");
    nSeverity = getValidationSeverity( "PMDomain", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getComment().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Description...                                                                                                                 
    stringValidation = tr("Description missing");                                                                         
    nSeverity = getValidationSeverity( "PMDomain", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getDescription().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

                                                                                                                                      
    // Duplicate...
    ADObject *pObjectParent = (ADObject*)getParent( "ADObject" );
    if ( pObjectParent )
    { 
        stringValidation = tr("Name duplicate");
        nSeverity = getValidationSeverity( "PMColumn", nStrictness, stringValidation );                                                          
        if ( nSeverity != ADValidation::Ignore && !getName().isEmpty() && pObjectParent->getDuplicate( this, ADObject::FieldName ) )                                                                                                
            eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

        stringValidation = tr("Code duplicate");                                                                         
        nSeverity = getValidationSeverity( "PMColumn", nStrictness, stringValidation );                                                          
        if ( nSeverity != ADValidation::Ignore && !getCode().isEmpty() && pObjectParent->getDuplicate( this, ADObject::FieldCode ) )                                                                                                
            eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                
    }
                                                                                                                                      
    // Type Missing...                                                                                                                
    stringValidation = tr("Type missing");                                                                         
    nSeverity = getValidationSeverity( "PMDomain", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getDataType().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Type Unknown...                                                                                                                
    stringValidation = tr("Type unknown");                                                                         
    nSeverity = getValidationSeverity( "PMDomain", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && !getDataType().isEmpty() && !((PMModel*)getParent("PMModel"))->getIsStandardDataType( getDataType() ) )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                
                                                                                                                                      
    // Not Used...                                                                                                                    
    stringValidation = tr("Not used");                                                                         
    nSeverity = getValidationSeverity( "PMDomain", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getUsedByCount() < 1 )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                
}

/*!
    Invoke our standard view.
*/
void PMDomain::doGoToView( ADObjectPathList, uint )
{
    slotObjectDialog();
}

/*!
    doLoadValueList

    Load Value list from given XML.
*/
bool PMDomain::doLoadValueList( QDomElement *pdomElemValueList )
{
    // load child nodes
    QDomElement         domElem;
    QDomNode            domNode;
    QString             stringDomain;

    domNode = pdomElemValueList->firstChild();
    while( !domNode.isNull() ) 
    {
        domElem = domNode.toElement();
        if( !domElem.isNull() ) 
        {
            // value
            if ( domElem.tagName() == "Value" )
            {
                stringDomain = domElem.attribute( "Val" );
                if ( !stringDomain.isNull() )
                    stringlistValues.append( stringDomain );
            }
        }
        domNode = domNode.nextSibling();
    }

    return true;
}


