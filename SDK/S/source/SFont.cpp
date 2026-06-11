/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "SFont.h"

/****************************************************************
 * SFont
 ****************************************************************/
SFont::SFont()
{
}

void SFont::setValue( const QFont &t )
{
    if ( t == font ) return;
    font = t;
    emit signalChanged();
}

void SFont::setSelected( const QFont &t )
{
    if ( t == font ) return;
    font = t;
    emit signalSync();
}

QWidget *SFont::getButton( QWidget *pParent )
{
    return new SFontButton( pParent );
}

QWidget *SFont::getToolBar( QWidget *pParent )
{
    return new SFontToolBar( pParent );
}

QWidget *SFont::getPanel( QWidget *pParent )
{
    return new SFontPanel( pParent );
}

QWidget *SFont::getSwatch( QWidget *pParent )
{
    return new SFontSwatch( pParent );
}

void SFont::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = pdomDoc->createElement( "SFont" );

    domElemThis.appendChild( CBDPersistNative::doSaveFont( font, pdomDoc ) );

    pdomElemParent->appendChild( domElemThis );
}

void SFont::doLoad( QDomElement *pdomElemThis )
{
    // load child nodes...
    font  = QFont();

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

        if ( domElem.tagName() == "Font" ) font = CBDPersistNative::doLoadFont( &domElem );

        domNode = domNode.nextSibling();
    }
}

/****************************************************************
 * SFontButton
 ****************************************************************/
SFontButton::SFontButton( QWidget *pParent )
    : QToolButton( pParent )
{
    setObjectName( "SFontButton" );

    pFont = (SFont*)g_SSettings->getSetting( "SFont" );
    if ( pFont )
    {
        setIcon( getIcon() );

        connect( pFont, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
        connect( pFont, SIGNAL(signalSync()), SLOT(slotRefresh()) );
        connect( this, SIGNAL(clicked()), SLOT(slotClicked()) );
    }
    else
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [SFont].";
    }
}

void SFontButton::slotRefresh()
{
    setIcon( getIcon() );
}

void SFontButton::slotClicked()
{
    SDialog dialog( this, "SFont", "Font" );
    dialog.exec();
}

QPixmap SFontButton::getIcon()
{
    int nSize = this->width();
    QFont font = pFont->getValue();

    font.setPixelSize( nSize );
    QPixmap pixmap( nSize, nSize );
    QPainter p( &pixmap );
    p.fillRect( QRect( 0, 0, nSize, nSize ), QBrush( Qt::white ) );
    p.setFont( font );
    p.setPen( Qt::black );
    p.drawText( 2, nSize - 4, "F" );
    return pixmap;
}

/****************************************************************
 * SFontToolBar
 ****************************************************************/
