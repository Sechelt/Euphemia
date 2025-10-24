#ifndef H_AWObject
#define H_AWObject

#include <ADObject.h> 

#include "AWClassWidget.h"
#include "AWDataWidget.h"
#include "AWPropWidget.h"
#include "AWEditorWidget.h"

class AWDataTable;

class AWObject : public ADObject
{
    Q_OBJECT
    friend class AWEditorWidget;
public:
    enum enumIconRoles
    {
        nIconRoleExpanded,
        nIconRoleCollapsed
    };
    enum SaveResults
    {
        SaveSuccess,
        SaveLoseChanges,
        SaveCancel,
        SaveError
    };

    AWObject( ADObject *pobjectParent = nullptr, const QString &stringName = QString() );
    virtual ~AWObject();

    virtual void setEditorWidgetParent( QWidget *p );
    virtual void setFolder( const QString &s ) { stringFolder = s; }

    // GETTERS
    virtual QPixmap         getIcon( enumIconRoles nRole ) { Q_UNUSED(nRole); return getIcon(); }
    virtual QPixmap         getIcon() = 0;
    virtual QString         getFolder() { return stringFolder; }
    virtual QWidget *       getEditorWidgetParent();
    virtual AWEditorWidget *getEditorWidget() { return pEditorWidget; }
    virtual QMenu *         getContextMenu( QWidget * );
    virtual AWClassWidget * getClassWidget( QWidget *pWidgetParent = nullptr );
    virtual AWDataWidget *  getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual AWPropWidget *  getPropWidget( QWidget *pWidgetParent = nullptr );
    virtual AWDataTable *   getListWidget( const QString &stringClass, QWidget *pWidgetParent = nullptr );
    virtual QString         getFriendlyClassName() { return tr( "Object" ); }
    virtual QString         getDialogCaption( AWObject *p = nullptr, const QString &stringEvent = QString() );
    virtual bool            isEditor() { return pEditorWidget != nullptr; }
    virtual bool            canCreateEditor() { return bCanCreateEditor; }
    virtual bool            canLoadChildren() { return false; }

    // DO'rs
    virtual void doLoadChildren() {}
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    //    virtual bool doLoad( QDomElement *pdomElem );
    virtual void doFind( ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems, const ADFindCriteria &Criteria );
    virtual void doMessageBox( const QString &stringSeverity, const QString &stringCaption, const QString &stringMessage );
    virtual void doGoTo();

signals:
    void signalEditorOpened( AWObject *, AWEditorWidget * );    // echo'd up the object hierarchy whenever editor widget opened (created and/or focused)
    void signalEditorOpened( AWEditorWidget * );                // same as above but NOT echo'd up the object hierarchy
    void signalEditorClosing( AWObject *, AWEditorWidget * );   // echo'd up the object hierarchy whenever editor widget opened (created and/or focused)
    void signalEditorClosing( AWEditorWidget * );               // same as above but NOT echo'd up the object hierarchy

public slots:
    virtual bool slotOpenEditor();  // noop as we do not need an editor by default
    virtual bool slotCloseEditor(); // noop unless isEditor
    virtual bool slotClassDialog( QWidget *pWidgetParent = nullptr );
    virtual bool slotObjectDialog( QWidget *pWidgetParent = nullptr );

protected:
    QString             stringFolder;           // a folder name - used by some widgets to organize - default is empty - no folder
    AWEditorWidget *    pEditorWidget;          // open editor - if any
    QWidget *           pEditorWidgetParent;    // because signal from treewidget (ie double click) will not know parent
    bool                bCanCreateEditor = false;
    bool                bLoadingChildren = false;

    // save/load level 3 (supporting)
    virtual void doClear(); // Extend ADObject::doDataClear so as to untangle from UI before being deleted.
    virtual QDomElement doSaveDynamicData( QDomDocument *pdomDoc );
    virtual void        doLoadDynamicData( QDomElement *pdomElem );
};

#endif


