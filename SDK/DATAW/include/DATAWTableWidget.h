#ifndef H_DATAWTableWidget
#define H_DATAWTableWidget

#include <QtWidgets>

#include <WDataNavWidget.h>

#include <AWEditorWidget.h>

#include <DATA.h>

#include "DATAWMeta.h"
#include "DATAWTableView.h"
#include "DATAWTableModel.h"

class DATAConnection;
class DATAStatement;

/*!
 * \brief Displays the results for the given DATAStatement.
 *  
 * \author pharvey (6/16/20)
 */
class DATAWTableWidget : public AWEditorWidget
{
    Q_OBJECT
public:
    DATAWTableWidget( AWObject *pObject, QWidget *pParent, DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringTable );
    ~DATAWTableWidget();

    virtual AWPalWidget *       getPalWidget() override;
    virtual AWPanWidget *       getPanWidget() override;

signals:
    void signalMessage( const QString &stringSeverity, const QString &stringMessage );

protected:
    DATAConnection *    pConnection;
    QString             stringCatalog;
    QString             stringSchema;
    QString             stringTable;
    WDataNavWidget *    pNavigation;
    QLabel *            pRowsAffected;
    QLabel *            pEdits;

    //
    DATAWTableModel *       pTableModel;
    DATAWTableView *        pTableView;
    QItemSelectionModel *   pSelection;

    virtual void doSaveState();
    virtual void doLoadState();

protected slots:
    void slotRowFirst();
    void slotRowPrevious();
    void slotRowLast();
    void slotRowNext();
    void slotRowNew();
    void slotRowDelete();
    void slotRowsAffected( int );
    void slotEdits( bool );
    void slotReadOnly( bool );

    void slotCurrentSelectionChanged( const QModelIndex &current, const QModelIndex &previous );
};

#endif


