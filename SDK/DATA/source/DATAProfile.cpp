#include "LibInfo.h"
#include "DATAProfile.h"

#include <QProgressDialog>

#include "DATAConnection.h"
#include "DATAStatement.h"
#include "DATADataType.h"

DATAInfo::DATAInfo( uint nInfo, const QString &stringInfo, const QVariant &vValue, const QString &stringDescription )
{
    this->nInfo = nInfo;
    this->stringInfo = stringInfo;
    this->vValue = vValue;
    this->stringDescription = stringDescription;
}

DATAInfo::DATAInfo()
{
    qDeleteAll( mapValues );
}

bool DATAInfo::isSupported( const QString &s )
{
    if ( !mapValues.contains( s ) ) return false;
    return mapValues[s]->vValue.toString() == "Y";
}

DATAInfo *DATAInfo::getSupported()
{
    DATAInfo *pInfo;
    foreach( pInfo, mapValues )
    {
        if ( !pInfo->vValue.isNull() && pInfo->vValue.toString() == "Y" ) return pInfo;
    }
    return new DATAInfo();
}

QString DATAInfo::getValueEncoded( DATAInfo *pInfo )
{
    if ( !pInfo->mapValues.count() ) return pInfo->vValue.toString();

    QString s;
    DATAInfo *p;
    foreach( p, pInfo->mapValues )
    {
        if ( !p->vValue.isNull() && p->vValue.toString() == "Y" ) 
        {
            if ( s.isEmpty() ) s = p->stringInfo + "=Y";
            else s += ( "," + p->stringInfo + "=Y" );
        }
        else
        {
            if ( s.isEmpty() ) s = p->stringInfo + "=N";
            else s += ( "," + p->stringInfo + "=N" );
        }
    }

    return s;
}

/*!
 * \brief Decode the value. 
 *  
 * The value is expected to be in the form... 
 *  
 * KeyValue: "Key=Value" 
 * Value: comma seperated list of KeyValue 
 * 
 * \author pharvey (1/26/21)
 * 
 * \param stringValue 
 * 
 * \return QMap<QString,DATAInfo*>
 */
QMap<QString,DATAInfo*> DATAInfo::getValueDecoded( const QString &stringValue )
{
    QMap<QString,DATAInfo*> m;

#if QT_VERSION < 0x060000
    QStringList listComma = stringValue.split( ',', QString::SkipEmptyParts );
#else
    QStringList listComma = stringValue.split( ',', Qt::SkipEmptyParts );
#endif
    // descreet value (not encoded)?
    if ( listComma.count() <= 1 ) return m;
    // bitmask
    QString stringPair;
    foreach( stringPair, listComma )
    {
        QStringList listKeyValue = stringPair.split( '=' );
        if ( listKeyValue.count() != 2 || ( listKeyValue.at( 1 ) != "Y" && listKeyValue.at( 1 ) != "N" ) )
        { 
            qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << stringValue;
            qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << listKeyValue;
            return QMap<QString,DATAInfo*>();
        }
        // \todo
    }

    return m;
}

DATAAttr::DATAAttr( uint nAttr, const QString &stringAttr, const QVariant &vValue, const QString &stringDescription )
{
    this->nAttr = nAttr;
    this->stringAttr = stringAttr;
    this->vValue = vValue;
    this->stringDescription = stringDescription;
}

DATAAttr::DATAAttr()
{
    qDeleteAll( mapValues );
}

QString DATAAttr::getValueEncoded( DATAAttr *pAttr )
{
    if ( !pAttr->mapValues.count() ) return pAttr->vValue.toString();

    QString s;
    DATAAttr *p;
    foreach( p, pAttr->mapValues )
    {
        if ( !p->vValue.isNull() && p->vValue.toString() == "Y" ) 
        {
            if ( s.isEmpty() ) s = p->stringAttr + "=Y";
            else s += ( "," + p->stringAttr + "=Y" );
        }
        else
        {
            if ( s.isEmpty() ) s = p->stringAttr + "=N";
            else s += ( "," + p->stringAttr + "=N" );
        }
    }

    return s;
}

DATAProfile::DATAProfile( sqlite3 *p, const QString &s )
{
    stringKey       = s;
    bModified       = false;
    pDatabase       = p;
    stringOS        = QSysInfo::productType();
    stringOS_VER    = QSysInfo::productVersion();
}

DATAProfile::DATAProfile( sqlite3 *p, DATAConnection *pConnection )
{
    stringKey       = pConnection->getProfileKey();
    bModified       = false;
    pDatabase       = p;
    stringOS        = QSysInfo::productType();
    stringOS_VER    = QSysInfo::productVersion();
    mapInfo         = getInfo( pConnection );
    // currently get a segfault (on free of statement) when we call SQLGetTypeInfo with sqlite 
    // hopefully this gets resolved but for now we avoid calling it
//    if ( mapInfo.value( "SQL_DBMS_NAME" )->vValue.toString() != "SQLite" )
    mapDataTypes    = getDataTypes( pConnection );
    mapFunctions    = getFunctions( pConnection );
    mapAttr         = getAttr( pConnection );
}

DATAProfile::DATAProfile( sqlite3 *p, int nID )
{
    bModified       = false;
    pDatabase       = p;

    doLoad( nID );
    doLoadInfo( nID );
    doLoadDataTypes( nID );
    doLoadFunctions( nID );
    doLoadAttrs( nID );
}

DATAProfile::~DATAProfile()
{
    doClear();
}

DATADataTypeSpec *DATAProfile::getDataTypeSpec( const QString &stringTYPE_NAME )
{
    DATADataTypeSpec *pTypeSpec = mapDataTypes.value( stringTYPE_NAME );
    if ( pTypeSpec ) return pTypeSpec;
     
    printf( "[PAH][%s][%s][%d] Could not find DATADataTypeSpec for TYPE_NAME %s. Attempting to make it work anyway.\n", __FILE__, __FUNCTION__, __LINE__, stringTYPE_NAME.toUtf8().constData() );
    DATAInfo *pInfo = mapInfo.value( "SQL_DBMS_NAME" );
    Q_ASSERT( pInfo );
    // SQLite
    if ( pInfo->vValue.toString().contains( "SQLite", Qt::CaseInsensitive ) )
    {
        // SQColumns:TYPE_NAME can return NCHAR but SQLGetTypeInfo:TYPE_NAME does not so map to CHAR and get details from there. 
        if ( stringTYPE_NAME.left( 5 ) == "NCHAR" )
            pTypeSpec = mapDataTypes.value( "CHAR" );
        else if ( stringTYPE_NAME.left( 8 ) == "NVARCHAR" )
            pTypeSpec = mapDataTypes.value( "VARCHAR" );
    }

    return pTypeSpec;
}

bool DATAProfile::doSave()
{
    int nProgress = 0;
    QProgressDialog Progress( nullptr, Qt::Popup );
    Progress.setCancelButton( nullptr );
    Progress.setMaximum( 8 );
    Progress.setLabelText( "Saving Profile..." );
    qApp->processEvents();

    // delete self from profiles database
    Progress.setLabelText( "Saving Profile Delete existing..." );
    Progress.setValue( nProgress++ );
    qApp->processEvents();
    doDelete();

    // insert self into profiles database
    char *pszErrMsg = 0;  
    QString stringSQL = QString( "\
INSERT INTO Profiles (  \
KEY,                    \
OS,                     \
OS_VER )                \
VALUES (  \
'%1', \
'%2', \
'%3' )" )
.arg( stringKey )
.arg( stringOS )
.arg( stringOS_VER );

    int nRetCode = sqlite3_exec( pDatabase, stringSQL.toLatin1().constData(), NULL, 0, &pszErrMsg );
    if ( nRetCode != SQLITE_OK )
    {
        qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << pszErrMsg;
        sqlite3_free( pszErrMsg );
        return false;
    }

    Progress.setLabelText( "Saving Profile Fetch ID..." );
    Progress.setValue( nProgress++ );
    qApp->processEvents();
    int nID = 0;
    if ( !getID( &nID ) ) return false;

    Progress.setLabelText( "Saving Profile Data Types..." );
    Progress.setValue( nProgress++ );
    qApp->processEvents();
    if ( !doInsertDataTypes( nID ) ) return false;

    Progress.setLabelText( "Saving Profile Functions..." );
    Progress.setValue( nProgress++ );
    qApp->processEvents();
    if ( !doInsertFunctions( nID ) ) return false;

    Progress.setLabelText( "Saving Profile Info..." );
    Progress.setValue( nProgress++ );
    qApp->processEvents();
    if ( !doInsertInfos( nID ) ) return false;

    Progress.setLabelText( "Saving Profile Attributes..." );
    Progress.setValue( nProgress++ );
    qApp->processEvents();
    if ( !doInsertAttrDbcs( nID ) ) return false;

    setModified( false );

    return true;
}

QDomElement DATAProfile::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = pdomDoc->createElement( "Profile" );
    domElemThis.setAttribute( "Key", stringKey );

    // features
    {
        QDomElement domElem = pdomDoc->createElement( "Info" );
        QMapIterator<QString,DATAInfo*> i(mapInfo);
        while ( i.hasNext() ) 
        {
            i.next();
//            domElem.setAttribute( i.key(), i.value() );
        }
        domElemThis.appendChild( domElem );
    }

    // data types
    {
        QDomElement domElem = pdomDoc->createElement( "DataTypes" );
        QMapIterator<QString,DATADataTypeSpec*> i(mapDataTypes);
        while ( i.hasNext() ) 
        {
            i.next();
            QDomElement Elem = pdomDoc->createElement( i.key() );
            DATADataTypeSpec *pDataTypeSpec = i.value();

            Elem.setAttribute( "TYPE_NAME", pDataTypeSpec->TYPE_NAME );
            Elem.setAttribute( "nDATA_TYPE", pDataTypeSpec->nDATA_TYPE );
            Elem.setAttribute( "DATA_TYPE", pDataTypeSpec->DATA_TYPE );
            Elem.setAttribute( "COLUMN_SIZE", pDataTypeSpec->COLUMN_SIZE );
            Elem.setAttribute( "LITERAL_PREFIX", pDataTypeSpec->LITERAL_PREFIX );
            Elem.setAttribute( "LITERAL_SUFFIX", pDataTypeSpec->LITERAL_SUFFIX );
            Elem.setAttribute( "CREATE_PARAMS", pDataTypeSpec->CREATE_PARAMS );
            Elem.setAttribute( "NULLABLE", pDataTypeSpec->NULLABLE );
            Elem.setAttribute( "CASE_SENSITIVE", pDataTypeSpec->CASE_SENSITIVE );
            Elem.setAttribute( "SEARCHABLE", pDataTypeSpec->SEARCHABLE );
            Elem.setAttribute( "UNSIGNED_ATTRIBUTE", pDataTypeSpec->UNSIGNED_ATTRIBUTE );
            Elem.setAttribute( "FIXED_PREC_SCALE", pDataTypeSpec->FIXED_PREC_SCALE );
            Elem.setAttribute( "AUTO_UNIQUE_VALUE", pDataTypeSpec->AUTO_UNIQUE_VALUE );
            Elem.setAttribute( "LOCAL_TYPE_NAME", pDataTypeSpec->LOCAL_TYPE_NAME );
            Elem.setAttribute( "MINIMUM_SCALE", pDataTypeSpec->MINIMUM_SCALE );
            Elem.setAttribute( "MAXIMUM_SCALE", pDataTypeSpec->MAXIMUM_SCALE );
            Elem.setAttribute( "SQL_DATA_TYPE", pDataTypeSpec->SQL_DATA_TYPE );
            Elem.setAttribute( "SQL_DATETIME_SUB", pDataTypeSpec->SQL_DATETIME_SUB );
            Elem.setAttribute( "NUM_PREC_RADIX", pDataTypeSpec->NUM_PREC_RADIX );
            Elem.setAttribute( "INTERVAL_PRECISION", pDataTypeSpec->INTERVAL_PRECISION );

            domElem.appendChild( Elem );
        }
        domElemThis.appendChild( domElem );
    }

    return domElemThis;
}

bool DATAProfile::doLoad( QDomElement *pdomElemThis /* Profile */ )
{
    doClear();

    QDomElement         domElem;
    QDomNode            domNode;
    domNode = pdomElemThis->firstChild();
    while( !domNode.isNull() ) 
    {
        domElem = domNode.toElement();
        if( domElem.isNull() ) 
        {
            domNode = domNode.nextSibling();
            continue;
        }

        if ( domElem.tagName() == "Info" ) doLoadInfo( &domElem );
        else if ( domElem.tagName() == "DataTypes" ) doLoadDataTypes( &domElem );

        domNode = domNode.nextSibling();
    }

    return true;
}

void DATAProfile::doClear()
{
    // leave stringKey alone
    qDeleteAll( mapInfo ); mapInfo.clear();
    qDeleteAll( mapDataTypes ); mapDataTypes.clear();
    qDeleteAll( mapFunctions ); mapFunctions.clear();
    qDeleteAll( mapAttr ); mapAttr.clear();
    bModified = false;
}

bool DATAProfile::getID( int *pnID )
{
    QString stringSQL = QString("SELECT ID FROM Profiles WHERE KEY = '%1'").arg( stringKey );

    sqlite3_stmt *pStatement;
    int nRetCode = sqlite3_prepare_v2( pDatabase, stringSQL.toLatin1().constData(), -1, &pStatement, NULL );
    if ( nRetCode != SQLITE_OK )
    {
        qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << sqlite3_errmsg( pDatabase );
        return false;
    }

    nRetCode = sqlite3_step( pStatement );
    if ( nRetCode != SQLITE_ROW && nRetCode != SQLITE_DONE ) 
    {
        qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << sqlite3_errmsg( pDatabase );
        sqlite3_finalize( pStatement );
        return false;
    }
    if ( nRetCode == SQLITE_DONE ) 
    {
        sqlite3_finalize( pStatement );
        return false;
    }

    *pnID = sqlite3_column_int( pStatement, 0 );

    sqlite3_finalize( pStatement );

    return true;
}

bool DATAProfile::doLoad( int nID )
{
    sqlite3_stmt *pStatement;
    QString stringSQL = QString( "SELECT * FROM Profiles WHERE ID = %1" ).arg( nID );
    int nRetCode = sqlite3_prepare_v2( pDatabase, stringSQL.toLatin1().constData(), -1, &pStatement, NULL );
    if ( nRetCode != SQLITE_OK )
    {
        qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << sqlite3_errmsg( pDatabase );
        return false;
    }

    // process results
    nRetCode = sqlite3_step( pStatement );
    while ( nRetCode != SQLITE_DONE )
    {
        // should never be busy
        Q_ASSERT( nRetCode != SQLITE_BUSY );

        // error?
        if ( nRetCode != SQLITE_ROW )
        {
            qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << sqlite3_errmsg( pDatabase );
            sqlite3_finalize( pStatement );
            return false;
        }

        // get data
        stringOS        = (const char*)sqlite3_column_text( pStatement, 2 );
        stringOS_VER    = (const char*)sqlite3_column_text( pStatement, 3 );

        // next
        nRetCode = sqlite3_step( pStatement );
    }

    // fini
    sqlite3_finalize( pStatement );

    return true;
}

bool DATAProfile::doLoadInfo( int nID )
{
    sqlite3_stmt *pStatement;
    QString stringSQL = QString( "SELECT * FROM Info WHERE ProfileID = %1" ).arg( nID );
    int nRetCode = sqlite3_prepare_v2( pDatabase, stringSQL.toLatin1().constData(), -1, &pStatement, NULL );
    if ( nRetCode != SQLITE_OK )
    {
        qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << sqlite3_errmsg( pDatabase );
        return false;
    }

    // process results
    nRetCode = sqlite3_step( pStatement );
    while ( nRetCode != SQLITE_DONE )
    {
        // should never be busy
        Q_ASSERT( nRetCode != SQLITE_BUSY );

        // error?
        if ( nRetCode != SQLITE_ROW )
        {
            qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << sqlite3_errmsg( pDatabase );
            sqlite3_finalize( pStatement );
            return false;
        }

        // get data
        DATAInfo *pInfo     = new DATAInfo();
        pInfo->stringInfo   = (const char *)sqlite3_column_text( pStatement, 1 );
        pInfo->nInfo        = g_Constants->mapInfoNums[pInfo->stringInfo];
        pInfo->vValue       = (const char *)sqlite3_column_text( pStatement, 2 );
        pInfo->mapValues    = DATAInfo::getValueDecoded( pInfo->vValue.toString() );
        mapInfo.insert( pInfo->stringInfo, pInfo );

        // next
        nRetCode = sqlite3_step( pStatement );
    }

    // fini
    sqlite3_finalize( pStatement );

    return true;
}

bool DATAProfile::doLoadDataTypes( int nID )
{
    sqlite3_stmt *pStatement;
    QString stringSQL = QString( "SELECT * FROM DataTypes WHERE ProfileID = %1" ).arg( nID );
    int nRetCode = sqlite3_prepare_v2( pDatabase, stringSQL.toLatin1().constData(), -1, &pStatement, NULL );
    if ( nRetCode != SQLITE_OK )
    {
        qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << sqlite3_errmsg( pDatabase );
        return false;
    }

    // process results
    nRetCode = sqlite3_step( pStatement );
    while ( nRetCode != SQLITE_DONE )
    {
        // should never be busy
        Q_ASSERT( nRetCode != SQLITE_BUSY );

        // error?
        if ( nRetCode != SQLITE_ROW )
        {
            qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << sqlite3_errmsg( pDatabase );
            sqlite3_finalize( pStatement );
            return false;
        }

        // get data
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME                = (const char *)sqlite3_column_text( pStatement, 1 );          
        pDataTypeSpec->nDATA_TYPE               = sqlite3_column_int( pStatement, 2 );          
        pDataTypeSpec->DATA_TYPE                = (const char *)sqlite3_column_text( pStatement, 3 );          
        pDataTypeSpec->COLUMN_SIZE              = (const char *)sqlite3_column_text( pStatement, 4 );        
        pDataTypeSpec->LITERAL_PREFIX           = (const char *)sqlite3_column_text( pStatement, 5 );     
        pDataTypeSpec->LITERAL_SUFFIX           = (const char *)sqlite3_column_text( pStatement, 6 );     
        pDataTypeSpec->CREATE_PARAMS            = (const char *)sqlite3_column_text( pStatement, 7 );      
        pDataTypeSpec->NULLABLE                 = (const char *)sqlite3_column_text( pStatement, 8 );           
        pDataTypeSpec->CASE_SENSITIVE           = (const char *)sqlite3_column_text( pStatement, 9 );     
        pDataTypeSpec->SEARCHABLE               = (const char *)sqlite3_column_text( pStatement, 10 );         
        pDataTypeSpec->UNSIGNED_ATTRIBUTE       = (const char *)sqlite3_column_text( pStatement, 11 ); 
        pDataTypeSpec->FIXED_PREC_SCALE         = (const char *)sqlite3_column_text( pStatement, 12 );   
        pDataTypeSpec->AUTO_UNIQUE_VALUE        = (const char *)sqlite3_column_text( pStatement, 13 );  
        pDataTypeSpec->LOCAL_TYPE_NAME          = (const char *)sqlite3_column_text( pStatement, 14 );    
        pDataTypeSpec->MINIMUM_SCALE            = (const char *)sqlite3_column_text( pStatement, 15 );      
        pDataTypeSpec->MAXIMUM_SCALE            = (const char *)sqlite3_column_text( pStatement, 16 );      
        pDataTypeSpec->SQL_DATA_TYPE            = (const char *)sqlite3_column_text( pStatement, 17 );      
        pDataTypeSpec->SQL_DATETIME_SUB         = (const char *)sqlite3_column_text( pStatement, 18 );   
        pDataTypeSpec->NUM_PREC_RADIX           = (const char *)sqlite3_column_text( pStatement, 19 );     
        pDataTypeSpec->INTERVAL_PRECISION       = (const char *)sqlite3_column_text( pStatement, 20 ); 
        pDataTypeSpec->stringDescription        = (const char *)sqlite3_column_text( pStatement, 21 );  
        pDataTypeSpec->vectorSyntax             = DATADataTypeSpec::getSyntaxDecoded( (const char *)sqlite3_column_text( pStatement, 22 ) );

        mapDataTypes[pDataTypeSpec->TYPE_NAME]  = pDataTypeSpec;

        // next
        nRetCode = sqlite3_step( pStatement );
    }

    // fini
    sqlite3_finalize( pStatement );

    return true;
}

bool DATAProfile::doLoadFunctions( int nID )
{
    sqlite3_stmt *pStatement;
    QString stringSQL = QString( "SELECT * FROM Functions WHERE ProfileID = %1" ).arg( nID );
    int nRetCode = sqlite3_prepare_v2( pDatabase, stringSQL.toLatin1().constData(), -1, &pStatement, NULL );
    if ( nRetCode != SQLITE_OK )
    {
        qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << sqlite3_errmsg( pDatabase );
        return false;
    }

    // process results
    nRetCode = sqlite3_step( pStatement );
    while ( nRetCode != SQLITE_DONE )
    {
        // should never be busy
        Q_ASSERT( nRetCode != SQLITE_BUSY );

        // error?
        if ( nRetCode != SQLITE_ROW )
        {
            qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << sqlite3_errmsg( pDatabase );
            sqlite3_finalize( pStatement );
            return false;
        }

        // get data
        DATAFunction *pFunction = new DATAFunction();
        pFunction->stringFunction       = (const char *)sqlite3_column_text( pStatement, 1 );          
        pFunction->bSupported           = sqlite3_column_int( pStatement, 2 );          
        pFunction->nCompliance          = (CBD::Standards)g_Constants->mapStandardsNums.value( ( const char * ) sqlite3_column_text( pStatement, 3 ) );
        pFunction->nFunction            = g_Constants->mapFunctionNums.value( pFunction->stringFunction );
        mapFunctions.insert( pFunction->nFunction, pFunction );

        // next
        nRetCode = sqlite3_step( pStatement );
    }

    // fini
    sqlite3_finalize( pStatement );

    return true;
}

bool DATAProfile::doLoadAttrs( int nID )
{
    return true;
}

bool DATAProfile::doLoadInfo( QDomElement *pdomElem )
{
    QDomNamedNodeMap NodeMap = pdomElem->attributes();
    for ( int n = 0; n < NodeMap.count(); n++ )
    {
        QDomNode Node = NodeMap.item( n );
        QDomAttr Attr = Node.toAttr();
        if ( Attr.isNull() ) continue;
        if ( Attr.name().isEmpty() ) continue;

//        mapInfo[Attr.name()] = Attr.value();
    }

    return true;
}

bool DATAProfile::doLoadDataTypes( QDomElement *pdomElem )
{
    QDomElement         domElem;
    QDomNode            domNode;
    domNode = pdomElem->firstChild();
    while( !domNode.isNull() ) 
    {
        domElem = domNode.toElement();
        if( domElem.isNull() ) 
        {
            domNode = domNode.nextSibling();
            continue;
        }

        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();

        pDataTypeSpec->TYPE_NAME          = domElem.attribute( "TYPE_NAME"          );
        pDataTypeSpec->nDATA_TYPE         = domElem.attribute( "nDATA_TYPE"         ).toInt();
        pDataTypeSpec->DATA_TYPE          = domElem.attribute( "DATA_TYPE"          );
        pDataTypeSpec->COLUMN_SIZE        = domElem.attribute( "COLUMN_SIZE"        );
        pDataTypeSpec->LITERAL_PREFIX     = domElem.attribute( "LITERAL_PREFIX"     );
        pDataTypeSpec->LITERAL_SUFFIX     = domElem.attribute( "LITERAL_SUFFIX"     );
        pDataTypeSpec->CREATE_PARAMS      = domElem.attribute( "CREATE_PARAMS"      );
        pDataTypeSpec->NULLABLE           = domElem.attribute( "NULLABLE"           );
        pDataTypeSpec->CASE_SENSITIVE     = domElem.attribute( "CASE_SENSITIVE"     );
        pDataTypeSpec->SEARCHABLE         = domElem.attribute( "SEARCHABLE"         );
        pDataTypeSpec->UNSIGNED_ATTRIBUTE = domElem.attribute( "UNSIGNED_ATTRIBUTE" );
        pDataTypeSpec->FIXED_PREC_SCALE   = domElem.attribute( "FIXED_PREC_SCALE"   );
        pDataTypeSpec->AUTO_UNIQUE_VALUE  = domElem.attribute( "AUTO_UNIQUE_VALUE"  );
        pDataTypeSpec->LOCAL_TYPE_NAME    = domElem.attribute( "LOCAL_TYPE_NAME"    );
        pDataTypeSpec->MINIMUM_SCALE      = domElem.attribute( "MINIMUM_SCALE"      );
        pDataTypeSpec->MAXIMUM_SCALE      = domElem.attribute( "MAXIMUM_SCALE"      );
        pDataTypeSpec->SQL_DATA_TYPE      = domElem.attribute( "SQL_DATA_TYPE"      );
        pDataTypeSpec->SQL_DATETIME_SUB   = domElem.attribute( "SQL_DATETIME_SUB"   );
        pDataTypeSpec->NUM_PREC_RADIX     = domElem.attribute( "NUM_PREC_RADIX"     );
        pDataTypeSpec->INTERVAL_PRECISION = domElem.attribute( "INTERVAL_PRECISION" );
        pDataTypeSpec->vectorSyntax       = DATADataTypeSpec::getSyntaxVector( DATADataTypeSpec::getSyntaxString( pDataTypeSpec->TYPE_NAME ) );
        mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;

        domNode = domNode.nextSibling();
    }

    return true;
}

bool DATAProfile::doInsertDataTypes( int nID )
{
    int nRetCode;
    sqlite3_stmt *pStatement;
    const char *szSQL = "INSERT INTO DataTypes ("\
                        "ProfileID,         "\
                        "TYPE_NAME,         "\
                        "nDATA_TYPE,        "\
                        "DATA_TYPE,         "\
                        "COLUMN_SIZE,       "\
                        "LITERAL_PREFIX,    "\
                        "LITERAL_SUFFIX,    "\
                        "CREATE_PARAMS,     "\
                        "NULLABLE,          "\
                        "CASE_SENSITIVE,    "\
                        "SEARCHABLE,        "\
                        "UNSIGNED_ATTRIBUTE,"\
                        "FIXED_PREC_SCALE,  "\
                        "AUTO_UNIQUE_VALUE, "\
                        "LOCAL_TYPE_NAME,   "\
                        "MINIMUM_SCALE,     "\
                        "MAXIMUM_SCALE,     "\
                        "SQL_DATA_TYPE,     "\
                        "SQL_DATETIME_SUB,  "\
                        "NUM_PREC_RADIX,    "\
                        "INTERVAL_PRECISION,"\
                        "Desc,              "\
                        "Syntax )           "\
                        "VALUES ( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ? )";

    nRetCode = sqlite3_prepare_v2( pDatabase, szSQL, -1, &pStatement, NULL );
    if ( nRetCode != SQLITE_OK ) 
    {
        qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << nRetCode << sqlite3_errmsg( pDatabase );
        return false;
    }

    QMapIterator<QString,DATADataTypeSpec*> i( mapDataTypes );
    while ( i.hasNext() ) 
    {
        i.next();

        nRetCode = sqlite3_bind_int( pStatement, 1, nID );
        if ( nRetCode != SQLITE_OK ) 
        {
            qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << nRetCode << sqlite3_errmsg( pDatabase );
            return false;
        }
        nRetCode = sqlite3_bind_text( pStatement, 2, i.value()->TYPE_NAME.toUtf8().constData(), i.value()->TYPE_NAME.size(), SQLITE_TRANSIENT );
        nRetCode = sqlite3_bind_int( pStatement, 3, i.value()->nDATA_TYPE );
        nRetCode = sqlite3_bind_text( pStatement, 4, i.value()->DATA_TYPE.toUtf8().constData(), i.value()->DATA_TYPE.size(), SQLITE_TRANSIENT );
        nRetCode = sqlite3_bind_text( pStatement, 5, i.value()->COLUMN_SIZE.toUtf8().constData(), i.value()->COLUMN_SIZE.size(), SQLITE_TRANSIENT );
        nRetCode = sqlite3_bind_text( pStatement, 6, i.value()->LITERAL_PREFIX.toUtf8().constData(), i.value()->LITERAL_PREFIX.size(), SQLITE_TRANSIENT );
        nRetCode = sqlite3_bind_text( pStatement, 7, i.value()->LITERAL_SUFFIX.toUtf8().constData(), i.value()->LITERAL_SUFFIX.size(), SQLITE_TRANSIENT );
        nRetCode = sqlite3_bind_text( pStatement, 8, i.value()->CREATE_PARAMS.toUtf8().constData(), i.value()->CREATE_PARAMS.size(), SQLITE_TRANSIENT );
        nRetCode = sqlite3_bind_text( pStatement, 9, i.value()->NULLABLE.toUtf8().constData(), i.value()->NULLABLE.size(), SQLITE_TRANSIENT );
        nRetCode = sqlite3_bind_text( pStatement, 10, i.value()->CASE_SENSITIVE.toUtf8().constData(), i.value()->CASE_SENSITIVE.size(), SQLITE_TRANSIENT );
        nRetCode = sqlite3_bind_text( pStatement, 11, i.value()->SEARCHABLE.toUtf8().constData(), i.value()->SEARCHABLE.size(), SQLITE_TRANSIENT );
        nRetCode = sqlite3_bind_text( pStatement, 12, i.value()->UNSIGNED_ATTRIBUTE.toUtf8().constData(), i.value()->UNSIGNED_ATTRIBUTE.size(), SQLITE_TRANSIENT );
        nRetCode = sqlite3_bind_text( pStatement, 13, i.value()->FIXED_PREC_SCALE.toUtf8().constData(), i.value()->FIXED_PREC_SCALE.size(), SQLITE_TRANSIENT );
        nRetCode = sqlite3_bind_text( pStatement, 14, i.value()->AUTO_UNIQUE_VALUE.toUtf8().constData(), i.value()->AUTO_UNIQUE_VALUE.size(), SQLITE_TRANSIENT );
        nRetCode = sqlite3_bind_text( pStatement, 15, i.value()->LOCAL_TYPE_NAME.toUtf8().constData(), i.value()->LOCAL_TYPE_NAME.size(), SQLITE_TRANSIENT );
        nRetCode = sqlite3_bind_text( pStatement, 16, i.value()->MINIMUM_SCALE.toUtf8().constData(), i.value()->MINIMUM_SCALE.size(), SQLITE_TRANSIENT );
        nRetCode = sqlite3_bind_text( pStatement, 17, i.value()->MAXIMUM_SCALE.toUtf8().constData(), i.value()->MAXIMUM_SCALE.size(), SQLITE_TRANSIENT );
        nRetCode = sqlite3_bind_text( pStatement, 18, i.value()->SQL_DATA_TYPE.toUtf8().constData(), i.value()->SQL_DATA_TYPE.size(), SQLITE_TRANSIENT );
        nRetCode = sqlite3_bind_text( pStatement, 19, i.value()->SQL_DATETIME_SUB.toUtf8().constData(), i.value()->SQL_DATETIME_SUB.size(), SQLITE_TRANSIENT );
        nRetCode = sqlite3_bind_text( pStatement, 20, i.value()->NUM_PREC_RADIX.toUtf8().constData(), i.value()->NUM_PREC_RADIX.size(), SQLITE_TRANSIENT );
        nRetCode = sqlite3_bind_text( pStatement, 21, i.value()->INTERVAL_PRECISION.toUtf8().constData(), i.value()->INTERVAL_PRECISION.size(), SQLITE_TRANSIENT );
        nRetCode = sqlite3_bind_text( pStatement, 22, i.value()->stringDescription.toUtf8().constData(), i.value()->stringDescription.size(), SQLITE_TRANSIENT );
        QString stringSyntax = DATADataTypeSpec::getSyntaxEncoded( i.value() );
        nRetCode = sqlite3_bind_text( pStatement, 23, stringSyntax.toUtf8().constData(), stringSyntax.size(), SQLITE_TRANSIENT );

        // Evaluate the prepared SQL statement.
        nRetCode = sqlite3_step( pStatement );
        if ( nRetCode != SQLITE_DONE ) 
        {
            qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << nRetCode << sqlite3_errmsg( pDatabase );
            return false;
        }

        nRetCode = sqlite3_reset( pStatement );
        if ( nRetCode != SQLITE_OK ) 
        {
            qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << nRetCode << sqlite3_errmsg( pDatabase );
            return false;
        }

        nRetCode = sqlite3_clear_bindings( pStatement );
        if ( nRetCode != SQLITE_OK ) 
        {
            qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << nRetCode << sqlite3_errmsg( pDatabase );
            return false;
        }
    }

    // Destroy the prepared statement object.
    nRetCode = sqlite3_finalize(pStatement);
    if ( nRetCode != SQLITE_OK ) 
    {
        qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << nRetCode << sqlite3_errmsg( pDatabase );
    }

    return true;
}

