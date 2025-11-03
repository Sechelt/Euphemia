#include "LibInfo.h"
#include "DATAWMeta.h"

#include <DATAConnection.h>
#include <DATAStatement.h>
#include <DATAProfile.h>

//
// DATAWMetaResultColumn
//

//
// DATAWMetaResultColumns
//
/*!
 * \brief Load meta that can only be determined from a result-set ie from SQLColAttribute.
 *
 * This is done by generating a result-set with a DBMS specific form of "SELECT * FROM tb LIMIT 1".
 * Some DBMS's may not support any form of LIMIT. In such cases we could get all data from table
 * but instead we just make this a noop.
 * 
 * -- MySQL, H2, HSQLDB, Postgres, and SQLite
 * SELECT * FROM BOOK LIMIT 1 OFFSET 2
 *
 * -- CUBRID supports a MySQL variant of the LIMIT .. OFFSET clause
 * SELECT * FROM BOOK LIMIT 2, 1
 *
 * -- Derby, SQL Server 2012, Oracle 12c, SQL:2008 standard
 * -- Some need a mandatory ORDER BY clause prior to OFFSET
 * SELECT * FROM BOOK [ ORDER BY ... ] OFFSET 2 ROWS FETCH NEXT 1 ROWS ONLY
 * 
 * -- Ingres
 * SELECT * FROM BOOK OFFSET 2 FETCH FIRST 1 ROWS ONLY
 * 
 * -- Firebird
 * SELECT * FROM BOOK ROWS 2 TO 3
 *
 * -- Sybase SQL Anywhere
 * SELECT TOP 1 ROWS START AT 3 * FROM BOOK
 *
 * -- DB2 (without OFFSET)
 * SELECT * FROM BOOK FETCH FIRST 1 ROWS ONLY
 *
 * -- Sybase ASE, SQL Server 2008 (without OFFSET)
 * SELECT TOP 1 * FROM BOOK
 *
 * \note Current support is limited to what has been tested in dev environment.
 *
 * \author pharvey (3/5/25)
 */
bool DATAWMetaResultColumns::doLoad( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable, qsizetype nCols )
{
    Q_UNUSED(stringCatalog);
    Q_UNUSED(stringSchema);

    // DBMS specific SQL to limit result-set to just 1 row
    QString stringSQL;
    QString stringDBMS_NAME = pConnection->getInfo( SQL_DBMS_NAME, nullptr ).toString();
    // QString stringDBMS_VER  = pConnection->getInfo( SQL_DBMS_VER, &nReturn ).toString();
    if ( stringDBMS_NAME == "SQLite" || 
         stringDBMS_NAME == "MySQL" ||
         stringDBMS_NAME == "MariaDB" )
    {
        stringSQL = "SELECT * FROM " + stringTable + " LIMIT 1 OFFSET 2";
    }
    else if ( stringDBMS_NAME == "SQL Server" )
    {
        stringSQL = "SELECT * FROM " + stringTable + " OFFSET 2 ROWS FETCH NEXT 1 ROWS ONLY";
    }
    else if ( stringDBMS_NAME == "DB2" )
    {
        stringSQL = "SELECT * FROM " + stringTable + " FETCH FIRST 1 ROWS ONLY";
    }
    else return false;

    // create statement and bind to result-set columns
    DATAStatement *pStatement = new DATAStatement( pConnection );
    doInitBind( pStatement, nCols );

    // generate a result-set
    SQLRETURN nReturn = pStatement->slotExecDirect( stringSQL );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        qWarning( "[PAH][%s][%s][%d] Failed to execute %s\n", __FILE__, __FUNCTION__, __LINE__, stringSQL.toUtf8().constData() );
        doFiniBind();
        delete pStatement;
        return false;
    }

    // get result-set column meta data
    for ( int nCol = 1; nCol <= nCols; nCol++ )
    {
        vectorResultColumns << DATAWMetaResultColumn(   pStatement->getColAttribute( nCol, SQL_DESC_AUTO_UNIQUE_VALUE ).toInt(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_BASE_COLUMN_NAME ).toString(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_BASE_TABLE_NAME ).toString(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_CASE_SENSITIVE ).toInt(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_CATALOG_NAME ).toString(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_CONCISE_TYPE ).toInt(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_COUNT ).toInt(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_DISPLAY_SIZE ).toInt(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_FIXED_PREC_SCALE ).toInt(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_LABEL ).toString(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_LENGTH ).toInt(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_LITERAL_PREFIX ).toString(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_LITERAL_SUFFIX ).toString(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_LOCAL_TYPE_NAME ).toString(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_NAME ).toString(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_NULLABLE ).toInt(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_NUM_PREC_RADIX ).toInt(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_OCTET_LENGTH ).toInt(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_PRECISION ).toInt(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_SCALE ).toInt(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_SCHEMA_NAME ).toString(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_SEARCHABLE ).toInt(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_TABLE_NAME ).toString(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_TYPE ).toInt(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_TYPE_NAME ).toString(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_UNNAMED ).toInt(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_UNSIGNED ).toInt(),
                                                        pStatement->getColAttribute( nCol, SQL_DESC_UPDATABLE ).toInt() );
    }

    doFiniBind();
    delete pStatement;
    bNull = false;
    return true;
}

