#include "LibInfo.h"
#include "DATAWTableModelODBC.h"

#include <DATAConnection.h>
#include <DATAStatement.h>
#include <DATADiagnostic.h>

DATAWTableModelODBC::DATAWTableModelODBC( QWidget *p, DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable )
    : DATAWTableModel( p, pConnection, stringCatalog, stringSchema, stringTable )
{
    SQLRETURN nReturn;
    aRowStatus[0] = SQL_ROW_IGNORE; // init to legit value just because

    // build SQL
    QString stringQuery;
    {
        // build query string specifiying columns explicitly
        // should not contain any hidden columns such as SQLite ROWID 
        listColumns = metaTable.mapColumns.keys();
        if ( listColumns.count() < 1 )
            printf( "[%s][%s][%d] WARNING: No columns.\n", __FILE__, __FUNCTION__, __LINE__ );

        stringQuery = "SELECT ";
        for ( int nIndex = 0; nIndex < listColumns.count(); nIndex++ )
        {
            if ( nIndex ) stringQuery += ", ";
            stringQuery += listColumns.at( nIndex );
        }
        stringQuery += " FROM " + stringTable;
    }

    // create buffers for a single row
    // for now... everything is a C-string with a 1k char max size
    for ( int nIndex = 0; nIndex < listColumns.count(); nIndex++ )
    {
        // create bound buffers
        // - simple for now by not worrying about actual col type/size 
        char *  ps = (char*)malloc( sizeof(char) * 10000 * DATAWTableModelODBCRowArraySize );
        SQLLEN *pn = (SQLLEN*)malloc( sizeof(SQLLEN) * DATAWTableModelODBCRowArraySize );
        *pn = SQL_COLUMN_IGNORE;
        vectorRowBoundData << ps;
        vectorRowBoundDataInd << pn;

        // credit edit buffers
        vectorDataEdit       << QVariant();
        vectorDataEditInd    << SQL_COLUMN_IGNORE;
    }

    // get a result set with a scrollable cursor
    {
        pStatement = new DATAStatement( pConnection );
        if ( !pStatement )
            qFatal( "[%s][%s][%d] CRITICAL: Failed to allocate a DATAStatement.\n", __FILE__, __FUNCTION__, __LINE__ );

        // ISO conforming method of setting cursor attr 
        //          SQL_ATTR_CURSOR_SCROLLABLE
        //          SQL_ATTR_CURSOR_SENSITIVITY
        // - SQL_SCROLLABLE and SQL_SENSITIVE is same as SQL_CURSOR_KEYSET_DRIVEN
        // - no equivalent to SQL_CURSOR_DYNAMIC
        // - just use ODBC method instead

        nReturn = pStatement->setAttrCursorType( DATAStatement::CursorKeysetDriven );
        if ( !SQL_SUCCEEDED( nReturn ) ) qWarning( "[%s][%s][%d] WARNING: Failed to set DATAStatement::CursorKeysetDriven.\n", __FILE__, __FUNCTION__, __LINE__ );

        nReturn = pStatement->setAttrRowBindType( SQL_ATTR_ROW_BIND_TYPE, SQL_BIND_BY_COLUMN );
        if ( !SQL_SUCCEEDED( nReturn ) ) qWarning( "[%s][%s][%d] WARNING: Failed to set SQL_ATTR_ROW_BIND_TYPE.\n", __FILE__, __FUNCTION__, __LINE__ );

        nReturn = pStatement->setAttrRowArraySize( DATAWTableModelODBCRowArraySize );
        if ( !SQL_SUCCEEDED( nReturn ) ) qWarning( "[%s][%s][%d] WARNING: Failed to set DATAStatement::RowArraySize.\n", __FILE__, __FUNCTION__, __LINE__ );

        nReturn = pStatement->setAttrRowStatusPtr( SQL_ATTR_ROW_STATUS_PTR, aRowStatus );
        if ( !SQL_SUCCEEDED( nReturn ) ) qWarning( "[%s][%s][%d] WARNING: Failed to set SQL_ATTR_ROW_STATUS_PTR.\n", __FILE__, __FUNCTION__, __LINE__ );

        nReturn = pStatement->setAttrConcurrency( DATAStatement::ConcurLock );
        if ( !SQL_SUCCEEDED( nReturn ) ) qWarning( "[%s][%s][%d] WARNING: Failed to set DATAStatement::ConcurLock.\n", __FILE__, __FUNCTION__, __LINE__ );
    }

    // bind cols
    // for now... everything is a SQL_C_CHAR with a 1k char max size
    {
        int nCol, nIndex;
        for ( nCol = 1, nIndex = 0; nCol <= listColumns.count(); nCol++, nIndex++ )
        {
            pStatement->doBindCol( nCol, SQL_C_CHAR, vectorRowBoundData[nIndex], 1000, vectorRowBoundDataInd[nIndex] );
            if ( !SQL_SUCCEEDED( nReturn ) )
                printf( "[%s][%s][%d] ERROR: Failed to bind column %d.\n", __FILE__, __FUNCTION__, __LINE__, nCol );
        }
    }

    // execute
    nReturn = pStatement->slotExecDirect( stringQuery );
    if ( !SQL_SUCCEEDED( nReturn ) )
        printf( "[%s][%s][%d] ERROR: Failed to execute: '%s'.\n", __FILE__, __FUNCTION__, __LINE__, stringQuery.toUtf8().constData() );

    // DATADiagnostic::getCursorRowCount is not reliable for SELECT so use SELECT COUNT(*) by calling DATAConnection::getRowCount
    nRows = pConnection->getRowCount( stringCatalog, stringSchema, stringTable );
    if ( nRows < 0 ) nRows = 0;
    nRowNew = nRows;    // index to new-record row
    nRows++;            // extra row at end is for new-record

    nCols = int(pStatement->getColumnCount());
    if ( nCols < 0 ) nCols = 0;

    nReturn = pStatement->doFetchScroll( SQL_FETCH_FIRST, 0 );
    if ( !SQL_SUCCEEDED( nReturn ) &&  nReturn != SQL_NO_DATA )
        printf( "[PAH][%s][%s][%d] Failed SQL_FETCH_FIRST.\n", __FILE__, __FUNCTION__, __LINE__ );

// printf( "[PAH][%s][%s][%d] %d\n", __FILE__, __FUNCTION__, __LINE__, aRowStatus[0] );

    // notify for those that want to display "Row of Rows" string
    // so leave off new-record row
    emit signalRowsData( nRows - 1 );
}

