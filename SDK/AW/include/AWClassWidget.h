#ifndef H_AWClassWidget
#define H_AWClassWidget

#include <CBD.h>

class AWObject;

/*!
 * \brief A tab widget to be filled with read-only widgets providing information about the class. 
 *  
 * Example; description and version of class. 
 *  
 * This can be incorporated into the \sa AWDataWidget returned by \sa getObjectWidget or 
 * invoked on its own. 
 * 
 * \author pharvey (1/21/20)
 */
class AWClassWidget : public QTabWidget
{
    Q_OBJECT
public:
    AWClassWidget( AWObject *pObject, QWidget *pwidgetParent );

    AWObject *getObject() { return pObject; }

protected:
    AWObject *pObject;
};

#endif 

