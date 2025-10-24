#include "LibInfo.h"
#include "LMDomain.h"

#include <ADTransaction.h>
#include <WValueListWidget.h>
#include <AWObjectNameWidget.h>
#include <AWObjectUsesWidget.h>
#include <AWObjectUsedByWidget.h>
#include <AWObjectChildrenWidget.h>

#include "LMModel.h" 
#include "LMRule.h"

#include "LMNotesWidget.h"
#include "LMDataTypeWidget.h"
#include "LMDomainChecksWidget.h"
#include "LMAdditionalChecksWidget.h"
#include "LMRulesUsedWidget.h"

LMDomain::LMDomain( LMModel *pmodel )
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

LMDomain::~LMDomain()
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

/*
    // Remove all listUsedBy
    // - Same as in ADObject constructor but we need here so as to use this class name (not ADObject).
    //   See LMDataItem::doUsesRemove() to understand why this is important.
    {                                       
                                            
        ADObject *                   pmodel;
                                            
        foreach( pmodel, listUsedBy )       
        {                                   
            pmodel->doUsesRemove( this );   
        }                                   
    }                                       
    listUsedBy.clear();                     

    // Remove all listUses
    // - Same as in ADObject constructor but we need here so as to use this class name (not ADObject).
    //   See LMDataItem::doUsesRemove() to understand why this is important.
    {                                       
        ADObject *                   pmodel;
                                            
        foreach( pmodel, listUses )         
        {                                   
            pmodel->doUsedByRemove( this ); 
        }                                   
    }                                       
    listUses.clear();                       
*/

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

void LMDomain::setDataTypeObject( const DATADataType &DataType )
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
void LMDomain::setDataType( const QString &s )    
{ 
    if ( DataType.stringDataType == s ) return;

    DataType.stringDataType = s;
    setModified();
}

void LMDomain::setLength( const QString &s )      
{ 
    if ( DataType.stringLength == s ) return;
    DataType.stringLength = s;
    setModified();
}

void LMDomain::setPrecision1( const QString &s )  
{ 
    if ( DataType.stringPrecision1 == s ) return;
    DataType.stringPrecision1 = s;
    setModified();
}

void LMDomain::setPrecision2( const QString &s )  
{ 
    if ( DataType.stringPrecision2 == s ) return;
    DataType.stringPrecision2 = s;
    setModified();
}

void LMDomain::setScale( const QString &s )       
{ 
    if ( DataType.stringScale == s ) return;
    DataType.stringScale = s;
    setModified();
}

void LMDomain::setMinimum( const QString &s )     
{ 
    if ( stringMinimum == s ) return;
    stringMinimum = s;
    setModified();
}

void LMDomain::setMaximum( const QString &s )     
{ 
    if ( stringMaximum == s ) return;
    stringMaximum = s;
    setModified();
}

void LMDomain::setDefault( const QString &s )     
{ 
    if ( stringDefault == s ) return;
    stringDefault = s;
    setModified();
}

void LMDomain::setFormat( const QString &s )      
{ 
    if ( stringFormat == s ) return;
    stringFormat = s;
    setModified();
}

void LMDomain::setUnit( const QString &s )        
{ 
    if ( stringUnit == s ) return;
    stringUnit = s;
    setModified();
}

void LMDomain::setForceUpperCase( bool b )        
{
    if ( bForceUpperCase == b ) return;
    bForceUpperCase = b;
    setModified();
}

void LMDomain::setForceLowerCase( bool b )        
{ 
    if ( bForceLowerCase == b ) return;
    bForceLowerCase = b;
    setModified();
}

void LMDomain::setRestrictModify( bool b )        
{ 
    if ( bRestrictModify == b ) return;
    bRestrictModify = b;
    setModified();
}

void LMDomain::setValues( const QStringList &s )  
{ 
    if ( stringlistValues == s ) return;
    stringlistValues = s;
    setModified();
}

void LMDomain::setServerCheck( const QString &s ) 
{ 
    if ( stringServerCheck == s ) return;
    stringServerCheck = s;
    setModified();
}

void LMDomain::setClientCheck( const QString &s ) 
{ 
    if ( stringClientCheck == s ) return;
    stringClientCheck = s;
    setModified();
}

/*!
    getIcon
    
    Return 16x16 icon for such things as browser (list view).
*/
QPixmap LMDomain::getIcon()
{
    return QPixmap( ":LM/LMDomain16x16" );
}

