#include "LibInfo.h"
#include "FRForm.h"

#include <DGraphicsSceneProxy.h>

#include "FRGraphicsProxyObject.h"

#include "FRFormEditorWidget.h"

#include "FRCalendarWidget.h" 
#include "FRCheckBox.h" 
#include "FRComboBox.h" 
#include "FRDateEdit.h" 
#include "FRDateTimeEdit.h" 
#include "FRDial.h" 
#include "FRDialogButtonBox.h" 
#include "FRDoubleSpinBox.h" 
#include "FRFrame.h" 
#include "FRGroupBox.h" 
#include "FRHorizontalScrollBar.h" 
#include "FRHorizontalSlider.h" 
#include "FRLabel.h" 
#include "FRLCDNumber.h" 
#include "FRLineEdit.h" 
#include "FRListWidget.h" 
#include "FROpenGLWidget.h" 
#include "FRPlainTextEdit.h" 
#include "FRProgressBar.h" 
#include "FRPushButton.h" 
#include "FRRadioButton.h" 
#include "FRScrollArea.h" 
#include "FRSpinBox.h" 
#include "FRStackedWidget.h" 
#include "FRTableWidget.h" 
#include "FRTabWidget.h" 
#include "FRTextBrowser.h" 
#include "FRTextEdit.h" 
#include "FRTimeEdit.h" 
#include "FRToolBox.h" 
#include "FRToolButton.h" 
#include "FRTreeWidget.h" 
#include "FRVerticalScrollBar.h" 
#include "FRVerticalSlider.h"

