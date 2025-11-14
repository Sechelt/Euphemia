#include "MainWindow.h"

#include <QOpenGLWidget>

MainWindow::MainWindow() 
    : QMainWindow()
{
    QToolBar *pToolBar = addToolBar( tr("Widgets") );
    pToolBar->setObjectName( "Widgets" );

    QComboBox *p = new QComboBox( pToolBar );
    p->addItem( "" );
    p->addItem( "QCalendarWidget" );
    p->addItem( "QCheckBox" );
    p->addItem( "QComboBox" );
    p->addItem( "QDateEdit" );
    p->addItem( "QDateTimeEdit" );
    p->addItem( "QDialogButtonBox" );
    p->addItem( "QDoubleSpinBox" );
    p->addItem( "QFrame" );
    p->addItem( "QGroupBox" );
    p->addItem( "QLabel" );
    p->addItem( "QLCDNumber" );
    p->addItem( "QLineEdit" );
    p->addItem( "QListWidget" );
    p->addItem( "QOpenGLWidget" );
    p->addItem( "QPlainTextEdit" );
    p->addItem( "QProgressBar" );
    p->addItem( "QPushButton" );
    p->addItem( "QRadioButton" );
    p->addItem( "QScrollArea" );
    p->addItem( "QScrollBar (H)" );
    p->addItem( "QScrollBar (V)" );
    p->addItem( "QSpinBox" );
    p->addItem( "QSplitter (H)" );
    p->addItem( "QSplitter (V)" );
    p->addItem( "QSlider (H)" );
    p->addItem( "QSlider (V)" );
    p->addItem( "QStackedWidget" );
    p->addItem( "QTableWidget" );
    p->addItem( "QTabWidget" );
    p->addItem( "QTextBrowser" );
    p->addItem( "QTextEdit" );
    p->addItem( "QTimeEdit" );
    p->addItem( "QToolBox" );
    p->addItem( "QToolButton" );
    p->addItem( "QTreeWidget" );
    p->addItem( "QDial" );
    connect( p, SIGNAL(activated(int)), SLOT(slotWidget(int)) );

    pToolBar->addWidget( p );
}

QWidget *MainWindow::getCalendarWidget()
{
    QWidget *p = new QCalendarWidget( this );
    return p;
}

QWidget *MainWindow::getCheckBox()
{
    QWidget *p = new QWidget( this );
    QVBoxLayout *pLayout = new QVBoxLayout( p );
    QCheckBox *pCheckBox = new QCheckBox( "Click To Agree", this );
    pLayout->addWidget( pCheckBox );
    pLayout->addStretch( 10 );
    return p;
}

QWidget *MainWindow::getComboBox()
{
    QWidget *p = new QWidget( this );
    QVBoxLayout *pLayout = new QVBoxLayout( p );
    QComboBox *pComboBox = new QComboBox( p );
    pComboBox->addItem( "Sparkling" );
    pComboBox->addItem( "Mineral" );
    pComboBox->addItem( "Purified" );
    pComboBox->addItem( "Distilled" );
    pLayout->addWidget( pComboBox );
    pLayout->addStretch( 10 );
    return p;
}

QWidget *MainWindow::getDateEdit()
{
    QWidget *p = new QWidget( this );
    QVBoxLayout *pLayout = new QVBoxLayout( p );
    QWidget *pDateEdit = new QDateEdit( this );
    pLayout->addWidget( pDateEdit );
    pLayout->addStretch( 10 );
    return p;
}

QWidget *MainWindow::getDateTimeEdit()
{
    QWidget *p = new QWidget( this );
    QVBoxLayout *pLayout = new QVBoxLayout( p );
    QDateTimeEdit *pDateTimeEdit = new QDateTimeEdit( this );
    pDateTimeEdit->setDateTime( QDateTime( QDate( 2025, 4, 4 ), QTime( 10, 30 ) ) );
    pLayout->addWidget( pDateTimeEdit );
    pLayout->addStretch( 10 );
    return p;
}

