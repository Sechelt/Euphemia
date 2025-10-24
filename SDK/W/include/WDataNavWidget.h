#ifndef H_WDataNavWidget
#define H_WDataNavWidget

#include <QtWidgets>

class WDataNavWidget : public QWidget
{
    Q_OBJECT
public:
    WDataNavWidget( QWidget *pParent );
    ~WDataNavWidget();

    void setRecord( int n );            // current items row number
    void setRecords( int n );
//    void setSelection( int nRow1, int nCol1, int nRow2, int nCol2 );
    void setFirst( bool b )         { pFirst->setEnabled( b ); }
    void setPrevious( bool b )      { pPrevious->setEnabled( b ); }
    void setNext( bool b )          { pNext->setEnabled( b ); }
    void setLast( bool b )          { pLast->setEnabled( b ); }
    void setNew( bool b )           { pNew->setEnabled( b ); }
    void setDelete( bool b )        { pDelete->setEnabled( b ); }

signals:
    void signalFirst();
    void signalPrevious();
    void signalNew();
    void signalNext();
    void signalLast();
    void signalDelete();

public slots:
    void slotRecords( int n ) { setRecords( n ); }

protected:
    int nRecord = 0;
    int nRecords = 0;
    QToolButton *   pFirst;
    QToolButton *   pPrevious;
    QToolButton *   pNext;
    QToolButton *   pLast;
    QToolButton *   pNew;
    QToolButton *   pDelete;

    QLineEdit *     pRecord;
//    QLineEdit *     pSelection;

    void setRecord();
};

#endif

