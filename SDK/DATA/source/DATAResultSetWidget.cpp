// #include <ctype.h>
#include "LibInfo.h"
#include "DATAResultSetWidget.h"

#include "DATAStatement.h"

#define MAX_COLUMN_WIDTH 255   /* see szColumn */

DATAResultSetWidget::DATAResultSetWidget( QWidget *pParent )
: QWidget( pParent )
{
    setObjectName( metaObject()->className() );

    pStatement              = 0;
    stringFileName          = "";
    nResultFormat           = ResultFormatGUIGrid;
    nRows                   = 0;
    nLimit                = 5000;
    bFirstRowColumnNames    = false;
    stringDelimitChar       = "|";

    // init GUI grid
    ptableResults = new QTableWidget( this );
    ptableResults->setRowCount( 1 );
    ptableResults->setEditTriggers( QAbstractItemView::NoEditTriggers );
    // ptableResults->setColumnMovingEnabled ( true );

    // init HTML grid
    ptextbrowserResults = new QTextBrowser( this );

    // init text view
    QFont qf( "Fixed", 10 ); // we want a fixed font of some sort
    qf.setFixedPitch( true );
    ptexteditResults = new QTextEdit( this );
    ptexteditResults->setObjectName( "ptexteditResults" );
    ptexteditResults->setLineWrapMode( QTextEdit::NoWrap );
    ptexteditResults->setFont( qf );
    ptexteditResults->setReadOnly( true );

    doLoadState();
}


DATAResultSetWidget::~DATAResultSetWidget()
{
    doSaveState();
}

void DATAResultSetWidget::setResultFormat( ResultFormatTypes n )
{
    nResultFormat = n;
    // use any results in hStmt and display in new format
}

int DATAResultSetWidget::getResultFormat()
{ 
    return nResultFormat; 
}

QString DATAResultSetWidget::getResult()
{
    if ( getResultFormat() == ResultFormatHTMLGrid )
        return ptextbrowserResults->toHtml();
    else if ( getResultFormat() == ResultFormatTextGrid ) 
        return ptexteditResults->toPlainText();
    else if ( getResultFormat() == ResultFormatTextDelimited ) 
        return ptexteditResults->toPlainText();

    return "Export not availible for this result format (GUI Grid).";
}

/*!
 * \brief Process current result-set.
 * 
 * \author pharvey (7/3/20)
 * 
 * \param pStatement 
 */
void DATAResultSetWidget::doResult( DATAStatement *pStatement )
{
    ptableResults->hide();
    ptextbrowserResults->hide();
    ptexteditResults->hide();
    doClear();

    this->pStatement = pStatement;
    
    if ( getResultFormat() == ResultFormatHTMLGrid ) 
    {
        doResultHTMLGrid();
        ptextbrowserResults->show();
    }
    else if ( getResultFormat() == ResultFormatTextGrid ) 
    {
        doResultTextGrid();
        ptexteditResults->show();
    }
    else if ( getResultFormat() == ResultFormatTextDelimited ) 
    {
        doResultTextDelimited();
        ptexteditResults->show();
    }
    else
    {
        doResultGUIGrid();
        ptableResults->show();
    }

    this->pStatement = nullptr;
}

void DATAResultSetWidget::doClear()
{
    nRows = 0;
    pStatement = 0;
    ptableResults->setColumnCount( 0 );
    ptableResults->setRowCount( 0 );
    ptextbrowserResults->setText( "" );
    ptexteditResults->clear();
}

bool DATAResultSetWidget::doSave( const QString &stringFileName )
{
    // get text
    QString stringResultSet;

    if ( getResultFormat() == ResultFormatHTMLGrid ) 
    {
        stringResultSet = ptextbrowserResults->toHtml();
    }
    else if ( getResultFormat() == ResultFormatTextGrid ) 
    {
        stringResultSet = ptexteditResults->toPlainText();
    }
    else if ( getResultFormat() == ResultFormatTextDelimited ) 
    {
        stringResultSet = ptexteditResults->toPlainText();
    }
    else
    {
        qWarning( "[PAH][%s][%s][%d] Can not save result-set in GUI format.\n", __FILE__, __FUNCTION__, __LINE__ );
        return false;
    }

    // save text
    QFile data( stringFileName );
    if ( data.open( QFile::WriteOnly | QFile::Truncate ) ) 
    {
        QTextStream out( &data );
        out << stringResultSet;
    }
    else
    {
        qWarning( "[PAH][%s][%s][%d] Failed to open %s.\n", __FILE__, __FUNCTION__, __LINE__, stringFileName.toUtf8().constData() );
        return false;
    }

    return true;
}

void DATAResultSetWidget::slotClear()
{
    doClear();
}