void DATAWMetaResultColumns::doInitBind( DATAStatement *pStatement, qsizetype nCols )
{
    aRowStatus[0] = SQL_ROW_IGNORE; // init to legit value just because

    if ( vectorRowBoundData.count() )
    {
        qWarning( "[PAH][%s][%s][%d] Buffer already allocated. Will renit.\n", __FILE__, __FUNCTION__, __LINE__ );
        doFiniBind();
    }

    // create buffers for a single row
    // for now... everything is a C-string with a 1k char max size
    for ( int nCol = 0; nCol < nCols; nCol++ )
    {
        // create bound buffers
        // - simple for now by not worrying about actual col type/size 
        char *  ps = (char*)malloc( sizeof(char) * 10000 * DATAWMetaResultColumnsRowArraySize );
        SQLLEN *pn = (SQLLEN*)malloc( sizeof(SQLLEN) * DATAWMetaResultColumnsRowArraySize );
        *pn = SQL_COLUMN_IGNORE;
        vectorRowBoundData << ps;
        vectorRowBoundDataInd << pn;
    }

    // bind cols
    // everything is a SQL_C_CHAR with a 1k char max size
    {
        SQLRETURN nReturn;
        int nCol, nIndex;

        for ( nCol = 1, nIndex = 0; nCol <= nCols; nCol++, nIndex++ )
        {
            nReturn = pStatement->doBindCol( nCol, SQL_C_CHAR, vectorRowBoundData[nIndex], 1000, vectorRowBoundDataInd[nIndex] );
            if ( !SQL_SUCCEEDED( nReturn ) ) qWarning( "[%s][%s][%d] ERROR: Failed to bind column %d. Return code: %d\n", __FILE__, __FUNCTION__, __LINE__, nCol, nReturn );
        }
    }

    //
    {
        SQLRETURN nReturn;

        nReturn = pStatement->setAttrCursorType( DATAStatement::CursorKeysetDriven );
        if ( !SQL_SUCCEEDED( nReturn ) ) qWarning( "[%s][%s][%d] WARNING: Failed to set DATAStatement::CursorKeysetDriven.\n", __FILE__, __FUNCTION__, __LINE__ );

        nReturn = pStatement->setAttrRowBindType( SQL_ATTR_ROW_BIND_TYPE, SQL_BIND_BY_COLUMN );
        if ( !SQL_SUCCEEDED( nReturn ) ) qWarning( "[%s][%s][%d] WARNING: Failed to set SQL_ATTR_ROW_BIND_TYPE.\n", __FILE__, __FUNCTION__, __LINE__ );

        nReturn = pStatement->setAttrRowArraySize( DATAWMetaResultColumnsRowArraySize );
        if ( !SQL_SUCCEEDED( nReturn ) ) qWarning( "[%s][%s][%d] WARNING: Failed to set DATAStatement::RowArraySize.\n", __FILE__, __FUNCTION__, __LINE__ );

        nReturn = pStatement->setAttrRowStatusPtr( SQL_ATTR_ROW_STATUS_PTR, aRowStatus );
        if ( !SQL_SUCCEEDED( nReturn ) ) qWarning( "[%s][%s][%d] WARNING: Failed to set SQL_ATTR_ROW_STATUS_PTR.\n", __FILE__, __FUNCTION__, __LINE__ );

        nReturn = pStatement->setAttrConcurrency( DATAStatement::ConcurLock );
        if ( !SQL_SUCCEEDED( nReturn ) ) qWarning( "[%s][%s][%d] WARNING: Failed to set DATAStatement::ConcurLock.\n", __FILE__, __FUNCTION__, __LINE__ );
    }
}

void DATAWMetaResultColumns::doFiniBind()
{
    char *ps;
    foreach( ps, vectorRowBoundData )
    {
        free( ps );
    }
    vectorRowBoundData.clear();

    SQLLEN *pn;
    foreach( pn, vectorRowBoundDataInd )
    {
        free( pn );
    }
    vectorRowBoundDataInd.clear();
}

//
// DATAWMetaColumn
//
bool DATAWMetaColumn::isString( SQLSMALLINT nType )
{
    switch ( nType )
    {
        case SQL_CHAR:
        case SQL_VARCHAR:
        case SQL_LONGVARCHAR:
        case SQL_WCHAR:
        case SQL_WVARCHAR:
        case SQL_WLONGVARCHAR:
//        case SQL_TYPE_DATE:
        case SQL_TYPE_TIME:
        case SQL_TYPE_TIMESTAMP:
//        case SQL_TYPE_UTCDATETIME:
//        case SQL_TYPE_UTCTIME:
//        case SQL_TYPE_DATE:
            return true;
    }
    return false;
}

bool DATAWMetaColumn::isInteger( SQLSMALLINT nType )
{
    switch ( nType )
    {
        case SQL_SMALLINT:
        case SQL_INTEGER:
        case SQL_BIT:
        case SQL_TINYINT:
        case SQL_BIGINT:
        case SQL_INTERVAL_MONTH:
        case SQL_INTERVAL_YEAR:
        case SQL_INTERVAL_YEAR_TO_MONTH:
        case SQL_INTERVAL_DAY:
        case SQL_INTERVAL_HOUR:
        case SQL_INTERVAL_MINUTE:
        case SQL_INTERVAL_SECOND:
        case SQL_INTERVAL_DAY_TO_HOUR:
//        case SQL_INTERVAL_DAY_TO_MINUTE:
        case SQL_INTERVAL_DAY_TO_SECOND:
        case SQL_INTERVAL_HOUR_TO_MINUTE:
        case SQL_INTERVAL_HOUR_TO_SECOND:
        case SQL_INTERVAL_MINUTE_TO_SECOND:
        case SQL_GUID:
            return true;
    }
    return false;
}

bool DATAWMetaColumn::isDecimal( SQLSMALLINT nType )
{
    switch ( nType )
    {
        case SQL_DECIMAL:
        case SQL_NUMERIC:
        case SQL_REAL:
        case SQL_FLOAT:
        case SQL_DOUBLE:
            return true;
    }
    return false;
}

bool DATAWMetaColumn::isBinary( SQLSMALLINT nType )
{
    switch ( nType )
    {
        case SQL_BINARY:
        case SQL_VARBINARY:
        case SQL_LONGVARBINARY:
            return true;
    }
    return false;
}

//
// DATAWMetaPrimaryKeyColumn
//

//
// DATAWMetaPrimaryKey
//
DATAWMetaPrimaryKey::DATAWMetaPrimaryKey( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable )
{
    doLoad( pConnection, stringCatalog, stringSchema, stringTable );
}

