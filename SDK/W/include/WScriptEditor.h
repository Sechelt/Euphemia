#ifndef H_WScriptEditor
#define H_WScriptEditor

#include "W.h"

class WLineNumberArea;
class WHighlighter;

/*!
 * \brief Script editor.
 * 
 * - text coloring
 *      - caller can assign a QSyntaxHighlighter to our document()
 *      - ie WSQL92Highlighter or WCPPHighlighter    
 * 
 * - line numbers
 *      - built in (using \sa WLineNumberArea) - always enabled
 * 
 * - line highlight
 *      - built in - use set/get to enable/disable
 * 
 * - code completion
 * 
 * \author pharvey (2/26/25)
 */
class WScriptEditor : public QPlainTextEdit
{
    friend class WLineNumberArea;
    Q_OBJECT

public:
    WScriptEditor( QWidget *pParent );

    void setCompleter( QCompleter *p );                                         // code completion
    void setHighlightCurrentLine( bool b ) { bHighlightCurrentLine = b; }       // highlight current line

    QCompleter *getCompleter() const;                                           // code completion
    bool isHighlightCurrentLine() const { return bHighlightCurrentLine; }       // highlight current line

protected:
    void resizeEvent( QResizeEvent *p )override;                                // line numbers
    void keyPressEvent( QKeyEvent *p ) override;                                // code completion
    void focusInEvent( QFocusEvent *p ) override;                               // code completion

    int getLineNumberAreaWidth();                                               // line numbers (called by WLineNumberArea)

    void doLineNumberAreaPaintEvent( QPaintEvent *event );                      // line numbers / highlight current line (called by WLineNumberArea)

private:                                                                        
    QFont font = QFont( "Consolas", 10 );                                       
    QWidget *pLineNumberArea;                                                   // line numbers
    QCompleter *pCompleter = nullptr;                                           // code completion
    bool bHighlightCurrentLine = false;                                         // highlight current line

    QString getTextUnderCursor() const;                                         

private slots:                                                                  
    void slotHighlightCurrentLine();                                            // highlight current line
    void slotUpdateLineNumberAreaWidth( int newBlockCount );                    // line numbers
    void slotUpdateLineNumberArea( const QRect &rect, int dy );                 // line numbers
    void slotInsertCompletion( const QString &s );                              // code completion
};

/*!
 * \brief Provides line numbering in left margin of given \sa WScriptEditor.
 * 
 * \author pharvey (2/26/25)
 */
class WLineNumberArea : public QWidget
{
public:
    WLineNumberArea( WScriptEditor *editor ) : QWidget( editor ), codeEditor( editor )
    { }

    QSize sizeHint() const override
    {
        return QSize( codeEditor->getLineNumberAreaWidth(), 0 );
    }

protected:
    void paintEvent( QPaintEvent *event )override
    {
        codeEditor->doLineNumberAreaPaintEvent( event );
    }

private:
    WScriptEditor *codeEditor;
};

/*!
 * \brief Syntax highlighter for given QTextDocument. 
 * 
 * \author pharvey (2/26/25)
 */
class WSQL92Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    WSQL92Highlighter( QTextDocument *parent = 0 );

protected:
    void highlightBlock( const QString &text )override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};

/*!
 * \brief C++ syntax highlighter for given QTextDocument. 
 * 
 * \author pharvey (2/26/25)
 */
class WCPPHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    WCPPHighlighter( QTextDocument *parent = 0 );

protected:
    void highlightBlock( const QString &text ) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};


#endif
