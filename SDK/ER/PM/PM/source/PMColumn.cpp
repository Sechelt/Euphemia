#include "LibInfo.h"
#include "PMColumn.h"

#include <WValueListWidget.h>
#include <ADTransaction.h>

#include "PMModel.h"
#include "PMTableBase.h"
#include "PMDomain.h"
#include "PMKey.h"

#include "PMDataTypeWidget.h"
#include "PMColumnChecksWidget.h" 
#include "PMAdditionalChecksWidget.h"
#include "PMRulesUsedWidget.h"
#include "PMNotesWidget.h"
#include "PMDDLWidget.h"

/*!
 * \brief
 *  
 * This allows a user to generate a draft list of data items before really thinking about 
 * entity organization.                                                                   
 *                                                                                        
 * Then, presumably later, when entities and attributes are created the User can select   
 * data items to add as attributes.                                                       
 *                                                                                        
 * A data item may exist as an attribute in more than one entity.                         
 *  
 * \author pharvey (11/26/19)
 * 
 * \param pmodel 
 */
PMColumn::PMColumn( PMTableBase *pParent )
    : AWObject( pParent ), PMObject( this ), PMDDLManager( this )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    vectorParams.resize( 10 ); // 3x more than needed should be enough

    bForceUpperCase     = false;
    bForceLowerCase     = false;
    bRestrictModify     = false;
    bMandatory          = false;
    bHidden             = false;

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

PMColumn::~PMColumn()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif


#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

void PMColumn::setDataTypeObject( const DATADataType &DataType )
{
    this->DataType = DataType;
    setModified();
}

/*!
    setDataType
    
    Sets the stringDataType. The thing to note here is that we also set
    stringDataTypeWithArgs. Technically we should also set stringDataTypeWithArgs
    whenever any of the data type args change but in the name of optimization
    we only set it here and assume setDataType() is always being called after 
    any setLength() etc.
*/    
void PMColumn::setDataType( const QString &s )    
{
    if ( DataType.stringDataType == s ) return;

    DataType.stringDataType = s;
    setModified();
}

/*!
 * \brief Set domain. 
 *  
 * The domain is not a stored variable - it is simply a reference to a domain. 
 * There should be 0-1 reference. 
 * 
 * \author pharvey (12/31/19)
 * 
 * \param pDomain 
 */
void PMColumn::setDomain( PMDomain *pDomain )   
{
    // remove any existing ref
    PMDomain *p = (PMDomain*)getConnected( QString(), QString(), "PMDomain" );
    if ( p )
    {
        if ( p == pDomain ) return;
        // Remove ref...
        doDisconnect( p );  
        setModified();
    }

    // Add ref...
    if ( pDomain )
    {
        doConnect( pDomain );
        setModified();
    }
} 

void PMColumn::setLength( const QString &s )      
{ 
    if ( DataType.stringLength == s ) return;
    DataType.stringLength = s;
    setModified();
}

void PMColumn::setPrecision1( const QString &s )  
{ 
    if ( DataType.stringPrecision1 == s ) return;
    DataType.stringPrecision1 = s;
    setModified();
}

void PMColumn::setPrecision2( const QString &s )  
{ 
    if ( DataType.stringPrecision2 == s ) return;
    DataType.stringPrecision2 = s;
    setModified();
}

void PMColumn::setScale( const QString &s )       
{ 
    if ( DataType.stringScale == s ) return;
    DataType.stringScale = s;
    setModified();
}

void PMColumn::setMinimum( const QString &s )     
{ 
    if ( stringMinimum == s ) return;
    stringMinimum = s;
    setModified();
}

void PMColumn::setMaximum( const QString &s )     
{ 
    if ( stringMaximum == s ) return;
    stringMaximum = s;
    setModified();
}

void PMColumn::setDefault( const QString &s )     
{ 
    if ( stringDefault == s ) return;
    stringDefault = s;
    setModified();
}

void PMColumn::setFormat( const QString &s )      
{ 
    if ( stringFormat == s ) return;
    stringFormat = s;
    setModified();
}

void PMColumn::setUnit( const QString &s )        
{ 
    if ( stringUnit == s ) return;
    stringUnit = s;
    setModified();
}

