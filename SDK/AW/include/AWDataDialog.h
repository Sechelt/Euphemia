#ifndef H_AWDataDialog
#define H_AWDataDialog

#include "AWDataWidget.h"

/*!
 * \class AWDataDialog 
 *  
 * \brief A dialog box for viewing/editing object details.
 *  
 * The main widget comes from calling AWObject::getObjectWidget.
 * 
 * This dialog will take steps to save/restore its geometry.
 *  
 * The dialog has a single button - 'Ok'. The added widgets will probably want to 
 * apply any changes as they are made. 
 *  
 * This class is NOT expected to be used as a base class. This 
 * implementation should be good enough as-is. 
 *  
 * \sa AWDataWidget 
 * \sa AWObject::slotObjectDialog 
 *  
 * \author pharvey (11/4/18)
 */
class AWDataDialog : public QDialog
{
    Q_OBJECT
public:
    AWDataDialog( AWObject *pObject, QWidget *pWidgetParent );

protected:
    virtual void doLoadState();
    virtual void doSaveState();

protected slots:
    virtual void slotOk();
    virtual void slotRejected();
};

#endif

