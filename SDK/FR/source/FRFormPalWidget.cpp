#include "LibInfo.h"
#include "FRFormPalWidget.h"

#include <WStackedWidget.h>
#include <WIconWidget.h>
#include <WIconLayout.h>

#include <DDiagramEditorWidget.h>

FRFormPalWidget::FRFormPalWidget( DDiagramEditorWidget *pEditor, QWidget *pParent )
    : DDiagramPalWidget( pEditor, pParent )
{

    // Widgets
    {
        QWidget *pWidgetTools = new QWidget();
        // pWidgetTools->setStyleSheet( "background-color: lightblue" );
        WIconLayout *pLayout = new WIconLayout;
        pLayout->addWidget( new WIconWidget( tr( "Calendar Widget" ), QPixmap( ":FR/CalendarWidget" ).scaledToHeight( 64 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Check Box" ), QPixmap( ":FR/CheckBox" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Combo Box" ), QPixmap( ":FR/ComboBox" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Date Edit" ), QPixmap( ":FR/DateEdit" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Date Time Edit" ), QPixmap( ":FR/DateTimeEdit" ).scaledToHeight( 64 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Dial" ), QPixmap( ":FR/Dial" ).scaledToHeight( 64 ), pWidgetTools ) );
//        pLayout->addWidget( new WIconWidget( tr( "Dialog Button Box" ), QPixmap( ":FR/DialogButtonBox" ).scaledToHeight( 64 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Double Spin Box" ), QPixmap( ":FR/DoubleSpinBox" ), pWidgetTools ) );
//        pLayout->addWidget( new WIconWidget( tr( "Frame" ), QPixmap( ":FR/Frame" ).scaledToHeight( 64 ), pWidgetTools ) );
//        pLayout->addWidget( new WIconWidget( tr( "Group Box" ), QPixmap( ":FR/GroupBox" ).scaledToHeight( 64 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Horizontal Scroll Bar" ), QPixmap( ":FR/ScrollBarH" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Horizontal Slider" ), QPixmap( ":FR/SliderH" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Label" ), QPixmap( ":FR/Label" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "LCD Number" ), QPixmap( ":FR/LCDNumber" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Line Edit" ), QPixmap( ":FR/LineEdit" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "List Widget" ), QPixmap( ":FR/ListWidget" ).scaledToHeight( 64 ), pWidgetTools ) );
//        pLayout->addWidget( new WIconWidget( tr( "OpenGL Widget" ), QPixmap( ":FR/OpenGLWidget" ).scaledToHeight( 64 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Plain Text Edit" ), QPixmap( ":FR/PlainTextEdit" ).scaledToHeight( 64 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Progress Bar" ), QPixmap( ":FR/ProgressBar" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Push Button" ), QPixmap( ":FR/PushButton" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Radio Button" ), QPixmap( ":FR/RadioButton" ), pWidgetTools ) );
//        pLayout->addWidget( new WIconWidget( tr( "Scroll Area" ), QPixmap( ":FR/ScrollArea" ).scaledToHeight( 64 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Spin Box" ), QPixmap( ":FR/SpinBox" ), pWidgetTools ) );
//        pLayout->addWidget( new WIconWidget( tr( "Stacked Widget" ), QPixmap( ":FR/StackedWidget" ).scaledToHeight( 64 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Table Widget" ), QPixmap( ":FR/TableWidget" ).scaledToHeight( 64 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Tab Widget" ), QPixmap( ":FR/TabWidget" ).scaledToHeight( 64 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Text Browser" ), QPixmap( ":FR/TextBrowser" ).scaledToHeight( 64 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Text Edit" ), QPixmap( ":FR/TextEdit" ).scaledToHeight( 64 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Time Edit" ), QPixmap( ":FR/TimeEdit" ), pWidgetTools ) );
//        pLayout->addWidget( new WIconWidget( tr( "Tool Box" ), QPixmap( ":FR/ToolBox" ).scaledToHeight( 64 ), pWidgetTools ) );
//        pLayout->addWidget( new WIconWidget( tr( "Tool Button" ), QPixmap( ":FR/ToolButton" ).scaledToHeight( 64 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Tree Widget" ), QPixmap( ":FR/TreeWidget" ).scaledToHeight( 64 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Vertical Scroll Bar" ), QPixmap( ":FR/ScrollBarV" ).scaledToHeight( 64 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Vertical Slider" ), QPixmap( ":FR/SliderV" ).scaledToHeight( 64 ), pWidgetTools ) );

        pWidgetTools->setLayout( pLayout );
        pStackedWidget->addWidget( tr( "Widgets" ), pWidgetTools );
    }
}

FRFormPalWidget::~FRFormPalWidget()
{
}