AWDataWidget *LMDomain::getObjectWidget( QWidget *pWidgetParent )
{
    LMModel *pModel = (LMModel*)getParent( "LMModel" );

    // caller must delete and this is done nicely if used in a AWDataDialog with proper parent
    AWDataWidget *p = new AWDataWidget( this, pWidgetParent );
    // AWDataDialog will adopt icon/title from our widget so...
    p->setWindowIcon( getIcon() );
    p->setWindowTitle( tr("Domain...") );

    p->addTab( new AWObjectNameWidget( this, p ), tr("Name") );

    {
        LMDataTypeWidget *pWidget = new LMDataTypeWidget( p, pModel, getDataType(), getLength(), getPrecision1(), getPrecision2(), getScale() ); 
        connect( pWidget, SIGNAL(signalDataTypeChanged(const QString&)), SLOT(slotDataType(const QString &)) );
        connect( pWidget, SIGNAL(signalLengthChanged(const QString&)), SLOT(slotLength(const QString &)) );
        connect( pWidget, SIGNAL(signalPrecision1Changed(const QString&)), SLOT(slotPrecision1(const QString &)) );
        connect( pWidget, SIGNAL(signalPrecision2Changed(const QString&)), SLOT(slotPrecision2(const QString &)) );
        connect( pWidget, SIGNAL(signalScaleChanged(const QString&)), SLOT(slotScale(const QString &)) );
        p->addTab( pWidget, tr("Data Type") );
    }

    p->addTab( new LMDomainChecksWidget( this, p ), tr("Checks") );

    {
        WValueListWidget *pWidget = new WValueListWidget( getValues(), p );
        connect( pWidget, SIGNAL(signalChanged(const QStringList &)), SLOT(slotValues(const QStringList &)) );
        p->addTab( pWidget, tr("Values") );
    }

    {
        LMAdditionalChecksWidget *pWidget = new LMAdditionalChecksWidget( p, getClientCheck(), getServerCheck() );
        connect( pWidget, SIGNAL(signalClient(const QString&)), SLOT(slotClientCheck(const QString &)) );
        connect( pWidget, SIGNAL(signalServer(const QString&)), SLOT(slotServerCheck(const QString &)) );
        p->addTab( pWidget, tr("Additional Checks") );
    }

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

QString LMDomain::getDataTypeWithArgs()
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

ADValidationDatabase LMDomain::getValidationConfiguration( ADValidationDatabase &mapClasses )
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
    mapClasses.insert( "LMDomain", mapStrictness );

    // no child classes to add here so we are done

    return mapClasses;
}

/*!
    doSave
    
    Save ourself to the XML.
*/
QDomElement LMDomain::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
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
    doLoad
    
    Load ourself from the given XML.
*/
bool LMDomain::doLoad( QDomElement *pdomElemThis )
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
void LMDomain::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes, ADObjectPathList listObjectPathItems )
{
    QString stringValidation;
    ADValidation::Severity nSeverity;

    // Name...                                                                                                                        
    stringValidation = tr("Name missing");
    nSeverity = getValidationSeverity( "LMDomain", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getName().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation );                                         

    // Code...                                                                                                                        
    stringValidation = tr("Code missing");
    nSeverity = getValidationSeverity( "LMDomain", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getCode().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Comment...                                                                                                                     
    stringValidation = tr("Comment missing");
    nSeverity = getValidationSeverity( "LMDomain", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getComment().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Description...                                                                                                                 
    stringValidation = tr("Description missing");                                                                         
    nSeverity = getValidationSeverity( "LMDomain", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getDescription().isEmpty() )                                                                                                
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
                                                                                                                                      
    // Type Missing...                                                                                                                
    stringValidation = tr("Type missing");                                                                         
    nSeverity = getValidationSeverity( "LMDomain", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getDataType().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Type Unknown...                                                                                                                
    stringValidation = tr("Type unknown");                                                                         
    nSeverity = getValidationSeverity( "LMDomain", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && !getDataType().isEmpty() && !((LMModel*)getParent("LMModel"))->getIsStandardDataType( getDataType() ) )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                
                                                                                                                                      
    // Not Used...                                                                                                                    
    stringValidation = tr("Not used");                                                                         
    nSeverity = getValidationSeverity( "LMDomain", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getUsedByCount() < 1 )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                
}

/*!
    Invoke our standard view.
*/
void LMDomain::doGoToView( ADObjectPathList, uint )
{
    slotObjectDialog();
}

/*!
    doLoadValueList

    Load Value list from given XML.
*/
bool LMDomain::doLoadValueList( QDomElement *pdomElemValueList )
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


