#ifndef H_AWDataWidget
#define H_AWDataWidget

#include <CBD.h>

class AWObject;

/*!
 * \class AWDataWidget 
 * \brief Base class for a widget used in a dialog for viewing/editing objects details. 
 *  
 * A AWObject based object provides one of these via \sa AWObject::getObjectWidget(). 
 * An application will, typically, call \sa AWObject::slotObjectDialog() to get the 
 * complete dialog. 
 *  
 * Derived classes can use an 'Apply' button or have apply be implicit as the values 
 * are changed in the UI. The dialog will not have an apply/cancel - just a 'Done' or 'Ok' 
 * button. 
 *  
 * \sa AWPropWidget 
 *  
 * \author pharvey (11/17/19)
 */
class AWDataWidget : public QTabWidget
{
    Q_OBJECT
public:
    AWDataWidget( AWObject *pObject, QWidget *pwidgetParent );

    int         indexOf( const QString &stringInherits );
    AWObject *  getObject() { return pObject; }
    QWidget *   getByClass( const QString &stringInherits );
    void        removeByClass( const QString &stringInherits );
    void        removeByName( const QString &stringName );
    void        removeByWidget( QWidget *pWidget );

protected:
    AWObject *pObject;
};

#endif 

