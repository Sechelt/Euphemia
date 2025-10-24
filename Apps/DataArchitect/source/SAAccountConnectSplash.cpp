#include "AppInfo.h"
#include "SAAccountConnectSplash.h"

#include <WDirSelectWidget.h>
#include <WLabelButton.h>

SAAccountConnectSplash::SAAccountConnectSplash( QWidget *pParent, const QString &stringURL )
: QDialog( pParent, Qt::Dialog | Qt::FramelessWindowHint )
{
    this->stringURL = stringURL;

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );

    {
        QHBoxLayout *pLayout = new QHBoxLayout();
        pLayout->addWidget( new QLabel( tr("Account"), this ) );
        pLayout->addStretch( 10 );

        WLabelButton *pButton = new WLabelButton( this );
        pButton->setFixedSize( 16, 16 );
        pButton->setPixmap( QPixmap( ":W/Cancel" ).scaled( 16, 16 ) );
        connect( pButton, SIGNAL(signalClicked()), SLOT(slotCancel()) );
        pLayout->addWidget( pButton );

        pLayoutTop->addLayout( pLayout );
    }

    pStack = new QStackedWidget( this );
    pLayoutTop->addWidget( pStack );

    pIntro = new SAAccountIntroPage; 
    pStack->addWidget( pIntro );
    connect( pIntro, SIGNAL(signalNext(SAAccountConnectSplash::Pages)), SLOT(slotNext(SAAccountConnectSplash::Pages)) );

    pRemote = new SAAccountRemotePage; 
    pStack->addWidget( pRemote );
    connect( pRemote, SIGNAL(signalNext(SAAccountConnectSplash::Pages)), SLOT(slotNext(SAAccountConnectSplash::Pages)) );

    pLocal = new SAAccountLocalPage; 
    pStack->addWidget( pLocal );
    connect( pLocal, SIGNAL(signalNext(SAAccountConnectSplash::Pages)), SLOT(slotNext(SAAccountConnectSplash::Pages)) );

    pODBC = new SAAccountODBCPage; 
    pStack->addWidget( pODBC );
    connect( pODBC, SIGNAL(signalNext(SAAccountConnectSplash::Pages)), SLOT(slotNext(SAAccountConnectSplash::Pages)) );
}

void SAAccountConnectSplash::slotNext( SAAccountConnectSplash::Pages n )
{
    switch( n )
    {
        case PageIntro:
            pStack->setCurrentWidget( pIntro );
            break;
        case PageLocal:
            pStack->setCurrentWidget( pLocal );
            break;
        case PageODBC:
            pStack->setCurrentWidget( pODBC );
            break;
        case PageRemote:
            pStack->setCurrentWidget( pRemote );
            break;
        case ResultAccept:
            accept();
            break;
        case ResultCancel:
            reject();
            break;
    }
}

void SAAccountConnectSplash::slotCancel()
{
    reject();
}

SAAccountPage::SAAccountPage( QWidget *pParent )
    : QWidget( pParent )
{
}

SAAccountIntroPage::SAAccountIntroPage( QWidget *pParent )
  : SAAccountPage( pParent )
{
    QVBoxLayout *pLayoutContent = new QVBoxLayout();

    {
        QVBoxLayout *pLayoutFields = new QVBoxLayout();

        pButtonLocal    = new QRadioButton( tr("Local (directory)"));
        pButtonODBC     = new QRadioButton( tr("ODBC (Driver or Data Source Name)"));
        pButtonRemote   = new QRadioButton( tr("Cloud (HTTPS)"));
        pButtonLocal->setChecked( true );

        pLayoutFields->addStretch( 10 );
        pLayoutFields->addWidget( pButtonLocal );
        pLayoutFields->addWidget( pButtonODBC );
        pLayoutFields->addWidget( pButtonRemote );
        pLayoutFields->addStretch( 10 );

        QGroupBox *pGroupBox = new QGroupBox();
        pGroupBox->setLayout( pLayoutFields );
        pLayoutContent->addWidget( pGroupBox );
    }

    {
        QHBoxLayout *pLayout = new QHBoxLayout();
        QPushButton *pNext = new QPushButton( tr("Next"), this );
        pLayout->addStretch( 10 );
        pLayout->addWidget( pNext );
        pLayout->addStretch( 10 );
        connect( pNext, SIGNAL(clicked()), SLOT(slotNext()) );

        pLayoutContent->addLayout( pLayout );
    }

    {
        QHBoxLayout *pLayoutTop = new QHBoxLayout( this );
        pLayoutTop->addStrut( 50 );
        pLayoutTop->addLayout( pLayoutContent );
        pLayoutTop->addStrut( 50 );
    }
}

