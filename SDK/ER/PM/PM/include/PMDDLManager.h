#ifndef H_PMDDLManager
#define H_PMDDLManager

#include <ADTransaction.h>
#include <AWObject.h>

/*!
 * \class PMDDLManager 
 * \brief Manual alternative for SQL scripts. 
 *  
 * Used by objects which can generate a SQL 'CREATE' and/or 'DROP' statement. 
 * Each of these objects will implement \sa getSQLCreate and \sa getSQLDrop to provide 
 * the 'automatically generated' SQL scripts. 
 * Each of these objects will also inherit one of these to manage case where User wants to override 
 * with 'manually generated' SQL scripts.  
 *  
 * Inherit PMDDLManager... 
 * \sa PMDomain 
 * \sa PMModel              <--------------
 * \sa PMProcedure 
 * \sa PMTableNormal 
 * \sa PMReference 
 * \sa PMSequence 
 *  
 * \sa PMIndex 
 * \sa PMInherit 
 * \sa PMView 
 *  
 * Provide getSQLCreate/SQLDrop... 
 *  
 * \author pharvey (1/12/20)
 */
class PMDDLManager
{
public:
    PMDDLManager( AWObject *pObject );

    virtual void setIncludeCreate( bool b );
    virtual void setIncludeDrop( bool b );
    virtual void setCreateManual( bool b );
    virtual void setDropManual( bool b );
    virtual void setCreateManual( const QString &s );
    virtual void setDropManual( const QString &s );

    virtual bool    isIncludeCreate()   { return bIncludeCreate;        }
    virtual bool    isIncludeDrop()     { return bIncludeDrop;          }
    virtual bool    isCreateManual()    { return bCreateManual;         }
    virtual bool    isDropManual()      { return bDropManual;           }
    virtual QString getCreateManual()   { return stringCreateManual;    }
    virtual QString getDropManual()     { return stringDropManual;      }

    virtual bool doSavePMDDLManager( QDomDocument *pdomDoc, QDomElement *pdomElem );
    virtual bool doLoadPMDDLManager( QDomElement *pdomElem );

protected:
    AWObject *pObject;              // object we are part of - so we can call its setModified()

    bool    bIncludeCreate;        // Include our CREATE statement
    bool    bIncludeDrop;          // Include our DROP statement
    bool    bCreateManual;         // Our CREATE statement is manual
    bool    bDropManual;           // Our DROP statement is manual
    QString stringCreateManual;    // Our manual CREATE statement
    QString stringDropManual;      // Our manual DROP statement
};

#endif