void DATAResultSetWidget::resizeEvent( QResizeEvent *p )
{
    resize( p->size() );

    ptableResults->resize( p->size() );
    ptextbrowserResults->resize( p->size() );
    ptexteditResults->resize( p->size() );
}

void DATAResultSetWidget::doResultTextGrid()
{
    QString     stringHorizSep;
    SWORD       nColumns;

    ptexteditResults->clear();

    if ( !pStatement ) return;

    nColumns = pStatement->getColumnCount();

    // GET A RESULTS HEADER (column headers)
    doResultTextGridHeader( nColumns, &stringHorizSep );
    // GET A RESULTS BODY (data)
    if ( nColumns > 0 ) doResultTextGridBody( nColumns );

    ptexteditResults->append( stringHorizSep );
    ptexteditResults->show();
}

void DATAResultSetWidget::doResultTextGridHeader( SWORD nColumns, QString *pqsHorizSep )
{
    QString     qsColumnHeader      = "";
    QString     qsColumn            = "";
    QString     qsColumnName        = "";
    int         nCol;
    SQLLEN      nMaxLength          = 10;
    QVariant    v;

    *pqsHorizSep = "";
    for ( nCol = 1; nCol <= nColumns; nCol++ )
    {
        int nWidth;

        v = pStatement->getColAttribute( nCol, SQL_DESC_DISPLAY_SIZE );
        if ( v.isNull() ) nMaxLength = 10;
        else nMaxLength = v.toInt();

        v = pStatement->getColAttribute( nCol, SQL_DESC_LABEL );
        {
            QString s = v.toString();
            // get rid of any unicode null chars
            s = s.replace( '\u0000', ' ' );
            qsColumnName = s.trimmed();
        }
        nWidth = qMax( (int)nMaxLength, qsColumnName.length() );
        nWidth = qMin( nWidth, MAX_COLUMN_WIDTH );
        qsColumn.fill( '-', nWidth );
        qsColumn += "-";
        *pqsHorizSep += ( "+" + qsColumn );
        qsColumn = QString( "| %1" ).arg( qsColumnName.toLatin1().constData(), nWidth );
        qsColumnHeader += qsColumn;
    }
//    *pqsHorizSep += "+\n";
//    qsColumnHeader += "|\n";
    *pqsHorizSep += "+";
    qsColumnHeader += "|";

    ptexteditResults->append( *pqsHorizSep );
    ptexteditResults->append( qsColumnHeader );
    ptexteditResults->append( *pqsHorizSep );

    return;
}

/*!
    doResultTextGridBody

    Display the data part of results formatted in a text box.
    
    NOTE: Do NOT rely upon SQLRowCount() as it often returns a misleading -1. For example
          when dealing with Driver generated result sets and other catalog type functions.
*/    
void DATAResultSetWidget::doResultTextGridBody( SWORD nColumns )
{
    QString         qsLine;
    QString         qsColumn;
    QString         qsColumnName        = "";
    SQLUSMALLINT    nCol                = 0;
    SQLLEN          nMaxLength          = 10;
    QVariant        v;

    // We want to report progress to user but we have no certian number of rows to base
    // the progress on so we use an arbitrary number and reset progress when/if exceeded.
    int             nProgressMax        = 1000;             // number of ticks in progress
    int             nProgress           = 0;                // number of ticks shown in progress

    // We want to limit the frequency we update the progress because updating the
    // progress for each row slows the process down too much.
    int             nRowsToTickMax      = 1000;             // number of rows before we update progress
    int             nRowsToTick         = nRowsToTickMax;   // number of rows left before we update progress

    if ( nLimit > 0 )
        nProgressMax = nLimit;

    QProgressDialog progress( "Formatting results...", "Cancel", 0, nProgressMax, this );
    progress.setModal( true );
    progress.setAutoClose( false );

    // PROCESS ALL ROWS
    // we should be sure that we will not empty result set
    // othervise it will break SQLRowCount
    for( ; SQL_SUCCEEDED( pStatement->doFetch() ); )
    {
        int nWidth;

        nRows++;
        if ( nLimit > 0 && nRows > nLimit )
        {
            emit signalMessage( "WARNING", "Results truncated." );
            break;
        }

        qsLine = "";
        // PROCESS ALL COLUMNS
        for ( nCol = 1; nCol <= nColumns; nCol++ )
        {
            v = pStatement->getColAttribute( nCol, SQL_DESC_DISPLAY_SIZE );
            if ( v.isNull() ) nMaxLength = 10;
            else nMaxLength = v.toInt();

            v = pStatement->getColAttribute( nCol, SQL_DESC_LABEL );
            qsColumnName = v.toString();

            v = pStatement->getData( nCol );

            nWidth = qMax( (int)nMaxLength, qsColumnName.length() );
            nWidth = qMin( nWidth, MAX_COLUMN_WIDTH );
            if ( !v.isNull() )
                qsColumn = QString( "| %1" ).arg( v.toString().toLatin1().constData(), nWidth );
            else
                qsColumn = QString( "| %1" ).arg( "", nWidth );

            qsLine += qsColumn;
        }
        qsLine += "|";
        ptexteditResults->append( qsLine );
    
        // update progress
        nRowsToTick--;
        if ( nRowsToTick < 1 )
        {
            nRowsToTick = nRowsToTickMax;
            nProgress++;
            if ( nProgress >= nProgressMax )
            {
                nProgress = 0;
                progress.reset();
            }
            progress.setValue( nProgress );
    
            qApp->processEvents();
            if ( progress.wasCanceled() )
                break;
        }

    } // while rows
}

