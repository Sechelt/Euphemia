#ifndef H_DATAWQuerySystem
#define H_DATAWQuerySystem

#include <DATASystem.h>
#include <DATAEnvironment.h>

#include "DATAWQuery.h"

/*!
 * \brief ODBC System. 
 *  
 * This is the top-most object when browsing ODBC. 
 * From here - all of ODBC (on this machine context) is exposed. 
 * 
 * Children it can produce are; 
 *  
 * 1  \sa DATAWQueryDrivers 
 * 1  \sa DATAWQueryDataSources (User)
 * 1  \sa DATAWQueryDataSources (System) 
 *  
 * \note The doClear() does not delete children - it calls doClear() in them.
 *  
 * \author pharvey (6/17/20)
 */
class DATAWQuerySystem : public DATAWQuery
{
    Q_OBJECT
public:
    DATAWQuerySystem( ADObject *pParent );
    virtual ~DATAWQuerySystem();

    // SETTERS

    // GETTERS
    virtual QPixmap                 getIcon();
    virtual QString                 getFriendlyClassName() { return tr( "System" ); }
    virtual ADObject *              getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );
    virtual QMenu *                 getContextMenu( QWidget * ); 
    virtual DATASystem *            getSystem()         { return pSystem;       }
    virtual DATAEnvironment *       getEnvironment()    { return pEnvironment;  }

    // DOERS
    virtual void doQuery();
    // virtual void doClear();

signals:
    void signalMessage( DATAMessage * );
    void signalDiagnostic( DATADiagnostic * );

public slots:
   virtual void slotProperties();

private:
    DATASystem *     pSystem;
    DATAEnvironment *pEnvironment;
};

#endif

