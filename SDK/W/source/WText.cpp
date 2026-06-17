/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "WText.h"

/****************************************************************
 * WTextButton
 ****************************************************************/
WTextButton::WTextButton( const CBD::CBDText &t, QWidget *pParent )
    : QToolButton( pParent )
{
    setObjectName( "WTextButton" );
    setToolTip( tr("configure text") );
    this->t = t;
    setIcon( getIcon() );
    connect( this, SIGNAL(clicked()), SLOT(slotClicked()) );
}

void WTextButton::slotRefresh( const CBD::CBDText &t1 )
{
    if ( t.font == t1.font && 
         t.color == t1.color && 
         t.brush == t1.brush && 
         t.nHAlign == t1.nHAlign && 
         t.nVAlign == t1.nVAlign ) return;
    t = t1;
    setIcon( getIcon() );
}

void WTextButton::slotClicked()
{
    CBD::CBDText t1 = WTextDialog::doEdit( t, this );
    if ( t.font == t1.font && 
         t.color == t1.color && 
         t.brush == t1.brush && 
         t.nHAlign == t1.nHAlign && 
         t.nVAlign == t1.nVAlign ) return;
    t = t1;
    emit signalChanged( t );
}

QPixmap WTextButton::getIcon()
{
    int nSize = this->width();

    // handle case where NoBrush - use a contrasting color to pen
    QBrush brush = t.brush;
    if ( brush.style() == Qt::NoBrush )
    {
        brush.setStyle( Qt::SolidPattern );
        if ( t.color.lightness() > 100 ) brush.setColor( Qt::black );
        else brush.setColor( Qt::white );
    }

    t.font.setPixelSize( nSize );
    QPixmap pixmap( nSize, nSize );
    QPainter p( &pixmap );
    p.fillRect( QRect( 0, 0, nSize, nSize ), brush );
    p.setFont( t.font );
    p.setPen( t.color );
    p.drawText( 2, nSize - 4, "T" );
    return pixmap;
}

/****************************************************************
 * WTextToolBar
 ****************************************************************/
WTextToolBar::WTextToolBar( const CBD::CBDText &t, QWidget *pParent )
    : QWidget( pParent )
{
    setObjectName( "WTextToolBar" );
    this->t = t;
    QHBoxLayout *pLayout = new QHBoxLayout( this );

    pFontComboBox = new QFontComboBox( this );
    pFontComboBox->setCurrentFont( t.font );
    connect( pFontComboBox, SIGNAL(currentFontChanged(const QFont &)), SLOT(slotFont(const QFont &)) );
    pLayout->addWidget( pFontComboBox );

    pSize = new QSpinBox( this );
    pSize->setMinimum( 2 );
    pSize->setMaximum( 100 );
    pSize->setValue( t.font.pointSize() );
    connect( pSize, SIGNAL(valueChanged(int)), SLOT(slotSize(int)) );
    pLayout->addWidget( pSize );

    pMore = new QToolButton( this );
    pMore->setText( ":" );
    pLayout->addWidget( pMore );
    connect( pMore, SIGNAL(clicked()), SLOT(slotMore()) );

    pLayout->addStretch( 11 );
}

void WTextToolBar::slotRefresh( const CBD::CBDText &t1 )
{
    if ( t.font == t1.font && 
         t.color == t1.color && 
         t.brush == t1.brush && 
         t.nHAlign == t1.nHAlign && 
         t.nVAlign == t1.nVAlign ) return;
    t = t1;
    pFontComboBox->setCurrentFont( t.font );
    pSize->setValue( t.font.pointSize() );
}

void WTextToolBar::slotFont( const QFont &font )
{
    t.font = font;
    emit signalChanged( t );
}

void WTextToolBar::slotSize( int n )
{
    t.font.setPointSize( n );
    emit signalChanged( t );
}

void WTextToolBar::slotMore()
{
    CBD::CBDText t1 = WTextDialog::doEdit( t, this );
    if ( t.font == t1.font && 
         t.color == t1.color && 
         t.brush == t1.brush && 
         t.nHAlign == t1.nHAlign && 
         t.nVAlign == t1.nVAlign ) return;
    t = t1;
    emit signalChanged( t );
}

/****************************************************************
 * WTextPanel
 ****************************************************************/
