#include "LibInfo.h"
#include "FRLabel.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRLabel::FRLabel( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRLabel::~FRLabel()
{
}

void FRLabel::setName( const QString &s )                   
{
    DRectangle::setName( s );  

    FRGraphicsProxyObject *pProxy = (FRGraphicsProxyObject*)getProxy();  
    Q_ASSERT( pProxy );

    QLabel *pLabel = (QLabel*)pProxy->widget();
    Q_ASSERT( pLabel );

    pLabel->setText( s );
}                                                              
                                                               
void FRLabel::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QLabel();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRLabel::getIcon()
{
    return QPixmap( ":FR/Label" );
}

AWPropWidget *FRLabel::getPropWidget( QWidget *pWidgetParent )
{
    AWPropWidget *pPropWidget = FRObject::getPropWidget( pWidgetParent );

    pPropWidget->addWidget( tr("Text"), new FRLabelPropWidget( this, pPropWidget ) );

    return pPropWidget;

}

//
//
//
FRLabelPropWidget::FRLabelPropWidget( FRLabel *p, QWidget *pParent )
    : QWidget( pParent )
{
    pLabel = p;

    FRGraphicsProxyObject *pProxy = (FRGraphicsProxyObject*)pLabel->getProxy();  
    Q_ASSERT( pProxy );

    pWidget = (QLabel*)pProxy->widget();
    Q_ASSERT( pLabel );

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );
    QFormLayout *pLayout = new QFormLayout();

    pLineEditText = new QLineEdit( pWidget->text(), this );
    pLayout->addRow( tr( "Text:" ), pLineEditText );
    connect( pLineEditText, SIGNAL(editingFinished()), SLOT(slotText()) );

    pJustify = new WTextHAlignComboBox( pLabel->getTextHAlign(), this );
    pLayout->addRow( tr( "Justify:" ), pJustify );
    connect( pJustify, SIGNAL(signalChanged(Qt::AlignmentFlag)), pLabel, SLOT(slotTextHAlign(Qt::AlignmentFlag)) );

    pAlign = new WTextVAlignComboBox( pLabel->getTextVAlign(), this );
    pLayout->addRow( tr( "Align:" ), pAlign );
    connect( pAlign, SIGNAL(signalChanged(Qt::AlignmentFlag)), pLabel, SLOT(slotTextVAlign(Qt::AlignmentFlag)) );

    pLayoutTop->addLayout( pLayout );
    pLayoutTop->addStretch( 10 );

    connect( pLabel, SIGNAL(signalModified()), SLOT(slotModified()) );
}

void FRLabelPropWidget::slotText()     
{
    pWidget->setText( pLineEditText->text() );
}

void FRLabelPropWidget::slotModified()
{
    pLineEditText->setText( pWidget->text() );
    // pJustify->setValue( pLabel->getTextHAlign() );
    // pAlign->setValue( pLabel->getTextVAlign() );
}


