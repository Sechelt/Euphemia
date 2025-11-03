#ifndef H_DATAWMeta
#define H_DATAWMeta

#include <DATA.h>

#define DATAWMetaResultColumnsRowArraySize 1

class DATAConnection;
class DATAStatement;

/*!
 * \brief Column meta returned via SQLColAttribute/IRD.
 * 
 * Some column meta info that is determined after a result-set is generated.
 * 
 * \author pharvey (3/5/25)
 */
class DATAWMetaResultColumn
{
public:
    DATAWMetaResultColumn() {}
    DATAWMetaResultColumn(  SQLLEN          nDESC_AUTO_UNIQUE_VALUE,
                            const QString & stringDESC_BASE_COLUMN_NAME,
                            const QString & stringDESC_BASE_TABLE_NAME, 
                            SQLLEN          nDESC_CASE_SENSITIVE,       
                            const QString & stringDESC_CATALOG_NAME,    
                            SQLLEN          nDESC_CONCISE_TYPE,         
                            SQLLEN          nDESC_COUNT,                
                            SQLLEN          nDESC_DISPLAY_SIZE,         
                            SQLLEN          nDESC_FIXED_PREC_SCALE,     
                            const QString & stringDESC_LABEL,           
                            SQLLEN          nDESC_LENGTH,               
                            const QString & stringDESC_LITERAL_PREFIX,  
                            const QString & stringDESC_LITERAL_SUFFIX,  
                            const QString & stringDESC_LOCAL_TYPE_NAME, 
                            const QString & stringDESC_NAME,            
                            SQLLEN          nDESC_NULLABLE,             
                            SQLLEN          nDESC_NUM_PREC_RADIX,       
                            SQLLEN          nDESC_OCTET_LENGTH,         
                            SQLLEN          nDESC_PRECISION,            
                            SQLLEN          nDESC_SCALE,                
                            const QString & stringDESC_SCHEMA_NAME,     
                            SQLLEN          nDESC_SEARCHABLE,           
                            const QString & stringDESC_TABLE_NAME,      
                            SQLLEN          nDESC_TYPE,                 
                            const QString & stringDESC_TYPE_NAME,       
                            SQLLEN          nDESC_UNNAMED,              
                            SQLLEN          nDESC_UNSIGNED,             
                            SQLLEN          nDESC_UPDATABLE )
    {
        this->nDESC_AUTO_UNIQUE_VALUE    = nDESC_AUTO_UNIQUE_VALUE;    
        this->stringDESC_BASE_COLUMN_NAME= stringDESC_BASE_COLUMN_NAME;
        this->stringDESC_BASE_TABLE_NAME = stringDESC_BASE_TABLE_NAME; 
        this->nDESC_CASE_SENSITIVE       = nDESC_CASE_SENSITIVE;       
        this->stringDESC_CATALOG_NAME    = stringDESC_CATALOG_NAME;    
        this->nDESC_CONCISE_TYPE         = nDESC_CONCISE_TYPE;         
        this->nDESC_COUNT                = nDESC_COUNT;            
        this->nDESC_DISPLAY_SIZE         = nDESC_DISPLAY_SIZE;         
        this->nDESC_FIXED_PREC_SCALE     = nDESC_FIXED_PREC_SCALE;     
        this->stringDESC_LABEL           = stringDESC_LABEL;           
        this->nDESC_LENGTH               = nDESC_LENGTH;               
        this->stringDESC_LITERAL_PREFIX  = stringDESC_LITERAL_PREFIX;  
        this->stringDESC_LITERAL_SUFFIX  = stringDESC_LITERAL_SUFFIX;  
        this->stringDESC_LOCAL_TYPE_NAME = stringDESC_LOCAL_TYPE_NAME; 
        this->stringDESC_NAME            = stringDESC_NAME;            
        this->nDESC_NULLABLE             = nDESC_NULLABLE;             
        this->nDESC_NUM_PREC_RADIX       = nDESC_NUM_PREC_RADIX;       
        this->nDESC_OCTET_LENGTH         = nDESC_OCTET_LENGTH;         
        this->nDESC_PRECISION            = nDESC_PRECISION;            
        this->nDESC_SCALE                = nDESC_SCALE;                
        this->stringDESC_SCHEMA_NAME     = stringDESC_SCHEMA_NAME;     
        this->nDESC_SEARCHABLE           = nDESC_SEARCHABLE;           
        this->stringDESC_TABLE_NAME      = stringDESC_TABLE_NAME;      
        this->nDESC_TYPE                 = nDESC_TYPE;                 
        this->stringDESC_TYPE_NAME       = stringDESC_TYPE_NAME;       
        this->nDESC_UNNAMED              = nDESC_UNNAMED;              
        this->nDESC_UNSIGNED             = nDESC_UNSIGNED;             
        this->nDESC_UPDATABLE            = nDESC_UPDATABLE;            

        bNull = false;
    }

public:
    SQLLEN          nDESC_AUTO_UNIQUE_VALUE;
    QString         stringDESC_BASE_COLUMN_NAME;
    QString         stringDESC_BASE_TABLE_NAME;
    SQLLEN          nDESC_CASE_SENSITIVE;
    QString         stringDESC_CATALOG_NAME;
    SQLLEN          nDESC_CONCISE_TYPE;
    SQLLEN          nDESC_COUNT; 
    SQLLEN          nDESC_DISPLAY_SIZE;
    SQLLEN          nDESC_FIXED_PREC_SCALE;
    QString         stringDESC_LABEL;
    SQLLEN          nDESC_LENGTH;
    QString         stringDESC_LITERAL_PREFIX;
    QString         stringDESC_LITERAL_SUFFIX;
    QString         stringDESC_LOCAL_TYPE_NAME;
    QString         stringDESC_NAME;
    SQLLEN          nDESC_NULLABLE;
    SQLLEN          nDESC_NUM_PREC_RADIX;
    SQLLEN          nDESC_OCTET_LENGTH;
    SQLLEN          nDESC_PRECISION;
    SQLLEN          nDESC_SCALE;
    QString         stringDESC_SCHEMA_NAME;
    SQLLEN          nDESC_SEARCHABLE;
    QString         stringDESC_TABLE_NAME;
    SQLLEN          nDESC_TYPE;
    QString         stringDESC_TYPE_NAME;
    SQLLEN          nDESC_UNNAMED;
    SQLLEN          nDESC_UNSIGNED;
    SQLLEN          nDESC_UPDATABLE;

