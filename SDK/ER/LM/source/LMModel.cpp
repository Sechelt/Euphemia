#include "LibInfo.h"
#include "LMModel.h"

#include <AWDocumentNameWidget.h>
#include <AWFactoryWidget.h>
#include <AWObjectUsesWidget.h>
#include <AWObjectUsedByWidget.h>
#include <AWObjectChildrenWidget.h>
#include <AWValidationWidget.h>

#include <DGraphicsSceneProxy.h>
#include <DCallOut.h>
#include <DTitle.h>
#include <DTitleDoc.h>

#include "LMModelEditorWidget.h"
#include "LMDataItem.h"
#include "LMDomain.h"
#include "LMRule.h"
#include "LMAssociativeLink.h"
#include "LMInheritance.h"
#include "LMInheritanceLink.h"
#include "LMValidationWidget.h"
#include "LMDomainsWidget.h"
#include "LMDataItemsWidget.h"
#include "LMNotesWidget.h"
#include "LMRulesWidget.h"
#include "LMDiagramDisplayWidget.h"

/*!
    LMDataTypes

    These are all of the supported, intrinsic, data types. In keeping with the concept
    of an LM - these data types should not be database specific. 
    
    This is the SAME as the ODBC Extended SQL Grammar in the hope that it will result in a low loss
    of information when LM is morphed to and from PM's while possibly leveraging the
    fact that an ODBC driver knows of these data types.
*/
static const char* LMDataTypes[] =
{                                 
    "CHAR",                       
    "VARCHAR",                    
    "LONG VARCHAR",
    "WCHAR",                      
    "VARWCHAR",                   
    "LONGWVARCHAR",               
    "DECIMAL",                    
    "NUMERIC",                    
    "SMALLINT",                   
    "INTEGER",                    
    "REAL",                       
    "FLOAT",                      
    "DOUBLE PRECISION",           
    "BIT",                        
    "TINYINT",                    
    "BIGINT",                     
    "BINARY",                     
    "VARBINARY",                  
    "LONG VARBINARY",             
    "DATE",                       
    "TIME",                       
    "TIMESTAMP",                  
    "UTCDATETIME",                
    "INTERVAL MONTH",             
    "INTERVAL YEAR",              
    "INTERVAL YEAR TO MONTH",     
    "INTERVAL DAY",               
    "INTERVAL HOUR",              
    "INTERVAL MINUTE",            
    "INTERVAL SECOND",            
    "INTERVAL DAY TO HOUR",       
    "INTERVAL DAY TO MINUTE",     
    "INTERVAL DAY TO SECOND",     
    "INTERVAL HOUR TO MINUTE",    
    "INTERVAL HOUR TO SECOND",    
    "INTERVAL MINUTE TO SECOND",  
    "GUID",                       
    0                             
};                                

/*!
    LMDataTypeSyntax
    
    All intrinsic data types formatted to indicate syntax. Used to present for edit.
    
    These MUST be in synch with LMDataTypes.
*/
static const char* LMDataTypeSyntax[] =
{                                      
    "CHAR(n)",                         
    "VARCHAR(n)",                      
    "LONG VARCHAR", 
    "WCHAR(n)",                        
    "VARWCHAR(n)",                     
    "LONGWVARCHAR",                    
    "DECIMAL(p,s)",                    
    "NUMERIC(p,s)",                    
    "SMALLINT",                        
    "INTEGER",                         
    "REAL",                            
    "FLOAT(p)",                        
    "DOUBLE PRECISION",                
    "BIT",                             
    "TINYINT",                         
    "BIGINT",                          
    "BINARY(n)",                       
    "VARBINARY(n)",                    
    "LONG VARBINARY",                  
    "DATE",                            
    "TIME(p)",                         
    "TIMESTAMP(p)",                    
    "UTCDATETIME",                     
    "UTCTIME",                         
    "INTERVAL MONTH(p)",               
    "INTERVAL YEAR(p)",                
    "INTERVAL YEAR(p) TO MONTH",       
    "INTERVAL DAY(p)",                 
    "INTERVAL HOUR(p)",                
    "INTERVAL MINUTE(p)",              
    "INTERVAL SECOND(p,q)",            
    "INTERVAL DAY(p) TO HOUR",         
    "INTERVAL DAY(p) TO MINUTE",       
    "INTERVAL DAY(p) TO SECOND(q)",    
    "INTERVAL HOUR(p) TO MINUTE",      
    "INTERVAL HOUR(p) TO SECOND(q)",   
    "INTERVAL MINUTE(p) TO SECOND(q)", 
    "GUID",                            
    0                                  
};                                     