SFontToolBar::SFontToolBar( QWidget *pParent )
    : QWidget( pParent )
{
    setObjectName( "SFontToolBar" );

    pSetting = (SFont*)g_SSettings->getSetting( "SFont" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [SFont].";
        return;
    }

    font = pSetting->getValue();

    QHBoxLayout *pLayout = new QHBoxLayout( this );

    pFontComboBox = new QFontComboBox( this );
    pFontComboBox->setCurrentFont( font );
    connect( pFontComboBox, SIGNAL(currentFontChanged(const QFont &)), SLOT(slotFont(const QFont &)) );
    pLayout->addWidget( pFontComboBox );

    pSize = new QSpinBox( this );
    pSize->setMinimum( 2 );
    pSize->setMaximum( 100 );
    pSize->setValue( font.pointSize() );
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

void SFontToolBar::slotRefresh()
{
    font = pSetting->getValue();
    pFontComboBox->setCurrentFont( font );
    pSize->setValue( font.pointSize() );
}

void SFontToolBar::slotFont( const QFont &t )
{
    font = t;
    pSetting->setValue( t );
}

void SFontToolBar::slotSize( int n )
{
    font.setPointSize( n );
    pSetting->setValue( font );
}

void SFontToolBar::slotMore()
{
    SDialog dialog( this, "SFont", tr("Font") );
    dialog.exec();
}

/****************************************************************
 * SFontPanel
 ****************************************************************/
SFontPanel::SFontPanel( QWidget *pParent )
    : QWidget( pParent )
{
    pSetting = (SFont*)g_SSettings->getSetting( "SFont" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [SFont].";
        return;
    }

    font = pSetting->getValue();

    QHBoxLayout *pLayoutTop = new QHBoxLayout( this ); // we can add optional palette to this 
    QVBoxLayout *pLayoutStandard = new QVBoxLayout();  // rest goes here

    // row 1
    {
        QHBoxLayout *pLayoutRow = new QHBoxLayout();
        // font
        {
            QVBoxLayout *pLayout = new QVBoxLayout();

            pLayout->addWidget( new QLabel( tr("Family"), this ) );

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

            pLayout->addWidget( new QLabel( tr("Style"), this ) );

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
        pLayoutStandard->addLayout( pLayoutRow );
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

            pLayoutGroupBox->addStretch( 10 );

            pGroupBox->setLayout( pLayoutGroupBox );
            pLayoutRow->addWidget( pGroupBox );
        }
        pLayoutStandard->addLayout( pLayoutRow );
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
        pLayoutStandard->addLayout( pLayoutRow );
    }

    pLayoutTop->addLayout( pLayoutStandard );

    // init data
    doInitWritingSystems();

    connect( pLineEditSize, SIGNAL(textEdited(const QString &)), SLOT(slotSize(const QString &)) );

    connect( pListWidgetFont, SIGNAL(itemClicked(QListWidgetItem *)), SLOT(slotFonts(QListWidgetItem *)) );
    connect( pListWidgetFontStyle, SIGNAL(itemClicked(QListWidgetItem *)), SLOT(slotFontStyles(QListWidgetItem *)) );
    connect( pListWidgetSize, SIGNAL(itemClicked(QListWidgetItem *)), SLOT(slotSizes(QListWidgetItem *)) );
    connect( pCheckBoxStrikeOut, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotStrikeout(Qt::CheckState)) );
    connect( pCheckBoxUnderline, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotUnderline(Qt::CheckState)) );
    connect( pComboBoxWritingSystem, SIGNAL(currentIndexChanged(int)), SLOT(slotWritingSystems(int)) );

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void SFontPanel::slotRefresh()
{
    font = pSetting->getValue();

    pLineEditFont->setText( font.family() ); 
    pLineEditFontStyle->setText( font.styleName() );
    pLineEditSize->setText( QString::number( font.pointSize() ) );
    pCheckBoxStrikeOut->setChecked( font.strikeOut() );
    pCheckBoxUnderline->setChecked( font.underline() );
}

int SFontPanel::getSelectedWritingSystem()
{
    int nIndex = pComboBoxWritingSystem->currentIndex();
    if ( nIndex < 0 ) return -1;
    return pComboBoxWritingSystem->currentData().toInt();
}

QString SFontPanel::getSelectedFont()
{
    if ( pListWidgetFont->currentItem() )
        return pListWidgetFont->currentItem()->text();

    return QString();
}

QString SFontPanel::getSelectedFontStyle()
{
    if ( pListWidgetFontStyle->currentItem() )
        return pListWidgetFontStyle->currentItem()->text();
    return QString();
}

QString SFontPanel::getSelectedSize() 
{
    if ( pListWidgetSize->currentItem() )
        return pListWidgetSize->currentItem()->text();
    return QString();
}

void SFontPanel::doInitWritingSystems()
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
    // pFontDialogSwatch->setText( QFontDatabase::writingSystemSample( nDefault ) );
}

void SFontPanel::doInitFonts()
{
    // set line edit based upon font and even if it is not in the list
    pLineEditFont->setText( font.family() ); 

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
    pLineEditFont->setText( font.family() ); // set even if not in list
    QList<QListWidgetItem *> listItems = pListWidgetFont->findItems( font.family(), Qt::MatchExactly );    
    if ( !listItems.isEmpty() ) pListWidgetFont->setCurrentItem( listItems.constFirst() );

    doInitFontStyles();
}

void SFontPanel::doInitFontStyles()
{
    // set line edit based upon font and even if it is not in the list
    pLineEditFontStyle->setText( font.styleName() );

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
    QList<QListWidgetItem *> listItems = pListWidgetFontStyle->findItems( font.styleName(), Qt::MatchExactly );
    if ( !listItems.isEmpty() ) pListWidgetFontStyle->setCurrentItem( listItems.constFirst() );

    doInitSizes();
}

