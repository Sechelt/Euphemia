#ifndef H_DATAResultSetWidget
#define H_DATAResultSetWidget

#include <QtWidgets>
#include <DATA.h>

class DATAStatement;

/*!
 * \brief Displays a result set (read-only).
 *  
 * Formats supported are; 
 *  
 *      - HTML Table
 *      - Text Boxed
 *      - Text Delimited
 *      - GUI Table
 *  
 * \author pharvey (6/16/20)
 */
class DATAResultSetWidget : public QWidget
{
    Q_OBJECT
public:
    enum ResultFormatTypes
    {
        ResultFormatHTMLGrid,
        ResultFormatTextGrid,
        ResultFormatTextDelimited,
        ResultFormatGUIGrid
    };

    DATAResultSetWidget( QWidget *pParent );
    ~DATAResultSetWidget();

    // SETTERS
    virtual void setResultFormat( ResultFormatTypes n );
    virtual void setLimit( int n ) { nLimit = n; }
    virtual void setFirstRowColumnNames( bool b ) { bFirstRowColumnNames = b; }
    virtual void setDelimitChar( const QString &string ) { stringDelimitChar = string; }

    // GETTERS
    virtual uint    getRows() { return nRows; }
    virtual int     getResultFormat();
    virtual QString getResult();
    virtual int     getLimit() { return nLimit; }
    virtual bool    getFirstRowColumnNames() { return bFirstRowColumnNames; }
    virtual QString getDelimitChar() { return stringDelimitChar; }

    virtual void doResult( DATAStatement *pStatement );
    virtual void doClear();
    virtual bool doSave( const QString &stringFileName );

signals:
    void signalMessage( const QString &stringSeverity, const QString &stringMessage );

public slots:
    virtual void slotClear();

protected:
    DATAStatement *         pStatement;                 // set in slotResult for convenience of other methods
    QTableWidget *          ptableResults;              // GRID RESULT
    QTextBrowser *          ptextbrowserResults;        // HTML RESULT
    QTextEdit *             ptexteditResults;           // TEXT RESULT
    QString                 stringFileName;
    ResultFormatTypes       nResultFormat;
    int                     nRows;
    int                     nLimit;
    bool                    bFirstRowColumnNames;
    QString                 stringDelimitChar;

    void resizeEvent( QResizeEvent * );

    virtual void doResultTextGrid();
    virtual void doResultTextGridHeader( SWORD nColumns, QString *pstringHorizSep );
    virtual void doResultTextGridBody( SWORD nColumns );

    virtual void doResultGUIGrid();
    virtual void doResultGUIGridHeader( SWORD nColumns );
    virtual void doResultGUIGridBody( SWORD nColumns );

    virtual void doResultHTMLGrid();

    virtual void doResultTextDelimited();

    virtual void doSaveState();
    virtual void doLoadState();
};

#endif


