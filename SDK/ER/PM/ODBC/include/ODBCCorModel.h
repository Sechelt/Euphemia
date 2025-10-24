#ifndef H_ODBCCorModel
#define H_ODBCCorModel

#include "ODBCMinModel.h"

/*! 
 * \class ODBCCorModel 
 * \brief A Physical Model based upon the ODBC Core grammar specification. 
 *  
 * This extends \sa ODBCMinModel by adding some features and data-types. 
 * This level of conformance is the most common in ODBC drivers/data-sources. 
 * It is common that *some* features from ODBC Extended are also available. 
 *  
 * \author pharvey (1/14/20)
 */
class ODBCCorModel : public ODBCMinModel
{
    Q_OBJECT
public:
    ODBCCorModel( ADObject *pParent );
    virtual ~ODBCCorModel();

    virtual AWClassWidget * getClassWidget( QWidget *pWidgetParent = nullptr );
};

#endif


