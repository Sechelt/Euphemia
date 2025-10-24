#include "LibInfo.h"
#include "DATASqlWidget.h"

#include <WFindTextDialog.h>

DATASqlWidget::DATASqlWidget( QWidget *pParent, const QString &stringText )
: WScriptEditor( pParent )
{
    setObjectName( metaObject()->className() );

    setText( stringText );

    connect( this, SIGNAL(modificationChanged(bool)), SIGNAL(signalModified(bool)));
    connect( this, SIGNAL(cursorPositionChanged()), SLOT(slotCursorPositionChanged()) );
    connect( this, SIGNAL(copyAvailable(bool)), SIGNAL(signalSelection(bool)) );

    // do we have signals for...
    // connect( this, SIGNAL(zzz(bool)), SIGNAL(signalChangedCut(bool)) );
    // connect( this, SIGNAL(zzz(bool)), SIGNAL(signalChangedPaste(bool)) );
    // connect( this, SIGNAL(zzz(bool)), SIGNAL(signalChangedUndo(bool)) );
    // connect( this, SIGNAL(zzz(bool)), SIGNAL(signalChangedRedo(bool)) );

    // So lets do what we can...
    QClipboard *pClipboard = QGuiApplication::clipboard();
    Q_ASSERT(pClipboard);
    connect( pClipboard, SIGNAL(changed(QClipboard::Mode)), SLOT(slotClipBoardChanged(QClipboard::Mode)) );
    connect( this, SIGNAL(copyAvailable(bool)), SLOT(slotChangedCopy(bool)) );
}

DATASqlWidget::~DATASqlWidget()
{
}

void DATASqlWidget::setText( const QString &stringText )
{
    setPlainText( stringText );
}

void DATASqlWidget::doReplace( const QString &stringNew )
{
}

void DATASqlWidget::doReplaceAll( const QString &stringNew )
{
}

void DATASqlWidget::slotCut()
{
    doCut();
}

void DATASqlWidget::slotCopy()
{
    doCopy();
}

void DATASqlWidget::slotPaste()
{
    doPaste();
}

void DATASqlWidget::slotDelete()
{
    doDelete();
}

void DATASqlWidget::slotSelectAll()
{
    doSelectAll();
}

void DATASqlWidget::slotSelectNone()
{
    doSelectNone();
}

void DATASqlWidget::slotUndo()
{
    doUndo();
}

void DATASqlWidget::slotRedo()
{
    doRedo();
}

void DATASqlWidget::slotClipBoardChanged( QClipboard::Mode )
{
    const QClipboard *pClipboard = QApplication::clipboard();
    const QMimeData *pMimeData = pClipboard->mimeData();
    if ( isReadOnly() || !pMimeData->hasText() ) 
    {
        emit signalChangedPaste( false );
        return;
    }
    emit signalChangedPaste( true );
}

void DATASqlWidget::slotChangedCopy( bool b )
{
    if ( !b )
    {
        emit signalChangedCut( false );
        emit signalChangedCopy( false );
        return;
    }

    if ( isReadOnly() )
    {
        emit signalChangedCut( false );
    }
    else
    {
        emit signalChangedCut( true );
    }

    emit signalChangedCopy( true );
}


void DATASqlWidget::slotCursorPositionChanged()
{

    emit signalCursorPosition( textCursor().blockNumber(), textCursor().positionInBlock() );
}