void DATAResultSetWidget::doResultGUIGrid()
{
    SQLLEN  nRows;
    SWORD       nCols;

    // GET NUMBER OF ROWS AFFECTED
    // - the postgreSQL driver will always return -1 for result sets generated by the driver
    //   so lets try to get by without relying on SQLRowCount
    nRows = pStatement->getRowCount();

    // GET NUMBER OF COLUMNS RETURNED
    nCols = pStatement->getColumnCount();

    if ( nRows < 0 ) nRows = 0;
    if ( nCols < 0 ) nCols = 0;

    if ( nLimit > 0 ) ptableResults->setRowCount( qMin( (int)nRows, nLimit ) );
    else ptableResults->setRowCount( nRows );

    ptableResults->setColumnCount( nCols );

    // GET A RESULTS HEADER (column headers)
    doResultGUIGridHeader( nCols );
    // GET A RESULTS BODY (data)
    if ( nCols > 0 ) doResultGUIGridBody( nCols );

    ptableResults->resize( this->size() );
    ptableResults->show();
}

void DATAResultSetWidget::doResultGUIGridHeader( SWORD nColumns )
{
    int             nCol;
    QStringList     stringlistHeader;
    QVariant        v;

    for( nCol = 0; nCol < nColumns; nCol++ )
    {
        v = pStatement->getColAttribute( nCol+1, SQL_DESC_LABEL );
        QString stringLabel = v.toString();
        // get rid of any unicode null chars
        stringLabel = stringLabel.replace( '\u0000', ' ' );
        stringlistHeader << stringLabel.trimmed();
    }

    ptableResults->setHorizontalHeaderLabels( stringlistHeader );
}

/*!
    doResultGUIGridBody

    Display the data part of results formatted in a GUI grid.
    
    NOTE: Do NOT rely upon SQLRowCount() as it often returns a misleading -1. For example
          when dealing with Driver generated result sets and other catalog type functions.
*/    
void DATAResultSetWidget::doResultGUIGridBody( SWORD nColumns )
{
    SQLUSMALLINT    nCol                = 0;
    QVariant        v;

    // PROCESS ALL ROWS
    // we should be sure that we will not empty result set
    // othervise it will break SQLRowCount
    for( ; SQL_SUCCEEDED( pStatement->doFetch() ); )
    {
        // SOME DRIVERS DO NOT RETURN THE ROW COUNT PROPERLY SO EXPAND IF NEED BE 
        if ( ptableResults->rowCount() < nRows+1 )
            ptableResults->setRowCount( nRows+1 );

        // PROCESS ALL COLUMNS
        for( nCol = 0; nCol < nColumns; nCol++ )
        {
            v = pStatement->getData( nCol+1 );

/*
            QString resultPrintable = "";                                      
            QString resultUnprintable = "";                                    
            QString result;                                                    
            bool isResultPrintable = true;                                     
                                                                               
            for(unsigned int i=0; szColumn[i] && i < sizeof(szColumn) - 1; i++)
            {                                                                  
                QString s;                                                     
                                                                               
                resultPrintable += szColumn[i];                                
                                                                               
                if(!isprint(szColumn[i]))                                      
                    isResultPrintable = false;                                 
                s.sprintf("\\%02d",(unsigned char)szColumn[i]);                
                resultUnprintable += s;                                        
            }                                                                  
                                                                               
            if(isResultPrintable)                                              
                result = resultPrintable;                                      
            else                                                               
                result = resultUnprintable;                                    
*/
            
            ptableResults->setItem( nRows, nCol, new QTableWidgetItem( v.toString() ) );
        }
        nRows++;
        if ( nLimit > 0 && nRows > nLimit )
        {
            emit signalMessage( "WARNING", "Results truncated." );
            break;
        }
    }
}

