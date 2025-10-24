#ifndef H_DATAWTableView
#define H_DATAWTableView

#include <QtWidgets>

class DATAWTableView : public QTableView
{
    Q_OBJECT
public:
    DATAWTableView( QWidget *pParent = nullptr );

protected:
    int     nIndex = -1;        // index of clicked column header
    QPoint  point;              // global point to place column header popup

    virtual void keyPressEvent( QKeyEvent *pEvent ) override;

protected slots:
    void slotHeaderSectionClicked( int nIndex );
    void slotFilter();
};

#endif


