#include "LibInfo.h"
#include "DATAWTableModelApp.h"

#include <DATAConnection.h>
#include <DATAStatement.h>
#include <DATAProfile.h>

DATAWTableModelApp::DATAWTableModelApp( QWidget *p, DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable )
    : DATAWTableModel( p, pConnection, stringCatalog, stringSchema, stringTable )
{
    doBuffersInitRowIds();  // determine what our rowid/key-set will be and load them all (also inits/reinits our state)
    doBuffersInit();        // init select buffers, init edit buffers
}

DATAWTableModelApp::~DATAWTableModelApp()
{
    if ( pStatement ) delete pStatement;
}

void DATAWTableModelApp::setFilter( int nIndex, const QString &stringValue )
{
    listFilters[nIndex] = stringValue;

    beginResetModel();

    doBuffersClear();
    doBuffersClearRowIds();
    doBuffersInitRowIds();

    emit signalEdits( false );
    emit signalRowsData( nRows - 1 );

    endResetModel();
}

int DATAWTableModelApp::rowCount( const QModelIndex & /* modelIndexParent */ ) const
{
    return nRows;
}

int DATAWTableModelApp::columnCount( const QModelIndex & /* modelIndexParent */ ) const
{
    return nCols;
}

QVariant DATAWTableModelApp::data( const QModelIndex &modelIndex, int nRole ) const
{
    if ( !modelIndex.isValid() ) return QVariant();

    // 0-based
    int nRow = modelIndex.row();
    int nCol = modelIndex.column();
    Q_ASSERT( nCol >= 0 );
//    Q_ASSERT( nCol < vectorReadOnly.count() );

//    if ( nRole == Qt::EditRole && vectorReadOnly[nCol] )
//   {
//        qWarning( "[PAH][%s][%s][%d] ReadOnly: %d\n", __FILE__, __FUNCTION__, __LINE__, nCol );
//        return QVariant();
//    }

// qWarning( "[PAH][%s][%s][%d] nRow=%d this->nRow=%d nRowEdit=%d nRowNew=%d\n", __FILE__, __FUNCTION__, __LINE__, nRow, this->nRow, nRowEdit, nRowNew );

    // editing and/or on new-record
    if ( nRow == nRowNew || nRow == nRowEdit )
    {
        switch ( nRole ) 
        {
            case Qt::DisplayRole:
            case Qt::EditRole:
                if ( nRow == nRowEdit )
                    return vectorDataEdit.at(nCol);
        }
        return QVariant();
    }

    // displaying
    switch ( nRole ) 
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
        if ( nRow != this->nRow ) doSelect(nRow);
        return vectorData.at( nCol );
    }

    return QVariant();
}

QVariant DATAWTableModelApp::headerData( int nSection, Qt::Orientation nOrientation, int nRole ) const
{
    // Vertical
    if ( nOrientation == Qt::Vertical ) 
    {
        // do we have a new-record row?
        if ( nRowNew < 0 ) return QVariant();
        // is this for the new-record row?
        if ( nSection != nRowNew ) return QVariant();
        // set icon
        if ( nRole == Qt::DecorationRole ) return QIcon( ":DATAW/NewRow" ); 

        return QVariant();
    }

    // horizontal
    // out of range?
    if ( nSection >= nCols ) return QVariant();

    QString stringColumn = metaTable.vectorColumnNames[nSection];

    // icon
    // set icon for columns of interest
    if ( nRole == Qt::DecorationRole )
    {
        // primary-key
        if ( metaTable.metaPrimaryKey.mapColumns.contains( stringColumn ) ) return QIcon( ":DATAW/PrimaryKey" );
        // foriegn-key?
        DATAWMetaForeignKeys m = metaTable.metaForeignKeys;
        if ( m.hasColumn( stringColumn  ) ) return QIcon( ":DATAW/ForeignKey" );
        // row-id
        if ( metaTable.metaBestRowId.mapColumns.contains( stringColumn ) ) return QIcon( ":DATAW/ID" );

        return QVariant();
    }

    // set text to column name
    if ( nRole == Qt::DisplayRole )
    {
        return stringColumn;
    }

    return QVariant();
}

