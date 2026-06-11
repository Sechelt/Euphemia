/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "SText.h"

/****************************************************************
 * SText
 ****************************************************************/
SText::SText()
{
    text.color = Qt::black;
}

void SText::setValue( const CBD::CBDText &t1 )
{
    if ( text.font == t1.font && 
         text.color == t1.color && 
         text.brush == t1.brush && 
         text.nHAlign == t1.nHAlign && 
         text.nVAlign == t1.nVAlign ) return;
    text = t1;
    emit signalChanged();
}

void SText::setSelected( const CBD::CBDText &t1 )
{
    if ( text.font == t1.font && 
         text.color == t1.color && 
         text.brush == t1.brush && 
         text.nHAlign == t1.nHAlign && 
         text.nVAlign == t1.nVAlign ) return;
    text = t1;
    emit signalSync();
}

QWidget *SText::getButton( QWidget *pParent )
{
    return new STextButton( pParent );
}

QWidget *SText::getToolBar( QWidget *pParent )
{
    return new STextToolBar( pParent );
}

QWidget *SText::getPanel( QWidget *pParent )
{
    return new STextPanel( pParent );
}

QWidget *SText::getSwatch( QWidget *pParent )
{
    return new STextSwatch( pParent );
}

void SText::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = pdomDoc->createElement( "SText" );

    domElemThis.setAttribute( "HAlign", text.nHAlign );
    domElemThis.setAttribute( "VAlign", text.nVAlign );
    domElemThis.appendChild( CBDPersistNative::doSaveFont( text.font, pdomDoc ) );
    domElemThis.appendChild( CBDPersistNative::doSaveColor( text.color, pdomDoc ) );
    domElemThis.appendChild( CBDPersistNative::doSaveBrush( text.brush, pdomDoc ) );

    pdomElemParent->appendChild( domElemThis );
}

void SText::doLoad( QDomElement *pdomElemThis )
{
    text.nHAlign = (Qt::AlignmentFlag)pdomElemThis->attribute( "HAlign", QString("%1").arg(Qt::AlignLeft) ).toInt();
    text.nVAlign = (Qt::AlignmentFlag)pdomElemThis->attribute( "VAlign", QString("%1").arg(Qt::AlignBottom) ).toInt();

    // load child nodes...
    text.font  = QFont();
    text.color = Qt::black;
    text.brush = QBrush();

    QDomElement         domElem;
    QDomNode            domNode;
    domNode = pdomElemThis->firstChild();
    while( !domNode.isNull() ) 
    {
        domElem = domNode.toElement();
        if( domElem.isNull() ) 
        {
            domNode = domNode.nextSibling();
            continue;
        }

        if ( domElem.tagName() == "Font" ) text.font = CBDPersistNative::doLoadFont( &domElem );
        else if ( domElem.tagName() == "Color" ) text.color = CBDPersistNative::doLoadColor( &domElem );
        else if ( domElem.tagName() == "Brush" ) text.brush = CBDPersistNative::doLoadBrush( &domElem );

        domNode = domNode.nextSibling();
    }
}

/****************************************************************
 * STextButton
 ****************************************************************/
STextButton::STextButton( QWidget *pParent )
    : QToolButton( pParent )
{
    setObjectName( "STextButton" );

    pText = (SText*)g_SSettings->getSetting( "SText" );
    if ( pText )
    {
        setIcon( getIcon() );

        connect( pText, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
        connect( pText, SIGNAL(signalSync()), SLOT(slotRefresh()) );
        connect( this, SIGNAL(clicked()), SLOT(slotClicked()) );
    }
    else
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [SText].";
    }
}

void STextButton::slotRefresh()
{
    setIcon( getIcon() );
}

void STextButton::slotClicked()
{
    SDialog dialog( this, "SText", "Font" );
    dialog.exec();
}

QPixmap STextButton::getIcon()
{
    int nSize = this->width();
    CBD::CBDText text = pText->getValue();

    // handle case where NoBrush - use a contrasting color to pen
    QBrush brush = text.brush;
    if ( brush.style() == Qt::NoBrush )
    {
        brush.setStyle( Qt::SolidPattern );
        if ( text.color.lightness() > 100 ) brush.setColor( Qt::black );
        else brush.setColor( Qt::white );
    }

    text.font.setPixelSize( nSize );
    QPixmap pixmap( nSize, nSize );
    QPainter p( &pixmap );
    p.fillRect( QRect( 0, 0, nSize, nSize ), brush );
    p.setFont( text.font );
    p.setPen( text.color );
    p.drawText( 2, nSize - 4, "T" );
    return pixmap;
}

/****************************************************************
 * STextToolBar
 ****************************************************************/
