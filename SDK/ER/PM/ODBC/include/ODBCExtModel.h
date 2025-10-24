#ifndef H_ODBCExtModel
#define H_ODBCExtModel

#include "ODBCCorModel.h"

/*! 
 * \class ODBCExtModel 
 * \brief A Physical Model based upon the ODBC Extended grammar specification. 
 *  
 * This extends \sa ODBCCorModel by adding some data-types. 
 * This level of conformance (as in 100%) is not the most common. 
 *  
 * \author pharvey (1/14/20)
 */
class ODBCExtModel : public ODBCCorModel
{
    Q_OBJECT
public:
    ODBCExtModel( ADObject *pParent );
    virtual ~ODBCExtModel();

    virtual AWClassWidget * getClassWidget( QWidget *pWidgetParent = nullptr );
};

#endif


