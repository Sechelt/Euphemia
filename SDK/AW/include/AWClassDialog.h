#ifndef H_AWClassDialog
#define H_AWClassDialog

#include "AWClassWidget.h"

/*!
 * \class AWClassDialog 
 *  
 * \brief A dialog box for viewing class information.
 *  
 * This class is NOT expected to be used as a base class... 
 * this implementation should be good enough as-is. 
 *  
 * \sa AWClassWidget 
 * \sa AWObject::slotClassDialog 
 *  
 * \author pharvey (1/21/20)
 */
class AWClassDialog : public QDialog
{
    Q_OBJECT
public:
    AWClassDialog( AWObject *pObject, QWidget *pWidgetParent );

protected:
    virtual void doLoadState();
    virtual void doSaveState();

protected slots:
    virtual void slotOk();
    virtual void slotRejected();
};

#endif

