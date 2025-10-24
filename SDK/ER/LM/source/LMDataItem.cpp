#include "LibInfo.h"
#include "LMDataItem.h"

#include <ADTransaction.h>
#include <AWObjectNameWidget.h>
#include <AWObjectUsesWidget.h>
#include <AWObjectUsedByWidget.h>
#include <AWObjectChildrenWidget.h>

#include "LMModel.h"
#include "LMModel.h"
#include "LMAttribute.h"
#include "LMDomain.h"
#include "LMRule.h"

#include "LMDataTypeWidget.h"
#include "LMDataItemChecksWidget.h" 
#include "LMAdditionalChecksWidget.h"
#include "LMRulesUsedWidget.h"
#include "LMNotesWidget.h"

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
LMDataItem::LMDataItem( LMModel *pmodel )
    : AWObject( pmodel ), LMObject( this )
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

LMDataItem::~LMDataItem()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif


#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

/*!
    Set the Name.
    
    We want to ensure we update others where needed.
*/
/*
void LMDataItem::setName( const QString &s )
{                                           
   AWObject::setName( s );                  
}                                           
*/

void LMDataItem::setDataTypeObject( const DATADataType &DataType )
{
    this->DataType = DataType;
    setModified();
}

/*!
    setDataType
    
    Sets the atringDataType. The thing to note here is that we also set
    stringDataTypeWithArgs. Technically we should also set stringDataTypeWithArgs
    whenever any of the data type args change but in the name of optimization
    we only set it here and assume setDataType() is always being called after 
    any setLength() etc.
*/    
void LMDataItem::setDataType( const QString &s )    
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
void LMDataItem::setDomain( LMDomain *pDomain )   
{
    // remove any existing reference
    LMDomain *p = (LMDomain*)getConnected( QString(), QString(), "LMDomain" );
    if ( p )
    {
        if ( p == pDomain ) return;
        // Remove Domain...
        doDisconnect( p );  
        setModified();
    }

    // Add Domain...
    if ( pDomain )
    {
        doConnect( pDomain );
        setModified();
    }
} 

void LMDataItem::setLength( const QString &s )      
{ 
    if ( DataType.stringLength == s ) return;
    DataType.stringLength = s;
    setModified();
}

void LMDataItem::setPrecision1( const QString &s )  
{ 
    if ( DataType.stringPrecision1 == s ) return;
    DataType.stringPrecision1 = s;
    setModified();
}

void LMDataItem::setPrecision2( const QString &s )  
{ 
    if ( DataType.stringPrecision2 == s ) return;
    DataType.stringPrecision2 = s;
    setModified();
}

void LMDataItem::setScale( const QString &s )       
{ 
    if ( DataType.stringScale == s ) return;
    DataType.stringScale = s;
    setModified();
}

void LMDataItem::setMinimum( const QString &s )     
{ 
    if ( stringMinimum == s ) return;
    stringMinimum = s;
    setModified();
}

void LMDataItem::setMaximum( const QString &s )     
{ 
    if ( stringMaximum == s ) return;
    stringMaximum = s;
    setModified();
}

void LMDataItem::setDefault( const QString &s )     
{ 
    if ( stringDefault == s ) return;
    stringDefault = s;
    setModified();
}

void LMDataItem::setFormat( const QString &s )      
{ 
    if ( stringFormat == s ) return;
    stringFormat = s;
    setModified();
}

void LMDataItem::setUnit( const QString &s )        
{ 
    if ( stringUnit == s ) return;
    stringUnit = s;
    setModified();
}

void LMDataItem::setForceUpperCase( bool b )        
{
    if ( bForceUpperCase == b ) return;
    bForceUpperCase = b;
    setModified();
}

void LMDataItem::setForceLowerCase( bool b )        
{ 
    if ( bForceLowerCase == b ) return;
    bForceLowerCase = b;
    setModified();
}

void LMDataItem::setRestrictModify( bool b )        
{ 
    if ( bRestrictModify == b ) return;
    bRestrictModify = b;
    setModified();
}

void LMDataItem::setValues( const QStringList &s )  
{ 
    if ( stringlistValues == s ) return;
    stringlistValues = s;
    setModified();
}

void LMDataItem::setServerCheck( const QString &s ) 
{ 
    if ( stringServerCheck == s ) return;
    stringServerCheck = s;
    setModified();
}

void LMDataItem::setClientCheck( const QString &s ) 
{ 
    if ( stringClientCheck == s ) return;
    stringClientCheck = s;
    setModified();
}

