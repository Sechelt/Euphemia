#ifndef H_DATAWTableModelApp
#define H_DATAWTableModelApp

#include "DATAWTableModel.h"

class DATAWTableModelApp : public  DATAWTableModel
{
    Q_OBJECT
public:
    DATAWTableModelApp( QWidget *p, DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable );
    ~DATAWTableModelApp();

    void setFilter( int nIndex, const QString &stringValue ) override;

    virtual int rowCount( const QModelIndex &modelIndexParent = QModelIndex() ) const override;
    virtual int columnCount( const QModelIndex &modelIndexParent = QModelIndex() ) const override;
    virtual QVariant data( const QModelIndex &modelIndex, int nRole = Qt::DisplayRole ) const override;
    virtual QVariant headerData( int nSection, Qt::Orientation nOrientation, int nRole = Qt::DisplayRole ) const override;
    virtual bool setData( const QModelIndex &modelIndex, const QVariant &vValue, int nRole = Qt::EditRole ) override;

    virtual bool removeRows( int nRow, int nCount, const QModelIndex &parentModelIndex = QModelIndex() ) override;

    void doEditCommit() override;
    void doEditRevert() override;

protected:
    bool bUpdateIncludesPK = false;                     // used when SQLite (perhaps for others)
    // row from last doSelect
    mutable int nRow  = -1;                             // not a current row - just used to avoid some selects
    mutable QVector<QVariant>   vectorData;
    mutable QVector<SQLLEN>     vectorDataInd;

    // row id's
    QList<QString>              listRowIdColumnNames;   // rowid column names for building WHERE etc
    QVector<QVector<QVariant>>  vectorRowIds;           // all rowids in table are loaded into this - rows then columns

    // operates on model
    void doReset();
    // these operate on a row
    bool doSelect( int nRow ) const;                // SELECTs a row into vectorRow (for given nRow, adopts nRow)
    bool doInsert();                                // INSERTs a row using vectorRowEditData (nRowEdit/nRowNew)
    bool doUpdate();                                // UPDATEs a row using vectorRowEditData (nRowEdit)
    bool doDelete( int nRow );                      // DELETEs row (given row)
    // to support above                             
    QString getSQLSelect( int nRow ) const;         // builds SQL for doSelect (for given nRow) 
    QString getSQLInsert();                         // builds SQL for doInsert (nRowEdit/nRowNew)
    QString getSQLUpdate();                         // builds SQL for doUpdate (nRowEdit)
    QString getSQLDelete( int nRow );               // builds SQL for doDelete
    // to support above                             
    QString getSQLColumns( bool bInsert = false ) const;    // builds SQL (column,...) for getSQLSelect and getSQLInsert (row n/a)
    QString getSQLColumnValues() const;             // builds SQL (calumn=value,...) for getSQLUpdate (nRowEdit)
    QString getSQLValues() const;                   // builds SQL (value,...) for getSQLInsert (nRowEdit/nRowNew)
    QString getSQLWhere( int nRow ) const;          // builds SQL (condition AND,...) for getSQLSelect, getSQLUpdate, and getSQLDelete (for given nRow)
    QString getSQLFilter() const;                   // builds SQL (condition AND,...) for doBuffersInitRowIds

    QMap<int,QVariant> getUpdatedKeyValues();

    void doEditInit( int nRow );

    void doBuffersInit();
    void doBuffersClear( bool bData = true, bool bDataEdit = true ) const;
    void doBuffersInitRowIds();
    void doBuffersClearRowIds();
    void doBuffersResetRowIds();
};

#endif


