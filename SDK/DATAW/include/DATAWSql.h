#ifndef H_DATAWSql
#define H_DATAWSql

#include <AWObject.h>

class AWDataObjectWidget;
class DATAWConnection;

/*!
 * \brief An SQL file. 
 *  
 * This is saved to a text file which only contains the SQL text. So no connection information. 
 * This produces no children. 
 * The connection information is determined as follows... 
 *  
 * 1. the first DATAWConnection found when tracing back to root object 
 * 2. a DATAWConnection (if any) belonging to root (needs to be selected from a list) 
 * 3. no DATAWConnection 
 *  
 * The ADObject data; OID, Name, Code etc are ignored.  
 * The ADDocument is ignored. 
 *  
 * \author pharvey (12/17/20)
 */
class DATAWSql : public AWObject
{
    Q_OBJECT
public:
    explicit DATAWSql( ADObject *pParent, const QString &stringName = QString() );
    virtual ~DATAWSql();

    virtual void setSQL( const QString & );
    virtual void setModified( bool b = true ) override;

    virtual QPixmap             getIcon() override;
    virtual QString             getFriendlyClassName() override { return tr( "SQL" ); }
    virtual AWDataWidget *      getObjectWidget( QWidget *pWidgetParent = nullptr ) override;
    virtual AWPropWidget *      getPropWidget( QWidget *pWidgetParent = nullptr ) override;
    virtual QMenu *             getContextMenu( QWidget * ) override;
    virtual QString             getSQL();
    virtual DATAWConnection *   getConnection();

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent ) override;
    virtual bool doLoad( QDomElement *pdomElemColumn ) override;
    virtual void doGoTo( ADObjectPathList listObjectPathItems, uint nDepth ) override;
    virtual void doGoToView( ADObjectPathList listObjectPathItems, uint nDepth ) override;
    virtual void doGoTo() override;
    virtual void doFind( ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems, const ADFindCriteria &Criteria ) override;
    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems ) override;
    virtual void doPreSubmit();

public slots:
    virtual bool slotOpenEditor();
    virtual bool slotCloseEditor();

protected:
    QString stringSQL;

    virtual void doClear() override;
    virtual void doSyncWithEditor();

protected slots:
    virtual void slotModified( bool ); // connected to editor - when editor in play
    virtual void slotExport();
    virtual void slotImport();
};

#endif