    //
    bool bNull = true;
};

/*!
 * \brief Column meta returned via SQLColAttribute/IRD.
 * 
 * Some column meta info that is determined after a result-set is generated.
 * 
 * doLoad( DATAConnection *, ... )
 * 
 * This will create a single-row result-set using DBMS specific SQL syntax.
 * The result columns are bound to get more meaningful values for;
 * 
 * doLoad( DATAStatement *, ... )
 * 
 * This will use the result-set in the statement.
 * 
 * \author pharvey (3/5/25)
 */
class DATAWMetaResultColumns
{
public:
    DATAWMetaResultColumns() {}

public:
    bool bNull = true;
    QVector<DATAWMetaResultColumn> vectorResultColumns; 

    bool doLoad( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable, qsizetype nCols );

protected:
    // bind buffers
    SQLUSMALLINT        aRowStatus[DATAWMetaResultColumnsRowArraySize]; // row status (1 per rows in data buffers)
    QVector<char*>      vectorRowBoundData;                             // row data (0-n cols) (single row data buffer)
    QVector<SQLLEN*>    vectorRowBoundDataInd;                          // row data ind (0-n cols) (single row data buffer)

    void doInitBind( DATAStatement *pStatement, qsizetype nCols );
    void doFiniBind();
};

// SQLColumns
class DATAWMetaColumn
{
public:
    DATAWMetaColumn() {}
    DATAWMetaColumn(    const QString   &stringCOLUMN_NAME,
                        SQLSMALLINT     nDATA_TYPE,
                        const QString   &stringTYPE_NAME,
                        SQLINTEGER      nCOLUMN_SIZE,
                        SQLINTEGER      nBUFFER_LENGTH,
                        SQLSMALLINT     nDECIMAL_DIGITS,
                        SQLSMALLINT     nNUM_PREC_RADIX,
                        SQLSMALLINT     nNULLABLE,
                        const QString   &stringREMARKS,
                        const QString   &stringCOLUMN_DEF,
                        SQLSMALLINT     nSQL_DATA_TYPE,
                        SQLSMALLINT     nSQL_DATETIME_SUB,
                        SQLINTEGER      nCHAR_OCTET_LENGTH,
                        SQLINTEGER      nORDINAL_POSITION,              // ORDINAL_POSITION (supposed to be 1 based but sqlite uses 0 based) reliable?
                        const QString   &stringIS_NULLABLE )
    {
        this->stringCOLUMN_NAME = stringCOLUMN_NAME; 
        this->nDATA_TYPE        = nDATA_TYPE;        
        this->stringTYPE_NAME   = stringTYPE_NAME;   
        this->nCOLUMN_SIZE      = nCOLUMN_SIZE;      
        this->nBUFFER_LENGTH    = nBUFFER_LENGTH;    
        this->nDECIMAL_DIGITS   = nDECIMAL_DIGITS;   
        this->nNUM_PREC_RADIX   = nNUM_PREC_RADIX;   
        this->nNULLABLE         = nNULLABLE;         
        this->stringREMARKS     = stringREMARKS;     
        this->stringCOLUMN_DEF  = stringCOLUMN_DEF;  
        this->nSQL_DATA_TYPE    = nSQL_DATA_TYPE;    
        this->nSQL_DATETIME_SUB = nSQL_DATETIME_SUB; 
        this->nCHAR_OCTET_LENGTH= nCHAR_OCTET_LENGTH; 
        this->nORDINAL_POSITION = nORDINAL_POSITION; 
        this->stringIS_NULLABLE = stringIS_NULLABLE;

        bNull = false;
    }

