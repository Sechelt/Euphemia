#include "LibInfo.h"
#include "DATAWTableModel.h"

#include <DATAConnection.h>

DATAWTableModel::DATAWTableModel( QWidget *p, DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable )
    : QAbstractTableModel( p )
{
    this->pConnection       = pConnection;
    this->stringCatalog     = stringCatalog;
    this->stringSchema      = stringSchema;
    this->stringTable       = stringTable;

    Q_ASSERT( pConnection );
    Q_ASSERT( pConnection->isConnected() );

    // get all meta for table 
    if ( !metaTable.doLoad( pConnection, stringCatalog, stringSchema, stringTable, "TABLE" ) )
    {
        qWarning( "[%s][%s][%d] WARNING: Had issues with loading meta info for table.\n", __FILE__, __FUNCTION__, __LINE__ );
        bReadOnly = true;
    }
}

void DATAWTableModel::doEditCommit()
{
}

void DATAWTableModel::doEditRevert()
{
}

Qt::ItemFlags DATAWTableModel::flags( const QModelIndex &modelIndex ) const
{
    if ( !modelIndex.isValid() ) return QAbstractTableModel::flags( modelIndex );
    if ( bReadOnly )  return QAbstractTableModel::flags( modelIndex );

    int nCol = modelIndex.column();

    QString stringColumnName = metaTable.vectorColumnNames.at( nCol );
    if ( stringColumnName.isEmpty() ) return QAbstractTableModel::flags( modelIndex );

    DATAWMetaColumn metaColumn = metaTable.mapColumns.value( stringColumnName );
    if ( metaColumn.bReadOnly ) return QAbstractTableModel::flags( modelIndex );

    return QAbstractTableModel::flags( modelIndex ) | Qt::ItemIsEditable;;
}