bool DATAWMetaPrimaryKey::doLoad( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable )
{
    mapColumns.clear();
    bNull = false;

    if ( !pConnection->isConnected() ) return false;

    DATAStatement *pStatement = pConnection->getPrimaryKeys( stringCatalog, stringSchema, stringTable );
    if ( !pStatement ) 
    {
        qWarning( "[PAH][%s][%s][%d] Failed to getPrimaryKeys. Looking for index named PrimaryKey.\n", __FILE__, __FUNCTION__, __LINE__ );

        // MS Access - may return as index named "PrimaryKey" 
        DATAStatement *pStatement = pConnection->getIndexs( stringCatalog, stringSchema, stringTable );
        if ( !pStatement ) 
        {
            qWarning( "[PAH][%s][%s][%d] Failed to getIndexs.\n", __FILE__, __FUNCTION__, __LINE__ );
            return false;
        }

        SQLRETURN n = pStatement->doFetch();
        while ( SQL_SUCCEEDED( n ) )
        {
            if ( pStatement->getData( 6 ).toString() == "PrimaryKey" )
            {
                QString stringColumn = pStatement->getData( 9 ).toString();
                qWarning( "[PAH][%s][%s][%d] Found index named PrimaryKey. Adding primary-key column %s.\n", __FILE__, __FUNCTION__, __LINE__, stringColumn.toUtf8().constData() );
                mapColumns.insert( stringColumn, DATAWMetaPrimaryKeyColumn( pStatement->getData( 5 ).toInt(), stringColumn ) );
            }
            n = pStatement->doFetch();
        }
        delete pStatement;

        bNull = false;

        return true;
    }

    // fetch primary keys
    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString stringColumn = pStatement->getData( 4 ).toString();

        mapColumns.insert( stringColumn, DATAWMetaPrimaryKeyColumn( pStatement->getData( 5 ).toInt(), stringColumn ) );
        stringPK_NAME = pStatement->getData( 6 ).toString();

        n = pStatement->doFetch();
    }
    delete pStatement;

    bNull = false;

    return true;
}

//
// DATAWMetaForeignKeyColumn
//

//
// DATAWMetaForeignKey
//

//
// DATAWMetaForeignKeys
//
DATAWMetaForeignKeys::DATAWMetaForeignKeys( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable )
{
    doLoad( pConnection, stringCatalog, stringSchema, stringTable );
}

bool DATAWMetaForeignKeys::doLoad( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable )
{
    mapForeignKeys.clear();
    bNull = true;

    if ( !pConnection->isConnected() ) return false;

    // use DBMS specific methods as needed
    QString stringDBMS_NAME = pConnection->getInfo( SQL_DBMS_NAME, nullptr ).toString();
    if ( stringDBMS_NAME == "ACCESS" && doLoadAccess( pConnection, stringCatalog, stringSchema, stringTable ) )
        return true;

    // generic method
    DATAStatement *pStatement = pConnection->getForeignKeys( stringCatalog, stringSchema, stringTable );
    if ( !pStatement ) return false;

    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString stringFK_NAME = pStatement->getData( 12 ).toString();
        if ( !mapForeignKeys.contains( stringFK_NAME ) )
        {
            mapForeignKeys.insert( stringFK_NAME, DATAWMetaForeignKey( pStatement->getData( 1 ).toString(),
                                                                       pStatement->getData( 2 ).toString(), 
                                                                       pStatement->getData( 3 ).toString(),
                                                                       pStatement->getData( 10 ).toInt(),
                                                                       pStatement->getData( 11 ).toInt(),
                                                                       pStatement->getData( 12 ).toString(),
                                                                       pStatement->getData( 13 ).toString(),
                                                                       pStatement->getData( 14 ).toInt() ) );
        }
        QString stringFKCOLUMN_NAME = pStatement->getData( 8 ).toString();

        mapForeignKeys[stringFK_NAME].mapColumns.insert( stringFKCOLUMN_NAME, 
                                                         DATAWMetaForeignKeyColumn( pStatement->getData( 9 ).toInt(),
                                                                                    pStatement->getData( 4 ).toString(),
                                                                                    stringFKCOLUMN_NAME ) );
        n = pStatement->doFetch();
    }
    delete pStatement;

    bNull = false;
    return true;
}

bool DATAWMetaForeignKeys::hasColumn( const QString &stringColumn )
{
    QMapIterator<QString,DATAWMetaForeignKey> i( mapForeignKeys );
    while ( i.hasNext() ) 
    {
        i.next();
        if ( i.value().mapColumns.contains( stringColumn ) ) return true;
    }

    return false;
}

/*!
 * \brief Load foreign keys for MS Access.
 * 
 * The MS Access ODBC driver does not return foreign keys. To get them we must use MS Access specific solution.
 * 
 * Foreign keys are found in a system table called; MSysRelationships but it is, by default, hidden and read/write restricted.
 * To change this the User must go into the MS Access application and;
 * 
 *          1. un-hide system tables (its in the menu somewhere - depends upon version of MS Access)
 *          2. copy MSysRelationships to MSysRelationshipsCopy (click on it + copy + paste + rename copy)
 * 
 * Unfortunately; this must be done manually.
 * 
 * \note The copy will be out-of-date if changes are made to the foreign keys.
 * 
 * \author pharv (3/10/2025)
 * 
 * \param pConnection   .
 * \param stringCatalog . 
 * \param stringSchema  .  
 * \param stringTable   .
 * 
 * \return bool .   
 */