QWidget *MainWindow::getDial()
{
    QDial *p = new QDial( this );
    p->setRange( 0, 100 );
    p->setValue( 47 );
    return p;
}

QWidget *MainWindow::getDialogButtonBox()
{
    QWidget *p = new QWidget( this );
    QVBoxLayout *pLayout = new QVBoxLayout( p );
    QWidget *pDialogButtonBox = new QDialogButtonBox( this );
    pLayout->addWidget( pDialogButtonBox );
    pLayout->addStretch( 10 );
    return p;
}

QWidget *MainWindow::getDoubleSpinBox()
{
    QWidget *p = new QWidget( this );
    QVBoxLayout *pLayout = new QVBoxLayout( p );
    QWidget *pDoubleSpinBox = new QDoubleSpinBox( this );
    pLayout->addWidget( pDoubleSpinBox );
    pLayout->addStretch( 10 );
    return p;
}

QWidget *MainWindow::getFrame()
{
//    QWidget *p = new QWidget( this );
//    QVBoxLayout *pLayout = new QVBoxLayout( p );
    QFrame *pFrame = new QFrame( this );
//    pLayout->addWidget( pFrame );
    pFrame->setFrameShape( QFrame::Panel );
    return pFrame;
}

QWidget *MainWindow::getGroupBox()
{
    QWidget *p = new QGroupBox( this );
    return p;
}

QWidget *MainWindow::getLabel()
{
    QWidget *p = new QWidget( this );
    QVBoxLayout *pLayout = new QVBoxLayout( p );
    QWidget *pLabel = new QLabel( "Product Name:", this );
    pLayout->addWidget( pLabel );
    pLayout->addStretch( 10 );
    return p;
}

QWidget *MainWindow::getLCDNumber()
{
    QWidget *p = new QWidget( this );
    QVBoxLayout *pLayout = new QVBoxLayout( p );
    QLCDNumber *pLCDNumber = new QLCDNumber( 4, this );
    pLCDNumber->display( 1030 );
    pLayout->addWidget( pLCDNumber );
    pLayout->addStretch( 10 );
    return p;
}

QWidget *MainWindow::getLineEdit()
{
    QWidget *p = new QWidget( this );
    QVBoxLayout *pLayout = new QVBoxLayout( p );
    QWidget *pLineEdit = new QLineEdit( this );
    pLayout->addWidget( pLineEdit );
    pLayout->addStretch( 10 );
    return p;
}

QWidget *MainWindow::getListWidget()
{
    QListWidget *p = new QListWidget( this );
    p->addItem( "Earth" );
    p->addItem( "Wind" );
    p->addItem( "Fire" );
    p->addItem( "Earth" );
    p->addItem( "Wind" );
    p->addItem( "Fire" );
    return p;
}

QWidget *MainWindow::getOpenGLWidget()
{
    QWidget *p = new QOpenGLWidget( this );
    return p;
}

QWidget *MainWindow::getPlainTextEdit()
{
    QWidget *p = new QPlainTextEdit( this );
    return p;
}

QWidget *MainWindow::getProgressBar()
{
    QWidget *p = new QWidget( this );
    QVBoxLayout *pLayout = new QVBoxLayout( p );
    QProgressBar *pProgressBar = new QProgressBar( this );
    pProgressBar->setRange( 0, 1000 );
    pProgressBar->setValue( 470 );
    pLayout->addWidget( pProgressBar );
    pLayout->addStretch( 10 );
    return p;
}

QWidget *MainWindow::getPushButton()
{
    QWidget *p = new QWidget( this );
    QVBoxLayout *pLayout = new QVBoxLayout( p );
    QWidget *pPushButton = new QPushButton( "Push", this );
    pLayout->addWidget( pPushButton );
    pLayout->addStretch( 10 );
    return p;
}

