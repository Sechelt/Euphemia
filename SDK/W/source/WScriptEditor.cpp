#include "LibInfo.h"
#include "WScriptEditor.h"

//
//
//
WScriptEditor::WScriptEditor( QWidget *pParent )
    : QPlainTextEdit( pParent )
{ 
    // we are a script editor and as such we never want line wrap
    setLineWrapMode( QPlainTextEdit::NoWrap );

    setFont( font );

    // left margin
    pLineNumberArea = new WLineNumberArea( this );

    connect( this, &WScriptEditor::blockCountChanged, this, &WScriptEditor::slotUpdateLineNumberAreaWidth );
    connect( this, &WScriptEditor::updateRequest, this, &WScriptEditor::slotUpdateLineNumberArea );
    connect( this, &WScriptEditor::cursorPositionChanged, this, &WScriptEditor::slotHighlightCurrentLine );

    slotUpdateLineNumberAreaWidth( 0 );
    slotHighlightCurrentLine();
}

int WScriptEditor::getLineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax( 1, blockCount() );
    while ( max >= 10 )
    {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance( QLatin1Char( '9' ) ) * digits;

    return space;
}

void WScriptEditor::setCompleter( QCompleter *p )
{
    if ( pCompleter ) pCompleter->disconnect( this );

    pCompleter = p;

    if ( !pCompleter ) return;

    pCompleter->setWidget( this );
    QObject::connect( pCompleter, QOverload<const QString &>::of(&QCompleter::activated), this, &WScriptEditor::slotInsertCompletion );
}

QCompleter *WScriptEditor::getCompleter() const
{
    return pCompleter;
}

void WScriptEditor::resizeEvent( QResizeEvent *e )
{
    QPlainTextEdit::resizeEvent( e );

    QRect cr = contentsRect();
    pLineNumberArea->setGeometry( QRect( cr.left(), cr.top(), getLineNumberAreaWidth(), cr.height() ) );
}

void WScriptEditor::keyPressEvent( QKeyEvent *e )
{
return QPlainTextEdit::keyPressEvent( e );

    if ( pCompleter && pCompleter->popup()->isVisible() ) 
    {
        // The following keys are forwarded by the completer to the widget
       switch ( e->key() ) 
       {
       case Qt::Key_Enter:
       case Qt::Key_Return:
       case Qt::Key_Escape:
       case Qt::Key_Tab:
       case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
       default:
           break;
       }
    }

    const bool isShortcut = ( e->modifiers().testFlag( Qt::ControlModifier ) && e->key() == Qt::Key_E ); // CTRL+E
    if ( !pCompleter || !isShortcut ) QPlainTextEdit::keyPressEvent( e ); // do not process the shortcut when we have a completer

    if ( isShortcut )
        printf( "[PAH][%s][%s][%d] CTRL+E\n", __FILE__, __FUNCTION__, __LINE__ );

    const bool ctrlOrShift = e->modifiers().testFlag( Qt::ControlModifier ) || e->modifiers().testFlag( Qt::ShiftModifier );
    if ( !pCompleter || (ctrlOrShift && e->text().isEmpty()) ) 
    {
        printf( "[PAH][%s][%s][%d] text is empty\n", __FILE__, __FUNCTION__, __LINE__ );
        return;
    }

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    const bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = getTextUnderCursor();

    if ( !isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 3 || eow.contains(e->text().right(1))) ) 
    {
        pCompleter->popup()->hide();
        return;
    }

    if ( completionPrefix != pCompleter->completionPrefix() ) 
    {
        pCompleter->setCompletionPrefix( completionPrefix );
        pCompleter->popup()->setCurrentIndex( pCompleter->completionModel()->index(0, 0) );
    }
    QRect cr = cursorRect();
    cr.setWidth( pCompleter->popup()->sizeHintForColumn(0) + pCompleter->popup()->verticalScrollBar()->sizeHint().width() );
    pCompleter->complete( cr ); // popup it up!
}

void WScriptEditor::focusInEvent(QFocusEvent *e)
{
return QPlainTextEdit::focusInEvent( e );

    if ( pCompleter ) pCompleter->setWidget( this );
    QPlainTextEdit::focusInEvent( e );
}

void WScriptEditor::doLineNumberAreaPaintEvent( QPaintEvent *event )
{
    QPainter painter( pLineNumberArea );
    painter.fillRect( event->rect(), Qt::lightGray );

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound( blockBoundingGeometry( block ).translated( contentOffset() ).top() );
    int bottom = top + qRound( blockBoundingRect( block ).height() );

    while ( block.isValid() && top <= event->rect().bottom() )
    {
        if ( block.isVisible() && bottom >= event->rect().top() )
        {
            QString number = QString::number( blockNumber + 1 );
            if ( blockNumber == textCursor().blockNumber() )
            {
                painter.fillRect( 0, top, pLineNumberArea->width(), fontMetrics().height(), Qt::green );
                painter.setPen( Qt::black );
            }
            else
                painter.setPen( Qt::black );
            painter.drawText( 0, top, pLineNumberArea->width(), fontMetrics().height(), Qt::AlignRight, number );
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound( blockBoundingRect( block ).height() );
        ++blockNumber;
    }
}

QString WScriptEditor::getTextUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select( QTextCursor::WordUnderCursor );
    return tc.selectedText();
}

void WScriptEditor::slotHighlightCurrentLine()
{
    if ( !bHighlightCurrentLine ) return;

    QList<QTextEdit::ExtraSelection> extraSelections;

    if ( !isReadOnly() )
    {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor( Qt::yellow ).lighter( 160 );

        selection.format.setBackground( lineColor );
        selection.format.setProperty( QTextFormat::FullWidthSelection, true );
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append( selection );
    }

    setExtraSelections( extraSelections );
}

void WScriptEditor::slotUpdateLineNumberAreaWidth( int /* newBlockCount */ )
{
    setViewportMargins( getLineNumberAreaWidth(), 0, 0, 0 );
}

void WScriptEditor::slotUpdateLineNumberArea( const QRect &rect, int dy )
{
    if ( dy ) 
        pLineNumberArea->scroll( 0, dy );
    else 
        pLineNumberArea->update( 0, rect.y(), pLineNumberArea->width(), rect.height() );

    if ( rect.contains( viewport()->rect() ) ) slotUpdateLineNumberAreaWidth( 0 );
}

void WScriptEditor::slotInsertCompletion( const QString &s )
{
printf( "[PAH][%s][%s][%d] %s\n", __FILE__, __FUNCTION__, __LINE__, s.toUtf8().constData() );
    if ( pCompleter->widget() != this ) return;
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
    QTextCursor tc = textCursor();
    int extra = s.length() - pCompleter->completionPrefix().length();
    tc.movePosition( QTextCursor::Left );
    tc.movePosition( QTextCursor::EndOfWord );
    tc.insertText( s.right( extra ) );
    setTextCursor( tc );
}