DATAWTableModelODBC::~DATAWTableModelODBC()
{
    char *ps;
    foreach( ps, vectorRowBoundData )
    {
        free( ps );
    }
    SQLLEN *pn;
    foreach( pn, vectorRowBoundDataInd )
    {
        free( pn );
    }
}

int DATAWTableModelODBC::rowCount( const QModelIndex & /* modelIndexParent */ ) const
{
    return nRows;
}

int DATAWTableModelODBC::columnCount( const QModelIndex & /* modelIndexParent */ ) const
{
    return nCols;
}

QVariant DATAWTableModelODBC::data( const QModelIndex &modelIndex, int nRole ) const
{
    if ( !modelIndex.isValid() ) return QVariant();

    // 0-based
    int nRow = modelIndex.row();
    int nCol = modelIndex.column();

// printf( "[PAH][%s][%s][%d] nRow=%d this->nRow=%d nRowEdit=%d nRowNew=%d\n", __FILE__, __FUNCTION__, __LINE__, nRow, this->nRow, nRowEdit, nRowNew );
    // editing and/or on new-record
    if ( nRow == nRowNew || nRow == nRowEdit )
    {
        switch ( nRole ) 
        {
            case Qt::DisplayRole:
            case Qt::EditRole:
                if ( nRow == nRowEdit )
                    return vectorDataEdit.at( nCol );
        }
        return QVariant();
    }

    switch ( nRole ) 
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
            // Do we care about aRowStatus ?
            // hopefully - SQL_ROW_SUCCESS, SQL_ROW_SUCCESS_WITH_INFO but SQL_ROW_ERROR, SQL_ROW_ADDED, SQL_ROW_UPDATED, SQL_ROW_DELETED, SQL_ROW_NOROW, SQL_ROW_PROCEED, SQL_ROW_IGNORE

            SQLRETURN nReturn = pStatement->doFetchScroll( SQL_FETCH_ABSOLUTE, (SQLLEN)nRow + 1 ); // 1-based
            if ( !SQL_SUCCEEDED( nReturn ) ) return QVariant();

            SQLLEN nInd = *(vectorRowBoundDataInd.at( nCol ));
            if ( nInd == SQL_NULL_DATA ) return QVariant();
            if ( nInd == SQL_NO_TOTAL ) return QVariant();
            if ( nInd < 0 ) return QVariant(); // error
            return QString( vectorRowBoundData.at( nCol ) );
    }

    return QVariant();
}

