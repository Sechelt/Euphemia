#ifndef H_DATAProfiles
#define H_DATAProfiles

#include <sqlite3.h> 

#include <CBD.h>
// #include <DATA.h>

class DATAProfile;
class DATAConnection;

/*!
 * \brief Profiles of all connections made. 
 *  
 * We keep a Profile of connections so that we can optimize behaviour. 
 * The Profiles are persisted in a SQLite3 database. 
 * This is loaded to memory during app init and is saved with a call to \sa doSave(). 
 * There can only be one instance of this and it is accessed by the app via a global \sa g_Profiles. 
 * Profiles are added/removed as the app sees fit. Typically only added when it does not exist. 
 * New info (ie unsupported feature) is added to a Profile as it is discovered. 
 * Unsupported feature info can be used to adjust/optimize behaviour. 
 *  
 * A Profiles Manager can be used to; 
 *  - remove a Profile - to cause it to be 'refreshed' when/if next used
 *  - edit - to allow human intelligence to come into play
 *  
 * The 'Profiles Manager' is, at this time, simply any tool to edit the SQLite3 database. 
 *  
 * \note A Profile's key is its connection string. It is important to create a Profile without auth info in connect string (ie with a driver connect). 
 *  
 * \author pharvey (1/10/21)
 */
class DATAProfiles
{
public:
    DATAProfiles();
    ~DATAProfiles();

    static DATAProfiles *instance(); // This is our single, global, instance. \sa g_Profiles

    // called by DATAConnection when connection made
    DATAProfile *   getProfile( DATAConnection *pConnection, bool bCreate = true );     // new profile loaded from database else load from connection else empty (possible)
    // called when profile needed with no connection (probably for a standard profile)
    DATAProfile *   getProfile( const QString &stringKey, bool bCreate = true );        // new profile loaded from database else empty (possible)

    bool doSave();

protected:
    QMap<QString,DATAProfile*>  mapProfiles;                // <key,profile>
    QString                     stringFileName;             // profiles database
    sqlite3 *                   pDatabase;                  // connection to profiles database
    QStringList                 listStandardProfileKeys;    // standard profiles we support

    static bool existsDatabase( const QString & );

    static sqlite3 *doOpenDatabase( const QString & ); 
    static sqlite3 *doCreateDatabase( const QString & );
    static sqlite3 *doCloseDatabase( sqlite3 * ); 
    static bool     doDropDatabase( const QString & );

    bool doLoad( const QString &stringKey );
    void doClear();

    bool doLoadStandard( const QString &stringKey );
    bool doLoadODBCMin( DATAProfile *pProfile );
    bool doLoadODBCCor( DATAProfile *pProfile );
    bool doLoadODBCExt( DATAProfile *pProfile );
    bool doLoadSQL92();
    bool doLoadSQLite();
    bool doLoadMSA();
};

// Simplify access to our single, global, instance.For example;
//  
//          g_Constants->mapFunctionNames 
//
#define g_Profiles DATAProfiles::instance()

#endif