/*
DATASqlWidget::DATASqlWidget( QWidget *pParent, SqlDialectTypes nDialect, const QStringList &listAutoCompletions, const QString &stringSQL )
: QsciScintilla( pParent )
{
    setObjectName( metaObject()->className() );

    setUtf8( true ); 
    setMarginLineNumbers( 1, true );
    setAutoIndent( true );
    setAutoCompletionCaseSensitivity( true );
    setAutoCompletionThreshold( 2 );
    setAutoCompletionSource( QsciScintilla::AcsAll );

    // Qscintilla has lexers for MySQL, MSSQL and SQL (Oracle?).
    // Lexers have an interface for setting/getting config options as well as read/write them via QSettings. Create a preference dialog?
    switch ( nDialect )
    {
        case SqlDialectSql92:
            QsciLexerSQL *pLexer = new QsciLexerSQL( this );
            // The default colourizing does not work well on a dark themed (Linux + KDE) so impose our own.
            setLexer( pLexer );

            // We want the background colour to always be white...
            pLexer->setPaper( QColor( Qt::white ), -1 );

            // Make the default text colour 'black'...
            pLexer->setColor( QColor( Qt::black ), QsciScintillaBase::STYLE_DEFAULT );

            // Customize specifics (like default SlickEdit)...
            pLexer->setColor( QColor( 0, 128, 0 ), QsciLexerSQL::Comment );
            pLexer->setColor( QColor( 0, 128, 0 ), QsciLexerSQL::CommentLine );
            pLexer->setColor( QColor( Qt::blue ), QsciLexerSQL::CommentDoc );
            pLexer->setColor( QColor( 0, 0, 128 ), QsciLexerSQL::Number );
            pLexer->setColor( QColor( 128, 0, 128 ), QsciLexerSQL::Keyword );
            pLexer->setColor( QColor( 0, 128, 128 ), QsciLexerSQL::DoubleQuotedString );
            pLexer->setColor( QColor( 0, 96, 96 ), QsciLexerSQL::SingleQuotedString );
            pLexer->setColor( QColor( Qt::black ), QsciLexerSQL::PlusKeyword );
            pLexer->setColor( QColor( Qt::black ), QsciLexerSQL::PlusPrompt );
            pLexer->setColor( QColor( Qt::black ), QsciLexerSQL::Operator );
            pLexer->setColor( QColor( Qt::black ), QsciLexerSQL::Identifier );
            pLexer->setColor( QColor( Qt::black ), QsciLexerSQL::PlusComment );
            pLexer->setColor( QColor( 0, 128, 0 ), QsciLexerSQL::CommentLineHash );
            pLexer->setColor( QColor( Qt::blue ), QsciLexerSQL::CommentDocKeyword );
            pLexer->setColor( QColor( Qt::red ), QsciLexerSQL::CommentDocKeywordError );
            pLexer->setColor( QColor( Qt::blue ), QsciLexerSQL::QuotedIdentifier );
            pLexer->setColor( QColor( Qt::blue ), QsciLexerSQL::QuotedOperator );

            // font
            QFont font;
            pLexer->setFont( font, QsciScintillaBase::STYLE_DEFAULT );
            // remove bold...
            pLexer->setFont( font, QsciLexerSQL::Keyword );

            // options
            pLexer->setHashComments( true );

            // auto completion
            pApi = new QsciAPIs( pLexer );

            break;
    }

    setAutoCompletions( listAutoCompletions );
    setSQL( stringSQL );

    connect( this, SIGNAL(modificationChanged(bool)), SIGNAL(signalModified(bool)));
    connect( this, SIGNAL(cursorPositionChanged(int,int)), SIGNAL(signalCursorPosition(int,int)) );
    connect( this, SIGNAL(copyAvailable(bool)), SIGNAL(signalSelection(bool)) );

    // QsciScintilla does not have signals for... 
    // connect( this, SIGNAL(zzz(bool)), SIGNAL(signalChangedCut(bool)) );
    // connect( this, SIGNAL(zzz(bool)), SIGNAL(signalChangedPaste(bool)) );
    // connect( this, SIGNAL(zzz(bool)), SIGNAL(signalChangedUndo(bool)) );
    // connect( this, SIGNAL(zzz(bool)), SIGNAL(signalChangedRedo(bool)) );
    // So lets do what we can...
    QClipboard *pClipboard = QGuiApplication::clipboard();
    Q_ASSERT(pClipboard);
    connect( pClipboard, SIGNAL(changed(QClipboard::Mode)), SLOT(slotClipBoardChanged(QClipboard::Mode)) );
    connect( this, SIGNAL(copyAvailable(bool)), SLOT(slotChangedCopy(bool)) );
}

DATASqlWidget::~DATASqlWidget()
{
    delete pApi;
}

void DATASqlWidget::setAutoCompletions( const QStringList &listAutoCompletions )
{
    pApi->clear();
    QString stringAutoCompletion;
    foreach( stringAutoCompletion, listAutoCompletions )
    {
        pApi->add( stringAutoCompletion );
    }
    pApi->prepare();
}

void DATASqlWidget::setSQL( const QString &stringSQL )
{
    setText( stringSQL );
    setModified( false );
}

int DATASqlWidget::getLine()
{
    int nLine = 0;
    int nColumn = 0;
    getCursorPosition( &nLine, &nColumn );
    return nLine; 
}

int DATASqlWidget::getColumn()
{
    int nLine = 0;
    int nColumn = 0;
    getCursorPosition( &nLine, &nColumn );
    return nColumn; 
}

bool DATASqlWidget::doFindFirst( const QString &s, bool re, bool cs, bool wo, bool wrap, bool forward, int line, int index, bool show, bool posix, bool cxx11 )
{
    bool b = findFirst( s, re, cs, wo, wrap, forward, line, index, show, posix, cxx11 );
printf( "[%s][%s][%d] [%s]=%s\n", __FILE__, __FUNCTION__, __LINE__, s.toUtf8().constData(), (b?"found":"not found") );
    return b;
}

bool DATASqlWidget::doFindFirstInSelection( const QString &s, bool re, bool cs, bool wo, bool forward, bool show, bool posix, bool cxx11 )
{
    return findFirstInSelection( s, re, cs, wo, forward, show, posix, cxx11 );
}

void DATASqlWidget::doReplaceAll( const QString &stringNew )
{
}

void DATASqlWidget::slotCut()
{
    doCut();
}

void DATASqlWidget::slotCopy()
{
    doCopy();
}

void DATASqlWidget::slotPaste()
{
    doPaste();
}

void DATASqlWidget::slotDelete()
{
    doDelete();
}

void DATASqlWidget::slotSelectAll()
{
    doSelectAll();
}

void DATASqlWidget::slotSelectNone()
{
    doSelectNone();
}

void DATASqlWidget::slotUndo()
{
    doUndo();
}

void DATASqlWidget::slotRedo()
{
    doRedo();
}

void DATASqlWidget::slotClipBoardChanged( QClipboard::Mode )
{
    const QClipboard *pClipboard = QApplication::clipboard();
    const QMimeData *pMimeData = pClipboard->mimeData();
    if ( isReadOnly() || !pMimeData->hasText() ) 
    {
        emit signalChangedPaste( false );
        return;
    }
    emit signalChangedPaste( true );
}

void DATASqlWidget::slotChangedCopy( bool b )
{
    if ( !b )
    {
        emit signalChangedCut( false );
        emit signalChangedCopy( false );
        return;
    }

    if ( isReadOnly() )
    {
        emit signalChangedCut( false );
    }
    else
    {
        emit signalChangedCut( true );
    }

    emit signalChangedCopy( true );
}

*/