bool DATAWMetaForeignKeys::doLoadAccess( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable )
{
    QString stringSQL = "SELECT * FROM [MSysRelationshipsCopy] WHERE [MSysRelationshipsCopy].[szObject] = ";
    stringSQL += "'";
    stringSQL += stringTable;
    stringSQL += "'";
    stringSQL += " ORDER BY [szReferencedObject] ASC;";

    DATAStatement *pStatement = pConnection->getExecute( stringSQL );
    if ( !pStatement ) 
    {
        qWarning( "[PAH][%s][%s][%d] Query for ForeignKeys failed.\nDid you create a copy of MSysRelationships called MSysRelationshipsCopy to avoid access restriction on MSysRelationships?\n", __FILE__, __FUNCTION__, __LINE__ );
        return false;
    }

    QString stringPrimaryKeyTableName;
    QString stringForeignKeyName;
    SQLSMALLINT nKEY_SEQ = -1;
    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString stringFK_NAME = pStatement->getData( 8 ).toString();
        if ( !mapForeignKeys.contains( stringFK_NAME ) )
        {
            mapForeignKeys.insert( stringFK_NAME, DATAWMetaForeignKey( stringCatalog,                           // PKTABLE_CAT
                                                                       stringSchema,                            // PKTABLE_SCHEM
                                                                       pStatement->getData( 7 ).toString(),     // PKTABLE_NAME
                                                                       SQL_NO_ACTION,                           // UPDATE_RULE
                                                                       SQL_NO_ACTION,                           // DELETE_RULE
                                                                       stringFK_NAME,                           // FK_NAME
                                                                       QString(),                               // PK_NAME
                                                                       SQL_NOT_DEFERRABLE ) );                  // DEFERRABILITY
            nKEY_SEQ = -1;
        }

        QString stringFKCOLUMN_NAME = pStatement->getData( 4 ).toString();
        mapForeignKeys[stringFK_NAME].mapColumns.insert( stringFKCOLUMN_NAME, 
                                                         DATAWMetaForeignKeyColumn( ++nKEY_SEQ,                             // KEY_SEQ
                                                                                    pStatement->getData( 6 ).toString(),    // PKCOLUMN_NAME
                                                                                    stringFKCOLUMN_NAME ) );                // FKCOLUMN_NAME
        n = pStatement->doFetch();
    }
    delete pStatement;
    return true;
}

//
// DATAWMetaSpecialColumn
//

//
// DATAWMetaSpecialColumns
//
DATAWMetaSpecialColumns::DATAWMetaSpecialColumns( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable, SQLSMALLINT nIdentifierType )
{
    doLoad( pConnection, stringCatalog, stringSchema, stringTable, nIdentifierType );
}

bool DATAWMetaSpecialColumns::doLoad( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable, SQLSMALLINT nIdentifierType )
{
    mapColumns.clear();
    bNull = true;

    if ( !pConnection->isConnected() ) return false;

    DATAStatement *pStatement = pConnection->getSpecialColumns( stringCatalog, stringSchema, stringTable, nIdentifierType );
    if ( !pStatement ) return false;

    this->nIdentifierType = nIdentifierType;

    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        nSCOPE = pStatement->getData( 1 ).toInt();
        QString stringCOLUMN_NAME = pStatement->getData( 2 ).toString();
        mapColumns.insert(stringCOLUMN_NAME, DATAWMetaSpecialColumn(    stringCOLUMN_NAME,
                                                                        pStatement->getData( 3 ).toInt(),
                                                                        pStatement->getData( 4 ).toString().toUpper(),
                                                                        pStatement->getData( 5 ).toInt(),
                                                                        pStatement->getData( 6 ).toInt(),
                                                                        pStatement->getData( 7 ).toInt(),
                                                                        pStatement->getData( 8 ).toInt() ) );

        n = pStatement->doFetch();
    }
    delete pStatement;

    bNull = false;
    return true;
}

//
// DATAWMetaIndexColumn
//

//
// DATAWMetaIndex
//

//
// DATAWMetaIndexes
//
DATAWMetaIndexes::DATAWMetaIndexes( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable )
{
    doLoad( pConnection, stringCatalog, stringSchema, stringTable );
}

QString DATAWMetaIndexes::getUnique()
{
    if ( bNull ) return QString();

    QMapIterator<QString,DATAWMetaIndex> i( mapIndexes );
    while ( i.hasNext() ) 
    {
        i.next();
        if ( i.value().nNON_UNIQUE == SQL_FALSE ) return i.value().stringINDEX_NAME;
    }

    return QString();
}

bool DATAWMetaIndexes::doLoad( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable )
{
    mapIndexes.clear();
    bNull = true;

    if ( !pConnection->isConnected() ) return false;

    DATAStatement *pStatement = pConnection->getIndexs( stringCatalog, stringSchema, stringTable );
    if ( !pStatement ) return false;

    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString stringINDEX_NAME     = pStatement->getData( 6 ).toString();
        if ( !mapIndexes.contains( stringINDEX_NAME ) )
        {
            mapIndexes.insert( stringINDEX_NAME, DATAWMetaIndex( pStatement->getData( 4 ).toInt(),
                                                                 pStatement->getData( 5 ).toString(),
                                                                 stringINDEX_NAME,
                                                                 pStatement->getData( 7 ).toInt(),
                                                                 pStatement->getData( 11 ).toInt(),
                                                                 pStatement->getData( 12 ).toInt(),
                                                                 pStatement->getData( 13 ).toString() ) );
        }
        QString stringCOLUMN_NAME = pStatement->getData( 9 ).toString();
        mapIndexes[stringINDEX_NAME].mapColumns.insert( stringCOLUMN_NAME, 
                                                        DATAWMetaIndexColumn( pStatement->getData( 8 ).toInt(),
                                                                              stringCOLUMN_NAME,
                                                                              pStatement->getData( 10 ).toChar() ) );

        n = pStatement->doFetch();
    }

    delete pStatement;
    bNull = false;
    return true;
}

//
// DATAWMetaTable
//
DATAWMetaTable::DATAWMetaTable( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable, const QString &stringType )
{
    doLoad( pConnection, stringCatalog, stringSchema, stringTable, stringType );
}

QStringList DATAWMetaTable::getCompletionStrings() const
{
    QStringList listStrings;

    for ( auto i = mapColumns.cbegin(), end = mapColumns.cend(); i != end; ++i )
        listStrings << i.key();

    return listStrings;
}

