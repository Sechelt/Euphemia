#include "LibInfo.h"
#include "AWDocumentNameWidget.h"

AWDocumentNameWidget::AWDocumentNameWidget( ADDocument *pDocument, QWidget *pwidgetParent, const QString &stringFileName )
: QWidget( pwidgetParent )
{
    this->pDocument = pDocument;

    QFormLayout *pLayout  = new QFormLayout( this );

    {
        QHBoxLayout *pHLayout = new QHBoxLayout();

        // OID
        pHLayout->addWidget( new QLabel( tr("OID"), this ) );
        pHLayout->addWidget( new QLabel( QString::number( pDocument->getObject()->getOID() ), this ) );

        // NAME
        plineeditName = new QLineEdit( this );
        plineeditName->setText( pDocument->getObject()->getName() );
        plineeditName->setToolTip( tr("a short, simple, and meaningful name to give this model") );
        pHLayout->addWidget( new QLabel( tr("Name"), this ) );
        pHLayout->addWidget( plineeditName );
        connect( plineeditName, SIGNAL(textChanged(const QString &)), SLOT(slotName(const QString &)) );

        // CODE
        pCode = new QLineEdit( this );
        pCode->setText( pDocument->getObject()->getCode() );
        pCode->setToolTip( tr("a system code") );
        pHLayout->addWidget( new QLabel( tr("Code"), this ) );
        pHLayout->addWidget( pCode );
        connect( pCode, SIGNAL(textChanged(const QString &)), SLOT(slotCode(const QString &)) );

        pLayout->addRow( pHLayout );
    }

    // COMMENT
    ptexteditComment = new QTextEdit( this );
    ptexteditComment->setText( pDocument->getObject()->getComment() );
    pLayout->addRow( tr("Comment"), ptexteditComment );
    connect( ptexteditComment, SIGNAL(textChanged()), SLOT(slotComment()) );

    {
        QHBoxLayout *pHLayout = new QHBoxLayout();
        // AUTHOR
        plineeditAuthor = new QLineEdit( this );
        plineeditAuthor->setText( pDocument->getAuthor() );
        plineeditAuthor->setToolTip( tr("the author of this model") );
        pHLayout->addWidget( new QLabel( tr("Author"), this ) );
        pHLayout->addWidget( plineeditAuthor );
        connect( plineeditAuthor, SIGNAL(textChanged(const QString &)), SLOT(slotAuthor(const QString &)) );

        // VERSION
        plineeditVersion = new QLineEdit( this );
        plineeditVersion->setText( pDocument->getVer() );
        plineeditVersion->setToolTip( tr("model revision identifier") );
        pHLayout->addWidget( new QLabel( tr("Version"), this ) );
        pHLayout->addWidget( plineeditVersion );
        connect( plineeditVersion, SIGNAL(textChanged(const QString &)), SLOT(slotVersion(const QString &)) );

        pLayout->addRow( pHLayout );
    }

    {
        QHBoxLayout *pHLayout = new QHBoxLayout();
        // CREATION DATE
        pCreatedDate = new QLabel( pDocument->getCreatedDate().toString(), this );
        pCreatedDate->setToolTip( tr("the date/time this model created") );
        pHLayout->addWidget( new QLabel( tr("Created"), this ) );
        pHLayout->addWidget( pCreatedDate );

        // MODIFICATION DATE
        pModifiedDate = new QLabel( pDocument->getModifiedDate().toString(), this );
        pModifiedDate->setToolTip( tr("the date/time this model last modified") );
        pHLayout->addWidget( new QLabel( tr("Modified"), this ) );
        pHLayout->addWidget( pModifiedDate );

        pLayout->addRow( pHLayout );
    }
    
    if ( !stringFileName.isNull() )
    { 
        // FILE NAME
        plabelFileName  = new QLabel( stringFileName, this );
        plabelFileName->setToolTip( tr("file name") );            
        pLayout->addRow( tr("File Name"), plabelFileName );             
    }

    plineeditName->setFocus();
}

AWDocumentNameWidget::~AWDocumentNameWidget()
{
}

void AWDocumentNameWidget::slotName( const QString &s )                   
{                                                                          
    pDocument->getObject()->setName( s.simplified() );                                 
}                                                                          
                                                                           
void AWDocumentNameWidget::slotCode( const QString &s )                   
{                                                                          
    pDocument->getObject()->setCode( s.simplified() );                                 
}                                                                          
                                                                           
void AWDocumentNameWidget::slotComment()                                  
{                                                                          
    pDocument->getObject()->setComment( ptexteditComment->toPlainText().simplified() );
}                                                                          
                                                                           
void AWDocumentNameWidget::slotAuthor( const QString &s )                 
{                                                                          
    pDocument->setAuthor( s.simplified() );                               
}                                                                          
                                                                           
void AWDocumentNameWidget::slotVersion( const QString &s )                
{                                                                          
    pDocument->setVer( s.simplified() );                              
}                                                                          
                                                                           

