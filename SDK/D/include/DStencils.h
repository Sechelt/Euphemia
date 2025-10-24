#ifndef H_DStencils
#define H_DStencils

#include "AWFolder.h"

class DStencils : public AWFolder
{
    Q_OBJECT
public:
    DStencils( AWObject *pParent = nullptr, const QString &stringName = QString() );
    virtual ~DStencils();

    virtual QPixmap     getIcon( AWObject::enumIconRoles nRole );
    virtual QPixmap     getIcon();
    virtual QMenu *     getContextMenu( QWidget * );
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

protected slots:
    virtual void slotNew();
    virtual void slotImport();
};

#endif