STextToolBar::STextToolBar( QWidget *pParent )
    : QWidget( pParent )
{
    setObjectName( "STextToolBar" );

    pSetting = (SText*)g_SSettings->getSetting( "SText" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [SText].";
        return;
    }

    text = pSetting->getValue();

    QHBoxLayout *pLayout = new QHBoxLayout( this );

    pFontComboBox = new QFontComboBox( this );
    pFontComboBox->setCurrentFont( text.font );
    connect( pFontComboBox, SIGNAL(currentFontChanged(const QFont &)), SLOT(slotFont(const QFont &)) );
    pLayout->addWidget( pFontComboBox );

    pSize = new QSpinBox( this );
    pSize->setMinimum( 2 );
    pSize->setMaximum( 100 );
    pSize->setValue( text.font.pointSize() );
    connect( pSize, SIGNAL(valueChanged(int)), SLOT(slotSize(int)) );
    pLayout->addWidget( pSize );

    pMore = new QToolButton( this );
    pMore->setText( ":" );
    pLayout->addWidget( pMore );
    connect( pMore, SIGNAL(clicked()), SLOT(slotMore()) );

    pLayout->addStretch( 11 );

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void STextToolBar::slotRefresh()
{
    text = pSetting->getValue();
    pFontComboBox->setCurrentFont( text.font );
    pSize->setValue( text.font.pointSize() );
}

void STextToolBar::slotFont( const QFont &t )
{
    text.font = t;
    pSetting->setValue( text );
}

void STextToolBar::slotSize( int n )
{
    text.font.setPointSize( n );
    pSetting->setValue( text );
}

void STextToolBar::slotMore()
{
    SDialog dialog( this, "SText", tr("Text") );
    dialog.exec();
}

/****************************************************************
 * STextPanel
 ****************************************************************/
STextPanel::STextPanel( QWidget *pParent )
    : QWidget( pParent )
{
    pSetting = (SText*)g_SSettings->getSetting( "SText" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [SText].";
        return;
    }

    text = pSetting->getValue();

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

            pLayoutRow->addWidget( pColorButton = new WColorButton( text.color, this, WColorButton::Pen ) );
            pLayoutRow->addWidget( pBrushButton = new WBrushButton( text.brush, this ) );

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
        pSwatch = new STextSwatch( this );
        pSwatch->setMinimumSize( 100, 100 );
        pLayoutRows->addWidget( pSwatch, 20 );
    }

    // init data
    doInitWritingSystems();

    connect( pLineEditSize, SIGNAL(textEdited(const QString &)), SLOT(slotSize(const QString &)) );

    connect( pListWidgetFont, SIGNAL(itemClicked(QListWidgetItem *)), SLOT(slotFonts(QListWidgetItem *)) );
    connect( pListWidgetFontStyle, SIGNAL(itemClicked(QListWidgetItem *)), SLOT(slotFontStyles(QListWidgetItem *)) );
    connect( pListWidgetSize, SIGNAL(itemClicked(QListWidgetItem *)), SLOT(slotSizes(QListWidgetItem *)) );
    connect( pCheckBoxStrikeOut, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotStrikeout(Qt::CheckState)) );
    connect( pCheckBoxUnderline, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotUnderline(Qt::CheckState)) );
    connect( pAlignHTextWidget,  SIGNAL(signalChanged(Qt::AlignmentFlag)), SLOT(slotAlignH(Qt::AlignmentFlag)) ); 
    connect( pAlignVTextWidget,  SIGNAL(signalChanged(Qt::AlignmentFlag)), SLOT(slotAlignV(Qt::AlignmentFlag)) ); 
    connect( pColorButton,  SIGNAL(signalChanged(QColor)), SLOT(slotColor(QColor)) ); 
    connect( pBrushButton,  SIGNAL(signalChanged(QBrush)), SLOT(slotBrush(QBrush)) ); 
    connect( pComboBoxWritingSystem, SIGNAL(currentIndexChanged(int)), SLOT(slotWritingSystems(int)) );

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void STextPanel::slotRefresh()
{
    text = pSetting->getValue();

    pLineEditFont->setText( text.font.family() ); 
    pLineEditFontStyle->setText( text.font.styleName() );
    pLineEditSize->setText( QString::number( text.font.pointSize() ) );
    pCheckBoxStrikeOut->setChecked( text.font.strikeOut() );
    pCheckBoxUnderline->setChecked( text.font.underline() );
    pAlignHTextWidget->setValue( text.nHAlign );
    pAlignVTextWidget->setValue( text.nVAlign );
}

int STextPanel::getSelectedWritingSystem()
{
    int nIndex = pComboBoxWritingSystem->currentIndex();
    if ( nIndex < 0 ) return -1;
    return pComboBoxWritingSystem->currentData().toInt();
}

QString STextPanel::getSelectedFont()
{
    if ( pListWidgetFont->currentItem() )
        return pListWidgetFont->currentItem()->text();

    return QString();
}

QString STextPanel::getSelectedFontStyle()
{
    if ( pListWidgetFontStyle->currentItem() )
        return pListWidgetFontStyle->currentItem()->text();
    return QString();
}

QString STextPanel::getSelectedSize() 
{
    if ( pListWidgetSize->currentItem() )
        return pListWidgetSize->currentItem()->text();
    return QString();
}

void STextPanel::doInitWritingSystems()
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

