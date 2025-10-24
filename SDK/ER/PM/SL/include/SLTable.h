#ifndef H_SLTable
#define H_SLTable

#include <PMTableNormal.h>

#include "SLColumn.h"
#include "SLIndex.h"

class SLModel;

class SLTable : public PMTableNormal
{
    Q_OBJECT
public:
    SLTable( SLModel *pParent );
    virtual ~SLTable();

    virtual void setStrict( bool );
    virtual void setRowID( bool );

    virtual ADObject *      getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );
    virtual AWDataWidget *  getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual bool            getStrict()    { return bStrict;   }
    virtual bool            getRowID()     { return bRowID;    }

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElemThis );

protected:
    bool    bStrict = false;        // using STRICT data types
    bool    bRowID  = true;         // has rowid
};

class SLTableWidget : public QWidget
{
    Q_OBJECT
public:
    SLTableWidget( SLTable *pTable, QWidget *pParent );

protected:
    SLTable *           pTable;
    QCheckBox *         pStrict;
    QCheckBox *         pRowID;
};

#endif 


