#ifndef H_FRForm
#define H_FRForm

#include <DDiagram.h>

class FRForm : public DDiagram
{
    Q_OBJECT
public:
    explicit FRForm( ADObject *pParent, const QString &stringName = QString() );
    virtual ~FRForm();

    virtual QPixmap         getIcon();
    virtual QString         getFriendlyClassName() { return tr( "Form" ); }
    virtual ADObject *      getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

public slots:
    virtual bool slotOpenEditor();

protected:
    virtual DGraphicsProxyItem *getProxy( QGraphicsObject *p );
};

#endif