//
//
//
WSQL92Highlighter::WSQL92Highlighter( QTextDocument *parent )
    : QSyntaxHighlighter( parent )
{
    HighlightingRule rule;

    // \todo this should be in a text file and added as a resource
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
    const QString keywordPatterns[] = {
QStringLiteral( "\\bABORT\\b" ),
QStringLiteral( "\\bABORTSESSION\\b" ),
QStringLiteral( "\\bABS\\b" ),
QStringLiteral( "\\bABSENT\\b" ),
QStringLiteral( "\\bABSOLUTE\\b" ),
QStringLiteral( "\\bACCESS\\b" ),
QStringLiteral( "\\bACCESSIBLE\\b" ),
QStringLiteral( "\\bACCESS_LOCK\\b" ),
QStringLiteral( "\\bACCOUNT\\b" ),
QStringLiteral( "\\bACOS\\b" ),
QStringLiteral( "\\bACOSH\\b" ),
QStringLiteral( "\\bACTION\\b" ),
QStringLiteral( "\\bADD\\b" ),
QStringLiteral( "\\bADD_MONTHS\\b" ),
QStringLiteral( "\\bADMIN\\b" ),
QStringLiteral( "\\bAFTER\\b" ),
QStringLiteral( "\\bAGGREGATE\\b" ),
QStringLiteral( "\\bALIAS\\b" ),
QStringLiteral( "\\bALL\\b" ),
QStringLiteral( "\\bALLOCATE\\b" ),
QStringLiteral( "\\bALLOW\\b" ),
QStringLiteral( "\\bALTER\\b" ),
QStringLiteral( "\\bALTERAND\\b" ),
QStringLiteral( "\\bAMP\\b" ),
QStringLiteral( "\\bANALYSE\\b" ),
QStringLiteral( "\\bANALYZE\\b" ),
QStringLiteral( "\\bAND\\b" ),
QStringLiteral( "\\bANSIDATE\\b" ),
QStringLiteral( "\\bANY\\b" ),
QStringLiteral( "\\bANY_VALUE\\b" ),
QStringLiteral( "\\bARE\\b" ),
QStringLiteral( "\\bARRAY\\b" ),
QStringLiteral( "\\bARRAY_AGG\\b" ),
QStringLiteral( "\\bARRAY_EXISTS\\b" ),
QStringLiteral( "\\bARRAY_MAX_CARDINALITY\\b" ),
QStringLiteral( "\\bAS\\b" ),
QStringLiteral( "\\bASC\\b" ),
QStringLiteral( "\\bASENSITIVE\\b" ),
QStringLiteral( "\\bASIN\\b" ),
QStringLiteral( "\\bASINH\\b" ),
QStringLiteral( "\\bASSERTION\\b" ),
QStringLiteral( "\\bASSOCIATE\\b" ),
QStringLiteral( "\\bASUTIME\\b" ),
QStringLiteral( "\\bASYMMETRIC\\b" ),
QStringLiteral( "\\bAT\\b" ),
QStringLiteral( "\\bATAN\\b" ),
QStringLiteral( "\\bATAN2\\b" ),
QStringLiteral( "\\bATANH\\b" ),
QStringLiteral( "\\bATOMIC\\b" ),
QStringLiteral( "\\bAUDIT\\b" ),
QStringLiteral( "\\bAUTHORIZATION\\b" ),
QStringLiteral( "\\bAUX\\b" ),
QStringLiteral( "\\bAUXILIARY\\b" ),
QStringLiteral( "\\bAVE\\b" ),
QStringLiteral( "\\bAVERAGE\\b" ),
QStringLiteral( "\\bAVG\\b" ),
QStringLiteral( "\\bBACKUP\\b" ),
QStringLiteral( "\\bBEFORE\\b" ),
QStringLiteral( "\\bBEGIN\\b" ),
QStringLiteral( "\\bBEGIN_FRAME\\b" ),
QStringLiteral( "\\bBEGIN_PARTITION\\b" ),
QStringLiteral( "\\bBETWEEN\\b" ),
QStringLiteral( "\\bBIGINT\\b" ),
QStringLiteral( "\\bBINARY\\b" ),
QStringLiteral( "\\bBIT\\b" ),
QStringLiteral( "\\bBLOB\\b" ),
QStringLiteral( "\\bBOOLEAN\\b" ),
QStringLiteral( "\\bBOTH\\b" ),
QStringLiteral( "\\bBREADTH\\b" ),
QStringLiteral( "\\bBREAK\\b" ),
QStringLiteral( "\\bBROWSE\\b" ),
QStringLiteral( "\\bBT\\b" ),
QStringLiteral( "\\bBTRIM\\b" ),
QStringLiteral( "\\bBUFFERPOOL\\b" ),
QStringLiteral( "\\bBULK\\b" ),
QStringLiteral( "\\bBUT\\b" ),
QStringLiteral( "\\bBY\\b" ),
QStringLiteral( "\\bBYTE\\b" ),
QStringLiteral( "\\bBYTEINT\\b" ),
QStringLiteral( "\\bBYTES\\b" ),
QStringLiteral( "\\bCALL\\b" ),
QStringLiteral( "\\bCALLED\\b" ),
QStringLiteral( "\\bCAPTURE\\b" ),
QStringLiteral( "\\bCARDINALITY\\b" ),
QStringLiteral( "\\bCASCADE\\b" ),
QStringLiteral( "\\bCASCADED\\b" ),
QStringLiteral( "\\bCASE\\b" ),
QStringLiteral( "\\bCASESPECIFIC\\b" ),
QStringLiteral( "\\bCASE_N\\b" ),
QStringLiteral( "\\bCAST\\b" ),
QStringLiteral( "\\bCATALOG\\b" ),
QStringLiteral( "\\bCCSID\\b" ),
QStringLiteral( "\\bCD\\b" ),
QStringLiteral( "\\bCEIL\\b" ),
QStringLiteral( "\\bCEILING\\b" ),
QStringLiteral( "\\bCHANGE\\b" ),
QStringLiteral( "\\bCHAR\\b" ),
QStringLiteral( "\\bCHAR2HEXINT\\b" ),
QStringLiteral( "\\bCHARACTER\\b" ),
QStringLiteral( "\\bCHARACTERS\\b" ),
QStringLiteral( "\\bCHARACTER_LENGTH\\b" ),
QStringLiteral( "\\bCHARS\\b" ),
QStringLiteral( "\\bCHAR_LENGTH\\b" ),
QStringLiteral( "\\bCHECK\\b" ),
QStringLiteral( "\\bCHECKPOINT\\b" ),
QStringLiteral( "\\bCLASS\\b" ),
QStringLiteral( "\\bCLASSIFIER\\b" ),
QStringLiteral( "\\bCLOB\\b" ),
QStringLiteral( "\\bCLONE\\b" ),
QStringLiteral( "\\bCLOSE\\b" ),
QStringLiteral( "\\bCLUSTER\\b" ),
QStringLiteral( "\\bCLUSTERED\\b" ),
QStringLiteral( "\\bCM\\b" ),
QStringLiteral( "\\bCOALESCE\\b" ),
QStringLiteral( "\\bCOLLATE\\b" ),
QStringLiteral( "\\bCOLLATION\\b" ),
QStringLiteral( "\\bCOLLECT\\b" ),
QStringLiteral( "\\bCOLLECTION\\b" ),
QStringLiteral( "\\bCOLLID\\b" ),
QStringLiteral( "\\bCOLUMN\\b" ),
QStringLiteral( "\\bCOLUMN_VALUE\\b" ),
QStringLiteral( "\\bCOMMENT\\b" ),
QStringLiteral( "\\bCOMMIT\\b" ),
QStringLiteral( "\\bCOMPLETION\\b" ),
QStringLiteral( "\\bCOMPRESS\\b" ),
QStringLiteral( "\\bCOMPUTE\\b" ),
QStringLiteral( "\\bCONCAT\\b" ),
QStringLiteral( "\\bCONCURRENTLY\\b" ),
QStringLiteral( "\\bCONDITION\\b" ),
QStringLiteral( "\\bCONNECT\\b" ),
QStringLiteral( "\\bCONNECTION\\b" ),
QStringLiteral( "\\bCONSTRAINT\\b" ),
QStringLiteral( "\\bCONSTRAINTS\\b" ),
QStringLiteral( "\\bCONSTRUCTOR\\b" ),
QStringLiteral( "\\bCONTAINS\\b" ),
QStringLiteral( "\\bCONTAINSTABLE\\b" ),
QStringLiteral( "\\bCONTENT\\b" ),
QStringLiteral( "\\bCONTINUE\\b" ),
QStringLiteral( "\\bCONVERT\\b" ),
QStringLiteral( "\\bCONVERT_TABLE_HEADER\\b" ),
QStringLiteral( "\\bCOPY\\b" ),
QStringLiteral( "\\bCORR\\b" ),
QStringLiteral( "\\bCORRESPONDING\\b" ),
QStringLiteral( "\\bCOS\\b" ),
QStringLiteral( "\\bCOSH\\b" ),
QStringLiteral( "\\bCOUNT\\b" ),
QStringLiteral( "\\bCOVAR_POP\\b" ),
QStringLiteral( "\\bCOVAR_SAMP\\b" ),
QStringLiteral( "\\bCREATE\\b" ),
QStringLiteral( "\\bCROSS\\b" ),
QStringLiteral( "\\bCS\\b" ),
QStringLiteral( "\\bCSUM\\b" ),
QStringLiteral( "\\bCT\\b" ),
QStringLiteral( "\\bCUBE\\b" ),
QStringLiteral( "\\bCUME_DIST\\b" ),
QStringLiteral( "\\bCURRENT\\b" ),
QStringLiteral( "\\bCURRENT_CATALOG\\b" ),
QStringLiteral( "\\bCURRENT_DATE\\b" ),
QStringLiteral( "\\bCURRENT_DEFAULT_TRANSFORM_GROUP\\b" ),
QStringLiteral( "\\bCURRENT_LC_CTYPE\\b" ),
QStringLiteral( "\\bCURRENT_PATH\\b" ),
QStringLiteral( "\\bCURRENT_ROLE\\b" ),
QStringLiteral( "\\bCURRENT_ROW\\b" ),
QStringLiteral( "\\bCURRENT_SCHEMA\\b" ),
QStringLiteral( "\\bCURRENT_SERVER\\b" ),
QStringLiteral( "\\bCURRENT_TIME\\b" ),
QStringLiteral( "\\bCURRENT_TIMESTAMP\\b" ),
QStringLiteral( "\\bCURRENT_TIMEZONE\\b" ),
QStringLiteral( "\\bCURRENT_TRANSFORM_GROUP_FOR_TYPE\\b" ),
QStringLiteral( "\\bCURRENT_USER\\b" ),
QStringLiteral( "\\bCURRVAL\\b" ),
QStringLiteral( "\\bCURSOR\\b" ),
QStringLiteral( "\\bCV\\b" ),
QStringLiteral( "\\bCYCLE\\b" ),
QStringLiteral( "\\bDATA\\b" ),
QStringLiteral( "\\bDATABASE\\b" ),
QStringLiteral( "\\bDATABASES\\b" ),
QStringLiteral( "\\bDATABLOCKSIZE\\b" ),
QStringLiteral( "\\bDATE\\b" ),
QStringLiteral( "\\bDATEFORM\\b" ),
QStringLiteral( "\\bDAY\\b" ),
QStringLiteral( "\\bDAYS\\b" ),
QStringLiteral( "\\bDAY_HOUR\\b" ),
QStringLiteral( "\\bDAY_MICROSECOND\\b" ),
QStringLiteral( "\\bDAY_MINUTE\\b" ),
QStringLiteral( "\\bDAY_SECOND\\b" ),
QStringLiteral( "\\bDBCC\\b" ),
QStringLiteral( "\\bDBINFO\\b" ),
QStringLiteral( "\\bDEALLOCATE\\b" ),
QStringLiteral( "\\bDEC\\b" ),
QStringLiteral( "\\bDECFLOAT\\b" ),
QStringLiteral( "\\bDECIMAL\\b" ),
QStringLiteral( "\\bDECLARE\\b" ),
QStringLiteral( "\\bDEFAULT\\b" ),
QStringLiteral( "\\bDEFERRABLE\\b" ),
QStringLiteral( "\\bDEFERRED\\b" ),
QStringLiteral( "\\bDEFINE\\b" ),
QStringLiteral( "\\bDEGREES\\b" ),
QStringLiteral( "\\bDEL\\b" ),
QStringLiteral( "\\bDELAYED\\b" ),
QStringLiteral( "\\bDELETE\\b" ),
QStringLiteral( "\\bDENSE_RANK\\b" ),
QStringLiteral( "\\bDENY\\b" ),
QStringLiteral( "\\bDEPTH\\b" ),
QStringLiteral( "\\bDEREF\\b" ),
QStringLiteral( "\\bDESC\\b" ),
QStringLiteral( "\\bDESCRIBE\\b" ),
QStringLiteral( "\\bDESCRIPTOR\\b" ),
QStringLiteral( "\\bDESTROY\\b" ),
QStringLiteral( "\\bDESTRUCTOR\\b" ),
QStringLiteral( "\\bDETERMINISTIC\\b" ),
QStringLiteral( "\\bDIAGNOSTIC\\b" ),
QStringLiteral( "\\bDIAGNOSTICS\\b" ),
QStringLiteral( "\\bDICTIONARY\\b" ),
QStringLiteral( "\\bDISABLE\\b" ),
QStringLiteral( "\\bDISABLED\\b" ),
QStringLiteral( "\\bDISALLOW\\b" ),
QStringLiteral( "\\bDISCONNECT\\b" ),
QStringLiteral( "\\bDISK\\b" ),
QStringLiteral( "\\bDISTINCT\\b" ),
QStringLiteral( "\\bDISTINCTROW\\b" ),
QStringLiteral( "\\bDISTRIBUTED\\b" ),
QStringLiteral( "\\bDIV\\b" ),
QStringLiteral( "\\bDO\\b" ),
QStringLiteral( "\\bDOCUMENT\\b" ),
QStringLiteral( "\\bDOMAIN\\b" ),
QStringLiteral( "\\bDOUBLE\\b" ),
QStringLiteral( "\\bDROP\\b" ),
QStringLiteral( "\\bDSSIZE\\b" ),
QStringLiteral( "\\bDUAL\\b" ),
QStringLiteral( "\\bDUMP\\b" ),
QStringLiteral( "\\bDYNAMIC\\b" ),
QStringLiteral( "\\bEACH\\b" ),
QStringLiteral( "\\bECHO\\b" ),
QStringLiteral( "\\bEDITPROC\\b" ),
QStringLiteral( "\\bELEMENT\\b" ),
QStringLiteral( "\\bELSE\\b" ),
QStringLiteral( "\\bELSEIF\\b" ),
QStringLiteral( "\\bEMPTY\\b" ),
QStringLiteral( "\\bENABLED\\b" ),
QStringLiteral( "\\bENCLOSED\\b" ),
QStringLiteral( "\\bENCODING\\b" ),
QStringLiteral( "\\bENCRYPTION\\b" ),
QStringLiteral( "\\bEND\\b" ),
QStringLiteral( "\\bEND\\b" ),
QStringLiteral( "\\bENDING\\b" ),
QStringLiteral( "\\bEND_FRAME\\b" ),
QStringLiteral( "\\bEND_PARTITION\\b" ),
QStringLiteral( "\\bEQ\\b" ),
QStringLiteral( "\\bEQUALS\\b" ),
QStringLiteral( "\\bERASE\\b" ),
QStringLiteral( "\\bERRLVL\\b" ),
QStringLiteral( "\\bERROR\\b" ),
QStringLiteral( "\\bERRORFILES\\b" ),
QStringLiteral( "\\bERRORTABLES\\b" ),
QStringLiteral( "\\bESCAPE\\b" ),
QStringLiteral( "\\bESCAPED\\b" ),
QStringLiteral( "\\bET\\b" ),
QStringLiteral( "\\bEVERY\\b" ),
QStringLiteral( "\\bEXCEPT\\b" ),
QStringLiteral( "\\bEXCEPTION\\b" ),
QStringLiteral( "\\bEXCLUSIVE\\b" ),
QStringLiteral( "\\bEXEC\\b" ),
QStringLiteral( "\\bEXECUTE\\b" ),
QStringLiteral( "\\bEXISTS\\b" ),
QStringLiteral( "\\bEXIT\\b" ),
QStringLiteral( "\\bEXP\\b" ),
QStringLiteral( "\\bEXPLAIN\\b" ),
QStringLiteral( "\\bEXTERNAL\\b" ),
QStringLiteral( "\\bEXTRACT\\b" ),
QStringLiteral( "\\bFALLBACK\\b" ),
QStringLiteral( "\\bFALSE\\b" ),
QStringLiteral( "\\bFASTEXPORT\\b" ),
QStringLiteral( "\\bFENCED\\b" ),
QStringLiteral( "\\bFETCH\\b" ),
QStringLiteral( "\\bFIELDPROC\\b" ),
QStringLiteral( "\\bFILE\\b" ),
QStringLiteral( "\\bFILLFACTOR\\b" ),
QStringLiteral( "\\bFILTER\\b" ),
QStringLiteral( "\\bFINAL\\b" ),
QStringLiteral( "\\bFIRST\\b" ),
QStringLiteral( "\\bFIRST_VALUE\\b" ),
QStringLiteral( "\\bFLOAT\\b" ),
QStringLiteral( "\\bFLOAT4\\b" ),
QStringLiteral( "\\bFLOAT8\\b" ),
QStringLiteral( "\\bFLOOR\\b" ),
QStringLiteral( "\\bFOR\\b" ),
QStringLiteral( "\\bFORCE\\b" ),
QStringLiteral( "\\bFOREIGN\\b" ),
QStringLiteral( "\\bFORMAT\\b" ),
QStringLiteral( "\\bFOUND\\b" ),
QStringLiteral( "\\bFRAME_ROW\\b" ),
QStringLiteral( "\\bFREE\\b" ),
QStringLiteral( "\\bFREESPACE\\b" ),
QStringLiteral( "\\bFREETEXT\\b" ),
QStringLiteral( "\\bFREETEXTTABLE\\b" ),
QStringLiteral( "\\bFREEZE\\b" ),
QStringLiteral( "\\bFROM\\b" ),
QStringLiteral( "\\bFULL\\b" ),
QStringLiteral( "\\bFULLTEXT\\b" ),
QStringLiteral( "\\bFUNCTION\\b" ),
QStringLiteral( "\\bFUSION\\b" ),
QStringLiteral( "\\bGE\\b" ),
QStringLiteral( "\\bGENERAL\\b" ),
QStringLiteral( "\\bGENERATED\\b" ),
QStringLiteral( "\\bGET\\b" ),
QStringLiteral( "\\bGIVE\\b" ),
QStringLiteral( "\\bGLOBAL\\b" ),
QStringLiteral( "\\bGO\\b" ),
QStringLiteral( "\\bGOTO\\b" ),
QStringLiteral( "\\bGRANT\\b" ),
QStringLiteral( "\\bGRAPHIC\\b" ),
QStringLiteral( "\\bGREATEST\\b" ),
QStringLiteral( "\\bGROUP\\b" ),
QStringLiteral( "\\bGROUPING\\b" ),
QStringLiteral( "\\bGROUPS\\b" ),
QStringLiteral( "\\bGT\\b" ),
QStringLiteral( "\\bHANDLER\\b" ),
QStringLiteral( "\\bHASH\\b" ),
QStringLiteral( "\\bHASHAMP\\b" ),
QStringLiteral( "\\bHASHBAKAMP\\b" ),
QStringLiteral( "\\bHASHBUCKET\\b" ),
QStringLiteral( "\\bHASHROW\\b" ),
QStringLiteral( "\\bHAVING\\b" ),
QStringLiteral( "\\bHELP\\b" ),
QStringLiteral( "\\bHIGH_PRIORITY\\b" ),
QStringLiteral( "\\bHOLD\\b" ),
QStringLiteral( "\\bHOLDLOCK\\b" ),
QStringLiteral( "\\bHOST\\b" ),
QStringLiteral( "\\bHOUR\\b" ),
QStringLiteral( "\\bHOURS\\b" ),
QStringLiteral( "\\bHOUR_MICROSECOND\\b" ),
QStringLiteral( "\\bHOUR_MINUTE\\b" ),
QStringLiteral( "\\bHOUR_SECOND\\b" ),
QStringLiteral( "\\bIDENTIFIED\\b" ),
QStringLiteral( "\\bIDENTITY\\b" ),
QStringLiteral( "\\bIDENTITYCOL\\b" ),
QStringLiteral( "\\bIDENTITY_INSERT\\b" ),
QStringLiteral( "\\bIF\\b" ),
QStringLiteral( "\\bIGNORE\\b" ),
QStringLiteral( "\\bILIKE\\b" ),
QStringLiteral( "\\bIMMEDIATE\\b" ),
QStringLiteral( "\\bIN\\b" ),
QStringLiteral( "\\bINCLUSIVE\\b" ),
QStringLiteral( "\\bINCONSISTENT\\b" ),
QStringLiteral( "\\bINCREMENT\\b" ),
QStringLiteral( "\\bINDEX\\b" ),
QStringLiteral( "\\bINDICATOR\\b" ),
QStringLiteral( "\\bINFILE\\b" ),
QStringLiteral( "\\bINHERIT\\b" ),
QStringLiteral( "\\bINITIAL\\b" ),
QStringLiteral( "\\bINITIALIZE\\b" ),
QStringLiteral( "\\bINITIALLY\\b" ),
QStringLiteral( "\\bINITIATE\\b" ),
QStringLiteral( "\\bINNER\\b" ),
QStringLiteral( "\\bINOUT\\b" ),
QStringLiteral( "\\bINPUT\\b" ),
QStringLiteral( "\\bINS\\b" ),
QStringLiteral( "\\bINSENSITIVE\\b" ),
QStringLiteral( "\\bINSERT\\b" ),
QStringLiteral( "\\bINSTEAD\\b" ),
QStringLiteral( "\\bINT\\b" ),
QStringLiteral( "\\bINT1\\b" ),
QStringLiteral( "\\bINT2\\b" ),
QStringLiteral( "\\bINT3\\b" ),
QStringLiteral( "\\bINT4\\b" ),
QStringLiteral( "\\bINT8\\b" ),
QStringLiteral( "\\bINTEGER\\b" ),
QStringLiteral( "\\bINTEGERDATE\\b" ),
QStringLiteral( "\\bINTERSECT\\b" ),
QStringLiteral( "\\bINTERSECTION\\b" ),
QStringLiteral( "\\bINTERVAL\\b" ),
QStringLiteral( "\\bINTO\\b" ),
QStringLiteral( "\\bIO_AFTER_GTIDS\\b" ),
QStringLiteral( "\\bIO_BEFORE_GTIDS\\b" ),
QStringLiteral( "\\bIS\\b" ),
QStringLiteral( "\\bISNULL\\b" ),
QStringLiteral( "\\bISOBID\\b" ),
QStringLiteral( "\\bISOLATION\\b" ),
QStringLiteral( "\\bITERATE\\b" ),
QStringLiteral( "\\bJAR\\b" ),
QStringLiteral( "\\bJOIN\\b" ),
QStringLiteral( "\\bJOURNAL\\b" ),
QStringLiteral( "\\bJSON\\b" ),
QStringLiteral( "\\bJSON_ARRAY\\b" ),
QStringLiteral( "\\bJSON_ARRAYAGG\\b" ),
QStringLiteral( "\\bJSON_EXISTS\\b" ),
QStringLiteral( "\\bJSON_OBJECT\\b" ),
QStringLiteral( "\\bJSON_OBJECTAGG\\b" ),
QStringLiteral( "\\bJSON_QUERY\\b" ),
QStringLiteral( "\\bJSON_SCALAR\\b" ),
QStringLiteral( "\\bJSON_SERIALIZE\\b" ),
QStringLiteral( "\\bJSON_TABLE\\b" ),
QStringLiteral( "\\bJSON_TABLE_PRIMITIVE\\b" ),
QStringLiteral( "\\bJSON_VALUE\\b" ),
QStringLiteral( "\\bKEEP\\b" ),
QStringLiteral( "\\bKEY\\b" ),
QStringLiteral( "\\bKEYS\\b" ),
QStringLiteral( "\\bKILL\\b" ),
QStringLiteral( "\\bKURTOSIS\\b" ),
QStringLiteral( "\\bLABEL\\b" ),
QStringLiteral( "\\bLAG\\b" ),
QStringLiteral( "\\bLANGUAGE\\b" ),
QStringLiteral( "\\bLARGE\\b" ),
QStringLiteral( "\\bLAST\\b" ),
QStringLiteral( "\\bLAST_VALUE\\b" ),
QStringLiteral( "\\bLATERAL\\b" ),
QStringLiteral( "\\bLC_CTYPE\\b" ),
QStringLiteral( "\\bLE\\b" ),
QStringLiteral( "\\bLEAD\\b" ),
QStringLiteral( "\\bLEADING\\b" ),
QStringLiteral( "\\bLEAST\\b" ),
QStringLiteral( "\\bLEAVE\\b" ),
QStringLiteral( "\\bLEFT\\b" ),
QStringLiteral( "\\bLESS\\b" ),
QStringLiteral( "\\bLEVEL\\b" ),
QStringLiteral( "\\bLIKE\\b" ),
QStringLiteral( "\\bLIKE_REGEX\\b" ),
QStringLiteral( "\\bLIMIT\\b" ),
QStringLiteral( "\\bLINEAR\\b" ),
QStringLiteral( "\\bLINENO\\b" ),
QStringLiteral( "\\bLINES\\b" ),
QStringLiteral( "\\bLISTAGG\\b" ),
QStringLiteral( "\\bLN\\b" ),
QStringLiteral( "\\bLOAD\\b" ),
QStringLiteral( "\\bLOADING\\b" ),
QStringLiteral( "\\bLOCAL\\b" ),
QStringLiteral( "\\bLOCALE\\b" ),
QStringLiteral( "\\bLOCALTIME\\b" ),
QStringLiteral( "\\bLOCALTIMESTAMP\\b" ),
QStringLiteral( "\\bLOCATOR\\b" ),
QStringLiteral( "\\bLOCATORS\\b" ),
QStringLiteral( "\\bLOCK\\b" ),
QStringLiteral( "\\bLOCKING\\b" ),
QStringLiteral( "\\bLOCKMAX\\b" ),
QStringLiteral( "\\bLOCKSIZE\\b" ),
QStringLiteral( "\\bLOG\\b" ),
QStringLiteral( "\\bLOG10\\b" ),
QStringLiteral( "\\bLOGGING\\b" ),
QStringLiteral( "\\bLOGON\\b" ),
QStringLiteral( "\\bLONG\\b" ),
QStringLiteral( "\\bLONGBLOB\\b" ),
QStringLiteral( "\\bLONGTEXT\\b" ),
QStringLiteral( "\\bLOOP\\b" ),
QStringLiteral( "\\bLOWER\\b" ),
QStringLiteral( "\\bLOW_PRIORITY\\b" ),
QStringLiteral( "\\bLPAD\\b" ),
QStringLiteral( "\\bLT\\b" ),
QStringLiteral( "\\bLTRIM\\b" ),
QStringLiteral( "\\bMACRO\\b" ),
QStringLiteral( "\\bMAINTAINED\\b" ),
QStringLiteral( "\\bMAP\\b" ),
QStringLiteral( "\\bMASTER_BIND\\b" ),
QStringLiteral( "\\bMASTER_SSL_VERIFY_SERVER_CERT\\b" ),
QStringLiteral( "\\bMATCH\\b" ),
QStringLiteral( "\\bMATCHES\\b" ),
QStringLiteral( "\\bMATCH_NUMBER\\b" ),
QStringLiteral( "\\bMATCH_RECOGNIZE\\b" ),
QStringLiteral( "\\bMATERIALIZED\\b" ),
QStringLiteral( "\\bMAVG\\b" ),
QStringLiteral( "\\bMAX\\b" ),
QStringLiteral( "\\bMAXEXTENTS\\b" ),
QStringLiteral( "\\bMAXIMUM\\b" ),
QStringLiteral( "\\bMAXVALUE\\b" ),
QStringLiteral( "\\bMCHARACTERS\\b" ),
QStringLiteral( "\\bMDIFF\\b" ),
QStringLiteral( "\\bMEDIUMBLOB\\b" ),
QStringLiteral( "\\bMEDIUMINT\\b" ),
QStringLiteral( "\\bMEDIUMTEXT\\b" ),
QStringLiteral( "\\bMEMBER\\b" ),
QStringLiteral( "\\bMERGE\\b" ),
QStringLiteral( "\\bMETHOD\\b" ),
QStringLiteral( "\\bMICROSECOND\\b" ),
QStringLiteral( "\\bMICROSECONDS\\b" ),
QStringLiteral( "\\bMIDDLEINT\\b" ),
QStringLiteral( "\\bMIN\\b" ),
QStringLiteral( "\\bMINDEX\\b" ),
QStringLiteral( "\\bMINIMUM\\b" ),
QStringLiteral( "\\bMINUS\\b" ),
QStringLiteral( "\\bMINUTE\\b" ),
QStringLiteral( "\\bMINUTES\\b" ),
QStringLiteral( "\\bMINUTE_MICROSECOND\\b" ),
QStringLiteral( "\\bMINUTE_SECOND\\b" ),
QStringLiteral( "\\bMLINREG\\b" ),
QStringLiteral( "\\bMLOAD\\b" ),
QStringLiteral( "\\bMLSLABEL\\b" ),
QStringLiteral( "\\bMOD\\b" ),
QStringLiteral( "\\bMODE\\b" ),
QStringLiteral( "\\bMODIFIES\\b" ),
QStringLiteral( "\\bMODIFY\\b" ),
QStringLiteral( "\\bMODULE\\b" ),
QStringLiteral( "\\bMONITOR\\b" ),
QStringLiteral( "\\bMONRESOURCE\\b" ),
QStringLiteral( "\\bMONSESSION\\b" ),
QStringLiteral( "\\bMONTH\\b" ),
QStringLiteral( "\\bMONTHS\\b" ),
QStringLiteral( "\\bMSUBSTR\\b" ),
QStringLiteral( "\\bMSUM\\b" ),
QStringLiteral( "\\bMULTISET\\b" ),
QStringLiteral( "\\bNAMED\\b" ),
QStringLiteral( "\\bNAMES\\b" ),
QStringLiteral( "\\bNATIONAL\\b" ),
QStringLiteral( "\\bNATURAL\\b" ),
QStringLiteral( "\\bNCHAR\\b" ),
QStringLiteral( "\\bNCLOB\\b" ),
QStringLiteral( "\\bNE\\b" ),
QStringLiteral( "\\bNESTED_TABLE_ID\\b" ),
QStringLiteral( "\\bNEW\\b" ),
QStringLiteral( "\\bNEW_TABLE\\b" ),
QStringLiteral( "\\bNEXT\\b" ),
QStringLiteral( "\\bNEXTVAL\\b" ),
QStringLiteral( "\\bNO\\b" ),
QStringLiteral( "\\bNOAUDIT\\b" ),
QStringLiteral( "\\bNOCHECK\\b" ),
QStringLiteral( "\\bNOCOMPRESS\\b" ),
QStringLiteral( "\\bNONCLUSTERED\\b" ),
QStringLiteral( "\\bNONE\\b" ),
QStringLiteral( "\\bNORMALIZE\\b" ),
QStringLiteral( "\\bNOT\\b" ),
QStringLiteral( "\\bNOTNULL\\b" ),
QStringLiteral( "\\bNOWAIT\\b" ),
QStringLiteral( "\\bNO_WRITE_TO_BINLOG\\b" ),
QStringLiteral( "\\bNTH_VALUE\\b" ),
QStringLiteral( "\\bNTILE\\b" ),
QStringLiteral( "\\bNULL\\b" ),
QStringLiteral( "\\bNULLIF\\b" ),
QStringLiteral( "\\bNULLIFZERO\\b" ),
QStringLiteral( "\\bNULLS\\b" ),
QStringLiteral( "\\bNUMBER\\b" ),
QStringLiteral( "\\bNUMERIC\\b" ),
QStringLiteral( "\\bNUMPARTS\\b" ),
QStringLiteral( "\\bOBID\\b" ),
QStringLiteral( "\\bOBJECT\\b" ),
QStringLiteral( "\\bOBJECTS\\b" ),
QStringLiteral( "\\bOCCURRENCES_REGEX\\b" ),
QStringLiteral( "\\bOCTET_LENGTH\\b" ),
QStringLiteral( "\\bOF\\b" ),
QStringLiteral( "\\bOFF\\b" ),
QStringLiteral( "\\bOFFLINE\\b" ),
QStringLiteral( "\\bOFFSET\\b" ),
QStringLiteral( "\\bOFFSETS\\b" ),
QStringLiteral( "\\bOLD\\b" ),
QStringLiteral( "\\bOLD_TABLE\\b" ),
QStringLiteral( "\\bOMIT\\b" ),
QStringLiteral( "\\bON\\b" ),
QStringLiteral( "\\bONE\\b" ),
QStringLiteral( "\\bONLINE\\b" ),
QStringLiteral( "\\bONLY\\b" ),
QStringLiteral( "\\bOPEN\\b" ),
QStringLiteral( "\\bOPENDATASOURCE\\b" ),
QStringLiteral( "\\bOPENQUERY\\b" ),
QStringLiteral( "\\bOPENROWSET\\b" ),
QStringLiteral( "\\bOPENXML\\b" ),
QStringLiteral( "\\bOPERATION\\b" ),
QStringLiteral( "\\bOPTIMIZATION\\b" ),
QStringLiteral( "\\bOPTIMIZE\\b" ),
QStringLiteral( "\\bOPTIMIZER_COSTS\\b" ),
QStringLiteral( "\\bOPTION\\b" ),
QStringLiteral( "\\bOPTIONALLY\\b" ),
QStringLiteral( "\\bOR\\b" ),
QStringLiteral( "\\bORDER\\b" ),
QStringLiteral( "\\bORDINALITY\\b" ),
QStringLiteral( "\\bORGANIZATION\\b" ),
QStringLiteral( "\\bOUT\\b" ),
QStringLiteral( "\\bOUTER\\b" ),
QStringLiteral( "\\bOUTFILE\\b" ),
QStringLiteral( "\\bOUTPUT\\b" ),
QStringLiteral( "\\bOVER\\b" ),
QStringLiteral( "\\bOVERLAPS\\b" ),
QStringLiteral( "\\bOVERLAY\\b" ),
QStringLiteral( "\\bOVERRIDE\\b" ),
QStringLiteral( "\\bPACKAGE\\b" ),
QStringLiteral( "\\bPAD\\b" ),
QStringLiteral( "\\bPADDED\\b" ),
QStringLiteral( "\\bPARAMETER\\b" ),
QStringLiteral( "\\bPARAMETERS\\b" ),
QStringLiteral( "\\bPART\\b" ),
QStringLiteral( "\\bPARTIAL\\b" ),
QStringLiteral( "\\bPARTITION\\b" ),
QStringLiteral( "\\bPARTITIONED\\b" ),
QStringLiteral( "\\bPARTITIONING\\b" ),
QStringLiteral( "\\bPASSWORD\\b" ),
QStringLiteral( "\\bPATH\\b" ),
QStringLiteral( "\\bPATTERN\\b" ),
QStringLiteral( "\\bPCTFREE\\b" ),
QStringLiteral( "\\bPER\\b" ),
QStringLiteral( "\\bPERCENT\\b" ),
QStringLiteral( "\\bPERCENTILE_CONT\\b" ),
QStringLiteral( "\\bPERCENTILE_DISC\\b" ),
QStringLiteral( "\\bPERCENT_RANK\\b" ),
QStringLiteral( "\\bPERIOD\\b" ),
QStringLiteral( "\\bPERM\\b" ),
QStringLiteral( "\\bPERMANENT\\b" ),
QStringLiteral( "\\bPIECESIZE\\b" ),
QStringLiteral( "\\bPIVOT\\b" ),
QStringLiteral( "\\bPLACING\\b" ),
QStringLiteral( "\\bPLAN\\b" ),
QStringLiteral( "\\bPORTION\\b" ),
QStringLiteral( "\\bPOSITION\\b" ),
QStringLiteral( "\\bPOSITION_REGEX\\b" ),
QStringLiteral( "\\bPOSTFIX\\b" ),
QStringLiteral( "\\bPOWER\\b" ),
QStringLiteral( "\\bPRECEDES\\b" ),
QStringLiteral( "\\bPRECISION\\b" ),
QStringLiteral( "\\bPREFIX\\b" ),
QStringLiteral( "\\bPREORDER\\b" ),
QStringLiteral( "\\bPREPARE\\b" ),
QStringLiteral( "\\bPRESERVE\\b" ),
QStringLiteral( "\\bPREVVAL\\b" ),
QStringLiteral( "\\bPRIMARY\\b" ),
QStringLiteral( "\\bPRINT\\b" ),
QStringLiteral( "\\bPRIOR\\b" ),
QStringLiteral( "\\bPRIQTY\\b" ),
QStringLiteral( "\\bPRIVATE\\b" ),
QStringLiteral( "\\bPRIVILEGES\\b" ),
QStringLiteral( "\\bPROC\\b" ),
QStringLiteral( "\\bPROCEDURE\\b" ),
QStringLiteral( "\\bPROFILE\\b" ),
QStringLiteral( "\\bPROGRAM\\b" ),
QStringLiteral( "\\bPROPORTIONAL\\b" ),
QStringLiteral( "\\bPROTECTION\\b" ),
QStringLiteral( "\\bPSID\\b" ),
QStringLiteral( "\\bPTF\\b" ),
QStringLiteral( "\\bPUBLIC\\b" ),
QStringLiteral( "\\bPURGE\\b" ),
QStringLiteral( "\\bQUALIFIED\\b" ),
QStringLiteral( "\\bQUALIFY\\b" ),
QStringLiteral( "\\bQUANTILE\\b" ),
QStringLiteral( "\\bQUERY\\b" ),
QStringLiteral( "\\bQUERYNO\\b" ),
QStringLiteral( "\\bRADIANS\\b" ),
QStringLiteral( "\\bRAISERROR\\b" ),
QStringLiteral( "\\bRANDOM\\b" ),
QStringLiteral( "\\bRANGE\\b" ),
QStringLiteral( "\\bRANGE_N\\b" ),
QStringLiteral( "\\bRANK\\b" ),
QStringLiteral( "\\bRAW\\b" ),
QStringLiteral( "\\bREAD\\b" ),
QStringLiteral( "\\bREADS\\b" ),
QStringLiteral( "\\bREADTEXT\\b" ),
QStringLiteral( "\\bREAD_WRITE\\b" ),
QStringLiteral( "\\bREAL\\b" ),
QStringLiteral( "\\bRECONFIGURE\\b" ),
QStringLiteral( "\\bRECURSIVE\\b" ),
QStringLiteral( "\\bREF\\b" ),
QStringLiteral( "\\bREFERENCES\\b" ),
QStringLiteral( "\\bREFERENCING\\b" ),
QStringLiteral( "\\bREFRESH\\b" ),
QStringLiteral( "\\bREGEXP\\b" ),
QStringLiteral( "\\bREGR_AVGX\\b" ),
QStringLiteral( "\\bREGR_AVGY\\b" ),
QStringLiteral( "\\bREGR_COUNT\\b" ),
QStringLiteral( "\\bREGR_INTERCEPT\\b" ),
QStringLiteral( "\\bREGR_R2\\b" ),
QStringLiteral( "\\bREGR_SLOPE\\b" ),
QStringLiteral( "\\bREGR_SXX\\b" ),
QStringLiteral( "\\bREGR_SXY\\b" ),
QStringLiteral( "\\bREGR_SYY\\b" ),
QStringLiteral( "\\bRELATIVE\\b" ),
QStringLiteral( "\\bRELEASE\\b" ),
QStringLiteral( "\\bRENAME\\b" ),
QStringLiteral( "\\bREPEAT\\b" ),
QStringLiteral( "\\bREPLACE\\b" ),
QStringLiteral( "\\bREPLICATION\\b" ),
QStringLiteral( "\\bREPOVERRIDE\\b" ),
QStringLiteral( "\\bREQUEST\\b" ),
QStringLiteral( "\\bREQUIRE\\b" ),
QStringLiteral( "\\bRESIGNAL\\b" ),
QStringLiteral( "\\bRESOURCE\\b" ),
QStringLiteral( "\\bRESTART\\b" ),
QStringLiteral( "\\bRESTORE\\b" ),
QStringLiteral( "\\bRESTRICT\\b" ),
QStringLiteral( "\\bRESULT\\b" ),
QStringLiteral( "\\bRESULT_SET_LOCATOR\\b" ),
QStringLiteral( "\\bRESUME\\b" ),
QStringLiteral( "\\bRET\\b" ),
QStringLiteral( "\\bRETRIEVE\\b" ),
QStringLiteral( "\\bRETURN\\b" ),
QStringLiteral( "\\bRETURNING\\b" ),
QStringLiteral( "\\bRETURNS\\b" ),
QStringLiteral( "\\bREVALIDATE\\b" ),
QStringLiteral( "\\bREVERT\\b" ),
QStringLiteral( "\\bREVOKE\\b" ),
QStringLiteral( "\\bRIGHT\\b" ),
QStringLiteral( "\\bRIGHTS\\b" ),
QStringLiteral( "\\bRLIKE\\b" ),
QStringLiteral( "\\bROLE\\b" ),
QStringLiteral( "\\bROLLBACK\\b" ),
QStringLiteral( "\\bROLLFORWARD\\b" ),
QStringLiteral( "\\bROLLUP\\b" ),
QStringLiteral( "\\bROUND_CEILING\\b" ),
QStringLiteral( "\\bROUND_DOWN\\b" ),
QStringLiteral( "\\bROUND_FLOOR\\b" ),
QStringLiteral( "\\bROUND_HALF_DOWN\\b" ),
QStringLiteral( "\\bROUND_HALF_EVEN\\b" ),
QStringLiteral( "\\bROUND_HALF_UP\\b" ),
QStringLiteral( "\\bROUND_UP\\b" ),
QStringLiteral( "\\bROUTINE\\b" ),
QStringLiteral( "\\bROW\\b" ),
QStringLiteral( "\\bROWCOUNT\\b" ),
QStringLiteral( "\\bROWGUIDCOL\\b" ),
QStringLiteral( "\\bROWID\\b" ),
QStringLiteral( "\\bROWNUM\\b" ),
QStringLiteral( "\\bROWS\\b" ),
QStringLiteral( "\\bROWSET\\b" ),
QStringLiteral( "\\bROW_NUMBER\\b" ),
QStringLiteral( "\\bRPAD\\b" ),
QStringLiteral( "\\bRULE\\b" ),
QStringLiteral( "\\bRUN\\b" ),
QStringLiteral( "\\bRUNNING\\b" ),
QStringLiteral( "\\bSAMPLE\\b" ),
QStringLiteral( "\\bSAMPLEID\\b" ),
QStringLiteral( "\\bSAVE\\b" ),
QStringLiteral( "\\bSAVEPOINT\\b" ),
QStringLiteral( "\\bSCHEMA\\b" ),
QStringLiteral( "\\bSCHEMAS\\b" ),
QStringLiteral( "\\bSCOPE\\b" ),
QStringLiteral( "\\bSCRATCHPAD\\b" ),
QStringLiteral( "\\bSCROLL\\b" ),
QStringLiteral( "\\bSEARCH\\b" ),
QStringLiteral( "\\bSECOND\\b" ),
QStringLiteral( "\\bSECONDS\\b" ),
QStringLiteral( "\\bSECOND_MICROSECOND\\b" ),
QStringLiteral( "\\bSECQTY\\b" ),
QStringLiteral( "\\bSECTION\\b" ),
QStringLiteral( "\\bSECURITY\\b" ),
QStringLiteral( "\\bSECURITYAUDIT\\b" ),
QStringLiteral( "\\bSEEK\\b" ),
QStringLiteral( "\\bSEL\\b" ),
QStringLiteral( "\\bSELECT\\b" ),
QStringLiteral( "\\bSEMANTICKEYPHRASETABLE\\b" ),
QStringLiteral( "\\bSEMANTICSIMILARITYDETAILSTABLE\\b" ),
QStringLiteral( "\\bSEMANTICSIMILARITYTABLE\\b" ),
QStringLiteral( "\\bSENSITIVE\\b" ),
QStringLiteral( "\\bSEPARATOR\\b" ),
QStringLiteral( "\\bSEQUENCE\\b" ),
QStringLiteral( "\\bSESSION\\b" ),
QStringLiteral( "\\bSESSION_USER\\b" ),
QStringLiteral( "\\bSET\\b" ),
QStringLiteral( "\\bSETRESRATE\\b" ),
QStringLiteral( "\\bSETS\\b" ),
QStringLiteral( "\\bSETSESSRATE\\b" ),
QStringLiteral( "\\bSETUSER\\b" ),
QStringLiteral( "\\bSHARE\\b" ),
QStringLiteral( "\\bSHOW\\b" ),
QStringLiteral( "\\bSHUTDOWN\\b" ),
QStringLiteral( "\\bSIGNAL\\b" ),
QStringLiteral( "\\bSIMILAR\\b" ),
QStringLiteral( "\\bSIMPLE\\b" ),
QStringLiteral( "\\bSIN\\b" ),
QStringLiteral( "\\bSINH\\b" ),
QStringLiteral( "\\bSIZE\\b" ),
QStringLiteral( "\\bSKEW\\b" ),
QStringLiteral( "\\bSKIP\\b" ),
QStringLiteral( "\\bSMALLINT\\b" ),
QStringLiteral( "\\bSOME\\b" ),
QStringLiteral( "\\bSOUNDEX\\b" ),
QStringLiteral( "\\bSOURCE\\b" ),
QStringLiteral( "\\bSPACE\\b" ),
QStringLiteral( "\\bSPATIAL\\b" ),
QStringLiteral( "\\bSPECIFIC\\b" ),
QStringLiteral( "\\bSPECIFICTYPE\\b" ),
QStringLiteral( "\\bSPOOL\\b" ),
QStringLiteral( "\\bSQL\\b" ),
QStringLiteral( "\\bSQLEXCEPTION\\b" ),
QStringLiteral( "\\bSQLSTATE\\b" ),
QStringLiteral( "\\bSQLTEXT\\b" ),
QStringLiteral( "\\bSQLWARNING\\b" ),
QStringLiteral( "\\bSQL_BIG_RESULT\\b" ),
QStringLiteral( "\\bSQL_CALC_FOUND_ROWS\\b" ),
QStringLiteral( "\\bSQL_SMALL_RESULT\\b" ),
QStringLiteral( "\\bSQRT\\b" ),
QStringLiteral( "\\bSS\\b" ),
QStringLiteral( "\\bSSL\\b" ),
QStringLiteral( "\\bSTANDARD\\b" ),
QStringLiteral( "\\bSTART\\b" ),
QStringLiteral( "\\bSTARTING\\b" ),
QStringLiteral( "\\bSTARTUP\\b" ),
QStringLiteral( "\\bSTATE\\b" ),
QStringLiteral( "\\bSTATEMENT\\b" ),
QStringLiteral( "\\bSTATIC\\b" ),
QStringLiteral( "\\bSTATISTICS\\b" ),
QStringLiteral( "\\bSTAY\\b" ),
QStringLiteral( "\\bSTDDEV_POP\\b" ),
QStringLiteral( "\\bSTDDEV_SAMP\\b" ),
QStringLiteral( "\\bSTEPINFO\\b" ),
QStringLiteral( "\\bSTOGROUP\\b" ),
QStringLiteral( "\\bSTORED\\b" ),
QStringLiteral( "\\bSTORES\\b" ),
QStringLiteral( "\\bSTRAIGHT_JOIN\\b" ),
QStringLiteral( "\\bSTRING_CS\\b" ),
QStringLiteral( "\\bSTRUCTURE\\b" ),
QStringLiteral( "\\bSTYLE\\b" ),
QStringLiteral( "\\bSUBMULTISET\\b" ),
QStringLiteral( "\\bSUBSCRIBER\\b" ),
QStringLiteral( "\\bSUBSET\\b" ),
QStringLiteral( "\\bSUBSTR\\b" ),
QStringLiteral( "\\bSUBSTRING\\b" ),
QStringLiteral( "\\bSUBSTRING_REGEX\\b" ),
QStringLiteral( "\\bSUCCEEDS\\b" ),
QStringLiteral( "\\bSUCCESSFUL\\b" ),
QStringLiteral( "\\bSUM\\b" ),
QStringLiteral( "\\bSUMMARY\\b" ),
QStringLiteral( "\\bSUSPEND\\b" ),
QStringLiteral( "\\bSYMMETRIC\\b" ),
QStringLiteral( "\\bSYNONYM\\b" ),
QStringLiteral( "\\bSYSDATE\\b" ),
QStringLiteral( "\\bSYSTEM\\b" ),
QStringLiteral( "\\bSYSTEM_TIME\\b" ),
QStringLiteral( "\\bSYSTEM_USER\\b" ),
QStringLiteral( "\\bSYSTIMESTAMP\\b" ),
QStringLiteral( "\\bTABLE\\b" ),
QStringLiteral( "\\bTABLESAMPLE\\b" ),
QStringLiteral( "\\bTABLESPACE\\b" ),
QStringLiteral( "\\bTAN\\b" ),
QStringLiteral( "\\bTANH\\b" ),
QStringLiteral( "\\bTBL_CS\\b" ),
QStringLiteral( "\\bTEMPORARY\\b" ),
QStringLiteral( "\\bTERMINATE\\b" ),
QStringLiteral( "\\bTERMINATED\\b" ),
QStringLiteral( "\\bTEXTSIZE\\b" ),
QStringLiteral( "\\bTHAN\\b" ),
QStringLiteral( "\\bTHEN\\b" ),
QStringLiteral( "\\bTHRESHOLD\\b" ),
QStringLiteral( "\\bTIME\\b" ),
QStringLiteral( "\\bTIMESTAMP\\b" ),
QStringLiteral( "\\bTIMEZONE_HOUR\\b" ),
QStringLiteral( "\\bTIMEZONE_MINUTE\\b" ),
QStringLiteral( "\\bTINYBLOB\\b" ),
QStringLiteral( "\\bTINYINT\\b" ),
QStringLiteral( "\\bTINYTEXT\\b" ),
QStringLiteral( "\\bTITLE\\b" ),
QStringLiteral( "\\bTO\\b" ),
QStringLiteral( "\\bTOP\\b" ),
QStringLiteral( "\\bTRACE\\b" ),
QStringLiteral( "\\bTRAILING\\b" ),
QStringLiteral( "\\bTRAN\\b" ),
QStringLiteral( "\\bTRANSACTION\\b" ),
QStringLiteral( "\\bTRANSLATE\\b" ),
QStringLiteral( "\\bTRANSLATE_CHK\\b" ),
QStringLiteral( "\\bTRANSLATE_REGEX\\b" ),
QStringLiteral( "\\bTRANSLATION\\b" ),
QStringLiteral( "\\bTREAT\\b" ),
QStringLiteral( "\\bTRIGGER\\b" ),
QStringLiteral( "\\bTRIM\\b" ),
QStringLiteral( "\\bTRIM_ARRAY\\b" ),
QStringLiteral( "\\bTRUE\\b" ),
QStringLiteral( "\\bTRUNCATE\\b" ),
QStringLiteral( "\\bTRY_CONVERT\\b" ),
QStringLiteral( "\\bTSEQUAL\\b" ),
QStringLiteral( "\\bTYPE\\b" ),
QStringLiteral( "\\bUC\\b" ),
QStringLiteral( "\\bUESCAPE\\b" ),
QStringLiteral( "\\bUID\\b" ),
QStringLiteral( "\\bUNDEFINED\\b" ),
QStringLiteral( "\\bUNDER\\b" ),
QStringLiteral( "\\bUNDO\\b" ),
QStringLiteral( "\\bUNION\\b" ),
QStringLiteral( "\\bUNIQUE\\b" ),
QStringLiteral( "\\bUNKNOWN\\b" ),
QStringLiteral( "\\bUNLOCK\\b" ),
QStringLiteral( "\\bUNNEST\\b" ),
QStringLiteral( "\\bUNPIVOT\\b" ),
QStringLiteral( "\\bUNSIGNED\\b" ),
QStringLiteral( "\\bUNTIL\\b" ),
QStringLiteral( "\\bUPD\\b" ),
QStringLiteral( "\\bUPDATE\\b" ),
QStringLiteral( "\\bUPDATETEXT\\b" ),
QStringLiteral( "\\bUPPER\\b" ),
QStringLiteral( "\\bUPPERCASE\\b" ),
QStringLiteral( "\\bUSAGE\\b" ),
QStringLiteral( "\\bUSE\\b" ),
QStringLiteral( "\\bUSER\\b" ),
QStringLiteral( "\\bUSING\\b" ),
QStringLiteral( "\\bUTC_DATE\\b" ),
QStringLiteral( "\\bUTC_TIME\\b" ),
QStringLiteral( "\\bUTC_TIMESTAMP\\b" ),
QStringLiteral( "\\bVALIDATE\\b" ),
QStringLiteral( "\\bVALIDPROC\\b" ),
QStringLiteral( "\\bVALUE\\b" ),
QStringLiteral( "\\bVALUES\\b" ),
QStringLiteral( "\\bVALUE_OF\\b" ),
QStringLiteral( "\\bVARBINARY\\b" ),
QStringLiteral( "\\bVARBYTE\\b" ),
QStringLiteral( "\\bVARCHAR\\b" ),
QStringLiteral( "\\bVARCHAR2\\b" ),
QStringLiteral( "\\bVARCHARACTER\\b" ),
QStringLiteral( "\\bVARGRAPHIC\\b" ),
QStringLiteral( "\\bVARIABLE\\b" ),
QStringLiteral( "\\bVARIADIC\\b" ),
QStringLiteral( "\\bVARIANT\\b" ),
QStringLiteral( "\\bVARYING\\b" ),
QStringLiteral( "\\bVAR_POP\\b" ),
QStringLiteral( "\\bVAR_SAMP\\b" ),
QStringLiteral( "\\bVCAT\\b" ),
QStringLiteral( "\\bVERBOSE\\b" ),
QStringLiteral( "\\bVERSIONING\\b" ),
QStringLiteral( "\\bVIEW\\b" ),
QStringLiteral( "\\bVIRTUAL\\b" ),
QStringLiteral( "\\bVOLATILE\\b" ),
QStringLiteral( "\\bVOLUMES\\b" ),
QStringLiteral( "\\bWAIT\\b" ),
QStringLiteral( "\\bWAITFOR\\b" ),
QStringLiteral( "\\bWHEN\\b" ),
QStringLiteral( "\\bWHENEVER\\b" ),
QStringLiteral( "\\bWHERE\\b" ),
QStringLiteral( "\\bWHILE\\b" ),
QStringLiteral( "\\bWIDTH_BUCKET\\b" ),
QStringLiteral( "\\bWINDOW\\b" ),
QStringLiteral( "\\bWITH\\b" ),
QStringLiteral( "\\bWITHIN\\b" ),
QStringLiteral( "\\bWITHIN_GROUP\\b" ),
QStringLiteral( "\\bWITHOUT\\b" ),
QStringLiteral( "\\bWLM\\b" ),
QStringLiteral( "\\bWORK\\b" ),
QStringLiteral( "\\bWRITE\\b" ),
QStringLiteral( "\\bWRITETEXT\\b" ),
QStringLiteral( "\\bXMLCAST\\b" ),
QStringLiteral( "\\bXMLEXISTS\\b" ),
QStringLiteral( "\\bXMLNAMESPACES\\b" ),
QStringLiteral( "\\bXOR\\b" ),
QStringLiteral( "\\bYEAR\\b" ),
QStringLiteral( "\\bYEARS\\b" ),
QStringLiteral( "\\bYEAR_MONTH\\b" ),
QStringLiteral( "\\bZEROFILL\\b" ),
QStringLiteral( "\\bZEROIFNULL\\b" ),
QStringLiteral( "\\bZONE\\b" )
    };

    // orange: 23,155,255,255
    QColor color;

    keywordFormat.setForeground( Qt::yellow );
    keywordFormat.setFontWeight( QFont::Bold );

    for ( const QString &pattern : keywordPatterns )
    {
        rule.pattern = QRegularExpression( pattern, QRegularExpression::CaseInsensitiveOption );
        rule.format = keywordFormat;
        highlightingRules.append( rule );
    }

    classFormat.setFontWeight( QFont::Bold );
    classFormat.setForeground( Qt::darkMagenta );
    rule.pattern = QRegularExpression( QStringLiteral( "\\bQ[A-Za-z]+\\b" ) );
    rule.format = classFormat;
    highlightingRules.append( rule );

    quotationFormat.setForeground( Qt::green );
    rule.pattern = QRegularExpression( QStringLiteral( "\".*\"" ) );
    rule.format = quotationFormat;
    highlightingRules.append( rule );

    // functionFormat.setFontItalic( true );
    color.setHsv( 23, 155, 255, 255 );
    functionFormat.setForeground( color );
    rule.pattern = QRegularExpression( QStringLiteral( "\\b[A-Za-z0-9_]+(?=\\()" ) );
    rule.format = functionFormat;
    highlightingRules.append( rule );

    singleLineCommentFormat.setForeground( Qt::cyan );
    rule.pattern = QRegularExpression( QStringLiteral( "//[^\n]*" ) );
    rule.format = singleLineCommentFormat;
    highlightingRules.append( rule );

    multiLineCommentFormat.setForeground( Qt::cyan );

    commentStartExpression = QRegularExpression( QStringLiteral( "/\\*" ) );
    commentEndExpression = QRegularExpression( QStringLiteral( "\\*/" ) );
}