QVariant DATAWTableModelODBC::headerData( int nSection, Qt::Orientation nOrientation, int nRole ) const
{
    // Vertical
    if ( nOrientation == Qt::Vertical ) 
    {
        if ( nSection != nRows - 1 ) return QVariant();

        if ( nRole == Qt::DecorationRole )
        {
            return QIcon( ":DATAW/NewRow" ); 
        }

        return QVariant();
    }

    // horizontal
    if ( nSection >= listColumns.count() ) return QVariant();

    QString stringColumn = listColumns.at( nSection );

    // icon
    if ( nRole == Qt::DecorationRole )
    {
        if ( metaTable.metaPrimaryKey.mapColumns.contains( stringColumn ) )
        {
            return QIcon( ":DATAW/PrimaryKey" );
        }

        DATAWMetaForeignKeys m = metaTable.metaForeignKeys;
        if ( m.hasColumn( stringColumn  ) )
        {
            return QIcon( ":DATAW/ForeignKey" );
        }

        if ( metaTable.metaBestRowId.mapColumns.contains( stringColumn ) )
        {
            return QIcon( ":DATAW/ID" );
        }
        return QVariant();
    }

    // text
    if ( nRole == Qt::DisplayRole )
    {
        return stringColumn;
    }

    return QVariant();
}

bool DATAWTableModelODBC::setData( const QModelIndex &modelIndex, const QVariant &vValue, int nRole )
{
    if ( bReadOnly ) return false;
    if ( modelIndex.isValid() && nRole != Qt::EditRole ) return false;

    // just do everything in strings for now
    QString stringValue = vValue.toString();

    // 0-based
    int nRow = modelIndex.row();
    int nCol = modelIndex.column();

    // start edit
    if ( nRowEdit < 0 )
    {
        if ( nRow != nRowNew )
        {
            // init edit buffers (with copy of bound buffers when for UPDATE)
            for ( int nIndex = 0; nIndex < vectorRowBoundData.count(); nIndex++ )
            {
                if ( nRow != nRowNew ) vectorDataEdit[nIndex] = QString( vectorRowBoundData[nIndex] );
                vectorDataEditInd[nIndex] = SQL_COLUMN_IGNORE;
            }
        }
        nRowEdit = nRow;
        emit signalRowsAffected( 0 );
    }
    else if ( nRowEdit >= 0 && nRowEdit != nRow )
    {
        printf( "[PAH][%s][%s][%d] ERROR: Current row changed from a row with edits to another but without a doEditCommit or doEditRevert.\n", __FILE__, __FUNCTION__, __LINE__ );
        doEditRevert();
        return false;
    }

    // apply new col value to edit buffer
    vectorDataEdit[nCol]       = stringValue; 
    vectorDataEditInd[nCol]    = stringValue.size();
    emit signalEdits( true );

    return true;
}

Qt::ItemFlags DATAWTableModelODBC::flags( const QModelIndex &modelIndex ) const 
{
    if ( !modelIndex.isValid() ) return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags( modelIndex ) | Qt::ItemIsEditable;
}

bool DATAWTableModelODBC::insertRows( int nRow, int nCount, const QModelIndex &parentModelIndex )
{
    Q_UNUSED( parentModelIndex );

    // copy edit buffers to bound buffers
    for ( int nIndex = 0; nIndex < vectorDataEdit.count(); nIndex++ )
    {
        QString stringValue = vectorDataEdit.at( nIndex ).toString();
#ifdef Q_OS_WIN
        strncpy_s( vectorRowBoundData[nIndex], 1000, stringValue.toUtf8().constData(), stringValue.size() );
#else
        strncpy( vectorRowBoundData[nIndex], stringValue.toUtf8().constData(), stringValue.size() );
#endif
        *(vectorRowBoundDataInd[nIndex]) = vectorDataEditInd.at( nIndex );
    }

    SQLRETURN nReturn = pStatement->doBulkOperations( SQL_ADD );
    if ( !SQL_SUCCEEDED(nReturn) )
    {
        printf( "[PAH][%s][%s][%d] WARNING: SQLBulkOperations(SQL_ADD) did not return success. %d\n", __FILE__, __FUNCTION__, __LINE__, nReturn );
        return false;
    }
    nRows++;
    nRowNew++;
    emit signalRowsAffected( 1 );
    emit signalEdits( false );
    emit signalRowsData( nRows - 1 );

    // clear RowDataInd for all columns
    for ( int nIndex = 0; nIndex < vectorRowBoundDataInd.count(); nIndex++ ) *(vectorRowBoundDataInd[nIndex]) = SQL_COLUMN_IGNORE;

    // QModelIndex modelIndexLeft  = index( nRowEdit, 0 );
    // QModelIndex modelIndexRight = index( nRowEdit, nCols - 1 );
    nRowEdit = -1;

    beginInsertRows( QModelIndex(), nRow, nRow + nCount - 1 );
    endInsertRows();

    return true;
}