void SFontPanel::doInitSizes()
{
    // set line edit based upon font and even if it is not in the list
    pLineEditSize->setText( QString::number( font.pointSize() ) );

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
    QList<QListWidgetItem *> listItems = pListWidgetSize->findItems( QString::number( font.pointSize() ), Qt::MatchExactly );    
    if ( !listItems.isEmpty() ) pListWidgetSize->setCurrentItem( listItems.constFirst() );

    doInitEffects();
}

void SFontPanel::doInitEffects()
{
    // set based upon font
    // enable/disable based upon feature availability or are we always available?
    pCheckBoxStrikeOut->setChecked( font.strikeOut() );
    pCheckBoxUnderline->setChecked( font.underline() );
}

void SFontPanel::slotSize( const QString &stringSize )
{
    int nSize = stringSize.toInt();
    if ( nSize < 1 ) return;
    font.setPointSize( nSize );

    QList<QListWidgetItem *> listItems = pListWidgetSize->findItems( stringSize, Qt::MatchExactly );
    if ( listItems.count() ) pListWidgetSize->setCurrentItem( listItems.constFirst() );

    pSetting->setValue( font );
}

void SFontPanel::slotWritingSystems( int nIndex )
{
    Q_ASSERT( nIndex >= 0 );

    // sync dependencies
    doInitFonts();

    pSetting->setValue( font );
}

void SFontPanel::slotFonts( QListWidgetItem *p )
{
    if ( !p ) return;

    font.setFamily( p->text() );

    pLineEditFont->setText( font.family() );

    // sync dependencies
    doInitFontStyles();

    pSetting->setValue( font );
}

void SFontPanel::slotFontStyles( QListWidgetItem *p )
{
    if ( !p ) return;

    font.setStyleName( p->text() );
    pLineEditFontStyle->setText( font.styleName() );

    // sync dependencies
    doInitSizes();

    pSetting->setValue( font );
}

void SFontPanel::slotSizes( QListWidgetItem *p )
{
    if ( !p ) return;

    font.setPointSize( p->text().toInt() );
    pLineEditSize->setText( QString::number( font.pointSize() ) );

    // sync dependencies - not sure effects qualifies but...
    doInitEffects();

    pSetting->setValue( font );
}

void SFontPanel::slotStrikeout( Qt::CheckState nState )
{
    if ( nState == Qt::Checked ) 
        font.setStrikeOut(true);
    else
        font.setStrikeOut( false );

    pSetting->setValue( font );
}

void SFontPanel::slotUnderline( Qt::CheckState nState )
{
    if ( nState == Qt::Checked ) 
        font.setUnderline( true );
    else
        font.setUnderline( false );

    pSetting->setValue( font );
}

/****************************************************************
 * SFontSwatch
 ****************************************************************/
SFontSwatch::SFontSwatch( QWidget *pParent, const QString &stringText )
    : QWidget( pParent )
{
    // pSettingDialogSwatch->setText( QFontDatabase::writingSystemSample( nWritingSystem ) );
    pSetting = (SFont*)g_SSettings->getSetting( "SFont" ); Q_ASSERT( pSetting );
    pSettingPen = (SPen*)g_SSettings->getSetting( "SPen" ); Q_ASSERT( pSettingPen );
    pSettingBrush = (SBrush*)g_SSettings->getSetting( "SBrush" ); Q_ASSERT( pSettingBrush );

    this->stringText    = stringText;

    slotRefresh();
    slotRefreshPen();
    slotRefreshBrush();

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSettingPen, SIGNAL(signalChanged()), SLOT(slotRefreshPen()) );
    connect( pSettingBrush, SIGNAL(signalChanged()), SLOT(slotRefreshBrush()) );
}

void SFontSwatch::slotRefresh()
{
    font = pSetting->getValue();
    update();
}

void SFontSwatch::slotRefreshPen()
{
    pen = pSettingPen->getValue();
    update();
}

void SFontSwatch::slotRefreshBrush()
{
    brush = pSettingBrush->getValue();
    update();
}

void SFontSwatch::paintEvent( QPaintEvent *pEvent )
{
    Q_UNUSED(pEvent);

    QPen pen( Qt::black );
    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setRenderHint( QPainter::SmoothPixmapTransform );
    painter.setPen( pen );
    painter.fillRect( rect(), brush );
    painter.drawText( rect(), Qt::AlignCenter, stringText );
}


