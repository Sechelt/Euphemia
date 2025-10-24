#ifndef H_AWObjectsWidget
#define H_AWObjectsWidget

#include "AWObject.h"
#include "AWDataTable.h"

/*!
 * \brief View/modify a list of objects. 
 *  
 * This lists all of the children that *match* the given stringClass. 
 *  
 * The following features can be enabled; 
 *  - New - to create new object
 *  - Delete - to request a delete on the object
 *  - Up - to move the object further back (beginning of list)
 *  - Down - to move the object further forward (end of list)
 *  - Data - to allow the objects Data dialog to be invoked
 *  
 * Classes can be derived from this or AWDataTable for scenarios such as; 
 *  - different columns
 *  - columns with custom data edit widgets
 *  - additional features
 *          - selecting something before creating object
 *          - creating a dependent before creating an object
 *          - customizing the delete
 *  
 * \sa AWFactoryWidget 
 *  
 * \author pharvey (12/6/19)
 */
class AWObjectsWidget : public AWDataTable
{
    Q_OBJECT
public:
    AWObjectsWidget( AWObject *pObject, QWidget *pWidgetParent, const QString &stringClass );
    virtual ~AWObjectsWidget();

public slots:
    virtual void slotNew();

protected:
    AWObject *  pObject;
    QString     stringClass;

    // SETTERS
    
    // GETTERS
    
    // DO'RS
    virtual void doRowGet( int nRow, AWObject *pObject );
    virtual void doRowSet( int nRow, AWObject *pObject );
};

#endif


