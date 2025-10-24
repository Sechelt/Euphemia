#ifndef H_MainWindow
#define H_MainWindow

#include <CBD.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();

protected:
    QWidget *pWidget = nullptr;

    QWidget *getCalendarWidget();
    QWidget *getCheckBox();
    QWidget *getComboBox();
    QWidget *getDateEdit();
    QWidget *getDateTimeEdit();
    QWidget *getDialogButtonBox();
    QWidget *getDial();
    QWidget *getDoubleSpinBox();
    QWidget *getFrame();
    QWidget *getGroupBox();
    QWidget *getLabel();
    QWidget *getLCDNumber();
    QWidget *getLineEdit();
    QWidget *getListWidget();
    QWidget *getOpenGLWidget();
    QWidget *getPlainTextEdit();
    QWidget *getProgressBar();
    QWidget *getPushButton();
    QWidget *getRadioButton();
    QWidget *getScrollArea();
    QWidget *getScrollBar( Qt::Orientation n );
    QWidget *getSpinBox();
    QWidget *getSplitter( Qt::Orientation n );
    QWidget *getSlider( Qt::Orientation n );
    QWidget *getStackedWidget();
    QWidget *getTableWidget();
    QWidget *getTabWidget();
    QWidget *getTextBrowser();
    QWidget *getTextEdit();
    QWidget *getTimeEdit();
    QWidget *getToolBox();
    QWidget *getToolButton();
    QWidget *getTreeWidget();

protected slots:
    void slotWidget( int nIndex );
};

#endif