    // SQLColumns
    QString         stringCOLUMN_NAME;
    SQLSMALLINT     nDATA_TYPE;
    QString         stringTYPE_NAME;
    SQLINTEGER      nCOLUMN_SIZE;
    SQLINTEGER      nBUFFER_LENGTH;
    SQLSMALLINT     nDECIMAL_DIGITS;
    SQLSMALLINT     nNUM_PREC_RADIX;
    SQLSMALLINT     nNULLABLE;
    QString         stringREMARKS;
    QString         stringCOLUMN_DEF;
    SQLSMALLINT     nSQL_DATA_TYPE;
    SQLSMALLINT     nSQL_DATETIME_SUB;
    SQLINTEGER      nCHAR_OCTET_LENGTH;
    SQLINTEGER      nORDINAL_POSITION;
    QString         stringIS_NULLABLE;
    bool            bAutoGenerated  = false; // this gets set after all columns loaded
    bool            bReadOnly       = false; // ie IF bAutoGenerated THEN true but there could be other reasons

    //
    bool bNull = true;

    // These indicate what we should convert to when getting data from QVariant.
    static bool isString( SQLSMALLINT nType );
    static bool isDecimal( SQLSMALLINT nType );
    static bool isInteger( SQLSMALLINT nType );
    static bool isBinary( SQLSMALLINT nType );
};