WTextPanel::WTextPanel( const CBD::CBDText &t, QWidget *pParent )
    : QWidget( pParent )
{
    this->t = t;

    QVBoxLayout *pLayoutRows = new QVBoxLayout( this );

    // row 1
    {
        QHBoxLayout *pLayoutRow = new QHBoxLayout();
        // font
        {
            QVBoxLayout *pLayout = new QVBoxLayout();

            pLayout->addWidget( new QLabel( tr("Font"), this ) );

            pLineEditFont    = new QLineEdit( this );
            pLineEditFont->setReadOnly( true );
            pLayout->addWidget( pLineEditFont );

            pListWidgetFont    = new QListWidget( this );
            pLayout->addWidget( pListWidgetFont );

            pLayoutRow->addLayout( pLayout );
        }

        // font style
        {
            QVBoxLayout *pLayout = new QVBoxLayout();

            pLayout->addWidget( new QLabel( tr("Font Style"), this ) );

            pLineEditFontStyle   = new QLineEdit( this );
            pLineEditFontStyle->setReadOnly( true );
            pLayout->addWidget( pLineEditFontStyle );

            pListWidgetFontStyle   = new QListWidget( this );
            pLayout->addWidget( pListWidgetFontStyle );

            pLayoutRow->addLayout( pLayout );
        }

        // size
        {
            QVBoxLayout *pLayout = new QVBoxLayout();
            pLayout->addWidget( new QLabel( tr("Size"), this ) );
            pLineEditSize    = new QLineEdit( this );
            pLineEditSize->setValidator( new QIntValidator( pLineEditSize ) );
            pLayout->addWidget( pLineEditSize );
            pListWidgetSize    = new QListWidget( this );
            pLayout->addWidget( pListWidgetSize );
            pLayoutRow->addLayout( pLayout );
        }
        pLayoutRows->addLayout( pLayoutRow );
    }

    // row 2
    {
        QHBoxLayout *pLayoutRow = new QHBoxLayout();
        // effects
        {
            QGroupBox *pGroupBox = new QGroupBox( tr("Effects"), this );
            QHBoxLayout *pLayoutGroupBox = new QHBoxLayout();

            pCheckBoxStrikeOut  = new QCheckBox( tr("Strikeout"), pGroupBox );
            pLayoutGroupBox->addWidget( pCheckBoxStrikeOut );

            pCheckBoxUnderline  = new QCheckBox( tr("Underline"), pGroupBox );
            pLayoutGroupBox->addWidget( pCheckBoxUnderline );

            // pLayoutGroupBox->addStretch( 10 );

            pGroupBox->setLayout( pLayoutGroupBox );
            pLayoutRow->addWidget( pGroupBox );

            pLayoutRow->addWidget( pAlignHTextWidget = new WTextHAlignComboBox( Qt::AlignLeft, this ) );
            pLayoutRow->addWidget( pAlignVTextWidget = new WTextVAlignComboBox( Qt::AlignTop, this ) );

            pLayoutRow->addWidget( pColorButton = new WColorButton( t.color, this, WColorButton::Pen ) );
            pLayoutRow->addWidget( pBrushButton = new WBrushButton( t.brush, this ) );

        }
        pLayoutRows->addLayout( pLayoutRow );
    }

    // row 3
    {
        QHBoxLayout *pLayoutRow = new QHBoxLayout();
        // writing system
        {
            QGroupBox *pGroupBox = new QGroupBox( tr("Writing System"), this );
            QVBoxLayout *pLayoutGroupBox = new QVBoxLayout();

            pComboBoxWritingSystem  = new QComboBox( pGroupBox );
            pComboBoxWritingSystem->setInsertPolicy( QComboBox::InsertAlphabetically );
            pLayoutGroupBox->addWidget( pComboBoxWritingSystem );
            pGroupBox->setLayout( pLayoutGroupBox );
            pLayoutRow->addWidget( pGroupBox );
        }
        pLayoutRows->addLayout( pLayoutRow );
    }

    // row 4
    {
        pSwatch = new WTextSwatch( t, this );
        pSwatch->setMinimumSize( 100, 100 );
        pLayoutRows->addWidget( pSwatch, 20 );
    }

    // init data
    doInitWritingSystems();

    connect( pLineEditSize, SIGNAL(textEdited(const QString &)), SLOT(slotSize(const QString &)) );

    connect( pListWidgetFont, SIGNAL(itemClicked(QListWidgetItem *)), SLOT(slotFonts(QListWidgetItem *)) );
    connect( pListWidgetFontStyle, SIGNAL(itemClicked(QListWidgetItem *)), SLOT(slotFontStyles(QListWidgetItem *)) );
    connect( pListWidgetSize, SIGNAL(itemClicked(QListWidgetItem *)), SLOT(slotSizes(QListWidgetItem *)) );
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    connect( pCheckBoxStrikeOut, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotStrikeout(Qt::CheckState)) );
    connect( pCheckBoxUnderline, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotUnderline(Qt::CheckState)) );
