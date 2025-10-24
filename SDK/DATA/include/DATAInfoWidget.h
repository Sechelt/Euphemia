#ifndef H_DATAInfoWidget
#define H_DATAInfoWidget

#include "DATA.h"

class DATAProfile;

/*!
 * \brief Lists results of SQLGetInfo calls.
 * 
 * \author pharvey (2020-06-08)
 */
class DATAInfoWidget : public QTreeWidget
{
    Q_OBJECT
public:
    DATAInfoWidget( DATAProfile *pProfile, QWidget *pParent );
    ~DATAInfoWidget();

protected:
    DATAProfile *pProfile;

    virtual void doSaveState();
    virtual void doLoadState();
};

#endif 