bool DATAWMetaTable::doLoad( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable, const QString &stringType )
{
    mapColumns.clear();
    bNull = true;

    if ( !pConnection->isConnected() ) return false;

    this->stringTable = stringTable;
    this->stringType  = stringType;

    QString stringDBMS_NAME = pConnection->getInfo( SQL_DBMS_NAME, nullptr ).toString();
    QString stringDBMS_VER  = pConnection->getInfo( SQL_DBMS_VER, nullptr ).toString();

    bCheckAutoGen = true;

    // query for columns
    DATAStatement *pStatement = pConnection->getColumns( stringCatalog, stringSchema.isEmpty() ? QString() : stringSchema, stringTable );
    if ( !pStatement ) return false;

    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString stringCOLUMN_NAME = pStatement->getData( 4 ).toString();
        mapColumns.insert( stringCOLUMN_NAME, DATAWMetaColumn( stringCOLUMN_NAME,                               // const QString   &stringCOLUMN_NAME,               
                                                               pStatement->getData( 5 ).toInt(),                // SQLSMALLINT     nDATA_TYPE,        
                                                               pStatement->getData( 6 ).toString().toUpper(),   // const QString   &stringTYPE_NAME,  
                                                               pStatement->getData( 7 ).toInt(),                // SQLINTEGER      nCOLUMN_SIZE,      
                                                               pStatement->getData( 8 ).toInt(),                // SQLINTEGER      nBUFFER_LENGTH,    
                                                               pStatement->getData( 9 ).toInt(),                // SQLSMALLINT     nDECIMAL_DIGITS,   
                                                               pStatement->getData( 10 ).toInt(),               // SQLSMALLINT     nNUM_PREC_RADIX,   
                                                               pStatement->getData( 11 ).toInt(),               // SQLSMALLINT     nNULLABLE,         
                                                               pStatement->getData( 12 ).toString(),            // const QString   &stringREMARKS,    
                                                               pStatement->getData( 13 ).toString(),            // const QString   &stringCOLUMN_DEF, 
                                                               pStatement->getData( 14 ).toInt(),               // SQLSMALLINT     nSQL_DATA_TYPE,    
                                                               pStatement->getData( 15 ).toInt(),               // SQLSMALLINT     nSQL_DATETIME_SUB, 
                                                               pStatement->getData( 16 ).toInt(),               // SQLINTEGER      nCHAR_OCTET_LENGTH,
                                                               pStatement->getData( 17 ).toInt(),               // SQLINTEGER      nORDINAL_POSITION, 
                                                               pStatement->getData( 18 ).toString() ) );        // const QString   &stringIS_NULLABLE 
        vectorColumnNames << stringCOLUMN_NAME;
        n = pStatement->doFetch();
    }

    delete pStatement;

    metaPrimaryKey.doLoad( pConnection, stringCatalog, stringSchema, stringTable );
    metaForeignKeys.doLoad( pConnection, stringCatalog, stringSchema, stringTable );
    metaIndexes.doLoad( pConnection, stringCatalog, stringSchema, stringTable );
    metaBestRowId.doLoad( pConnection, stringCatalog, stringSchema, stringTable, SQL_BEST_ROWID );
    metaRowVer.doLoad( pConnection, stringCatalog, stringSchema, stringTable, SQL_ROWVER );

    // how to id a row
    if ( metaBestRowId.mapColumns.count() )
        nUnique = UniqueRowId; // this could be something like a rowid or be a pk
    else if ( metaPrimaryKey.mapColumns.count() )
        nUnique = UniquePrimaryKey;
    else if ( !metaIndexes.getUnique().isNull() )
        nUnique = UniqueIndex;
    else
        nUnique = UniqueNone;

    // this has to go at end because it may use some of the other meta data
    for ( int nCol = 0; nCol < vectorColumnNames.count(); nCol++ )
    {
        QString stringColumnName = vectorColumnNames.at( nCol );
        mapColumns[stringColumnName].bAutoGenerated = isColumnAutoGenerated( stringDBMS_NAME, stringDBMS_VER, pConnection, stringTable, stringColumnName );
        mapColumns[stringColumnName].bReadOnly = mapColumns[stringColumnName].bAutoGenerated;
    }

    bNull = false;
    return true;
}

/*!
 * \brief Determine if the value for a column is auto generated by DBMS during an INSERT.
 * 
 * Use to restrict edit on a column.
 * Use to determine where to get a new row-id from.
 * 
 * ODBC does not provide for this explicitly...
 * 
 *     - SQLColumns: It may be indicated by TYPE_NAME but does not support hidden columns.
 *     - SQLColumns: Some ODBC drivers (EasySoft) may add additional result-set columns but not always available and does not support hidden columns.
 *     - SQLColAttribute: SQL_DESC_AUTO_UNIQUE_VALUE but not reliable even when bound cols.
 *     - SQLColAttribute: SQL_DESC_UPDATABLE could be used to indicate read-only and could infer auto-generated but not reliable even when bound cols.
 * 
 * So we use DBMS specific techniques to get this.
 * See; isAG* methods for details.
 * 
 * \author pharvey (2/25/25)
 * 
 * \param stringColumn This may be any column from SQLColumns or a hidden column.
 * 
 * \return bool 
 */
