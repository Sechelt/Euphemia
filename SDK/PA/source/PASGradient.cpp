/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "PASGradient.h"

/****************************************************************
 * PASGradient
 ****************************************************************/
PASGradient::PASGradient()
{
}

void PASGradient::setType( int t )
{
    if ( t == nType ) return;
    nType = t;
    emit signalChanged();
}

void PASGradient::setSpread( QGradient::Spread t )
{
    if ( t == nSpread ) return;
    nSpread = t;
    emit signalChanged();
}

void PASGradient::setSelected( int nType, QGradient::Spread nSpread )
{
    this->nType     = nType;
    this->nSpread   = nSpread;
    emit signalSync();
}

QWidget *PASGradient::getToolBar( QWidget *pParent )
{
    return new PASGradientToolBar( pParent );
}

QWidget *PASGradient::getPanel( QWidget *pParent )
{
    return new PASGradientPanel( pParent );
}

void PASGradient::doSave( QDomDocument *pdomDoc, QDomElement *pdomElem )
{
    QDomElement domElem = pdomDoc->createElement( "PASGradient" );

    domElem.setAttribute( "Type", nType );
    domElem.setAttribute( "Spread", nSpread );

    pdomElem->appendChild( domElem );
}

void PASGradient::doLoad( QDomElement *pdomElem )
{
    nType   = pdomElem->attribute( "Type", QString::number( int(nType) ) ).toInt();
    nSpread = QGradient::Spread( pdomElem->attribute( "Spread", QString::number( int(nSpread) ) ).toInt());
}

/****************************************************************
 * PASGradientToolBar
 ****************************************************************/