// SQLPrimaryKeys
class DATAWMetaPrimaryKeyColumn
{
public:
    DATAWMetaPrimaryKeyColumn() {}
    DATAWMetaPrimaryKeyColumn( SQLSMALLINT n, const QString &s )
    {
        nKEY_SEQ = n;
        stringCOLUMN_NAME = s;
        bNull = false;
    }

    SQLSMALLINT     nKEY_SEQ;
    QString         stringCOLUMN_NAME;

    bool bNull = true;
};

class DATAWMetaPrimaryKey
{
public:
    DATAWMetaPrimaryKey() {}
    DATAWMetaPrimaryKey( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable );

    QString                                 stringPK_NAME;  // can be null
    QMap<QString,DATAWMetaPrimaryKeyColumn> mapColumns;     // key=COLUMN_NAME (alt KEY_SEQ)

    bool bNull = true;

    bool doLoad( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable );
};

// SQLForeignKeys
class DATAWMetaForeignKeyColumn
{
public:
    DATAWMetaForeignKeyColumn() {}
    DATAWMetaForeignKeyColumn( SQLSMALLINT n, const QString &stringPK, const QString &stringFK )
    {
        nKEY_SEQ = n;
        stringPKCOLUMN_NAME = stringPK;
        stringFKCOLUMN_NAME = stringFK;
        bNull = false;
    }

    SQLSMALLINT nKEY_SEQ;
    QString     stringPKCOLUMN_NAME;
    QString     stringFKCOLUMN_NAME;

    bool bNull = true;
};

class DATAWMetaForeignKey
{
public:
    DATAWMetaForeignKey() {}
    DATAWMetaForeignKey( const QString &stringPKTABLE_CAT,
                         const QString &stringPKTABLE_SCHEM,
                         const QString &stringPKTABLE_NAME,
                         SQLSMALLINT    nUPDATE_RULE,
                         SQLSMALLINT    nDELETE_RULE,
                         const QString &stringFK_NAME,
                         const QString &stringPK_NAME,
                         SQLSMALLINT    nDEFERRABILITY
                       )
    {
        this->stringPKTABLE_CAT     = stringPKTABLE_CAT;  
        this->stringPKTABLE_SCHEM   = stringPKTABLE_SCHEM;
        this->stringPKTABLE_NAME    = stringPKTABLE_NAME; 
        this->nUPDATE_RULE          = nUPDATE_RULE;       
        this->nDELETE_RULE          = nDELETE_RULE;       
        this->stringFK_NAME         = stringFK_NAME;      
        this->stringPK_NAME         = stringPK_NAME;      
        this->nDEFERRABILITY        = nDEFERRABILITY;
        bNull = false;     
    }

    QString                                     stringPKTABLE_CAT;
    QString                                     stringPKTABLE_SCHEM;
    QString                                     stringPKTABLE_NAME;
    SQLSMALLINT                                 nUPDATE_RULE;
    SQLSMALLINT                                 nDELETE_RULE;
    QString                                     stringFK_NAME;
    QString                                     stringPK_NAME;
    SQLSMALLINT                                 nDEFERRABILITY;
    QMap<QString,DATAWMetaForeignKeyColumn>     mapColumns; // 1-n, key=FK_NAME (alt KEY_SEQ)

    bool bNull = true;
};

class DATAWMetaForeignKeys
{
public:
    DATAWMetaForeignKeys() {}
    DATAWMetaForeignKeys( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable );

    QMap<QString,DATAWMetaForeignKey> mapForeignKeys; // 0-n, key=stringFK_NAME

    bool bNull = true;

    bool doLoad( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable );

    bool hasColumn( const QString &stringColumn );

protected:
    bool doLoadAccess( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable );

};