bool DATAProfile::doInsertFunctions( int nID )
{
    QMapIterator<int,DATAFunction*> i( mapFunctions );
    while ( i.hasNext() ) 
    {
        i.next();
        if ( !doInsertFunction( nID, i.value() ) ) return false;
    }
    return true;
}

bool DATAProfile::doInsertFunction( int nID, DATAFunction *pFunction )
{
    char *pszErrMsg = 0;  
    QString stringSQL = QString( "\
INSERT INTO Functions (  \
ProfileID,          \
Function,           \
Supported,          \
Compliance          \
)                   \
VALUES (\
%1,     \
'%2',   \
%4,     \
'%5'    \
)" )
.arg( nID )
.arg( pFunction->stringFunction )
.arg( (int)pFunction->bSupported )
.arg( g_Constants->mapStandardsNames[pFunction->nCompliance] );

    int nRetCode = sqlite3_exec( pDatabase, stringSQL.toLatin1().constData(), NULL, 0, &pszErrMsg );
    if ( nRetCode != SQLITE_OK )
    {
        qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << pszErrMsg;
        sqlite3_free( pszErrMsg );
        return false;
    }

    return true;
}

bool DATAProfile::doInsertInfos( int nID )
{
    QMapIterator<QString,DATAInfo*> i( mapInfo );
    while ( i.hasNext() ) 
    {
        i.next();
        if ( !doInsertInfo( nID, i.value() ) ) return false;
    }
    return true;
}

bool DATAProfile::doInsertInfo( int nID, DATAInfo *pInfo )
{
    char *pszErrMsg = 0;  
    QString stringSQL = QString( "\
INSERT INTO Info (  \
ProfileID,          \
Info,               \
Value               \
)                   \
VALUES (\
%1,     \
'%2',   \
'%3'    \
)" )
.arg( nID )
.arg( g_Constants->mapInfoNames[pInfo->nInfo] )
.arg( DATAInfo::getValueEncoded( pInfo ) );

    int nRetCode = sqlite3_exec( pDatabase, stringSQL.toLatin1().constData(), NULL, 0, &pszErrMsg );
    if ( nRetCode != SQLITE_OK )
    {
        qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << pszErrMsg;
        sqlite3_free( pszErrMsg );
        return false;
    }

    return true;
}

bool DATAProfile::doInsertAttrDbcs( int nID )
{
    int nRetCode;
    sqlite3_stmt *pStatement;
    const char *szSQL = "INSERT INTO AttrDbc ( ProfileID, Attr, Value ) VALUES ( ?, ?, ? );";

    nRetCode = sqlite3_prepare_v2( pDatabase, szSQL, -1, &pStatement, NULL );
    if ( nRetCode != SQLITE_OK ) 
    {
        qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << nRetCode << sqlite3_errmsg( pDatabase );
        return false;
    }

    QMapIterator<QString,DATAAttr*> i( mapAttr );
    while ( i.hasNext() ) 
    {
        i.next();

        nRetCode = sqlite3_bind_int( pStatement, 1, nID );
        if ( nRetCode != SQLITE_OK ) 
        {
            qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << nRetCode << sqlite3_errmsg( pDatabase );
            return false;
        }

        nRetCode = sqlite3_bind_text( pStatement, 2, g_Constants->mapAttrDbcNames[i.value()->nAttr].toLatin1().constData(), -1, SQLITE_TRANSIENT );
        if ( nRetCode != SQLITE_OK ) 
        {
            qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << nRetCode << sqlite3_errmsg( pDatabase );
            return false;
        }
        QString stringValueEncoded = DATAAttr::getValueEncoded( i.value() );
        nRetCode = sqlite3_bind_text( pStatement, 3, stringValueEncoded.toLatin1().constData(), -1, SQLITE_TRANSIENT );
        if ( nRetCode != SQLITE_OK ) 
        {
            qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << nRetCode << sqlite3_errmsg( pDatabase );
            return false;
        }

        // Evaluate the prepared SQL statement.
        nRetCode = sqlite3_step( pStatement );
        if ( nRetCode != SQLITE_DONE ) 
        {
            qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << nRetCode << sqlite3_errmsg( pDatabase );
            return false;
        }

        nRetCode = sqlite3_reset( pStatement );
        if ( nRetCode != SQLITE_OK ) 
        {
            qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << nRetCode << sqlite3_errmsg( pDatabase );
            return false;
        }

        nRetCode = sqlite3_clear_bindings( pStatement );
        if ( nRetCode != SQLITE_OK ) 
        {
            qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << nRetCode << sqlite3_errmsg( pDatabase );
            return false;
        }
    }

    // Destroy the prepared statement object.
    nRetCode = sqlite3_finalize( pStatement );
    if ( nRetCode != SQLITE_OK ) 
    {
        qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << nRetCode << sqlite3_errmsg( pDatabase );
    }

    return true;
}

bool DATAProfile::doInsertUnsupported( int nID )
{
    QMapIterator<int,DATAFunction*> i( mapFunctions );
    while ( i.hasNext() ) 
    {
        i.next();
        if ( !doInsertUnsupported( nID, i.value() ) ) return false;
    }
    return true;
}

bool DATAProfile::doInsertUnsupported( int nID, DATAFunction *pFunction )
{
    DATAUnsupported d;
    foreach( d, pFunction->listUnsupported )
    {
        if ( !doInsertUnsupported( nID, pFunction->stringFunction, d ) ) return false;
    }

    return true;
}

bool DATAProfile::doInsertUnsupported( int nID, const QString &stringFunction, const DATAUnsupported &Uns )
{
    char *pszErrMsg = 0;  
    QString stringSQL = QString( "\
INSERT INTO HYC00 ( \
ProfileID,          \
Function,           \
Option1,            \
Option2,            \
Option3,            \
Context             \
)                   \
VALUES (    \
%1,         \
'%2',       \
'%3',       \
'%4',       \
'%5',       \
'%6'        \
)" )
.arg( nID )
.arg( stringFunction )
.arg( Uns.listOptions.count() > 0 ? Uns.listOptions.at( 0 ) : "" )
.arg( Uns.listOptions.count() > 1 ? Uns.listOptions.at( 1 ) : "" )
.arg( Uns.listOptions.count() > 2 ? Uns.listOptions.at( 2 ) : "" )
.arg( Uns.stringContext );

    int nRetCode = sqlite3_exec( pDatabase, stringSQL.toLatin1().constData(), NULL, 0, &pszErrMsg );
    if ( nRetCode != SQLITE_OK )
    {
        qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << pszErrMsg;
        sqlite3_free( pszErrMsg );
        return false;
    }

    return true;
}

bool DATAProfile::doDelete()                    // Profiles
{
    int nID = 0;
    if ( !getID( &nID ) ) return false;

    doDeleteInfo( nID );   // Info
    doDeleteDataTypes( nID );  // DataTypes
    doDeleteFunctions( nID );  // Functions
    doDeleteAttrs( nID );      // AttrDbc
    doDeleteUnsupported( nID );// HYC00

    QString stringSQL = QString( "DELETE FROM Profiles WHERE ID=%1" ).arg( nID );
    sqlite3_exec( pDatabase, stringSQL.toLatin1().constData(), nullptr, nullptr, nullptr );
    return true;
}

bool DATAProfile::doDeleteInfo( int nID )   // Info
{
    QString stringSQL = QString( "DELETE FROM Info WHERE ProfileID=%1" ).arg( nID );
    sqlite3_exec( pDatabase, stringSQL.toLatin1().constData(), nullptr, nullptr, nullptr );
    return true;
}

bool DATAProfile::doDeleteDataTypes( int nID )  // DataTypes
{
    QString stringSQL = QString( "DELETE FROM DataTypes WHERE ProfileID=%1" ).arg( nID );
    sqlite3_exec( pDatabase, stringSQL.toLatin1().constData(), nullptr, nullptr, nullptr );
    return true;
}

bool DATAProfile::doDeleteFunctions( int nID )  // Functions
{
    QString stringSQL = QString( "DELETE FROM Functions WHERE ProfileID=%1" ).arg( nID );
    sqlite3_exec( pDatabase, stringSQL.toLatin1().constData(), nullptr, nullptr, nullptr );
    return true;
}

bool DATAProfile::doDeleteAttrs( int nID )      // AttrDbc
{
    QString stringSQL = QString( "DELETE FROM AttrDbc WHERE ProfileID=%1" ).arg( nID );
    sqlite3_exec( pDatabase, stringSQL.toLatin1().constData(), nullptr, nullptr, nullptr );
    return true;
}

bool DATAProfile::doDeleteUnsupported( int nID )// HYC00
{
    QString stringSQL = QString( "DELETE FROM HYC00 WHERE ProfileID=%1" ).arg( nID );
    sqlite3_exec( pDatabase, stringSQL.toLatin1().constData(), nullptr, nullptr, nullptr );
    return true;
}

/*!
 * \brief Get ALL info we can get via SQLGetInfo.
 *  
 * This can be used; 
 *  
 * - as a cache when you know you are going to be accessing this info a lot 
 * - when you want to display it 
 * - when you want to save it (perhaps for use when disconnected or when driver does not even exist)
 *  
 * \author pharvey (2020-01-31)
 * 
 * \return QMap&lt;QString,QString&gt; 
 */
QMap<QString,DATAInfo*> DATAProfile::getInfo( DATAConnection *pConnection )
{
    Q_ASSERT( pConnection );
    Q_ASSERT( pConnection->isConnected() );

    QMap<QString,DATAInfo*> mapInfo;

    mapInfo["SQL_ACCESSIBLE_PROCEDURES"] = getInfoData( pConnection, SQL_ACCESSIBLE_PROCEDURES );
    mapInfo["SQL_ACCESSIBLE_TABLES"] = getInfoData( pConnection, SQL_ACCESSIBLE_TABLES );
    mapInfo["SQL_ACTIVE_ENVIRONMENTS"] = getInfoData( pConnection, SQL_ACTIVE_ENVIRONMENTS );
    mapInfo["SQL_COLLATION_SEQ"] = getInfoData( pConnection, SQL_COLLATION_SEQ );
    mapInfo["SQL_DATA_SOURCE_NAME"] = getInfoData( pConnection, SQL_DATA_SOURCE_NAME );
    mapInfo["SQL_DATA_SOURCE_READ_ONLY"] = getInfoData( pConnection, SQL_DATA_SOURCE_READ_ONLY );
    mapInfo["SQL_DATABASE_NAME"] = getInfoData( pConnection, SQL_DATABASE_NAME );
    mapInfo["SQL_DBMS_NAME"] = getInfoData( pConnection, SQL_DBMS_NAME );
    mapInfo["SQL_DBMS_VER"] = getInfoData( pConnection, SQL_DBMS_VER );
    mapInfo["SQL_DESCRIBE_PARAMETER"] = getInfoData( pConnection, SQL_DESCRIBE_PARAMETER );
    mapInfo["SQL_DM_VER"] = getInfoData( pConnection, SQL_DM_VER );
    mapInfo["SQL_DRIVER_NAME"] = getInfoData( pConnection, SQL_DRIVER_NAME );
    mapInfo["SQL_DRIVER_ODBC_VER"] = getInfoData( pConnection, SQL_DRIVER_ODBC_VER );
    mapInfo["SQL_DRIVER_VER"] = getInfoData( pConnection, SQL_DRIVER_VER );
    mapInfo["SQL_EXPRESSIONS_IN_ORDERBY"] = getInfoData( pConnection, SQL_EXPRESSIONS_IN_ORDERBY );
    mapInfo["SQL_LIKE_ESCAPE_CLAUSE"] = getInfoData( pConnection, SQL_LIKE_ESCAPE_CLAUSE );
    mapInfo["SQL_MULT_RESULT_SETS"] = getInfoData( pConnection, SQL_MULT_RESULT_SETS );
    mapInfo["SQL_MULTIPLE_ACTIVE_TXN"] = getInfoData( pConnection, SQL_MULTIPLE_ACTIVE_TXN );
    mapInfo["SQL_NEED_LONG_DATA_LEN"] = getInfoData( pConnection, SQL_NEED_LONG_DATA_LEN );
    mapInfo["SQL_ODBC_VER"] = getInfoData( pConnection, SQL_ODBC_VER );
    mapInfo["SQL_ORDER_BY_COLUMNS_IN_SELECT"] = getInfoData( pConnection, SQL_ORDER_BY_COLUMNS_IN_SELECT );
    mapInfo["SQL_PROCEDURE_TERM"] = getInfoData( pConnection, SQL_PROCEDURE_TERM );
    mapInfo["SQL_ROW_UPDATES"] = getInfoData( pConnection, SQL_ROW_UPDATES );
    mapInfo["SQL_SERVER_NAME"] = getInfoData( pConnection, SQL_SERVER_NAME );
    mapInfo["SQL_USER_NAME"] = getInfoData( pConnection, SQL_USER_NAME );
    mapInfo["SQL_XOPEN_CLI_YEAR"] = getInfoData( pConnection, SQL_XOPEN_CLI_YEAR );
    mapInfo["SQL_DRIVER_HDBC"] = getInfoData( pConnection, SQL_DRIVER_HDBC );
    mapInfo["SQL_DRIVER_HENV"] = getInfoData( pConnection, SQL_DRIVER_HENV );
    mapInfo["SQL_DRIVER_HDESC"] = getInfoData( pConnection, SQL_DRIVER_HDESC );
    mapInfo["SQL_DRIVER_HLIB"] = getInfoData( pConnection, SQL_DRIVER_HLIB );
    mapInfo["SQL_DRIVER_HSTMT"] = getInfoData( pConnection, SQL_DRIVER_HSTMT );
    mapInfo["SQL_MAX_ASYNC_CONCURRENT_STATEMENTS"] = getInfoData( pConnection, SQL_MAX_ASYNC_CONCURRENT_STATEMENTS );
    mapInfo["SQL_MAX_BINARY_LITERAL_LEN"] = getInfoData( pConnection, SQL_MAX_BINARY_LITERAL_LEN );
    mapInfo["SQL_MAX_CHAR_LITERAL_LEN"] = getInfoData( pConnection, SQL_MAX_CHAR_LITERAL_LEN );
    mapInfo["SQL_MAX_STATEMENT_LEN"] = getInfoData( pConnection, SQL_MAX_STATEMENT_LEN );
    mapInfo["SQL_ASYNC_MODE"] = getInfoData( pConnection, SQL_ASYNC_MODE );
    mapInfo["SQL_CONCAT_NULL_BEHAVIOR"] = getInfoData( pConnection, SQL_CONCAT_NULL_BEHAVIOR );
    mapInfo["SQL_CORRELATION_NAME"] = getInfoData( pConnection, SQL_CORRELATION_NAME );
    mapInfo["SQL_CURSOR_COMMIT_BEHAVIOR"] = getInfoData( pConnection, SQL_CURSOR_COMMIT_BEHAVIOR );
    mapInfo["SQL_CURSOR_ROLLBACK_BEHAVIOR"] = getInfoData( pConnection, SQL_CURSOR_ROLLBACK_BEHAVIOR );
    mapInfo["SQL_CURSOR_SENSITIVITY"] = getInfoData( pConnection, SQL_CURSOR_SENSITIVITY );
    mapInfo["SQL_DEFAULT_TXN_ISOLATION"] = getInfoData( pConnection, SQL_DEFAULT_TXN_ISOLATION );
    mapInfo["SQL_FILE_USAGE"] = getInfoData( pConnection, SQL_FILE_USAGE );
    mapInfo["SQL_GROUP_BY"] = getInfoData( pConnection, SQL_GROUP_BY );
    mapInfo["SQL_NULL_COLLATION"] = getInfoData( pConnection, SQL_NULL_COLLATION );
    mapInfo["SQL_ODBC_INTERFACE_CONFORMANCE"] = getInfoData( pConnection, SQL_ODBC_INTERFACE_CONFORMANCE );
    mapInfo["SQL_TXN_CAPABLE"] = getInfoData( pConnection, SQL_TXN_CAPABLE );
    mapInfo["SQL_AGGREGATE_FUNCTIONS"] = getInfoData( pConnection, SQL_AGGREGATE_FUNCTIONS );
    mapInfo["SQL_ALTER_DOMAIN"] = getInfoData( pConnection, SQL_ALTER_DOMAIN );
    mapInfo["SQL_ALTER_TABLE"] = getInfoData( pConnection, SQL_ALTER_TABLE );
    mapInfo["SQL_BATCH_ROW_COUNT"] = getInfoData( pConnection, SQL_BATCH_ROW_COUNT );
    mapInfo["SQL_BATCH_SUPPORT"] = getInfoData( pConnection, SQL_BATCH_SUPPORT );
    mapInfo["SQL_BOOKMARK_PERSISTENCE"] = getInfoData( pConnection, SQL_BOOKMARK_PERSISTENCE );
    mapInfo["SQL_CATALOG_USAGE"] = getInfoData( pConnection, SQL_CATALOG_USAGE );
    mapInfo["SQL_CONVERT_FUNCTIONS"] = getInfoData( pConnection, SQL_CONVERT_FUNCTIONS );
    mapInfo["SQL_DATETIME_LITERALS"] = getInfoData( pConnection, SQL_DATETIME_LITERALS );
    mapInfo["SQL_DYNAMIC_CURSOR_ATTRIBUTES1"] = getInfoData( pConnection, SQL_DYNAMIC_CURSOR_ATTRIBUTES1 );
    mapInfo["SQL_DYNAMIC_CURSOR_ATTRIBUTES2"] = getInfoData( pConnection, SQL_DYNAMIC_CURSOR_ATTRIBUTES2 );
    mapInfo["SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1"] = getInfoData( pConnection, SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1 );
    mapInfo["SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES2"] = getInfoData( pConnection, SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES2 );
    mapInfo["SQL_GETDATA_EXTENSIONS"] = getInfoData( pConnection, SQL_GETDATA_EXTENSIONS );
    mapInfo["SQL_INFO_SCHEMA_VIEWS"] = getInfoData( pConnection, SQL_INFO_SCHEMA_VIEWS );
    mapInfo["SQL_INSERT_STATEMENT"] = getInfoData( pConnection, SQL_INSERT_STATEMENT );
    mapInfo["SQL_KEYSET_CURSOR_ATTRIBUTES1"] = getInfoData( pConnection, SQL_KEYSET_CURSOR_ATTRIBUTES1 );
    mapInfo["SQL_KEYSET_CURSOR_ATTRIBUTES2"] = getInfoData( pConnection, SQL_KEYSET_CURSOR_ATTRIBUTES2 );
    mapInfo["SQL_NUMERIC_FUNCTIONS"] = getInfoData( pConnection, SQL_NUMERIC_FUNCTIONS );
    mapInfo["SQL_OJ_CAPABILITIES"] = getInfoData( pConnection, SQL_OJ_CAPABILITIES );
    mapInfo["SQL_PARAM_ARRAY_ROW_COUNTS"] = getInfoData( pConnection, SQL_PARAM_ARRAY_ROW_COUNTS );
    mapInfo["SQL_PARAM_ARRAY_SELECTS"] = getInfoData( pConnection, SQL_PARAM_ARRAY_SELECTS );
    mapInfo["SQL_POS_OPERATIONS"] = getInfoData( pConnection, SQL_POS_OPERATIONS );
    mapInfo["SQL_SCHEMA_USAGE"] = getInfoData( pConnection, SQL_SCHEMA_USAGE );
    mapInfo["SQL_SCROLL_OPTIONS"] = getInfoData( pConnection, SQL_SCROLL_OPTIONS );
    mapInfo["SQL_SQL92_FOREIGN_KEY_DELETE_RULE"] = getInfoData( pConnection, SQL_SQL92_FOREIGN_KEY_DELETE_RULE );
    mapInfo["SQL_SQL92_FOREIGN_KEY_UPDATE_RULE"] = getInfoData( pConnection, SQL_SQL92_FOREIGN_KEY_UPDATE_RULE );
    mapInfo["SQL_SQL92_GRANT"] = getInfoData( pConnection, SQL_SQL92_GRANT );
    mapInfo["SQL_SQL92_NUMERIC_VALUE_FUNCTIONS"] = getInfoData( pConnection, SQL_SQL92_NUMERIC_VALUE_FUNCTIONS );
    mapInfo["SQL_SQL92_PREDICATES"] = getInfoData( pConnection, SQL_SQL92_PREDICATES );
    mapInfo["SQL_SQL92_RELATIONAL_JOIN_OPERATORS"] = getInfoData( pConnection, SQL_SQL92_RELATIONAL_JOIN_OPERATORS );
    mapInfo["SQL_SQL92_REVOKE"] = getInfoData( pConnection, SQL_SQL92_REVOKE );
    mapInfo["SQL_SQL92_ROW_VALUE_CONSTRUCTOR"] = getInfoData( pConnection, SQL_SQL92_ROW_VALUE_CONSTRUCTOR );
    mapInfo["SQL_SQL92_STRING_FUNCTIONS"] = getInfoData( pConnection, SQL_SQL92_STRING_FUNCTIONS );
    mapInfo["SQL_SQL92_VALUE_EXPRESSIONS"] = getInfoData( pConnection, SQL_SQL92_VALUE_EXPRESSIONS );
    mapInfo["SQL_STANDARD_CLI_CONFORMANCE"] = getInfoData( pConnection, SQL_STANDARD_CLI_CONFORMANCE );
    mapInfo["SQL_STATIC_CURSOR_ATTRIBUTES1"] = getInfoData( pConnection, SQL_STATIC_CURSOR_ATTRIBUTES1 );
    mapInfo["SQL_STATIC_CURSOR_ATTRIBUTES2"] = getInfoData( pConnection, SQL_STATIC_CURSOR_ATTRIBUTES2 );
    mapInfo["SQL_STRING_FUNCTIONS"] = getInfoData( pConnection, SQL_STRING_FUNCTIONS );
    mapInfo["SQL_SUBQUERIES"] = getInfoData( pConnection, SQL_SUBQUERIES );
    mapInfo["SQL_SYSTEM_FUNCTIONS"] = getInfoData( pConnection, SQL_SYSTEM_FUNCTIONS );
    mapInfo["SQL_TIMEDATE_ADD_INTERVALS"] = getInfoData( pConnection, SQL_TIMEDATE_ADD_INTERVALS );
    mapInfo["SQL_TIMEDATE_DIFF_INTERVALS"] = getInfoData( pConnection, SQL_TIMEDATE_DIFF_INTERVALS );
    mapInfo["SQL_TIMEDATE_FUNCTIONS"] = getInfoData( pConnection, SQL_TIMEDATE_FUNCTIONS );
    mapInfo["SQL_TXN_ISOLATION_OPTION"] = getInfoData( pConnection, SQL_TXN_ISOLATION_OPTION );
    mapInfo["SQL_UNION"] = getInfoData( pConnection, SQL_UNION );
    mapInfo["SQL_CONVERT_BIGINT"] = getInfoData( pConnection, SQL_CONVERT_BIGINT );
    mapInfo["SQL_CONVERT_BINARY"] = getInfoData( pConnection, SQL_CONVERT_BINARY );
    mapInfo["SQL_CONVERT_BIT"] = getInfoData( pConnection, SQL_CONVERT_BIT );
    mapInfo["SQL_CONVERT_CHAR"] = getInfoData( pConnection, SQL_CONVERT_CHAR );
    mapInfo["SQL_CONVERT_GUID"] = getInfoData( pConnection, SQL_CONVERT_GUID );
    mapInfo["SQL_CONVERT_DATE"] = getInfoData( pConnection, SQL_CONVERT_DATE );
    mapInfo["SQL_CONVERT_DECIMAL"] = getInfoData( pConnection, SQL_CONVERT_DECIMAL );
    mapInfo["SQL_CONVERT_DOUBLE"] = getInfoData( pConnection, SQL_CONVERT_DOUBLE );
    mapInfo["SQL_CONVERT_FLOAT"] = getInfoData( pConnection, SQL_CONVERT_FLOAT );
    mapInfo["SQL_CONVERT_INTEGER"] = getInfoData( pConnection, SQL_CONVERT_INTEGER );
    mapInfo["SQL_CONVERT_INTERVAL_YEAR_MONTH"] = getInfoData( pConnection, SQL_CONVERT_INTERVAL_YEAR_MONTH );
    mapInfo["SQL_CONVERT_INTERVAL_DAY_TIME"] = getInfoData( pConnection, SQL_CONVERT_INTERVAL_DAY_TIME );
    mapInfo["SQL_CONVERT_LONGVARBINARY"] = getInfoData( pConnection, SQL_CONVERT_LONGVARBINARY );
    mapInfo["SQL_CONVERT_LONGVARCHAR"] = getInfoData( pConnection, SQL_CONVERT_LONGVARCHAR );
    mapInfo["SQL_CONVERT_NUMERIC"] = getInfoData( pConnection, SQL_CONVERT_NUMERIC );
    mapInfo["SQL_CONVERT_REAL"] = getInfoData( pConnection, SQL_CONVERT_REAL );
    mapInfo["SQL_CONVERT_SMALLINT"] = getInfoData( pConnection, SQL_CONVERT_SMALLINT );
    mapInfo["SQL_CONVERT_TIME"] = getInfoData( pConnection, SQL_CONVERT_TIME );
    mapInfo["SQL_CONVERT_TIMESTAMP"] = getInfoData( pConnection, SQL_CONVERT_TIMESTAMP );
    mapInfo["SQL_CONVERT_TINYINT"] = getInfoData( pConnection, SQL_CONVERT_TINYINT );
    mapInfo["SQL_CONVERT_VARBINARY"] = getInfoData( pConnection, SQL_CONVERT_VARBINARY );
    mapInfo["SQL_CONVERT_VARCHAR"] = getInfoData( pConnection, SQL_CONVERT_VARCHAR );
    mapInfo["SQL_CATALOG_LOCATION"] = getInfoData( pConnection, SQL_CATALOG_LOCATION );
    mapInfo["SQL_CATALOG_NAME"] = getInfoData( pConnection, SQL_CATALOG_NAME );
    mapInfo["SQL_CATALOG_NAME_SEPARATOR"] = getInfoData( pConnection, SQL_CATALOG_NAME_SEPARATOR );
    mapInfo["SQL_CATALOG_TERM"] = getInfoData( pConnection, SQL_CATALOG_TERM );
    mapInfo["SQL_COLUMN_ALIAS"] = getInfoData( pConnection, SQL_COLUMN_ALIAS );
    mapInfo["SQL_CREATE_ASSERTION"] = getInfoData( pConnection, SQL_CREATE_ASSERTION );
    mapInfo["SQL_CREATE_CHARACTER_SET"] = getInfoData( pConnection, SQL_CREATE_CHARACTER_SET );
    mapInfo["SQL_CREATE_COLLATION"] = getInfoData( pConnection, SQL_CREATE_COLLATION );
    mapInfo["SQL_CREATE_DOMAIN"] = getInfoData( pConnection, SQL_CREATE_DOMAIN );
    mapInfo["SQL_CREATE_SCHEMA"] = getInfoData( pConnection, SQL_CREATE_SCHEMA );
    mapInfo["SQL_CREATE_TABLE"] = getInfoData( pConnection, SQL_CREATE_TABLE );
    mapInfo["SQL_CREATE_TRANSLATION"] = getInfoData( pConnection, SQL_CREATE_TRANSLATION );
    mapInfo["SQL_CREATE_VIEW"] = getInfoData( pConnection, SQL_CREATE_VIEW );
    mapInfo["SQL_DDL_INDEX"] = getInfoData( pConnection, SQL_DDL_INDEX );
    mapInfo["SQL_DROP_ASSERTION"] = getInfoData( pConnection, SQL_DROP_ASSERTION );
    mapInfo["SQL_DROP_CHARACTER_SET"] = getInfoData( pConnection, SQL_DROP_CHARACTER_SET );
    mapInfo["SQL_DROP_COLLATION"] = getInfoData( pConnection, SQL_DROP_COLLATION );
    mapInfo["SQL_DROP_DOMAIN"] = getInfoData( pConnection, SQL_DROP_DOMAIN );
    mapInfo["SQL_DROP_SCHEMA"] = getInfoData( pConnection, SQL_DROP_SCHEMA );
    mapInfo["SQL_DROP_TABLE"] = getInfoData( pConnection, SQL_DROP_TABLE );
    mapInfo["SQL_DROP_TRANSLATION"] = getInfoData( pConnection, SQL_DROP_TRANSLATION );
    mapInfo["SQL_DROP_VIEW"] = getInfoData( pConnection, SQL_DROP_VIEW );
    mapInfo["SQL_IDENTIFIER_CASE"] = getInfoData( pConnection, SQL_IDENTIFIER_CASE );
    mapInfo["SQL_IDENTIFIER_QUOTE_CHAR"] = getInfoData( pConnection, SQL_IDENTIFIER_QUOTE_CHAR );
    mapInfo["SQL_INDEX_KEYWORDS"] = getInfoData( pConnection, SQL_INDEX_KEYWORDS );
    mapInfo["SQL_INTEGRITY"] = getInfoData( pConnection, SQL_INTEGRITY );
    mapInfo["SQL_KEYWORDS"] = getInfoData( pConnection, SQL_KEYWORDS );
    mapInfo["SQL_MAX_CATALOG_NAME_LEN"] = getInfoData( pConnection, SQL_MAX_CATALOG_NAME_LEN );
    mapInfo["SQL_MAX_COLUMN_NAME_LEN"] = getInfoData( pConnection, SQL_MAX_COLUMN_NAME_LEN );
    mapInfo["SQL_MAX_COLUMNS_IN_INDEX"] = getInfoData( pConnection, SQL_MAX_COLUMNS_IN_INDEX );
    mapInfo["SQL_MAX_COLUMNS_IN_TABLE"] = getInfoData( pConnection, SQL_MAX_COLUMNS_IN_TABLE );
    mapInfo["SQL_MAX_IDENTIFIER_LEN"] = getInfoData( pConnection, SQL_MAX_IDENTIFIER_LEN );
    mapInfo["SQL_MAX_INDEX_SIZE"] = getInfoData( pConnection, SQL_MAX_INDEX_SIZE );
    mapInfo["SQL_MAX_PROCEDURE_NAME_LEN"] = getInfoData( pConnection, SQL_MAX_PROCEDURE_NAME_LEN );
    mapInfo["SQL_MAX_ROW_SIZE"] = getInfoData( pConnection, SQL_MAX_ROW_SIZE );
    mapInfo["SQL_MAX_ROW_SIZE_INCLUDES_LONG"] = getInfoData( pConnection, SQL_MAX_ROW_SIZE_INCLUDES_LONG );
    mapInfo["SQL_MAX_SCHEMA_NAME_LEN"] = getInfoData( pConnection, SQL_MAX_SCHEMA_NAME_LEN );
    mapInfo["SQL_MAX_TABLE_NAME_LEN"] = getInfoData( pConnection, SQL_MAX_TABLE_NAME_LEN );
    mapInfo["SQL_NON_NULLABLE_COLUMNS"] = getInfoData( pConnection, SQL_NON_NULLABLE_COLUMNS );
    mapInfo["SQL_PROCEDURE_TERM"] = getInfoData( pConnection, SQL_PROCEDURE_TERM );
    mapInfo["SQL_PROCEDURES"] = getInfoData( pConnection, SQL_PROCEDURES );
    mapInfo["SQL_QUOTED_IDENTIFIER_CASE"] = getInfoData( pConnection, SQL_QUOTED_IDENTIFIER_CASE );
    mapInfo["SQL_SCHEMA_TERM"] = getInfoData( pConnection, SQL_SCHEMA_TERM );
    mapInfo["SQL_SPECIAL_CHARACTERS"] = getInfoData( pConnection, SQL_SPECIAL_CHARACTERS );
    mapInfo["SQL_SQL_CONFORMANCE"] = getInfoData( pConnection, SQL_SQL_CONFORMANCE );
    mapInfo["SQL_SQL92_DATETIME_FUNCTIONS"] = getInfoData( pConnection, SQL_SQL92_DATETIME_FUNCTIONS );
    mapInfo["SQL_SQL92_RELATIONAL_JOIN_OPERATORS"] = getInfoData( pConnection, SQL_SQL92_RELATIONAL_JOIN_OPERATORS );
    mapInfo["SQL_TABLE_TERM"] = getInfoData( pConnection, SQL_TABLE_TERM );

    return mapInfo;
}