void PMColumn::setForceUpperCase( bool b )        
{
    if ( bForceUpperCase == b ) return;
    bForceUpperCase = b;
    setModified();
}

void PMColumn::setForceLowerCase( bool b )        
{ 
    if ( bForceLowerCase == b ) return;
    bForceLowerCase = b;
    setModified();
}

void PMColumn::setRestrictModify( bool b )        
{ 
    if ( bRestrictModify == b ) return;
    bRestrictModify = b;
    setModified();
}

void PMColumn::setMandatory( bool b )        
{ 
    if ( bMandatory == b ) return;
    bMandatory = b;
    setModified();
}

void PMColumn::setHidden( bool b )        
{ 
    if ( bHidden == b ) return;
    bHidden = b;
    setModified();
}

void PMColumn::setValues( const QStringList &s )  
{ 
    if ( stringlistValues == s ) return;
    stringlistValues = s;
    setModified();
}

void PMColumn::setServerCheck( const QString &s ) 
{ 
    if ( stringServerCheck == s ) return;
    stringServerCheck = s;
    setModified();
}

void PMColumn::setClientCheck( const QString &s ) 
{ 
    if ( stringClientCheck == s ) return;
    stringClientCheck = s;
    setModified();
}

/*!
    Return 16x16 icon for such things as browser (list view).
*/
QPixmap PMColumn::getIcon()
{
    return QPixmap( ":PM/Column" );
}

AWDataWidget *PMColumn::getObjectWidget( QWidget *pWidgetParent )
{
    // PMModel *pModel = (PMModel*)getParent( "PMModel" );

    // caller must delete and this is done nicely if used in a AWDataDialog with proper parent
    AWDataWidget *p = AWObject::getObjectWidget( pWidgetParent );

    p->removeByClass( "AWFactoryWidget" );

    p->insertTab( 1, new PMRulesUsedWidget( this, p ), tr("Rules") );
    p->insertTab( 1, new PMNotesWidget( this, p ), tr("Notes") );
    p->insertTab( 1, new PMDDLWidget( this, getCreateAuto(), getDropAuto(), p ), tr("DDL") );
    // Additional Checks
    {
        PMAdditionalChecksWidget *pWidget = new PMAdditionalChecksWidget( p, getClientCheck(), getServerCheck() );
        connect( pWidget, SIGNAL(signalClient(const QString&)), SLOT(slotClientCheck(const QString &)) );
        connect( pWidget, SIGNAL(signalServer(const QString&)), SLOT(slotServerCheck(const QString &)) );
        p->insertTab( 1, pWidget, tr("Additional Checks") );
    }
    // Value list
    {
        WValueListWidget *pWidget = new WValueListWidget( getValues(), p );
        connect( pWidget, SIGNAL(signalChanged(const QStringList &)), SLOT(slotValues(const QStringList &)) );
        p->insertTab( 1, pWidget, tr("Values") );
    }
    p->insertTab( 1, new PMColumnChecksWidget( this, p ), tr("Checks") );
    // Data type
    {
        p->insertTab( 1, new PMDataTypeWidget( this, p ), tr("Data Type") );
    }

    p->setCurrentIndex( 0 );

    return p;
}

QString PMColumn::getDataTypeWithArgs()
{
    QString s = DATADataTypeSpec::getDataTypeWithArgs( getDataTypeSpec(), getDataTypeObject() );
    if ( s.isEmpty() ) return DataType.stringDataType;
    return s;
}

DATADataTypeSpec *PMColumn::getDataTypeSpec()
{
    PMModel *pModel = (PMModel*)getParent( "PMModel" );
    DATADataTypeSpec *p = pModel->getDataTypeSpec( DataType.stringDataType );
    return p;
}

QString PMColumn::getDataTypeToDisplay( int nFlags )
{
    QString stringToDisplay;

    if ( nFlags & PMModel::DisplayDataType )
    {
        PMDomain *pDomain = getDomain();
        // has DOMAIN
        if ( pDomain )
        {
            if ( nFlags & PMModel::DisplayDomain )
            {
                stringToDisplay = pDomain->getName();
            }
            else
            {
                if ( nFlags & PMModel::DisplayParams )
                    stringToDisplay = pDomain->getDataTypeWithArgs();
                else
                    stringToDisplay = pDomain->getName();
            }
        }
        // no Domain
        else
        {
            if ( nFlags & PMModel::DisplayParams )
                stringToDisplay = getDataTypeWithArgs();
            else
                stringToDisplay = DataType.stringDataType;
        }
    }

    return stringToDisplay;
}