bool DATAWTableModelApp::setData( const QModelIndex &modelIndex, const QVariant &vValue, int nRole )
{
    if ( bReadOnly ) return false;
    if ( modelIndex.isValid() && nRole != Qt::EditRole ) return false;

    // just do everything in strings for now
    QString stringValue = vValue.toString();

    // 0-based
    int nRow = modelIndex.row();
    int nCol = modelIndex.column();
    Q_ASSERT( nCol >= 0 );

    // start edit
    if ( nRowEdit < 0 )
    {
        doEditInit( nRow );
    }
    else if ( nRowEdit >= 0 && nRowEdit != nRow )
    {
        qWarning( "[PAH][%s][%s][%d] ERROR: Current row changed from a row with edits to another but without a doEditCommit or doEditRevert.\n", __FILE__, __FUNCTION__, __LINE__ );
        doEditRevert();
        return false;
    }

    // apply new col value to edit buffer
    vectorDataEdit[nCol]       = stringValue; 
    vectorDataEditInd[nCol]    = stringValue.size();
    emit signalEdits( true );

    return true;
}

bool DATAWTableModelApp::removeRows( int nRow, int nCount, const QModelIndex &parentModelIndex )
{
    Q_UNUSED( nCount );
    Q_UNUSED( parentModelIndex );

    return doDelete( nRow );
}

void DATAWTableModelApp::doEditCommit()
{
    if ( !isEdit() ) return;

    // INSERT
    if ( nRowEdit == nRowNew )
    {
        doInsert();
        return;
    }

    // UPDATE
    doUpdate();
    return;
}

void DATAWTableModelApp::doEditRevert()
{
    if ( !isEdit() ) return;

    for ( int nCol = 0; nCol < nCols; nCol++ )
    {
        vectorDataEdit[nCol]     = QVariant();
        vectorDataEditInd[nCol]  = SQL_COLUMN_IGNORE;
    }

    QModelIndex modelIndexLeft  = index( nRowEdit, 0 );
    QModelIndex modelIndexRight = index( nRowEdit, nCols - 1 );
    nRowEdit = -1;
    emit signalEdits( false );
    emit dataChanged( modelIndexLeft, modelIndexRight, { Qt::EditRole } );
}

void DATAWTableModelApp::doReset()
{
    beginResetModel();

    doBuffersClear();
    doBuffersClearRowIds();
    doBuffersInitRowIds();

    emit signalEdits( false );
    emit signalRowsData( nRows - 1 );

    endResetModel();
}

// todo: use a prepared statement
bool DATAWTableModelApp::doSelect( int n ) const
{
    // avoid fetching row just because we changed column
    if ( n == nRow ) return true;

    nRow = n;

    QString stringSQL = getSQLSelect( nRow );
    if ( stringSQL.isEmpty() ) return false;

    // CLEAR ROW 
    // - for case when we fail to get new data
    doBuffersClear( true, false );

    // EXECUTE
    DATAStatement *pStatement = new DATAStatement( pConnection );
    SQLRETURN nReturn = pStatement->slotExecDirect( stringSQL );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        printf( "[%s][%s][%d] Failed to execute: '%s'.\n", __FILE__, __FUNCTION__, __LINE__, stringSQL.toUtf8().constData() );
        bReadOnly = true;
        emit signalReadOnly( true );
        delete pStatement;
        return false;
    }
    // FETCH ROW
    nReturn = pStatement->doFetch();
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        printf( "[%s][%s][%d] Expected 1 row and none fetched.\n", __FILE__, __FUNCTION__, __LINE__ );
        bReadOnly = true;
        emit signalReadOnly( true );
        delete pStatement;
        return false;
    }

    // GET COLUMN VALUES
    for ( int nCol = 0; nCol < nCols; nCol++ ) { vectorData[nCol] = pStatement->getData( nCol + 1 ); }

    delete pStatement;
    return true;
}

bool DATAWTableModelApp::doInsert()
{
    // sanity checks just for development
#ifdef QT_DEBUG
    if ( bReadOnly )
    {
        qWarning( "[PAH][%s][%s][%d] Can not insert - bReadOnly = %d.\n", __FILE__, __FUNCTION__, __LINE__, bReadOnly );
        return false;
    }
    if ( nRowNew < 0 )
    {
        qWarning( "[PAH][%s][%s][%d] Can not insert - nRowNew = %d.\n", __FILE__, __FUNCTION__, __LINE__, nRowNew );
        return false;
    }
    if ( nRowEdit != nRowNew )
    {
        qWarning( "[PAH][%s][%s][%d] Can not insert - nRowEdit %d should be nRowNew %d.\n", __FILE__, __FUNCTION__, __LINE__, nRowEdit, nRowNew );
        return false;
    }
#endif

    QString stringSQL = getSQLInsert();
    if ( stringSQL.isEmpty() ) return false;


    // execute the INSERT
    DATAStatement *pStatement = new DATAStatement( pConnection );
    SQLRETURN nReturn = pStatement->slotExecDirect( stringSQL );
    if ( !SQL_SUCCEEDED( nReturn ) ) 
    {
        qWarning( "[PAH][%s][%s][%d] Insert failed: %s\n", __FILE__, __FUNCTION__, __LINE__, stringSQL.toUtf8().constData() );
        delete pStatement;
        emit signalRowsAffected( 0 );
        return false;
    }
    emit signalRowsAffected( 1 );

    delete pStatement;
    pStatement = nullptr;

    // we could use DBMS specific ways to get rowid from last insert but...
    // reload all rowids
    // bonus; allows us to pickup some changes from elsewhere (depending upon transaction isolation)
    doReset();

    return true;
}