// SQLSpecialColumns
class DATAWMetaSpecialColumn
{
public:
    DATAWMetaSpecialColumn() {}
    DATAWMetaSpecialColumn( const QString &stringCOLUMN_NAME, SQLSMALLINT nDATA_TYPE, const QString &stringTYPE_NAME, SQLINTEGER nCOLUMN_SIZE, SQLINTEGER nBUFFER_LENGTH, SQLSMALLINT nDECIMAL_DIGITS, SQLSMALLINT nPSEUDO_COLUMN )
    {
        this->stringCOLUMN_NAME = stringCOLUMN_NAME; 
        this->nDATA_TYPE        = nDATA_TYPE;        
        this->stringTYPE_NAME   = stringTYPE_NAME;   
        this->nCOLUMN_SIZE      = nCOLUMN_SIZE;      
        this->nBUFFER_LENGTH    = nBUFFER_LENGTH;    
        this->nDECIMAL_DIGITS   = nDECIMAL_DIGITS;   
        this->nPSEUDO_COLUMN    = nPSEUDO_COLUMN;    
        bNull = false;
    }

    QString         stringCOLUMN_NAME;
    SQLSMALLINT     nDATA_TYPE;
    QString         stringTYPE_NAME;
    SQLINTEGER      nCOLUMN_SIZE;
    SQLINTEGER      nBUFFER_LENGTH;
    SQLSMALLINT     nDECIMAL_DIGITS;
    SQLSMALLINT     nPSEUDO_COLUMN;

    bool bNull = true;
};

class DATAWMetaSpecialColumns
{
public:
    DATAWMetaSpecialColumns() {}
    DATAWMetaSpecialColumns( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable, SQLSMALLINT nIdentifierType );

    SQLSMALLINT                             nIdentifierType;
    SQLSMALLINT                             nSCOPE;
    QMap<QString,DATAWMetaSpecialColumn>    mapColumns; // key=COLUMN_NAME (is this unique???)

    bool bNull = true;

    bool doLoad( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable, SQLSMALLINT nIdentifierType );
};

// SQLStatistics
class DATAWMetaIndexColumn
{
public:
    DATAWMetaIndexColumn() {}
    DATAWMetaIndexColumn( SQLSMALLINT nOrdinalPosition, const QString &stringColumnName, const QChar &charOrder )
    {
        nORDINAL_POSITION   = nOrdinalPosition;
        stringCOLUMN_NAME   = stringColumnName;
        charASC_OR_DESC     = charOrder;
    }

    SQLSMALLINT     nORDINAL_POSITION;
    QString         stringCOLUMN_NAME;
    QChar           charASC_OR_DESC;

    bool bNull = true;
};

class DATAWMetaIndex
{
public:
    DATAWMetaIndex() {}
    DATAWMetaIndex( SQLSMALLINT    nNON_UNIQUE,
                    const QString &stringINDEX_QUALIFIER,
                    const QString &stringINDEX_NAME,
                    SQLSMALLINT    nTYPE,
                    SQLINTEGER     nCARDINALITY,
                    SQLINTEGER     nPAGES,
                    const QString &stringFILTER_CONDITION )
    {
        this->nNON_UNIQUE               = nNON_UNIQUE;              
        this->stringINDEX_QUALIFIER     = stringINDEX_QUALIFIER;    
        this->stringINDEX_NAME          = stringINDEX_NAME;         
        this->nTYPE                     = nTYPE;                    
        this->nCARDINALITY              = nCARDINALITY;             
        this->nPAGES                    = nPAGES;                   
        this->stringFILTER_CONDITION    = stringFILTER_CONDITION;    
        bNull = false;
    }

    SQLSMALLINT                             nNON_UNIQUE;
    QString                                 stringINDEX_QUALIFIER;
    QString                                 stringINDEX_NAME;
    SQLSMALLINT                             nTYPE;
    SQLINTEGER                              nCARDINALITY;
    SQLINTEGER                              nPAGES;
    QString                                 stringFILTER_CONDITION;
    QMap<QString,DATAWMetaIndexColumn>      mapColumns;                 // 1-n, key=COLUMN_NAME

    bool bNull = true;
};

