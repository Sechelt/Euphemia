#ifndef H_DATAWSqlEditorWidget
#define H_DATAWSqlEditorWidget

#include <QtWidgets>

#include <AWEditorWidget.h>

#include <DATASqlWidget.h>
#include <DATAResultSetWidget.h>

class WFindTextDialog;

class DATAWEditorToolBar;
class DATAWConnection;
class DATAWSqlWidget;
class DATAWSql;

/*!
 * \brief A SQL editor widget. 
 *  
 * Wraps \sa DATASqlWidget to provide history. 
 *  
 * This supports \sa DATAWSqlEditorWidget.
 *  
 * \author pharvey (12/18/20)
 */
class DATAWSqlWidget : public QWidget
{
    Q_OBJECT
public:
    DATAWSqlWidget( QWidget *pParent, const QStringList &listCompletionStrings );
    ~DATAWSqlWidget();

    void setSQL( const QString &s )     { pSqlWidget->setText( s );      }
    void setModified( bool b = true );   
    void setHistoryMax( int n )         { nHistoryMax = n;              }

    QString         getSQL()        { return pSqlWidget->getText();         }
    QString         getSQLSelected(){ return pSqlWidget->getSelection();    }
    int             getHistoryMax() { return nHistoryMax;           }
    bool            hasHistory()    { return listHistory.count();   }
    DATASqlWidget * getSqlWidget()  { return pSqlWidget;            }
    bool            isReadOnly()    { return pSqlWidget->isReadOnly();          }
    bool            hasSelection()  { return pSqlWidget->hasSelection();        }
    bool            canCut()        { return !isReadOnly() && hasSelection();   }
    bool            canCopy()       { return hasSelection();        }
    bool            canPaste()      { return !isReadOnly();         }
    bool            canUndo()       { return pSqlWidget->canUndo(); }
    bool            canRedo()       { return pSqlWidget->canRedo(); }

    void doCut()                 { pSqlWidget->doCut();         } 
    void doCopy()                { pSqlWidget->doCopy();        } 
    void doPaste()               { pSqlWidget->doPaste();       } 
    void doDelete()              { pSqlWidget->doDelete();      } 
    void doSelectAll()           { pSqlWidget->doSelectAll();   } 
    void doSelectNone()          { pSqlWidget->doSelectNone();  }   
    void doUndo()                { pSqlWidget->doUndo();        } 
    void doRedo()                { pSqlWidget->doRedo();        } 

    bool doFindFirst( const QString &expr, bool re, bool cs, bool wo, bool wrap, bool forward = true, int line = -1, int index = -1, bool show = true, bool	posix = false, bool cxx11 = false );
    bool doFindFirstInSelection( const QString &s, bool re, bool cs, bool wo, bool forward = true, bool show = true, bool posix = false, bool cxx11 = false );
    void doFindNext();
    void doReplace( const QString &stringNew );
    void doReplaceAll( const QString &stringNew );
    void doCancelFind();

    void doHistoryPush( const QString &s );
    void doHistoryClear();

signals:
    void signalChangedCut( bool);
    void signalChangedCopy( bool );
    void signalChangedPaste( bool );
    void signalChangedUndo( bool );
    void signalChangedRedo( bool );
    void signalModified( bool );
    void signalSelection( bool );
    void signalHasHistory( bool );
    void signalCursorPosition( int, int );

public slots:
    void slotCut();
    void slotCopy();
    void slotPaste();
    void slotDelete();
    void slotSelectAll();
    void slotSelectNone();
    void slotUndo();
    void slotRedo();

protected:
    QStatusBar *    pStatusBar;
    QStringList     listHistory;    // last() is the one we are editing
    int             nHistoryCurrent;
    int             nHistoryMax;
    QSlider *       pSlider;
    QString         stringFind;

    DATASqlWidget * pSqlWidget;

    // status bar
    QLabel *            pLabelLine;
    QLabel *            pLabelColumn;
    QLabel *            pLabelModified;

    void doInitStatusBar();

protected slots:
    void slotHistoryGoTo( int );
    void slotCursorPosition( int, int );
    void slotModified( bool );
};

/*!
 * \brief A SQL results widget. 
 *  
 * Wraps a QMainWindow around a \sa DATAResultSetWidget so we can get a QStatusBar.
 * 
 * This supports \sa DATAWSqlEditorWidget.
 *  
 * \author pharvey (7/7/20)
 */
class DATAWResultWidget : public QMainWindow
{
    Q_OBJECT
public:
    DATAWResultWidget( QWidget *pParent );
    ~DATAWResultWidget();

    DATAResultSetWidget *getResultSetWidget() { return pResultSetWidget; }

    void doResult( DATAStatement *pStatement );

protected:
    QWidget *               pWidgetCentral;
    DATAResultSetWidget *   pResultSetWidget;
    // status bar
    QStatusBar *        pStatusBar;
    QLabel *            pLabelRows;
    QLabel *            pLabelRowsAffected;
    QLabel *            pLabelSeconds;

    void doInitStatusBar();
    void doClear();
};

/*!
 * \brief An editor widget for \sa DATAWSql. 
 *  
 * It combines a \sa DATAWSqlWidget (for editing SQL) with 0-n \sa DATAWResultWidget (for viewing results).
 *  
 * \author pharvey (6/16/20)
 */
class DATAWSqlEditorWidget : public AWEditorWidget
{
    Q_OBJECT
public:
    DATAWSqlEditorWidget( DATAWSql *pSql, QWidget *pParent );
    virtual ~DATAWSqlEditorWidget();

