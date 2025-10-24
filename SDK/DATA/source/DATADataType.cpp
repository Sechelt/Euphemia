#include "LibInfo.h"
#include "DATADataType.h"

DATADataTypeSpec::DATADataTypeSpec() 
{
}

// SQL Data Types -> Appendix D: Data Types
QString DATADataTypeSpec::getDataTypeStr( int n )
{
    switch ( n )
    {
        case SQL_LONGVARCHAR:
            return "SQL_LONGVARCHAR";
        case SQL_BINARY:
            return "SQL_BINARY";
        case SQL_VARBINARY:
            return "SQL_VARBINARY";
        case SQL_LONGVARBINARY:
            return "SQL_LONGVARBINARY";
        case SQL_BIGINT:
            return "SQL_BIGINT";
        case SQL_TINYINT:
            return "SQL_TINYINT";
        case SQL_BIT:
            return "SQL_BIT";
        case SQL_WCHAR:
            return "SQL_WCHAR";
        case SQL_WVARCHAR:
            return "SQL_WVARCHAR";
        case SQL_WLONGVARCHAR:
            return "SQL_WLONGVARCHAR";
        case SQL_GUID:
            return "SQL_GUID";
        case SQL_CHAR:
            return "SQL_CHAR";
        case SQL_NUMERIC:
            return "SQL_NUMERIC";
        case SQL_DECIMAL:
            return "SQL_DECIMAL";
        case SQL_INTEGER:
            return "SQL_INTEGER";
        case SQL_SMALLINT:
            return "SQL_SMALLINT";
        case SQL_FLOAT:
            return "SQL_FLOAT";
        case SQL_REAL:
            return "SQL_REAL";
        case SQL_DOUBLE:
            return "SQL_DOUBLE";
        case SQL_VARCHAR:
            return "SQL_VARCHAR";
        case SQL_INTERVAL_YEAR:            
            return "SQL_INTERVAL_YEAR";
        case SQL_INTERVAL_MONTH:           
            return "SQL_INTERVAL_MONTH";
        case SQL_INTERVAL_YEAR_TO_MONTH:   
            return "SQL_INTERVAL_YEAR_TO_MONTH";
        case SQL_INTERVAL_DAY:
            return "SQL_INTERVAL_DAY";
        case SQL_INTERVAL_HOUR:
            return "SQL_INTERVAL_HOUR";
        case SQL_INTERVAL_MINUTE:
            return "SQL_INTERVAL_MINUTE";
        case SQL_INTERVAL_SECOND:
            return "SQL_INTERVAL_SECOND";
        case SQL_INTERVAL_DAY_TO_HOUR:
            return "SQL_INTERVAL_DAY_TO_HOUR";
        case SQL_INTERVAL_DAY_TO_MINUTE:
            return "SQL_INTERVAL_DAY_TO_MINUTE";
        case SQL_INTERVAL_DAY_TO_SECOND:
            return "SQL_INTERVAL_DAY_TO_SECOND";
        case SQL_INTERVAL_HOUR_TO_MINUTE:
            return "SQL_INTERVAL_HOUR_TO_MINUTE";
        case SQL_INTERVAL_HOUR_TO_SECOND:
            return "SQL_INTERVAL_HOUR_TO_SECOND";
        case SQL_INTERVAL_MINUTE_TO_SECOND:
            return "SQL_INTERVAL_MINUTE_TO_SECOND";
        case SQL_UNKNOWN_TYPE:
            return "SQL_UNKNOWN_TYPE";
        // ODBC v2
        case SQL_DATE:
            return "SQL_DATE";
        case SQL_TIME:
            return "SQL_TIME";
        case SQL_TIMESTAMP:
            return "SQL_TIMESTAMP";
        // ODBC v3
        case SQL_TYPE_DATE:             // 91
            return "SQL_TYPE_DATE";
        case SQL_TYPE_TIME:             // 92
            return "SQL_TYPE_TIME";
        case SQL_TYPE_TIMESTAMP:        // 93
            return "SQL_TYPE_TIMESTAMP";
    }

    return QString::number( n );
}

QString DATADataTypeSpec::getNullableStr( int n )
{
    switch ( n )
    {
        case SQL_NO_NULLS:
            return "SQL_NO_NULLS";
        case SQL_NULLABLE:
            return "SQL_NULLABLE";
    }
    return QString::number( n );
}

QString DATADataTypeSpec::getCaseSensitiveStr( int n )
{
    switch ( n )
    {
        case SQL_TRUE:
            return "SQL_TRUE";
        case SQL_FALSE:
            return "SQL_FALSE";
    }
    return QString::number( n );
}

QString DATADataTypeSpec::getSearchableStr( int n )
{
    switch ( n )
    {
        case SQL_PRED_NONE:
            return "SQL_PRED_NONE";
        case SQL_PRED_CHAR:
            return "SQL_PRED_CHAR";
        case SQL_PRED_BASIC:
            return "SQL_PRED_BASIC";
        case SQL_SEARCHABLE:
            return "SQL_SEARCHABLE";
    }
    return QString::number( n );
}