bool DATAWMetaTable::isColumnAutoGenerated( const QString &stringDBMS_NAME, const QString &stringDBMS_VER, DATAConnection *pConnection, const QString &stringTable, const QString &stringColumn )
{
    Q_UNUSED( stringDBMS_VER );

    if ( !bCheckAutoGen ) return false;
     
//    if ( stringDBMS_NAME == "SQL Server" ) return isAGSQLServer( stringTable, stringColumn );
    if ( stringDBMS_NAME == "ACCESS" ) return isAGAccess( pConnection, stringTable, stringColumn );
    if ( stringDBMS_NAME == "MariaDB" ) return isAGMariaDB( pConnection, stringDBMS_NAME, stringTable, stringColumn );
    if ( stringDBMS_NAME == "MySQL" ) return isAGMySQL( pConnection, stringDBMS_NAME, stringTable, stringColumn );
    if ( stringDBMS_NAME == "SQLite" ) return isAGSQLite( pConnection, stringTable, stringColumn );
//    if ( stringDBMS_NAME == "DB2" ) return isAGDB2( stringTable, stringColumn );

    // limit warning to 1 message per table
    bCheckAutoGen = false;
    qWarning( "[PAH][%s][%s][%d] I do not know how to determine if %s.%s.%s is auto generated.\nReturning false.", __FILE__, __FUNCTION__, __LINE__, stringDBMS_NAME.toUtf8().constData(), stringTable.toUtf8().constData(), stringColumn.toUtf8().constData() );

    return false;
}

bool DATAWMetaTable::isAGSQLServer( const QString &stringTable, const QString &stringColumn )
{
    Q_UNUSED( stringTable );
    Q_UNUSED( stringColumn );

    return false;
}

/*!
 * \brief Determine if the value for a column is auto generated by DBMS during an INSERT.
 * 
 * IF SQLGetTypeInfo:TYPE_NAME eq 'COUNTER' THEN true
 * IF SQLGetTypeInfo:AUTO_UNIQUE_VALUE eq true THEN true
 * 
 * \author pharvey (3/7/2025)
 * 
 * \param stringTable  
 * \param stringColumn 
 * 
 * \return bool 
 */
bool DATAWMetaTable::isAGAccess( DATAConnection *pConnection, const QString &stringTable, const QString &stringColumn )
{
    Q_UNUSED( stringTable );

    // SQLColumns:TYPE_NAME
    DATAWMetaColumn metaColumn = mapColumns.value( stringColumn );
    if ( metaColumn.bNull )
    {
        qWarning( "[PAH][%s][%s][%d] Could not find %s in meta\n", __FILE__, __FUNCTION__, __LINE__, stringColumn.toUtf8().constData() );
        return false;
    }

    if ( metaColumn.stringTYPE_NAME == "COUNTER" ) return true;

    DATAProfile *pProfile = pConnection->getProfile();
    DATADataTypeSpec *pDataTypeSpec = pProfile->mapDataTypes.value( metaColumn.stringTYPE_NAME );
    if ( !pDataTypeSpec )
    {
        qWarning( "[PAH][%s][%s][%d] Could not find type name %s in profile.\n", __FILE__, __FUNCTION__, __LINE__, metaColumn.stringTYPE_NAME.toUtf8().constData() );
        return false;
    }

    // SQLGetTypeInfo:AUTO_UNIQUE_VALUE
    if ( pDataTypeSpec->AUTO_UNIQUE_VALUE.toInt() ) return true;

    return false;
}

/*!
 * \brief Determine the given column is auto-generated.
 * 
 *  Uses...
 * 
 *  Call to \sa isAGMySQL
 * 
 * \author pharvey (3/7/25)
 * 
 * \param stringTable  
 * \param stringColumn 
 * 
 * \return bool 
 */
bool DATAWMetaTable::isAGMariaDB( DATAConnection *pConnection, const QString &stringSchema, const QString &stringTable, const QString &stringColumn )
{
    return isAGMySQL( pConnection, stringSchema, stringTable, stringColumn );
}

/*!
 * \brief Determine the given column is auto-generated.
 * 
 *  Uses...
 * 
 *  select * from INFORMATION_SCHEMA.COLUMNS where TABLE_SCHEMA='yourschema' and TABLE_NAME='yourtable' and EXTRA like '%auto_increment%'
 * 
 * \author pharvey (3/7/25)
 * 
 * \param stringTable  
 * \param stringColumn 
 * 
 * \return bool 
 */
bool DATAWMetaTable::isAGMySQL( DATAConnection *pConnection, const QString &stringSchema, const QString &stringTable, const QString &stringColumn )
{
    QString stringSQL = "SELECT COUNT(*) FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA='" + stringSchema + "' and TABLE_NAME='" + stringTable + "' and COLUMN_NAME='" + stringColumn + "' and EXTRA like '%auto_increment%'";
    DATAStatement *pStatement = new DATAStatement( pConnection );
    SQLRETURN nReturn = pStatement->slotExecDirect( stringSQL );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        qWarning( "[PAH][%s][%s][%d] Failed to execute; %s\n", __FILE__, __FUNCTION__, __LINE__, stringSQL.toUtf8().constData() );
        delete pStatement;
        return false;
    }
    nReturn = pStatement->doFetch();
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
       qWarning( "[PAH][%s][%s][%d] Failed to fetch after; %s\n", __FILE__, __FUNCTION__, __LINE__, stringSQL.toUtf8().constData() );
       delete pStatement;
       return false;
    }

    if ( pStatement->getData( 1 ).toInt() )
    {
        delete pStatement;
        return true;
    }

    delete pStatement;

    return false;
}

