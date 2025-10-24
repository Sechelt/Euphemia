#include <ADValidation.h>
#include <ADObjectPath.h>
#include <ADObject.h>

class AWOutputValidationWidget : public QTableWidget
{
    Q_OBJECT
public:
    AWOutputValidationWidget( QWidget *pParent );
    ~AWOutputValidationWidget();

signals:
    void signalGoTo( ADObjectPathList listObjectPathItems );
    void signalGoToView( ADObjectPathList listObjectPathItems );

public slots:
    void slotMessage( ADEventValidation *p );
    void slotClear();

protected:
    QVector<ADObjectPathList> vectorPaths;

    virtual void doSaveState();
    virtual void doLoadState();

protected slots:
    void slotClicked( int nRow, int nColumn );
    void slotDoubleClicked( int nRow, int nColumn );
};