bool DATAWTableModelApp::doUpdate()
{
    QMap<int,QVariant>  mapKeyValues    = getUpdatedKeyValues();    // nCol,Value - so we can update our vectorRowIds
    QString             stringSQL       = getSQLUpdate();
    if ( stringSQL.isEmpty() ) return false;

    // EXECUTE
    DATAStatement *pStatement = new DATAStatement( pConnection );
    SQLRETURN nReturn = pStatement->slotExecDirect( stringSQL );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        qWarning( "[%s][%s][%d] Failed to execute: '%s'.\n", __FILE__, __FUNCTION__, __LINE__, stringSQL.toUtf8().constData() );
        delete pStatement;
        emit signalRowsAffected( 0 );
        return false;
    }
    emit signalRowsAffected( 1 );
    delete pStatement;
    pStatement = nullptr;

    // SQLite - _ROWID_ *could* be an alias for the PK which appears as a different column.
    //          So we could have a PK being updated and getUpdatedKeyValues() not realizing that its the row-id.
    //          To be safe; we reload all row-id's whenever a PK is updated.
    QString stringDBMS_NAME = pConnection->getInfo( SQL_DBMS_NAME, nullptr ).toString();
    if ( stringDBMS_NAME == "SQLite" && bUpdateIncludesPK )
    {
        doReset();
        return true;
    }

    // update RowIds
    for ( auto i = mapKeyValues.cbegin(), end = mapKeyValues.cend(); i != end; ++i )
    {
        vectorRowIds[nRowEdit][i.key()] = i.value();
    }

    QModelIndex modelIndexLeft  = index( nRowEdit, 0 );
    QModelIndex modelIndexRight = index( nRowEdit, nCols - 1 );
    nRowEdit = -1;
    emit signalEdits( false );
    emit dataChanged( modelIndexLeft, modelIndexRight, { Qt::DisplayRole, Qt::EditRole } );

    return true;
}

bool DATAWTableModelApp::doDelete( int nRow )
{
    if ( nRow == nRowEdit )
    {
        qWarning( "[PAH][%s][%s][%d] Can not delete a row that is being edited.\n", __FILE__, __FUNCTION__, __LINE__ );
        return false;
    }
    if ( nRow == nRowNew )
    {
        qWarning( "[PAH][%s][%s][%d] Can not delete new-record row.\n", __FILE__, __FUNCTION__, __LINE__ );
        return false;
    }

    QString stringSQL = getSQLDelete( nRow );
    if ( stringSQL.isEmpty() ) return false;

    // EXECUTE
    DATAStatement *pStatement = new DATAStatement( pConnection );
    SQLRETURN nReturn = pStatement->slotExecDirect( stringSQL );
    if ( !SQL_SUCCEEDED( nReturn ) )
    {
        qWarning( "[%s][%s][%d] Failed to execute: '%s'.\n", __FILE__, __FUNCTION__, __LINE__, stringSQL.toUtf8().constData() );
        delete pStatement;
        emit signalRowsAffected( 0 );
        return false;
    }

    beginRemoveRows( QModelIndex(), nRow, nRow ); // 0-based

    emit signalRowsAffected( 1 );
    delete pStatement;
    pStatement = nullptr;

    nRowNew--;
    nRows--;
    emit signalRowsData( nRows - 1 );

    vectorRowIds.remove( nRow );

    endRemoveRows();

    return true;
}

