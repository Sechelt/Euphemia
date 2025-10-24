#ifndef H_EWorkspace
#define H_EWorkspace

#include <AWObject.h>

/*!
 * \brief Workspace (root model).
 *  
 * In this case; the workspace may not show in the UI in anyway. 
 * We use it to assign to g_Root. 
 * It could be used for other things in the future but at the moment 
 * we just use to create our \sa EFile objects. 
 *  
 * \author pharvey (4/16/23)
 */
class EWorkspace : public AWObject
{
    Q_OBJECT
public:
    explicit EWorkspace( const QString &stringName = QString() );
    ~EWorkspace();

    virtual QPixmap     getIcon()  override { return QPixmap(); }
    virtual ADObject *  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr ) override;
    virtual QMenu *     getContextMenu( QWidget * ) override;

    AWObject::SaveResults doCloseChildren( bool bRequest = true );
    void doClear() { AWObject::doClear(); }

public slots:
    virtual void slotNewFile();
};

#endif