/*!
    LMDataTypeDescs
    
    Brief help message for each intrinsic data type. Used to present for edit.
    
    These MUST be in synch with LMDataTypes.
*/
static const char* LMDataTypeDescs[] =                                                                                                                                                                                                   
{                                                                                                                                                                                                                                        
    "<B>CHAR(n)</B><P>Character string of fixed string length <B>n</B>.</P>",                                                                                                                                                            
    "<B>VARCHAR(n)</B><P>Variable-length character string with a maximum string length <B>n</B>.</P>",                                                                                                                                   
    "<B>LONG VARCHAR</B><P>Variable length character data. Maximum length is data source-dependent.</P>",                                                                                                                                
    "<B>WCHAR(n)</B><P>Unicode character string of fixed string length <B>n</B>.</P>",                                                                                                                                                   
    "<B>VARWCHAR(n)</B><P>Unicode variable-length character string with a maximum string length <B>n</B>.</P>",                                                                                                                          
    "<B>LONGWVARCHAR</B><P>Unicode variable-length character data. Maximum length is data source-dependent.</P>",                                                                                                                        
                                                                                                                                                                                                                                         
    "<B>DECIMAL(p,s)</B><P>Signed, exact, numeric value with a precision of at least <B>p</B> and scale s. (The maximum precision is driver-defined.) (1 &lt;= <B>p</B> &lt;= 15; <B>s</B> &lt;= <B>p</B>).</P>",                        
    "<B>NUMERIC(p,s)</B><P>Signed, exact, numeric value with a precision <B>p</B> and scale <B>s</B> (1 &lt;= <B>p</B> &lt;= 15; <B>s</B> &lt;= <B>p</B>).</P>",                                                                         
                                                                                                                                                                                                                                         
    "<B>SMALLINT</B><P>Exact numeric value with precision 5 and scale 0 (signed: -32,768 &lt;= <B>n</B> &lt;= 32,767, unsigned: 0 &lt;= <B>n</B> &lt;= 65,535).</P>",                                                                    
    "<B>INTEGER</B><P>Exact numeric value with precision 10 and scale 0 (signed: -2[31] &lt;= <B>n</B> &lt;= 2[31] - 1, unsigned: 0 &lt;= <B>n</B> &lt;= 2[32] - 1).</P>",                                                               
    "<B>REAL</B><P>Signed, approximate, numeric value with a binary precision 24 (zero or absolute value 10[-38] to 10[38]).</P>",                                                                                                       
                                                                                                                                                                                                                                         
    "<B>FLOAT(p)</B><P>Signed, approximate, numeric value with a binary precision of at least <B>p</B>. (The maximum precision is driver-defined.)",                                                                                     
                                                                                                                                                                                                                                         
    "<B>DOUBLE PRECISION</B><P>Signed, approximate, numeric value with a binary precision 53 (zero or absolute value 10[-308] to 10[308]).</P>",                                                                                         
    "<B>BIT</B><P>Single bit binary data.</P>",                                                                                                                                                                                          
    "<B>TINYINT</B><P>Exact numeric value with precision 3 and scale 0 (signed: -128 &lt;= <B>n</B> &lt;= 127, unsigned: 0 &lt;= <B>n</B> &lt;= 255)",                                                                                   
    "<B>BIGINT</B><P>Exact numeric value with precision 19 (if signed) or 20 (if unsigned) and scale 0 (signed: -2[63] &lt;= <B>n</B> &lt;= 2[63] - 1, unsigned: 0 &lt;= <B>n</B> &lt;= 2[64] - 1.</P>",                                 
                                                                                                                                                                                                                                         
    "<B>BINARY(n)</B><P>Binary data of fixed length <B>n</B>.</P>",                                                                                                                                                                      
    "<B>VARBINARY(n)</B><P>Variable length binary data of maximum length <B>n</B>. The maximum is set by the user.</P>",                                                                                                                 
                                                                                                                                                                                                                                         
    "<B>LONG VARBINARY</B><P>Variable length binary data. Maximum length is data source-dependent.</P>",                                                                                                                                 
    "<B>DATE</B><P>Year, month, and day fields, conforming to the rules of the Gregorian calendar.</P>",                                                                                                                                 
                                                                                                                                                                                                                                         
    "<B>TIME(p)</B><P>Hour, minute, and second fields, with valid values for hours of 00 to 23, valid values for minutes of 00 to 59, and valid values for seconds of 00 to 61. Precision <B>p</B> indicates the seconds precision.</P>",
    "<B>TIMESTAMP(p)</B><P>Year, month, day, hour, minute, and second fields, with valid values as defined for the DATE and TIME data types.</P>",                                                                                       
                                                                                                                                                                                                                                         
    "<B>UTCDATETIME</B><P>Year, month, day, hour, minute, second, utchour, and utcminute fields. The utchour and utcminute fields have 1/10th microsecond precision.</P>",                                                               
    "<B>UTCTIME</B><P>Hour, minute, second, utchour, and utcminute fields. The utchour and utcminute fields have 1/10th microsecond precision.</P>",                                                                                     
                                                                                                                                                                                                                                         
    "<B>INTERVAL MONTH(p)</B><P>Number of months between two dates; <B>p</B> is the interval leading precision.</P>",                                                                                                                    
    "<B>INTERVAL YEAR(p)</B><P>Number of years between two dates; <B>p</B> is the interval leading precision.</P>",                                                                                                                      
    "<B>INTERVAL YEAR(p) TO MONTH</B><P>Number of years and months between two dates; <B>p</B> is the interval leading precision.</P>",                                                                                                  
    "<B>INTERVAL DAY(p)</B><P>Number of days between two dates; <B>p</B> is the interval leading precision.</P>",                                                                                                                        
    "<B>INTERVAL HOUR(p)</B><P>Number of hours between two date/times; <B>p</B> is the interval leading precision.</P>",                                                                                                                 
    "<B>INTERVAL MINUTE(p)</B><P>Number of minutes between two date/times; <B>p</B> is the interval leading precision.</P>",                                                                                                             
                                                                                                                                                                                                                                         
    "<B>INTERVAL SECOND(p,q)</B><P>Number of seconds between two date/times; <B>p</B> is the interval leading precision and <B>q</B> is the interval seconds precision.</P>",                                                            
                                                                                                                                                                                                                                         
    "<B>INTERVAL DAY(p) TO HOUR</B><P>Number of days/hours between two date/times; <B>p</B> is the interval leading precision.</P>",                                                                                                     
    "<B>INTERVAL DAY(p) TO MINUTE</B><P>Number of days/hours/minutes between two date/times; <B>p</B> is the interval leading precision.</P>",                                                                                           
                                                                                                                                                                                                                                         
    "<B>INTERVAL DAY(p) TO SECOND(q)</B><P>Number of days/hours/minutes/seconds between two date/times; <B>p</B> is the interval leading precision and <B>q</B> is the interval seconds precision.</P>",                                 
                                                                                                                                                                                                                                         
    "<B>INTERVAL HOUR(p) TO MINUTE</B><P>Number of hours/minutes between two date/times; <B>p</B> is the interval leading precision.</P>",                                                                                               
                                                                                                                                                                                                                                         
    "<B>INTERVAL HOUR(p) TO SECOND(q)</B><P>Number of hours/minutes/seconds between two date/times; <B>p</B> is the interval leading precision and <B>q</B> is the interval seconds precision.</P>",                                     
    "<B>INTERVAL MINUTE(p) TO SECOND(q)</B><P>Number of minutes/seconds between two date/times; <B>p</B> is the interval leading precision and <B>q</B> is the interval seconds precision.</P>",                                         
                                                                                                                                                                                                                                         
    "<B>GUID</B><P>Fixed length Globally Unique Identifier.</P>",                                                                                                                                                                        
    0                                                                                                                                                                                                                                    
};                                                                                                                                                                                                                                       