QString DATAWTableModelApp::getSQLSelect( int nRow ) const
{
    QString stringSQL           = "SELECT";
    QString stringSQLColumns    = getSQLColumns();
    QString stringSQLWhere      = getSQLWhere( nRow );

    if ( stringSQLColumns.isEmpty() ) return QString();
    if ( stringSQLWhere.isEmpty() ) return QString();

    return stringSQL + " " + stringSQLColumns + " FROM " + stringTable + " WHERE " + stringSQLWhere;
}

QString DATAWTableModelApp::getSQLInsert()
{
    QString stringSQL           = "INSERT INTO";
    QString stringSQLColumns    = getSQLColumns( true );
    QString stringSQLValues     = getSQLValues();

    if ( stringSQLColumns.isEmpty() ) return QString();
    if ( stringSQLValues.isEmpty() ) return QString();

    return stringSQL + + " " + stringTable + " ( " + stringSQLColumns + " ) VALUES ( " + stringSQLValues + " )";
}

QString DATAWTableModelApp::getSQLUpdate()
{
    QString stringSQL               = "UPDATE";
    QString stringSQLColumnValues   = getSQLColumnValues();
    QString stringSQLWhere          = getSQLWhere( nRowEdit );

    if ( stringSQLColumnValues.isEmpty() ) return QString();
    if ( stringSQLWhere.isEmpty() ) return QString();

    // FYI: SQLite does not like () around column-values
    // return stringSQL + " " + stringTable + " SET ( " + stringSQLColumnValues + " ) WHERE " + stringSQLWhere;
    return stringSQL + " " + stringTable + " SET " + stringSQLColumnValues + " WHERE " + stringSQLWhere;
}

QString DATAWTableModelApp::getSQLDelete( int nRow )
{
    QString stringSQL           = "DELETE";
    QString stringSQLWhere      = getSQLWhere( nRow );

    if ( stringSQLWhere.isEmpty() ) return QString();

    return stringSQL + " FROM " + stringTable + " WHERE " + stringSQLWhere;
}

// for SELECT and INSERT
QString DATAWTableModelApp::getSQLColumns( bool bInsert ) const
{
    QString stringSQL;

    for ( int nCol = 0; nCol < nCols; nCol++ )
    {
        QString stringColumnName = metaTable.vectorColumnNames.at( nCol );
        if ( bInsert )
        {
            DATAWMetaColumn metaColumn = metaTable.mapColumns.value( stringColumnName );
            if ( metaColumn.bReadOnly ) continue;
        }

        if ( !stringSQL.isEmpty() ) stringSQL += ", ";
        stringSQL += stringColumnName;
    }

    return stringSQL;
}

// for UPDATE
QString DATAWTableModelApp::getSQLColumnValues() const
{
    QString stringSQL;

    // We need to get DATADataTypeSpec for each column to determine prefix/suffix for column values.
    // Use TYPE_NAME to get DATADataTypeSpec but careful about case sensitivity
    // Note sure if data source specific data types will cause a problem.
    // SQLColumns: DATA_TYPE = SQL concise type ie SQL_INTERVAL_YEAR_TO_MONTH
    // SQLGetTypeInfo: DATA_TYPE 

    DATAProfile *pProfile = pConnection->getProfile();
    if ( !pProfile ) 
    {
        qWarning( "[PAH][%s][%s][%d] pProfile == null\n", __FILE__, __FUNCTION__, __LINE__ );
        return QString();
    }

    for ( int nCol = 0; nCol < nCols; nCol++ )
    {
        if ( vectorDataEditInd[nCol] == SQL_COLUMN_IGNORE ) continue;

        // get COLUMN_NAME for nCol
        QString stringCOLUMN_NAME = metaTable.vectorColumnNames.at( nCol );
        if ( stringCOLUMN_NAME.isEmpty() ) 
        {
            qWarning( "[PAH][%s][%s][%d] Could not find COLUMN_NAME for %d column.\n", __FILE__, __FUNCTION__, __LINE__, nCol );
            return QString();
        }

        // get DATA_TYPE for COLUMN_NAME
        if ( !metaTable.mapColumns.contains( stringCOLUMN_NAME ) ) 
        {
            qWarning( "[PAH][%s][%s][%d] Could not find DATAWMetaColumn for %s.\n", __FILE__, __FUNCTION__, __LINE__, stringCOLUMN_NAME.toUtf8().constData() );
            return QString();
        }

        DATAWMetaColumn metaColumn = metaTable.mapColumns.value( stringCOLUMN_NAME );

        // get LITERAL_PREFIX and LITERAL_SUFFIX for TYPE_NAME
        DATADataTypeSpec *pTypeSpec = pProfile->getDataTypeSpec( metaColumn.stringTYPE_NAME );
        if ( !pTypeSpec ) 
        {
            qWarning( "[PAH][%s][%s][%d] WARNING: Could not find DATADataTypeSpec where TYPE_NAME %s.\n", __FILE__, __FUNCTION__, __LINE__, metaColumn.stringTYPE_NAME.toUtf8().constData() );
            return QString();
        }

        // add comma for additional columns
        if ( !stringSQL.isEmpty() ) stringSQL += ", ";
        stringSQL += stringCOLUMN_NAME + " = ";

        // value
        // no prefix than numeric (for now)
        if ( pTypeSpec->LITERAL_PREFIX.isEmpty() )
        {
            if ( vectorDataEdit.at( nCol ).toString().isEmpty() )
                stringSQL += "NULL";
            else
                stringSQL += vectorDataEdit.at( nCol ).toString();
        }
        else
        {
            if ( !pTypeSpec->LITERAL_PREFIX.isEmpty() ) stringSQL += pTypeSpec->LITERAL_PREFIX;
            stringSQL += vectorDataEdit.at( nCol ).toString();
            if ( !pTypeSpec->LITERAL_SUFFIX.isEmpty() ) stringSQL += pTypeSpec->LITERAL_SUFFIX;
        }
    }

    return stringSQL;
}

