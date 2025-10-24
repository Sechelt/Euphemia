#ifndef H_AWFactoryWidget
#define H_AWFactoryWidget

#include "AWObject.h"

/*!
 * \class AWFactoryWidget
 * \brief Shows a tab for each class from the object's factory. Each tab lists objects created by the factory.
 *  
 * Objects can be; browsed, created, edited, and deleted. 
 *  
 * \author pharvey (2/19/20)
 */
class AWFactoryWidget : public QTabWidget
{
    Q_OBJECT
public:
    AWFactoryWidget( AWObject *pObject, QWidget *pWidgetParent );
    virtual ~AWFactoryWidget();

    virtual void setAllowCreate( bool b );
    virtual void setAllowDelete( bool b );
    virtual void setAllowEdit( bool b );

protected:
    bool bAllowCreate   = false;
    bool bAllowDelete   = false;
    bool bAllowEdit     = true;
};

#endif