QString DATADataTypeSpec::getUnsignedAttributeStr( int n )
{
    switch ( n )
    {
        case SQL_TRUE:
            return "SQL_TRUE";
        case SQL_FALSE:
            return "SQL_FALSE";
    }
    return QString::number( n );
}

QString DATADataTypeSpec::getFixedPrecScaleStr( int n )
{
    switch ( n )
    {
        case SQL_TRUE:
            return "SQL_TRUE";
        case SQL_FALSE:
            return "SQL_FALSE";
    }
    return QString::number( n );
}

QString DATADataTypeSpec::getAutoUniqueValueStr( int n )
{
    switch ( n )
    {
        case SQL_TRUE:
            return "SQL_TRUE";
        case SQL_FALSE:
            return "SQL_FALSE";
    }
    return QString::number( n );
}

QString DATADataTypeSpec::getSqlDataTypeStr( int n )
{
    switch ( n )
    {
        case SQL_DATE:
        case SQL_TIME:
        case SQL_TIMESTAMP:
            return "SQL_DATETIME";
        case SQL_INTERVAL_YEAR:            
        case SQL_INTERVAL_MONTH:           
        case SQL_INTERVAL_YEAR_TO_MONTH:   
        case SQL_INTERVAL_DAY:
        case SQL_INTERVAL_HOUR:
        case SQL_INTERVAL_MINUTE:
        case SQL_INTERVAL_SECOND:
        case SQL_INTERVAL_DAY_TO_HOUR:
        case SQL_INTERVAL_DAY_TO_MINUTE:
        case SQL_INTERVAL_DAY_TO_SECOND:
        case SQL_INTERVAL_HOUR_TO_MINUTE:
        case SQL_INTERVAL_HOUR_TO_SECOND:
        case SQL_INTERVAL_MINUTE_TO_SECOND:
            return "SQL_INTERVAL";
    }

    return getDataTypeStr( n );
}

QString DATADataTypeSpec::getSqlDateTimeSubStr( int n )
{
    switch ( n )
    {
        case SQL_CODE_DATE:
            return "SQL_CODE_DATE";
        case SQL_CODE_TIME:
            return "SQL_CODE_TIME";
        case SQL_CODE_TIMESTAMP:
            return "SQL_CODE_TIMESTAMP";
    }
    return QString::number( n );
}

/*!
 * \brief Determine the data-type (without params/braces) and return syntax string for it using known format. 
 *  
 * An ODBC driver can return DATA_TYPE (stringTypeName) in different forms due to inconsistent implementations. 
 *  
 * Some use upper case and some use lower case. 
 * Some include braces (when params need) some don't. 
 * Some include tokens in the braces and some don't. 
 *  
 * We return the syntax in a string. It will include the type-name and any braces (with tokens) as needed. 
 * For example; 
 *  
 * "numeric"    ---> "NUMERIC(p,s)" 
 * "NUMERIC()"  ---> "NUMERIC(p,s)" 
 *  
 * \note This only works for standard data-types. 
 *  
 * \author pharvey (5/30/20)
 * 
 * \param stringTypeName 
 * 
 * \return QString 
 */
QString DATADataTypeSpec::getSyntaxString( const QString &stringTypeName )
{
    QString stringUpperCase = stringTypeName.toUpper();
    QString stringStripped;
    bool bInBraces = false;
    QChar c;
    foreach( c, stringUpperCase )
    {
        if ( c == '(' )
        {
            bInBraces = true;
            continue;
        }
        if ( c == ')' )
        {
            bInBraces = false;
            continue;
        }
        if ( bInBraces ) continue;

        stringStripped.append( c );
    }

    // catch types which require params
    if ( stringStripped == "CHAR" ) return "CHAR(n)";                  
    else if ( stringStripped == "VARCHAR" ) return "VARCHAR(n)";           
    else if ( stringStripped == "WCHAR" ) return "WCHAR(n)";             
    else if ( stringStripped == "VARWCHAR" ) return "VARWCHAR(n)";
    else if ( stringStripped == "DECIMAL" ) return "DECIMAL(p,s)";
    else if ( stringStripped == "NUMERIC" ) return "NUMERIC(p,s)";          
    else if ( stringStripped == "FLOAT" ) return "FLOAT(p)";
    else if ( stringStripped == "BINARY" ) return "BINARY(n)";
    else if ( stringStripped == "VARBINARY" ) return "VARBINARY(n)";
    else if ( stringStripped == "TIME" ) return "TIME(p)";
    else if ( stringStripped == "TIMESTAMP" ) return "TIMESTAMP(p)";
    else if ( stringStripped == "INTERVAL MONTH" ) return "INTERVAL MONTH(p)";
    else if ( stringStripped == "INTERVAL YEAR" ) return "INERVAL YEAR(p)";
    else if ( stringStripped == "INTERVAL YEAR TO MONTH" ) return "INTERVAL YEAR(p) TO MONTH";
    else if ( stringStripped == "INTERVAL DAY" ) return "INTERVAL DAY(p)";
    else if ( stringStripped == "INTERVAL HOUR" ) return "INTERVAL HOUR(p)";
    else if ( stringStripped == "INTERVAL MINUTE" ) return "INTERVAL MINUTE(p)";
    else if ( stringStripped == "INTERVAL SECOND" ) return "INTERVAL SECOND(p,q)";
    else if ( stringStripped == "INTERVAL DAY TO HOUR" ) return "INTERVAL DAY(p) TO HOUR";
    else if ( stringStripped == "INTERVAL DAY TO MINUTE" ) return "INTERVAL DAY(p) TO MINUTE";
    else if ( stringStripped == "INTERVAL DAY TO SECOND" ) return "INTERVAL DAY(p) TO SECOND(q)";
    else if ( stringStripped == "INTERVAL HOUR TO MINUTE" ) return "INTERVAL HOUR(p) TO MINUTE";
    else if ( stringStripped == "INTERVAL HOUR TO SECOND" ) return "INTERVAL HOUR(p) TO SECOND(q)";
    else if ( stringStripped == "INTERVAL MINUTE TO SECOND" ) return "INTERVAL MINUTE(p) TO SECOND(q)";

    // all else
    return stringTypeName;
}

