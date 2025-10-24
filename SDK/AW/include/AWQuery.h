#ifndef H_AWQuery
#define H_AWQuery

#include "AWObject.h"

class AWQuery : public AWObject
{
    Q_OBJECT
public:
    AWQuery( ADObject *pModelParent = nullptr, const QString &stringName = QString() );
    virtual ~AWQuery();

    // SETTERS

    // GETTERS
    virtual QPixmap         getIcon();
    virtual QString         getFriendlyClassName() { return tr( "Query" ); }
    virtual QMenu *         getContextMenu( QWidget * );
    virtual AWDataWidget *  getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual bool            isQueried() { return bQueried; }
    virtual bool            canLoadChildren() { return !bQueried; }

    // DO'rs
    virtual void doLoadChildren() override { doQuery(); }
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElem );
    virtual void doQuery();
    virtual void doClear();

protected slots:
    virtual void slotRefresh();

protected:
    bool bQueried;

    virtual void eventModified( ADObject *pSource );
};

#endif


