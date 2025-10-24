#ifndef H_ODBCDrvModel
#define H_ODBCDrvModel

#include "ODBCModel.h"

/*! 
 * \class ODBCDrvModel 
 * \brief Use an ODBC connection to query driver for conformance.
 *  
 * In this case we want to use all features exposed by the ODBC driver/data-source. 
 * We make a connection to a driver/data-source and query it to determine supported 
 * features and data-types. 
 *  
 * The features and data-types are then stored in the model - so no need to make a 
 * connection and requery the driver/data-source. 
 *  
 * \author pharvey (1/14/20)
 */
class ODBCDrvModel : public ODBCModel
{
    Q_OBJECT
public:
    ODBCDrvModel( ADObject *pParent );
    virtual ~ODBCDrvModel();

    virtual AWClassWidget *getClassWidget( QWidget *pWidgetParent = nullptr );
    virtual QMenu *        getContextMenu( QWidget * );

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElemThis );
    virtual bool doLoadClasses( QDomElement *pdomElem );

public slots:
    virtual bool slotQueryForFeatures(); // call here to load features from driver/source via a connection
};

#endif