FRForm::FRForm( ADObject *pParent, const QString &stringName )
    : DDiagram( pParent, stringName )
{
    // widgets
    mapMeta.insert( "FRCalendarWidget", ADMeta( QIcon(":FR/CalendarWidget"), "FRCalendarWidget", "Calendar Widget" ) );
    mapMeta.insert( "FRCheckBox", ADMeta( QIcon(":FR/CheckBox"), "FRCheckBox", "Check Box" ) );
    mapMeta.insert( "FRComboBox", ADMeta( QIcon(":FR/ComboBox"), "FRComboBox", "Combo Box" ) );
    mapMeta.insert( "FRDateEdit", ADMeta( QIcon(":FR/DateEdit"), "FRDateEdit", "Date Edit" ) );
    mapMeta.insert( "FRDateTimeEdit", ADMeta( QIcon(":FR/DateTimeEdit"), "FRDateTimeEdit", "Date Time Edit" ) );
    mapMeta.insert( "FRDial", ADMeta( QIcon(":FR/Dial"), "FRDial", "Dial" ) );
    mapMeta.insert( "FRDialogButtonBox", ADMeta( QIcon(":FR/DialogButtonBox"), "FRDialogButtonBox", "Dialog Button Box" ) );
    mapMeta.insert( "FRDoubleSpinBox", ADMeta( QIcon(":FR/DoubleSpinBox"), "FRDoubleSpinBox", "Double Spin Box" ) );
    mapMeta.insert( "FRFrame", ADMeta( QIcon(":FR/Frame"), "FRFrame", "Frame" ) );
    mapMeta.insert( "FRGroupBox", ADMeta( QIcon(":FR/GroupBox"), "FRGroupBox", "Group Box" ) );
    mapMeta.insert( "FRHorizontalScrollBar", ADMeta( QIcon(":FR/ScrollBarH"), "FRHorizontalScrollBar", "Horizontal Scroll Bar" ) );
    mapMeta.insert( "FRHorizontalSlider", ADMeta( QIcon(":FR/SliderH"), "FRHorizontalSlider", "Horizontal Slider" ) );
    mapMeta.insert( "FRLabel", ADMeta( QIcon(":FR/Label"), "FRLabel", "Label" ) );
    mapMeta.insert( "FRLCDNumber", ADMeta( QIcon(":FR/LCDNumber"), "FRLCDNumber", "LCD Number" ) );
    mapMeta.insert( "FRLineEdit", ADMeta( QIcon(":FR/LineEdit"), "FRLineEdit", "Line Edit" ) );
    mapMeta.insert( "FRListWidget", ADMeta( QIcon(":FR/ListWidget"), "FRListWidget", "List Widget" ) );
    mapMeta.insert( "FROpenGLWidget", ADMeta( QIcon(":FR/OpenGLWidget"), "FROpenGLWidget", "OpenGL Widget" ) );
    mapMeta.insert( "FRPlainTextEdit", ADMeta( QIcon(":FR/PlainTextEdit"), "FRPlainTextEdit", "Plain Text Edit" ) );
    mapMeta.insert( "FRProgressBar", ADMeta( QIcon(":FR/ProgressBar"), "FRProgressBar", "Progress Bar" ) );
    mapMeta.insert( "FRPushButton", ADMeta( QIcon(":FR/PushButton"), "FRPushButton", "Push Button" ) );
    mapMeta.insert( "FRRadioButton", ADMeta( QIcon(":FR/RadioButton"), "FRRadioButton", "Radio Button" ) );
    mapMeta.insert( "FRScrollArea", ADMeta( QIcon(":FR/ScrollArea"), "FRScrollArea", "Scroll Area" ) );
    mapMeta.insert( "FRSpinBox", ADMeta( QIcon(":FR/SpinBox"), "FRSpinBox", "Spin Box" ) );
    mapMeta.insert( "FRStackedWidget", ADMeta( QIcon(":FR/StackedWidget"), "FRStackedWidget", "Stacked Widget" ) );
    mapMeta.insert( "FRTableWidget", ADMeta( QIcon(":FR/TableWidget"), "FRTableWidget", "Table Widget" ) );
    mapMeta.insert( "FRTabWidget", ADMeta( QIcon(":FR/TabWidget"), "FRTabWidget", "Tab Widget" ) );
    mapMeta.insert( "FRTextBrowser", ADMeta( QIcon(":FR/TextBrowser"), "FRTextBrowser", "Text Browser" ) );
    mapMeta.insert( "FRTextEdit", ADMeta( QIcon(":FR/TextEdit"), "FRTextEdit", "Text Edit" ) );
    mapMeta.insert( "FRTimeEdit", ADMeta( QIcon(":FR/TimeEdit"), "FRTimeEdit", "Time Edit" ) );
    mapMeta.insert( "FRToolBox", ADMeta( QIcon(":FR/ToolBox"), "FRToolBox", "Tool Box" ) );
    mapMeta.insert( "FRToolButton", ADMeta( QIcon(":FR/ToolButton"), "FRToolButton", "Tool Button" ) );
    mapMeta.insert( "FRTreeWidget", ADMeta( QIcon(":FR/TreeWidget"), "FRTreeWidget", "Tree Widget" ) );
    mapMeta.insert( "FRVerticalScrollBar", ADMeta( QIcon(":FR/VerticalScrollBar"), "FRVerticalScrollBar", "Vertical Scroll Bar" ) );
    mapMeta.insert( "FRVerticalSlider", ADMeta( QIcon(":FR/VerticalSlider"), "FRVerticalSlider", "Vertical Slider" ) );

    pen = QPen();
    pen.setWidth( 0 );
    brush = QBrush();
}

FRForm::~FRForm()
{
}

QPixmap FRForm::getIcon()
{
    return QPixmap( ":FR/Form" );
}

