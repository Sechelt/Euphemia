#ifndef H_DATAWTableModel
#define H_DATAWTableModel

#include <QtWidgets>

#include "DATAWMeta.h"

class DATAConnection;

class DATAWTableModel : public  QAbstractTableModel
{
    Q_OBJECT
public:
    DATAWTableModel( QWidget *p, DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable );

    virtual void setFilter( int nIndex, const QString &stringValue ) { Q_UNUSED(nIndex); Q_UNUSED(stringValue); }

    int getRowsData() { return nRows -1; }

    // QTableView calls our commit() and revert() but we ignore them as we want to do row-based changes not cell-based
    // these are noop by default
    virtual void doEditCommit();    // do UPDATE or INSERT if row modified
                                // called when  - Moving to a different row (not cell) in the table
                                //              - Pressing the Enter key or Tab key after editing a cell.
                                //              - Clicking outside the currently edited cell.
    virtual void doEditRevert();    // undo any row modifications
                                // called when Esc while on an edited row

    virtual bool isEdit() const { return ( nRowEdit >= 0 ); }

signals:
    void signalRowsData( int ); // number of data rows (so not including any new-record row)
                                // connect to nav widget
    void signalRowsAffected( int );
    void signalReadOnly( bool ) const;
    void signalEdits( bool );

protected:
    DATAConnection *pConnection;
    DATAStatement * pStatement      = nullptr;
    DATAWMetaTable  metaTable;
    // table we are to edit
    QString stringCatalog;
    QString stringSchema;
    QString stringTable;
    //
    QStringList     listFilters;            // each column can have 0-1 value to filter on applied as follows in WHERE "WHERE = value"
    mutable bool    bReadOnly   = false;    // table read-only indicator, could not determine key-set or there was some other problem
    int             nRowNew     = 0;        // 0-n, index to new-record, =nRows-1, makes code easier to follow 
    int             nRows       = 0;        // 0-n, excludes h-header, includes any new-record row
    int             nCols       = 0;        // 0-n, excludes v-header

    // edit row buffers (existing row or a new-record row)
    // edit   - copies bound buffers into edit buffers
    // commit - copies edit buffers into bound buffers, does UPDATE or INSERT, reinits 
    // revert - simply reinits these buffers
    int                         nRowEdit            = -1;                    // row being modified (0-n) (-1 none being modified)
    mutable QVector<QVariant>   vectorDataEdit;                              // row with original col values which may have edits
    mutable QVector<SQLLEN>     vectorDataEditInd;                           // row ind (indicates which cols have been edited)

    virtual Qt::ItemFlags flags( const QModelIndex &index ) const override;
};

#endif