/*!
    doResultHTMLGrid

    Display the  results formatted in a HTML table.
    
    NOTE: Do NOT rely upon SQLRowCount() as it often returns a misleading -1. For example
          when dealing with Driver generated result sets and other catalog type functions.
*/    
void DATAResultSetWidget::doResultHTMLGrid()
{
    QString         stringHTML = "<TABLE border>\n";
    SWORD           nColumns;
    SQLUSMALLINT    nCol;
    int             nProgress       = 0;
    int             nProgressMax    = 1000;

    if ( nLimit > 0 )
        nProgressMax = nLimit;

    QProgressDialog progress( "Formatting results...", "Cancel", 0, nProgressMax, this );
    progress.setModal( true );
    progress.setAutoClose( false );

    // GET NUMBER OF COLUMNS RETURNED
    nColumns = pStatement->getColumnCount();

    stringHTML += "\t<TR>\n";
    QVariant v;
    for( nCol = 0; nCol < nColumns; nCol++ )
    {
        v = pStatement->getColAttribute( nCol+1, SQL_DESC_LABEL );
        stringHTML += "\t\t<TD><B>";

        {
            QString stringLabel = v.toString();
            stringLabel = stringLabel.replace( '\u0000', ' ' );
            stringLabel = stringLabel.trimmed();
            stringHTML += stringLabel;
        }

        stringHTML += "</B></TD>";
    }
    stringHTML += "\n\t</TR>\n";

    // BODY
    if ( nColumns > 0 )
    {
        // PROCESS ALL ROWS
        // we should be sure that we will not empty result set
        // othervise it will break SQLRowCount
        for( ; SQL_SUCCEEDED( pStatement->doFetch() ); )
        {
            nRows++;
            if ( nLimit > 0 && nRows > nLimit )
            {
                emit signalMessage( "WARNING", "Results truncated." );
                break;
            }
            stringHTML += "\t<TR>\n";
    
            // cols
            for( nCol = 1; nCol <= nColumns; nCol++ )
            {
                stringHTML += "<TD>";

                v = pStatement->getData( nCol );

                stringHTML += v.toString();
                stringHTML += "</TD>";
    
            }
            stringHTML += "\n\t</TR>\n";

            // update progress
            nProgress++;
            if ( nProgress >= nProgressMax )
            {
                nProgress = 0;
                progress.reset();
            }
            progress.setValue( nProgress );

            qApp->processEvents();
            if ( progress.wasCanceled() )
                break;
        }
    }

    // DONE
    stringHTML += "</TABLE>\n";
    ptextbrowserResults->setText( stringHTML );
}

/*!
    doResultTextDelimited

    Display the results formatted as delimited columns.
    
    NOTE: Do NOT rely upon SQLRowCount() as it often returns a misleading -1. For example
          when dealing with Driver generated result sets and other catalog type functions.
*/    
void DATAResultSetWidget::doResultTextDelimited()
{
    QString         stringResult    = "";
    SWORD           nColumns;
    SQLUSMALLINT    nCol;
    QVariant        v;

    // GET NUMBER OF COLUMNS RETURNED
    nColumns = pStatement->getColumnCount();

    // HEADER
    if ( this->bFirstRowColumnNames )
    {
        for( nCol = 0; nCol < nColumns; nCol++ )
        {
            if ( nCol ) stringResult += stringDelimitChar;
            v = pStatement->getColAttribute( nCol+1, SQL_DESC_LABEL );

            {
                QString stringLabel = v.toString();
                stringLabel = stringLabel.replace( '\u0000', ' ' );
                stringLabel = stringLabel.trimmed();
                stringResult += stringLabel;
            }
        }
        stringResult += "\n";
    }

    // BODY
    if ( nColumns > 0 )
    {
        // PROCESS ALL ROWS
        // we should be sure that we will not empty result set
        // othervise it will break SQLRowCount
        for( ; SQL_SUCCEEDED( pStatement->doFetch() ); )
        {
            nRows++;
            if ( nLimit > 0 && nRows > nLimit )
            {
                emit signalMessage( "WARNING", "Results truncated." );
                break;
            }
            if ( nRows > 1 ) stringResult += "\n";
    
            // cols
            for( nCol = 1; nCol <= nColumns; nCol++ )
            {
                if ( nCol > 1 ) stringResult += stringDelimitChar;
    
                v = pStatement->getData( nCol );
                stringResult += v.toString();
            }
        }
    }

    // DONE
    ptexteditResults->setText( stringResult );
}

void DATAResultSetWidget::doLoadState()
{
    QSettings settings;
    QString stringPath( "/CBDSQLResultArea/Defaults/" );

    setFirstRowColumnNames( settings.value( stringPath + "ColumnNames", getFirstRowColumnNames() ).toBool() );
    setDelimitChar( settings.value( stringPath + "Delimiter", getDelimitChar() ).toString() );
    setLimit( settings.value( stringPath + "Limit", getLimit() ).toInt() );
    setResultFormat( (ResultFormatTypes)settings.value( stringPath + "Format", getResultFormat() ).toInt() );
}

void DATAResultSetWidget::doSaveState()
{
    // DO NOT replace the defaults here - it should only be done through a preferences dialog
}