class DATAWMetaIndexes
{
public:
    DATAWMetaIndexes() {}
    DATAWMetaIndexes( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable );

    QMap<QString,DATAWMetaIndex> mapIndexes;  // 0-n, key=INDEX_NAME

    bool bNull = true;

    QString getUnique();

    bool doLoad( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable );
};

// SQLColumns etc
class DATAWMetaTable
{
public:
    enum Unique
    {
        UniqueNone,
        UniqueRowId,
        UniquePrimaryKey,
        UniqueIndex
    };

    DATAWMetaTable() {}
    DATAWMetaTable( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable, const QString &stringType );

    QString                             stringTable;            // TABLE_NAME
    QString                             stringType;             // TABLE_TYPE
    QVector<QString>                    vectorColumnNames;      // 0-n, n = mapColumns.count, so we can get column name using a column number
    QMap<QString,DATAWMetaColumn>       mapColumns;             // 0-n, key=COLUMN_NAME
    DATAWMetaResultColumns              metaResultColumns;
    DATAWMetaPrimaryKey                 metaPrimaryKey;         // 0-1
    DATAWMetaForeignKeys                metaForeignKeys;        // 0-n
    DATAWMetaIndexes                    metaIndexes;            // 0-n
    DATAWMetaSpecialColumns             metaBestRowId;          // 0-1
    DATAWMetaSpecialColumns             metaRowVer;             // 0-1
    Unique                              nUnique = UniqueNone;   // how we id a row

    bool bNull = true;                                      // true if failed to load

    QStringList getCompletionStrings() const;

    bool doLoad( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable, const QString &stringType );

protected:
    bool bCheckAutoGen = true;
    bool isColumnAutoGenerated( const QString &stringDBMS_NAME, const QString &stringDBMS_VER, DATAConnection *pConnection, const QString &stringTable, const QString &stringColumn );

    bool isAGSQLServer( const QString &stringTable, const QString &stringColumn );
    bool isAGAccess( DATAConnection *pConnection, const QString &stringTable, const QString &stringColumn );
    bool isAGMariaDB( DATAConnection *pConnection, const QString &stringSchema, const QString &stringTable, const QString &stringColumn );
    bool isAGMySQL( DATAConnection *pConnection, const QString &stringSchema, const QString &stringTable, const QString &stringColumn );
    bool isAGSQLite( DATAConnection *pConnection, const QString &stringTable, const QString &stringColumn );
    bool isAGDB2( const QString &stringTable, const QString &stringColumn );

};

// SQLTables - to get tables
class DATAWMetaSchema
{
public:
    DATAWMetaSchema() {}
    DATAWMetaSchema( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema );

    QString                         stringSchema;   // TABLE_SCHEM
    QMap<QString,DATAWMetaTable>    mapTables;      // 0-n, key=TABLE_NAME

    bool bNull = true;                              // true if failed to load

    QStringList getCompletionStrings() const;

    bool doLoad( DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema );
};


// SQLTables - to get schemas
class DATAWMetaCatalog
{
public:
    DATAWMetaCatalog() {}
    DATAWMetaCatalog( DATAConnection *pConnection, const QString &stringCatalog );

    QString                         stringCatalog;  // TABLE_CAT
    QMap<QString,DATAWMetaSchema>   mapSchemas;     // 0-n, key=TABLE_SCHEM

    bool bNull = true;

    QStringList getCompletionStrings() const;

    bool doLoad( DATAConnection *pConnection, const QString &stringCatalog );
};

// SQLTables - to get catalogs
class DATAWMeta
{
public:
    DATAWMeta() {}
    DATAWMeta( DATAConnection *pConnection );

    QMap<QString,DATAWMetaCatalog> mapCatalogs; // 0-n, key=TABLE_CAT

    bool bNull = true;                          // true if failed to load

    QStringList getCompletionStrings() const;

    bool doLoad( DATAConnection *pConnection );
};

#endif