// for INSERT
QString DATAWTableModelApp::getSQLValues() const
{
    QString stringSQL;

    DATAProfile *pProfile = pConnection->getProfile();
    if ( !pProfile ) 
    {
        printf( "[PAH][%s][%s][%d] pProfile == null\n", __FILE__, __FUNCTION__, __LINE__ );
        return QString();
    }

    for ( int nCol = 0; nCol < nCols; nCol++ )
    {
        // get COLUMN_NAME for nCol
        QString stringCOLUMN_NAME = metaTable.vectorColumnNames.at( nCol );
        if ( stringCOLUMN_NAME.isEmpty() ) 
        {
            printf( "[PAH][%s][%s][%d] Could not find COLUMN_NAME for %d column.\n", __FILE__, __FUNCTION__, __LINE__, nCol );
            return QString();
        }

        // get DATA_TYPE for COLUMN_NAME
        if ( !metaTable.mapColumns.contains( stringCOLUMN_NAME ) ) 
        {
            printf( "[PAH][%s][%s][%d] Could not find DATAWMetaColumn for %s.\n", __FILE__, __FUNCTION__, __LINE__, stringCOLUMN_NAME.toUtf8().constData() );
            return QString();
        }

        DATAWMetaColumn metaColumn = metaTable.mapColumns.value( stringCOLUMN_NAME );
        if ( metaColumn.bReadOnly ) continue;

        // get LITERAL_PREFIX and LITERAL_SUFFIX for TYPE_NAME
        DATADataTypeSpec *pTypeSpec = pProfile->getDataTypeSpec( metaColumn.stringTYPE_NAME );
        if ( !pTypeSpec ) 
        {
            qWarning( "[PAH][%s][%s][%d] Could not find DATADataTypeSpec where TYPE_NAME %s.\n", __FILE__, __FUNCTION__, __LINE__, metaColumn.stringTYPE_NAME.toUtf8().constData() );
            return QString();
        }

        // add comma for additional columns
        if ( !stringSQL.isEmpty() ) stringSQL += ", ";

        if ( vectorDataEditInd[nCol] == SQL_COLUMN_IGNORE ) 
            stringSQL += "NULL";
        else
        {
            // value
            // no prefix than numeric (for now)
            if ( pTypeSpec->LITERAL_PREFIX.isEmpty() )
            {
                if ( vectorDataEdit.at( nCol ).toString().isEmpty() )
                    stringSQL += "NULL";
                else
                    stringSQL += vectorDataEdit.at( nCol ).toString();
            }
            else
            {
                if ( !pTypeSpec->LITERAL_PREFIX.isEmpty() ) stringSQL += pTypeSpec->LITERAL_PREFIX;
                stringSQL += vectorDataEdit.at( nCol ).toString();
                if ( !pTypeSpec->LITERAL_SUFFIX.isEmpty() ) stringSQL += pTypeSpec->LITERAL_SUFFIX;
            }
        }
    }

    return stringSQL;
}