    void setSQL( const QString &s )                                     { pSqlWidget->setSQL( s );  }
    void setBatched( bool b )                                           { bBatched = b;             }
    void setResultFormat( DATAResultSetWidget::ResultFormatTypes n )    { nResultFormat = n;        }
    void setLimit( int n )                                              { nLimit = n;               }
    void setFirstRowColumnNames( bool b )                               { bFirstRowColumnNames = b; }
    void setDelimitChar( const QString &s )                             { stringDelimitChar = s;    }
    void setModified( bool b = true )                                   { pSqlWidget->setModified( b );  }

    QString                 getSQL()                    { return pSqlWidget->getSQL();          }
    QString                 getSQLSelected()            { return pSqlWidget->getSQLSelected();  }
    AWPalWidget *           getPalWidget();
    AWPanWidget *           getPanWidget();
    bool                    isBatched()                 { return bBatched;              }
    DATAResultSetWidget::ResultFormatTypes  getResultFormat()           { return nResultFormat;         }
    int                     getLimit()                  { return nLimit;                }
    QString                 getDelimitChar()            { return stringDelimitChar;     }
    DATAWSqlWidget *        getSqlWidget()              { return pSqlWidget;            }
    QTabWidget *            getResultsTabWidget()       { return pResultsTabWidget;     }

    bool isFirstRowColumnNames()     { return bFirstRowColumnNames;  }
    bool isReadOnly()   { return pSqlWidget->isReadOnly();      }
    bool hasSelection() { return pSqlWidget->hasSelection();    }
    bool canCut()       { return !isReadOnly() && hasSelection(); }
    bool canCopy()      { return hasSelection(); }
    bool canPaste()     { return !isReadOnly(); }
    bool canUndo()      { return pSqlWidget->canUndo(); }
    bool canRedo()      { return pSqlWidget->canRedo(); }

    void doSubmit();
    void doSubmitSelected();
    void doTables();
    void doColumns();
    void doPrimaryKey();
    void doForeignKeys();
    void doIndexs();
    void doSpecialColumns( SQLSMALLINT );
    void doCut()                 { pSqlWidget->doCut();         } 
    void doCopy()                { pSqlWidget->doCopy();        } 
    void doPaste()               { pSqlWidget->doPaste();       } 
    void doDelete()              { pSqlWidget->doDelete();      } 
    void doSelectAll()           { pSqlWidget->doSelectAll();   } 
    void doSelectNone()          { pSqlWidget->doSelectNone();  }   
    void doUndo()                { pSqlWidget->doUndo();        } 
    void doRedo()                { pSqlWidget->doRedo();        } 

    bool doFindFirst( const QString &expr, bool re, bool cs, bool wo, bool wrap, bool forward = true, int line = -1, int index = -1, bool show = true, bool	posix = false, bool cxx11 = false );
    bool doFindFirstInSelection( const QString &s, bool re, bool cs, bool wo, bool forward = true, bool show = true, bool posix = false, bool cxx11 = false );
    void doFindNext();
    void doReplace( const QString &stringNew );
    void doReplaceAll( const QString &stringNew );
    void doCancelFind();

    bool isFind();
    bool canFind()      { return true;          }
    bool canReplace()   { return !isReadOnly(); }

signals:
    void signalChangedCut( bool);
    void signalChangedCopy( bool );
    void signalChangedPaste( bool );
    void signalChangedUndo( bool );
    void signalChangedRedo( bool );
    void signalModified( bool );
    void signalSelection( bool );
    void signalHasHistory( bool );
    void signalCursorPosition( int, int );

public slots:
    virtual void slotChangedCut( bool );
    virtual void slotChangedCopy( bool );
    virtual void slotChangedPaste( bool );
    virtual void slotChangedUndo( bool );
    virtual void slotChangedRedo( bool );
    virtual void slotChangedSelection( bool );

    void slotSubmit();
    void slotSubmitSelected();
    void slotTables();
    void slotTableAttr( const QString & );
    void slotBatched( bool );
    void slotHistoryClear();
    void slotHistoryMax( int );
    void slotExport();
    void slotResultFormat( DATAResultSetWidget::ResultFormatTypes );
    void slotLimit( int );
    void slotFirstRowColumnNames( bool );
    void slotDelimitChar( const QString & );
    // edit
    void slotCut();
    void slotCopy();
    void slotPaste();
    void slotDelete();
    void slotSelectAll();
    void slotSelectNone();
    void slotUndo();
    void slotRedo();
    void slotFind();

protected:
    DATAWConnection *       pConnection;
    // config values
    bool                    bBatched;
    DATAResultSetWidget::ResultFormatTypes nResultFormat;
    int                     nLimit;
    bool                    bFirstRowColumnNames;
    QString                 stringDelimitChar;
    int                     nHistoryMax;
    WFindTextDialog *       pFindTextDialog;

    //
    DATAWEditorToolBar *    pToolBarEditor;
    QAction *pCut;
    QAction *pCopy;
    QAction *pPaste;
    QAction *pDelete;
    QAction *pSelectAll;
    QAction *pSelectNone;
    QAction *pUndo;
    QAction *pRedo;
    QAction *pFind;

    // principle widgets are in a splitter
    QSplitter *             pSplitter;
    DATAWSqlWidget *        pSqlWidget;             // 1 SQL editor
    QTabWidget *            pResultsTabWidget;      // 0-n DATAWResultWidget's

    QStringList getSQLStatements( const QString &s );

    void doInitToolBar();

    void doSubmit( const QString &stringSQL );
    void doClear();
    void doSaveState();
    void doLoadState();

protected slots:
    void slotFindFirst( const QString &, bool, bool, bool );
    void slotFindNext();
    void slotFindPrevious();
    void slotReplace( const QString & );
    void slotReplaceAll( const QString & );
    void slotFindTextChanged();
    void slotFindFinished( int );
};

#endif