void SAAccountIntroPage::slotNext()
{
    if( pButtonLocal->isChecked() ) emit signalNext( SAAccountConnectSplash::PageLocal );
    if( pButtonODBC->isChecked() ) emit signalNext( SAAccountConnectSplash::PageODBC );
    if( pButtonRemote->isChecked() ) emit signalNext( SAAccountConnectSplash::PageRemote );
}

SAAccountRemotePage::SAAccountRemotePage( QWidget *pParent )
: SAAccountPage( pParent )
{
    QVBoxLayout *pLayoutContent = new QVBoxLayout();

    {
        QVBoxLayout *pLayoutFields = new QVBoxLayout();
        pUID = new QLineEdit( tr("email address"), this );
        pPWD = new QLineEdit( tr("password"), this );

        pLayoutFields->addWidget( pUID );
        pLayoutFields->addWidget( pPWD );

        {
            WLabelButton *pButton = new WLabelButton( this );
            pButton->setText( tr("forgot your password?") );
            QFont font = pButton->font();
            font.setPointSize( int(font.pointSize() * 0.75) );
            font.setUnderline( true );
            pButton->setFont( font );  
            pButton->setAlignment( Qt::AlignHCenter );
            pLayoutFields->addWidget( pButton );
        }

        pLayoutContent->addLayout( pLayoutFields );
    }

    {
        QHBoxLayout *pLayout = new QHBoxLayout();
        QPushButton *pNext = new QPushButton( tr("Login"), this );
        pLayout->addStretch( 10 );
        pLayout->addWidget( pNext );
        pLayout->addStretch( 10 );
        connect( pNext, SIGNAL(clicked()), SLOT(slotNext()) );
        pLayoutContent->addSpacing( 50 );
        pLayoutContent->addLayout( pLayout );
    }

    {
        WLabelButton *pButton = new WLabelButton( this );
        pButton->setText( tr("create an account?") );
        QFont font = pButton->font();
        font.setPointSize( int(font.pointSize() * 0.75) );
        font.setUnderline( true );
        pButton->setFont( font );  
        pButton->setAlignment( Qt::AlignHCenter );
        pLayoutContent->addWidget( pButton );
    }

    {
        QHBoxLayout *pLayoutTop = new QHBoxLayout( this );
        pLayoutTop->addStrut( 50 );
        pLayoutTop->addLayout( pLayoutContent );
        pLayoutTop->addStrut( 50 );
    }
}

void SAAccountRemotePage::slotNext()
{
    emit signalNext( SAAccountConnectSplash::ResultAccept );
}

SAAccountLocalPage::SAAccountLocalPage( QWidget *pParent )
: SAAccountPage( pParent )
{
    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );

    // dir
    {
        QHBoxLayout *pLayout = new QHBoxLayout();
        pURL = new WDirSelectWidget( tr("local directory"), this );
        // pLayout->addStretch( 10 );
        pLayout->addWidget( pURL );
        // pLayout->addStretch( 10 );

        pLayoutTop->addLayout( pLayout );
    }

    // button
    {
        QHBoxLayout *pLayout = new QHBoxLayout();
        QPushButton *pNext = new QPushButton( tr("Next"), this );
        pLayout->addStretch( 10 );
        pLayout->addWidget( pNext );
        pLayout->addStretch( 10 );
        connect( pNext, SIGNAL(clicked()), SLOT(slotNext()) );

        pLayoutTop->addLayout( pLayout );
    }
}

void SAAccountLocalPage::slotNext()
{
    emit signalNext( SAAccountConnectSplash::ResultAccept );
}

SAAccountODBCPage::SAAccountODBCPage( QWidget *pParent )
: SAAccountPage( pParent )
{
}

void SAAccountODBCPage::slotNext()
{
    emit signalNext( SAAccountConnectSplash::ResultAccept );
}