QWidget *MainWindow::getRadioButton()
{
    QWidget *p = new QWidget( this );
    QVBoxLayout *pLayout = new QVBoxLayout( p );
    QWidget *pRadioButton = new QRadioButton( "Earth", this );
    pLayout->addWidget( pRadioButton );
    pLayout->addStretch( 10 );
    return p;
}

QWidget *MainWindow::getScrollArea()
{
    QWidget *p = new QScrollArea( this );
    return p;
}

QWidget *MainWindow::getScrollBar( Qt::Orientation n )
{
    QWidget *p = new QWidget( this );
    QVBoxLayout *pLayout = new QVBoxLayout( p );
    QScrollBar *pScrollBar = new QScrollBar( n, this );
    pLayout->addWidget( pScrollBar );
    pLayout->addStretch( 10 );
    return p;
}

QWidget *MainWindow::getSpinBox()
{
    QWidget *p = new QWidget( this );
    QVBoxLayout *pLayout = new QVBoxLayout( p );
    QSpinBox *pSpinBox = new QSpinBox( this );
    pSpinBox->setRange( 0, 1000 );
    pSpinBox->setValue( 470 );
    pLayout->addWidget( pSpinBox );
    pLayout->addStretch( 10 );
    return p;
}

QWidget *MainWindow::getSplitter( Qt::Orientation n )
{
    QWidget *p = new QSplitter( n, this );
    return p;
}

QWidget *MainWindow::getSlider( Qt::Orientation n )
{
    QWidget *p = new QWidget( this );
    QBoxLayout *pLayout;
    if ( n == Qt::Horizontal ) pLayout = new QVBoxLayout( p );
    else pLayout = new QHBoxLayout( p );
    QSlider *pSlider = new QSlider( n, this );
    pSlider->setRange( 0, 1000 );
    pSlider->setValue( 470 );
    pLayout->addWidget( pSlider );
    pLayout->addStretch( 10 );
    return p;
}

QWidget *MainWindow::getStackedWidget()
{
    QWidget *p = new QStackedWidget( this );
    return p;
}

QWidget *MainWindow::getTableWidget()
{
    QTableWidget *p = new QTableWidget( 2, 2, this );
    p->setHorizontalHeaderItem( 0, new QTableWidgetItem( "Product" ) );
    p->setHorizontalHeaderItem( 1, new QTableWidgetItem( "Cost" ) );
    p->setItem( 0, 0, new QTableWidgetItem( "Mineral" ) );
    p->setItem( 0, 1, new QTableWidgetItem( "5.00" ) );
    p->setItem( 1, 0, new QTableWidgetItem( "Sparkling" ) );
    p->setItem( 1, 1, new QTableWidgetItem( "10.00" ) );
    return p;
}

QWidget *MainWindow::getTabWidget()
{
    QTabWidget *p = new QTabWidget( this );
    p->addTab( new QWidget( p ), "Mineral" );
    p->addTab( new QWidget( p ), "Sparkling" );
    return p;
}

QWidget *MainWindow::getTextBrowser()
{
    QTextBrowser *p = new QTextBrowser( this );
    p->setHtml( "Browse <B>formatted</B> text." );
    return p;
}

QWidget *MainWindow::getTextEdit()
{
    QTextEdit *p = new QTextEdit( this );
    p->setHtml( "Edit <B>formatted</B> text." );
    return p;
}

QWidget *MainWindow::getTimeEdit()
{
    QWidget *p = new QWidget( this );
    QVBoxLayout *pLayout = new QVBoxLayout( p );
    QTimeEdit *pTimeEdit = new QTimeEdit( this );
    pTimeEdit->setTime( QTime( 10, 30 ) );
    pLayout->addWidget( pTimeEdit );
    pLayout->addStretch( 10 );
    return p;
}

QWidget *MainWindow::getToolBox()
{
    QToolBox *p = new QToolBox( this );
    return p;
}

