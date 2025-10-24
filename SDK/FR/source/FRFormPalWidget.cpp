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
        pLayout->addWidget( new WIconWidget( tr( "Calendar Widget" ), QPixmap( ":FR/CalendarWidget" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Check Box" ), QPixmap( ":FR/CheckBox" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Combo Box" ), QPixmap( ":FR/ComboBox" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Date Edit" ), QPixmap( ":FR/DateEdit" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Date Time Edit" ), QPixmap( ":FR/DateTimeEdit" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Dial" ), QPixmap( ":FR/Dial" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Dialog Button Box" ), QPixmap( ":FR/DialogButtonBox" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Double Spin Box" ), QPixmap( ":FR/DoubleSpinBox" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Frame" ), QPixmap( ":FR/Frame" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Group Box" ), QPixmap( ":FR/GroupBox" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Horizontal Scroll Bar" ), QPixmap( ":FR/ScrollBarH" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Horizontal Slider" ), QPixmap( ":FR/SliderH" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Label" ), QPixmap( ":FR/Label" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "LCD Number" ), QPixmap( ":FR/LCDNumber" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Line Edit" ), QPixmap( ":FR/LineEdit" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "List Widget" ), QPixmap( ":FR/ListWidget" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "OpenGL Widget" ), QPixmap( ":FR/OpenGLWidget" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Plain Text Edit" ), QPixmap( ":FR/PlainTextEdit" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Progress Bar" ), QPixmap( ":FR/ProgressBar" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Push Button" ), QPixmap( ":FR/PushButton" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Radio Button" ), QPixmap( ":FR/RadioButton" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Scroll Area" ), QPixmap( ":FR/ScrollArea" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Spin Box" ), QPixmap( ":FR/SpinBox" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Stacked Widget" ), QPixmap( ":FR/StackedWidget" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Table Widget" ), QPixmap( ":FR/TableWidget" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Tab Widget" ), QPixmap( ":FR/TabWidget" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Text Browser" ), QPixmap( ":FR/TextBrowser" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Text Edit" ), QPixmap( ":FR/TextEdit" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Time Edit" ), QPixmap( ":FR/TimeEdit" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Tool Box" ), QPixmap( ":FR/ToolBox" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Tool Button" ), QPixmap( ":FR/ToolButton" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Tree Widget" ), QPixmap( ":FR/TreeWidget" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Vertical Scroll Bar" ), QPixmap( ":FR/VerticalScrollBar" ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Vertical Slider" ), QPixmap( ":FR/VerticalSlider" ), pWidgetTools ) );

        pWidgetTools->setLayout( pLayout );
        pStackedWidget->addWidget( tr( "Widgets" ), pWidgetTools );
    }
}

FRFormPalWidget::~FRFormPalWidget()
{
}