LMModel::LMModel( ADObject *pParent, const QString &stringName )
    : DDiagram( pParent, stringName ), LMObject( this )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    mapMeta.clear();
    mapMeta.insert( "LMAssociativeLink", ADMeta( QIcon( ":LM/AssociationLink16x16" ), "LMAssociativeLink", "Association Link" ) );
    mapMeta.insert( "LMDataItem", ADMeta( QIcon( ":LM/DataItem16x16" ), "LMDataItem", "Data Item" ) );
    mapMeta.insert( "LMDomain", ADMeta( QIcon( ":LM/LMDomain16x16" ), "LMDomain", "Domain" ) );
    mapMeta.insert( "LMEntityAssociative", ADMeta( QIcon( ":LM/Association16x16" ), "LMEntityAssociative", "Association" ) );
    mapMeta.insert( "LMEntityNormal", ADMeta( QIcon( ":LM/Entity16x16" ), "LMEntityNormal", "Entity" ) );
    mapMeta.insert( "LMInheritance", ADMeta( QIcon( ":LM/Inheritance16x16" ), "LMInheritance", "Inheritance" ) );
    mapMeta.insert( "LMInheritanceLink", ADMeta( QIcon( ":LM/InheritanceLink16x16" ), "LMInheritanceLink", "Inheritance Link" ) );
    mapMeta.insert( "LMRelationship", ADMeta( QIcon( ":LM/Relationship16x16" ), "LMRelationship", "Relationship" ) );
    mapMeta.insert( "LMRule", ADMeta( QIcon( ":LM/Rule16x16" ), "LMRule", "Rule" ) );

    // mark-up stuff
    mapMeta.insert( "DTitle", ADMeta( QIcon( ":DR/Title" ), "DTitle", "Title" ) );
    mapMeta.insert( "DTitleDoc", ADMeta( QIcon( ":DR/Title" ), "DTitleDoc", "Title Doc" ) );
    mapMeta.insert( "DCallOut", ADMeta( QIcon( ":DR/CallOut" ), "DCallOut", "Call Out" ) );

    nDisplayFlags = 0;
    nDisplayFlags |= DisplayDataType;
    nDisplayFlags |= (int)DisplayDomain;
    nDisplayFlags |= (int)DisplayParams;
    nDisplayFlags |= (int)DisplayDefault;

    slotRuler( false );
    setModified( false );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