/*!
 * \brief Determine the given column is auto-generated.
 * 
 *  Uses...
 *
 *      1. IF _ROWID_ THEN it is auto-generated column (this does not cause table to show up in sqlite_sequence)
 *      2. IF ( SELECT COUNT(*) FROM sqlite_sequence WHERE name='yourtable' ) THE PrimaryKey IS auto-generated (and is an alias for _ROWID_ where _ROWID_ exists)
 *
 * There are some exceptions so we assume...
 *
 *      - there can be 0-1 auto-generated columns
 *      - the only auto-generated column is one which is, or equates, to a rowid
 *      - the only _ROWID_ (or alias) column in a table is the hidden column (even when its possible to have a regular column named same and override)
 *      - a column declared INTEGER PRIMARY KEY becomes an alias for _ROWID_ and is auto-generated (even without AUTOINCREMENT) 
 *
 *  Also...
 *
 *      - that we may not want to manually set an auto-generated column (even when it is allowed)
 *
 *  NOTE:   If column declared 'INTEGER PRIMARY KEY' without AUTOINCREMENT (common) freed row-id values can be reused. This
 *          could cause a problem with a key-set cache and UPDATE.
 *
 *
 * For this reason it is important to understand PrimaryKey in SQLite. From SQLite documentation...
 * 
 * PRIMARY KEY
 *
 * Each table in SQLite may have at most one PRIMARY KEY. If the keywords PRIMARY KEY are added to a column definition, then the primary key
 * for the table consists of that single column. Or, if a PRIMARY KEY clause is specified as a table-constraint, then the primary key of the
 * table consists of the list of columns specified as part of the PRIMARY KEY clause. The PRIMARY KEY clause must contain only column names
 * — the use of expressions in an indexed-column of a PRIMARY KEY is not supported. An error is raised if more than one PRIMARY KEY clause
 * appears in a CREATE TABLE statement. The PRIMARY KEY is optional for ordinary tables but is required for WITHOUT ROWID tables.
 *
 * If a table has a single column primary key and the declared type of that column is "INTEGER" and the table is not a WITHOUT ROWID table,
 * then the column is known as an INTEGER PRIMARY KEY. See below for a description of the special properties and behaviors associated with
 * an INTEGER PRIMARY KEY.
 *
 * Each row in a table with a primary key must have a unique combination of values in its primary key columns. For the purposes of determining
 * the uniqueness of primary key values, NULL values are considered distinct from all other values, including other NULLs. If an INSERT or
 * UPDATE statement attempts to modify the table content so that two or more rows have identical primary key values, that is a constraint violation.
 *
 * According to the SQL standard, PRIMARY KEY should always imply NOT NULL. Unfortunately, due to a bug in some early versions, this is not
 * the case in SQLite. Unless the column is an INTEGER PRIMARY KEY or the table is a WITHOUT ROWID table or a STRICT table or the column is
 * declared NOT NULL, SQLite allows NULL values in a PRIMARY KEY column. SQLite could be fixed to conform to the standard, but doing so might
 * break legacy applications. Hence, it has been decided to merely document the fact that SQLite allows NULLs in most PRIMARY KEY columns.
 *
 * ROWIDs and the INTEGER PRIMARY KEY
 *
 * Except for WITHOUT ROWID tables, all rows within SQLite tables have a 64-bit signed integer key that uniquely identifies the row within
 * its table. This integer is usually called the "rowid". The rowid value can be accessed using one of the special case-independent names
 * "rowid", "oid", or "_rowid_" in place of a column name. If a table contains a user defined column named "rowid", "oid" or "_rowid_", then
 * that name always refers the explicitly declared column and cannot be used to retrieve the integer rowid value.
 *
 * The rowid (and "oid" and "_rowid_") is omitted in WITHOUT ROWID tables. WITHOUT ROWID tables are only available in SQLite version 3.8.2
 * (2013-12-06) and later. A table that lacks the WITHOUT ROWID clause is called a "rowid table".
 *
 * The data for rowid tables is stored as a B-Tree structure containing one entry for each table row, using the rowid value as the key. This
 * means that retrieving or sorting records by rowid is fast. Searching for a record with a specific rowid, or for all records with rowids
 * within a specified range is around twice as fast as a similar search made by specifying any other PRIMARY KEY or indexed value.
 *
 * With one exception noted below, if a rowid table has a primary key that consists of a single column and the declared type of that column
 * is "INTEGER" in any mixture of upper and lower case, then the column becomes an alias for the rowid. Such a column is usually referred to
 * as an "integer primary key". A PRIMARY KEY column only becomes an integer primary key if the declared type name is exactly "INTEGER". Other
 * integer type names like "INT" or "BIGINT" or "SHORT INTEGER" or "UNSIGNED INTEGER" causes the primary key column to behave as an ordinary
 * table column with integer affinity and a unique index, not as an alias for the rowid.
 *
 * The exception mentioned above is that if the declaration of a column with declared type "INTEGER" includes an "PRIMARY KEY DESC" clause, it
 * does not become an alias for the rowid and is not classified as an integer primary key. This quirk is not by design. It is due to a bug in
 * early versions of SQLite. But fixing the bug could result in backwards incompatibilities. Hence, the original behavior has been retained
 * (and documented) because odd behavior in a corner case is far better than a compatibility break. This means that the following three table
 * declarations all cause the column "x" to be an alias for the rowid (an integer primary key):
 *
 * CREATE TABLE t(x INTEGER PRIMARY KEY ASC, y, z);
 * CREATE TABLE t(x INTEGER, y, z, PRIMARY KEY(x ASC));
 * CREATE TABLE t(x INTEGER, y, z, PRIMARY KEY(x DESC));
 * But the following declaration does not result in "x" being an alias for the rowid:
 *
 * CREATE TABLE t(x INTEGER PRIMARY KEY DESC, y, z);
 * Rowid values may be modified using an UPDATE statement in the same way as any other column value can, either using one of the built-in
 * aliases ("rowid", "oid" or "_rowid_") or by using an alias created by an integer primary key. Similarly, an INSERT statement may provide
 * a value to use as the rowid for each row inserted. Unlike normal SQLite columns, an integer primary key or rowid column must contain integer
 * values. Integer primary key or rowid columns are not able to hold floating point values, strings, BLOBs, or NULLs.
 *
 * If an UPDATE statement attempts to set an integer primary key or rowid column to a NULL or blob value, or to a string or real value that
 * cannot be losslessly converted to an integer, a "datatype mismatch" error occurs and the statement is aborted. If an INSERT statement
 * attempts to insert a blob value, or a string or real value that cannot be losslessly converted to an integer into an integer primary key or
 * rowid column, a "datatype mismatch" error occurs and the statement is aborted.
 *
 * If an INSERT statement attempts to insert a NULL value into a rowid or integer primary key column, the system chooses an integer value to
 * use as the rowid automatically. A detailed description of how this is done is provided separately.
 *
 * The parent key of a foreign key constraint is not allowed to use the rowid. The parent key must use named columns only.
 *
 * 
 * \author pharvey (3/7/25)
 * 
 * \param stringTable  
 * \param stringColumn 
 * 
 * \return bool 
 */