QString DATAWTableModelApp::getSQLWhere( int nRow ) const
{
    QString stringSQL;

    DATAProfile *pProfile = pConnection->getProfile();
    if ( !pProfile ) 
    {
        printf( "[PAH][%s][%s][%d] pProfile == null\n", __FILE__, __FUNCTION__, __LINE__ );
        return QString();
    }

    for ( int nCol = 0; nCol < listRowIdColumnNames.count(); nCol++ )
    {
        // get COLUMN_NAME for nCol
        QString stringCOLUMN_NAME = listRowIdColumnNames.at( nCol );
        if ( stringCOLUMN_NAME.isEmpty() ) 
        {
            printf( "[PAH][%s][%s][%d] Could not find COLUMN_NAME for %d column.\n", __FILE__, __FUNCTION__, __LINE__, nCol );
            return QString();
        }

        if ( nCol ) stringSQL += " AND ";
        stringSQL += stringCOLUMN_NAME + " = ";

        // get TYPE_NAME for COLUMN_NAME
        QString     stringTYPE_NAME;
        SQLSMALLINT nDATA_TYPE;
        if ( metaTable.nUnique == DATAWMetaTable::UniqueRowId )
        {
            if ( !metaTable.metaBestRowId.mapColumns.contains( stringCOLUMN_NAME ) )
            {
                printf( "[PAH][%s][%s][%d] Could not find DATAWMetaSpecialColumn for %s.\n", __FILE__, __FUNCTION__, __LINE__, stringCOLUMN_NAME.toUtf8().constData() );
                return QString();
            }
            QMap<QString,DATAWMetaSpecialColumn> mapColumns = metaTable.metaBestRowId.mapColumns;
            stringTYPE_NAME = mapColumns.value( stringCOLUMN_NAME ).stringTYPE_NAME;
            nDATA_TYPE      = mapColumns.value( stringCOLUMN_NAME ).nDATA_TYPE;
        }
        else
        {
            QMap<QString,DATAWMetaColumn> mapColumns = metaTable.mapColumns; 
            if ( !mapColumns.contains( stringCOLUMN_NAME ) )
            {
                printf( "[PAH][%s][%s][%d] Could not find DATAWMetaColumn for %s.\n", __FILE__, __FUNCTION__, __LINE__, stringCOLUMN_NAME.toUtf8().constData() );
                return QString();
            }
            DATAWMetaColumn metaColumn = mapColumns.value( stringCOLUMN_NAME );
            stringTYPE_NAME = metaColumn.stringTYPE_NAME;
            nDATA_TYPE = metaColumn.nDATA_TYPE;
        }

        // get LITERAL_PREFIX and LITERAL_SUFFIX for TYPE_NAME
        DATADataTypeSpec *pTypeSpec = pProfile->getDataTypeSpec( stringTYPE_NAME );
        if ( !pTypeSpec ) 
        {
            qWarning( "[PAH][%s][%s][%d] Could not find DATADataTypeSpec where TYPE_NAME %s.\n", __FILE__, __FUNCTION__, __LINE__, stringTYPE_NAME.toUtf8().constData() );
            return QString();
        }

        // prefix
        if ( !pTypeSpec->LITERAL_PREFIX.isNull() )
        {
            stringSQL += pTypeSpec->LITERAL_PREFIX;
        }

        // value
        stringSQL += vectorRowIds.at( nRow ).at( nCol ).toString();

        // suffix
        if ( !pTypeSpec->LITERAL_SUFFIX.isNull() )
        {
            stringSQL += pTypeSpec->LITERAL_SUFFIX;
        }
    }

    return stringSQL;
}