LMModel::~LMModel()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

/*!
    setDisplayFlag
    
    Applies given flag to all entities
 */
void LMModel::setDisplayFlag( int nFlag )
{ 
    nDisplayFlags |= (int)nFlag; 
}

QPixmap LMModel::getIcon()
{
    return QPixmap( ":LM/LogicalModel48x48" );
}

ADObject *LMModel::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "LMAssociativeLink" )
    {
        p = new LMAssociativeLink( this );
        p->setFolder( tr( "Associative Links" ) );
    }
    else if ( stringClass == "LMDataItem" )
    {
        p = new LMDataItem( this );
        p->setFolder( tr( "Data Items" ) );
    }
    else if ( stringClass == "LMDomain" )
    {
        p = new LMDomain( this );
        p->setFolder( tr( "Domains" ) );
    }
    else if ( stringClass == "LMEntityAssociative" )
    {
        p = new LMEntityAssociative( this );
        p->setFolder( tr( "Associative Entities" ) );
    }
    else if ( stringClass == "LMEntityNormal" )
    {
        p = new LMEntityNormal( this );
        p->setFolder( tr( "Entities" ) );
    }
    else if ( stringClass == "LMInheritance" )
    {
        p = new LMInheritance( this );
        p->setFolder( tr( "Inheritances" ) );
    }
    else if ( stringClass == "LMInheritanceLink" )
    {
        p = new LMInheritanceLink( this );
        p->setFolder( tr( "Inheritance Links" ) );
    }
    else if ( stringClass == "LMRelationship" )
    {
       p = new LMRelationship( this );
       p->setFolder( tr( "Relationships" ) );
    }
    else if ( stringClass == "LMRule" )
    {
        p = new LMRule( this );
        p->setFolder( tr( "Rules" ) );
    }
    else if ( stringClass == "DCallOut" )
    {
        p = new DCallOut( this );
        p->setFolder( tr( "MarkUp" ) );
    }
    else if ( stringClass == "DTitle" )
    {
        p = new DTitle( this );
        p->setFolder( tr( "MarkUp" ) );
    }
    else if ( stringClass == "DTitleDoc" )
    {
        p = new DTitleDoc( this );
        p->setFolder( tr( "MarkUp" ) );
    }

    if ( !p )
    {
        doMessageBox( "WARNING", "Create Object...", "Unknown class requested " + s );
        return nullptr;
    }

    if ( p->inherits( "DObject" ) )
    {
        DObject *pObject = (DObject*)p; 
        DGraphicsProxyItem *pProxy = new DGraphicsProxyObject( pObject );
        pProxy->doCreateGraphicsObject();
        pProxy->setScene( pScene );
        pObject->setProxy( pProxy );
        pObject->setPos( QPointF( pScene->width() / 2, pScene->height() / 2 ) ); // to pos when created by AWFactoryWidget
    }

    // announce the birth of a new child
    emit signalCreated( p );    
    emit signalCreated( this, p );
    // set parent modified by default (can be set back if this is a load)           
    setModified();              

   return p;
}