QWidget *MainWindow::getToolButton()
{
    QWidget *p = new QWidget( this );
    QVBoxLayout *pLayout = new QVBoxLayout( p );
    QToolButton *pToolButton = new QToolButton( this );
    pLayout->addWidget( pToolButton );
    pLayout->addStretch( 10 );
    return p;
}

QWidget *MainWindow::getTreeWidget()
{
    QStringList listStrings;
    QTreeWidgetItem *pItem;

    QTreeWidget *p = new QTreeWidget( this );
    listStrings << "Type" << "Brand";
    p->setHeaderLabels( listStrings );

    listStrings.clear(); listStrings << "Sparkling" << "";
    pItem = new QTreeWidgetItem( p, listStrings );
    listStrings.clear(); listStrings << "" << "Waterloo";
    new QTreeWidgetItem( pItem, listStrings );
    p->insertTopLevelItem( 0, pItem );

    return p;
}

void MainWindow::slotWidget( int nIndex )
{
    if ( pWidget )
    {
        delete pWidget;
        pWidget = nullptr;
    }

    if ( nIndex == 0 )
        return;
    else if ( nIndex == 1 )
        pWidget = getCalendarWidget();
    else if ( nIndex == 2 )
        pWidget = getCheckBox();
    else if ( nIndex == 3 )
        pWidget = getComboBox();
    else if ( nIndex == 4 )
        pWidget = getDateEdit();
    else if ( nIndex == 5 )
        pWidget = getDateTimeEdit();
    else if ( nIndex == 6 )
        pWidget = getDialogButtonBox();
    else if ( nIndex == 7 )
        pWidget = getDoubleSpinBox();
    else if ( nIndex == 8 )
        pWidget = getFrame();
    else if ( nIndex == 9 )
        pWidget = getGroupBox();
    else if ( nIndex == 10 )
        pWidget = getLabel();
    else if ( nIndex == 11 )
        pWidget = getLCDNumber();
    else if ( nIndex == 12 )
        pWidget = getLineEdit();
    else if ( nIndex == 13 )
        pWidget = getListWidget();
    else if ( nIndex == 14 )
        pWidget = getOpenGLWidget();
    else if ( nIndex == 15 )
        pWidget = getPlainTextEdit();
    else if ( nIndex == 16 )
        pWidget = getProgressBar();
    else if ( nIndex == 17 )
        pWidget = getPushButton();
    else if ( nIndex == 18 )
        pWidget = getRadioButton();
    else if ( nIndex == 19 )
        pWidget = getScrollArea();
    else if ( nIndex == 20 )
        pWidget = getScrollBar( Qt::Vertical );
    else if ( nIndex == 21 )
        pWidget = getScrollBar( Qt::Horizontal );
    else if ( nIndex == 22 )
        pWidget = getSpinBox();
    else if ( nIndex == 23 )
        pWidget = getSplitter( Qt::Vertical );
    else if ( nIndex == 24 )
        pWidget = getSplitter( Qt::Horizontal );
    else if ( nIndex == 25 )
        pWidget = getSlider( Qt::Vertical );
    else if ( nIndex == 26 )
        pWidget = getSlider( Qt::Horizontal );
    else if ( nIndex == 27 )
        pWidget = getStackedWidget();
    else if ( nIndex == 28 )
        pWidget = getTableWidget();
    else if ( nIndex == 29 )
        pWidget = getTabWidget();
    else if ( nIndex == 30 )
        pWidget = getTextBrowser();
    else if ( nIndex == 31 )
        pWidget = getTextEdit();
    else if ( nIndex == 32 )
        pWidget = getTimeEdit();
    else if ( nIndex == 33 )
        pWidget = getToolBox();
    else if ( nIndex == 34 )
        pWidget = getToolButton();
    else if ( nIndex == 35 )
        pWidget = getTreeWidget();
    else if ( nIndex == 36 )
        pWidget = getDial();
    else
    {
        qInfo() << "Selection not handled " << nIndex; 
    }

    if ( pWidget ) setCentralWidget( pWidget );

}

