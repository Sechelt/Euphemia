#ifndef H_DATADataTypesWidget
#define H_DATADataTypesWidget

#include "DATA.h"

class DATAProfile;

class DATADataTypesWidget : public QTableWidget
{
    Q_OBJECT
public:
    DATADataTypesWidget( DATAProfile *pProfile, QWidget *pParent );
    ~DATADataTypesWidget();

protected:
    DATAProfile *pProfile;

protected slots:
    virtual void slotContextMenu( const QPoint &point );
    virtual void slotCopy();

    virtual void doSaveState();
    virtual void doLoadState();
};

#endif 