DATAInfo *DATAProfile::getInfoData( DATAConnection *pConnection, SQLUSMALLINT nInfoType, SQLRETURN *pnReturn )
{
    SQLRETURN nReturn;
    if ( !pnReturn ) pnReturn = &nReturn;

    DATAInfo *pInfoParent = new DATAInfo();    // this
    pInfoParent->nInfo    = nInfoType;
    pInfoParent->vValue   = pConnection->getInfo( nInfoType, pnReturn );

    DATAInfo *pInfoChild;

    switch ( nInfoType )
    {
        case SQL_ACCESSIBLE_PROCEDURES:
            pInfoParent->stringInfo = "SQL_ACCESSIBLE_PROCEDURES";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_ACCESSIBLE_TABLES:
            pInfoParent->stringInfo = "SQL_ACCESSIBLE_TABLES";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_CATALOG_NAME:
            pInfoParent->stringInfo = "SQL_CATALOG_NAME";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_CATALOG_NAME_SEPARATOR:
            pInfoParent->stringInfo = "SQL_CATALOG_NAME_SEPARATOR";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_CATALOG_TERM:
            pInfoParent->stringInfo = "SQL_CATALOG_TERM";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_COLLATION_SEQ:
            pInfoParent->stringInfo = "SQL_COLLATION_SEQ";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_COLUMN_ALIAS:
            pInfoParent->stringInfo = "SQL_COLUMN_ALIAS";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_DATA_SOURCE_NAME:
            pInfoParent->stringInfo = "SQL_DATA_SOURCE_NAME";
            pInfoParent->stringDescription = QObject::tr( "data source name used during connection" );
            break;
        case SQL_DATA_SOURCE_READ_ONLY:
            pInfoParent->stringInfo = "SQL_DATA_SOURCE_READ_ONLY";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_DATABASE_NAME:
            pInfoParent->stringInfo = "SQL_DATABASE_NAME";
            pInfoParent->stringDescription = QObject::tr( "name of the database" );
            break;
        case SQL_DBMS_NAME:
            pInfoParent->stringInfo = "SQL_DBMS_NAME";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_DBMS_VER:
            pInfoParent->stringInfo = "SQL_DBMS_VER";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_DESCRIBE_PARAMETER:
            pInfoParent->stringInfo = "SQL_DESCRIBE_PARAMETER";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_DM_VER:
            pInfoParent->stringInfo = "SQL_DM_VER";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_DRIVER_NAME:
            pInfoParent->stringInfo = "SQL_DRIVER_NAME";
            pInfoParent->stringDescription = QObject::tr( "name of the ODBC driver" );
            break;
        case SQL_DRIVER_ODBC_VER:
            pInfoParent->stringInfo = "SQL_DRIVER_ODBC_VER";
            pInfoParent->stringDescription = QObject::tr( "drivers ODBC compliance level" );
            break;
        case SQL_DRIVER_VER:
            pInfoParent->stringInfo = "SQL_DRIVER_VER";
            pInfoParent->stringDescription = QObject::tr( "version of driver" );
            break;
        case SQL_EXPRESSIONS_IN_ORDERBY:
            pInfoParent->stringInfo = "SQL_EXPRESSIONS_IN_ORDERBY";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_IDENTIFIER_QUOTE_CHAR:
            pInfoParent->stringInfo = "SQL_IDENTIFIER_QUOTE_CHAR";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_INTEGRITY:
            pInfoParent->stringInfo = "SQL_INTEGRITY";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_KEYWORDS:
            pInfoParent->stringInfo = "SQL_KEYWORDS";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_LIKE_ESCAPE_CLAUSE:
            pInfoParent->stringInfo = "SQL_LIKE_ESCAPE_CLAUSE";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_MAX_ROW_SIZE_INCLUDES_LONG:
            pInfoParent->stringInfo = "SQL_MAX_ROW_SIZE_INCLUDES_LONG";
            pInfoParent->stringDescription = QObject::tr( "maximum row size returned for the SQL_MAX_ROW_SIZE information type includes the length of all SQL_LONGVARCHAR and SQL_LONGVARBINARY columns in the row" );
            break;
        case SQL_MULT_RESULT_SETS:
            pInfoParent->stringInfo = "SQL_MULT_RESULT_SETS";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_MULTIPLE_ACTIVE_TXN:
            pInfoParent->stringInfo = "SQL_MULTIPLE_ACTIVE_TXN";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_NEED_LONG_DATA_LEN:
            pInfoParent->stringInfo = "SQL_NEED_LONG_DATA_LEN";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_ODBC_VER:
            pInfoParent->stringInfo = "SQL_ODBC_VER";
            pInfoParent->stringDescription = QObject::tr( "ODBC sub-system version" );
            break;
        case SQL_ORDER_BY_COLUMNS_IN_SELECT:
            pInfoParent->stringInfo = "SQL_ORDER_BY_COLUMNS_IN_SELECT";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_PROCEDURE_TERM:
            pInfoParent->stringInfo = "SQL_PROCEDURE_TERM";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_PROCEDURES:
            pInfoParent->stringInfo = "SQL_PROCEDURES";
            pInfoParent->stringDescription = QObject::tr( "DBMS supports procedures" );
            break;
        case SQL_ROW_UPDATES:
            pInfoParent->stringInfo = "SQL_ROW_UPDATES";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_SCHEMA_TERM:
            pInfoParent->stringInfo = "SQL_SCHEMA_TERM";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_SEARCH_PATTERN_ESCAPE:
            pInfoParent->stringInfo = "SQL_SEARCH_PATTERN_ESCAPE";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_SERVER_NAME:
            pInfoParent->stringInfo = "SQL_SERVER_NAME";
            pInfoParent->stringDescription = QObject::tr( "name of the server" );
            break;
        case SQL_SPECIAL_CHARACTERS:
            pInfoParent->stringInfo = "SQL_SPECIAL_CHARACTERS";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_TABLE_TERM:
            pInfoParent->stringInfo = "SQL_TABLE_TERM";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_USER_NAME:
            pInfoParent->stringInfo = "SQL_USER_NAME";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_XOPEN_CLI_YEAR:
            pInfoParent->stringInfo = "SQL_XOPEN_CLI_YEAR";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_ACTIVE_ENVIRONMENTS:
            pInfoParent->stringInfo = "SQL_ACTIVE_ENVIRONMENTS";
            pInfoParent->stringDescription = QObject::tr( "maximum number of active environments that the driver can support" );
            break;
        case SQL_MAX_CATALOG_NAME_LEN:
            pInfoParent->stringInfo = "SQL_MAX_CATALOG_NAME_LEN";
            pInfoParent->stringDescription = QObject::tr( "maximum length of a catalog name in the data source" );
            break;
        case SQL_MAX_COLUMN_NAME_LEN:
            pInfoParent->stringInfo = "SQL_MAX_COLUMN_NAME_LEN";
            pInfoParent->stringDescription = QObject::tr( "maximum length of a column name in the data source" );
            break;
        case SQL_MAX_COLUMNS_IN_GROUP_BY:
            pInfoParent->stringInfo = "SQL_MAX_COLUMNS_IN_GROUP_BY";
            pInfoParent->stringDescription = QObject::tr( "maximum number of columns allowed in a GROUP BY clause" );
            break;
        case SQL_MAX_COLUMNS_IN_INDEX:
            pInfoParent->stringInfo = "SQL_MAX_COLUMNS_IN_INDEX";
            pInfoParent->stringDescription = QObject::tr( "maximum number of columns allowed in an index" );
            break;
        case SQL_MAX_COLUMNS_IN_ORDER_BY:
            pInfoParent->stringInfo = "SQL_MAX_COLUMNS_IN_ORDER_BY";
            pInfoParent->stringDescription = QObject::tr( "maximum number of columns allowed in an ORDER BY clause" );
            break;
        case SQL_MAX_COLUMNS_IN_SELECT:
            pInfoParent->stringInfo = "SQL_MAX_COLUMNS_IN_SELECT";
            pInfoParent->stringDescription = QObject::tr( "maximum number of columns allowed in a select list" );
            break;
        case SQL_MAX_COLUMNS_IN_TABLE:
            pInfoParent->stringInfo = "SQL_MAX_COLUMNS_IN_TABLE";
            pInfoParent->stringDescription = QObject::tr( "maximum number of columns allowed in a table" );
            break;
        case SQL_MAX_CONCURRENT_ACTIVITIES:
            pInfoParent->stringInfo = "SQL_MAX_CONCURRENT_ACTIVITIES";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_MAX_CURSOR_NAME_LEN:
            pInfoParent->stringInfo = "SQL_MAX_CURSOR_NAME_LEN";
            pInfoParent->stringDescription = QObject::tr( "maximum length of a cursor name in the data source" );
            break;
        case SQL_MAX_DRIVER_CONNECTIONS:
            pInfoParent->stringInfo = "SQL_MAX_DRIVER_CONNECTIONS";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_MAX_IDENTIFIER_LEN:
            pInfoParent->stringInfo = "SQL_MAX_IDENTIFIER_LEN";
            pInfoParent->stringDescription = QObject::tr( "maximum size in characters that the data source supports for user-defined names" );
            break;
        case SQL_MAX_PROCEDURE_NAME_LEN:
            pInfoParent->stringInfo = "SQL_MAX_PROCEDURE_NAME_LEN";
            pInfoParent->stringDescription = QObject::tr( "maximum length of a procedure name in the data source" );
            break;
        case SQL_MAX_SCHEMA_NAME_LEN:
            pInfoParent->stringInfo = "SQL_MAX_SCHEMA_NAME_LEN";
            pInfoParent->stringDescription = QObject::tr( "maximum length of a schema name in the data source" );
            break;
        case SQL_MAX_TABLE_NAME_LEN:
            pInfoParent->stringInfo = "SQL_MAX_TABLE_NAME_LEN";
            pInfoParent->stringDescription = QObject::tr( "maximum length of a table name in the data source" );
            break;
        case SQL_MAX_TABLES_IN_SELECT:
            pInfoParent->stringInfo = "SQL_MAX_TABLES_IN_SELECT";
            pInfoParent->stringDescription = QObject::tr( "maximum number of tables allowed in the FROM clause of a SELECT statement" );
            break;
        case SQL_MAX_USER_NAME_LEN:
            pInfoParent->stringInfo = "SQL_MAX_USER_NAME_LEN";
            pInfoParent->stringDescription = QObject::tr( "maximum length of a user name in the data source" );
            break;
        case SQL_DRIVER_HDBC:
            pInfoParent->stringInfo = "SQL_DRIVER_HDBC";
            pInfoParent->stringDescription = QObject::tr( "connection handle" );
            break;
        case SQL_DRIVER_HENV:
            pInfoParent->stringInfo = "SQL_DRIVER_HENV";
            pInfoParent->stringDescription = QObject::tr( "environment handle" );
            break;
        case SQL_DRIVER_HDESC:
            pInfoParent->stringInfo = "SQL_DRIVER_HDESC";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_DRIVER_HLIB:
            pInfoParent->stringInfo = "SQL_DRIVER_HLIB";
            pInfoParent->stringDescription = QObject::tr( "the load library handle returned to the Driver Manager when it loaded the driver" );
            break;
        case SQL_DRIVER_HSTMT:
            pInfoParent->stringInfo = "SQL_DRIVER_HSTMT";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;

            // SQLUINTEGER
            // SQL_ASYNC_DBC_FUNCTIONS - conflict?
            // SQL_ASYNC_NOTIFICATION  - conflict?

        case SQL_MAX_ASYNC_CONCURRENT_STATEMENTS:
            pInfoParent->stringInfo = "SQL_MAX_ASYNC_CONCURRENT_STATEMENTS";
            pInfoParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_MAX_BINARY_LITERAL_LEN:
            pInfoParent->stringInfo = "SQL_MAX_BINARY_LITERAL_LEN";
            pInfoParent->stringDescription = QObject::tr( "max length of binary literal in an SQL statement" );
            break;
        case SQL_MAX_CHAR_LITERAL_LEN:
            pInfoParent->stringInfo = "SQL_MAX_CHAR_LITERAL_LEN";
            pInfoParent->stringDescription = QObject::tr( "maximum length of a character literal in an SQL statement" );
            break;
        case SQL_MAX_INDEX_SIZE:
            pInfoParent->stringInfo = "SQL_MAX_INDEX_SIZE";
            pInfoParent->stringDescription = QObject::tr( "maximum number of bytes allowed in the combined fields of an index" );
            break;
        case SQL_MAX_ROW_SIZE:
            pInfoParent->stringInfo = "SQL_MAX_ROW_SIZE";
            pInfoParent->stringDescription = QObject::tr( "maximum length of a single row in a table" );
            break;
        case SQL_MAX_STATEMENT_LEN:
            pInfoParent->stringInfo = "SQL_MAX_STATEMENT_LEN";
            pInfoParent->stringDescription = QObject::tr( "maximum length (number of characters, including white space) of an SQL statement" );
            break;
            // SINGULAR VALUE
            // - list all possible values and flag 1 of them (if any) with 'Y'
        case SQL_ASYNC_MODE:
            pInfoParent->stringInfo = "SQL_ASYNC_MODE";
            pInfoParent->stringDescription = QObject::tr( "the level of asynchronous support in the driver" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AM_CONNECTION;
            pInfoChild->stringInfo = "SQL_AM_CONNECTION";
            pInfoChild->stringDescription = QObject::tr( "Connection level asynchronous execution is supportepInfoChild-> Either all statement handles associated with a given connection handle are in asynchronous mode or all are in synchronous mode. A statement handle on a connection cannot be in asynchronous mode while another statement handle on the same connection is in synchronous mode, and vice versa." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_AM_CONNECTION ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AM_STATEMENT;
            pInfoChild->stringInfo = "SQL_AM_STATEMENT";
            pInfoChild->stringDescription = QObject::tr( "Statement level asynchronous execution is supportepInfoChild-> Some statement handles associated with a connection handle can be in asynchronous mode, while other statement handles on the same connection are in synchronous mode." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_AM_STATEMENT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AM_NONE;
            pInfoChild->stringInfo = "SQL_AM_NONE";
            pInfoChild->stringDescription = QObject::tr( "Asynchronous mode is not supportepInfoChild->" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_AM_NONE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_CATALOG_LOCATION:
            pInfoParent->stringInfo = "SQL_CATALOG_LOCATION";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CL_START;
            pInfoChild->stringInfo = "SQL_CL_START";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_CL_START ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CL_END;
            pInfoChild->stringInfo = "SQL_CL_END";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_CL_END ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_CONCAT_NULL_BEHAVIOR:
            pInfoParent->stringInfo = "SQL_CONCAT_NULL_BEHAVIOR";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CB_NULL;
            pInfoChild->stringInfo = "SQL_CB_NULL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_CB_NULL ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CB_NON_NULL;
            pInfoChild->stringInfo = "SQL_CB_NON_NULL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_CB_NON_NULL ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_CORRELATION_NAME:
            pInfoParent->stringInfo = "SQL_CORRELATION_NAME";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CN_NONE;
            pInfoChild->stringInfo = "SQL_CN_NONE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_CN_NONE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CN_DIFFERENT;
            pInfoChild->stringInfo = "SQL_CN_DIFFERENT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_CN_DIFFERENT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CN_ANY;
            pInfoChild->stringInfo = "SQL_CN_ANY";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_CN_ANY ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_CURSOR_COMMIT_BEHAVIOR:
            pInfoParent->stringInfo = "SQL_CURSOR_COMMIT_BEHAVIOR";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CB_DELETE;
            pInfoChild->stringInfo = "SQL_CB_DELETE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_CB_DELETE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CB_CLOSE;
            pInfoChild->stringInfo = "SQL_CB_CLOSE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_CB_CLOSE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CB_PRESERVE;
            pInfoChild->stringInfo = "SQL_CB_PRESERVE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_CB_PRESERVE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_CURSOR_ROLLBACK_BEHAVIOR:
            pInfoParent->stringInfo = "SQL_CURSOR_ROLLBACK_BEHAVIOR";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CB_DELETE;
            pInfoChild->stringInfo = "SQL_CB_DELETE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_CB_DELETE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CB_CLOSE;
            pInfoChild->stringInfo = "SQL_CB_CLOSE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_CB_CLOSE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CB_PRESERVE;
            pInfoChild->stringInfo = "SQL_CB_PRESERVE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_CB_PRESERVE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_CURSOR_SENSITIVITY:
            pInfoParent->stringInfo = "SQL_CURSOR_SENSITIVITY";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_INSENSITIVE;
            pInfoChild->stringInfo = "SQL_INSENSITIVE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_INSENSITIVE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_UNSPECIFIED;
            pInfoChild->stringInfo = "SQL_UNSPECIFIED";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_UNSPECIFIED ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_SENSITIVE;
            pInfoChild->stringInfo = "SQL_SENSITIVE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_SENSITIVE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_DEFAULT_TXN_ISOLATION:
            pInfoParent->stringInfo = "SQL_DEFAULT_TXN_ISOLATION";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_TXN_READ_UNCOMMITTED;
            pInfoChild->stringInfo = "SQL_TXN_READ_UNCOMMITTED";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_TXN_READ_UNCOMMITTED ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_TXN_READ_COMMITTED;
            pInfoChild->stringInfo = "SQL_TXN_READ_COMMITTED";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_TXN_READ_COMMITTED ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_TXN_REPEATABLE_READ;
            pInfoChild->stringInfo = "SQL_TXN_REPEATABLE_READ";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_TXN_REPEATABLE_READ ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_TXN_SERIALIZABLE;
            pInfoChild->stringInfo = "SQL_TXN_SERIALIZABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_TXN_SERIALIZABLE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
#ifndef Q_OS_WIN
        case SQL_DRIVER_AWARE_POOLING_SUPPORTED:
            pInfoParent->stringInfo = "SQL_DRIVER_AWARE_POOLING_SUPPORTED";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_DRIVER_AWARE_POOLING_CAPABLE;
            pInfoChild->stringInfo = "SQL_DRIVER_AWARE_POOLING_CAPABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_DRIVER_AWARE_POOLING_CAPABLE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_DRIVER_AWARE_POOLING_NOT_CAPABLE;
            pInfoChild->stringInfo = "SQL_DRIVER_AWARE_POOLING_NOT_CAPABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_DRIVER_AWARE_POOLING_NOT_CAPABLE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
#endif
        case SQL_FILE_USAGE:
            pInfoParent->stringInfo = "SQL_FILE_USAGE";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_FILE_NOT_SUPPORTED;
            pInfoChild->stringInfo = "SQL_FILE_NOT_SUPPORTED";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_FILE_NOT_SUPPORTED ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_FILE_TABLE;
            pInfoChild->stringInfo = "SQL_FILE_TABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_FILE_TABLE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_FILE_CATALOG;
            pInfoChild->stringInfo = "SQL_FILE_CATALOG";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_FILE_CATALOG ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_GROUP_BY:
            pInfoParent->stringInfo = "SQL_GROUP_BY";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_GB_COLLATE;
            pInfoChild->stringInfo = "SQL_GB_COLLATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_GB_COLLATE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_GB_NOT_SUPPORTED;
            pInfoChild->stringInfo = "SQL_GB_NOT_SUPPORTED";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_GB_NOT_SUPPORTED ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_GB_GROUP_BY_EQUALS_SELECT;
            pInfoChild->stringInfo = "SQL_GB_GROUP_BY_EQUALS_SELECT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_GB_GROUP_BY_EQUALS_SELECT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_GB_GROUP_BY_CONTAINS_SELECT;
            pInfoChild->stringInfo = "SQL_GB_GROUP_BY_CONTAINS_SELECT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_GB_GROUP_BY_CONTAINS_SELECT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_GB_NO_RELATION;
            pInfoChild->stringInfo = "SQL_GB_NO_RELATION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_GB_NO_RELATION ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_IDENTIFIER_CASE:
            pInfoParent->stringInfo = "SQL_IDENTIFIER_CASE";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_IC_UPPER;
            pInfoChild->stringInfo = "SQL_IC_UPPER";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_IC_UPPER ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_IC_LOWER;
            pInfoChild->stringInfo = "SQL_IC_LOWER";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_IC_LOWER ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_IC_SENSITIVE;
            pInfoChild->stringInfo = "SQL_IC_SENSITIVE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_IC_SENSITIVE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_IC_MIXED;
            pInfoChild->stringInfo = "SQL_IC_MIXED";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_IC_MIXED ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_NON_NULLABLE_COLUMNS:
            pInfoParent->stringInfo = "SQL_NON_NULLABLE_COLUMNS";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_NNC_NULL;
            pInfoChild->stringInfo = "SQL_NNC_NULL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_NNC_NULL ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_NNC_NON_NULL;
            pInfoChild->stringInfo = "SQL_NNC_NON_NULL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_NNC_NON_NULL ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_NULL_COLLATION:
            pInfoParent->stringInfo = "SQL_NULL_COLLATION";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_NC_END;
            pInfoChild->stringInfo = "SQL_NC_END";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_NC_END ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_NC_HIGH;
            pInfoChild->stringInfo = "SQL_NC_HIGH";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_NC_HIGH ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_NC_LOW;
            pInfoChild->stringInfo = "SQL_NC_LOW";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_NC_LOW ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_NC_START;
            pInfoChild->stringInfo = "SQL_NC_START";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_NC_START ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_ODBC_INTERFACE_CONFORMANCE:
            pInfoParent->stringInfo = "SQL_ODBC_INTERFACE_CONFORMANCE";
            pInfoParent->stringDescription = QObject::tr( "An SQLUINTEGER value indicating the level of the ODBC 3.x interface that the driver conforms to." );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_OIC_CORE;
            pInfoChild->stringInfo = "SQL_OIC_CORE";
            pInfoChild->stringDescription = QObject::tr( "The minimum level that all ODBC drivers are expected to conform to. This level includes basic interface elements such as connection functions, functions for preparing and executing an SQL statement, basic result set metadata functions, basic catalog functions, and so on." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_OIC_CORE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_OIC_LEVEL1;
            pInfoChild->stringInfo = "SQL_OIC_LEVEL1";
            pInfoChild->stringDescription = QObject::tr( "A level including the core standards compliance level functionality, plus scrollable cursors, bookmarks, positioned updates and deletes, and so on." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_OIC_LEVEL1 ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_OIC_LEVEL2;
            pInfoChild->stringInfo = "SQL_OIC_LEVEL2";
            pInfoChild->stringDescription = QObject::tr( "A level including level 1 standards compliance level functionality, plus advanced features such as sensitive cursors; update, delete, and refresh by bookmarks; stored procedure support; catalog functions for primary and foreign keys; multicatalog support; and so on." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_OIC_LEVEL2 ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_QUOTED_IDENTIFIER_CASE:
            pInfoParent->stringInfo = "SQL_QUOTED_IDENTIFIER_CASE";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_IC_UPPER;
            pInfoChild->stringInfo = "SQL_IC_UPPER";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_IC_UPPER ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_IC_LOWER;
            pInfoChild->stringInfo = "SQL_IC_LOWER";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_IC_LOWER ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_IC_SENSITIVE;
            pInfoChild->stringInfo = "SQL_IC_SENSITIVE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_IC_SENSITIVE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_IC_MIXED;
            pInfoChild->stringInfo = "SQL_IC_MIXED";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_IC_MIXED ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_TXN_CAPABLE:
            pInfoParent->stringInfo = "SQL_TXN_CAPABLE";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_TC_NONE;
            pInfoChild->stringInfo = "SQL_TC_NONE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_TC_NONE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_TC_DML;
            pInfoChild->stringInfo = "SQL_TC_DML";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_TC_DML ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_TC_DDL_COMMIT;
            pInfoChild->stringInfo = "SQL_TC_DDL_COMMIT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_TC_DDL_COMMIT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_TC_DDL_IGNORE;
            pInfoChild->stringInfo = "SQL_TC_DDL_IGNORE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_TC_DDL_IGNORE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_TC_ALL;
            pInfoChild->stringInfo = "SQL_TC_ALL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() == SQL_TC_ALL ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
            // BITMASKS
            // - list all possible values and flag 0-n of them with 'Y'
        case SQL_AGGREGATE_FUNCTIONS:
            pInfoParent->stringInfo = "SQL_AGGREGATE_FUNCTIONS";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AF_ALL;
            pInfoChild->stringInfo = "SQL_AF_ALL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AF_ALL ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AF_AVG;
            pInfoChild->stringInfo = "SQL_AF_AVG";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AF_AVG ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AF_COUNT;
            pInfoChild->stringInfo = "SQL_AF_COUNT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AF_COUNT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AF_DISTINCT;
            pInfoChild->stringInfo = "SQL_AF_DISTINCT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AF_DISTINCT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AF_MIN;
            pInfoChild->stringInfo = "SQL_AF_MIN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AF_MIN ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AF_SUM;
            pInfoChild->stringInfo = "SQL_AF_SUM";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AF_SUM ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_ALTER_DOMAIN:
            pInfoParent->stringInfo = "SQL_ALTER_DOMAIN";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AD_ADD_DOMAIN_CONSTRAINT;
            pInfoChild->stringInfo = "SQL_AD_ADD_DOMAIN_CONSTRAINT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AD_ADD_DOMAIN_CONSTRAINT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AD_ADD_DOMAIN_DEFAULT;
            pInfoChild->stringInfo = "SQL_AD_ADD_DOMAIN_DEFAULT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AD_ADD_DOMAIN_DEFAULT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AD_CONSTRAINT_NAME_DEFINITION;
            pInfoChild->stringInfo = "SQL_AD_CONSTRAINT_NAME_DEFINITION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AD_CONSTRAINT_NAME_DEFINITION ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AD_DROP_DOMAIN_CONSTRAINT;
            pInfoChild->stringInfo = "SQL_AD_DROP_DOMAIN_CONSTRAINT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AD_DROP_DOMAIN_CONSTRAINT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AD_DROP_DOMAIN_DEFAULT;
            pInfoChild->stringInfo = "SQL_AD_DROP_DOMAIN_DEFAULT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AD_DROP_DOMAIN_DEFAULT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AD_ADD_CONSTRAINT_DEFERRABLE;
            pInfoChild->stringInfo = "SQL_AD_ADD_CONSTRAINT_DEFERRABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AD_ADD_CONSTRAINT_DEFERRABLE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AD_ADD_CONSTRAINT_NON_DEFERRABLE;
            pInfoChild->stringInfo = "SQL_AD_ADD_CONSTRAINT_NON_DEFERRABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AD_ADD_CONSTRAINT_NON_DEFERRABLE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AD_ADD_CONSTRAINT_INITIALLY_DEFERRED;
            pInfoChild->stringInfo = "SQL_AD_ADD_CONSTRAINT_INITIALLY_DEFERRED";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AD_ADD_CONSTRAINT_INITIALLY_DEFERRED ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AD_ADD_CONSTRAINT_INITIALLY_IMMEDIATE;
            pInfoChild->stringInfo = "SQL_AD_ADD_CONSTRAINT_INITIALLY_IMMEDIATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AD_ADD_CONSTRAINT_INITIALLY_IMMEDIATE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_ALTER_TABLE:
            pInfoParent->stringInfo = "SQL_ALTER_TABLE";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AT_ADD_COLUMN_COLLATION;
            pInfoChild->stringInfo = "SQL_AT_ADD_COLUMN_COLLATION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AT_ADD_COLUMN_COLLATION ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AT_ADD_COLUMN_DEFAULT;
            pInfoChild->stringInfo = "SQL_AT_ADD_COLUMN_DEFAULT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AT_ADD_COLUMN_DEFAULT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AT_ADD_COLUMN_SINGLE;
            pInfoChild->stringInfo = "SQL_AT_ADD_COLUMN_SINGLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AT_ADD_COLUMN_SINGLE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AT_ADD_CONSTRAINT;
            pInfoChild->stringInfo = "SQL_AT_ADD_CONSTRAINT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AT_ADD_CONSTRAINT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AT_ADD_TABLE_CONSTRAINT;
            pInfoChild->stringInfo = "SQL_AT_ADD_TABLE_CONSTRAINT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AT_ADD_TABLE_CONSTRAINT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AT_CONSTRAINT_NAME_DEFINITION;
            pInfoChild->stringInfo = "SQL_AT_CONSTRAINT_NAME_DEFINITION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AT_CONSTRAINT_NAME_DEFINITION ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AT_DROP_COLUMN_CASCADE;
            pInfoChild->stringInfo = "SQL_AT_DROP_COLUMN_CASCADE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AT_DROP_COLUMN_CASCADE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AT_DROP_COLUMN_DEFAULT;
            pInfoChild->stringInfo = "SQL_AT_DROP_COLUMN_DEFAULT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AT_DROP_COLUMN_DEFAULT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AT_DROP_COLUMN_RESTRICT;
            pInfoChild->stringInfo = "SQL_AT_DROP_COLUMN_RESTRICT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AT_DROP_COLUMN_RESTRICT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AT_DROP_TABLE_CONSTRAINT_CASCADE;
            pInfoChild->stringInfo = "SQL_AT_DROP_TABLE_CONSTRAINT_CASCADE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AT_DROP_TABLE_CONSTRAINT_CASCADE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AT_DROP_TABLE_CONSTRAINT_RESTRICT;
            pInfoChild->stringInfo = "SQL_AT_DROP_TABLE_CONSTRAINT_RESTRICT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AT_DROP_TABLE_CONSTRAINT_RESTRICT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AT_SET_COLUMN_DEFAULT;
            pInfoChild->stringInfo = "SQL_AT_SET_COLUMN_DEFAULT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AT_SET_COLUMN_DEFAULT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AT_CONSTRAINT_INITIALLY_DEFERRED;
            pInfoChild->stringInfo = "SQL_AT_CONSTRAINT_INITIALLY_DEFERRED";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AT_CONSTRAINT_INITIALLY_DEFERRED ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AT_CONSTRAINT_INITIALLY_IMMEDIATE;
            pInfoChild->stringInfo = "SQL_AT_CONSTRAINT_INITIALLY_IMMEDIATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AT_CONSTRAINT_INITIALLY_IMMEDIATE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AT_CONSTRAINT_DEFERRABLE;
            pInfoChild->stringInfo = "SQL_AT_CONSTRAINT_DEFERRABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AT_CONSTRAINT_DEFERRABLE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_AT_CONSTRAINT_NON_DEFERRABLE;
            pInfoChild->stringInfo = "SQL_AT_CONSTRAINT_NON_DEFERRABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_AT_CONSTRAINT_NON_DEFERRABLE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_BATCH_ROW_COUNT:
            pInfoParent->stringInfo = "SQL_BATCH_ROW_COUNT";
            pInfoParent->stringDescription = QObject::tr( "behavior of the driver with respect to the availability of row counts" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_BRC_ROLLED_UP;
            pInfoChild->stringInfo = "SQL_BRC_ROLLED_UP";
            pInfoChild->stringDescription = QObject::tr( "Row counts for consecutive INSERT, DELETE, or UPDATE statements are rolled up into one. If this bit is not set, then row counts are available for each individual statement." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_BRC_ROLLED_UP ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_BRC_PROCEDURES;
            pInfoChild->stringInfo = "SQL_BRC_PROCEDURES";
            pInfoChild->stringDescription = QObject::tr( "Row counts, if any, are available when a batch is executed in a stored procedure. If row counts are available, they can be rolled up or individually available, depending on the SQL_BRC_ROLLED_UP bit." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_BRC_PROCEDURES ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_BRC_EXPLICIT;
            pInfoChild->stringInfo = "SQL_BRC_EXPLICIT";
            pInfoChild->stringDescription = QObject::tr( "Row counts, if any, are available when a batch is executed directly by calling SQLExecute or SQLExecDirect. If row counts are available, they can be rolled up or individually available, depending on the SQL_BRC_ROLLED_UP bit." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_BRC_EXPLICIT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_BATCH_SUPPORT:
            pInfoParent->stringInfo = "SQL_BATCH_SUPPORT";
            pInfoParent->stringDescription = QObject::tr( "the driver's support for batches" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_BS_SELECT_EXPLICIT;
            pInfoChild->stringInfo = "SQL_BS_SELECT_EXPLICIT";
            pInfoChild->stringDescription = QObject::tr( "The driver supports explicit batches that can have result-set generating statements." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_BS_SELECT_EXPLICIT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_BS_ROW_COUNT_EXPLICIT;
            pInfoChild->stringInfo = "SQL_BS_ROW_COUNT_EXPLICIT";
            pInfoChild->stringDescription = QObject::tr( "The driver supports explicit batches that can have row-count generating statements." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_BS_ROW_COUNT_EXPLICIT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_BS_SELECT_PROC;
            pInfoChild->stringInfo = "SQL_BS_SELECT_PROC";
            pInfoChild->stringDescription = QObject::tr( "The driver supports explicit procedures that can have result-set generating statements." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_BS_SELECT_PROC ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_BS_ROW_COUNT_PROC;
            pInfoChild->stringInfo = "SQL_BS_ROW_COUNT_PROC";
            pInfoChild->stringDescription = QObject::tr( "The driver supports explicit procedures that can have row-count generating statements." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_BS_ROW_COUNT_PROC ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_BOOKMARK_PERSISTENCE:
            pInfoParent->stringInfo = "SQL_BOOKMARK_PERSISTENCE";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_BP_CLOSE;
            pInfoChild->stringInfo = "SQL_BP_CLOSE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_BP_CLOSE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_BP_DELETE;
            pInfoChild->stringInfo = "SQL_BP_DELETE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_BP_DELETE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_BP_DROP;
            pInfoChild->stringInfo = "SQL_BP_DROP";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_BP_DROP ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_BP_TRANSACTION;
            pInfoChild->stringInfo = "SQL_BP_TRANSACTION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_BP_TRANSACTION ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_BP_UPDATE;
            pInfoChild->stringInfo = "SQL_BP_UPDATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_BP_UPDATE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_BP_OTHER_HSTMT;
            pInfoChild->stringInfo = "SQL_BP_OTHER_HSTMT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_BP_OTHER_HSTMT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_CATALOG_USAGE:
            pInfoParent->stringInfo = "SQL_CATALOG_USAGE";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CU_DML_STATEMENTS;
            pInfoChild->stringInfo = "SQL_CU_DML_STATEMENTS";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CU_DML_STATEMENTS ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CU_PROCEDURE_INVOCATION;
            pInfoChild->stringInfo = "SQL_CU_PROCEDURE_INVOCATION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CU_PROCEDURE_INVOCATION ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CU_TABLE_DEFINITION;
            pInfoChild->stringInfo = "SQL_CU_TABLE_DEFINITION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CU_TABLE_DEFINITION ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CU_INDEX_DEFINITION;
            pInfoChild->stringInfo = "SQL_CU_INDEX_DEFINITION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CU_INDEX_DEFINITION ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_CONVERT_FUNCTIONS:
            pInfoParent->stringInfo = "SQL_CONVERT_FUNCTIONS";
            pInfoParent->stringDescription = QObject::tr( "the scalar conversion functions supported by the driver and associated data source" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_FN_CVT_CAST;
            pInfoChild->stringInfo = "SQL_FN_CVT_CAST";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_FN_CVT_CAST ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_FN_CVT_CONVERT;
            pInfoChild->stringInfo = "SQL_FN_CVT_CONVERT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_FN_CVT_CONVERT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_CREATE_ASSERTION:
            pInfoParent->stringInfo = "SQL_CREATE_ASSERTION";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CA_CREATE_ASSERTION;
            pInfoChild->stringInfo = "SQL_CA_CREATE_ASSERTION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA_CREATE_ASSERTION ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CA_CONSTRAINT_INITIALLY_DEFERRED;
            pInfoChild->stringInfo = "SQL_CA_CONSTRAINT_INITIALLY_DEFERRED";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA_CONSTRAINT_INITIALLY_DEFERRED ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CA_CONSTRAINT_INITIALLY_IMMEDIATE;
            pInfoChild->stringInfo = "SQL_CA_CONSTRAINT_INITIALLY_IMMEDIATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA_CONSTRAINT_INITIALLY_IMMEDIATE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CA_CONSTRAINT_DEFERRABLE;
            pInfoChild->stringInfo = "SQL_CA_CONSTRAINT_DEFERRABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA_CONSTRAINT_DEFERRABLE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CA_CONSTRAINT_NON_DEFERRABLE;
            pInfoChild->stringInfo = "SQL_CA_CONSTRAINT_NON_DEFERRABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA_CONSTRAINT_NON_DEFERRABLE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_CREATE_CHARACTER_SET:
            pInfoParent->stringInfo = "SQL_CREATE_CHARACTER_SET";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CCS_CREATE_CHARACTER_SET;
            pInfoChild->stringInfo = "SQL_CCS_CREATE_CHARACTER_SET";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CCS_CREATE_CHARACTER_SET ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CCS_COLLATE_CLAUSE;
            pInfoChild->stringInfo = "SQL_CCS_COLLATE_CLAUSE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CCS_COLLATE_CLAUSE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CCS_LIMITED_COLLATION;
            pInfoChild->stringInfo = "SQL_CCS_LIMITED_COLLATION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CCS_LIMITED_COLLATION ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_CREATE_COLLATION:
            pInfoParent->stringInfo = "SQL_CREATE_COLLATION";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CCOL_CREATE_COLLATION;
            pInfoChild->stringInfo = "SQL_CCOL_CREATE_COLLATION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CCOL_CREATE_COLLATION ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_CREATE_DOMAIN:
            pInfoParent->stringInfo = "SQL_CREATE_DOMAIN";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CDO_CREATE_DOMAIN;
            pInfoChild->stringInfo = "SQL_CDO_CREATE_DOMAIN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CDO_CREATE_DOMAIN ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CDO_CONSTRAINT_NAME_DEFINITION;
            pInfoChild->stringInfo = "SQL_CDO_CONSTRAINT_NAME_DEFINITION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CDO_CONSTRAINT_NAME_DEFINITION ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CDO_DEFAULT;
            pInfoChild->stringInfo = "SQL_CDO_DEFAULT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CDO_DEFAULT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CDO_CONSTRAINT;
            pInfoChild->stringInfo = "SQL_CDO_CONSTRAINT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CDO_CONSTRAINT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CDO_COLLATION;
            pInfoChild->stringInfo = "SQL_CDO_COLLATION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CDO_COLLATION ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CDO_CONSTRAINT_INITIALLY_DEFERRED;
            pInfoChild->stringInfo = "SQL_CDO_CONSTRAINT_INITIALLY_DEFERRED";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CDO_CONSTRAINT_INITIALLY_DEFERRED ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CDO_CONSTRAINT_INITIALLY_IMMEDIATE;
            pInfoChild->stringInfo = "SQL_CDO_CONSTRAINT_INITIALLY_IMMEDIATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CDO_CONSTRAINT_INITIALLY_IMMEDIATE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CDO_CONSTRAINT_DEFERRABLE;
            pInfoChild->stringInfo = "SQL_CDO_CONSTRAINT_DEFERRABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CDO_CONSTRAINT_DEFERRABLE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CDO_CONSTRAINT_NON_DEFERRABLE;
            pInfoChild->stringInfo = "SQL_CDO_CONSTRAINT_NON_DEFERRABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CDO_CONSTRAINT_NON_DEFERRABLE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_CREATE_SCHEMA:
            pInfoParent->stringInfo = "SQL_CREATE_SCHEMA";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CS_CREATE_SCHEMA;
            pInfoChild->stringInfo = "SQL_CS_CREATE_SCHEMA";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CS_CREATE_SCHEMA ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CS_AUTHORIZATION;
            pInfoChild->stringInfo = "SQL_CS_AUTHORIZATION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CS_AUTHORIZATION ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CS_DEFAULT_CHARACTER_SET;
            pInfoChild->stringInfo = "SQL_CS_DEFAULT_CHARACTER_SET";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CS_DEFAULT_CHARACTER_SET ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_CREATE_TABLE:
            pInfoParent->stringInfo = "SQL_CREATE_TABLE";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CT_CREATE_TABLE;
            pInfoChild->stringInfo = "SQL_CT_CREATE_TABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CT_CREATE_TABLE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CT_TABLE_CONSTRAINT;
            pInfoChild->stringInfo = "SQL_CT_TABLE_CONSTRAINT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CT_TABLE_CONSTRAINT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CT_CONSTRAINT_NAME_DEFINITION;
            pInfoChild->stringInfo = "SQL_CT_CONSTRAINT_NAME_DEFINITION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CT_CONSTRAINT_NAME_DEFINITION ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo();
            pInfoChild->nInfo = SQL_CT_COMMIT_PRESERVE;
            pInfoChild->stringInfo = "SQL_CT_COMMIT_PRESERVE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CT_COMMIT_PRESERVE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CT_COMMIT_DELETE;
            pInfoChild->stringInfo = "SQL_CT_COMMIT_DELETE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CT_COMMIT_DELETE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CT_GLOBAL_TEMPORARY;
            pInfoChild->stringInfo = "SQL_CT_GLOBAL_TEMPORARY";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CT_GLOBAL_TEMPORARY ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CT_LOCAL_TEMPORARY;
            pInfoChild->stringInfo = "SQL_CT_LOCAL_TEMPORARY";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CT_LOCAL_TEMPORARY ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CT_COLUMN_CONSTRAINT;
            pInfoChild->stringInfo = "SQL_CT_COLUMN_CONSTRAINT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CT_COLUMN_CONSTRAINT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CT_COLUMN_DEFAULT;
            pInfoChild->stringInfo = "SQL_CT_COLUMN_DEFAULT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CT_COLUMN_DEFAULT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CT_COLUMN_DEFAULT;
            pInfoChild->stringInfo = "SQL_CT_COLUMN_DEFAULT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CT_COLUMN_DEFAULT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CT_COLUMN_COLLATION;
            pInfoChild->stringInfo = "SQL_CT_COLUMN_COLLATION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CT_COLUMN_COLLATION ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CT_CONSTRAINT_INITIALLY_DEFERRED;
            pInfoChild->stringInfo = "SQL_CT_CONSTRAINT_INITIALLY_DEFERRED";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CT_CONSTRAINT_INITIALLY_DEFERRED ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CT_CONSTRAINT_INITIALLY_IMMEDIATE;
            pInfoChild->stringInfo = "SQL_CT_CONSTRAINT_INITIALLY_IMMEDIATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CT_CONSTRAINT_INITIALLY_IMMEDIATE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CT_CONSTRAINT_DEFERRABLE;
            pInfoChild->stringInfo = "SQL_CT_CONSTRAINT_DEFERRABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CT_CONSTRAINT_DEFERRABLE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CT_CONSTRAINT_NON_DEFERRABLE;
            pInfoChild->stringInfo = "SQL_CT_CONSTRAINT_NON_DEFERRABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CT_CONSTRAINT_NON_DEFERRABLE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_CREATE_TRANSLATION:
            pInfoParent->stringInfo = "SQL_CREATE_TRANSLATION";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CTR_CREATE_TRANSLATION;
            pInfoChild->stringInfo = "SQL_CTR_CREATE_TRANSLATION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CTR_CREATE_TRANSLATION ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_CREATE_VIEW:
            pInfoParent->stringInfo = "SQL_CREATE_VIEW";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CV_CREATE_VIEW;
            pInfoChild->stringInfo = "SQL_CV_CREATE_VIEW";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CV_CREATE_VIEW ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CV_CHECK_OPTION;
            pInfoChild->stringInfo = "SQL_CV_CHECK_OPTION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CV_CHECK_OPTION ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CV_CASCADED;
            pInfoChild->stringInfo = "SQL_CV_CASCADED";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CV_CASCADED ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CV_LOCAL;
            pInfoChild->stringInfo = "SQL_CV_LOCAL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CV_LOCAL ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_DATETIME_LITERALS:
            pInfoParent->stringInfo = "SQL_DATETIME_LITERALS";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DL_SQL92_DATE;
            pInfoChild->stringInfo = "SQL_DL_SQL92_DATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DL_SQL92_DATE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DL_SQL92_TIME;
            pInfoChild->stringInfo = "SQL_DL_SQL92_TIME";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DL_SQL92_TIME ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DL_SQL92_TIMESTAMP;
            pInfoChild->stringInfo = "SQL_DL_SQL92_TIMESTAMP";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DL_SQL92_TIMESTAMP ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DL_SQL92_INTERVAL_YEAR;
            pInfoChild->stringInfo = "SQL_DL_SQL92_INTERVAL_YEAR";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DL_SQL92_INTERVAL_YEAR ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DL_SQL92_INTERVAL_MONTH;
            pInfoChild->stringInfo = "SQL_DL_SQL92_INTERVAL_MONTH";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DL_SQL92_INTERVAL_MONTH ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DL_SQL92_INTERVAL_DAY;
            pInfoChild->stringInfo = "SQL_DL_SQL92_INTERVAL_DAY";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DL_SQL92_INTERVAL_DAY ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DL_SQL92_INTERVAL_HOUR;
            pInfoChild->stringInfo = "SQL_DL_SQL92_INTERVAL_HOUR";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DL_SQL92_INTERVAL_HOUR ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DL_SQL92_INTERVAL_MINUTE;
            pInfoChild->stringInfo = "SQL_DL_SQL92_INTERVAL_MINUTE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DL_SQL92_INTERVAL_MINUTE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DL_SQL92_INTERVAL_SECOND;
            pInfoChild->stringInfo = "SQL_DL_SQL92_INTERVAL_SECOND";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DL_SQL92_INTERVAL_SECOND ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DL_SQL92_INTERVAL_YEAR_TO_MONTH;
            pInfoChild->stringInfo = "SQL_DL_SQL92_INTERVAL_YEAR_TO_MONTH";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DL_SQL92_INTERVAL_YEAR_TO_MONTH ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DL_SQL92_INTERVAL_DAY_TO_HOUR;
            pInfoChild->stringInfo = "SQL_DL_SQL92_INTERVAL_DAY_TO_HOUR";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DL_SQL92_INTERVAL_DAY_TO_HOUR ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DL_SQL92_INTERVAL_DAY_TO_MINUTE;
            pInfoChild->stringInfo = "SQL_DL_SQL92_INTERVAL_DAY_TO_MINUTE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DL_SQL92_INTERVAL_DAY_TO_MINUTE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DL_SQL92_INTERVAL_DAY_TO_SECOND;
            pInfoChild->stringInfo = "SQL_DL_SQL92_INTERVAL_DAY_TO_SECOND";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DL_SQL92_INTERVAL_DAY_TO_SECOND ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DL_SQL92_INTERVAL_HOUR_TO_MINUTE;
            pInfoChild->stringInfo = "SQL_DL_SQL92_INTERVAL_HOUR_TO_MINUTE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DL_SQL92_INTERVAL_HOUR_TO_MINUTE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DL_SQL92_INTERVAL_HOUR_TO_SECOND;
            pInfoChild->stringInfo = "SQL_DL_SQL92_INTERVAL_HOUR_TO_SECOND";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DL_SQL92_INTERVAL_HOUR_TO_SECOND ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DL_SQL92_INTERVAL_MINUTE_TO_SECOND;
            pInfoChild->stringInfo = "SQL_DL_SQL92_INTERVAL_MINUTE_TO_SECOND";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DL_SQL92_INTERVAL_MINUTE_TO_SECOND ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_DDL_INDEX:
            pInfoParent->stringInfo = "SQL_DDL_INDEX";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DI_CREATE_INDEX;
            pInfoChild->stringInfo = "SQL_DI_CREATE_INDEX";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DI_CREATE_INDEX ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DI_DROP_INDEX;
            pInfoChild->stringInfo = "SQL_DI_DROP_INDEX";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DI_DROP_INDEX ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_DROP_ASSERTION:
            pInfoParent->stringInfo = "SQL_DROP_ASSERTION";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DA_DROP_ASSERTION;
            pInfoChild->stringInfo = "SQL_DA_DROP_ASSERTION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DA_DROP_ASSERTION ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_DROP_CHARACTER_SET:
            pInfoParent->stringInfo = "SQL_DROP_CHARACTER_SET";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DCS_DROP_CHARACTER_SET;
            pInfoChild->stringInfo = "SQL_DCS_DROP_CHARACTER_SET";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DCS_DROP_CHARACTER_SET ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_DROP_COLLATION:
            pInfoParent->stringInfo = "SQL_DROP_CHARACTER_SET";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DC_DROP_COLLATION;
            pInfoChild->stringInfo = "SQL_DC_DROP_COLLATION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DC_DROP_COLLATION ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_DROP_DOMAIN:
            pInfoParent->stringInfo = "SQL_DROP_DOMAIN";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DD_CASCADE;
            pInfoChild->stringInfo = "SQL_DD_CASCADE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DD_CASCADE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DD_DROP_DOMAIN;
            pInfoChild->stringInfo = "SQL_DD_DROP_DOMAIN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DD_DROP_DOMAIN ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DD_RESTRICT;
            pInfoChild->stringInfo = "SQL_DD_RESTRICT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DD_RESTRICT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_DROP_SCHEMA:
            pInfoParent->stringInfo = "SQL_DROP_SCHEMA";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DS_DROP_SCHEMA;
            pInfoChild->stringInfo = "SQL_DS_DROP_SCHEMA";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DS_DROP_SCHEMA ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DS_CASCADE;
            pInfoChild->stringInfo = "SQL_DS_CASCADE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DS_CASCADE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DS_RESTRICT;
            pInfoChild->stringInfo = "SQL_DS_RESTRICT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DS_RESTRICT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_DROP_TABLE:
            pInfoParent->stringInfo = "SQL_DROP_TABLE";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DT_DROP_TABLE;
            pInfoChild->stringInfo = "SQL_DT_DROP_TABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DS_DROP_SCHEMA ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DT_CASCADE;
            pInfoChild->stringInfo = "SQL_DT_CASCADE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DT_CASCADE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DT_RESTRICT;
            pInfoChild->stringInfo = "SQL_DT_RESTRICT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DT_RESTRICT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_DROP_TRANSLATION:
            pInfoParent->stringInfo = "SQL_DROP_TRANSLATION";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DTR_DROP_TRANSLATION;
            pInfoChild->stringInfo = "SQL_DTR_DROP_TRANSLATION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DTR_DROP_TRANSLATION ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_DROP_VIEW:
            pInfoParent->stringInfo = "SQL_DROP_VIEW";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DV_DROP_VIEW;
            pInfoChild->stringInfo = "SQL_DV_DROP_VIEW";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DV_DROP_VIEW ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DV_CASCADE;
            pInfoChild->stringInfo = "SQL_DV_CASCADE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DV_CASCADE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_DV_RESTRICT;
            pInfoChild->stringInfo = "SQL_DV_RESTRICT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_DV_RESTRICT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_DYNAMIC_CURSOR_ATTRIBUTES1:
            pInfoParent->stringInfo = "SQL_DYNAMIC_CURSOR_ATTRIBUTES1";
            pInfoParent->stringDescription = QObject::tr( "An SQLUINTEGER bitmask that describes the attributes of a static cursor that are supported by the driver. This bitmask contains the first subset of attributes; for the second subset, see SQL_STATIC_CURSOR_ATTRIBUTES2." );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_NEXT;
            pInfoChild->stringInfo = "SQL_CA1_NEXT";
            pInfoChild->stringDescription = QObject::tr( "A FetchOrientation argument of SQL_FETCH_NEXT is supported in a call to SQLFetchScroll when the cursor is a dynamic cursor." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA1_NEXT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_ABSOLUTE;
            pInfoChild->stringInfo = "SQL_CA1_ABSOLUTE";
            pInfoChild->stringDescription = QObject::tr( "FetchOrientation arguments of SQL_FETCH_FIRST, SQL_FETCH_LAST, and SQL_FETCH_ABSOLUTE are supported in a call to SQLFetchScroll when the cursor is a dynamic cursor. (The rowset that will be fetched is independent of the current cursor position.)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA1_ABSOLUTE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_RELATIVE;
            pInfoChild->stringInfo = "SQL_CA1_RELATIVE";
            pInfoChild->stringDescription = QObject::tr( "FetchOrientation arguments of SQL_FETCH_PRIOR and SQL_FETCH_RELATIVE are supported in a call to SQLFetchScroll when the cursor is a dynamic cursor. (The rowset that will be fetched is dependent on the current cursor position. Note that this is separated from SQL_FETCH_NEXT because in a forward-only cursor, only SQL_FETCH_NEXT is supportepInfoChild->)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA1_RELATIVE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_BOOKMARK;
            pInfoChild->stringInfo = "SQL_CA1_BOOKMARK";
            pInfoChild->stringDescription = QObject::tr( "A FetchOrientation argument of SQL_FETCH_BOOKMARK is supported in a call to SQLFetchScroll when the cursor is a dynamic cursor." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA1_BOOKMARK ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_LOCK_EXCLUSIVE;
            pInfoChild->stringInfo = "SQL_CA1_LOCK_EXCLUSIVE";
            pInfoChild->stringDescription = QObject::tr( "A LockType argument of SQL_LOCK_EXCLUSIVE is supported in a call to SQLSetPos when the cursor is a dynamic cursor." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA1_LOCK_EXCLUSIVE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_LOCK_NO_CHANGE;
            pInfoChild->stringInfo = "SQL_CA1_LOCK_NO_CHANGE";
            pInfoChild->stringDescription = QObject::tr( "A LockType argument of SQL_LOCK_NO_CHANGE is supported in a call to SQLSetPos when the cursor is a dynamic cursor." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA1_LOCK_NO_CHANGE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_LOCK_UNLOCK;
            pInfoChild->stringInfo = "SQL_CA1_LOCK_UNLOCK";
            pInfoChild->stringDescription = QObject::tr( "A LockType argument of SQL_LOCK_UNLOCK is supported in a call to SQLSetPos when the cursor is a dynamic cursor." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA1_LOCK_UNLOCK ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_POS_POSITION;
            pInfoChild->stringInfo = "SQL_CA1_POS_POSITION";
            pInfoChild->stringDescription = QObject::tr( "An Operation argument of SQL_POSITION is supported in a call to SQLSetPos when the cursor is a dynamic cursor." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA1_POS_POSITION ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_POS_UPDATE;
            pInfoChild->stringInfo = "SQL_CA1_POS_UPDATE";
            pInfoChild->stringDescription = QObject::tr( "An Operation argument of SQL_UPDATE is supported in a call to SQLSetPos when the cursor is a dynamic cursor." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA1_POS_UPDATE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_POS_DELETE;
            pInfoChild->stringInfo = "SQL_CA1_POS_DELETE";
            pInfoChild->stringDescription = QObject::tr( "An Operation argument of SQL_DELETE is supported in a call to SQLSetPos when the cursor is a dynamic cursor." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA1_POS_DELETE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_POS_REFRESH;
            pInfoChild->stringInfo = "SQL_CA1_POS_REFRESH";
            pInfoChild->stringDescription = QObject::tr( "An Operation argument of SQL_REFRESH is supported in a call to SQLSetPos when the cursor is a dynamic cursor." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA1_POS_REFRESH ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_POSITIONED_UPDATE;
            pInfoChild->stringInfo = "SQL_CA1_POSITIONED_UPDATE";
            pInfoChild->stringDescription = QObject::tr( "An UPDATE WHERE CURRENT OF SQL statement is supported when the cursor is a dynamic cursor. (An SQL-92 Entry level conformant driver will always return this option as supportepInfoChild->)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA1_POSITIONED_UPDATE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_POSITIONED_DELETE;
            pInfoChild->stringInfo = "SQL_CA1_POSITIONED_DELETE";
            pInfoChild->stringDescription = QObject::tr( "A DELETE WHERE CURRENT OF SQL statement is supported when the cursor is a dynamic cursor. (An SQL-92 Entry level conformant driver will always return this option as supportepInfoChild->)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA1_POSITIONED_DELETE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_SELECT_FOR_UPDATE;
            pInfoChild->stringInfo = "SQL_CA1_SELECT_FOR_UPDATE";
            pInfoChild->stringDescription = QObject::tr( "A SELECT FOR UPDATE SQL statement is supported when the cursor is a dynamic cursor. (An SQL-92 Entry level conformant driver will always return this option as supportepInfoChild->)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA1_SELECT_FOR_UPDATE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_BULK_ADD;
            pInfoChild->stringInfo = "SQL_CA1_BULK_ADD";
            pInfoChild->stringDescription = QObject::tr( "An Operation argument of SQL_ADD is supported in a call to SQLBulkOperations when the cursor is a dynamic cursor." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA1_BULK_ADD ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_BULK_UPDATE_BY_BOOKMARK;
            pInfoChild->stringInfo = "SQL_CA1_BULK_UPDATE_BY_BOOKMARK";
            pInfoChild->stringDescription = QObject::tr( "An Operation argument of SQL_UPDATE_BY_BOOKMARK is supported in a call to SQLBulkOperations when the cursor is a dynamic cursor." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA1_BULK_UPDATE_BY_BOOKMARK ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_BULK_DELETE_BY_BOOKMARK;
            pInfoChild->stringInfo = "SQL_CA1_BULK_DELETE_BY_BOOKMARK";
            pInfoChild->stringDescription = QObject::tr( "An Operation argument of SQL_DELETE_BY_BOOKMARK is supported in a call to SQLBulkOperations when the cursor is a dynamic cursor." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA1_BULK_DELETE_BY_BOOKMARK ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_BULK_FETCH_BY_BOOKMARK;
            pInfoChild->stringInfo = "SQL_CA1_BULK_FETCH_BY_BOOKMARK";
            pInfoChild->stringDescription = QObject::tr( "An Operation argument of SQL_FETCH_BY_BOOKMARK is supported in a call to SQLBulkOperations when the cursor is a dynamic cursor." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA1_BULK_FETCH_BY_BOOKMARK ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_DYNAMIC_CURSOR_ATTRIBUTES2:
            pInfoParent->stringInfo = "SQL_DYNAMIC_CURSOR_ATTRIBUTES2";
            pInfoParent->stringDescription = QObject::tr( "An SQLUINTEGER bitmask that describes the attributes of a static cursor that are supported by the driver. This bitmask contains the second subset of attributes; for the first subset, see SQL_STATIC_CURSOR_ATTRIBUTES1." );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_READ_ONLY_CONCURRENCY;
            pInfoChild->stringInfo = "SQL_CA2_READ_ONLY_CONCURRENCY";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA2_READ_ONLY_CONCURRENCY ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_LOCK_CONCURRENCY;
            pInfoChild->stringInfo = "SQL_CA2_LOCK_CONCURRENCY";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA2_LOCK_CONCURRENCY ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_OPT_ROWVER_CONCURRENCY;
            pInfoChild->stringInfo = "SQL_CA2_OPT_ROWVER_CONCURRENCY";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA2_OPT_ROWVER_CONCURRENCY ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_OPT_VALUES_CONCURRENCY;
            pInfoChild->stringInfo = "SQL_CA2_OPT_VALUES_CONCURRENCY";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA2_OPT_VALUES_CONCURRENCY ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_SENSITIVITY_ADDITIONS;
            pInfoChild->stringInfo = "SQL_CA2_SENSITIVITY_ADDITIONS";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA2_SENSITIVITY_ADDITIONS ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_SENSITIVITY_DELETIONS;
            pInfoChild->stringInfo = "SQL_CA2_SENSITIVITY_DELETIONS";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA2_SENSITIVITY_DELETIONS ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_SENSITIVITY_UPDATES;
            pInfoChild->stringInfo = "SQL_CA2_SENSITIVITY_UPDATES";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA2_SENSITIVITY_UPDATES ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_MAX_ROWS_SELECT;
            pInfoChild->stringInfo = "SQL_CA2_MAX_ROWS_SELECT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA2_MAX_ROWS_SELECT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_MAX_ROWS_INSERT;
            pInfoChild->stringInfo = "SQL_CA2_MAX_ROWS_INSERT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA2_MAX_ROWS_INSERT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_MAX_ROWS_DELETE;
            pInfoChild->stringInfo = "SQL_CA2_MAX_ROWS_DELETE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA2_MAX_ROWS_DELETE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_MAX_ROWS_UPDATE;
            pInfoChild->stringInfo = "SQL_CA2_MAX_ROWS_UPDATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA2_MAX_ROWS_UPDATE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_MAX_ROWS_CATALOG;
            pInfoChild->stringInfo = "SQL_CA2_MAX_ROWS_CATALOG";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA2_MAX_ROWS_CATALOG ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_MAX_ROWS_AFFECTS_ALL;
            pInfoChild->stringInfo = "SQL_CA2_MAX_ROWS_AFFECTS_ALL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA2_MAX_ROWS_AFFECTS_ALL ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_CRC_EXACT;
            pInfoChild->stringInfo = "SQL_CA2_CRC_EXACT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA2_CRC_EXACT ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_CRC_APPROXIMATE;
            pInfoChild->stringInfo = "SQL_CA2_CRC_APPROXIMATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA2_CRC_APPROXIMATE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_SIMULATE_NON_UNIQUE;
            pInfoChild->stringInfo = "SQL_CA2_SIMULATE_NON_UNIQUE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA2_SIMULATE_NON_UNIQUE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_SIMULATE_TRY_UNIQUE;
            pInfoChild->stringInfo = "SQL_CA2_SIMULATE_TRY_UNIQUE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA2_SIMULATE_TRY_UNIQUE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_SIMULATE_UNIQUE;
            pInfoChild->stringInfo = "SQL_CA2_SIMULATE_UNIQUE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & SQL_CA2_SIMULATE_UNIQUE ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1:
            pInfoParent->stringInfo = "SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_NEXT;
            pInfoChild->stringInfo = "SQL_CA1_NEXT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_LOCK_EXCLUSIVE;
            pInfoChild->stringInfo = "SQL_CA1_LOCK_EXCLUSIVE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_LOCK_NO_CHANGE;
            pInfoChild->stringInfo = "SQL_CA1_LOCK_NO_CHANGE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_LOCK_UNLOCK;
            pInfoChild->stringInfo = "SQL_CA1_LOCK_UNLOCK";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_POS_POSITION;
            pInfoChild->stringInfo = "SQL_CA1_POS_POSITION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_POS_UPDATE;
            pInfoChild->stringInfo = "SQL_CA1_POS_UPDATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_POS_DELETE;
            pInfoChild->stringInfo = "SQL_CA1_POS_DELETE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_POS_REFRESH;
            pInfoChild->stringInfo = "SQL_CA1_POS_REFRESH";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_POSITIONED_UPDATE;
            pInfoChild->stringInfo = "SQL_CA1_POSITIONED_UPDATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_POSITIONED_DELETE;
            pInfoChild->stringInfo = "SQL_CA1_POSITIONED_DELETE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_SELECT_FOR_UPDATE;
            pInfoChild->stringInfo = "SQL_CA1_SELECT_FOR_UPDATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_BULK_ADD;
            pInfoChild->stringInfo = "SQL_CA1_BULK_ADD";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_BULK_UPDATE_BY_BOOKMARK;
            pInfoChild->stringInfo = "SQL_CA1_BULK_UPDATE_BY_BOOKMARK";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_BULK_DELETE_BY_BOOKMARK;
            pInfoChild->stringInfo = "SQL_CA1_BULK_DELETE_BY_BOOKMARK";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_BULK_FETCH_BY_BOOKMARK;
            pInfoChild->stringInfo = "SQL_CA1_BULK_FETCH_BY_BOOKMARK";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES2:
            pInfoParent->stringInfo = "SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES2";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_READ_ONLY_CONCURRENCY;
            pInfoChild->stringInfo = "SQL_CA2_READ_ONLY_CONCURRENCY";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_LOCK_CONCURRENCY;
            pInfoChild->stringInfo = "SQL_CA2_LOCK_CONCURRENCY";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_OPT_ROWVER_CONCURRENCY;
            pInfoChild->stringInfo = "SQL_CA2_OPT_ROWVER_CONCURRENCY";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_OPT_VALUES_CONCURRENCY;
            pInfoChild->stringInfo = "SQL_CA2_OPT_VALUES_CONCURRENCY";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_SENSITIVITY_ADDITIONS;
            pInfoChild->stringInfo = "SQL_CA2_SENSITIVITY_ADDITIONS";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_SENSITIVITY_DELETIONS;
            pInfoChild->stringInfo = "SQL_CA2_SENSITIVITY_DELETIONS";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_SENSITIVITY_UPDATES;
            pInfoChild->stringInfo = "SQL_CA2_SENSITIVITY_UPDATES";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_MAX_ROWS_SELECT;
            pInfoChild->stringInfo = "SQL_CA2_MAX_ROWS_SELECT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_MAX_ROWS_INSERT;
            pInfoChild->stringInfo = "SQL_CA2_MAX_ROWS_INSERT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_MAX_ROWS_DELETE;
            pInfoChild->stringInfo = "SQL_CA2_MAX_ROWS_DELETE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_MAX_ROWS_UPDATE;
            pInfoChild->stringInfo = "SQL_CA2_MAX_ROWS_UPDATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_MAX_ROWS_CATALOG;
            pInfoChild->stringInfo = "SQL_CA2_MAX_ROWS_CATALOG";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_MAX_ROWS_AFFECTS_ALL;
            pInfoChild->stringInfo = "SQL_CA2_MAX_ROWS_AFFECTS_ALL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_CRC_EXACT;
            pInfoChild->stringInfo = "SQL_CA2_CRC_EXACT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_CRC_APPROXIMATE;
            pInfoChild->stringInfo = "SQL_CA2_CRC_APPROXIMATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_SIMULATE_NON_UNIQUE;
            pInfoChild->stringInfo = "SQL_CA2_SIMULATE_NON_UNIQUE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_SIMULATE_TRY_UNIQUE;
            pInfoChild->stringInfo = "SQL_CA2_SIMULATE_TRY_UNIQUE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_SIMULATE_UNIQUE;
            pInfoChild->stringInfo = "SQL_CA2_SIMULATE_UNIQUE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_GETDATA_EXTENSIONS:
            pInfoParent->stringInfo = "SQL_GETDATA_EXTENSIONS";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_GD_ANY_COLUMN;
            pInfoChild->stringInfo = "SQL_GD_ANY_COLUMN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_GD_ANY_ORDER;
            pInfoChild->stringInfo = "SQL_GD_ANY_ORDER";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_GD_BLOCK;
            pInfoChild->stringInfo = "SQL_GD_BLOCK";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_GD_BOUND;
            pInfoChild->stringInfo = "SQL_GD_BOUND";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_INDEX_KEYWORDS:
            pInfoParent->stringInfo = "SQL_INDEX_KEYWORDS";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_IK_NONE;
            pInfoChild->stringInfo = "SQL_IK_NONE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_IK_ASC;
            pInfoChild->stringInfo = "SQL_IK_ASC";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_IK_DESC;
            pInfoChild->stringInfo = "SQL_IK_DESC";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_IK_ALL;
            pInfoChild->stringInfo = "SQL_IK_ALL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_INFO_SCHEMA_VIEWS:
            pInfoParent->stringInfo = "SQL_INFO_SCHEMA_VIEWS";
            pInfoParent->stringDescription = QObject::tr( "An SQLUINTEGER bitmask enumerating the views in the INFORMATION_SCHEMA that are supported by the driver. The views in, and the contents of, INFORMATION_SCHEMA are as defined in SQL-92." );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_ISV_ASSERTIONS;
            pInfoChild->stringInfo = "SQL_ISV_ASSERTIONS";
            pInfoChild->stringDescription = QObject::tr( "Identifies the catalog's assertions that are owned by a given user. (Full level)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_ISV_CHARACTER_SETS;
            pInfoChild->stringInfo = "SQL_ISV_CHARACTER_SETS";
            pInfoChild->stringDescription = QObject::tr( "Identifies the catalog's character sets that are accessible to a given user. (Intermediate level)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_ISV_CHECK_CONSTRAINTS;
            pInfoChild->stringInfo = "SQL_ISV_CHECK_CONSTRAINTS";
            pInfoChild->stringDescription = QObject::tr( "Identifies the CHECK constraints that are owned by a given user. (Intermediate level)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_ISV_COLLATIONS;
            pInfoChild->stringInfo = "SQL_ISV_COLLATIONS";
            pInfoChild->stringDescription = QObject::tr( "Identifies the character collations for the catalog that are accessible to a given user. (Full level)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_ISV_COLUMN_DOMAIN_USAGE;
            pInfoChild->stringInfo = "SQL_ISV_COLUMN_DOMAIN_USAGE";
            pInfoChild->stringDescription = QObject::tr( "Identifies columns for the catalog that are dependent on domains defined in the catalog and are owned by a given user. (Intermediate level)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_ISV_COLUMN_PRIVILEGES;
            pInfoChild->stringInfo = "SQL_ISV_COLUMN_PRIVILEGES";
            pInfoChild->stringDescription = QObject::tr( "Identifies the privileges on columns of persistent tables that are available to or granted by a given user. (FIPS Transitional level)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_ISV_COLUMNS;
            pInfoChild->stringInfo = "SQL_ISV_COLUMNS";
            pInfoChild->stringDescription = QObject::tr( "Identifies the columns of persistent tables that are accessible to a given user. (FIPS Transitional level)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_ISV_CONSTRAINT_COLUMN_USAGE;
            pInfoChild->stringInfo = "SQL_ISV_CONSTRAINT_COLUMN_USAGE";
            pInfoChild->stringDescription = QObject::tr( "Similar to CONSTRAINT_TABLE_USAGE view, columns are identified for the various constraints that are owned by a given user. (Intermediate level)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_ISV_CONSTRAINT_TABLE_USAGE;
            pInfoChild->stringInfo = "SQL_ISV_CONSTRAINT_TABLE_USAGE";
            pInfoChild->stringDescription = QObject::tr( "Identifies the tables that are used by constraints (referential, unique, and assertions), and are owned by a given user. (Intermediate level)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_ISV_DOMAIN_CONSTRAINTS;
            pInfoChild->stringInfo = "SQL_ISV_DOMAIN_CONSTRAINTS";
            pInfoChild->stringDescription = QObject::tr( "Identifies the domain constraints (of the domains in the catalog) that are accessible to a given user. (Intermediate level)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_ISV_DOMAINS;
            pInfoChild->stringInfo = "SQL_ISV_DOMAINS";
            pInfoChild->stringDescription = QObject::tr( "Identifies the domains defined in a catalog that are accessible to the user. (Intermediate level)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_ISV_KEY_COLUMN_USAGE;
            pInfoChild->stringInfo = "SQL_ISV_KEY_COLUMN_USAGE";
            pInfoChild->stringDescription = QObject::tr( "Identifies columns defined in the catalog that are constrained as keys by a given user. (Intermediate level)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_ISV_REFERENTIAL_CONSTRAINTS;
            pInfoChild->stringInfo = "SQL_ISV_REFERENTIAL_CONSTRAINTS";
            pInfoChild->stringDescription = QObject::tr( "Identifies the referential constraints that are owned by a given user. (Intermediate level)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_ISV_SCHEMATA;
            pInfoChild->stringInfo = "SQL_ISV_SCHEMATA";
            pInfoChild->stringDescription = QObject::tr( "Identifies the schemas that are owned by a given user. (Intermediate level)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_ISV_SQL_LANGUAGES;
            pInfoChild->stringInfo = "SQL_ISV_SQL_LANGUAGES";
            pInfoChild->stringDescription = QObject::tr( "Identifies the SQL conformance levels, options, and dialects supported by the SQL implementation. (Intermediate level)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_ISV_TABLE_CONSTRAINTS;
            pInfoChild->stringInfo = "SQL_ISV_TABLE_CONSTRAINTS";
            pInfoChild->stringDescription = QObject::tr( "Identifies the table constraints that are owned by a given user. (Intermediate level)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_ISV_TABLE_PRIVILEGES;
            pInfoChild->stringInfo = "SQL_ISV_TABLE_PRIVILEGES";
            pInfoChild->stringDescription = QObject::tr( "Identifies the privileges on persistent tables that are available to or granted by a given user. (FIPS Transitional level)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_ISV_TABLES;
            pInfoChild->stringInfo = "SQL_ISV_TABLES";
            pInfoChild->stringDescription = QObject::tr( "Identifies the persistent tables defined in a catalog that are accessible to a given user. (FIPS Transitional level)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_ISV_TRANSLATIONS;
            pInfoChild->stringInfo = "SQL_ISV_TRANSLATIONS";
            pInfoChild->stringDescription = QObject::tr( "Identifies character translations for the catalog that are accessible to a given user. (Full level)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_ISV_USAGE_PRIVILEGES;
            pInfoChild->stringInfo = "SQL_ISV_USAGE_PRIVILEGES";
            pInfoChild->stringDescription = QObject::tr( "Identifies the USAGE privileges on catalog objects that are available to or owned by a given user. (FIPS Transitional level)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_ISV_VIEW_COLUMN_USAGE;
            pInfoChild->stringInfo = "SQL_ISV_VIEW_COLUMN_USAGE";
            pInfoChild->stringDescription = QObject::tr( "Identifies the columns on which the catalog's views that are owned by a given user are dependent. (Intermediate level)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_ISV_VIEW_TABLE_USAGE;
            pInfoChild->stringInfo = "SQL_ISV_VIEW_TABLE_USAGE";
            pInfoChild->stringDescription = QObject::tr( "Identifies the tables on which the catalog's views that are owned by a given user are dependent. (Intermediate level)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_ISV_VIEWS;
            pInfoChild->stringInfo = "SQL_ISV_VIEWS";
            pInfoChild->stringDescription = QObject::tr( "Identifies the viewed tables defined in this catalog that are accessible to a given user. (FIPS Transitional level)" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_INSERT_STATEMENT:
            pInfoParent->stringInfo = "SQL_INSERT_STATEMENT";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_IS_INSERT_LITERALS;
            pInfoChild->stringInfo = "SQL_IS_INSERT_LITERALS";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_IS_INSERT_SEARCHED;
            pInfoChild->stringInfo = "SQL_IS_INSERT_SEARCHED";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_IS_SELECT_INTO;
            pInfoChild->stringInfo = "SQL_IS_SELECT_INTO";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_KEYSET_CURSOR_ATTRIBUTES1:
            pInfoParent->stringInfo = "SQL_KEYSET_CURSOR_ATTRIBUTES1";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_NEXT;
            pInfoChild->stringInfo = "SQL_CA1_NEXT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_ABSOLUTE;
            pInfoChild->stringInfo = "SQL_CA1_ABSOLUTE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_RELATIVE;
            pInfoChild->stringInfo = "SQL_CA1_RELATIVE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_BOOKMARK;
            pInfoChild->stringInfo = "SQL_CA1_BOOKMARK";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_LOCK_EXCLUSIVE;
            pInfoChild->stringInfo = "SQL_CA1_LOCK_EXCLUSIVE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_LOCK_NO_CHANGE;
            pInfoChild->stringInfo = "SQL_CA1_LOCK_NO_CHANGE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_LOCK_UNLOCK;
            pInfoChild->stringInfo = "SQL_CA1_LOCK_UNLOCK";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_POS_POSITION;
            pInfoChild->stringInfo = "SQL_CA1_POS_POSITION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_POS_UPDATE;
            pInfoChild->stringInfo = "SQL_CA1_POS_UPDATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_POS_DELETE;
            pInfoChild->stringInfo = "SQL_CA1_POS_DELETE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_POS_REFRESH;
            pInfoChild->stringInfo = "SQL_CA1_POS_REFRESH";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_POSITIONED_UPDATE;
            pInfoChild->stringInfo = "SQL_CA1_POSITIONED_UPDATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_POSITIONED_DELETE;
            pInfoChild->stringInfo = "SQL_CA1_POSITIONED_DELETE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_SELECT_FOR_UPDATE;
            pInfoChild->stringInfo = "SQL_CA1_SELECT_FOR_UPDATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_BULK_ADD;
            pInfoChild->stringInfo = "SQL_CA1_BULK_ADD";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_BULK_UPDATE_BY_BOOKMARK;
            pInfoChild->stringInfo = "SQL_CA1_BULK_UPDATE_BY_BOOKMARK";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_BULK_DELETE_BY_BOOKMARK;
            pInfoChild->stringInfo = "SQL_CA1_BULK_DELETE_BY_BOOKMARK";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_BULK_FETCH_BY_BOOKMARK;
            pInfoChild->stringInfo = "SQL_CA1_BULK_FETCH_BY_BOOKMARK";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_KEYSET_CURSOR_ATTRIBUTES2:
            pInfoParent->stringInfo = "SQL_KEYSET_CURSOR_ATTRIBUTES2";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_READ_ONLY_CONCURRENCY;
            pInfoChild->stringInfo = "SQL_CA2_READ_ONLY_CONCURRENCY";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_LOCK_CONCURRENCY;
            pInfoChild->stringInfo = "SQL_CA2_LOCK_CONCURRENCY";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_OPT_ROWVER_CONCURRENCY;
            pInfoChild->stringInfo = "SQL_CA2_OPT_ROWVER_CONCURRENCY";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_OPT_VALUES_CONCURRENCY;
            pInfoChild->stringInfo = "SQL_CA2_OPT_VALUES_CONCURRENCY";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_SENSITIVITY_ADDITIONS;
            pInfoChild->stringInfo = "SQL_CA2_SENSITIVITY_ADDITIONS";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_SENSITIVITY_DELETIONS;
            pInfoChild->stringInfo = "SQL_CA2_SENSITIVITY_DELETIONS";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_SENSITIVITY_UPDATES;
            pInfoChild->stringInfo = "SQL_CA2_SENSITIVITY_UPDATES";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_MAX_ROWS_SELECT;
            pInfoChild->stringInfo = "SQL_CA2_MAX_ROWS_SELECT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_MAX_ROWS_INSERT;
            pInfoChild->stringInfo = "SQL_CA2_MAX_ROWS_INSERT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_MAX_ROWS_DELETE;
            pInfoChild->stringInfo = "SQL_CA2_MAX_ROWS_DELETE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_MAX_ROWS_UPDATE;
            pInfoChild->stringInfo = "SQL_CA2_MAX_ROWS_UPDATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_MAX_ROWS_CATALOG;
            pInfoChild->stringInfo = "SQL_CA2_MAX_ROWS_CATALOG";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_MAX_ROWS_AFFECTS_ALL;
            pInfoChild->stringInfo = "SQL_CA2_MAX_ROWS_AFFECTS_ALL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_CRC_EXACT;
            pInfoChild->stringInfo = "SQL_CA2_CRC_EXACT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_CRC_APPROXIMATE;
            pInfoChild->stringInfo = "SQL_CA2_CRC_APPROXIMATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_SIMULATE_NON_UNIQUE;
            pInfoChild->stringInfo = "SQL_CA2_SIMULATE_NON_UNIQUE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_SIMULATE_TRY_UNIQUE;
            pInfoChild->stringInfo = "SQL_CA2_SIMULATE_TRY_UNIQUE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_SIMULATE_UNIQUE;
            pInfoChild->stringInfo = "SQL_CA2_SIMULATE_UNIQUE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_NUMERIC_FUNCTIONS:
            pInfoParent->stringInfo = "SQL_NUMERIC_FUNCTIONS";
            pInfoParent->stringDescription = QObject::tr( "the scalar numeric functions supported by the driver and associated data source" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_NUM_ABS;
            pInfoChild->stringInfo = "SQL_FN_NUM_ABS";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_NUM_ACOS;
            pInfoChild->stringInfo = "SQL_FN_NUM_ACOS";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_NUM_ASIN;
            pInfoChild->stringInfo = "SQL_FN_NUM_ASIN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_NUM_ATAN;
            pInfoChild->stringInfo = "SQL_FN_NUM_ATAN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_NUM_ATAN2;
            pInfoChild->stringInfo = "SQL_FN_NUM_ATAN2";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_NUM_CEILING;
            pInfoChild->stringInfo = "SQL_FN_NUM_CEILING";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_NUM_COS;
            pInfoChild->stringInfo = "SQL_FN_NUM_COS";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_NUM_COT;
            pInfoChild->stringInfo = "SQL_FN_NUM_COT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_NUM_DEGREES;
            pInfoChild->stringInfo = "SQL_FN_NUM_DEGREES";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_NUM_EXP;
            pInfoChild->stringInfo = "SQL_FN_NUM_EXP";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_NUM_FLOOR;
            pInfoChild->stringInfo = "SQL_FN_NUM_FLOOR";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_NUM_LOG;
            pInfoChild->stringInfo = "SQL_FN_NUM_LOG";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_NUM_LOG10;
            pInfoChild->stringInfo = "SQL_FN_NUM_LOG10";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_NUM_MOD;
            pInfoChild->stringInfo = "SQL_FN_NUM_MOD";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_NUM_PI;
            pInfoChild->stringInfo = "SQL_FN_NUM_PI";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_NUM_POWER;
            pInfoChild->stringInfo = "SQL_FN_NUM_POWER";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_NUM_RADIANS;
            pInfoChild->stringInfo = "SQL_FN_NUM_RADIANS";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_NUM_RAND;
            pInfoChild->stringInfo = "SQL_FN_NUM_RAND";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_NUM_ROUND;
            pInfoChild->stringInfo = "SQL_FN_NUM_ROUND";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_NUM_SIGN;
            pInfoChild->stringInfo = "SQL_FN_NUM_SIGN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_NUM_SIN;
            pInfoChild->stringInfo = "SQL_FN_NUM_SIN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_NUM_SQRT;
            pInfoChild->stringInfo = "SQL_FN_NUM_SQRT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_NUM_TAN;
            pInfoChild->stringInfo = "SQL_FN_NUM_TAN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_NUM_TRUNCATE;
            pInfoChild->stringInfo = "SQL_FN_NUM_TRUNCATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_OJ_CAPABILITIES:
            pInfoParent->stringInfo = "SQL_OJ_CAPABILITIES";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_OJ_LEFT;
            pInfoChild->stringInfo = "SQL_OJ_LEFT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_OJ_RIGHT;
            pInfoChild->stringInfo = "SQL_OJ_RIGHT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_OJ_FULL;
            pInfoChild->stringInfo = "SQL_OJ_FULL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_OJ_NESTED;
            pInfoChild->stringInfo = "SQL_OJ_NESTED";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_OJ_NOT_ORDERED;
            pInfoChild->stringInfo = "SQL_OJ_NOT_ORDERED";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_OJ_INNER;
            pInfoChild->stringInfo = "SQL_OJ_INNER";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_OJ_ALL_COMPARISON_OPS;
            pInfoChild->stringInfo = "SQL_OJ_ALL_COMPARISON_OPS";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_PARAM_ARRAY_ROW_COUNTS:
            pInfoParent->stringInfo = "SQL_PARAM_ARRAY_ROW_COUNTS";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_PARC_BATCH;
            pInfoChild->stringInfo = "SQL_PARC_BATCH";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_PARC_NO_BATCH;
            pInfoChild->stringInfo = "SQL_PARC_NO_BATCH";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_PARAM_ARRAY_SELECTS:
            pInfoParent->stringInfo = "SQL_PARAM_ARRAY_SELECTS";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_PAS_BATCH;
            pInfoChild->stringInfo = "SQL_PAS_BATCH";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_PAS_NO_BATCH;
            pInfoChild->stringInfo = "SQL_PAS_NO_BATCH";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_PAS_NO_SELECT;
            pInfoChild->stringInfo = "SQL_PAS_NO_SELECT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_POS_OPERATIONS:
            pInfoParent->stringInfo = "SQL_POS_OPERATIONS";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_POS_POSITION;
            pInfoChild->stringInfo = "SQL_POS_POSITION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_POS_REFRESH;
            pInfoChild->stringInfo = "SQL_POS_REFRESH";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_POS_UPDATE;
            pInfoChild->stringInfo = "SQL_POS_UPDATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_POS_DELETE;
            pInfoChild->stringInfo = "SQL_POS_DELETE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_POS_ADD;
            pInfoChild->stringInfo = "SQL_POS_ADD";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_SCHEMA_USAGE:
            pInfoParent->stringInfo = "SQL_SCHEMA_USAGE";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SU_DML_STATEMENTS;
            pInfoChild->stringInfo = "SQL_SU_DML_STATEMENTS";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SU_PROCEDURE_INVOCATION;
            pInfoChild->stringInfo = "SQL_SU_PROCEDURE_INVOCATION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SU_TABLE_DEFINITION;
            pInfoChild->stringInfo = "SQL_SU_TABLE_DEFINITION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SU_INDEX_DEFINITION;
            pInfoChild->stringInfo = "SQL_SU_INDEX_DEFINITION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SU_PRIVILEGE_DEFINITION;
            pInfoChild->stringInfo = "SQL_SU_PRIVILEGE_DEFINITION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_SCROLL_OPTIONS:
            pInfoParent->stringInfo = "SQL_SCROLL_OPTIONS";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SO_FORWARD_ONLY;
            pInfoChild->stringInfo = "SQL_SO_FORWARD_ONLY";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SO_STATIC;
            pInfoChild->stringInfo = "SQL_SO_STATIC";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SO_KEYSET_DRIVEN;
            pInfoChild->stringInfo = "SQL_SO_KEYSET_DRIVEN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SO_DYNAMIC;
            pInfoChild->stringInfo = "SQL_SO_DYNAMIC";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SO_MIXED;
            pInfoChild->stringInfo = "SQL_SO_MIXED";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_SQL_CONFORMANCE:
            pInfoParent->stringInfo = "SQL_SQL_CONFORMANCE";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SC_SQL92_ENTRY;
            pInfoChild->stringInfo = "SQL_SC_SQL92_ENTRY";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SC_FIPS127_2_TRANSITIONAL;
            pInfoChild->stringInfo = "SQL_SC_FIPS127_2_TRANSITIONAL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SC_SQL92_FULL;
            pInfoChild->stringInfo = "SQL_SC_SQL92_FULL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SC_SQL92_INTERMEDIATE;
            pInfoChild->stringInfo = "SQL_SC_SQL92_INTERMEDIATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_SQL92_DATETIME_FUNCTIONS:
            pInfoParent->stringInfo = "SQL_SQL92_DATETIME_FUNCTIONS";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SDF_CURRENT_DATE;
            pInfoChild->stringInfo = "SQL_SDF_CURRENT_DATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SDF_CURRENT_TIME;
            pInfoChild->stringInfo = "SQL_SDF_CURRENT_TIME";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SDF_CURRENT_TIMESTAMP;
            pInfoChild->stringInfo = "SQL_SDF_CURRENT_TIMESTAMP";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_SQL92_FOREIGN_KEY_DELETE_RULE:
            pInfoParent->stringInfo = "SQL_SQL92_FOREIGN_KEY_DELETE_RULE";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SFKD_CASCADE;
            pInfoChild->stringInfo = "SQL_SFKD_CASCADE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SFKD_NO_ACTION;
            pInfoChild->stringInfo = "SQL_SFKD_NO_ACTION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SFKD_SET_DEFAULT;
            pInfoChild->stringInfo = "SQL_SFKD_SET_DEFAULT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SFKD_SET_NULL;
            pInfoChild->stringInfo = "SQL_SFKD_SET_NULL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_SQL92_FOREIGN_KEY_UPDATE_RULE:
            pInfoParent->stringInfo = "SQL_SQL92_FOREIGN_KEY_UPDATE_RULE";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SFKU_CASCADE;
            pInfoChild->stringInfo = "SQL_SFKU_CASCADE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SFKU_NO_ACTION;
            pInfoChild->stringInfo = "SQL_SFKU_NO_ACTION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SFKU_SET_DEFAULT;
            pInfoChild->stringInfo = "SQL_SFKU_SET_DEFAULT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SFKU_SET_NULL;
            pInfoChild->stringInfo = "SQL_SFKU_SET_NULL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_SQL92_GRANT:
            pInfoParent->stringInfo = "SQL_SQL92_GRANT";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SG_DELETE_TABLE;
            pInfoChild->stringInfo = "SQL_SG_DELETE_TABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SG_INSERT_COLUMN;
            pInfoChild->stringInfo = "SQL_SG_INSERT_COLUMN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SG_INSERT_TABLE;
            pInfoChild->stringInfo = "SQL_SG_INSERT_TABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SG_REFERENCES_TABLE;
            pInfoChild->stringInfo = "SQL_SG_REFERENCES_TABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SG_UPDATE_COLUMN;
            pInfoChild->stringInfo = "SQL_SG_UPDATE_COLUMN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SG_REFERENCES_COLUMN;
            pInfoChild->stringInfo = "SQL_SG_REFERENCES_COLUMN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SG_SELECT_TABLE;
            pInfoChild->stringInfo = "SQL_SG_SELECT_TABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SG_UPDATE_TABLE;
            pInfoChild->stringInfo = "SQL_SG_UPDATE_TABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SG_USAGE_ON_DOMAIN;
            pInfoChild->stringInfo = "SQL_SG_USAGE_ON_DOMAIN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SG_USAGE_ON_CHARACTER_SET;
            pInfoChild->stringInfo = "SQL_SG_USAGE_ON_CHARACTER_SET";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SG_USAGE_ON_COLLATION;
            pInfoChild->stringInfo = "SQL_SG_USAGE_ON_COLLATION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SG_USAGE_ON_TRANSLATION;
            pInfoChild->stringInfo = "SQL_SG_USAGE_ON_TRANSLATION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SG_WITH_GRANT_OPTION;
            pInfoChild->stringInfo = "SQL_SG_WITH_GRANT_OPTION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_SQL92_NUMERIC_VALUE_FUNCTIONS:
            pInfoParent->stringInfo = "SQL_SQL92_NUMERIC_VALUE_FUNCTIONS";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SNVF_BIT_LENGTH;
            pInfoChild->stringInfo = "SQL_SNVF_BIT_LENGTH";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SNVF_CHAR_LENGTH;
            pInfoChild->stringInfo = "SQL_SNVF_CHAR_LENGTH";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SNVF_CHARACTER_LENGTH;
            pInfoChild->stringInfo = "SQL_SNVF_CHARACTER_LENGTH";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SNVF_EXTRACT;
            pInfoChild->stringInfo = "SQL_SNVF_EXTRACT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SNVF_OCTET_LENGTH;
            pInfoChild->stringInfo = "SQL_SNVF_OCTET_LENGTH";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SNVF_POSITION;
            pInfoChild->stringInfo = "SQL_SNVF_POSITION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_SQL92_PREDICATES:
            pInfoParent->stringInfo = "SQL_SQL92_PREDICATES";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SP_BETWEEN;
            pInfoChild->stringInfo = "SQL_SP_BETWEEN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SP_COMPARISON;
            pInfoChild->stringInfo = "SQL_SP_COMPARISON";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SP_EXISTS;
            pInfoChild->stringInfo = "SQL_SP_EXISTS";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SP_IN;
            pInfoChild->stringInfo = "SQL_SP_IN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SP_ISNOTNULL;
            pInfoChild->stringInfo = "SQL_SP_ISNOTNULL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SP_ISNULL;
            pInfoChild->stringInfo = "SQL_SP_ISNULL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SP_LIKE;
            pInfoChild->stringInfo = "SQL_SP_LIKE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SP_MATCH_FULL;
            pInfoChild->stringInfo = "SQL_SP_MATCH_FULL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SP_MATCH_PARTIAL;
            pInfoChild->stringInfo = "SQL_SP_MATCH_PARTIAL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SP_MATCH_UNIQUE_FULL;
            pInfoChild->stringInfo = "SQL_SP_MATCH_UNIQUE_FULL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SP_MATCH_UNIQUE_PARTIAL;
            pInfoChild->stringInfo = "SQL_SP_MATCH_UNIQUE_PARTIAL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SP_OVERLAPS;
            pInfoChild->stringInfo = "SQL_SP_OVERLAPS";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SP_QUANTIFIED_COMPARISON;
            pInfoChild->stringInfo = "SQL_SP_QUANTIFIED_COMPARISON";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SP_UNIQUE;
            pInfoChild->stringInfo = "SQL_SP_UNIQUE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_SQL92_RELATIONAL_JOIN_OPERATORS:
            pInfoParent->stringInfo = "SQL_SQL92_RELATIONAL_JOIN_OPERATORS";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SRJO_CORRESPONDING_CLAUSE;
            pInfoChild->stringInfo = "SQL_SRJO_CORRESPONDING_CLAUSE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SRJO_CROSS_JOIN;
            pInfoChild->stringInfo = "SQL_SRJO_CROSS_JOIN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SRJO_EXCEPT_JOIN;
            pInfoChild->stringInfo = "SQL_SRJO_EXCEPT_JOIN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SRJO_FULL_OUTER_JOIN;
            pInfoChild->stringInfo = "SQL_SRJO_FULL_OUTER_JOIN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SRJO_INNER_JOIN;
            pInfoChild->stringInfo = "SQL_SRJO_INNER_JOIN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SRJO_INTERSECT_JOIN;
            pInfoChild->stringInfo = "SQL_SRJO_INTERSECT_JOIN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SRJO_LEFT_OUTER_JOIN;
            pInfoChild->stringInfo = "SQL_SRJO_LEFT_OUTER_JOIN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SRJO_NATURAL_JOIN;
            pInfoChild->stringInfo = "SQL_SRJO_NATURAL_JOIN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SRJO_RIGHT_OUTER_JOIN;
            pInfoChild->stringInfo = "SQL_SRJO_RIGHT_OUTER_JOIN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SRJO_UNION_JOIN;
            pInfoChild->stringInfo = "SQL_SRJO_UNION_JOIN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_SQL92_REVOKE:
            pInfoParent->stringInfo = "SQL_SQL92_REVOKE";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SR_CASCADE;
            pInfoChild->stringInfo = "SQL_SR_CASCADE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SR_DELETE_TABLE;
            pInfoChild->stringInfo = "SQL_SR_DELETE_TABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SR_GRANT_OPTION_FOR;
            pInfoChild->stringInfo = "SQL_SR_GRANT_OPTION_FOR";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SR_INSERT_COLUMN;
            pInfoChild->stringInfo = "SQL_SR_INSERT_COLUMN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SR_INSERT_TABLE;
            pInfoChild->stringInfo = "SQL_SR_INSERT_TABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SR_REFERENCES_COLUMN;
            pInfoChild->stringInfo = "SQL_SR_REFERENCES_COLUMN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SR_REFERENCES_TABLE;
            pInfoChild->stringInfo = "SQL_SR_REFERENCES_TABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SR_RESTRICT;
            pInfoChild->stringInfo = "SQL_SR_RESTRICT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SR_SELECT_TABLE;
            pInfoChild->stringInfo = "SQL_SR_SELECT_TABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SR_UPDATE_COLUMN;
            pInfoChild->stringInfo = "SQL_SR_UPDATE_COLUMN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SR_UPDATE_TABLE;
            pInfoChild->stringInfo = "SQL_SR_UPDATE_TABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SR_USAGE_ON_DOMAIN;
            pInfoChild->stringInfo = "SQL_SR_USAGE_ON_DOMAIN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SR_USAGE_ON_CHARACTER_SET;
            pInfoChild->stringInfo = "SQL_SR_USAGE_ON_CHARACTER_SET";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SR_USAGE_ON_COLLATION;
            pInfoChild->stringInfo = "SQL_SR_USAGE_ON_COLLATION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SR_USAGE_ON_TRANSLATION;
            pInfoChild->stringInfo = "SQL_SR_USAGE_ON_TRANSLATION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_SQL92_ROW_VALUE_CONSTRUCTOR:
            pInfoParent->stringInfo = "SQL_SQL92_ROW_VALUE_CONSTRUCTOR";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SRVC_VALUE_EXPRESSION;
            pInfoChild->stringInfo = "SQL_SRVC_VALUE_EXPRESSION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SRVC_NULL;
            pInfoChild->stringInfo = "SQL_SRVC_NULL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SRVC_DEFAULT;
            pInfoChild->stringInfo = "SQL_SRVC_DEFAULT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SRVC_ROW_SUBQUERY;
            pInfoChild->stringInfo = "SQL_SRVC_ROW_SUBQUERY";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_SQL92_STRING_FUNCTIONS:
            pInfoParent->stringInfo = "SQL_SQL92_STRING_FUNCTIONS";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SSF_CONVERT;
            pInfoChild->stringInfo = "SQL_SSF_CONVERT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SSF_LOWER;
            pInfoChild->stringInfo = "SQL_SSF_LOWER";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SSF_UPPER;
            pInfoChild->stringInfo = "SQL_SSF_UPPER";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SSF_SUBSTRING;
            pInfoChild->stringInfo = "SQL_SSF_SUBSTRING";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SSF_TRANSLATE;
            pInfoChild->stringInfo = "SQL_SSF_TRANSLATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SSF_TRIM_BOTH;
            pInfoChild->stringInfo = "SQL_SSF_TRIM_BOTH";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SSF_TRIM_LEADING;
            pInfoChild->stringInfo = "SQL_SSF_TRIM_LEADING";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SSF_TRIM_TRAILING;
            pInfoChild->stringInfo = "SQL_SSF_TRIM_TRAILING";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_SQL92_VALUE_EXPRESSIONS:
            pInfoParent->stringInfo = "SQL_SQL92_VALUE_EXPRESSIONS";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SVE_CASE;
            pInfoChild->stringInfo = "SQL_SVE_CASE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SVE_CAST;
            pInfoChild->stringInfo = "SQL_SVE_CAST";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SVE_COALESCE;
            pInfoChild->stringInfo = "SQL_SVE_COALESCE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SVE_NULLIF;
            pInfoChild->stringInfo = "SQL_SVE_NULLIF";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_STANDARD_CLI_CONFORMANCE:
            pInfoParent->stringInfo = "SQL_STANDARD_CLI_CONFORMANCE";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SCC_XOPEN_CLI_VERSION1;
            pInfoChild->stringInfo = "SQL_SCC_XOPEN_CLI_VERSION1";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SCC_ISO92_CLI;
            pInfoChild->stringInfo = "SQL_SCC_ISO92_CLI";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_STATIC_CURSOR_ATTRIBUTES1:
            pInfoParent->stringInfo = "SQL_STATIC_CURSOR_ATTRIBUTES1";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_NEXT;
            pInfoChild->stringInfo = "SQL_CA1_NEXT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_ABSOLUTE;
            pInfoChild->stringInfo = "SQL_CA1_ABSOLUTE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_RELATIVE;
            pInfoChild->stringInfo = "SQL_CA1_RELATIVE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_BOOKMARK;
            pInfoChild->stringInfo = "SQL_CA1_BOOKMARK";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_LOCK_NO_CHANGE;
            pInfoChild->stringInfo = "SQL_CA1_LOCK_NO_CHANGE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_LOCK_EXCLUSIVE;
            pInfoChild->stringInfo = "SQL_CA1_LOCK_EXCLUSIVE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_LOCK_UNLOCK;
            pInfoChild->stringInfo = "SQL_CA1_LOCK_UNLOCK";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_POS_POSITION;
            pInfoChild->stringInfo = "SQL_CA1_POS_POSITION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_POS_UPDATE;
            pInfoChild->stringInfo = "SQL_CA1_POS_UPDATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_POS_DELETE;
            pInfoChild->stringInfo = "SQL_CA1_POS_DELETE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_POS_REFRESH;
            pInfoChild->stringInfo = "SQL_CA1_POS_REFRESH";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_POSITIONED_UPDATE;
            pInfoChild->stringInfo = "SQL_CA1_POSITIONED_UPDATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_POSITIONED_DELETE;
            pInfoChild->stringInfo = "SQL_CA1_POSITIONED_DELETE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_SELECT_FOR_UPDATE;
            pInfoChild->stringInfo = "SQL_CA1_SELECT_FOR_UPDATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_BULK_ADD;
            pInfoChild->stringInfo = "SQL_CA1_BULK_ADD";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_BULK_UPDATE_BY_BOOKMARK;
            pInfoChild->stringInfo = "SQL_CA1_BULK_UPDATE_BY_BOOKMARK";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_BULK_DELETE_BY_BOOKMARK;
            pInfoChild->stringInfo = "SQL_CA1_BULK_DELETE_BY_BOOKMARK";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA1_BULK_FETCH_BY_BOOKMARK;
            pInfoChild->stringInfo = "SQL_CA1_BULK_FETCH_BY_BOOKMARK";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_STATIC_CURSOR_ATTRIBUTES2:
            pInfoParent->stringInfo = "SQL_STATIC_CURSOR_ATTRIBUTES2";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_READ_ONLY_CONCURRENCY;
            pInfoChild->stringInfo = "SQL_CA2_READ_ONLY_CONCURRENCY";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_LOCK_CONCURRENCY;
            pInfoChild->stringInfo = "SQL_CA2_LOCK_CONCURRENCY";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_OPT_ROWVER_CONCURRENCY;
            pInfoChild->stringInfo = "SQL_CA2_OPT_ROWVER_CONCURRENCY";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_OPT_VALUES_CONCURRENCY;
            pInfoChild->stringInfo = "SQL_CA2_OPT_VALUES_CONCURRENCY";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_SENSITIVITY_ADDITIONS;
            pInfoChild->stringInfo = "SQL_CA2_SENSITIVITY_ADDITIONS";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_SENSITIVITY_DELETIONS;
            pInfoChild->stringInfo = "SQL_CA2_SENSITIVITY_DELETIONS";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_SENSITIVITY_UPDATES;
            pInfoChild->stringInfo = "SQL_CA2_SENSITIVITY_UPDATES";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_MAX_ROWS_SELECT;
            pInfoChild->stringInfo = "SQL_CA2_MAX_ROWS_SELECT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_MAX_ROWS_INSERT;
            pInfoChild->stringInfo = "SQL_CA2_MAX_ROWS_INSERT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_MAX_ROWS_DELETE;
            pInfoChild->stringInfo = "SQL_CA2_MAX_ROWS_DELETE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_MAX_ROWS_UPDATE;
            pInfoChild->stringInfo = "SQL_CA2_MAX_ROWS_UPDATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_MAX_ROWS_CATALOG;
            pInfoChild->stringInfo = "SQL_CA2_MAX_ROWS_CATALOG";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_MAX_ROWS_AFFECTS_ALL;
            pInfoChild->stringInfo = "SQL_CA2_MAX_ROWS_AFFECTS_ALL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_CRC_EXACT;
            pInfoChild->stringInfo = "SQL_CA2_CRC_EXACT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_CRC_APPROXIMATE;
            pInfoChild->stringInfo = "SQL_CA2_CRC_APPROXIMATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_SIMULATE_NON_UNIQUE;
            pInfoChild->stringInfo = "SQL_CA2_SIMULATE_NON_UNIQUE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_SIMULATE_TRY_UNIQUE;
            pInfoChild->stringInfo = "SQL_CA2_SIMULATE_TRY_UNIQUE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_CA2_SIMULATE_UNIQUE;
            pInfoChild->stringInfo = "SQL_CA2_SIMULATE_UNIQUE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_STRING_FUNCTIONS:
            pInfoParent->stringInfo = "SQL_STRING_FUNCTIONS";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_STR_ASCII;
            pInfoChild->stringInfo = "SQL_FN_STR_ASCII";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_STR_BIT_LENGTH;
            pInfoChild->stringInfo = "SQL_FN_STR_BIT_LENGTH";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_STR_CHAR;
            pInfoChild->stringInfo = "SQL_FN_STR_CHAR";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_STR_CHAR_LENGTH;
            pInfoChild->stringInfo = "SQL_FN_STR_CHAR_LENGTH";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_STR_CHARACTER_LENGTH;
            pInfoChild->stringInfo = "SQL_FN_STR_CHARACTER_LENGTH";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_STR_CONCAT;
            pInfoChild->stringInfo = "SQL_FN_STR_CONCAT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_STR_DIFFERENCE;
            pInfoChild->stringInfo = "SQL_FN_STR_DIFFERENCE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_STR_INSERT;
            pInfoChild->stringInfo = "SQL_FN_STR_INSERT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_STR_LCASE;
            pInfoChild->stringInfo = "SQL_FN_STR_LCASE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_STR_LEFT;
            pInfoChild->stringInfo = "SQL_FN_STR_LEFT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_STR_LENGTH;
            pInfoChild->stringInfo = "SQL_FN_STR_LENGTH";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_STR_LOCATE;
            pInfoChild->stringInfo = "SQL_FN_STR_LOCATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_STR_LTRIM;
            pInfoChild->stringInfo = "SQL_FN_STR_LTRIM";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_STR_OCTET_LENGTH;
            pInfoChild->stringInfo = "SQL_FN_STR_OCTET_LENGTH";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_STR_POSITION;
            pInfoChild->stringInfo = "SQL_FN_STR_POSITION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_STR_REPEAT;
            pInfoChild->stringInfo = "SQL_FN_STR_REPEAT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_STR_REPLACE;
            pInfoChild->stringInfo = "SQL_FN_STR_REPLACE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_STR_RIGHT;
            pInfoChild->stringInfo = "SQL_FN_STR_RIGHT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_STR_RTRIM;
            pInfoChild->stringInfo = "SQL_FN_STR_RTRIM";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_STR_SOUNDEX;
            pInfoChild->stringInfo = "SQL_FN_STR_SOUNDEX";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_STR_SPACE;
            pInfoChild->stringInfo = "SQL_FN_STR_SPACE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_STR_SUBSTRING;
            pInfoChild->stringInfo = "SQL_FN_STR_SUBSTRING";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_STR_UCASE;
            pInfoChild->stringInfo = "SQL_FN_STR_UCASE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_SUBQUERIES:
            pInfoParent->stringInfo = "SQL_SUBQUERIES";
            pInfoParent->stringDescription = QObject::tr( "predicates that support subqueries" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SQ_CORRELATED_SUBQUERIES;
            pInfoChild->stringInfo = "SQL_SQ_CORRELATED_SUBQUERIES";
            pInfoChild->stringDescription = QObject::tr( "All predicates that support subqueries support correlated subqueries." );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SQ_COMPARISON;
            pInfoChild->stringInfo = "SQL_SQ_COMPARISON";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SQ_EXISTS;
            pInfoChild->stringInfo = "SQL_SQ_EXISTS";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SQ_IN;
            pInfoChild->stringInfo = "SQL_SQ_IN";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_SQ_QUANTIFIED;
            pInfoChild->stringInfo = "SQL_SQ_QUANTIFIED";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_SYSTEM_FUNCTIONS:
            pInfoParent->stringInfo = "SQL_SYSTEM_FUNCTIONS";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_SYS_DBNAME;
            pInfoChild->stringInfo = "SQL_FN_SYS_DBNAME";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_SYS_IFNULL;
            pInfoChild->stringInfo = "SQL_FN_SYS_IFNULL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_SYS_USERNAME;
            pInfoChild->stringInfo = "SQL_FN_SYS_USERNAME";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_TIMEDATE_ADD_INTERVALS:
            pInfoParent->stringInfo = "SQL_TIMEDATE_ADD_INTERVALS";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TSI_FRAC_SECOND;
            pInfoChild->stringInfo = "SQL_FN_TSI_FRAC_SECOND";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TSI_SECOND;
            pInfoChild->stringInfo = "SQL_FN_TSI_SECOND";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TSI_MINUTE;
            pInfoChild->stringInfo = "SQL_FN_TSI_MINUTE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TSI_HOUR;
            pInfoChild->stringInfo = "SQL_FN_TSI_HOUR";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TSI_DAY;
            pInfoChild->stringInfo = "SQL_FN_TSI_DAY";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TSI_WEEK;
            pInfoChild->stringInfo = "SQL_FN_TSI_WEEK";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TSI_MONTH;
            pInfoChild->stringInfo = "SQL_FN_TSI_MONTH";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TSI_QUARTER;
            pInfoChild->stringInfo = "SQL_FN_TSI_QUARTER";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TSI_YEAR;
            pInfoChild->stringInfo = "SQL_FN_TSI_YEAR";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_TIMEDATE_DIFF_INTERVALS:
            pInfoParent->stringInfo = "SQL_TIMEDATE_DIFF_INTERVALS";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TSI_FRAC_SECOND;
            pInfoChild->stringInfo = "SQL_FN_TSI_FRAC_SECOND";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TSI_SECOND;
            pInfoChild->stringInfo = "SQL_FN_TSI_SECOND";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TSI_MINUTE;
            pInfoChild->stringInfo = "SQL_FN_TSI_MINUTE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TSI_HOUR;
            pInfoChild->stringInfo = "SQL_FN_TSI_HOUR";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TSI_DAY;
            pInfoChild->stringInfo = "SQL_FN_TSI_DAY";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TSI_WEEK;
            pInfoChild->stringInfo = "SQL_FN_TSI_WEEK";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TSI_MONTH;
            pInfoChild->stringInfo = "SQL_FN_TSI_MONTH";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TSI_QUARTER;
            pInfoChild->stringInfo = "SQL_FN_TSI_QUARTER";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TSI_YEAR;
            pInfoChild->stringInfo = "SQL_FN_TSI_YEAR";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_TIMEDATE_FUNCTIONS:
            pInfoParent->stringInfo = "SQL_TIMEDATE_FUNCTIONS";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TD_CURRENT_DATE;
            pInfoChild->stringInfo = "SQL_FN_TD_CURRENT_DATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TD_CURRENT_TIME;
            pInfoChild->stringInfo = "SQL_FN_TD_CURRENT_TIME";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TD_CURRENT_TIMESTAMP;
            pInfoChild->stringInfo = "SQL_FN_TD_CURRENT_TIMESTAMP";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TD_CURDATE;
            pInfoChild->stringInfo = "SQL_FN_TD_CURDATE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TD_CURTIME;
            pInfoChild->stringInfo = "SQL_FN_TD_CURTIME";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TD_DAYNAME;
            pInfoChild->stringInfo = "SQL_FN_TD_DAYNAME";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TD_DAYOFMONTH;
            pInfoChild->stringInfo = "SQL_FN_TD_DAYOFMONTH";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TD_DAYOFWEEK;
            pInfoChild->stringInfo = "SQL_FN_TD_DAYOFWEEK";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TD_DAYOFYEAR;
            pInfoChild->stringInfo = "SQL_FN_TD_DAYOFYEAR";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TD_EXTRACT;
            pInfoChild->stringInfo = "SQL_FN_TD_EXTRACT";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TD_HOUR;
            pInfoChild->stringInfo = "SQL_FN_TD_HOUR";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TD_MINUTE;
            pInfoChild->stringInfo = "SQL_FN_TD_MINUTE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TD_MONTH;
            pInfoChild->stringInfo = "SQL_FN_TD_MONTH";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TD_MONTHNAME;
            pInfoChild->stringInfo = "SQL_FN_TD_MONTHNAME";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TD_NOW;
            pInfoChild->stringInfo = "SQL_FN_TD_NOW";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TD_QUARTER;
            pInfoChild->stringInfo = "SQL_FN_TD_QUARTER";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TD_SECOND;
            pInfoChild->stringInfo = "SQL_FN_TD_SECOND";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TD_TIMESTAMPADD;
            pInfoChild->stringInfo = "SQL_FN_TD_TIMESTAMPADD";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TD_TIMESTAMPDIFF;
            pInfoChild->stringInfo = "SQL_FN_TD_TIMESTAMPDIFF";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TD_WEEK;
            pInfoChild->stringInfo = "SQL_FN_TD_WEEK";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_FN_TD_YEAR;
            pInfoChild->stringInfo = "SQL_FN_TD_YEAR";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_TXN_ISOLATION_OPTION:
            pInfoParent->stringInfo = "SQL_TXN_ISOLATION_OPTION";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_TXN_READ_UNCOMMITTED;
            pInfoChild->stringInfo = "SQL_TXN_READ_UNCOMMITTED";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_TXN_READ_COMMITTED;
            pInfoChild->stringInfo = "SQL_TXN_READ_COMMITTED";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_TXN_REPEATABLE_READ;
            pInfoChild->stringInfo = "SQL_TXN_REPEATABLE_READ";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_TXN_SERIALIZABLE;
            pInfoChild->stringInfo = "SQL_TXN_SERIALIZABLE";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
        case SQL_UNION:
            pInfoParent->stringInfo = "SQL_UNION";
            pInfoParent->stringDescription = QObject::tr( "" );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_U_UNION;
            pInfoChild->stringInfo = "SQL_U_UNION";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );

            pInfoChild = new DATAInfo(); pInfoChild->nInfo = SQL_U_UNION_ALL;
            pInfoChild->stringInfo = "SQL_U_UNION_ALL";
            pInfoChild->stringDescription = QObject::tr( "" );
            pInfoChild->vValue = (!pInfoParent->vValue.isNull() && pInfoParent->vValue.toUInt() & pInfoChild->nInfo ? "Y" : "" );
            pInfoParent->mapValues.insert( pInfoChild->stringInfo,  pInfoChild );
            break;
            // CONVERT
        case SQL_CONVERT_BIGINT:
            pInfoParent->stringInfo = "SQL_CONVERT_BIGINT";
            pInfoParent->stringDescription = QObject::tr( "" );
            pInfoParent->mapValues = getConvertValues( pInfoParent->vValue );
            break;
        case SQL_CONVERT_BINARY:
            pInfoParent->stringInfo = "SQL_CONVERT_BINARY";
            pInfoParent->stringDescription = QObject::tr( "" );
            pInfoParent->mapValues = getConvertValues( pInfoParent->vValue );
            break;
        case SQL_CONVERT_BIT :
            pInfoParent->stringInfo = "SQL_CONVERT_BIT";
            pInfoParent->stringDescription = QObject::tr( "" );
            pInfoParent->mapValues = getConvertValues( pInfoParent->vValue );
            break;
        case SQL_CONVERT_CHAR:
            pInfoParent->stringInfo = "SQL_CONVERT_CHAR";
            pInfoParent->stringDescription = QObject::tr( "" );
            pInfoParent->mapValues = getConvertValues( pInfoParent->vValue );
            break;
        case SQL_CONVERT_GUID:
            pInfoParent->stringInfo = "SQL_CONVERT_GUID";
            pInfoParent->stringDescription = QObject::tr( "" );
            pInfoParent->mapValues = getConvertValues( pInfoParent->vValue );
            break;
        case SQL_CONVERT_DATE:
            pInfoParent->stringInfo = "SQL_CONVERT_DATE";
            pInfoParent->stringDescription = QObject::tr( "" );
            pInfoParent->mapValues = getConvertValues( pInfoParent->vValue );
            break;
        case SQL_CONVERT_DECIMAL:
            pInfoParent->stringInfo = "SQL_CONVERT_DECIMAL";
            pInfoParent->stringDescription = QObject::tr( "" );
            pInfoParent->mapValues = getConvertValues( pInfoParent->vValue );
            break;
        case SQL_CONVERT_DOUBLE:
            pInfoParent->stringInfo = "SQL_CONVERT_DOUBLE";
            pInfoParent->stringDescription = QObject::tr( "" );
            pInfoParent->mapValues = getConvertValues( pInfoParent->vValue );
            break;
        case SQL_CONVERT_FLOAT:
            pInfoParent->stringInfo = "SQL_CONVERT_FLOAT";
            pInfoParent->stringDescription = QObject::tr( "" );
            pInfoParent->mapValues = getConvertValues( pInfoParent->vValue );
            break;
        case SQL_CONVERT_INTEGER:
            pInfoParent->stringInfo = "SQL_CONVERT_INTEGER";
            pInfoParent->stringDescription = QObject::tr( "" );
            pInfoParent->mapValues = getConvertValues( pInfoParent->vValue );
            break;
        case SQL_CONVERT_INTERVAL_YEAR_MONTH:
            pInfoParent->stringInfo = "SQL_CONVERT_INTERVAL_YEAR_MONTH";
            pInfoParent->stringDescription = QObject::tr( "" );
            pInfoParent->mapValues = getConvertValues( pInfoParent->vValue );
            break;
        case SQL_CONVERT_INTERVAL_DAY_TIME:
            pInfoParent->stringInfo = "SQL_CONVERT_INTERVAL_DAY_TIME";
            pInfoParent->stringDescription = QObject::tr( "" );
            pInfoParent->mapValues = getConvertValues( pInfoParent->vValue );
            break;
        case SQL_CONVERT_LONGVARBINARY:
            pInfoParent->stringInfo = "SQL_CONVERT_LONGVARBINARY";
            pInfoParent->stringDescription = QObject::tr( "" );
            pInfoParent->mapValues = getConvertValues( pInfoParent->vValue );
            break;
        case SQL_CONVERT_LONGVARCHAR:
            pInfoParent->stringInfo = "SQL_CONVERT_LONGVARCHAR";
            pInfoParent->stringDescription = QObject::tr( "" );
            pInfoParent->mapValues = getConvertValues( pInfoParent->vValue );
            break;
        case SQL_CONVERT_NUMERIC:
            pInfoParent->stringInfo = "SQL_CONVERT_NUMERIC";
            pInfoParent->stringDescription = QObject::tr( "" );
            pInfoParent->mapValues = getConvertValues( pInfoParent->vValue );
            break;
        case SQL_CONVERT_REAL:
            pInfoParent->stringInfo = "SQL_CONVERT_REAL";
            pInfoParent->stringDescription = QObject::tr( "" );
            pInfoParent->mapValues = getConvertValues( pInfoParent->vValue );
            break;
        case SQL_CONVERT_SMALLINT :
            pInfoParent->stringInfo = "SQL_CONVERT_SMALLINT";
            pInfoParent->stringDescription = QObject::tr( "" );
            pInfoParent->mapValues = getConvertValues( pInfoParent->vValue );
            break;
        case SQL_CONVERT_TIME:
            pInfoParent->stringInfo = "SQL_CONVERT_TIME";
            pInfoParent->stringDescription = QObject::tr( "" );
            pInfoParent->mapValues = getConvertValues( pInfoParent->vValue );
            break;
        case SQL_CONVERT_TIMESTAMP:
            pInfoParent->stringInfo = "SQL_CONVERT_TIMESTAMP";
            pInfoParent->stringDescription = QObject::tr( "" );
            pInfoParent->mapValues = getConvertValues( pInfoParent->vValue );
            break;
        case SQL_CONVERT_TINYINT:
            pInfoParent->stringInfo = "SQL_CONVERT_TINYINT";
            pInfoParent->stringDescription = QObject::tr( "" );
            pInfoParent->mapValues = getConvertValues( pInfoParent->vValue );
            break;
        case SQL_CONVERT_VARBINARY:
            pInfoParent->stringInfo = "SQL_CONVERT_VARBINARY";
            pInfoParent->stringDescription = QObject::tr( "" );
            pInfoParent->mapValues = getConvertValues( pInfoParent->vValue );
            break;
        case SQL_CONVERT_VARCHAR:
            pInfoParent->stringInfo = "SQL_CONVERT_VARCHAR";
            pInfoParent->stringDescription = QObject::tr( "" );
            pInfoParent->mapValues = getConvertValues( pInfoParent->vValue );
            break;
        default:
            qDebug() << "[PAH]" << __FILE__ << __FUNCTION__  << __LINE__ << " InfoType: " << nInfoType << " not handlepInfoChild->";
            *pnReturn = SQL_ERROR;
            return pInfoParent;
    }

    return pInfoParent;
}

QMap<QString,DATADataTypeSpec*> DATAProfile::getDataTypes( DATAConnection *pConnection )
{
    QMap<QString,DATADataTypeSpec*> mapDataTypes;
    DATAStatement *                 pStatement = 0;
    SQLRETURN                       nReturn;

    Q_ASSERT( pConnection->isConnected() );

    pStatement = new DATAStatement( pConnection );

    nReturn = pStatement->doTypeInfo();
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        delete pStatement;
        return mapDataTypes;
    }

    nReturn = pStatement->doFetch();
    while ( SQL_SUCCEEDED( nReturn ) )
    {
        DATADataTypeSpec *pDataTypeSpec = new DATADataTypeSpec();
        pDataTypeSpec->TYPE_NAME                 = pStatement->getData( 1 ).toString().toUpper(); // ensures its always upper as we use it as a key
        pDataTypeSpec->nDATA_TYPE                = pStatement->getData( 2 ).toInt();
        pDataTypeSpec->DATA_TYPE                 = DATADataTypeSpec::getDataTypeStr( pDataTypeSpec->nDATA_TYPE );
        pDataTypeSpec->COLUMN_SIZE               = pStatement->getData( 3 ).toString();
        pDataTypeSpec->LITERAL_PREFIX            = pStatement->getData( 4 ).toString();
        pDataTypeSpec->LITERAL_SUFFIX            = pStatement->getData( 5 ).toString();
        pDataTypeSpec->CREATE_PARAMS             = pStatement->getData( 6 ).toString();
        pDataTypeSpec->NULLABLE                  = DATADataTypeSpec::getNullableStr( pStatement->getData( 7 ).toInt() );
        pDataTypeSpec->CASE_SENSITIVE            = DATADataTypeSpec::getCaseSensitiveStr( pStatement->getData( 8 ).toInt() );
        pDataTypeSpec->SEARCHABLE                = DATADataTypeSpec::getSearchableStr( pStatement->getData( 9 ).toInt() );
        pDataTypeSpec->UNSIGNED_ATTRIBUTE        = DATADataTypeSpec::getUnsignedAttributeStr( pStatement->getData( 10 ).toInt() );
        pDataTypeSpec->FIXED_PREC_SCALE          = DATADataTypeSpec::getFixedPrecScaleStr( pStatement->getData( 11 ).toInt() );
        pDataTypeSpec->AUTO_UNIQUE_VALUE         = DATADataTypeSpec::getAutoUniqueValueStr( pStatement->getData( 12 ).toInt() );
        pDataTypeSpec->LOCAL_TYPE_NAME           = pStatement->getData( 13 ).toString();
        pDataTypeSpec->MINIMUM_SCALE             = pStatement->getData( 14 ).toString();
        pDataTypeSpec->MAXIMUM_SCALE             = pStatement->getData( 15 ).toString();
        pDataTypeSpec->SQL_DATA_TYPE             = DATADataTypeSpec::getSqlDataTypeStr( pStatement->getData( 16 ).toInt() );
        pDataTypeSpec->SQL_DATETIME_SUB          = DATADataTypeSpec::getSqlDateTimeSubStr( pStatement->getData( 17 ).toInt() );
        pDataTypeSpec->NUM_PREC_RADIX            = pStatement->getData( 18 ).toString();
        pDataTypeSpec->INTERVAL_PRECISION        = pStatement->getData( 19 ).toString();
        pDataTypeSpec->vectorSyntax              = DATADataTypeSpec::getSyntaxVector( DATADataTypeSpec::getSyntaxString( pDataTypeSpec->TYPE_NAME ) );

        if ( mapDataTypes.contains( pDataTypeSpec->TYPE_NAME ) )
        {
            printf( "[PAH][%s][%s][%d] TYPE_NAME %s exists. Ignoring.\n", __FILE__, __FUNCTION__, __LINE__, pDataTypeSpec->TYPE_NAME.toUtf8().constData() );
            delete pDataTypeSpec;
        }
        else 
            mapDataTypes[pDataTypeSpec->TYPE_NAME] = pDataTypeSpec;

        nReturn = pStatement->doFetch();
    }

    delete pStatement;

    return mapDataTypes;
}

QMap<int,DATAFunction*> DATAProfile::getFunctions( DATAConnection *pConnection, SQLRETURN *pnReturn )
{
    QMap<int,DATAFunction*> m;

    SQLRETURN nReturn;
    if ( !pnReturn ) pnReturn = &nReturn;

    if ( !pConnection->isAlloc() )
    {
        *pnReturn = SQL_ERROR;
        return m;
    }

    SQLUSMALLINT n[SQL_API_ODBC3_ALL_FUNCTIONS_SIZE];
    *pnReturn = pConnection->getFunctions( SQL_API_ODBC3_ALL_FUNCTIONS, n );

    if ( !SQL_SUCCEEDED( *pnReturn ) ) return m;

    // the string is dup as we can lookup in global g_Constants
    m[SQL_API_SQLALLOCHANDLE] = new DATAFunction( "SQL_API_SQLALLOCHANDLE", SQL_API_SQLALLOCHANDLE, SQL_FUNC_EXISTS( n, SQL_API_SQLALLOCHANDLE ), CBD::StandardISO92 );
    m[SQL_API_SQLBINDCOL] = new DATAFunction( "SQL_API_SQLBINDCOL", SQL_API_SQLBINDCOL, SQL_FUNC_EXISTS( n, SQL_API_SQLBINDCOL ), CBD::StandardISO92 );
    m[SQL_API_SQLCANCEL] = new DATAFunction( "SQL_API_SQLCANCEL", SQL_API_SQLCANCEL, SQL_FUNC_EXISTS( n, SQL_API_SQLCANCEL ), CBD::StandardISO92 );
    m[SQL_API_SQLCLOSECURSOR] = new DATAFunction( "SQL_API_SQLCLOSECURSOR", SQL_API_SQLCLOSECURSOR, SQL_FUNC_EXISTS( n, SQL_API_SQLCLOSECURSOR ), CBD::StandardISO92 );
    m[SQL_API_SQLCOLATTRIBUTE] = new DATAFunction( "SQL_API_SQLCOLATTRIBUTE", SQL_API_SQLCOLATTRIBUTE, SQL_FUNC_EXISTS( n, SQL_API_SQLCOLATTRIBUTE ), CBD::StandardISO92 );
    m[SQL_API_SQLCONNECT] = new DATAFunction( "SQL_API_SQLCONNECT", SQL_API_SQLCONNECT, SQL_FUNC_EXISTS( n, SQL_API_SQLCONNECT ), CBD::StandardISO92 );
    m[SQL_API_SQLCOPYDESC] = new DATAFunction( "SQL_API_SQLCOPYDESC", SQL_API_SQLCOPYDESC, SQL_FUNC_EXISTS( n, SQL_API_SQLCOPYDESC ), CBD::StandardISO92 );
    m[SQL_API_SQLDATASOURCES] = new DATAFunction( "SQL_API_SQLDATASOURCES", SQL_API_SQLDATASOURCES, SQL_FUNC_EXISTS( n, SQL_API_SQLDATASOURCES ), CBD::StandardISO92 );
    m[SQL_API_SQLDESCRIBECOL] = new DATAFunction( "SQL_API_SQLDESCRIBECOL", SQL_API_SQLDESCRIBECOL, SQL_FUNC_EXISTS( n, SQL_API_SQLDESCRIBECOL ), CBD::StandardISO92 );
    m[SQL_API_SQLDISCONNECT] = new DATAFunction( "SQL_API_SQLDISCONNECT", SQL_API_SQLDISCONNECT, SQL_FUNC_EXISTS( n, SQL_API_SQLDISCONNECT ), CBD::StandardISO92 );
    m[SQL_API_SQLDRIVERS] = new DATAFunction( "SQL_API_SQLDRIVERS", SQL_API_SQLDRIVERS, SQL_FUNC_EXISTS( n, SQL_API_SQLDRIVERS ), CBD::StandardISO92 );
    m[SQL_API_SQLENDTRAN] = new DATAFunction( "SQL_API_SQLENDTRAN", SQL_API_SQLENDTRAN, SQL_FUNC_EXISTS( n, SQL_API_SQLENDTRAN ), CBD::StandardISO92 );
    m[SQL_API_SQLEXECDIRECT] = new DATAFunction( "SQL_API_SQLEXECDIRECT", SQL_API_SQLEXECDIRECT, SQL_FUNC_EXISTS( n, SQL_API_SQLEXECDIRECT ), CBD::StandardISO92 );
    m[SQL_API_SQLEXECUTE] = new DATAFunction( "SQL_API_SQLEXECUTE", SQL_API_SQLEXECUTE, SQL_FUNC_EXISTS( n, SQL_API_SQLEXECUTE ), CBD::StandardISO92 );
    m[SQL_API_SQLFETCH] = new DATAFunction( "SQL_API_SQLFETCH", SQL_API_SQLFETCH, SQL_FUNC_EXISTS( n, SQL_API_SQLFETCH ), CBD::StandardISO92 );
    m[SQL_API_SQLFETCHSCROLL] = new DATAFunction( "SQL_API_SQLFETCHSCROLL", SQL_API_SQLFETCHSCROLL, SQL_FUNC_EXISTS( n, SQL_API_SQLFETCHSCROLL ), CBD::StandardISO92 );
    m[SQL_API_SQLFREEHANDLE] = new DATAFunction( "SQL_API_SQLFREEHANDLE", SQL_API_SQLFREEHANDLE, SQL_FUNC_EXISTS( n, SQL_API_SQLFREEHANDLE ), CBD::StandardISO92 );
    m[SQL_API_SQLFREESTMT] = new DATAFunction( "SQL_API_SQLFREESTMT", SQL_API_SQLFREESTMT, SQL_FUNC_EXISTS( n, SQL_API_SQLFREESTMT ), CBD::StandardISO92 );
    m[SQL_API_SQLGETCONNECTATTR] = new DATAFunction( "SQL_API_SQLGETCONNECTATTR", SQL_API_SQLGETCONNECTATTR, SQL_FUNC_EXISTS( n, SQL_API_SQLGETCONNECTATTR ), CBD::StandardISO92 );
    m[SQL_API_SQLGETCURSORNAME] = new DATAFunction( "SQL_API_SQLGETCURSORNAME", SQL_API_SQLGETCURSORNAME, SQL_FUNC_EXISTS( n, SQL_API_SQLGETCURSORNAME ), CBD::StandardISO92 );
    m[SQL_API_SQLGETDATA] = new DATAFunction( "SQL_API_SQLGETDATA", SQL_API_SQLGETDATA, SQL_FUNC_EXISTS( n, SQL_API_SQLGETDATA ), CBD::StandardISO92 );
    m[SQL_API_SQLGETDESCFIELD] = new DATAFunction( "SQL_API_SQLGETDESCFIELD", SQL_API_SQLGETDESCFIELD, SQL_FUNC_EXISTS( n, SQL_API_SQLGETDESCFIELD ), CBD::StandardISO92 );
    m[SQL_API_SQLGETDESCREC] = new DATAFunction( "SQL_API_SQLGETDESCREC", SQL_API_SQLGETDESCREC, SQL_FUNC_EXISTS( n, SQL_API_SQLGETDESCREC ), CBD::StandardISO92 );
    m[SQL_API_SQLGETDIAGFIELD] = new DATAFunction( "SQL_API_SQLGETDIAGFIELD", SQL_API_SQLGETDIAGFIELD, SQL_FUNC_EXISTS( n, SQL_API_SQLGETDIAGFIELD ), CBD::StandardISO92 );
    m[SQL_API_SQLGETDIAGREC] = new DATAFunction( "SQL_API_SQLGETDIAGREC", SQL_API_SQLGETDIAGREC, SQL_FUNC_EXISTS( n, SQL_API_SQLGETDIAGREC ), CBD::StandardISO92 );
    m[SQL_API_SQLGETENVATTR] = new DATAFunction( "SQL_API_SQLGETENVATTR", SQL_API_SQLGETENVATTR, SQL_FUNC_EXISTS( n, SQL_API_SQLGETENVATTR ), CBD::StandardISO92 );
    m[SQL_API_SQLGETFUNCTIONS] = new DATAFunction( "SQL_API_SQLGETFUNCTIONS", SQL_API_SQLGETFUNCTIONS, SQL_FUNC_EXISTS( n, SQL_API_SQLGETFUNCTIONS ), CBD::StandardISO92 );
    m[SQL_API_SQLGETINFO] = new DATAFunction( "SQL_API_SQLGETINFO", SQL_API_SQLGETINFO, SQL_FUNC_EXISTS( n, SQL_API_SQLGETINFO ), CBD::StandardISO92 );
    m[SQL_API_SQLGETSTMTATTR] = new DATAFunction( "SQL_API_SQLGETSTMTATTR", SQL_API_SQLGETSTMTATTR, SQL_FUNC_EXISTS( n, SQL_API_SQLGETSTMTATTR ), CBD::StandardISO92 );
    m[SQL_API_SQLGETTYPEINFO] = new DATAFunction( "SQL_API_SQLGETTYPEINFO", SQL_API_SQLGETTYPEINFO, SQL_FUNC_EXISTS( n, SQL_API_SQLGETTYPEINFO ), CBD::StandardISO92 );
    m[SQL_API_SQLNUMRESULTCOLS] = new DATAFunction( "SQL_API_SQLNUMRESULTCOLS", SQL_API_SQLNUMRESULTCOLS, SQL_FUNC_EXISTS( n, SQL_API_SQLNUMRESULTCOLS ), CBD::StandardISO92 );
    m[SQL_API_SQLPARAMDATA] = new DATAFunction( "SQL_API_SQLPARAMDATA", SQL_API_SQLPARAMDATA, SQL_FUNC_EXISTS( n, SQL_API_SQLPARAMDATA ), CBD::StandardISO92 );
    m[SQL_API_SQLPREPARE] = new DATAFunction( "SQL_API_SQLPREPARE", SQL_API_SQLPREPARE, SQL_FUNC_EXISTS( n, SQL_API_SQLPREPARE ), CBD::StandardISO92 );
    m[SQL_API_SQLPUTDATA] = new DATAFunction( "SQL_API_SQLPUTDATA", SQL_API_SQLPUTDATA, SQL_FUNC_EXISTS( n, SQL_API_SQLPUTDATA ), CBD::StandardISO92 );
    m[SQL_API_SQLROWCOUNT] = new DATAFunction( "SQL_API_SQLROWCOUNT", SQL_API_SQLROWCOUNT, SQL_FUNC_EXISTS( n, SQL_API_SQLROWCOUNT ), CBD::StandardISO92 );
    m[SQL_API_SQLSETCONNECTATTR] = new DATAFunction( "SQL_API_SQLSETCONNECTATTR", SQL_API_SQLSETCONNECTATTR, SQL_FUNC_EXISTS( n, SQL_API_SQLSETCONNECTATTR ), CBD::StandardISO92 );
    m[SQL_API_SQLSETCURSORNAME] = new DATAFunction( "SQL_API_SQLSETCURSORNAME", SQL_API_SQLSETCURSORNAME, SQL_FUNC_EXISTS( n, SQL_API_SQLSETCURSORNAME ), CBD::StandardISO92 );
    m[SQL_API_SQLSETDESCFIELD] = new DATAFunction( "SQL_API_SQLSETDESCFIELD", SQL_API_SQLSETDESCFIELD, SQL_FUNC_EXISTS( n, SQL_API_SQLSETDESCFIELD ), CBD::StandardISO92 );
    m[SQL_API_SQLSETDESCREC] = new DATAFunction( "SQL_API_SQLSETDESCREC", SQL_API_SQLSETDESCREC, SQL_FUNC_EXISTS( n, SQL_API_SQLSETDESCREC ), CBD::StandardISO92 );
    m[SQL_API_SQLSETENVATTR] = new DATAFunction( "SQL_API_SQLSETENVATTR", SQL_API_SQLSETENVATTR, SQL_FUNC_EXISTS( n, SQL_API_SQLSETENVATTR ), CBD::StandardISO92 );
    m[SQL_API_SQLSETSTMTATTR] = new DATAFunction( "SQL_API_SQLSETSTMTATTR", SQL_API_SQLSETSTMTATTR, SQL_FUNC_EXISTS( n, SQL_API_SQLSETSTMTATTR ), CBD::StandardISO92 );
    m[SQL_API_SQLCOLUMNS] = new DATAFunction( "SQL_API_SQLCOLUMNS", SQL_API_SQLCOLUMNS, SQL_FUNC_EXISTS( n, SQL_API_SQLCOLUMNS ), CBD::StandardOpenGroup );
    m[SQL_API_SQLSPECIALCOLUMNS] = new DATAFunction( "SQL_API_SQLSPECIALCOLUMNS", SQL_API_SQLSPECIALCOLUMNS, SQL_FUNC_EXISTS( n, SQL_API_SQLSPECIALCOLUMNS ), CBD::StandardOpenGroup );
    m[SQL_API_SQLSTATISTICS] = new DATAFunction( "SQL_API_SQLSTATISTICS", SQL_API_SQLSTATISTICS, SQL_FUNC_EXISTS( n, SQL_API_SQLSTATISTICS ), CBD::StandardOpenGroup );
    m[SQL_API_SQLTABLES] = new DATAFunction( "SQL_API_SQLTABLES", SQL_API_SQLTABLES, SQL_FUNC_EXISTS( n, SQL_API_SQLTABLES ), CBD::StandardOpenGroup );
    m[SQL_API_SQLBINDPARAMETER] = new DATAFunction( "SQL_API_SQLBINDPARAMETER", SQL_API_SQLBINDPARAMETER, SQL_FUNC_EXISTS( n, SQL_API_SQLBINDPARAMETER ), CBD::StandardODBC );
    m[SQL_API_SQLBROWSECONNECT] = new DATAFunction( "SQL_API_SQLBROWSECONNECT", SQL_API_SQLBROWSECONNECT, SQL_FUNC_EXISTS( n, SQL_API_SQLBROWSECONNECT ), CBD::StandardODBC );
    m[SQL_API_SQLBULKOPERATIONS] = new DATAFunction( "SQL_API_SQLBULKOPERATIONS", SQL_API_SQLBULKOPERATIONS, SQL_FUNC_EXISTS( n, SQL_API_SQLBULKOPERATIONS ), CBD::StandardODBC );
    m[SQL_API_SQLCOLUMNPRIVILEGES] = new DATAFunction( "SQL_API_SQLCOLUMNPRIVILEGES", SQL_API_SQLCOLUMNPRIVILEGES, SQL_FUNC_EXISTS( n, SQL_API_SQLCOLUMNPRIVILEGES ), CBD::StandardODBC );
    m[SQL_API_SQLDESCRIBEPARAM] = new DATAFunction( "SQL_API_SQLDESCRIBEPARAM", SQL_API_SQLDESCRIBEPARAM, SQL_FUNC_EXISTS( n, SQL_API_SQLDESCRIBEPARAM ), CBD::StandardODBC );
    m[SQL_API_SQLDRIVERCONNECT] = new DATAFunction( "SQL_API_SQLDRIVERCONNECT", SQL_API_SQLDRIVERCONNECT, SQL_FUNC_EXISTS( n, SQL_API_SQLDRIVERCONNECT ), CBD::StandardODBC );
    m[SQL_API_SQLFOREIGNKEYS] = new DATAFunction( "SQL_API_SQLFOREIGNKEYS", SQL_API_SQLFOREIGNKEYS, SQL_FUNC_EXISTS( n, SQL_API_SQLFOREIGNKEYS ), CBD::StandardODBC );
    m[SQL_API_SQLMORERESULTS] = new DATAFunction( "SQL_API_SQLMORERESULTS", SQL_API_SQLMORERESULTS, SQL_FUNC_EXISTS( n, SQL_API_SQLMORERESULTS ), CBD::StandardODBC );
    m[SQL_API_SQLNATIVESQL] = new DATAFunction( "SQL_API_SQLNATIVESQL", SQL_API_SQLNATIVESQL, SQL_FUNC_EXISTS( n, SQL_API_SQLNATIVESQL ), CBD::StandardODBC );
    m[SQL_API_SQLNUMPARAMS] = new DATAFunction( "SQL_API_SQLNUMPARAMS", SQL_API_SQLNUMPARAMS, SQL_FUNC_EXISTS( n, SQL_API_SQLNUMPARAMS ), CBD::StandardODBC );
    m[SQL_API_SQLPRIMARYKEYS] = new DATAFunction( "SQL_API_SQLPRIMARYKEYS", SQL_API_SQLPRIMARYKEYS, SQL_FUNC_EXISTS( n, SQL_API_SQLPRIMARYKEYS ), CBD::StandardODBC );
    m[SQL_API_SQLPROCEDURECOLUMNS] = new DATAFunction( "SQL_API_SQLPROCEDURECOLUMNS", SQL_API_SQLPROCEDURECOLUMNS, SQL_FUNC_EXISTS( n, SQL_API_SQLPROCEDURECOLUMNS ), CBD::StandardODBC );
    m[SQL_API_SQLPROCEDURES] = new DATAFunction( "SQL_API_SQLPROCEDURES", SQL_API_SQLPROCEDURES, SQL_FUNC_EXISTS( n, SQL_API_SQLPROCEDURES ), CBD::StandardODBC );
    m[SQL_API_SQLSETPOS] = new DATAFunction( "SQL_API_SQLSETPOS", SQL_API_SQLSETPOS, SQL_FUNC_EXISTS( n, SQL_API_SQLSETPOS ), CBD::StandardODBC );
    m[SQL_API_SQLTABLEPRIVILEGES] = new DATAFunction( "SQL_API_SQLTABLEPRIVILEGES", SQL_API_SQLTABLEPRIVILEGES, SQL_FUNC_EXISTS( n, SQL_API_SQLTABLEPRIVILEGES ), CBD::StandardODBC );
    m[SQL_API_SQLCANCELHANDLE] = new DATAFunction( "SQL_API_SQLCANCELHANDLE", SQL_API_SQLCANCELHANDLE, SQL_FUNC_EXISTS( n, SQL_API_SQLCANCELHANDLE ), CBD::StandardODBC );

    return m;
}

QMap<QString,DATAAttr*> DATAProfile::getAttr( DATAConnection *pConnection )
{
    QMap<QString,DATAAttr*> mapAttr;

    Q_ASSERT( pConnection->isConnected() );

    mapAttr["SQL_ATTR_ACCESS_MODE"] = getAttrData( pConnection, SQL_ATTR_ACCESS_MODE );
#ifdef Q_OS_WIN
    mapAttr["SQL_ATTR_ASYNC_DBC_EVENT"] = getAttrData( pConnection, SQL_ATTR_ASYNC_DBC_EVENT );
#endif
    mapAttr["SQL_ATTR_ASYNC_DBC_FUNCTIONS_ENABLE"] = getAttrData( pConnection, SQL_ATTR_ASYNC_DBC_FUNCTIONS_ENABLE );
    mapAttr["SQL_ATTR_ASYNC_ENABLE"] = getAttrData( pConnection, SQL_ATTR_ASYNC_ENABLE );
    mapAttr["SQL_ATTR_AUTO_IPD"] = getAttrData( pConnection, SQL_ATTR_AUTO_IPD );
    mapAttr["SQL_ATTR_AUTOCOMMIT"] = getAttrData( pConnection, SQL_ATTR_AUTOCOMMIT );
    mapAttr["SQL_ATTR_CONNECTION_DEAD"] = getAttrData( pConnection, SQL_ATTR_CONNECTION_DEAD );
    mapAttr["SQL_ATTR_CONNECTION_TIMEOUT"] = getAttrData( pConnection, SQL_ATTR_CONNECTION_TIMEOUT );
    mapAttr["SQL_ATTR_CURRENT_CATALOG"] = getAttrData( pConnection, SQL_ATTR_CURRENT_CATALOG );
    mapAttr["SQL_ATTR_ENLIST_IN_DTC"] = getAttrData( pConnection, SQL_ATTR_ENLIST_IN_DTC );
    mapAttr["SQL_ATTR_LOGIN_TIMEOUT"] = getAttrData( pConnection, SQL_ATTR_LOGIN_TIMEOUT );
    mapAttr["SQL_ATTR_METADATA_ID"] = getAttrData( pConnection, SQL_ATTR_METADATA_ID );
    mapAttr["SQL_ATTR_ODBC_CURSORS"] = getAttrData( pConnection, SQL_ATTR_ODBC_CURSORS );
    mapAttr["SQL_ATTR_PACKET_SIZE"] = getAttrData( pConnection, SQL_ATTR_PACKET_SIZE );
    mapAttr["SQL_ATTR_QUIET_MODE"] = getAttrData( pConnection, SQL_ATTR_QUIET_MODE );
    mapAttr["SQL_ATTR_TRACE"] = getAttrData( pConnection, SQL_ATTR_TRACE );
    mapAttr["SQL_ATTR_TRACEFILE"] = getAttrData( pConnection, SQL_ATTR_TRACEFILE );
    mapAttr["SQL_ATTR_TRANSLATE_LIB"] = getAttrData( pConnection, SQL_ATTR_TRANSLATE_LIB );
    mapAttr["SQL_ATTR_TRANSLATE_OPTION"] = getAttrData( pConnection, SQL_ATTR_TRANSLATE_OPTION );
    mapAttr["SQL_ATTR_TXN_ISOLATION"] = getAttrData( pConnection, SQL_ATTR_TXN_ISOLATION );

    return mapAttr;
}

DATAAttr *DATAProfile::getAttrData( DATAConnection *pConnection, SQLUSMALLINT nAttrType, SQLRETURN *pnReturn )
{
    SQLRETURN nReturn;
    if ( !pnReturn ) pnReturn = &nReturn;

    DATAAttr *pAttrParent = new DATAAttr();    // this
    pAttrParent->nAttr    = nAttrType;
    pAttrParent->vValue   = pConnection->getAttr( nAttrType, pnReturn );

    DATAAttr *pAttrChild;     // temp for creating value list

    switch ( nAttrType )
    {
        // simple value
#ifdef Q_OS_WIN
        case SQL_ATTR_ASYNC_DBC_EVENT:
            pAttrParent->stringAttr = "SQL_ATTR_ASYNC_DBC_EVENT";
            pAttrParent->stringDescription = QObject::tr( "" );
            break;
#endif
        case SQL_ATTR_CONNECTION_TIMEOUT:
            pAttrParent->stringAttr = "SQL_ATTR_CONNECTION_TIMEOUT";
            pAttrParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_ATTR_CURRENT_CATALOG:
            pAttrParent->stringAttr = "SQL_ATTR_CURRENT_CATALOG";
            pAttrParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_ATTR_ENLIST_IN_DTC:
            pAttrParent->stringAttr = "SQL_ATTR_ENLIST_IN_DTC";
            pAttrParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_ATTR_LOGIN_TIMEOUT:
            pAttrParent->stringAttr = "SQL_ATTR_LOGIN_TIMEOUT";
            pAttrParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_ATTR_PACKET_SIZE:
            pAttrParent->stringAttr = "SQL_ATTR_PACKET_SIZE";
            pAttrParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_ATTR_QUIET_MODE:
            pAttrParent->stringAttr = "SQL_ATTR_QUIET_MODE";
            pAttrParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_ATTR_TRACEFILE:
            pAttrParent->stringAttr = "SQL_ATTR_TRACEFILE";
            pAttrParent->stringDescription = QObject::tr( "" );
            break;
        case SQL_ATTR_TRANSLATE_LIB:
            pAttrParent->stringAttr = "SQL_ATTR_TRANSLATE_LIB";
            pAttrParent->stringDescription = QObject::tr( "" );
            break;
        // SINGULAR VALUE
        // - list all possible values and flag 1 of them (if any) with 'Y'
        case SQL_ATTR_ACCESS_MODE:
            pAttrParent->stringAttr = "SQL_ATTR_ACCESS_MODE";
            pAttrParent->stringDescription = QObject::tr( "" );

            pAttrChild = new DATAAttr(); pAttrChild->nAttr = SQL_MODE_READ_ONLY;
            pAttrChild->stringAttr = "SQL_MODE_READ_ONLY";
            pAttrChild->stringDescription = QObject::tr( "" );
            pAttrChild->vValue = (!pAttrParent->vValue.isNull() && pAttrParent->vValue.toUInt() == SQL_MODE_READ_ONLY ? "Y" : "" );
            pAttrParent->mapValues.insert( pAttrChild->stringAttr, pAttrChild );

            pAttrChild = new DATAAttr(); pAttrChild->nAttr = SQL_MODE_READ_WRITE;
            pAttrChild->stringAttr = "SQL_MODE_READ_WRITE";
            pAttrChild->stringDescription = QObject::tr( "" );
            pAttrChild->vValue = (!pAttrParent->vValue.isNull() && pAttrParent->vValue.toUInt() == SQL_MODE_READ_WRITE ? "Y" : "" );
            pAttrParent->mapValues.insert( pAttrChild->stringAttr, pAttrChild );
            break;
        case SQL_ATTR_ASYNC_DBC_FUNCTIONS_ENABLE:
            pAttrParent->stringAttr = "SQL_ATTR_ASYNC_DBC_FUNCTIONS_ENABLE";
            pAttrParent->stringDescription = QObject::tr( "" );

            pAttrChild = new DATAAttr(); pAttrChild->nAttr = SQL_ASYNC_DBC_ENABLE_ON;
            pAttrChild->stringAttr = "SQL_ASYNC_DBC_ENABLE_ON";
            pAttrChild->stringDescription = QObject::tr( "" );
            pAttrChild->vValue = (!pAttrParent->vValue.isNull() && pAttrParent->vValue.toUInt() == SQL_ASYNC_DBC_ENABLE_ON ? "Y" : "" );
            pAttrParent->mapValues.insert( pAttrChild->stringAttr, pAttrChild );

            pAttrChild = new DATAAttr(); pAttrChild->nAttr = SQL_ASYNC_DBC_ENABLE_OFF;
            pAttrChild->stringAttr = "SQL_ASYNC_DBC_ENABLE_OFF";
            pAttrChild->stringDescription = QObject::tr( "" );
            pAttrChild->vValue = (!pAttrParent->vValue.isNull() && pAttrParent->vValue.toUInt() == SQL_ASYNC_DBC_ENABLE_OFF ? "Y" : "" );
            pAttrParent->mapValues.insert( pAttrChild->stringAttr, pAttrChild );
            break;
        case SQL_ATTR_ASYNC_ENABLE:
            pAttrParent->stringAttr = "SQL_ATTR_ASYNC_ENABLE";
            pAttrParent->stringDescription = QObject::tr( "" );

            pAttrChild = new DATAAttr(); pAttrChild->nAttr = SQL_ASYNC_ENABLE_ON;
            pAttrChild->stringAttr = "SQL_ASYNC_ENABLE_ON";
            pAttrChild->stringDescription = QObject::tr( "" );
            pAttrChild->vValue = (!pAttrParent->vValue.isNull() && pAttrParent->vValue.toUInt() == SQL_ASYNC_ENABLE_ON ? "Y" : "" );
            pAttrParent->mapValues.insert( pAttrChild->stringAttr, pAttrChild );

            pAttrChild = new DATAAttr(); pAttrChild->nAttr = SQL_ASYNC_ENABLE_OFF;
            pAttrChild->stringAttr = "SQL_ASYNC_ENABLE_OFF";
            pAttrChild->stringDescription = QObject::tr( "" );
            pAttrChild->vValue = (!pAttrParent->vValue.isNull() && pAttrParent->vValue.toUInt() == SQL_ASYNC_ENABLE_OFF ? "Y" : "" );
            pAttrParent->mapValues.insert( pAttrChild->stringAttr, pAttrChild );
            break;
        case SQL_ATTR_AUTO_IPD:
            pAttrParent->stringAttr = "SQL_ATTR_AUTO_IPD";
            pAttrParent->stringDescription = QObject::tr( "" );

            pAttrChild = new DATAAttr(); pAttrChild->nAttr = SQL_TRUE;
            pAttrChild->stringAttr = "SQL_TRUE";
            pAttrChild->stringDescription = QObject::tr( "" );
            pAttrChild->vValue = (!pAttrParent->vValue.isNull() && pAttrParent->vValue.toUInt() == SQL_TRUE ? "Y" : "" );
            pAttrParent->mapValues.insert( pAttrChild->stringAttr, pAttrChild );

            pAttrChild = new DATAAttr(); pAttrChild->nAttr = SQL_FALSE;
            pAttrChild->stringAttr = "SQL_FALSE";
            pAttrChild->stringDescription = QObject::tr( "" );
            pAttrChild->vValue = (!pAttrParent->vValue.isNull() && pAttrParent->vValue.toUInt() == SQL_FALSE ? "Y" : "" );
            pAttrParent->mapValues.insert( pAttrChild->stringAttr, pAttrChild );
            break;
        case SQL_ATTR_AUTOCOMMIT:
            pAttrParent->stringAttr = "SQL_ATTR_AUTOCOMMIT";
            pAttrParent->stringDescription = QObject::tr( "" );

            pAttrChild = new DATAAttr(); pAttrChild->nAttr = SQL_AUTOCOMMIT_ON;
            pAttrChild->stringAttr = "SQL_AUTOCOMMIT_ON";
            pAttrChild->stringDescription = QObject::tr( "" );
            pAttrChild->vValue = (!pAttrParent->vValue.isNull() && pAttrParent->vValue.toUInt() == SQL_AUTOCOMMIT_ON ? "Y" : "" );
            pAttrParent->mapValues.insert( pAttrChild->stringAttr, pAttrChild );

            pAttrChild = new DATAAttr(); pAttrChild->nAttr = SQL_AUTOCOMMIT_OFF;
            pAttrChild->stringAttr = "SQL_AUTOCOMMIT_OFF";
            pAttrChild->stringDescription = QObject::tr( "" );
            pAttrChild->vValue = (!pAttrParent->vValue.isNull() && pAttrParent->vValue.toUInt() == SQL_AUTOCOMMIT_OFF ? "Y" : "" );
            pAttrParent->mapValues.insert( pAttrChild->stringAttr, pAttrChild );
            break;
        case SQL_ATTR_CONNECTION_DEAD:
            pAttrParent->stringAttr = "SQL_ATTR_CONNECTION_DEAD";
            pAttrParent->stringDescription = QObject::tr( "" );

            pAttrChild = new DATAAttr(); pAttrChild->nAttr = SQL_CD_TRUE;
            pAttrChild->stringAttr = "SQL_CD_TRUE";
            pAttrChild->stringDescription = QObject::tr( "" );
            pAttrChild->vValue = (!pAttrParent->vValue.isNull() && pAttrParent->vValue.toUInt() == SQL_CD_TRUE ? "Y" : "" );
            pAttrParent->mapValues.insert( pAttrChild->stringAttr, pAttrChild );

            pAttrChild = new DATAAttr(); pAttrChild->nAttr = SQL_CD_FALSE;
            pAttrChild->stringAttr = "SQL_CD_FALSE";
            pAttrChild->stringDescription = QObject::tr( "" );
            pAttrChild->vValue = (!pAttrParent->vValue.isNull() && pAttrParent->vValue.toUInt() == SQL_CD_FALSE ? "Y" : "" );
            pAttrParent->mapValues.insert( pAttrChild->stringAttr, pAttrChild );
            break;
        case SQL_ATTR_METADATA_ID:
            pAttrParent->stringAttr = "SQL_ATTR_METADATA_ID";
            pAttrParent->stringDescription = QObject::tr( "" );

            pAttrChild = new DATAAttr(); pAttrChild->nAttr = SQL_TRUE;
            pAttrChild->stringAttr = "SQL_TRUE";
            pAttrChild->stringDescription = QObject::tr( "" );
            pAttrChild->vValue = (!pAttrParent->vValue.isNull() && pAttrParent->vValue.toUInt() == SQL_TRUE ? "Y" : "" );
            pAttrParent->mapValues.insert( pAttrChild->stringAttr, pAttrChild );

            pAttrChild = new DATAAttr(); pAttrChild->nAttr = SQL_FALSE;
            pAttrChild->stringAttr = "SQL_FALSE";
            pAttrChild->stringDescription = QObject::tr( "" );
            pAttrChild->vValue = (!pAttrParent->vValue.isNull() && pAttrParent->vValue.toUInt() == SQL_FALSE ? "Y" : "" );
            pAttrParent->mapValues.insert( pAttrChild->stringAttr, pAttrChild );
            break;
        case SQL_ATTR_ODBC_CURSORS:
            pAttrParent->stringAttr = "SQL_ATTR_ODBC_CURSORS";
            pAttrParent->stringDescription = QObject::tr( "" );

// deprecated on MS
#ifndef Q_OS_WIN
            pAttrChild = new DATAAttr(); pAttrChild->nAttr = SQL_CUR_USE_IF_NEEDED;
            pAttrChild->stringAttr = "SQL_CUR_USE_IF_NEEDED";
            pAttrChild->stringDescription = QObject::tr( "" );
            pAttrChild->vValue = (!pAttrParent->vValue.isNull() && pAttrParent->vValue.toUInt() == SQL_CUR_USE_IF_NEEDED ? "Y" : "" );
            pAttrParent->mapValues.insert( pAttrChild->stringAttr, pAttrChild );

            pAttrChild = new DATAAttr(); pAttrChild->nAttr = SQL_CUR_USE_ODBC;
            pAttrChild->stringAttr = "SQL_CUR_USE_ODBC";
            pAttrChild->stringDescription = QObject::tr( "" );
            pAttrChild->vValue = (!pAttrParent->vValue.isNull() && pAttrParent->vValue.toUInt() == SQL_CUR_USE_ODBC ? "Y" : "" );
            pAttrParent->mapValues.insert( pAttrChild->stringAttr, pAttrChild );
#endif
            pAttrChild = new DATAAttr(); pAttrChild->nAttr = SQL_CUR_USE_DRIVER;
            pAttrChild->stringAttr = "SQL_CUR_USE_DRIVER";
            pAttrChild->stringDescription = QObject::tr( "" );
            pAttrChild->vValue = (!pAttrParent->vValue.isNull() && pAttrParent->vValue.toUInt() == SQL_CUR_USE_DRIVER ? "Y" : "" );
            pAttrParent->mapValues.insert( pAttrChild->stringAttr, pAttrChild );
            break;
        case SQL_ATTR_TRACE:
            pAttrParent->stringAttr = "SQL_ATTR_TRACE";
            pAttrParent->stringDescription = QObject::tr( "" );

            pAttrChild = new DATAAttr(); pAttrChild->nAttr = SQL_OPT_TRACE_OFF;
            pAttrChild->stringAttr = "SQL_OPT_TRACE_OFF";
            pAttrChild->stringDescription = QObject::tr( "" );
            pAttrChild->vValue = (!pAttrParent->vValue.isNull() && pAttrParent->vValue.toUInt() == SQL_OPT_TRACE_OFF ? "Y" : "" );
            pAttrParent->mapValues.insert( pAttrChild->stringAttr, pAttrChild );

            pAttrChild = new DATAAttr(); pAttrChild->nAttr = SQL_OPT_TRACE_ON;
            pAttrChild->stringAttr = "SQL_OPT_TRACE_ON";
            pAttrChild->stringDescription = QObject::tr( "" );
            pAttrChild->vValue = (!pAttrParent->vValue.isNull() && pAttrParent->vValue.toUInt() == SQL_OPT_TRACE_ON ? "Y" : "" );
            pAttrParent->mapValues.insert( pAttrChild->stringAttr, pAttrChild );
            break;
        // BITMASKS
        // - list all possible values and flag 0-n of them with 'Y'
        case SQL_ATTR_TRANSLATE_OPTION:
            pAttrParent->stringAttr = "SQL_ATTR_TRANSLATE_OPTION";
            pAttrParent->stringDescription = QObject::tr( "" );
            // possible values are translator dependent - just show the mask
            break;
        case SQL_ATTR_TXN_ISOLATION:
            pAttrParent->stringAttr = "SQL_ATTR_TXN_ISOLATION";
            pAttrParent->stringDescription = QObject::tr( "" );

            pAttrChild = new DATAAttr(); pAttrChild->nAttr = SQL_TXN_READ_UNCOMMITTED;
            pAttrChild->stringAttr = "SQL_TXN_READ_UNCOMMITTED";
            pAttrChild->stringDescription = QObject::tr( "" );
            pAttrChild->vValue = (!pAttrParent->vValue.isNull() && pAttrParent->vValue.toUInt() & SQL_TXN_READ_UNCOMMITTED ? "Y" : "" );
            pAttrParent->mapValues.insert( pAttrChild->stringAttr, pAttrChild );

            pAttrChild = new DATAAttr(); pAttrChild->nAttr = SQL_TXN_READ_COMMITTED;
            pAttrChild->stringAttr = "SQL_TXN_READ_COMMITTED";
            pAttrChild->stringDescription = QObject::tr( "" );
            pAttrChild->vValue = (!pAttrParent->vValue.isNull() && pAttrParent->vValue.toUInt() & SQL_TXN_READ_COMMITTED ? "Y" : "" );
            pAttrParent->mapValues.insert( pAttrChild->stringAttr, pAttrChild );

            pAttrChild = new DATAAttr(); pAttrChild->nAttr = SQL_TXN_REPEATABLE_READ;
            pAttrChild->stringAttr = "SQL_TXN_REPEATABLE_READ";
            pAttrChild->stringDescription = QObject::tr( "" );
            pAttrChild->vValue = (!pAttrParent->vValue.isNull() && pAttrParent->vValue.toUInt() & SQL_TXN_REPEATABLE_READ ? "Y" : "" );
            pAttrParent->mapValues.insert( pAttrChild->stringAttr, pAttrChild );

            pAttrChild = new DATAAttr(); pAttrChild->nAttr = SQL_TXN_SERIALIZABLE;
            pAttrChild->stringAttr = "SQL_TXN_SERIALIZABLE";
            pAttrChild->stringDescription = QObject::tr( "" );
            pAttrChild->vValue = (!pAttrParent->vValue.isNull() && pAttrParent->vValue.toUInt() & SQL_TXN_SERIALIZABLE ? "Y" : "" );
            pAttrParent->mapValues.insert( pAttrChild->stringAttr, pAttrChild );
            break;
        default:
            qDebug() << "[PAH]" << __FILE__ << __FUNCTION__  << __LINE__ << " AttrType: " << nAttrType << " not handlepAttrChild->";
            *pnReturn = SQL_ERROR;
            return pAttrParent;
    }

    return pAttrParent;
/*
    SQL_ATTR_ACCESS_MODE               
    SQL_ATTR_ASYNC_DBC_EVENT           
    SQL_ATTR_ASYNC_DBC_FUNCTIONS_ENABLE
    SQL_ATTR_ASYNC_DBC_PCALLBACK       
    SQL_ATTR_ASYNC_DBC_PCONTEXT        
    SQL_ATTR_ASYNC_ENABLE              
    SQL_ATTR_AUTO_IPD                  
    SQL_ATTR_AUTOCOMMIT                
    SQL_ATTR_CONNECTION_DEAD           
    SQL_ATTR_CONNECTION_TIMEOUT        
    SQL_ATTR_CURRENT_CATALOG           
    SQL_ATTR_ENLIST_IN_DTC             
    SQL_ATTR_LOGIN_TIMEOUT             
    SQL_ATTR_METADATA_ID               
    SQL_ATTR_ODBC_CURSORS              
    SQL_ATTR_PACKET_SIZE               
    SQL_ATTR_QUIET_MODE                
    SQL_ATTR_TRACE                     
    SQL_ATTR_TRACEFILE                 
    SQL_ATTR_TRANSLATE_LIB             
    SQL_ATTR_TRANSLATE_OPTION          
    SQL_ATTR_TXN_ISOLATION             
*/
}

QMap<QString,DATAInfo*> DATAProfile::getConvertValues( const QVariant &v )
{
    QMap<QString,DATAInfo*> l;

    if ( v.isNull() ) return l;

    uint n = v.toUInt();
    DATAInfo *pInfo;

    if ( n & SQL_CVT_BIGINT )
    {
        pInfo = new DATAInfo(); 
        pInfo->nInfo             = SQL_CVT_BIGINT;
        pInfo->stringInfo        = "SQL_CVT_BIGINT";
        pInfo->vValue            = "Y";
        pInfo->stringDescription = QObject::tr( "" );
        l.insert( pInfo->stringInfo, pInfo );
    }
    if ( n & SQL_CVT_BINARY )
    {
        pInfo = new DATAInfo(); 
        pInfo->nInfo             = SQL_CVT_BINARY;
        pInfo->stringInfo        = "SQL_CVT_BINARY";
        pInfo->vValue            = "Y";
        pInfo->stringDescription = QObject::tr( "" );
        l.insert( pInfo->stringInfo, pInfo );
    }
    if ( n & SQL_CVT_BIT )
    {
        pInfo = new DATAInfo(); 
        pInfo->nInfo             = SQL_CVT_BIT;
        pInfo->stringInfo        = "SQL_CVT_BIT";
        pInfo->vValue            = "Y";
        pInfo->stringDescription = QObject::tr( "" );
        l.insert( pInfo->stringInfo, pInfo );
    }
    if ( n & SQL_CVT_GUID )
    {
        pInfo = new DATAInfo(); 
        pInfo->nInfo             = SQL_CVT_GUID;
        pInfo->stringInfo        = "SQL_CVT_GUID";
        pInfo->vValue            = "Y";
        pInfo->stringDescription = QObject::tr( "" );
        l.insert( pInfo->stringInfo, pInfo );
    }
    if ( n & SQL_CVT_CHAR )
    {
        pInfo = new DATAInfo(); 
        pInfo->nInfo             = SQL_CVT_CHAR;
        pInfo->stringInfo        = "SQL_CVT_CHAR";
        pInfo->vValue            = "Y";
        pInfo->stringDescription = QObject::tr( "" );
        l.insert( pInfo->stringInfo, pInfo );
    }
    if ( n & SQL_CVT_DATE )
    {
        pInfo = new DATAInfo(); 
        pInfo->nInfo             = SQL_CVT_DATE;
        pInfo->stringInfo        = "SQL_CVT_DATE";
        pInfo->vValue            = "Y";
        pInfo->stringDescription = QObject::tr( "" );
        l.insert( pInfo->stringInfo, pInfo );
    }
    if ( n & SQL_CVT_DECIMAL )
    {
        pInfo = new DATAInfo(); 
        pInfo->nInfo             = SQL_CVT_DECIMAL;
        pInfo->stringInfo        = "SQL_CVT_DECIMAL";
        pInfo->vValue            = "Y";
        pInfo->stringDescription = QObject::tr( "" );
        l.insert( pInfo->stringInfo, pInfo );
    }
    if ( n & SQL_CVT_DOUBLE )
    {
        pInfo = new DATAInfo(); 
        pInfo->nInfo             = SQL_CVT_DOUBLE;
        pInfo->stringInfo        = "SQL_CVT_DOUBLE";
        pInfo->vValue            = "Y";
        pInfo->stringDescription = QObject::tr( "" );
        l.insert( pInfo->stringInfo, pInfo );
    }
    if ( n & SQL_CVT_FLOAT )
    {
        pInfo = new DATAInfo(); 
        pInfo->nInfo             = SQL_CVT_FLOAT;
        pInfo->stringInfo        = "SQL_CVT_FLOAT";
        pInfo->vValue            = "Y";
        pInfo->stringDescription = QObject::tr( "" );
        l.insert( pInfo->stringInfo, pInfo );
    }
    if ( n & SQL_CVT_INTEGER ) 
    {
        pInfo = new DATAInfo(); 
        pInfo->nInfo             = SQL_CVT_INTEGER;
        pInfo->stringInfo        = "SQL_CVT_INTEGER";
        pInfo->vValue            = "Y";
        pInfo->stringDescription = QObject::tr( "" );
        l.insert( pInfo->stringInfo, pInfo );
    }
    if ( n & SQL_CVT_INTERVAL_YEAR_MONTH )
    {
        pInfo = new DATAInfo(); 
        pInfo->nInfo             = SQL_CVT_INTERVAL_YEAR_MONTH;
        pInfo->stringInfo        = "SQL_CVT_INTERVAL_YEAR_MONTH";
        pInfo->vValue            = "Y";
        pInfo->stringDescription = QObject::tr( "" );
        l.insert( pInfo->stringInfo, pInfo );
    }
    if ( n & SQL_CVT_INTERVAL_DAY_TIME )
    {
        pInfo = new DATAInfo(); 
        pInfo->nInfo             = SQL_CVT_INTERVAL_DAY_TIME;
        pInfo->stringInfo        = "SQL_CVT_INTERVAL_DAY_TIME";
        pInfo->vValue            = "Y";
        pInfo->stringDescription = QObject::tr( "" );
        l.insert( pInfo->stringInfo, pInfo );
    }
    if ( n & SQL_CVT_LONGVARBINARY )
    {
        pInfo = new DATAInfo(); 
        pInfo->nInfo             = SQL_CVT_LONGVARBINARY;
        pInfo->stringInfo        = "SQL_CVT_LONGVARBINARY";
        pInfo->vValue            = "Y";
        pInfo->stringDescription = QObject::tr( "" );
        l.insert( pInfo->stringInfo, pInfo );
    }
    if ( n & SQL_CVT_LONGVARCHAR )
    {
        pInfo = new DATAInfo(); 
        pInfo->nInfo             = SQL_CVT_LONGVARCHAR;
        pInfo->stringInfo        = "SQL_CVT_LONGVARCHAR";
        pInfo->vValue            = "Y";
        pInfo->stringDescription = QObject::tr( "" );
        l.insert( pInfo->stringInfo, pInfo );
    }
    if ( n & SQL_CVT_NUMERIC )
    {
        pInfo = new DATAInfo(); 
        pInfo->nInfo             = SQL_CVT_NUMERIC;
        pInfo->stringInfo        = "SQL_CVT_NUMERIC";
        pInfo->vValue            = "Y";
        pInfo->stringDescription = QObject::tr( "" );
        l.insert( pInfo->stringInfo, pInfo );
    }
    if ( n & SQL_CVT_REAL )
    {
        pInfo = new DATAInfo(); 
        pInfo->nInfo             = SQL_CVT_REAL;
        pInfo->stringInfo        = "SQL_CVT_REAL";
        pInfo->vValue            = "Y";
        pInfo->stringDescription = QObject::tr( "" );
        l.insert( pInfo->stringInfo, pInfo );
    }
    if ( n & SQL_CVT_SMALLINT )
    {
        pInfo = new DATAInfo(); 
        pInfo->nInfo             = SQL_CVT_SMALLINT;
        pInfo->stringInfo        = "SQL_CVT_SMALLINT";
        pInfo->vValue            = "Y";
        pInfo->stringDescription = QObject::tr( "" );
        l.insert( pInfo->stringInfo, pInfo );
    }
    if ( n & SQL_CVT_TIME )
    {
        pInfo = new DATAInfo(); 
        pInfo->nInfo             = SQL_CVT_TIME;
        pInfo->stringInfo        = "SQL_CVT_TIME";
        pInfo->vValue            = "Y";
        pInfo->stringDescription = QObject::tr( "" );
        l.insert( pInfo->stringInfo, pInfo );
    }
    if ( n & SQL_CVT_TIMESTAMP )
    {
        pInfo = new DATAInfo(); 
        pInfo->nInfo             = SQL_CVT_TIMESTAMP;
        pInfo->stringInfo        = "SQL_CVT_TIMESTAMP";
        pInfo->vValue            = "Y";
        pInfo->stringDescription = QObject::tr( "" );
        l.insert( pInfo->stringInfo, pInfo );
    }
    if ( n & SQL_CVT_TINYINT )
    {
        pInfo = new DATAInfo(); 
        pInfo->nInfo             = SQL_CVT_TINYINT;
        pInfo->stringInfo        = "SQL_CVT_TINYINT";
        pInfo->vValue            = "Y";
        pInfo->stringDescription = QObject::tr( "" );
        l.insert( pInfo->stringInfo, pInfo );
    }
    if ( n & SQL_CVT_VARBINARY )
    {
        pInfo = new DATAInfo(); 
        pInfo->nInfo             = SQL_CVT_VARBINARY;
        pInfo->stringInfo        = "SQL_CVT_VARBINARY";
        pInfo->vValue            = "Y";
        pInfo->stringDescription = QObject::tr( "" );
        l.insert( pInfo->stringInfo, pInfo );
    }
    if ( n & SQL_CVT_VARCHAR )
    {
        pInfo = new DATAInfo(); 
        pInfo->nInfo             = SQL_CVT_VARCHAR;
        pInfo->stringInfo        = "SQL_CVT_VARCHAR";
        pInfo->vValue            = "Y";
        pInfo->stringDescription = QObject::QObject::tr( "" );
        l.insert( pInfo->stringInfo, pInfo );
    }

    return l;
}