QString DATAWTableModelApp::getSQLFilter() const
{
    QString stringSQL;

    DATAProfile *pProfile = pConnection->getProfile();
    if ( !pProfile ) 
    {
        printf( "[PAH][%s][%s][%d] pProfile == null\n", __FILE__, __FUNCTION__, __LINE__ );
        return QString();
    }

    for ( int nCol = 0; nCol < nCols; nCol++ )
    {
        QString stringValue = listFilters.at( nCol );
        if ( stringValue.isEmpty() ) continue;

        QString stringCOLUMN_NAME = metaTable.vectorColumnNames.at( nCol );
        if ( stringCOLUMN_NAME.isEmpty() ) 
        {
            printf( "[PAH][%s][%s][%d] Could not find COLUMN_NAME for %d column.\n", __FILE__, __FUNCTION__, __LINE__, nCol );
            return QString();
        }

        // get DATA_TYPE for COLUMN_NAME
        if ( !metaTable.mapColumns.contains( stringCOLUMN_NAME ) ) 
        {
            printf( "[PAH][%s][%s][%d] Could not find DATAWMetaColumn for %s.\n", __FILE__, __FUNCTION__, __LINE__, stringCOLUMN_NAME.toUtf8().constData() );
            return QString();
        }

        DATAWMetaColumn metaColumn = metaTable.mapColumns.value( stringCOLUMN_NAME );

        // get LITERAL_PREFIX and LITERAL_SUFFIX for TYPE_NAME
        DATADataTypeSpec *pTypeSpec = pProfile->getDataTypeSpec( metaColumn.stringTYPE_NAME );
        if ( !pTypeSpec ) 
        {
            qWarning( "[PAH][%s][%s][%d] Could not find DATADataTypeSpec where TYPE_NAME %s.\n", __FILE__, __FUNCTION__, __LINE__, metaColumn.stringTYPE_NAME.toUtf8().constData() );
            return QString();
        }

        // add AND for additional columns
        if ( !stringSQL.isEmpty() ) stringSQL += " AND ";
        stringSQL += stringCOLUMN_NAME + " = ";

        // value
        // no prefix than numeric (for now)
        if ( pTypeSpec->LITERAL_PREFIX.isEmpty() )
        {
            stringSQL += stringValue;
        }
        else
        {
            if ( !pTypeSpec->LITERAL_PREFIX.isEmpty() ) stringSQL += pTypeSpec->LITERAL_PREFIX;
            stringSQL += stringValue;
            if ( !pTypeSpec->LITERAL_SUFFIX.isEmpty() ) stringSQL += pTypeSpec->LITERAL_SUFFIX;
        }
    }

    return stringSQL;
}

QMap<int,QVariant> DATAWTableModelApp::getUpdatedKeyValues()
{
    QMap<int,QVariant> map;
    bUpdateIncludesPK = false;

    for ( int nCol = 0; nCol < nCols; nCol++ )
    {
        if ( vectorDataEditInd[nCol] == SQL_COLUMN_IGNORE ) continue;
        QString stringCOLUMN_NAME = metaTable.vectorColumnNames.at( nCol );
        if ( stringCOLUMN_NAME.isEmpty() ) 
            printf( "[PAH][%s][%s][%d] Could not find COLUMN_NAME for %d column.\n", __FILE__, __FUNCTION__, __LINE__, nCol );
        if ( metaTable.metaPrimaryKey.mapColumns.contains( stringCOLUMN_NAME ) ) bUpdateIncludesPK = true;
        if ( !listRowIdColumnNames.contains( stringCOLUMN_NAME ) ) continue;
        map.insert( nCol, vectorDataEdit.at( nCol ) );

    }

    return map;
}

void DATAWTableModelApp::doEditInit( int nRow )
{
    // for UPDATE?
    if ( nRow != nRowNew )
    {
        // refresh row
        doSelect( nRow );
        // copy to edit buffers
        for ( int nCol = 0; nCol < nCols; nCol++ )
        {
            if ( nRow != nRowNew ) vectorDataEdit[nCol] = vectorData[nCol];
            vectorDataEditInd[nCol]  = SQL_COLUMN_IGNORE;
        }
    }
    nRowEdit = nRow;
    emit signalRowsAffected( 0 );
}

/*!
 * \brief Init I/O buffers.
 * 
 * We init one for each column and use ind=SQL_COLUMN_IGNORE to decide which to bind.
 * Column order is important and is always the same as returned by SQLColumns.
 * Hidden columns are not included - just those returned by SQLColumn.
 * 
 * \author pharvey (2/24/25)
 */
void DATAWTableModelApp::doBuffersInit()
{
    for ( int nCol = 0; nCol < nCols; nCol++ )
    {
        listFilters          << QString();
        vectorData           << QVariant();
        vectorDataInd        << SQL_COLUMN_IGNORE;
        vectorDataEdit       << QVariant();
        vectorDataEditInd    << SQL_COLUMN_IGNORE;
    }
}

void DATAWTableModelApp::doBuffersClear( bool bData, bool bDataEdit ) const
{
    for ( int nCol = 0; nCol < nCols; nCol++ )
    {
        if ( bData )
        {
            vectorData[nCol]    = QVariant();
            vectorDataInd[nCol] = SQL_COLUMN_IGNORE;
        }
        if ( bDataEdit )
        {
            vectorDataEdit[nCol]     = QVariant();
            vectorDataEditInd[nCol]  = SQL_COLUMN_IGNORE;
        }
    }
}