PASGradientToolBar::PASGradientToolBar( QWidget *p )
    : QWidget( p )
{
    pSetting = (PASGradient*)g_SSettings->getSetting( "PASGradient" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not load [PASGradient].";
        return;
    }

    QHBoxLayout *pLayout = new QHBoxLayout( this );
    pType = new QComboBox( this );
    doAddType( "Linear",           int(PAGradientFill::StandardGradientLinear) );
    doAddType( "Radial",           int(PAGradientFill::StandardGradientRadial) );
    doAddType( "Conical",          int(PAGradientFill::StandardGradientConical) );
    doAddType( "WarmFlame",        int(QGradient::WarmFlame) );                          
    doAddType( "NightFade",        int(QGradient::NightFade) );                          
    doAddType( "SpringWarmth",     int(QGradient::SpringWarmth) );                       
    doAddType( "JuicyPeach",       int(QGradient::JuicyPeach) );                         
    doAddType( "YoungPassion",     int(QGradient::YoungPassion) );                       
    doAddType( "LadyLips",         int(QGradient::LadyLips) );                           
    doAddType( "SunnyMorning",     int(QGradient::SunnyMorning) );                       
    doAddType( "RainyAshville",    int(QGradient::RainyAshville) );                      
    doAddType( "FrozenDreams",     int(QGradient::FrozenDreams) );                       
    doAddType( "WinterNeva",       int(QGradient::WinterNeva) );                         
    doAddType( "DustyGrass",       int(QGradient::DustyGrass) );                         
    doAddType( "TemptingAzure",    int(QGradient::TemptingAzure) );                      
    doAddType( "HeavyRain",        int(QGradient::HeavyRain) );                          
    doAddType( "AmyCrisp",         int(QGradient::AmyCrisp) );                           
    doAddType( "MeanFruit",        int(QGradient::MeanFruit) );                          
    doAddType( "DeepBlue",         int(QGradient::DeepBlue) );                           
    doAddType( "RipeMalinka",      int(QGradient::RipeMalinka) );                        
    doAddType( "CloudyKnoxville",  int(QGradient::CloudyKnoxville) );                    
    doAddType( "MalibuBeach",      int(QGradient::MalibuBeach) );                        
    doAddType( "NewLife",          int(QGradient::NewLife) );                            
    doAddType( "TrueSunset",       int(QGradient::TrueSunset) );                         
    doAddType( "MorpheusDen",      int(QGradient::MorpheusDen) );                        
    doAddType( "RareWind",         int(QGradient::RareWind) );                           
    doAddType( "NearMoon",         int(QGradient::NearMoon) );                           
    doAddType( "WildApple",        int(QGradient::WildApple) );                          
    doAddType( "SaintPetersburg",  int(QGradient::SaintPetersburg) );                    
    doAddType( "PlumPlate",        int(QGradient::PlumPlate) );                          
    doAddType( "EverlastingSky",   int(QGradient::EverlastingSky) );                     
    doAddType( "HappyFisher",      int(QGradient::HappyFisher) );                        
    doAddType( "Blessing",         int(QGradient::Blessing) );                           
    doAddType( "SharpeyeEagle",    int(QGradient::SharpeyeEagle) );                      
    doAddType( "LadogaBottom",     int(QGradient::LadogaBottom) );                       
    doAddType( "LemonGate",        int(QGradient::LemonGate) );                          
    doAddType( "ItmeoBranding",    int(QGradient::ItmeoBranding) );                      
    doAddType( "ZeusMiracle",      int(QGradient::ZeusMiracle) );                        
    doAddType( "OldHat",           int(QGradient::OldHat) );                             
    doAddType( "StarWine",         int(QGradient::StarWine) );                           
    doAddType( "HappyAcid",        int(QGradient::HappyAcid) );                          
    doAddType( "AwesomePine",      int(QGradient::AwesomePine) );                        
    doAddType( "NewYork",          int(QGradient::NewYork) );                            
    doAddType( "ShyRainbow",       int(QGradient::ShyRainbow) );                         
    doAddType( "MixedHopes",       int(QGradient::MixedHopes) );                         
    doAddType( "FlyHigh",          int(QGradient::FlyHigh) );                            
    doAddType( "StrongBliss",      int(QGradient::StrongBliss) );                        
    doAddType( "FreshMilk",        int(QGradient::FreshMilk) );                          
    doAddType( "SnowAgain",        int(QGradient::SnowAgain) );                          
    doAddType( "FebruaryInk",      int(QGradient::FebruaryInk) );                        
    doAddType( "KindSteel",        int(QGradient::KindSteel) );                          
    doAddType( "SoftGrass",        int(QGradient::SoftGrass) );                          
    doAddType( "GrownEarly",       int(QGradient::GrownEarly) );                         
    doAddType( "SharpBlues",       int(QGradient::SharpBlues) );                         
    doAddType( "ShadyWater",       int(QGradient::ShadyWater) );                         
    doAddType( "DirtyBeauty",      int(QGradient::DirtyBeauty) );                        
    doAddType( "GreatWhale",       int(QGradient::GreatWhale) );                         
    doAddType( "TeenNotebook",     int(QGradient::TeenNotebook) );                       
    doAddType( "PoliteRumors",     int(QGradient::PoliteRumors) );                       
    doAddType( "SweetPeriod",      int(QGradient::SweetPeriod) );                        
    doAddType( "WideMatrix",       int(QGradient::WideMatrix) );                         
    doAddType( "SoftCherish",      int(QGradient::SoftCherish) );                        
    doAddType( "RedSalvation",     int(QGradient::RedSalvation) );                       
    doAddType( "BurningSpring",    int(QGradient::BurningSpring) );                      
    doAddType( "NightParty",       int(QGradient::NightParty) );                         
    doAddType( "SkyGlider",        int(QGradient::SkyGlider) );                          
    doAddType( "HeavenPeach",      int(QGradient::HeavenPeach) );                        
    doAddType( "PurpleDivision",   int(QGradient::PurpleDivision) );                     
    doAddType( "AquaSplash",       int(QGradient::AquaSplash) );                         
    doAddType( "SpikyNaga",        int(QGradient::SpikyNaga) );                          
    doAddType( "LoveKiss",         int(QGradient::LoveKiss) );                           
    doAddType( "CleanMirror",      int(QGradient::CleanMirror) );                        
    doAddType( "PremiumDark",      int(QGradient::PremiumDark) );                        
    doAddType( "ColdEvening",      int(QGradient::ColdEvening) );                        
    doAddType( "CochitiLake",      int(QGradient::CochitiLake) );                        
    doAddType( "SummerGames",      int(QGradient::SummerGames) );                        
    doAddType( "PassionateBed",    int(QGradient::PassionateBed) );                      
    doAddType( "MountainRock",     int(QGradient::MountainRock) );                       
    doAddType( "DesertHump",       int(QGradient::DesertHump) );                         
    doAddType( "JungleDay",        int(QGradient::JungleDay) );                          
    doAddType( "PhoenixStart",     int(QGradient::PhoenixStart) );                       
    doAddType( "OctoberSilence",   int(QGradient::OctoberSilence) );                     
    doAddType( "FarawayRiver",     int(QGradient::FarawayRiver) );                       
    doAddType( "AlchemistLab",     int(QGradient::AlchemistLab) );                       
    doAddType( "OverSun",          int(QGradient::OverSun) );                            
    doAddType( "PremiumWhite",     int(QGradient::PremiumWhite) );                       
    doAddType( "MarsParty",        int(QGradient::MarsParty) );                          
    doAddType( "EternalConstance", int(QGradient::EternalConstance) );                   
    doAddType( "JapanBlush",       int(QGradient::JapanBlush) );                         
    doAddType( "SmilingRain",      int(QGradient::SmilingRain) );                        
    doAddType( "CloudyApple",      int(QGradient::CloudyApple) );                        
    doAddType( "BigMango",         int(QGradient::BigMango) );                           
    doAddType( "HealthyWater",     int(QGradient::HealthyWater) );                       
    doAddType( "AmourAmour",       int(QGradient::AmourAmour) );                         
    doAddType( "RiskyConcrete",    int(QGradient::RiskyConcrete) );                      
    doAddType( "StrongStick",      int(QGradient::StrongStick) );                        
    doAddType( "ViciousStance",    int(QGradient::ViciousStance) );                      
    doAddType( "PaloAlto",         int(QGradient::PaloAlto) );                           
    doAddType( "HappyMemories",    int(QGradient::HappyMemories) );                      
    doAddType( "MidnightBloom",    int(QGradient::MidnightBloom) );                      
    doAddType( "Crystalline",      int(QGradient::Crystalline) );                        
    doAddType( "PartyBliss",       int(QGradient::PartyBliss) );                         
    doAddType( "ConfidentCloud",   int(QGradient::ConfidentCloud) );                     
    doAddType( "LeCocktail",       int(QGradient::LeCocktail) );                         
    doAddType( "RiverCity",        int(QGradient::RiverCity) );                          
    doAddType( "FrozenBerry",      int(QGradient::FrozenBerry) );                        
    doAddType( "ChildCare",        int(QGradient::ChildCare) );                          
    doAddType( "FlyingLemon",      int(QGradient::FlyingLemon) );                        
    doAddType( "NewRetrowave",     int(QGradient::NewRetrowave) );                       
    doAddType( "HiddenJaguar",     int(QGradient::HiddenJaguar) );                       
    doAddType( "AboveTheSky",      int(QGradient::AboveTheSky) );                        
    doAddType( "Nega",             int(QGradient::Nega) );                               
    doAddType( "DenseWater",       int(QGradient::DenseWater) );                         
    doAddType( "Seashore",         int(QGradient::Seashore) );                           
    doAddType( "MarbleWall",       int(QGradient::MarbleWall) );                         
    doAddType( "CheerfulCaramel",  int(QGradient::CheerfulCaramel) );                    
    doAddType( "NightSky",         int(QGradient::NightSky) );                           
    doAddType( "MagicLake",        int(QGradient::MagicLake) );                          
    doAddType( "YoungGrass",       int(QGradient::YoungGrass) );                         
    doAddType( "ColorfulPeach",    int(QGradient::ColorfulPeach) );                      
    doAddType( "GentleCare",       int(QGradient::GentleCare) );                         
    doAddType( "PlumBath",         int(QGradient::PlumBath) );                           
    doAddType( "HappyUnicorn",     int(QGradient::HappyUnicorn) );                       
    doAddType( "AfricanField",     int(QGradient::AfricanField) );                       
    doAddType( "SolidStone",       int(QGradient::SolidStone) );                         
    doAddType( "OrangeJuice",      int(QGradient::OrangeJuice) );                        
    doAddType( "GlassWater",       int(QGradient::GlassWater) );                         
    doAddType( "NorthMiracle",     int(QGradient::NorthMiracle) );                       
    doAddType( "FruitBlend",       int(QGradient::FruitBlend) );                         
    doAddType( "MillenniumPine",   int(QGradient::MillenniumPine) );                     
    doAddType( "HighFlight",       int(QGradient::HighFlight) );                         
    doAddType( "MoleHall",         int(QGradient::MoleHall) );                           
    doAddType( "SpaceShift",       int(QGradient::SpaceShift) );                         
    doAddType( "ForestInei",       int(QGradient::ForestInei) );                         
    doAddType( "RoyalGarden",      int(QGradient::RoyalGarden) );                        
    doAddType( "RichMetal",        int(QGradient::RichMetal) );                          
    doAddType( "JuicyCake",        int(QGradient::JuicyCake) );                          
    doAddType( "SmartIndigo",      int(QGradient::SmartIndigo) );                        
    doAddType( "SandStrike",       int(QGradient::SandStrike) );                         
    doAddType( "NorseBeauty",      int(QGradient::NorseBeauty) );                        
    doAddType( "AquaGuidance",     int(QGradient::AquaGuidance) );                       
    doAddType( "SunVeggie",        int(QGradient::SunVeggie) );                          
    doAddType( "SeaLord",          int(QGradient::SeaLord) );                            
    doAddType( "BlackSea",         int(QGradient::BlackSea) );                           
    doAddType( "GrassShampoo",     int(QGradient::GrassShampoo) );                       
    doAddType( "LandingAircraft",  int(QGradient::LandingAircraft) );                    
    doAddType( "WitchDance",       int(QGradient::WitchDance) );                         
    doAddType( "SleeplessNight",   int(QGradient::SleeplessNight) );                     
    doAddType( "AngelCare",        int(QGradient::AngelCare) );                          
    doAddType( "CrystalRiver",     int(QGradient::CrystalRiver) );                       
    doAddType( "SoftLipstick",     int(QGradient::SoftLipstick) );                       
    doAddType( "SaltMountain",     int(QGradient::SaltMountain) );                       
    doAddType( "PerfectWhite",     int(QGradient::PerfectWhite) );                       
    doAddType( "FreshOasis",       int(QGradient::FreshOasis) );                         
    doAddType( "StrictNovember",   int(QGradient::StrictNovember) );                     
    doAddType( "MorningSalad",     int(QGradient::MorningSalad) );                       
    doAddType( "DeepRelief",       int(QGradient::DeepRelief) );                         
    doAddType( "SeaStrike",        int(QGradient::SeaStrike) );                          
    doAddType( "NightCall",        int(QGradient::NightCall) );                          
    doAddType( "SupremeSky",       int(QGradient::SupremeSky) );                         
    doAddType( "LightBlue",        int(QGradient::LightBlue) );                          
    doAddType( "MindCrawl",        int(QGradient::MindCrawl) );                          
    doAddType( "LilyMeadow",       int(QGradient::LilyMeadow) );                         
    doAddType( "SugarLollipop",    int(QGradient::SugarLollipop) );                      
    doAddType( "SweetDessert",     int(QGradient::SweetDessert) );                       
    doAddType( "MagicRay",         int(QGradient::MagicRay) );                           
    doAddType( "TeenParty",        int(QGradient::TeenParty) );                          
    doAddType( "FrozenHeat",       int(QGradient::FrozenHeat) );                         
    doAddType( "GagarinView",      int(QGradient::GagarinView) );                        
    doAddType( "FabledSunset",     int(QGradient::FabledSunset) );                       
    doAddType( "PerfectBlue",      int(QGradient::PerfectBlue) );                        
    pType->setCurrentIndex( pType->findData( pSetting->getType() ) );
    pLayout->addWidget( pType );
    connect( pType, SIGNAL(activated(int)), SLOT(slotType(int)) );

    pSpread = new QComboBox( this );
    pSpread->addItem( "PadSpread", (int)QGradient::PadSpread ); 
    pSpread->addItem( "RepeatSpread", (int)QGradient::RepeatSpread ); 
    pSpread->addItem( "ReflectSpread", (int)QGradient::ReflectSpread ); 
    pSpread->setCurrentIndex( pSpread->findData( (int)pSetting->getSpread() ) );
    pLayout->addWidget( pSpread );
    connect( pSpread, SIGNAL(activated(int)), SLOT(slotSpread(int)) );

    pLayout->addStretch( 10 );

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void PASGradientToolBar::slotRefresh()
{
    pType->setCurrentIndex( pType->findData( (int)pSetting->getType() ) );
    pSpread->setCurrentIndex( pSpread->findData( (int)pSetting->getSpread() ) );
}

void PASGradientToolBar::slotType( int n )
{
    pSetting->setType( pType->itemData( n, Qt::UserRole ).toInt() );
}

void PASGradientToolBar::slotSpread( int n )
{
    pSetting->setSpread( (QGradient::Spread)n );
}

QPixmap PASGradientToolBar::getSwatch( const QSize size, int nType )
{
    QPixmap pixmap( size );
    QRect r( 0, 0, size.width(), size.height() );
    pixmap.fill( Qt::white );
    QPainter painter( &pixmap );
/*
    painter.setRenderHint( QPainter::Antialiasing, nAntialiasing );                    
    painter.setRenderHint( QPainter::TextAntialiasing, nTextAntialiasing );            
    painter.setRenderHint( QPainter::SmoothPixmapTransform, nSmoothPixmapTransform );  
    painter.setRenderHint( QPainter::LosslessImageRendering, nLosslessImageRendering );
*/

    if ( nType == PAGradientFill::StandardGradientLinear )
    {
        QLinearGradient gradient( QPoint( r.left(), r.center().y() ), QPoint( r.right(),  r.center().y() )  );
        painter.setBrush( gradient );
        painter.setPen( Qt::NoPen );
        painter.drawRoundedRect( r, 2, 2 );
    }
    else if ( nType == PAGradientFill::StandardGradientRadial )
    {
        QRadialGradient gradient( r.center(), r.width() / 2 );
        painter.setBrush( gradient );
        painter.setPen( Qt::NoPen );
        painter.drawRoundedRect( r, 2, 2 );
    }
    else if ( nType == PAGradientFill::StandardGradientConical )
    {
        QConicalGradient gradient( r.center(), 0.0 );
        painter.setBrush( gradient );
        painter.setPen( Qt::NoPen );
        painter.drawRoundedRect( r, 2, 2 );
    }
    else
    {
        QGradient gradient( (QGradient::Preset)nType );
        painter.setBrush( gradient );
        painter.setPen( Qt::NoPen );
        painter.drawRoundedRect( r, 2, 2 );
    }

    return pixmap;
}

void PASGradientToolBar::doAddType( const QString &stringText, int nType )
{
    pType->addItem( QIcon( getSwatch( QSize( 48, 48 ), nType ) ), stringText, nType );
}

/****************************************************************
 * PASGradientPanel
 ****************************************************************/
PASGradientPanel::PASGradientPanel( QWidget *p )
    : QWidget( p )
{
    pSetting = (PASGradient*)g_SSettings->getSetting( "PASGradient" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not load [PASGradient].";
        return;
    }

    QFormLayout *pLayout = new QFormLayout( this );

    pType = new QComboBox( this );
    doAddType( "Linear",           int(PAGradientFill::StandardGradientLinear) );
    doAddType( "Radial",           int(PAGradientFill::StandardGradientRadial) );
    doAddType( "Conical",          int(PAGradientFill::StandardGradientConical) );
    doAddType( "WarmFlame",        int(QGradient::WarmFlame) );                          
    doAddType( "NightFade",        int(QGradient::NightFade) );                          
    doAddType( "SpringWarmth",     int(QGradient::SpringWarmth) );                       
    doAddType( "JuicyPeach",       int(QGradient::JuicyPeach) );                         
    doAddType( "YoungPassion",     int(QGradient::YoungPassion) );                       
    doAddType( "LadyLips",         int(QGradient::LadyLips) );                           
    doAddType( "SunnyMorning",     int(QGradient::SunnyMorning) );                       
    doAddType( "RainyAshville",    int(QGradient::RainyAshville) );                      
    doAddType( "FrozenDreams",     int(QGradient::FrozenDreams) );                       
    doAddType( "WinterNeva",       int(QGradient::WinterNeva) );                         
    doAddType( "DustyGrass",       int(QGradient::DustyGrass) );                         
    doAddType( "TemptingAzure",    int(QGradient::TemptingAzure) );                      
    doAddType( "HeavyRain",        int(QGradient::HeavyRain) );                          
    doAddType( "AmyCrisp",         int(QGradient::AmyCrisp) );                           
    doAddType( "MeanFruit",        int(QGradient::MeanFruit) );                          
    doAddType( "DeepBlue",         int(QGradient::DeepBlue) );                           
    doAddType( "RipeMalinka",      int(QGradient::RipeMalinka) );                        
    doAddType( "CloudyKnoxville",  int(QGradient::CloudyKnoxville) );                    
    doAddType( "MalibuBeach",      int(QGradient::MalibuBeach) );                        
    doAddType( "NewLife",          int(QGradient::NewLife) );                            
    doAddType( "TrueSunset",       int(QGradient::TrueSunset) );                         
    doAddType( "MorpheusDen",      int(QGradient::MorpheusDen) );                        
    doAddType( "RareWind",         int(QGradient::RareWind) );                           
    doAddType( "NearMoon",         int(QGradient::NearMoon) );                           
    doAddType( "WildApple",        int(QGradient::WildApple) );                          
    doAddType( "SaintPetersburg",  int(QGradient::SaintPetersburg) );                    
    doAddType( "PlumPlate",        int(QGradient::PlumPlate) );                          
    doAddType( "EverlastingSky",   int(QGradient::EverlastingSky) );                     
    doAddType( "HappyFisher",      int(QGradient::HappyFisher) );                        
    doAddType( "Blessing",         int(QGradient::Blessing) );                           
    doAddType( "SharpeyeEagle",    int(QGradient::SharpeyeEagle) );                      
    doAddType( "LadogaBottom",     int(QGradient::LadogaBottom) );                       
    doAddType( "LemonGate",        int(QGradient::LemonGate) );                          
    doAddType( "ItmeoBranding",    int(QGradient::ItmeoBranding) );                      
    doAddType( "ZeusMiracle",      int(QGradient::ZeusMiracle) );                        
    doAddType( "OldHat",           int(QGradient::OldHat) );                             
    doAddType( "StarWine",         int(QGradient::StarWine) );                           
    doAddType( "HappyAcid",        int(QGradient::HappyAcid) );                          
    doAddType( "AwesomePine",      int(QGradient::AwesomePine) );                        
    doAddType( "NewYork",          int(QGradient::NewYork) );                            
    doAddType( "ShyRainbow",       int(QGradient::ShyRainbow) );                         
    doAddType( "MixedHopes",       int(QGradient::MixedHopes) );                         
    doAddType( "FlyHigh",          int(QGradient::FlyHigh) );                            
    doAddType( "StrongBliss",      int(QGradient::StrongBliss) );                        
    doAddType( "FreshMilk",        int(QGradient::FreshMilk) );                          
    doAddType( "SnowAgain",        int(QGradient::SnowAgain) );                          
    doAddType( "FebruaryInk",      int(QGradient::FebruaryInk) );                        
    doAddType( "KindSteel",        int(QGradient::KindSteel) );                          
    doAddType( "SoftGrass",        int(QGradient::SoftGrass) );                          
    doAddType( "GrownEarly",       int(QGradient::GrownEarly) );                         
    doAddType( "SharpBlues",       int(QGradient::SharpBlues) );                         
    doAddType( "ShadyWater",       int(QGradient::ShadyWater) );                         
    doAddType( "DirtyBeauty",      int(QGradient::DirtyBeauty) );                        
    doAddType( "GreatWhale",       int(QGradient::GreatWhale) );                         
    doAddType( "TeenNotebook",     int(QGradient::TeenNotebook) );                       
    doAddType( "PoliteRumors",     int(QGradient::PoliteRumors) );                       
    doAddType( "SweetPeriod",      int(QGradient::SweetPeriod) );                        
    doAddType( "WideMatrix",       int(QGradient::WideMatrix) );                         
    doAddType( "SoftCherish",      int(QGradient::SoftCherish) );                        
    doAddType( "RedSalvation",     int(QGradient::RedSalvation) );                       
    doAddType( "BurningSpring",    int(QGradient::BurningSpring) );                      
    doAddType( "NightParty",       int(QGradient::NightParty) );                         
    doAddType( "SkyGlider",        int(QGradient::SkyGlider) );                          
    doAddType( "HeavenPeach",      int(QGradient::HeavenPeach) );                        
    doAddType( "PurpleDivision",   int(QGradient::PurpleDivision) );                     
    doAddType( "AquaSplash",       int(QGradient::AquaSplash) );                         
    doAddType( "SpikyNaga",        int(QGradient::SpikyNaga) );                          
    doAddType( "LoveKiss",         int(QGradient::LoveKiss) );                           
    doAddType( "CleanMirror",      int(QGradient::CleanMirror) );                        
    doAddType( "PremiumDark",      int(QGradient::PremiumDark) );                        
    doAddType( "ColdEvening",      int(QGradient::ColdEvening) );                        
    doAddType( "CochitiLake",      int(QGradient::CochitiLake) );                        
    doAddType( "SummerGames",      int(QGradient::SummerGames) );                        
    doAddType( "PassionateBed",    int(QGradient::PassionateBed) );                      
    doAddType( "MountainRock",     int(QGradient::MountainRock) );                       
    doAddType( "DesertHump",       int(QGradient::DesertHump) );                         
    doAddType( "JungleDay",        int(QGradient::JungleDay) );                          
    doAddType( "PhoenixStart",     int(QGradient::PhoenixStart) );                       
    doAddType( "OctoberSilence",   int(QGradient::OctoberSilence) );                     
    doAddType( "FarawayRiver",     int(QGradient::FarawayRiver) );                       
    doAddType( "AlchemistLab",     int(QGradient::AlchemistLab) );                       
    doAddType( "OverSun",          int(QGradient::OverSun) );                            
    doAddType( "PremiumWhite",     int(QGradient::PremiumWhite) );                       
    doAddType( "MarsParty",        int(QGradient::MarsParty) );                          
    doAddType( "EternalConstance", int(QGradient::EternalConstance) );                   
    doAddType( "JapanBlush",       int(QGradient::JapanBlush) );                         
    doAddType( "SmilingRain",      int(QGradient::SmilingRain) );                        
    doAddType( "CloudyApple",      int(QGradient::CloudyApple) );                        
    doAddType( "BigMango",         int(QGradient::BigMango) );                           
    doAddType( "HealthyWater",     int(QGradient::HealthyWater) );                       
    doAddType( "AmourAmour",       int(QGradient::AmourAmour) );                         
    doAddType( "RiskyConcrete",    int(QGradient::RiskyConcrete) );                      
    doAddType( "StrongStick",      int(QGradient::StrongStick) );                        
    doAddType( "ViciousStance",    int(QGradient::ViciousStance) );                      
    doAddType( "PaloAlto",         int(QGradient::PaloAlto) );                           
    doAddType( "HappyMemories",    int(QGradient::HappyMemories) );                      
    doAddType( "MidnightBloom",    int(QGradient::MidnightBloom) );                      
    doAddType( "Crystalline",      int(QGradient::Crystalline) );                        
    doAddType( "PartyBliss",       int(QGradient::PartyBliss) );                         
    doAddType( "ConfidentCloud",   int(QGradient::ConfidentCloud) );                     
    doAddType( "LeCocktail",       int(QGradient::LeCocktail) );                         
    doAddType( "RiverCity",        int(QGradient::RiverCity) );                          
    doAddType( "FrozenBerry",      int(QGradient::FrozenBerry) );                        
    doAddType( "ChildCare",        int(QGradient::ChildCare) );                          
    doAddType( "FlyingLemon",      int(QGradient::FlyingLemon) );                        
    doAddType( "NewRetrowave",     int(QGradient::NewRetrowave) );                       
    doAddType( "HiddenJaguar",     int(QGradient::HiddenJaguar) );                       
    doAddType( "AboveTheSky",      int(QGradient::AboveTheSky) );                        
    doAddType( "Nega",             int(QGradient::Nega) );                               
    doAddType( "DenseWater",       int(QGradient::DenseWater) );                         
    doAddType( "Seashore",         int(QGradient::Seashore) );                           
    doAddType( "MarbleWall",       int(QGradient::MarbleWall) );                         
    doAddType( "CheerfulCaramel",  int(QGradient::CheerfulCaramel) );                    
    doAddType( "NightSky",         int(QGradient::NightSky) );                           
    doAddType( "MagicLake",        int(QGradient::MagicLake) );                          
    doAddType( "YoungGrass",       int(QGradient::YoungGrass) );                         
    doAddType( "ColorfulPeach",    int(QGradient::ColorfulPeach) );                      
    doAddType( "GentleCare",       int(QGradient::GentleCare) );                         
    doAddType( "PlumBath",         int(QGradient::PlumBath) );                           
    doAddType( "HappyUnicorn",     int(QGradient::HappyUnicorn) );                       
    doAddType( "AfricanField",     int(QGradient::AfricanField) );                       
    doAddType( "SolidStone",       int(QGradient::SolidStone) );                         
    doAddType( "OrangeJuice",      int(QGradient::OrangeJuice) );                        
    doAddType( "GlassWater",       int(QGradient::GlassWater) );                         
    doAddType( "NorthMiracle",     int(QGradient::NorthMiracle) );                       
    doAddType( "FruitBlend",       int(QGradient::FruitBlend) );                         
    doAddType( "MillenniumPine",   int(QGradient::MillenniumPine) );                     
    doAddType( "HighFlight",       int(QGradient::HighFlight) );                         
    doAddType( "MoleHall",         int(QGradient::MoleHall) );                           
    doAddType( "SpaceShift",       int(QGradient::SpaceShift) );                         
    doAddType( "ForestInei",       int(QGradient::ForestInei) );                         
    doAddType( "RoyalGarden",      int(QGradient::RoyalGarden) );                        
    doAddType( "RichMetal",        int(QGradient::RichMetal) );                          
    doAddType( "JuicyCake",        int(QGradient::JuicyCake) );                          
    doAddType( "SmartIndigo",      int(QGradient::SmartIndigo) );                        
    doAddType( "SandStrike",       int(QGradient::SandStrike) );                         
    doAddType( "NorseBeauty",      int(QGradient::NorseBeauty) );                        
    doAddType( "AquaGuidance",     int(QGradient::AquaGuidance) );                       
    doAddType( "SunVeggie",        int(QGradient::SunVeggie) );                          
    doAddType( "SeaLord",          int(QGradient::SeaLord) );                            
    doAddType( "BlackSea",         int(QGradient::BlackSea) );                           
    doAddType( "GrassShampoo",     int(QGradient::GrassShampoo) );                       
    doAddType( "LandingAircraft",  int(QGradient::LandingAircraft) );                    
    doAddType( "WitchDance",       int(QGradient::WitchDance) );                         
    doAddType( "SleeplessNight",   int(QGradient::SleeplessNight) );                     
    doAddType( "AngelCare",        int(QGradient::AngelCare) );                          
    doAddType( "CrystalRiver",     int(QGradient::CrystalRiver) );                       
    doAddType( "SoftLipstick",     int(QGradient::SoftLipstick) );                       
    doAddType( "SaltMountain",     int(QGradient::SaltMountain) );                       
    doAddType( "PerfectWhite",     int(QGradient::PerfectWhite) );                       
    doAddType( "FreshOasis",       int(QGradient::FreshOasis) );                         
    doAddType( "StrictNovember",   int(QGradient::StrictNovember) );                     
    doAddType( "MorningSalad",     int(QGradient::MorningSalad) );                       
    doAddType( "DeepRelief",       int(QGradient::DeepRelief) );                         
    doAddType( "SeaStrike",        int(QGradient::SeaStrike) );                          
    doAddType( "NightCall",        int(QGradient::NightCall) );                          
    doAddType( "SupremeSky",       int(QGradient::SupremeSky) );                         
    doAddType( "LightBlue",        int(QGradient::LightBlue) );                          
    doAddType( "MindCrawl",        int(QGradient::MindCrawl) );                          
    doAddType( "LilyMeadow",       int(QGradient::LilyMeadow) );                         
    doAddType( "SugarLollipop",    int(QGradient::SugarLollipop) );                      
    doAddType( "SweetDessert",     int(QGradient::SweetDessert) );                       
    doAddType( "MagicRay",         int(QGradient::MagicRay) );                           
    doAddType( "TeenParty",        int(QGradient::TeenParty) );                          
    doAddType( "FrozenHeat",       int(QGradient::FrozenHeat) );                         
    doAddType( "GagarinView",      int(QGradient::GagarinView) );                        
    doAddType( "FabledSunset",     int(QGradient::FabledSunset) );                       
    doAddType( "PerfectBlue",      int(QGradient::PerfectBlue) );                        
    pType->setCurrentIndex( pType->findData( pSetting->getType() ) );
    connect( pType, SIGNAL(activated(int)), SLOT(slotType(int)) );

    pSpread = new QComboBox( this );
    pSpread->addItem( "PadSpread", (int)QGradient::PadSpread ); 
    pSpread->addItem( "RepeatSpread", (int)QGradient::RepeatSpread ); 
    pSpread->addItem( "ReflectSpread", (int)QGradient::ReflectSpread ); 
    pSpread->setCurrentIndex( pSpread->findData( (int)pSetting->getSpread() ) );
    connect( pSpread, SIGNAL(activated(int)), SLOT(slotSpread(int)) );

    pLayout->addRow( tr("Type"), pType );
    pLayout->addRow( tr("Spread"), pSpread );

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void PASGradientPanel::slotRefresh()
{
    pType->setCurrentIndex( pType->findData( (int)pSetting->getType() ) );
    pSpread->setCurrentIndex( pSpread->findData( (int)pSetting->getSpread() ) );
}

void PASGradientPanel::slotType( int n )
{
    pSetting->setType( pType->itemData( n, Qt::UserRole ).toInt() );
}

void PASGradientPanel::slotSpread( int n )
{
    pSetting->setSpread( (QGradient::Spread)n );
}

QPixmap PASGradientPanel::getSwatch( const QSize size, int nType )
{
    QPixmap pixmap( size );
    QRect r( 0, 0, size.width(), size.height() );
    pixmap.fill( Qt::white );
    QPainter painter( &pixmap );
/*
    painter.setRenderHint( QPainter::Antialiasing, nAntialiasing );                    
    painter.setRenderHint( QPainter::TextAntialiasing, nTextAntialiasing );            
    painter.setRenderHint( QPainter::SmoothPixmapTransform, nSmoothPixmapTransform );  
    painter.setRenderHint( QPainter::LosslessImageRendering, nLosslessImageRendering );
*/

    if ( nType == PAGradientFill::StandardGradientLinear )
    {
        QLinearGradient gradient( QPoint( r.left(), r.center().y() ), QPoint( r.right(),  r.center().y() )  );
        painter.setBrush( gradient );
        painter.setPen( Qt::NoPen );
        painter.drawRoundedRect( r, 2, 2 );
    }
    else if ( nType == PAGradientFill::StandardGradientRadial )
    {
        QRadialGradient gradient( r.center(), r.width() / 2 );
        painter.setBrush( gradient );
        painter.setPen( Qt::NoPen );
        painter.drawRoundedRect( r, 2, 2 );
    }
    else if ( nType == PAGradientFill::StandardGradientConical )
    {
        QConicalGradient gradient( r.center(), 0.0 );
        painter.setBrush( gradient );
        painter.setPen( Qt::NoPen );
        painter.drawRoundedRect( r, 2, 2 );
    }
    else
    {
        QGradient gradient( (QGradient::Preset)nType );
        painter.setBrush( gradient );
        painter.setPen( Qt::NoPen );
        painter.drawRoundedRect( r, 2, 2 );
    }

    return pixmap;
}

void PASGradientPanel::doAddType( const QString &stringText, int nType )
{
    pType->addItem( QIcon( getSwatch( QSize( 48, 48 ), nType ) ), stringText, nType );
}