#else
    connect( pCheckBoxStrikeOut, SIGNAL(stateChanged(int)), SLOT(slotStrikeout(int)) );
    connect( pCheckBoxUnderline, SIGNAL(stateChanged(int)), SLOT(slotUnderline(int)) );
#endif
    connect( pAlignHTextWidget,  SIGNAL(signalChanged(Qt::AlignmentFlag)), SLOT(slotAlignH(Qt::AlignmentFlag)) ); 
    connect( pAlignVTextWidget,  SIGNAL(signalChanged(Qt::AlignmentFlag)), SLOT(slotAlignV(Qt::AlignmentFlag)) ); 
    connect( pColorButton,  SIGNAL(signalChanged(QColor)), SLOT(slotColor(QColor)) ); 
    connect( pBrushButton,  SIGNAL(signalChanged(QBrush)), SLOT(slotBrush(QBrush)) ); 
    connect( pComboBoxWritingSystem, SIGNAL(currentIndexChanged(int)), SLOT(slotWritingSystems(int)) );
}

void WTextPanel::slotRefresh( const CBD::CBDText &t1 )
{
    if ( t.font == t1.font && 
         t.color == t1.color && 
         t.brush == t1.brush && 
         t.nHAlign == t1.nHAlign && 
         t.nVAlign == t1.nVAlign ) return;
    t = t1;

    pLineEditFont->setText( t.font.family() ); 
    pLineEditFontStyle->setText( t.font.styleName() );
    pLineEditSize->setText( QString::number( t.font.pointSize() ) );
    pCheckBoxStrikeOut->setChecked( t.font.strikeOut() );
    pCheckBoxUnderline->setChecked( t.font.underline() );
    pAlignHTextWidget->setValue( t.nHAlign );
    pAlignVTextWidget->setValue( t.nVAlign );

    pSwatch->slotRefresh( t );
}

int WTextPanel::getSelectedWritingSystem()
{
    int nIndex = pComboBoxWritingSystem->currentIndex();
    if ( nIndex < 0 ) return -1;
    return pComboBoxWritingSystem->currentData().toInt();
}

QString WTextPanel::getSelectedFont()
{
    if ( pListWidgetFont->currentItem() )
        return pListWidgetFont->currentItem()->text();

    return QString();
}

QString WTextPanel::getSelectedFontStyle()
{
    if ( pListWidgetFontStyle->currentItem() )
        return pListWidgetFontStyle->currentItem()->text();
    return QString();
}

QString WTextPanel::getSelectedSize() 
{
    if ( pListWidgetSize->currentItem() )
        return pListWidgetSize->currentItem()->text();
    return QString();
}

void WTextPanel::doInitWritingSystems()
{
    // we assume that Any is always viable
    QFontDatabase::WritingSystem nDefault = QFontDatabase::Any;
    pComboBoxWritingSystem->addItem( tr("Any"), nDefault );

    // load combo box with name/id
    QList<QFontDatabase::WritingSystem> listWritingSystems = fontDatabase.writingSystems();
    for ( int n = 0; n < listWritingSystems.count(); n++ )
    {
        QString stringSystemName = QFontDatabase::writingSystemName( listWritingSystems.at( n ) );
        pComboBoxWritingSystem->addItem( stringSystemName, listWritingSystems.at( n ) );
    }
    // set default current
    pComboBoxWritingSystem->setCurrentIndex( pComboBoxWritingSystem->findData( nDefault ) );

    doInitFonts();

    // provide some sample text (appropriate for writing system) to the swatch 
    pSwatch->setText( QFontDatabase::writingSystemSample( nDefault ) );
}

