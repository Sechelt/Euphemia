#include "LibInfo.h"
#include "FRFormPalWidget.h"

#include <WStackedWidget.h>
#include <WIconWidget.h>
#include <WIconLayout.h>

#include <DDiagramEditorWidget.h>

FRFormPalWidget::FRFormPalWidget( DDiagramEditorWidget *pEditor, QWidget *pParent )
    : DDiagramPalWidget( pEditor, pParent )
{

    // Buttons
    {
        QWidget *pWidgetTools = new QWidget();
        // pWidgetTools->setStyleSheet( "background-color: lightblue" );
        WIconLayout *pLayout = new WIconLayout;

        pLayout->addWidget( new WIconWidget( tr( "Push Button" ), QPixmap( ":FR/PushButton" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Radio Button" ), QPixmap( ":FR/RadioButton" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Tool Button" ), QPixmap( ":FR/ToolButton" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Check Box" ), QPixmap( ":FR/CheckBox" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Command Link Button" ), QPixmap( ":FR/CommandLinkButton" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Dialog Button Box" ), QPixmap( ":FR/DialogButtonBox" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );

        pWidgetTools->setLayout( pLayout );
        pStackedWidget->addWidget( tr( "Buttons" ), pWidgetTools );
    }

    // Item Views
    {
        QWidget *pWidgetTools = new QWidget();
        WIconLayout *pLayout = new WIconLayout;

        pLayout->addWidget( new WIconWidget( tr( "List View" ), QPixmap( ":FR/ListView" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Tree View" ), QPixmap( ":FR/TreeView" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Table View" ), QPixmap( ":FR/TableView" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Column View" ), QPixmap( ":FR/ColumnView" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Undo View" ), QPixmap( ":FR/UndoView" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );

        pWidgetTools->setLayout( pLayout );
        pStackedWidget->addWidget( tr( "Item Views" ), pWidgetTools );
    }

    // Item Widgets
    {
        QWidget *pWidgetTools = new QWidget();
        WIconLayout *pLayout = new WIconLayout;

        pLayout->addWidget( new WIconWidget( tr( "List Widget" ), QPixmap( ":FR/ListWidget" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Tree Widget" ), QPixmap( ":FR/TreeWidget" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Table Widget" ), QPixmap( ":FR/TableWidget" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );

        pWidgetTools->setLayout( pLayout );
        pStackedWidget->addWidget( tr( "Item Widgets" ), pWidgetTools );
    }

    // Containers
    {
        QWidget *pWidgetTools = new QWidget();
        WIconLayout *pLayout = new WIconLayout;

        pLayout->addWidget( new WIconWidget( tr( "Group Box" ), QPixmap( ":FR/GroupBox" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Scroll Area" ), QPixmap( ":FR/ScrollArea" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Tool Box" ), QPixmap( ":FR/ToolBox" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Tab Widget" ), QPixmap( ":FR/TabWidget" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Stacked Widget" ), QPixmap( ":FR/StackedWidget" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Frame" ), QPixmap( ":FR/Frame" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Widget" ), QPixmap( ":FR/Widget" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "MDI Area" ), QPixmap( ":FR/MDIArea" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Dock Widget" ), QPixmap( ":FR/DockWidget" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );

        pWidgetTools->setLayout( pLayout );
        pStackedWidget->addWidget( tr( "Containers" ), pWidgetTools );
    }

    // Input Widgets
    {
        QWidget *pWidgetTools = new QWidget();
        WIconLayout *pLayout = new WIconLayout;

        pLayout->addWidget( new WIconWidget( tr( "Combo Box" ), QPixmap( ":FR/ComboBox" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Font Combo Box" ), QPixmap( ":FR/FontComboBox" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Line Edit" ), QPixmap( ":FR/LineEdit" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Text Edit" ), QPixmap( ":FR/TextEdit" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Plain Text Edit" ), QPixmap( ":FR/PlainTextEdit" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Spin Box" ), QPixmap( ":FR/SpinBox" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Double Spin Box" ), QPixmap( ":FR/DoubleSpinBox" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Time Edit" ), QPixmap( ":FR/TimeEdit" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Date Edit" ), QPixmap( ":FR/DateEdit" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Date Time Edit" ), QPixmap( ":FR/DateTimeEdit" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Dial" ), QPixmap( ":FR/Dial" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Horizontal Scroll Bar" ), QPixmap( ":FR/ScrollBarH" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Vertical Scroll Bar" ), QPixmap( ":FR/ScrollBarV" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Horizontal Slider" ), QPixmap( ":FR/SliderH" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Vertical Slider" ), QPixmap( ":FR/SliderV" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Key Sequence Edit" ), QPixmap( ":FR/KeySequenceEdit" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );

        pWidgetTools->setLayout( pLayout );
        pStackedWidget->addWidget( tr( "Input Widgets" ), pWidgetTools );
    }

    // Display Widgets
    {
        QWidget *pWidgetTools = new QWidget();
        WIconLayout *pLayout = new WIconLayout;

        pLayout->addWidget( new WIconWidget( tr( "Label" ), QPixmap( ":FR/Label" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Text Browser" ), QPixmap( ":FR/TextBrowser" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Graphics View" ), QPixmap( ":FR/GraphicsView" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Calendar Widget" ), QPixmap( ":FR/CalendarWidget" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "LCD Number" ), QPixmap( ":FR/LCDNumber" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Progress Bar" ), QPixmap( ":FR/ProgressBar" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Horizontal Line" ), QPixmap( ":FR/HorizontalLine" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Vertical Line" ), QPixmap( ":FR/VerticalLine" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "OpenGL Widget" ), QPixmap( ":FR/OpenGLWidget" ).scaled( QSize( 32, 32 ) ), pWidgetTools ) );

        pWidgetTools->setLayout( pLayout );
        pStackedWidget->addWidget( tr( "Display Widgets" ), pWidgetTools );
    }
}

FRFormPalWidget::~FRFormPalWidget()
{
}


