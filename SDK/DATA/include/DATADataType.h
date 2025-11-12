#ifndef H_DATADataType
#define H_DATADataType

#include "DATASystem.h"

/*!
 * \brief Basic elements needed to define a data-type. 
 *  
 * \author pharvey (5/27/20)
 */
class DATADataType
{
public:
    QString stringDataType;         // i.e. NUMERIC
    QString stringLength;           // n i.e. VARCHAR(n)
    QString stringPrecision1;       // p i.e. FLOAT(p)
    QString stringPrecision2;       // q i.e. INTERVAL DAY(p) TO SECOND(q)
    QString stringScale;            // s i.e. NUMERIC(p,s)
};

/*!
 * \class DATADataTypeSpec 
 * \brief Describes a supported data type. 
 *  
 * This holds information about a supported data-type. To be used for validation and when 
 * specifying a domain or column data-type.                                               
 *  
 * NOT for use in describing a result set column. 
 *  
 * This reflects the ODBC SQLGetTypeInfo result-set. 
 *  
 * This can be created without SQLGetTypeInfo, using implicit knowledge, to support case where; 
 *      - we want to deal with a specific set of data types (ODBC Minimum, SQL92, etc) 
 *      - we can not connect to a driver (perhaps not installed or configured)
 *      - we are dealing with an incomplete driver 
 *  
 * This can be extended (derive a new class from it) to hold additional fields for vendor specific 
 * features not supported by ODBC specification. 
 *  
 * \sa PMModel::mapDataTypes                                                              
 *  
 * \author pharvey (1/15/20)
 */
class DATADataTypeSpec
{
public:
    DATADataTypeSpec();

    static QString getDataTypeStr( int n );
    static QString getNullableStr( int n );
    static QString getCaseSensitiveStr( int n );
    static QString getSearchableStr( int n );
    static QString getUnsignedAttributeStr( int n );
    static QString getFixedPrecScaleStr( int n );
    static QString getAutoUniqueValueStr( int n );
    static QString getSqlDataTypeStr( int n );
    static QString getSqlDateTimeSubStr( int n );

    // Pass TYPE_NAME to this and get a version with standard tokens.
    // 
    // ie "NUMERIC()"           ---> "NUMERIC(p,s)" 
    //    "NUMERIC(xyz,xyz)"    ---> "NUMERIC(p,s)"
    // 
    // Works for standard data-types. Others should use some other method - and leave this alone.
    static QString getSyntaxString( const QString &stringTypeName );
    // Pass SyntaxString and get a parsed version in the form of a vector.
    // Used to load vectorSyntax.
    // Non-standard data-types can call this if they use the standard tokens (n,p,q,s).
    static QVector<QString> getSyntaxVector( const QString &stringSyntaxString );
    // construct DDL part from standard case
    // non-standard data-type? use some other way to build DDL part but leave this alone
    static QString getDataTypeWithArgs( DATADataTypeSpec *pDataTypeSpec, const DATADataType &DataType );

    bool isNull() { return TYPE_NAME.isEmpty(); }

    QString TYPE_NAME;              // Data-type name. ie "INTERVAL DAY() TO SECOND()" Used for DDL. 
    QString DATA_TYPE;              // SQL data type as text (more readable for display). 
    QString COLUMN_SIZE;            // LongLong: Max precision or chars. isNull if not relevant for data type.
    QString LITERAL_PREFIX;         // String: Prefix for a literal. ie "'" for a string. isNull if not relevant.
    QString LITERAL_SUFFIX;         // String: Suffix for a literal. ie "'" for a string. isNull if not relevant.
    QString CREATE_PARAMS;          // String: Comma seperated list of keywords/tokens. Keywords in order to be applied and in local language (for display). ie "precision,scale". isNull if no params.
    QString NULLABLE;               // SQL_NO_NULLS, SQL_NULLABLE, or SQL_NULLABLE_UNKNOWN
    QString CASE_SENSITIVE;         // Whether data (for sorting etc) is case sensitive. SQL_TRUE, or SQL_FALSE
    QString SEARCHABLE;             // How data type is used in a WHERE. SQL_PRED_NONE, SQL_PRED_CHAR, SQL_PRED_BASIC, or SQL_SEARCHABLE
    QString UNSIGNED_ATTRIBUTE;     // Int: Whether data is unsigned. SQL_TRUE or SQL_FALSE. isNull if not relevant.
    QString FIXED_PREC_SCALE;       // Whether data has predefined precision/scale. SQL_TRUE, or SQL_FALSE.
    QString AUTO_UNIQUE_VALUE;      // Int: Whether data is auto incrementing. SQL_TRUE, or SQL_FALSE. isNull if not relevant.
    QString LOCAL_TYPE_NAME;        // String: Localized name for data type (to be used for display only). isNull if not available. 
    QString MINIMUM_SCALE;          // Int: Min and Max will be same if fixed scale. isNull if not relevant.
    QString MAXIMUM_SCALE;          // Int: Min and Max will be same if fixed scale. isNull if not relevant.
    QString SQL_DATA_TYPE;          // Same as nDataType except for; interval and datetime.
    QString SQL_DATETIME_SUB;       // Int: datetime/interval sub code else isNull
    QString NUM_PREC_RADIX;         // LonLong: 2 = COLUMN_SIZE as bits, 10 = COLUMN_SIZE as decimal digits, else isNull
    QString INTERVAL_PRECISION;     // Int: Interval leading precision else isNull 

    int     nDATA_TYPE;
    int     nSQL_DATA_TYPE;

    QString stringDescription;      // HTML formatted description to help User understand data-type.

    // The following is used to;
    // 1. Provide syntax parts useful for UI when prompting for parameters (if any). Including; keywords and standardized parameter names.
    // 2. Provide syntax parts useful for constructing DDL using parameter values.

    // Parsed data-type syntax.
    // 
    // For example; 
    // 
    // "INTERVAL DAY(p) TO SECOND(q)" ---> {  "INTERVAL DAY(",
    //                                        "p", 
    //                                        ") TO SECOND(",
    //                                        "q",
    //                                        ")" }
    //
    // Note the token 'p' indicates that precision1 should be inserted at the location. The tokens are;
    // 
    //  n - length
    //  p - precision1
    //  q - precision2
    //  s - scale
    // 
    // This can come from parsing TYPE_NAME or set using intrinsic knowledge of the data source, driver or standard.
    QVector<QString> vectorSyntax;

    static QString             getSyntaxEncoded( DATADataTypeSpec * );
    static QVector<QString>    getSyntaxDecoded( const QString &stringEncoded );
};

#endif


