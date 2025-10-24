#ifndef H_DATAAttrWidget
#define H_DATAAttrWidget

#include "DATA.h"

class DATAProfile;

/*!
 * \brief Lists results of SQLGetAttr calls.
 * 
 * \author pharvey (2020-06-08)
 */
class DATAAttrWidget : public QTreeWidget
{
    Q_OBJECT
public:
    DATAAttrWidget( DATAProfile *pProfile, QWidget *pParent );
    ~DATAAttrWidget();

protected:
    DATAProfile *pProfile;

    virtual void doSaveState();
    virtual void doLoadState();
};

#endif 


