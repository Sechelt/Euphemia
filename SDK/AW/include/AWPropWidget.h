#ifndef H_AWPropWidget
#define H_AWPropWidget

#include <WStackedWidget.h>

class AWObject;

/*!
 * \class AWPropWidget 
 * \brief Base class for widgets shown in a properties dock window. 
 *  
 * An AWObject provides one of these via \sa AWObject::getPropWidget().  
 *  
 * This can be used in two ways; 
 * 1. derive a new class from this and then return an instance via getPropWidget 
 * 2. create an instance of this, with widgets layed out on it, and then return it via getPropWidget  
 *  
 * The application will display this appropriately. The application should not have to 
 * interact with the prop widget or any of its children. The interaction should be 
 * between the object that created it (ie apply changes as they are made). 
 *  
 * The application may choose NOT to show one of these when multiple objects 
 * selected. Alternatively; show one for the 1st object in selection list. 
 *  
 * An editor may choose to provide actions (menu/toolbar) for changing properties 
 * (ie pen, font, etc) that can apply to a complete selection of objects. 
 *  
 * \sa AWDataWidget 
 *  
 * \author pharvey (11/17/19)
 */
class AWPropWidget : public WStackedWidget
{
    Q_OBJECT
public:
    AWPropWidget( AWObject *pObject, QWidget *pwidgetParent );
    virtual ~AWPropWidget();

    virtual AWObject *  getObject() { return pObject; }

protected:
    AWObject *pObject;
};

#endif 

