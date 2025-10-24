#ifndef H_AWOutputSearchWidget
#define H_AWOutputSearchWidget

#include <CBD.h>

#include <ADObjectPath.h>
#include <ADObject.h>

class AWOutputSearchWidget : public QWidget
{
    Q_OBJECT
public:
    AWOutputSearchWidget( QWidget *pParent );
    ~AWOutputSearchWidget();

    void setFocusFind();
    void setFocusReplace();

signals:
    void signalGoTo( ADObjectPathList listObjectPathItems );
    void signalGoToView( ADObjectPathList listObjectPathItems );
    void signalFind( const ADFindCriteria &Criteria );

public slots:
    void slotMessage( ADEventSearch *p );
    void slotClear();

protected:
    QLineEdit *pLineEdit;
    QPushButton *pButton;
    QTableWidget * pTableWidget;
    QVector<ADObjectPathList> vectorPaths;

    QCheckBox *pName;
    QCheckBox *pCode;
    QCheckBox *pComment;
    QCheckBox *pDynamicData;
    QCheckBox *pOther;
    QCheckBox *pCaseSensitive;

    virtual void doSaveState();
    virtual void doLoadState();

protected slots:
    void slotFind();
    void slotClicked( int nRow, int nColumn );
    void slotDoubleClicked( int nRow, int nColumn );
};

#endif