bool DATAWTableModelODBC::removeRows( int nRow, int nCount, const QModelIndex &parentModelIndex )
{
    Q_UNUSED( nCount );
    Q_UNUSED( parentModelIndex );

printf( "[PAH][%s][%s][%d] nRow=%d nCount=%d\n", __FILE__, __FUNCTION__, __LINE__, nRow, nCount );
    // can not delete a row that is being edited
    if ( nRow == nRowEdit ) return false;
    // can not delete new-record row
    if ( nRow == nRowNew ) return false;

    beginRemoveRows( QModelIndex(), nRow, nRow ); // 0-based
    SQLRETURN nReturn = pStatement->doSetPos( 1, SQL_DELETE, SQL_LOCK_NO_CHANGE ); // 1-based
    endRemoveRows();

    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        printf( "[PAH][%s][%s][%d] WARNING: SQLSetPos returned %d. Not success.\n", __FILE__, __FUNCTION__, __LINE__, nReturn );
        return false;
    }

    if ( aRowStatus[0] != SQL_ROW_DELETED )
    {
        printf( "[PAH][%s][%s][%d] WARNING: Result %d is not SQL_ROW_DELETED.\n", __FILE__, __FUNCTION__, __LINE__, aRowStatus[0] );
        return false;
    }

    nRowNew--;
    nRows--;
    emit signalRowsAffected( 1 );
    emit signalRowsData( nRows - 1 );

    return true;
}

void DATAWTableModelODBC::doEditCommit()
{
    if ( nRowEdit < 0 ) return;

    // INSERT
    if ( nRowEdit == nRowNew )
    {
        insertRows( nRowNew, 1, QModelIndex() );
        return;
    }

    // UPDATE
    // copy edit buffers to bound buffers
    for ( int nIndex = 0; nIndex < vectorDataEdit.count(); nIndex++ )
    {
        QString stringValue = vectorDataEdit.at( nIndex ).toString();
#ifdef Q_OS_WIN
        strncpy_s( vectorRowBoundData[nIndex], 1000, stringValue.toUtf8().constData(), stringValue.size() );
#else
        strncpy( vectorRowBoundData[nIndex], stringValue.toUtf8().constData(), stringValue.size() );
#endif
        *(vectorRowBoundDataInd[nIndex]) = vectorDataEditInd.at( nIndex );
    }

    // UPDATE
    if ( nRowEdit < nRowNew )
    {
        // here we assume the following;
        // - row-set has only 1 row
        // - the current cursor row is the same as the UI row at this time

        // app buffers to database (seems to bypass row-set and result-set) 
        SQLRETURN nReturn = pStatement->doSetPos( 1, SQL_UPDATE, SQL_LOCK_NO_CHANGE ); // pos in row-set, 1-based
        if ( !SQL_SUCCEEDED(nReturn) )
            printf( "[PAH][%s][%s][%d] WARNING: SQLSetPos(SQL_UPDATE) did not return success. %d\n", __FILE__, __FUNCTION__, __LINE__, nReturn );

        // refresh row-set row from result-set
        // pStatement->doSetPos( 1, SQL_REFRESH, SQL_LOCK_NO_CHANGE );

        // refresh row-set (and app buffers?) from result-set 
        // pStatement->doFetchScroll( SQL_FETCH_RELATIVE, (SQLLEN)0 );

    }
    else
    {
        printf( "[PAH][%s][%s][%d] ERROR: Invalid nRowEdit %d of %d\n", __FILE__, __FUNCTION__, __LINE__, nRowEdit, nRows );
        return;
    }

    // clear RowDataInd for all columns
    for ( int nIndex = 0; nIndex < vectorRowBoundDataInd.count(); nIndex++ ) *(vectorRowBoundDataInd[nIndex]) = SQL_COLUMN_IGNORE;

    QModelIndex modelIndexLeft  = index( nRowEdit, 0 );
    QModelIndex modelIndexRight = index( nRowEdit, nCols - 1 );
    nRowEdit = -1;
    emit signalRowsAffected( 1 );
    emit signalEdits( false );
    emit dataChanged( modelIndexLeft, modelIndexRight, { Qt::DisplayRole, Qt::EditRole } );

    return;
}

void DATAWTableModelODBC::doEditRevert()
{
    if ( nRowEdit < 0 ) return;
    QModelIndex modelIndexLeft  = index( nRowEdit, 0 );
    QModelIndex modelIndexRight = index( nRowEdit, nCols - 1 );
    nRowEdit = -1;
    emit signalEdits( false );
    emit dataChanged( modelIndexLeft, modelIndexRight, { Qt::EditRole } );
}