PMDomain *PMColumn::getDomain()
{
    return (PMDomain*)getConnected( QString(), QString(), "PMDomain" );
}

bool PMColumn::isPrimaryKey()
{
    QList<ADObjectReferenceKey> l = getUsedByKeys( nullptr, QString(), QString(), "PMKey" );
    ADObjectReferenceKey Key;
    foreach( Key, l )
    {
        if ( ((PMKey*)Key.pObject)->isPrimary() ) return true;
    }

    return false;
}

bool PMColumn::isAlternateKey()
{
    QList<ADObjectReferenceKey> l = getUsedByKeys( nullptr, QString(), QString(), "PMKey" );
    ADObjectReferenceKey Key;
    foreach( Key, l )
    {
        if ( !((PMKey*)Key.pObject)->isPrimary() ) return true;
    }

    return false;
}

bool PMColumn::isForeignKey()
{
    return ( getUsesKeys( nullptr, QString(), QString(), "PMReferenceColumn" ).count() );
}

ADValidationDatabase PMColumn::getValidationConfiguration( ADValidationDatabase &mapClasses )
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
        mapValidations.insert( tr( "Data Item type missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Data Item type unknown" ), ADValidation::Information );
        mapValidations.insert( tr( "Data Item not used" ), ADValidation::Ignore );
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
        mapValidations.insert( tr( "Data Item type missing" ), ADValidation::Warning );
        mapValidations.insert( tr( "Data Item type unknown" ), ADValidation::Warning );
        mapValidations.insert( tr( "Data Item not used" ), ADValidation::Information );
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
        mapValidations.insert( tr( "Data Item type missing" ), ADValidation::Error );
        mapValidations.insert( tr( "Data Item type unknown" ), ADValidation::Error );
        mapValidations.insert( tr( "Data Item not used" ), ADValidation::Warning );
        mapStrictness.insert( ADValidation::High, mapValidations );
    }
    mapClasses.insert( "PMColumn", mapStrictness );

    // no child classes to add here so we are done

    return mapClasses;
}

QString PMColumn::getCreateAuto()
{
    PMModel *pModel = (PMModel*)getParent( "PMModel" );

    QString     stringSQL   = getName();
                                                                 
    // DOMAIN                                                    
    if ( getDomain() )                                           
    {  
        if ( pModel->hasFeature( PMModel::FeatureDomain, PMModel::FeatureSupportCreate ) )
            stringSQL += " " + getDomain()->getName();
        else
        {
            // we have a domain but its not for DDL so apply its datatype instead of the domain name
            stringSQL += " " + getDomain()->getDataTypeWithArgs();   
            if ( !getDomain()->getDefault().isEmpty() ) stringSQL += " DEFAULT " + getDomain()->getDefault();
        }
    }                                                            
    // not DOMAIN                                                
    else                                                         
    {                                                            
        stringSQL += " " + getDataTypeWithArgs();                
        if ( !getDefault().isEmpty() ) stringSQL += " DEFAULT " + getDefault();             
    }                                                            
                                                                 
    // column specific                                           
    if ( isMandatory() ) stringSQL += " NOT NULL";                                

    return stringSQL;
}

QString PMColumn::getDropAuto()
{
    return QString();
}

/*!
 * \brief Save ourself to XML. 
 *  
 * Some attempt is made to avoid saving attributes (ie if they are empty). QString is often used for this - so we can check if something is null.
 *  
 * Unlike PMColumn; we actually save the values here. This is because a PMColumn is simply a PMColumn in a table. No need to save values in two places. 
 *  
 * An PMColumn can use a PMDomain but is not dependent upon using one. So we save values even when we are using a PMDomain (in case we decide not to use 
 * the domain later). The domain will have precedence over local values when one is specified. 
 *  
 * A reference to a PMDomain is saved in generic code which saves all references so no need to do it here. 
 *  
 * \author pharvey (12/30/19)
 * 
 * \param pdomDoc 
 * \param pdomElemParent 
 * 
 * \return QDomElement 
 */
