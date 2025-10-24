#ifndef H_ODBCModel
#define H_ODBCModel

#include <PMModel.h>

class DATAInfo;

class ODBCDomain;
class ODBCTable;
class ODBCReference;
class ODBCView;
class ODBCProcedure;

/*!
 * \class ODBCModel
 * \brief Entity Relationship: Physical Model (ODBC)
 *  
 * ODBC Interface Conformance
 * -------------------------- 
 *  
 * We use this to determine what the driver can do for us (the app). This is found via SQLGetInfo. 
 * This has little/no influence on what the Physical Model can provide (read; what the data source can do). 
 *  
 *       SQL_ODBC_INTERFACE_CONFORMANCE 
 *              - SQL_OIC_CORE
 *                      The minimum level that all ODBC drivers are expected to comply with. This
 *                      level includes basic interface elements such as connection functions, functions for preparing and
 *                      executing an SQL statement, basic result set metadata functions, basic catalog functions, and so on.
 *              - SQL_OIC_LEVEL1
 *                      A level including the core standards compliance level functionality, plus
 *                      scrollable cursors, bookmarks, positioned updates and deletes, and so on.
 *              - SQL_OIC_LEVEL2
 *                      A level including level 1 standards compliance level functionality, plus
 *                      advanced features such as sensitive cursors; update, delete, and refresh by bookmarks; stored
 *                      procedure support; catalog functions for primary and foreign keys; multi-catalog support; and
 *                      so on.
 *  
 * ODBC SQL Conformance 
 * -------------------- 
 *  
 * We use this to determine what features we can provide in the Physical Model. 
 * This can be derived from the ODBC specification and/or by querying an 
 * ODBC driver/data source using SQLGetInfo, SQLGetDataTypes, etc. 
 *  
 * Specification 
 *  
 *      The ODBC specification provides its own levels of SQL compliance (see; ODBC SQL Grammar).  
 *  
 * Driver/Data Source 
 *  
 *      We can call SQLGetInfo to get SQL conformance to SQL92.
 *      However; this will probably result in a sub set of features the driver supports since the driver can only claim conformance
 *      when it is 100% conforming. Support for higher level features can be determined via additional calls to SQLGetInfo. 
 *  
 *              SQL_SQL_CONFORMANCE
 *                      - SQL_SC_SQL92_ENTRY            Entry level SQL-92 compliant
 *                      - SQL_SC_FIPS127_2_TRANSITIONAL FIPS 127-2 transitional level compliant
 *                      - SQL_SC_SQL92_FULL             Full level SQL-92 compliant
 *                      - SQL_SC_SQL92_INTERMEDIATE     Intermediate level SQL-92 compliant
 *  
 * ODBC SQL Grammar 
 * ---------------- 
 *  
 * SQL grammars come from the various standards bodies, such as Open Group, ANSI, and ISO. 
 * Some vendors define their own grammar, which may vary slightly from the standards.
 *  
 * ODBC has its own levels of SQL conformance. In general; they are a subset of SQL92. 
 *  
 *      SQL_ODBC_SQL_CONFORMANCE 
 *  
 *              1. SQL_OSC_MINIMUM
 *  
 *                  Data Definition Language (DDL): CREATE TABLE and DROP TABLE.
 *                  Data Manipulation Language (DML): simple SELECT, INSERT, UPDATE SEARCHED, and DELETE SEARCHED.
 *                  Expressions: simple (such as A > B + C).
 *                  Data types: CHAR, VARCHAR, or LONG VARCHAR.
 *              
 *              2. SQL_OSC_CORE
 *  
 *                  DDL: ALTER TABLE, CREATE INDEX, DROP INDEX, CREATE VIEW, DROP VIEW, GRANT, and REVOKE.
 *                  DML: full SELECT.
 *                  Expressions: subquery, set functions such as SUM and MIN.
 *                  Data types: DECIMAL, NUMERIC, SMALLINT, INTEGER, REAL, FLOAT, DOUBLE PRECISION.
 *              
 *              3. SQL_OSC_EXTENDED
 *  
 *                  DML: outer joins, positioned UPDATE, positioned DELETE, SELECT FOR UPDATE, and unions.
 *                  Expressions: scalar functions such as SUBSTRING and ABS, date, time, and timestamp literals.
 *                  Data types: BIT, TINYINT, BIGINT, BINARY, VARBINARY, LONG VARBINARY, DATE, TIME, TIMESTAMP
 *                  Batch SQL statements.
 *                  Procedure calls.
 *  
 * The SQL specified in each will vary slightly depending upon the ODBC version the driver
 * conforms to, and (sadly) where you get your information from. 
 * 
 * All ODBC drivers must support, at least, the Minimum SQL Grammar and most support much
 * of the Core and some of the Extended.
 * 
 * This model attempts to support the 3 SQL Grammars specified in the ODBC specification AND 
 * a 4th option to query an ODBC driver and adopt the SQL Grammar returned.
 * 
 * Domains
 * 
 * Domains are availible in this model to make it easier to specify common column
 * data types - even if the current SQL Grammar does not support domains.
 * 
 * References
 *     
 * References between tables, and the columns used in those references, are supported 
 * in the model for documentation purposes - even if the current SQL Grammar does not
 * support this type of referential integrety check. Columns can also be flagged as Primary or
 * Foreign key. 
 * 
 * Resources...
 * 
 * ODBC SQL Grammar can be found in Appendix C and Appendix D of the 
 * ODBC Programmers Reference;
 * 
 * http://msdn.microsoft.com/library/default.asp?url=/library/en-us/odbc/htm/odappcpr_1.asp
 */
class ODBCModel : public PMModel
{
    Q_OBJECT
public:
    ODBCModel( ADObject *pParent );
    virtual ~ODBCModel();

    virtual ADObject *          getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );
    virtual DATAInfo *          getInfo( const QString &stringKey );
    virtual void                getValidationConfiguration(); // entry point - someone (parent or app perhaps) calls this to load default configuration
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses ); // load default configuration - all classes have this
    virtual bool                hasFeature( FeatureTypes nFeature, FeatureSupportTypes nSupport );

    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );

public slots:
    virtual bool slotOpenEditor();
};

#endif


