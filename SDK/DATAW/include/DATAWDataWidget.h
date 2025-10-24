#ifndef H_DATAWDataWidget
#define H_DATAWDataWidget

#include <QtWidgets>

#include <AWEditorWidget.h>

#include <DATA.h>

class DATAConnection;
class DATAStatement;

/*!
 * \brief Displays the results for the given DATAStatement.
 *  
 * \author pharvey (6/16/20)
 */
class DATAWDataWidget : public AWEditorWidget
{
    Q_OBJECT
public:
    DATAWDataWidget( AWObject *pObject, QWidget *pParent, DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable );
    ~DATAWDataWidget();

    // SETTERS
    virtual void setMaxRows( int n ) { nMaxRows = n; }

    // GETTERS
    virtual uint                getRows()                   { return nRows;     }
    virtual uint                getMaxRows()                { return nMaxRows;  }
    virtual AWPalWidget *       getPalWidget() override;
    virtual AWPanWidget *       getPanWidget() override;

signals:
    void signalMessage( const QString &stringSeverity, const QString &stringMessage );

//public slots:
//    virtual void slotClear() { doClear(); }

protected:
    DATAConnection *    pConnection;
    DATAStatement *     pStatement;
    QString             stringCatalog;
    QString             stringSchema;
    QString             stringTable;
    QStringList         listRowID;
    QStringList         listColumns;
    QStringList         listColumnsQuery;
    QString             stringQuery;
    QTableWidget *      ptableResults;
    uint                nRows;
    uint                nMaxRows;

    virtual QStringList getRowID();
    virtual QStringList getColumns();

    virtual void doClear();
    virtual void doResultGUIGrid();
    virtual void doResultGUIGridHeader( SWORD nColumns );
    virtual void doResultGUIGridBody( SWORD nColumns );
    virtual void doSaveState();
    virtual void doLoadState();
};

#endif


