#ifndef H_ODBCMinModel
#define H_ODBCMinModel

#include "ODBCModel.h"

/*! 
 * \class ODBCMinModel 
 * \brief A Physical Model based upon the ODBC Minimal grammar specification. 
 *  
 * Not much is offered in the specification. Probably only useful for drivers working with text files etc. 
 * We implement some things (ie references) into the model - just for documentation purposes. 
 *  
 * \author pharvey (1/14/20)
 */
class ODBCMinModel : public ODBCModel
{
    Q_OBJECT
public:
    ODBCMinModel( ADObject *pParent );
    virtual ~ODBCMinModel();

    virtual AWClassWidget * getClassWidget( QWidget *pWidgetParent = nullptr );
};

#endif


