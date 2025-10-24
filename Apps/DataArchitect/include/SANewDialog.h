#ifndef H_SANewDialog
#define H_SANewDialog

#include <WIconWidget.h>

class SANewWidget : public QWidget
{
    Q_OBJECT
public:
    SANewWidget( QWidget *pParent, bool bPhysicalModels = false );

    QString getSelected();

protected:
    QVector<WIconWidget*> vectorIcons;

protected slots:
    void slotClicked( WIconWidget * );
};

class SANewDialog : public QDialog
{
    Q_OBJECT
public:
    SANewDialog( QWidget *pParent, bool bPhysicalModels = false );

    QString getSelected() { return pNewWidget->getSelected(); }

protected:
    SANewWidget *pNewWidget;

    void doLoadState();
    void doSaveState();

protected slots:
    void slotOk();          // save changes
    void slotRejected();    // loose changes
};


#endif