void WSQL92Highlighter::highlightBlock( const QString &text )
{
    for ( const HighlightingRule &rule : qAsConst( highlightingRules ) )
    {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch( text );
        while ( matchIterator.hasNext() )
        {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat( match.capturedStart(), match.capturedLength(), rule.format );
        }
    }

    setCurrentBlockState( 0 );

    int startIndex = 0;
    if ( previousBlockState() != 1 ) startIndex = text.indexOf( commentStartExpression );

    while ( startIndex >= 0 )
    {
        QRegularExpressionMatch match = commentEndExpression.match( text, startIndex );
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if ( endIndex == -1 )
        {
            setCurrentBlockState( 1 );
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex + match.capturedLength();
        }
        setFormat( startIndex, commentLength, multiLineCommentFormat );
        startIndex = text.indexOf( commentStartExpression, startIndex + commentLength );
    }
}


//
//
//
WCPPHighlighter::WCPPHighlighter( QTextDocument *parent )
    : QSyntaxHighlighter( parent )
{
    HighlightingRule rule;

    keywordFormat.setForeground( Qt::darkBlue );
    keywordFormat.setFontWeight( QFont::Bold );

    const QString keywordPatterns[] = {
        QStringLiteral( "\\bchar\\b" ), QStringLiteral( "\\bclass\\b" ), QStringLiteral( "\\bconst\\b" ),
        QStringLiteral( "\\bdouble\\b" ), QStringLiteral( "\\benum\\b" ), QStringLiteral( "\\bexplicit\\b" ),
        QStringLiteral( "\\bfriend\\b" ), QStringLiteral( "\\binline\\b" ), QStringLiteral( "\\bint\\b" ),
        QStringLiteral( "\\blong\\b" ), QStringLiteral( "\\bnamespace\\b" ), QStringLiteral( "\\boperator\\b" ),
        QStringLiteral( "\\bprivate\\b" ), QStringLiteral( "\\bprotected\\b" ), QStringLiteral( "\\bpublic\\b" ),
        QStringLiteral( "\\bshort\\b" ), QStringLiteral( "\\bsignals\\b" ), QStringLiteral( "\\bsigned\\b" ),
        QStringLiteral( "\\bslots\\b" ), QStringLiteral( "\\bstatic\\b" ), QStringLiteral( "\\bstruct\\b" ),
        QStringLiteral( "\\btemplate\\b" ), QStringLiteral( "\\btypedef\\b" ), QStringLiteral( "\\btypename\\b" ),
        QStringLiteral( "\\bunion\\b" ), QStringLiteral( "\\bunsigned\\b" ), QStringLiteral( "\\bvirtual\\b" ),
        QStringLiteral( "\\bvoid\\b" ), QStringLiteral( "\\bvolatile\\b" ), QStringLiteral( "\\bbool\\b" )
    };

    for ( const QString &pattern : keywordPatterns )
    {
        rule.pattern = QRegularExpression( pattern );
        rule.format = keywordFormat;
        highlightingRules.append( rule );
    }

    classFormat.setFontWeight( QFont::Bold );
    classFormat.setForeground( Qt::darkMagenta );
    rule.pattern = QRegularExpression( QStringLiteral( "\\bQ[A-Za-z]+\\b" ) );
    rule.format = classFormat;
    highlightingRules.append( rule );

    quotationFormat.setForeground( Qt::darkGreen );
    rule.pattern = QRegularExpression( QStringLiteral( "\".*\"" ) );
    rule.format = quotationFormat;
    highlightingRules.append( rule );

    functionFormat.setFontItalic( true );
    functionFormat.setForeground( Qt::blue );
    rule.pattern = QRegularExpression( QStringLiteral( "\\b[A-Za-z0-9_]+(?=\\()" ) );
    rule.format = functionFormat;
    highlightingRules.append( rule );

    singleLineCommentFormat.setForeground( Qt::red );
    rule.pattern = QRegularExpression( QStringLiteral( "//[^\n]*" ) );
    rule.format = singleLineCommentFormat;
    highlightingRules.append( rule );

    multiLineCommentFormat.setForeground( Qt::red );

    commentStartExpression = QRegularExpression( QStringLiteral( "/\\*" ) );
    commentEndExpression = QRegularExpression( QStringLiteral( "\\*/" ) );
}

void WCPPHighlighter::highlightBlock( const QString &text )
{
    for ( const HighlightingRule &rule : qAsConst( highlightingRules ) )
    {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch( text );
        while ( matchIterator.hasNext() )
        {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat( match.capturedStart(), match.capturedLength(), rule.format );
        }
    }

    setCurrentBlockState( 0 );

    int startIndex = 0;
    if ( previousBlockState() != 1 ) startIndex = text.indexOf( commentStartExpression );

    while ( startIndex >= 0 )
    {
        QRegularExpressionMatch match = commentEndExpression.match( text, startIndex );
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if ( endIndex == -1 )
        {
            setCurrentBlockState( 1 );
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex + match.capturedLength();
        }
        setFormat( startIndex, commentLength, multiLineCommentFormat );
        startIndex = text.indexOf( commentStartExpression, startIndex + commentLength );
    }
}