QDomElement PMColumn::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
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
    if ( isForceUpperCase() ) domElemThis.setAttribute( "ForceUpperCase", 1 );
    if ( isForceLowerCase() ) domElemThis.setAttribute( "ForceLowerCase", 1 );
    if ( isRestrictModify() ) domElemThis.setAttribute( "RestrictModify", 1 );
    if ( isHidden() ) domElemThis.setAttribute( "Hidden", 1 );
    if ( isMandatory() ) domElemThis.setAttribute( "Mandatory", 1 );

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
 * \brief Load from XML. 
 *  
 * A reference to a PMDomain is loaded in generic code which loads all references so no need to do it here. 
 * 
 * \author pharvey (12/31/19)
 * 
 * \param pdomElemThis 
 * 
 * \return bool 
 */
bool PMColumn::doLoad( QDomElement *pdomElemThis )
{
    AWObject::doLoad( pdomElemThis );
    doLoadPMObject( pdomElemThis );
    doLoadPMDDLManager( pdomElemThis );

    // load attributes
    QString stringAttribute;

    // vectorParams
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
    bHidden         = pdomElemThis->attribute( "Hidden", "0" ).toInt();
    bMandatory      = pdomElemThis->attribute( "Mandatory", "0" ).toInt();

    // data type params
    // - these are positional so not loaded by param name
    // - load until load fails (or crazy high number)
    for ( int n = 0; n < 10; n++ )
    {
        QString s = pdomElemThis->attribute( QString("P%1").arg( n ) );
        if ( s.isNull() ) break;
        else vectorParams[n] = s;
    }

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
void PMColumn::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes, ADObjectPathList listObjectPathItems )
{
    QString stringValidation;
    ADValidation::Severity nSeverity;

    // Name...                                                                                                                        
    stringValidation = tr("Name missing");
    nSeverity = getValidationSeverity( "PMColumn", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getName().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation );                                         

    // Code...                                                                                                                        
    stringValidation = tr("Code missing");
    nSeverity = getValidationSeverity( "PMColumn", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getCode().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Comment...                                                                                                                     
    stringValidation = tr("Comment missing");
    nSeverity = getValidationSeverity( "PMColumn", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getComment().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Description...                                                                                                                 
    stringValidation = tr("Description missing");                                                                         
    nSeverity = getValidationSeverity( "PMColumn", nStrictness, stringValidation );                                                          
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

    // Domain has precedence over data-type so...
    if ( !getDomain() )
    {
        PMModel *pModel = (PMModel*)getParent( "PMModel" );
        // Type Missing...
        stringValidation = tr("Data Item type missing");
        nSeverity = getValidationSeverity( "PMColumn", nStrictness, stringValidation );                                                          
        if ( nSeverity != ADValidation::Ignore && getDataType().isEmpty() )                                                                                                
            eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                                         

        // Type Unknown...
        stringValidation = tr("Data Item type unknown");
        nSeverity = getValidationSeverity( "PMColumn", nStrictness, stringValidation );                                                          
        if ( nSeverity != ADValidation::Ignore && !getDataType().isEmpty() && !pModel->getIsStandardDataType( getDataType() ) )                                                                                                
            eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                                         
    }

    // Not Used...
    stringValidation= tr("Data Item not used");                                                               
    nSeverity       = getValidationSeverity( "PMColumn", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getUsedByCount() < 1 )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                
}

/*!
    Invoke our standard view.
*/
void PMColumn::doGoToView( ADObjectPathList, uint )
{
    slotObjectDialog();
}

/*!
    Load Value list from given XML.
*/
bool PMColumn::doLoadValueList( QDomElement *pdomElemValueList )
{
    // load child nodes
    QDomElement         domElem;
    QDomNode            domNode;
    QString             stringDataItem;

    domNode = pdomElemValueList->firstChild();
    while( !domNode.isNull() ) 
    {
        domElem = domNode.toElement();
        if( !domElem.isNull() ) 
        {
            // value
            if ( domElem.tagName() == "Value" )
            {
                stringDataItem = domElem.attribute( "Val" );
                if ( !stringDataItem.isNull() )
                    stringlistValues.append( stringDataItem );
            }
        }
        domNode = domNode.nextSibling();
    }

    return true;
}


