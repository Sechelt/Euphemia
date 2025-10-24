#ifndef H_DATAWTableModelODBC
#define H_DATAWTableModelODBC

#include "DATAWTableModel.h"

#define DATAWTableModelODBCRowArraySize 1

class DATAWTableModelODBC : public  DATAWTableModel
{
    Q_OBJECT
public:
    DATAWTableModelODBC( QWidget *p, DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable );
    ~DATAWTableModelODBC();

    virtual int rowCount( const QModelIndex &modelIndexParent = QModelIndex() ) const override;
    virtual int columnCount( const QModelIndex &modelIndexParent = QModelIndex() ) const override;
    virtual QVariant data( const QModelIndex &modelIndex, int nRole = Qt::DisplayRole ) const override;
    virtual QVariant headerData( int nSection, Qt::Orientation nOrientation, int nRole = Qt::DisplayRole ) const override;
    virtual bool setData( const QModelIndex &modelIndex, const QVariant &vValue, int nRole = Qt::EditRole ) override;
    virtual Qt::ItemFlags flags( const QModelIndex &modelIndex ) const override;
    virtual bool insertRows( int nRow, int nCount, const QModelIndex &parentModelIndex ) override;
    virtual bool removeRows( int nRow, int nCount, const QModelIndex &parentModelIndex = QModelIndex() ) override;

    void doEditCommit() override;
    void doEditRevert() override;

protected:
    //
    QList<QString>      listColumns;

    // bound buffers
    SQLUSMALLINT        aRowStatus[DATAWTableModelODBCRowArraySize];    // row status (1 per rows in data buffers)
    QVector<char*>      vectorRowBoundData;                             // row data (0-n cols) (single row data buffer)
    QVector<SQLLEN*>    vectorRowBoundDataInd;                          // row data ind (0-n cols) (single row data buffer)
};

#endif