void STextPanel::doInitFonts()
{
    // set line edit based upon font and even if it is not in the list
    pLineEditFont->setText( text.font.family() ); 

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
    pLineEditFont->setText( text.font.family() ); // set even if not in list
    QList<QListWidgetItem *> listItems = pListWidgetFont->findItems( text.font.family(), Qt::MatchExactly );    
    if ( !listItems.isEmpty() ) pListWidgetFont->setCurrentItem( listItems.constFirst() );

    doInitFontStyles();
}

void STextPanel::doInitFontStyles()
{
    // set line edit based upon font and even if it is not in the list
    pLineEditFontStyle->setText( text.font.styleName() );

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
    QList<QListWidgetItem *> listItems = pListWidgetFontStyle->findItems( text.font.styleName(), Qt::MatchExactly );
    if ( !listItems.isEmpty() ) pListWidgetFontStyle->setCurrentItem( listItems.constFirst() );

    doInitSizes();
}

void STextPanel::doInitSizes()
{
    // set line edit based upon font and even if it is not in the list
    pLineEditSize->setText( QString::number( text.font.pointSize() ) );

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
    QList<QListWidgetItem *> listItems = pListWidgetSize->findItems( QString::number( text.font.pointSize() ), Qt::MatchExactly );    
    if ( !listItems.isEmpty() ) pListWidgetSize->setCurrentItem( listItems.constFirst() );

    doInitEffects();
}

void STextPanel::doInitEffects()
{
    // set based upon font
    // enable/disable based upon feature availability or are we always available?
    pCheckBoxStrikeOut->setChecked( text.font.strikeOut() );
    pCheckBoxUnderline->setChecked( text.font.underline() );
    pAlignHTextWidget->setValue( text.nHAlign );
    pAlignVTextWidget->setValue( text.nVAlign );
}

void STextPanel::slotSize( const QString &stringSize )
{
    int nSize = stringSize.toInt();
    if ( nSize < 1 ) return;
    text.font.setPointSize( nSize );

    QList<QListWidgetItem *> listItems = pListWidgetSize->findItems( stringSize, Qt::MatchExactly );
    if ( listItems.count() ) pListWidgetSize->setCurrentItem( listItems.constFirst() );

    pSetting->setValue( text );
}

void STextPanel::slotWritingSystems( int nIndex )
{
    Q_ASSERT( nIndex >= 0 );

    // sync dependencies
    doInitFonts();

    pSetting->setValue( text );
}

void STextPanel::slotFonts( QListWidgetItem *p )
{
    if ( !p ) return;

    text.font.setFamily( p->text() );

    pLineEditFont->setText( text.font.family() );

    // sync dependencies
    doInitFontStyles();

    pSetting->setValue( text );
}

void STextPanel::slotFontStyles( QListWidgetItem *p )
{
    if ( !p ) return;

    text.font.setStyleName( p->text() );
    pLineEditFontStyle->setText( text.font.styleName() );

    // sync dependencies
    doInitSizes();

    pSetting->setValue( text );
}

void STextPanel::slotSizes( QListWidgetItem *p )
{
    if ( !p ) return;

    text.font.setPointSize( p->text().toInt() );
    pLineEditSize->setText( QString::number( text.font.pointSize() ) );

    // sync dependencies - not sure effects qualifies but...
    doInitEffects();

    pSetting->setValue( text );
}

void STextPanel::slotStrikeout( Qt::CheckState nState )
{
    if ( nState == Qt::Checked ) 
        text.font.setStrikeOut(true);
    else
        text.font.setStrikeOut( false );

    pSetting->setValue( text );
}

void STextPanel::slotUnderline( Qt::CheckState nState )
{
    if ( nState == Qt::Checked ) 
        text.font.setUnderline( true );
    else
        text.font.setUnderline( false );

    pSetting->setValue( text );
}

void STextPanel::slotAlignH( Qt::AlignmentFlag n )
{
    text.nHAlign = n;
    pSetting->setValue( text );
}

void STextPanel::slotAlignV( Qt::AlignmentFlag n )
{
    text.nVAlign = n;
    pSetting->setValue( text );
}

void STextPanel::slotColor( QColor color )
{
    text.color = color;
    pSetting->setValue( text );
}

void STextPanel::slotBrush( QBrush brush ) 
{
    text.brush = brush;
    pSetting->setValue( text );
}

/****************************************************************
 * STextSwatch
 ****************************************************************/
STextSwatch::STextSwatch( QWidget *pParent, const QString &stringText )
    : QWidget( pParent )
{
    pSetting = (SText*)g_SSettings->getSetting( "SText" ); Q_ASSERT( pSetting );

    this->stringText    = stringText;

    slotRefresh();

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
}

void STextSwatch::setText( const QString &s )
{
    stringText = s;
    update();
}

void STextSwatch::slotRefresh()
{
    text = pSetting->getValue();
    update();
}

void STextSwatch::paintEvent( QPaintEvent *pEvent )
{
    Q_UNUSED(pEvent);

    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setRenderHint( QPainter::SmoothPixmapTransform );
    painter.setFont( text.font );
    painter.setPen( text.color );
    painter.fillRect( rect(), text.brush );
    painter.drawText( rect(), Qt::AlignCenter, stringText );
}