/*!
    Return 16x16 icon for such things as browser (list view).
*/
QPixmap LMDataItem::getIcon()
{
    return QPixmap( ":LM/DataItem16x16" );
}

AWDataWidget *LMDataItem::getObjectWidget( QWidget *pWidgetParent )
{
    LMModel *pModel = (LMModel*)getParent( "LMModel" );

    // caller must delete and this is done nicely if used in a AWDataDialog with proper parent
    AWDataWidget *p = new AWDataWidget( this, pWidgetParent );
    p->setWindowIcon( getIcon() );
    p->setWindowTitle( tr("Data Item...") );
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
    p->addTab( new LMDataItemChecksWidget( this, p ), tr("Checks") );
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

QString LMDataItem::getDataTypeWithArgs()
{
    QString stringDataTypeWithArgs;

    // no Data Type
    if ( DataType.stringDataType.isEmpty() ) return QString();

    // unknown Data Type
    LMModel *       pmodel              = (LMModel*)getParent( "LMModel" );
    int             nDataType           = pmodel->getDataType( DataType.stringDataType );
    const char **   ppszDataTypeSyntax  = pmodel->getDataTypeSyntax();

    if ( nDataType < 0 ) return QString();

    // substitute arg values in syntax
    char        cChar;
    bool        bInArgs = false;

    for ( int nChar=0; ; nChar++)
    {
        cChar = ppszDataTypeSyntax[nDataType][nChar];
        if ( !cChar )
            break;

        if ( cChar == '(' )
        {
            stringDataTypeWithArgs += cChar;
            bInArgs = true;
            continue;
        }

        if ( cChar == ')' )
        {
            stringDataTypeWithArgs += cChar;
            bInArgs = false;
            continue;
        }

        if ( bInArgs )
        {
            if ( cChar == 'n' ) // length
            {
                stringDataTypeWithArgs += DataType.stringLength;
            }
            else if ( cChar == 'p' ) // precision1
            {
                stringDataTypeWithArgs += DataType.stringPrecision1;
            }
            else if ( cChar == 'q' ) // precision2
            {
                stringDataTypeWithArgs += DataType.stringPrecision2;
            }
            else if ( cChar == 's' ) // scale
            {
                stringDataTypeWithArgs += DataType.stringScale;
            }
            else if ( cChar == ',' ) // literal
            {
                stringDataTypeWithArgs += cChar;
            }
        }
        else
            stringDataTypeWithArgs += cChar;

    } // for

    return stringDataTypeWithArgs;
}

QString LMDataItem::getDataTypeToDisplay( int nFlags )
{
    QString stringToDisplay;

    if ( nFlags & LMModel::DisplayDataType )
    {
        LMDomain *pDomain = getDomain();
        // has DOMAIN
        if ( pDomain )
        {
            if ( nFlags & LMModel::DisplayDomain )
            {
                stringToDisplay = pDomain->getName();
            }
            else
            {
                if ( nFlags & LMModel::DisplayParams )
                    stringToDisplay = pDomain->getDataTypeWithArgs();
                else
                    stringToDisplay = pDomain->getName();
            }
        }
        // no Domain
        else
        {
            if ( nFlags & LMModel::DisplayParams )
                stringToDisplay = getDataTypeWithArgs();
            else
                stringToDisplay = DataType.stringDataType;
        }
    }

    return stringToDisplay;
}

LMDomain *LMDataItem::getDomain()
{
    return (LMDomain*)getConnected( QString(), QString(), "LMDomain" );
}

ADValidationDatabase LMDataItem::getValidationConfiguration( ADValidationDatabase &mapClasses )
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
    mapClasses.insert( "LMDataItem", mapStrictness );

    // no child classes to add here so we are done

    return mapClasses;
}

/*!
 * \brief Save ourself to XML. 
 *  
 * Some attempt is made to avoid saving attributes (ie if they are empty). QString is often used for this - so we can check if something is null.
 *  
 * Unlike LMAttribute; we actually save the values here. This is because a LMAttribute is simply a LMDataItem in a table. No need to save values in two places. 
 *  
 * An LMDataItem can use a LMDomain but is not dependent upon using one. So we save values even when we are using a LMDomain (in case we decide not to use 
 * the domain later). The domain will have precedence over local values when one is specified. 
 *  
 * A reference to a LMDomain is saved in generic code which saves all references so no need to do it here. 
 *  
 * \author pharvey (12/30/19)
 * 
 * \param pdomDoc 
 * \param pdomElemParent 
 * 
 * \return QDomElement 
 */