// same as in DDiagram - but here also as makes compiler happy
void LMModel::getValidationConfiguration()
{
    mapValidationConfiguration.clear();
    getValidationConfiguration( mapValidationConfiguration );
}

/*!
 * \brief Load our validation data. 
 *  
 * Each class has a static method that is called to load its validation data. 
 * This does not do the validation just build the data representing all of the 
 * validations we can do. 
 *  
 * The validation level can then be configured. 
 *  
 * Each class will then access this data during a validation to determine what 
 * kind of a message (if any) to emit for a failed validation. 
 *  
 * \note This is maintained in the diagram as the diagram is the top level for 
 * running a validation.  
 * 
 * \author pharvey (11/28/19)
 * 
 * \param mapClasses 
 */
ADValidationDatabase LMModel::getValidationConfiguration( ADValidationDatabase &mapClasses )
{
    // add ourself
    ADValidationStrictness mapStrictness;
    // Low
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Name missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Comment missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Description missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Author missing" ), ADValidation::Information );
        mapStrictness.insert( ADValidation::Low, mapValidations );
    }

    // Medium:
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Name missing" ), ADValidation::Warning );
        mapValidations.insert( tr( "Comment missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Description missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Author missing" ), ADValidation::Warning );
        mapStrictness.insert( ADValidation::Medium, mapValidations );
    }

    // High:
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Name missing" ), ADValidation::Error );
        mapValidations.insert( tr( "Comment missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Description missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Author missing" ), ADValidation::Error );
        mapStrictness.insert( ADValidation::High, mapValidations );
    }

    //
    mapClasses.insert( "LMModel", mapStrictness );

    // Top Level                                                                     
    LMRule::getValidationConfiguration( mapClasses );             
    LMDomain::getValidationConfiguration( mapClasses );           
    LMDataItem::getValidationConfiguration( mapClasses );         
    LMAssociativeLink::getValidationConfiguration( mapClasses );         
    LMEntityAssociative::getValidationConfiguration( mapClasses );         
    LMEntityNormal::getValidationConfiguration( mapClasses );         
    LMInheritance::getValidationConfiguration( mapClasses );         
    LMInheritanceLink::getValidationConfiguration( mapClasses );         
    LMRelationship::getValidationConfiguration( mapClasses );         

    return mapClasses;
}