void DATAWTableModelApp::doBuffersInitRowIds()
{
    // get row-id column names (if any)
    switch ( metaTable.nUnique )
    {
        case DATAWMetaTable::UniqueRowId:
            listRowIdColumnNames = metaTable.metaBestRowId.mapColumns.keys();
            if ( listRowIdColumnNames.isEmpty() )
            {
                qWarning( "[PAH][%s][%s][%d] Could not find rowid columns. Failed to determine rowid.\n", __FILE__, __FUNCTION__, __LINE__ );
            }
            break;
        case DATAWMetaTable::UniquePrimaryKey:
            listRowIdColumnNames = metaTable.metaPrimaryKey.mapColumns.keys();
            if ( listRowIdColumnNames.isEmpty() )
            {
                qWarning( "[PAH][%s][%s][%d] Could not find primary key columns. Failed to determine rowid.\n", __FILE__, __FUNCTION__, __LINE__ );
            }
            break;
        case DATAWMetaTable::UniqueIndex:
            {
                QString stringINDEX_NAME = metaTable.metaIndexes.getUnique();
                QMap<QString,DATAWMetaIndex> mapIndexes = metaTable.metaIndexes.mapIndexes;
                DATAWMetaIndex meta = mapIndexes.value( stringINDEX_NAME );
                listRowIdColumnNames = meta.mapColumns.keys();
                if ( listRowIdColumnNames.isEmpty() )
                {
                    qWarning( "[PAH][%s][%s][%d] Could not find index [%s]. Failed to determine rowid.\n", __FILE__, __FUNCTION__, __LINE__, stringINDEX_NAME.toUtf8().constData() );
                }
            }
            break;
        case DATAWMetaTable::UniqueNone:
            // here we could use all columns and "UPDATE/DELETE with LIMIT 1
            // unfortunately not all data sources will support LIMIT
            // to be safe we switch to readonly
            bReadOnly = true;
            emit signalReadOnly( true );
            listRowIdColumnNames = metaTable.vectorColumnNames;
            if ( listRowIdColumnNames.isEmpty() )
            {
                qWarning( "[PAH][%s][%s][%d] Could not find columns. Failed to determine rowid.\n", __FILE__, __FUNCTION__, __LINE__ );
            }
            break;
    }

    // build SQL to get row-ids
    // we assume that values are numeric or string so we treat as a string
    // this means no need to bind buffers
    QString stringSQL = "SELECT ";
    {
        for ( int nIndex = 0; nIndex < listRowIdColumnNames.count(); nIndex++ )
        {
            if ( nIndex ) stringSQL += ", ";
            stringSQL += listRowIdColumnNames.at( nIndex );
        }
        stringSQL += " FROM " + stringTable;
    }

    // filters
    QString stringFilter = getSQLFilter();
    if ( !stringFilter.isEmpty() ) stringSQL += " WHERE " + stringFilter;

    // fetch all row-ids
    {
        nRows = 0;
        DATAStatement *pStatement = new DATAStatement( pConnection );
        SQLRETURN nReturn = pStatement->slotExecDirect( stringSQL );
        if ( SQL_SUCCEEDED( nReturn ) )
        {
            nReturn = pStatement->doFetch();
            while ( SQL_SUCCEEDED( nReturn ) )
            {
                nRows++;
                QVector<QVariant> vectorRow;
                for ( int n = 1; n <= listRowIdColumnNames.count(); n++ )
                {
                    vectorRow << pStatement->getData( n );
                }
                vectorRowIds << vectorRow;
                nReturn = pStatement->doFetch();
            }
        }
        else
        {
            bReadOnly = true;
            emit signalReadOnly( true );
            qWarning( "[%s][%s][%d] Failed to execute: '%s'.\n", __FILE__, __FUNCTION__, __LINE__, stringSQL.toUtf8().constData() );
        }
        delete pStatement;
    }

    emit signalRowsData( nRows );

    // !!! INIT/REINIT STATE !!!

    // rows
    nRowEdit = -1;
    if ( bReadOnly )
    {
        nRowNew = -1;
    }
    else
    {
        // add new-record row
        nRowNew = nRows;    // index to new-record row
        nRows++;            // extra row at end is for new-record
    }
    // cols
    nCols = metaTable.mapColumns.count();
}

void DATAWTableModelApp::doBuffersClearRowIds()
{
    listRowIdColumnNames.clear();
    vectorRowIds.clear();
}

