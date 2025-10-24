#include "LibInfo.h"
#include "ADTransaction.h"

#include <ADObject.h>

ADTransaction::ADTransaction()
{
    pRoot = nullptr;
    nState = TransactionIdle;
    nCopyMode = CopyReferenceOnly;
    nDeleteMode = DeleteReferencedUnused;
}

ADTransaction::~ADTransaction()
{
    if ( pProgress )
    {
        delete pProgress;
        pProgress = nullptr;
    }
}

ADTransaction* ADTransaction::instance()
{
    static ADTransaction instance;
    return &instance;
}

bool ADTransaction::initCopy( ADObject *p, CopyModes nCopy )
{
    if ( !p ) return false;
    if ( !isIdle() )
    {
        p->doMessage( "DEBUG", QObject::tr("Can not change transaction state. Transaction in use.") );
        return false;
    }

    pRoot = p;
    nCopyMode = nCopy;
    nState = TransactionCopy;

    return true;
}

bool ADTransaction::initPaste( ADObject *p )
{
    if ( !p ) return false;
    if ( !isIdle() )
    {
        p->doMessage( "DEBUG", QObject::tr("Can not change transaction state. Transaction in use.") );
        return false;
    }

    pRoot = p;
    nState = TransactionPaste;

    return true;
}

bool ADTransaction::initDelete( ADObject *p, DeleteModes nDelete )
{
    if ( !p ) return false;
    if ( !isIdle() )
    {
        p->doMessage( "DEBUG", QObject::tr("Can not change transaction state. Transaction in use.") );
        return false;
    }

    pRoot = p;
    nDeleteMode = nDelete;
    nState = TransactionDelete;

    return true;
}

bool ADTransaction::initDelete()
{
    hashObjectsCopied.clear();
    mapXRef.clear();
    nState = TransactionDelete;

    return true;
}

bool ADTransaction::initSave( ADObject *p )
{
    if ( !p ) return false;
    if ( !isIdle() )
    {
        p->doMessage( "DEBUG", QObject::tr("Can not change transaction state. Transaction in use.") );
        return false;
    }

    pRoot = p;
    nState = TransactionSave;
    QApplication::setOverrideCursor( Qt::WaitCursor );

    return true;
}

bool ADTransaction::initLoad( ADObject *p )
{
    if ( !p ) return false;
    if ( !isIdle() )
    {
        p->doMessage( "DEBUG", QObject::tr("Can not change transaction state. Transaction in use.") );
        return false;
    }

    pRoot = p;
    nState = TransactionLoad;
    QApplication::setOverrideCursor( Qt::WaitCursor );

    return true;
}


void ADTransaction::fini()
{
    pRoot = nullptr;
    nState = TransactionIdle;
    nCopyMode = CopyReferenceOnly;
    nDeleteMode = DeleteReferencedUnused;
    hashObjectsCopied.clear();
    mapXRef.clear();

    QApplication::restoreOverrideCursor();
}

void ADTransaction::doDumpXRef()
{
    qDebug() << "source-path: destination-path";
    QMapIterator<QString,QString> i( mapXRef );
    while ( i.hasNext() ) 
    {
        i.next();
        qDebug() << i.key() << ": " << i.value();
    }
}