void WTextPanel::doInitFonts()
{
    // set line edit based upon font and even if it is not in the list
    pLineEditFont->setText( t.font.family() ); 

    // clear in case we are re-initializing
    pListWidgetFont->clear();

    // IF no selected writing system THEN leave fonts empty 
    int n = getSelectedWritingSystem();
    if ( n < 0 ) return;

    // init based on other controls and then try to sync with font
    QFontDatabase::WritingSystem nWritingSystem = (QFontDatabase::WritingSystem)n;
    QStringList listFamilies = fontDatabase.families( nWritingSystem );

    for ( int n = 0; n < listFamilies.count(); n++ )
    {
        pListWidgetFont->addItem( new QListWidgetItem( listFamilies.at( n ), pListWidgetFont ) );
    }

    // sync with current font if we can
    pLineEditFont->setText( t.font.family() ); // set even if not in list
    QList<QListWidgetItem *> listItems = pListWidgetFont->findItems( t.font.family(), Qt::MatchExactly );    
    if ( !listItems.isEmpty() ) pListWidgetFont->setCurrentItem( listItems.constFirst() );

    doInitFontStyles();
}

void WTextPanel::doInitFontStyles()
{
    // set line edit based upon font and even if it is not in the list
    pLineEditFontStyle->setText( t.font.styleName() );

    // clear in case we are re-initializing
    pListWidgetFontStyle->clear();

    // IF no selected writing system THEN leave fonts empty 
    QString stringFamily = getSelectedFont();
    if ( stringFamily.isEmpty() ) return;

    // init based on other controls and then try to sync with font 
    QStringList listStyles = fontDatabase.styles( stringFamily );

    for ( int n = 0; n < listStyles.count(); n++ )
    {
        pListWidgetFontStyle->addItem( new QListWidgetItem( listStyles.at( n ), pListWidgetFontStyle ) );
    }

    // sync with current font if we can
    QList<QListWidgetItem *> listItems = pListWidgetFontStyle->findItems( t.font.styleName(), Qt::MatchExactly );
    if ( !listItems.isEmpty() ) pListWidgetFontStyle->setCurrentItem( listItems.constFirst() );

    doInitSizes();
}

void WTextPanel::doInitSizes()
{
    // set line edit based upon font and even if it is not in the list
    pLineEditSize->setText( QString::number( t.font.pointSize() ) );

    // clear in case we are re-initializing
    pListWidgetSize->clear();

    // IF no selected style THEN leave sizes empty 
    QString stringFamily    = getSelectedFont();
    QString stringStyle     = getSelectedFontStyle();
    if ( stringStyle.isEmpty() ) return;

    // init based on other controls and then try to sync with font 
    QList<int> listSizes = fontDatabase.pointSizes( stringFamily, stringStyle );

    for ( int n = 0; n < listSizes.count(); n++ )
    {
        pListWidgetSize->addItem( new QListWidgetItem( QString::number( listSizes.at( n ) ), pListWidgetSize ) );
    }

    // sync with current font
    QList<QListWidgetItem *> listItems = pListWidgetSize->findItems( QString::number( t.font.pointSize() ), Qt::MatchExactly );    
    if ( !listItems.isEmpty() ) pListWidgetSize->setCurrentItem( listItems.constFirst() );

    doInitEffects();
}

void WTextPanel::doInitEffects()
{
    // set based upon font
    // enable/disable based upon feature availability or are we always available?
    pCheckBoxStrikeOut->setChecked( t.font.strikeOut() );
    pCheckBoxUnderline->setChecked( t.font.underline() );
    pAlignHTextWidget->setValue( t.nHAlign );
    pAlignVTextWidget->setValue( t.nVAlign );
}

void WTextPanel::slotSize( const QString &stringSize )
{
    int nSize = stringSize.toInt();
    if ( nSize < 1 ) return;
    t.font.setPointSize( nSize );

    QList<QListWidgetItem *> listItems = pListWidgetSize->findItems( stringSize, Qt::MatchExactly );
    if ( listItems.count() ) pListWidgetSize->setCurrentItem( listItems.constFirst() );

    pSwatch->slotRefresh( t );

    emit signalChanged( t );
}