bool DATAWMetaTable::isAGSQLite( DATAConnection *pConnection, const QString &stringTable, const QString &stringColumn )
{
    // col is hidden

    // the rowid (hidden column) has some alias's and all rowid names are case insensitive
    // NOTE: We assume no declared columns are using these names - even though they could!
    QStringList listHiddenColumnNames;
    listHiddenColumnNames << "_ROWID_" << "ROWID" << "_OID_" << "OID";

    if ( listHiddenColumnNames.contains( stringColumn, Qt::CaseInsensitive ) ) return true;

    // col in pk

    if ( metaPrimaryKey.bNull ) return false;
    if ( !metaPrimaryKey.mapColumns.keys().contains( stringColumn, Qt::CaseInsensitive ) ) return false;

    QString stringSQL = "SELECT COUNT(*) FROM sqlite_sequence WHERE name='" + stringTable + "'";
    DATAStatement *pStatement = new DATAStatement( pConnection );
    SQLRETURN nReturn = pStatement->slotExecDirect( stringSQL );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        qWarning( "[PAH][%s][%s][%d] Failed to execute; %s\n", __FILE__, __FUNCTION__, __LINE__, stringSQL.toUtf8().constData() );
        delete pStatement;
        return false;
    }
    nReturn = pStatement->doFetch();
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
       qWarning( "[PAH][%s][%s][%d] Failed to fetch after; %s\n", __FILE__, __FUNCTION__, __LINE__, stringSQL.toUtf8().constData() );
       delete pStatement;
       return false;
    }

    if ( pStatement->getData( 1 ).toInt() )
    {
        delete pStatement;
        return true;
    }

    delete pStatement;
    return false;
}

bool DATAWMetaTable::isAGDB2( const QString &stringTable, const QString &stringColumn )
{
    Q_UNUSED( stringTable );
    Q_UNUSED( stringColumn );

    return false;
}


//
// DATAWMetaSchema
//
DATAWMetaSchema::DATAWMetaSchema( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema )
{
    doLoad( pConnection, stringCatalog, stringSchema );
}

QStringList DATAWMetaSchema::getCompletionStrings() const
{
    QStringList listStrings;

    for ( auto i = mapTables.cbegin(), end = mapTables.cend(); i != end; ++i )
        listStrings << i.key() << i.value().getCompletionStrings();

    return listStrings;
}

bool DATAWMetaSchema::doLoad( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema )
{
    mapTables.clear();
    bNull = true;

    if ( !pConnection->isConnected() ) return false;

    DATAStatement *pStatement;
    pStatement = pConnection->getTables( stringCatalog, stringSchema.isEmpty() ? QString() : stringSchema );
    if ( !pStatement ) return false;

    this->stringSchema = stringSchema;

    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString stringTABLE_NAME = pStatement->getData( 3 ).toString();
        mapTables.insert( stringTABLE_NAME, DATAWMetaTable( pConnection, stringCatalog, stringSchema, stringTABLE_NAME, pStatement->getData( 4 ).toString() ) );
        n = pStatement->doFetch();
    }

    delete pStatement;

    bNull = false;
    return true;
}

//
// DATAWMetaCatalog
//
DATAWMetaCatalog::DATAWMetaCatalog( DATAConnection *pConnection, const QString &stringCatalog )
{
    doLoad( pConnection, stringCatalog );
}

QStringList DATAWMetaCatalog::getCompletionStrings() const
{
    QStringList listStrings;

    for ( auto i = mapSchemas.cbegin(), end = mapSchemas.cend(); i != end; ++i )
        listStrings << i.key() << i.value().getCompletionStrings();

    return listStrings;
}

bool DATAWMetaCatalog::doLoad( DATAConnection *pConnection, const QString &stringCatalog )
{
    mapSchemas.clear();
    bNull = true;

    if ( !pConnection->isConnected() ) return false;

    // query
    DATAStatement *pStatement = pConnection->getSchemas( stringCatalog );
    if ( !pStatement ) return false;

    this->stringCatalog = stringCatalog;

    // process results
    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString stringTABLE_SCHEM = pStatement->getData( 2 ).toString();
        mapSchemas.insert( stringTABLE_SCHEM, DATAWMetaSchema( pConnection, stringCatalog, stringTABLE_SCHEM ) );
        n = pStatement->doFetch();
    }
    delete pStatement;

    bNull = false;
    return true;
}

//
// DATAWMeta
//
DATAWMeta::DATAWMeta( DATAConnection *pConnection )
{
    doLoad( pConnection );
}

QStringList DATAWMeta::getCompletionStrings() const
{
    QStringList listStrings;

    for ( auto i = mapCatalogs.cbegin(), end = mapCatalogs.cend(); i != end; ++i )
        listStrings << i.key() << i.value().getCompletionStrings();

    return listStrings;
}

bool DATAWMeta::doLoad( DATAConnection *pConnection )
{
    mapCatalogs.clear();
    bNull = true;

    if ( !pConnection->isConnected() ) return false;

    DATAStatement *pStatement = pConnection->getCatalogs();
    if ( pStatement )
    {
        // process results
        SQLRETURN n = pStatement->doFetch();
        while ( SQL_SUCCEEDED( n ) )
        {
            QString stringTABLE_CAT = pStatement->getData( 1 ).toString();
            mapCatalogs.insert( stringTABLE_CAT, DATAWMetaCatalog( pConnection, stringTABLE_CAT ) );
            n = pStatement->doFetch();
        }
        delete pStatement;
    }

    bNull = false;
    return true;
}