/*!
 * \brief Parses a data-type syntax string. 
 *  
 * The data-type string must use the following tokens to represent a param; 
 *  
 * 'n' - length 
 * 'p' - precision1 
 * 'q' - precision2 
 * 's' - scale 
 *  
 * For examples; "NUMERIC(p,s)" 
 *               "VARCHAR(n)"
 *  
 * A call to \sa getSyntaxString can often be used to generate a SyntaxString. 
 *  
 * \note Minor variations in the format of the SyntaxString can result in an error. 
 *       Consider calling getSyntaxString in such a case.
 *  
 * \author pharvey (5/29/20)
 * 
 * \param stringSyntaxString 
 * 
 * \return QVector&lt;QString&gt; 
 */
QVector<QString> DATADataTypeSpec::getSyntaxVector( const QString &stringSyntaxString )
{
    QVector<QString>    vectorStrings;                                                                           
    QString             stringAccum;                                                                   
    bool                bInArgs = false;                                                              
    QString             stringTokens( "npqs" );

    for ( int n = 0; n < stringSyntaxString.size(); ++n )
    {
        if ( bInArgs )                                                                        
        {                                                                                     
            if ( stringSyntaxString.at( n ) == QChar( ')' ) )
            {
                bInArgs = false;                                                                  
                stringAccum += ")";
                continue;
            }

            if ( stringSyntaxString.at( n ) == QChar( ',' ) ) continue;

            if ( stringTokens.contains( stringSyntaxString.at( n ) ) )
            {
                vectorStrings << QString( stringSyntaxString.at( n ) );
                continue;
            }
        }

        if ( stringSyntaxString.at( n ) == QChar( '(' ) )                                                              
        {                                                                                     
            vectorStrings << stringAccum << "(";
            stringAccum = "";   // "" instead of null to handle case where brace pair at end 
            bInArgs = true;                                                                   
            continue;                                                                         
        }                                                                                     

        stringAccum += stringSyntaxString.at( n );
    }
                                                                            
    if ( !stringAccum.isNull() ) vectorStrings << stringAccum;

    return vectorStrings;
}

/*!
 * \brief To get the data type with any param values inserted. 
 *  
 * \author pharvey (1/15/20)
 *  
 * \param DataTypeSpec  - data-type spec
 * \param DataType      - param values
 * 
 * \return QString 
 */
QString DATADataTypeSpec::getDataTypeWithArgs( DATADataTypeSpec *pDataTypeSpec, const DATADataType &DataType )
{
    // data type not in profile - failed lookup reported so just bypass
    if ( !pDataTypeSpec ) return QString();

    QString stringDataTypeWithArgs;

    QString s;
    foreach( s, pDataTypeSpec->vectorSyntax )
    {
        if ( s == "n" ) stringDataTypeWithArgs += DataType.stringLength;
        else if ( s == "p" ) stringDataTypeWithArgs += DataType.stringPrecision1;
        else if ( s == "q" ) stringDataTypeWithArgs += DataType.stringPrecision2;
        else if ( s == "s" ) stringDataTypeWithArgs += DataType.stringScale;
        else stringDataTypeWithArgs += s;
    }

    return stringDataTypeWithArgs;
}

QString DATADataTypeSpec::getSyntaxEncoded( DATADataTypeSpec *pDataTypeSpec )
{
    QString stringEncoded;
    QString s;
    foreach( s, pDataTypeSpec->vectorSyntax )
    {
        if ( stringEncoded.isEmpty() )
            stringEncoded = s;
        else
            stringEncoded += ( "|" + s );
    }

    return stringEncoded;
}

QVector<QString> DATADataTypeSpec::getSyntaxDecoded( const QString &stringEncoded )
{
    QVector<QString> v;
    QStringList l = stringEncoded.split( '|' );
    QString s;
    foreach( s, l )
    {
        v.append( s );
    }

    return v;
}