ADObject *FRForm::getObject( const QString &s, ADObject * )
{
    FRObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    DGraphicsProxyItem *pProxy = nullptr;

    if ( stringClass == "FRCalendarWidget" )
    {
        p = new FRCalendarWidget( this );
    }
    else if ( stringClass == "FRCheckBox" )
    {
        p = new FRCheckBox( this );
    }
    else if ( stringClass == "FRComboBox" )
    {
        p = new FRComboBox( this );
    }
    else if ( stringClass == "FRDateEdit" )
    {
        p = new FRDateEdit( this );
    }
    else if ( stringClass == "FRDateTimeEdit" )
    {
        p = new FRDateTimeEdit( this );
    }
    else if ( stringClass == "FRDial" )
    {
        p = new FRDial( this );
    }
    else if ( stringClass == "FRDialogButtonBox" )
    {
        p = new FRDialogButtonBox( this );
    }
    else if ( stringClass == "FRDoubleSpinBox" )
    {
        p = new FRDoubleSpinBox( this );
    }
    else if ( stringClass == "FRFrame" )
    {
        p = new FRFrame( this );
    }
    else if ( stringClass == "FRGroupBox" )
    {
        p = new FRGroupBox( this );
    }
    else if ( stringClass == "FRHorizontalScrollBar" )
    {
        p = new FRHorizontalScrollBar( this );
    }
    else if ( stringClass == "FRHorizontalSlider" )
    {
        p = new FRHorizontalSlider( this );
    }
    else if ( stringClass == "FRLabel" )
    {
        p = new FRLabel( this );
    }
    else if ( stringClass == "FRLCDNumber" )
    {
        p = new FRLCDNumber( this );
    }
    else if ( stringClass == "FRLineEdit" )
    {
        p = new FRLineEdit( this );
    }
    else if ( stringClass == "FRListWidget" )
    {
        p = new FRListWidget( this );
    }
    else if ( stringClass == "FROpenGLWidget" )
    {
        p = new FROpenGLWidget( this );
    }
    else if ( stringClass == "FRPlainTextEdit" )
    {
        p = new FRPlainTextEdit( this );
    }
    else if ( stringClass == "FRProgressBar" )
    {
        p = new FRProgressBar( this );
    }
    else if ( stringClass == "FRPushButton" )
    {
        p = new FRPushButton( this );
    }
    else if ( stringClass == "FRRadioButton" )
    {
        p = new FRRadioButton( this );
    }
    else if ( stringClass == "FRScrollArea" )
    {
        p = new FRScrollArea( this );
    }
    else if ( stringClass == "FRSpinBox" )
    {
        p = new FRSpinBox( this );
    }
    else if ( stringClass == "FRStackedWidget" )
    {
        p = new FRStackedWidget( this );
    }
    else if ( stringClass == "FRTableWidget" )
    {
        p = new FRTableWidget( this );
    }
    else if ( stringClass == "FRTabWidget" )
    {
        p = new FRTabWidget( this );
    }
    else if ( stringClass == "FRTextBrowser" )
    {
        p = new FRTextBrowser( this );
    }
    else if ( stringClass == "FRTextEdit" )
    {
        p = new FRTextEdit( this );
    }
    else if ( stringClass == "FRTimeEdit" )
    {
        p = new FRTimeEdit( this );
    }
    else if ( stringClass == "FRToolBox" )
    {
        p = new FRToolBox( this );
    }
    else if ( stringClass == "FRToolButton" )
    {
        p = new FRToolButton( this );
    }
    else if ( stringClass == "FRTreeWidget" )
    {
        p = new FRTreeWidget( this );
    }
    else if ( stringClass == "FRVerticalScrollBar" )
    {
        p = new FRVerticalScrollBar( this );
    }
    else if ( stringClass == "FRVerticalSlider" )
    {
        p = new FRVerticalSlider( this );
    }
    
    if ( !p ) return DDiagram::getObject( stringClass );

    // provide a two-way interface to the scene via a proxy
    pProxy = new FRGraphicsProxyObject( p );
    pProxy->doCreateGraphicsObject();
    pProxy->setScene( pScene );
    p->setProxy( pProxy );
    p->setPos( QPointF( pScene->width() / 2, pScene->height() / 2 ) ); // to pos when created by AWFactoryWidget

    // announce the birth of a new child
    emit signalCreated( p );    
    emit signalCreated( this, p );
    // set parent modified by default (can be set back if this is a load)           
    setModified();              

    // the caller should doConnect to a model ASAP
    return (ADObject*)p;
}

bool FRForm::slotOpenEditor()
{
    if ( pEditorWidget ) return true;

    // create view
    FRFormEditorWidget *p;
    pEditorWidget = p = new FRFormEditorWidget( this, getEditorWidgetParent() );

    if ( isCrossHairs() ) doCreateCrossHairs();

    emit signalEditorOpened( pEditorWidget );
    emit signalEditorOpened( this, pEditorWidget );

    return true;
}

/*!
 * \brief Return the proxy item. 
 *  
 * We override DDiagram::getProxy() because we need to account for FRGraphicsObject. 
 * 
 * \author pharvey (11/27/20)
 * 
 * \param p 
 * 
 * \return DGraphicsProxyItem* 
 */
DGraphicsProxyItem *FRForm::getProxy( QGraphicsObject *p )
{
    if ( !p ) return nullptr;
    if ( p->inherits( "DGraphicsObject" ) ) return ((DGraphicsObject*)p)->getProxy();
    if ( p->inherits( "FRGraphicsObject" ) ) return ((FRGraphicsObject*)p)->getProxy();

    return nullptr;
}