void LMModel::doValidate( ADValidation::Strictness /* nStrictness */, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    // use our nStrictness - not the one provided

    QString stringValidation;
    ADValidation::Severity nSeverity;

    // Name...                                                                                                                        
    stringValidation = tr("Name missing");
    nSeverity = getValidationSeverity( "LMModel", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getName().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation );                                         

    // Comment...                                                                                                                     
    stringValidation = tr("Comment missing");
    nSeverity = getValidationSeverity( "LMModel", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getComment().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Description...                                                                                                                 
    stringValidation = tr("Description missing");                                                                         
    nSeverity = getValidationSeverity( "LMModel", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getDescription().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Annotation...                                                                                                                 
    stringValidation = tr("Annotation missing");                                                                         
    nSeverity = getValidationSeverity( "LMModel", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getAnnotation().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Author...                                                                                                                        
    stringValidation = tr("Author missing");
    nSeverity = getValidationSeverity( "LModel", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getAuthor().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Author...                                                                                                                        
    stringValidation = tr("Version missing");
    nSeverity = getValidationSeverity( "LModel", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getVer().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // check our children                                                                                                              
    QList<ADObject*>    listChildren = getObjects( QString(), QString(), QString() );                                            
    ADObject *          pObject;
    int                 nIndex = -1;                                                                                           

    foreach( pObject, listChildren )
    {                                                                                                                                  
        nIndex++;                                                                                                                      

        // append child to path                                                                                                        
        listObjectPathItems += ADObjectPathItem( nValueType, pObject, pObject->metaObject()->className(), pObject->getName(), nIndex );
        // validate child                                                                                                              
        pObject->doValidate( nStrictness, nValueType, listObjectPathItems );                                                                        
        // remove child from path                                                                                                      
        listObjectPathItems.pop_back();                                                                                                
    }                                                                                                                                  
}

/*!
 * \brief Creates a tab widget with a number of tabs for editing/viewing object properties.
 *  
 * We introduce notes \sa LMObject and some other things. So add tabs for them. 
 *  
 * \sa LMDiagramDisplayWidget 
 * \sa LMNotesWidget
 * 
 * \author pharvey (5/8/20)
 * 
 * \param pWidgetParent 
 * 
 * \return AWDataWidget* 
 */
AWDataWidget *LMModel::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = DDiagram::getObjectWidget( pWidgetParent );
    
    p->insertTab( 1, new LMDiagramDisplayWidget( this, p ), tr("Display") );
    // Factory has Domains etc listed but with less columns. So *may* want to reintroduce these...
    // p->addTab( new LMDomainsWidget( this, p ), tr("Domains") );
    // p->addTab( new LMDataItemsWidget( this, p ), tr("Data Items") );
    // p->addTab( new LMRulesWidget( this, p ), tr("Rules") );
    {
        LMNotesWidget *pWidget = new LMNotesWidget( p, getDescription(), getAnnotation() );
        connect( pWidget, SIGNAL(signalDescriptionChanged(const QString&)), SLOT(slotDescription(const QString &)) );
        connect( pWidget, SIGNAL(signalAnnotationChanged(const QString&)), SLOT(slotAnnotation(const QString &)) );
        p->insertTab( 2, pWidget, tr("Notes") );
    }

    p->setCurrentIndex( 0 );

    return p;
}

QMenu *LMModel::getContextMenu( QWidget *pParent )
{
    QMenu *     pMenu = DDiagram::getContextMenu( pParent );
    QAction *   pAction;

    QMenu *pMenuCreate = new QMenu( tr( "Create" ) );
    pMenu->insertMenu( pMenu->actions().first(), pMenuCreate );

    pAction = new QAction( QPixmap( ":W/PhysicalModel48x48" ), tr("Physical Model From This"), this );
    connect( pAction, SIGNAL(triggered()), this, SLOT(slotCreatePhysicalModel()) );
    pMenuCreate->addAction( pAction );

    return pMenu; // caller invokes and then deletes
}

/*!
    getDataTypes
  
    Return a NULL terminated list of data types supported by this model.  
*/    
const char ** LMModel::getDataTypes()
{
    return LMDataTypes;
}

/*!
    getDataType
    
    Return index to Data Type else -1.
*/
int LMModel::getDataType( const QString &stringDataType )
{
    for ( int n=0; LMDataTypes[n]; n++ )
    {
        if ( stringDataType == LMDataTypes[n] )
            return n;
    }

    return (-1);
}

/*!
    getDataTypeSyntax
  
    Return a NULL terminated list of data type syntax. The index of
    each corresponds to the index into the data types list.
*/    
const char ** LMModel::getDataTypeSyntax()
{
    return LMDataTypeSyntax;
}

/*!
    getDataTypeDescs
  
    Return a NULL terminated list of data types descriptions. The index of
    each desc corresponds to the index into the data types list.
*/    
const char ** LMModel::getDataTypeDescs()
{
    return LMDataTypeDescs;
}

/*!
    Return list of Domain names.
    
    Good for validation and pick lists.
*/
QStringList LMModel::getDomains()
{
    QStringList         listNames;
    QList<ADObject*>    listDomains = getObjects( QString(), QString(), "LMDomain" );
    ADObject *          pObject;

    foreach( pObject, listDomains )
    {
        listNames += pObject->getName();
    }

    return listNames;
}

/*!
    Return the Domain object for the given name.
*/
LMDomain * LMModel::getDomain( const QString str, const bool cs )
{
    QList<ADObject*>    listDomains = getObjects( QString(), QString(), "LMDomain" );
    ADObject *          pObject;

    foreach( pObject, listDomains )
    {
        LMDomain *pDomain = (LMDomain*)pObject;
        if ( cs )
        {
            if ( str == pDomain->getName() ) return pDomain;
        }
        else
        {
            if ( str.toLower() == pDomain->getName().toLower() ) return pDomain;
        }
    }

    return nullptr;
}

/*!
    getIsStandardDataType
    
    Returns true if the given data type is a standard data type for an LMModel.
*/
bool LMModel::getIsStandardDataType( const QString &str )
{
    int n;
    const char ** p = LMDataTypes;

    for ( n=0; p[n]; n++ )
    {
        if ( p[n] == str.toUpper() || p[n] == str.toLower() )
            return true;
    }

    return false;
}

/*!
    getIsDomainDataType
    
    Returns true if the given Domain name exists.
*/    
bool LMModel::getIsDomainDataType( const QString &str )
{
    if ( getIsStandardDataType( str ) ) return false;

    QList<ADObject*>    listDomains = getObjects( QString(), QString(), "LMDomain" );
    ADObject *          pObject;
    QString             stringName = str.toLower();

    foreach( pObject, listDomains )
    {
        if ( stringName == pObject->getName().toLower() )
            return true;
    }

    return false;
}

QDomElement LMModel::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = DDiagram::doSave( pdomDoc, pdomElemParent );
    doSaveLMObject( pdomDoc, &domElemThis );

    domElemThis.setAttribute( "DisplayFlags", nDisplayFlags );

    return domElemThis;
}

bool LMModel::doLoad( QDomElement *pdomElemThis )
{
    DDiagram::doLoad( pdomElemThis );
    LMObject::doLoadLMObject( pdomElemThis );

    nDisplayFlags = pdomElemThis->attribute( "DisplayFlags", QString::number( nDisplayFlags ) ).toInt();

    return true;
}

/*!
    doClearDisplayFlag
    
    Clears given flag in all entities.
 */
void LMModel::doClearDisplayFlag( int nFlag )
{ 
    nDisplayFlags &= ~nFlag; 
}

bool LMModel::slotOpenEditor()
{
    if ( pEditorWidget ) return true;

    // create view
    pEditorWidget = new LMModelEditorWidget( this, getEditorWidgetParent() );

    if ( isCrossHairs() ) doCreateCrossHairs();

    emit signalEditorOpened( pEditorWidget );
    emit signalEditorOpened( this, pEditorWidget );

    return true;
}

void LMModel::slotCreatePhysicalModel()
{
    signalCreatePhysicalModel( this );
}

/*!
    getModel
    
    This exists because we do not want to create duplicate objects during a paste.
*/
ADObject *LMModel::getObject( QDomElement *pdomElem )
{
    int     nOID            = pdomElem->attribute( "OID", "0" ).toInt();
    QString stringName      = pdomElem->attribute( "Name" );
    QString stringClass     = pdomElem->tagName();

    QList<ADObject*> listChildren = getObjects();

    if ( listChildren.count() > 0 )
    {
        QObject *           pobject;
        ADObject *          pmodelFound;

        // Search for match on OID first because it is a more accurate result. This
        // handles the case where we are pasting to same LMModel.
        foreach( pobject, listChildren )
        {
            if ( !(pobject->metaObject()->className() == stringClass) )
                continue;
            pmodelFound = (ADObject*)pobject;
            if ( pmodelFound->getOID() != nOID )
                continue;
            if ( pmodelFound->getName() != stringName )
                continue;

            //
            // FOUND  IT!
            //
            return pmodelFound;

        } // while

        // No OID match so lets try to be smart and match on remaining criteria
        // this handles the condition where a paste is being made to another LMModel
        // where some of the dependent objects exist but with diff OID. This is not
        // perfect but close enough for now.
        foreach( pobject, listChildren )
        {
            if ( !(pobject->metaObject()->className() == stringClass) )
                continue;
            pmodelFound = (ADObject*)pobject;
            if ( pmodelFound->getName() != stringName )
                continue;

            //
            // FOUND  IT!
            //
            return pmodelFound;

        } // while
    }

    return 0;
}

/*!
    Set focus to ourself and invoke our property view if we are the target.
*/
void LMModel::doGoToView( ADObjectPathList listObjectPathItems, uint nDepth )
{
    slotOpenEditor();

    if ( int(nDepth) >= listObjectPathItems.count() )
    {
        slotObjectDialog();
        return; 
    }
    // 
    ADObject::doGoToView( listObjectPathItems, nDepth );
}

bool LMModel::doDragDrop( const QStringList &stringList, const QPointF &pointScene )
{
    QString stringClass = stringList.first();

    if ( !canDrop( stringClass, pointScene ) ) return false;

    //
    slotSelectNone();
    ADObject *p = getObject( stringClass );
    if ( !p ) return false;
    if ( !p->inherits( "DObject" ) )
    {
        Q_ASSERT( p->inherits( "AWObject" ) );
        // just created LMRule or similar
        ((AWObject*)p)->slotObjectDialog();
        return true;
    }

    DObject *pObject = (DObject*)p;

    // common
    if ( isSnapGrid() && pObject->inherits( "DRectangle" ) ) pObject->setPos( getGridCenter( pointScene ) );
    else pObject->setPos( pointScene );

    getSelectionManager()->setSelected( pObject );
    doEnsurePages();

   return true; 
}

