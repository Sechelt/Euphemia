#ifndef H_DATASqlWidget
#define H_DATASqlWidget

#include <QtWidgets>

#include <WScriptEditor.h>

/*!
 * \brief wrapper for \sa WScriptEditor
 * 
 * \note Allows for the editor to be more easily swapped out (if needed).
 * 
 * \author pharv (2/26/2025)
 */
class DATASqlWidget : public WScriptEditor
{
    Q_OBJECT
public:
    DATASqlWidget( QWidget *pParent, const QString &stringText = QString() );
    ~DATASqlWidget();

    void    setText( const QString &stringText );

    QString getText()           { return toPlainText();                     }
    QString getTextSelected()   { return textCursor().selectedText();       } // use for copy/cut as it has some formatting
    QString getSelection()      { return textCursor().selection().toPlainText();          } // use to get plain text
    int     getLine()           { return textCursor().blockNumber();        }
    int     getColumn()         { return textCursor().positionInBlock();    }
    bool    hasSelection()      { return textCursor().hasSelection();       }
    bool    canCut()            { return !isReadOnly() && hasSelection();   }
    bool    canCopy()           { return hasSelection();                    }
    bool    canPaste()          { return !isReadOnly();                     }
    bool    canRedo()           { return document()->availableRedoSteps();  }
    bool    canUndo()           { return document()->availableUndoSteps();  }

    void doCut()                 { cut();               }
    void doCopy()                { copy();              }
    void doPaste()               { paste();             }
    void doDelete()              { cut();               } 
    void doSelectAll()           { selectAll();         }
    void doSelectNone()          { textCursor().clearSelection();        }
    void doUndo()                { undo();              }
    void doRedo()                { redo();              }

    bool doFind( const QString &exp, QTextDocument::FindFlags options = QTextDocument::FindFlags() )            { return find( exp, options ); }
    bool doFind( const QRegularExpression &exp, QTextDocument::FindFlags options = QTextDocument::FindFlags() ) { return find( exp, options ); }
    void doReplace( const QString &stringNew );
    void doReplaceAll( const QString &stringNew );

signals:
    void signalChangedCut( bool);
    void signalChangedCopy( bool );
    void signalChangedPaste( bool );
    void signalChangedUndo( bool );
    void signalChangedRedo( bool );
    void signalModified( bool b = true );
    void signalCursorPosition( int nLine, int nColumn ); 
    void signalSelection( bool );

public slots:
    void slotCut();
    void slotCopy();
    void slotPaste();
    void slotDelete();
    void slotSelectAll();
    void slotSelectNone();
    void slotUndo();
    void slotRedo();

protected slots:
    void slotClipBoardChanged( QClipboard::Mode );
    void slotChangedCopy( bool b );
    void slotCursorPositionChanged();
};


/*
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexersql.h>
#include <Qsci/qsciapis.h>
*/

/*!
 * \brief An SQL Editor. 
 *  
 * This extends QsciScintilla so as to; 
 *  
 *      - create an interface that is more CodeByDesign
 *      - abstract from QsciScintilla in case it is replaced in the future
 *  
 * Supports; 
 *  
 *      - syntax colouring
 *      - code completion
 *  
 * Supports different dialects including; 
 *  
 *      - SQL92
 *  
 * The interface for this is designed for general purpose use so it 
 * does NOT support; 
 *  
 *      - history
 *      - executing SQL
 *      - file save/load
 * 
 * This is done, in part, in \sa DATAWSqlEditorWidget.
 * 
 * \author pharvey (6/16/20)
 */
/*
class DATASqlWidget : public QsciScintilla
{
    Q_OBJECT
public:
    enum SqlDialectTypes
    {
        SqlDialectSql92
    };

    DATASqlWidget( QWidget *pParent, SqlDialectTypes nDialect = SqlDialectSql92, const QStringList &listAutoCompletions = QStringList(), const QString &stringSQL = QString() );
    ~DATASqlWidget();

    void    setAutoCompletions( const QStringList &listAutoCompletions );
    void    setSQL( const QString &stringSQL );

    QString getSQL()            { return text();           }
    QString getSQLSelected()    { return selectedText();   }
    int     getLine();
    int     getColumn();
    bool    hasSelection()      { return hasSelectedText(); }
    bool    canCut()            { return !isReadOnly() && hasSelection(); }
    bool    canCopy()           { return hasSelection(); }
    bool    canPaste()          { return !isReadOnly(); }
    bool    canRedo()           { return isRedoAvailable(); }
    bool    canUndo()           { return isUndoAvailable(); }

    void doCut()                 { cut();               }
    void doCopy()                { copy();              }
    void doPaste()               { paste();             }
    void doDelete()              { cut();               } // \todo QsciCommand to delete ?
    void doSelectAll()           { selectAll();         }
    void doSelectNone()          { selectAll( false );  }
    void doUndo()                { undo();              }
    void doRedo()                { redo();              }

    bool doFindFirst( const QString &expr, bool re, bool cs, bool wo, bool wrap, bool forward = true, int line = -1, int index = -1, bool show = true, bool	posix = false, bool cxx11 = false );
    bool doFindFirstInSelection( const QString &s, bool re, bool cs, bool wo, bool forward = true, bool show = true, bool posix = false, bool cxx11 = false );
    bool doFindNext()                   { return findNext();    }
    void doReplace( const QString &s )  { replace( s );         }
    void doReplaceAll( const QString &stringNew );
    void doCancelFind()                 { cancelFind(); }

signals:
    void signalChangedCut( bool);
    void signalChangedCopy( bool );
    void signalChangedPaste( bool );
    void signalChangedUndo( bool );
    void signalChangedRedo( bool );
    void signalModified( bool b = true );
    void signalCursorPosition( int nLine, int nColumn ); 
    void signalSelection( bool );

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
    QsciAPIs *pApi;

protected slots:
    void slotClipBoardChanged( QClipboard::Mode );
    void slotChangedCopy( bool b );
};
*/

#endif