QDomElement LMDataItem::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = AWObject::doSave( pdomDoc, pdomElemParent );
    
    doSaveLMObject( pdomDoc, &domElemThis );

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
 * \brief Load from XML. 
 *  
 * A reference to a LMDomain is loaded in generic code which loads all references so no need to do it here. 
 * 
 * \author pharvey (12/31/19)
 * 
 * \param pdomElemThis 
 * 
 * \return bool 
 */
bool LMDataItem::doLoad( QDomElement *pdomElemThis )
{
    AWObject::doLoad( pdomElemThis );
    doLoadLMObject( pdomElemThis );

    // load attributes
    QString stringAttribute;

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
void LMDataItem::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes, ADObjectPathList listObjectPathItems )
{
    QString stringValidation;
    ADValidation::Severity nSeverity;

    // Name...                                                                                                                        
    stringValidation = tr("Name missing");
    nSeverity = getValidationSeverity( "LMDataItem", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getName().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation );                                         

    // Code...                                                                                                                        
    stringValidation = tr("Code missing");
    nSeverity = getValidationSeverity( "LMDataItem", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getCode().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Comment...                                                                                                                     
    stringValidation = tr("Comment missing");
    nSeverity = getValidationSeverity( "LMDataItem", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getComment().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Description...                                                                                                                 
    stringValidation = tr("Description missing");                                                                         
    nSeverity = getValidationSeverity( "LMDataItem", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getDescription().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Duplicate...
    ADObject *pObjectParent = (ADObject*)getParent( "ADObject" );
    if ( pObjectParent )
    { 
        stringValidation = tr("Name duplicate");
        nSeverity = getValidationSeverity( "LMDataItem", nStrictness, stringValidation );                                                          
        if ( nSeverity != ADValidation::Ignore && !getName().isEmpty() && pObjectParent->getDuplicate( this, ADObject::FieldName ) )                                                                                                
            eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

        stringValidation = tr("Code duplicate");                                                                         
        nSeverity = getValidationSeverity( "LMDataItem", nStrictness, stringValidation );                                                          
        if ( nSeverity != ADValidation::Ignore && !getCode().isEmpty() && pObjectParent->getDuplicate( this, ADObject::FieldCode ) )                                                                                                
            eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                
    }

    // Domain has precedence over data-type so...
    if ( !getDomain() )
    {
        LMModel *pModel = (LMModel*)getParent( "LMModel" );
        // Type Missing...
        stringValidation = tr("Data Item type missing");
        nSeverity = getValidationSeverity( "LMDataItem", nStrictness, stringValidation );                                                          
        if ( nSeverity != ADValidation::Ignore && getDataType().isEmpty() )                                                                                                
            eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                                         

        // Type Unknown...
        stringValidation = tr("Data Item type unknown");
        nSeverity = getValidationSeverity( "LMDataItem", nStrictness, stringValidation );                                                          
        if ( nSeverity != ADValidation::Ignore && !getDataType().isEmpty() && !pModel->getIsStandardDataType( getDataType() ) )                                                                                                
            eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                                         
    }

    // Not Used...
    stringValidation= tr("Data Item not used");                                                               
    nSeverity       = getValidationSeverity( "LMDataItem", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getUsedByCount() < 1 )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                
}

/*!
    doUsesAdd
    
    We replace this so as to intercept LMDomain.
 
    \note not using this optimization for domain - will see how it goes without it
*/
void LMDataItem::doUsesAdd( ADObject *pmodel )
{
    Q_UNUSED(pmodel);                                             
/*
    ADObject::doUsesAdd( pmodel );            
                                              
    // We should have exactly one of these    
    if ( pmodel->inherits( "LMDomain" ) )     
    {                                         
        pdomain = (LMDomain*)pmodel;          
    }                                         
*/
}                                             

/*!
    doUsesRemove
    
    We replace this so as to intercept LMDomain.
*/
void LMDataItem::doUsesRemove( ADObject *pmodel )
{
    Q_UNUSED(pmodel);                                             
/*
    ADObject::doUsesRemove( pmodel );    
                                         
    //                                   
    if ( pmodel->inherits( "LMDomain" ) )
    {                                    
        pdomain = 0;                     
    }                                    
*/
}

/*!
    Invoke our standard view.
*/
void LMDataItem::doGoToView( ADObjectPathList, uint )
{
    slotObjectDialog();
}

/*!
    Load Value list from given XML.
*/
bool LMDataItem::doLoadValueList( QDomElement *pdomElemValueList )
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


