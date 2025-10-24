#ifndef H_DATAFunctionsWidget
#define H_DATAFunctionsWidget

#include "DATA.h"

class DATAProfile;
class DATAFunction;

/*!
 * \brief Lists results of SQLGetFunctions. 
 *  
 * Includes 'optional feature unsupported' (HYC00). 
 * 
 * \author pharvey (2020-06-08)
 */
class DATAFunctionsWidget : public QTreeWidget
{
    Q_OBJECT
public:
    DATAFunctionsWidget( DATAProfile *pProfile, QWidget *pParent );
    ~DATAFunctionsWidget();

protected:
    DATAProfile *pProfile;

    virtual void doUnsupported( DATAFunction *, QTreeWidgetItem *pItemParent );

protected slots:
    virtual void slotContextMenu( const QPoint &point );
    virtual void slotCopy();

    virtual void doSaveState();
    virtual void doLoadState();
};

#endif 