void WTextPanel::slotWritingSystems( int nIndex )
{
    Q_ASSERT( nIndex >= 0 );

    // sync dependencies
    doInitFonts();

    pSwatch->slotRefresh( t );

    emit signalChanged( t );
}

void WTextPanel::slotFonts( QListWidgetItem *p )
{
    if ( !p ) return;

    t.font.setFamily( p->text() );

    pLineEditFont->setText( t.font.family() );

    // sync dependencies
    doInitFontStyles();

    pSwatch->slotRefresh( t );

    emit signalChanged( t );
}

void WTextPanel::slotFontStyles( QListWidgetItem *p )
{
    if ( !p ) return;

    t.font.setStyleName( p->text() );
    pLineEditFontStyle->setText( t.font.styleName() );

    // sync dependencies
    doInitSizes();

    pSwatch->slotRefresh( t );

    emit signalChanged( t );
}

void WTextPanel::slotSizes( QListWidgetItem *p )
{
    if ( !p ) return;

    t.font.setPointSize( p->text().toInt() );
    pLineEditSize->setText( QString::number( t.font.pointSize() ) );

    // sync dependencies - not sure effects qualifies but...
    doInitEffects();

    pSwatch->slotRefresh( t );

    emit signalChanged( t );
}

void WTextPanel::slotStrikeout( Qt::CheckState nState )
{
    if ( nState == Qt::Checked ) 
        t.font.setStrikeOut(true);
    else
        t.font.setStrikeOut( false );

    pSwatch->slotRefresh( t );

    emit signalChanged( t );
}

void WTextPanel::slotUnderline( Qt::CheckState nState )
{
    if ( nState == Qt::Checked ) 
        t.font.setUnderline( true );
    else
        t.font.setUnderline( false );

    pSwatch->slotRefresh( t );

    emit signalChanged( t );
}

void WTextPanel::slotAlignH( Qt::AlignmentFlag n )
{
    t.nHAlign = n;
    pSwatch->slotRefresh( t );
    emit signalChanged( t );
}

void WTextPanel::slotAlignV( Qt::AlignmentFlag n )
{
    t.nVAlign = n;
    pSwatch->slotRefresh( t );
    emit signalChanged( t );
}

void WTextPanel::slotColor( QColor color )
{
    t.color = color;
    pSwatch->slotRefresh( t );
    emit signalChanged( t );
}

void WTextPanel::slotBrush( QBrush brush ) 
{
    t.brush = brush;
    pSwatch->slotRefresh( t );
    emit signalChanged( t );
}

/****************************************************************
 * WTextSwatch
 ****************************************************************/
WTextSwatch::WTextSwatch( const CBD::CBDText &t, QWidget *pParent, const QString &stringText )
    : QWidget( pParent )
{
    this->t = t;
    this->stringText = stringText;
}

void WTextSwatch::setText( const QString &s )
{
    stringText = s;
    update();
}

void WTextSwatch::slotRefresh( const CBD::CBDText &t1 )
{
    if ( t.font == t1.font && 
         t.color == t1.color && 
         t.brush == t1.brush && 
         t.nHAlign == t1.nHAlign && 
         t.nVAlign == t1.nVAlign ) return;
    t = t1;
    update();
}

void WTextSwatch::paintEvent( QPaintEvent *pEvent )
{
    Q_UNUSED(pEvent);

    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setRenderHint( QPainter::SmoothPixmapTransform );
    painter.setFont( t.font );
    painter.setPen( t.color );
    painter.fillRect( rect(), t.brush );
    painter.drawText( rect(), Qt::AlignCenter, stringText );
}

/****************************************************************
 * WTextDialog
 ****************************************************************/
WTextDialog::WTextDialog( const CBD::CBDText &t, QWidget *pParent )
    : QDialog( pParent )
{
    setWindowTitle( tr("Text") );

    QVBoxLayout *pLayout = new QVBoxLayout( this );
    pPanel = new WTextPanel( t, this );
    pLayout->addWidget( pPanel );

    QDialogButtonBox *pButtonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this );
    pLayout->addWidget( pButtonBox ); 

    connect( pButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept );
    connect( pButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject );
}

CBD::CBDText WTextDialog::doEdit( const CBD::CBDText &t, QWidget *pParent )
{
    WTextDialog dialog( t, pParent );
    if ( dialog.exec() == QDialog::Accepted )
        return dialog.getText();
    return t;
}


