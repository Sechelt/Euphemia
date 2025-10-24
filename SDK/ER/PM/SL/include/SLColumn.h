#ifndef H_SLColumn
#define H_SLColumn

#include <PMColumn.h>

class SLTable;



/*!
 * \brief A column.
 * 
 * Data Types
 * CREATE TABLE t ( c dt,... );
 * ============================
 * 
 * INTEGER
 * -------
 * 
 * INT
 * INTEGER
 * TINYINT
 * SMALLINT
 * MEDIUMINT
 * BIGINT
 * UNSIGNED BIG INT
 * INT2
 * INT8
 * 
 * TEXT
 * ----
 * 
 * CHARACTER(20)
 * VARCHAR(255)
 * VARYING CHARACTER(255)
 * NCHAR(55)
 * NATIVE CHARACTER(70)
 * NVARCHAR(100)
 * TEXT
 * CLOB
 * 
 * BLOB
 * ----
 * 
 * BLOB
 * 
 * REAL
 * ----
 * 
 * REAL
 * DOUBLE
 * DOUBLE PRECISION
 * FLOAT
 * 
 * NUMERIC
 * -------
 * 
 * NUMERIC
 * DECIMAL(10,5)
 * BOOLEAN
 * DATE
 * DATETIME
 * 
 * Data Types
 * CREATE TABLE t ( c dt,... ) STRICT;
 * ===================================
 * 
 *  INT
 *  INTEGER
 *  REAL
 *  TEXT
 *  BLOB
 *  ANY
 * 
 * \author pharvey (3/22/25)
 */
class SLColumn : public PMColumn
{
    Q_OBJECT
public:
    SLColumn( SLTable *pParent );
    virtual ~SLColumn();
};

#endif 


