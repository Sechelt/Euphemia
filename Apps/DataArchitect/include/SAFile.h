#ifndef H_SAFile
#define H_SAFile

#include <DFile.h>
#include <QPrinter>

class DATAWQuerySystem;
class DDiagram;
class LMModel;

/*!
 * \class SAFile 
 * \brief File (root class).
 *  
 * This introduces all of the documents we want to support in this app. 
 *  
 * \sa getObject 
 *  
 * \author pharvey (2/7/20)
 */
class SAFile : public DFile
{
    Q_OBJECT
public:
    explicit SAFile( const QString &stringName = QString() );
    virtual ~SAFile();

    virtual QPixmap         getIcon() override;
    virtual AWDataWidget *  getObjectWidget( QWidget *pWidgetParent = nullptr ) override;
    virtual ADObject *      getObject( const QString &stringClass, ADObject *pObjectParent = nullptr ) override;
    virtual QMenu *         getContextMenu( QWidget * ) override;

    virtual void doClear() override; // just to unprotect method

public slots:
    virtual bool slotNewDocument();
    virtual void slotNewDataConnection();
    virtual void slotNewDataSourceBrowser();

protected slots:
    virtual void slotCreatePhysicalModel( LMModel * );
};

#endif


