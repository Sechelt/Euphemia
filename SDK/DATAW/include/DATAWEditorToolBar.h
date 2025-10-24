#ifndef H_DATAWEditorToolBar
#define H_DATAWEditorToolBar

#include <QtWidgets>

#include <DATAResultSetWidget.h>

class DATAWConnection;

/*!
 * \brief ToolBar for \sa DATAWSqlEditorWidget
 * 
 * This can be put into the app tool bar or into an editors tool bar or both.
 * Think twice about merging this into one or the other.
 * 
 * \sa DATAWConnectionToolBar 
 *  
 * \author pharvey (12/24/20)
 */
class DATAWEditorToolBar : public QToolBar
{
    Q_OBJECT
public:
    DATAWEditorToolBar( DATAWConnection *pConnection, QWidget *pParent );
    ~DATAWEditorToolBar();

    void setResultFormat( DATAResultSetWidget::ResultFormatTypes n );
    void setLimit( int n );
    void setBatched( bool b = true );
    void setFirstRowColumnNames( bool b );
    void setDelimitChar( const QString &s );
    void setHistoryMax( int n );

    DATAResultSetWidget::ResultFormatTypes getResultFormat();
    int     getLimit();
    bool    isBatched();
    bool    getFirstRowColumnNames();
    QString getDelimitChar();

signals:
    void signalSubmit();
    void signalSubmitSelected();
    void signalTables();
    void signalTableAttr( const QString & );
    void signalResultFormat( DATAResultSetWidget::ResultFormatTypes );
    void signalLimit( int );
    void signalHistoryClear();
    void signalHistoryMax( int );
    void signalBatched( bool );
    void signalExport();
    void signalFirstRowColumnNames( bool );
    void signalDelimitChar( const QString & );

public slots:
    void slotSelection( bool );              // enable/disable SubmitSelected

protected slots:
    void slotChangedConnected();
    void slotBatched( bool );
    void slotFirstRowColumnNames( bool );
    void slotResultFormat( const QString & );

protected:
    DATAWConnection *pConnection;
    bool        hasSelection;
    QAction *   pSubmit;                        // submit to data source
    QAction *   pSubmitSelected;                // submit just the selected text
    QAction *   pTables;                        // SQLTables
    QComboBox * pTableAttr;                     // table attributes
    QComboBox * pResultFormat;                  // desired format for displaying the results
    QSpinBox *  pLimit;                         // limit number of records returned
    QAction *   pHistoryClear;                  // clear history
    QSpinBox *  pHistoryMax;                    // maximum history (-1 = unlimited, 0 = no history)
    QAction *   pBatched;                       // submit multiple statements as a series of individual statements or as a batch
    QAction *   pExport;                        // export result set
    QAction *   pFirstRowColumns;               // (when ResultFormatTextDelimited) the 1st row of result set should be column names
    QLineEdit * pDelimitChar;                   // (when ResultFormatTextDelimited) delimit char between result columns

    void doSyncControls();
    bool isConnected();
};

#endif

