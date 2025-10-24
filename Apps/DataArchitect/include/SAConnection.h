#ifndef H_SAConnection
#define H_SAConnection

#include <DATAWConnection.h>

/*!
 * \brief A data source connection. Here we expand on the object factory of \sa DATAWConnection.
 * 
 * \author pharvey (1/14/25)
 */
class SAConnection : public DATAWConnection
{
    Q_OBJECT
public:
    explicit SAConnection( AWObject *pParent = nullptr, const QString &stringName = QString() );
    virtual ~SAConnection();

    virtual ADObject *getObject( const QString &stringClass, ADObject *pObjectParent = nullptr ) override;
    virtual QMenu *   getContextMenu( QWidget *pParent ) override;

protected slots:
    virtual void slotNewForm();
    virtual void slotNewERD();
    virtual void slotNewERDFromDataSource();
    virtual void slotImport();
};

#endif
