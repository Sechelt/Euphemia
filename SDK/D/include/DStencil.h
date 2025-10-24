#ifndef H_DStencil
#define H_DStencil

#include "AWFolder.h"

class DStencil : public AWFolder
{
    Q_OBJECT
public:
    DStencil( AWObject *pParent = nullptr, const QString &stringName = QString() );
    virtual ~DStencil();

    virtual QPixmap     getIcon( AWObject::enumIconRoles nRole );
    virtual QPixmap     getIcon();
    virtual QMenu *     getContextMenu( QWidget * );
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

protected slots:
    virtual void slotAddSvg();
    virtual void slotAddBitmap();
    virtual void slotExport();
};

#endif


