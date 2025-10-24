#include "AppInfo.h"
#include "SANewDialog.h"

#include <WIconLayout.h>

SANewWidget::SANewWidget( QWidget *pParent, bool bPhysicalModels )
: QWidget( pParent )
{
    WIconLayout *pLayout = new WIconLayout( this, 20, 20, 20 );
    WIconWidget *p;

    // SQL Editor
    p = new WIconWidget( tr( "SQL" ), QIcon( ":DA/SQLEditor" ).pixmap( 64, 128 ), this, tr("SQL Editor.") );
    connect( p, SIGNAL(signalClicked(WIconWidget *)), SLOT(slotClicked(WIconWidget *)) );
    vectorIcons.append( p );
    pLayout->addWidget( p );

    if ( !bPhysicalModels )
    {
        // diagram
        p = new WIconWidget( tr( "Diagram" ), QIcon( ":DA/Diagram" ).pixmap( 64, 128 ), this, tr("Generic diagram.") );
        connect( p, SIGNAL(signalClicked(WIconWidget *)), SLOT(slotClicked(WIconWidget *)) );
        vectorIcons.append( p );
        pLayout->addWidget( p );
        // Organization Chart
        p = new WIconWidget( tr( "Organization Chart" ), QIcon( ":DA/OrgChart" ).pixmap( 64, 128 ), this, tr("Specify reporting structure.") );
        connect( p, SIGNAL(signalClicked(WIconWidget *)), SLOT(slotClicked(WIconWidget *)) );
        vectorIcons.append( p );
        pLayout->addWidget( p );
        // Form
        p = new WIconWidget( tr( "Form" ), QIcon( ":DA/Form" ).pixmap( 64, 128 ), this, tr("Form/Screen for 'storyboard' or Page for reports.") );
        connect( p, SIGNAL(signalClicked(WIconWidget *)), SLOT(slotClicked(WIconWidget *)) );
        vectorIcons.append( p );
        pLayout->addWidget( p );
        // logic
        p = new WIconWidget( tr( "Logic" ), QIcon( ":DA/Logic" ).pixmap( 64, 128 ), this, tr("Specify Logic using a diagram.") );
        connect( p, SIGNAL(signalClicked(WIconWidget *)), SLOT(slotClicked(WIconWidget *)) );
        vectorIcons.append( p );
        pLayout->addWidget( p );
        // ER Logical Model
        p = new WIconWidget( tr( "ER: Logical Model" ), QIcon( ":DA/ER-LogicalModel" ).pixmap( 64, 128 ), this, tr("Entity Relationships - Logical Model") );
        connect( p, SIGNAL(signalClicked(WIconWidget *)), SLOT(slotClicked(WIconWidget *)) );
        vectorIcons.append( p );
        pLayout->addWidget( p );
    }
    // ER SQL92 Model
    p = new WIconWidget( tr( "ER: SQL92" ), QIcon( ":DA/ER-PhysicalModel" ).pixmap( 64, 128 ), this, tr("Entity Relationships - Physical Model based upon SQL92 specification.") );
    connect( p, SIGNAL(signalClicked(WIconWidget *)), SLOT(slotClicked(WIconWidget *)) );
    vectorIcons.append( p );
    pLayout->addWidget( p );
    // ER ODBC Model
    p = new WIconWidget( tr( "ER: ODBC Minimum" ), QIcon( ":DA/ER-PhysicalModel" ).pixmap( 64, 128 ), this, tr("Entity Relationships - Physical Model based upon ODBC (Minimum) specification.") );
    connect( p, SIGNAL(signalClicked(WIconWidget *)), SLOT(slotClicked(WIconWidget *)) );
    vectorIcons.append( p );
    pLayout->addWidget( p );
    // ER ODBC Model
    p = new WIconWidget( tr( "ER: ODBC Core" ), QIcon( ":DA/ER-PhysicalModel" ).pixmap( 64, 128 ), this, tr("Entity Relationships - Physical Model based upon ODBC (Core) specification.") );
    connect( p, SIGNAL(signalClicked(WIconWidget *)), SLOT(slotClicked(WIconWidget *)) );
    vectorIcons.append( p );
    pLayout->addWidget( p );
    // ER ODBC Model
    p = new WIconWidget( tr( "ER: ODBC Extended" ), QIcon( ":DA/ER-PhysicalModel" ).pixmap( 64, 128 ), this, tr("Entity Relationships - Physical Model based upon ODBC (Extended) specification.") );
    connect( p, SIGNAL(signalClicked(WIconWidget *)), SLOT(slotClicked(WIconWidget *)) );
    vectorIcons.append( p );
    pLayout->addWidget( p );
    // ER ODBC Model
    p = new WIconWidget( tr( "ER: ODBC Driver" ), QIcon( ":DA/ER-PhysicalModel" ).pixmap( 64, 128 ), this, tr("Entity Relationships - Physical Model based upon querying features of an ODBC driver.") );
    connect( p, SIGNAL(signalClicked(WIconWidget *)), SLOT(slotClicked(WIconWidget *)) );
    vectorIcons.append( p );
    pLayout->addWidget( p );
    // ER MS Access
    p = new WIconWidget( tr( "ER: MS Access" ), QIcon( ":DA/ER-PhysicalModel" ).pixmap( 64, 128 ), this, tr("Entity Relationships - Physical Model with knowledge of MS Access.") );
    connect( p, SIGNAL(signalClicked(WIconWidget *)), SLOT(slotClicked(WIconWidget *)) );
    vectorIcons.append( p );
    pLayout->addWidget( p );
    // ER SQLite
    p = new WIconWidget( tr( "ER: SQLite" ), QIcon( ":DA/ER-PhysicalModel" ).pixmap( 64, 128 ), this, tr("Entity Relationships - Physical Model with knowledge of SQLite.") );
    connect( p, SIGNAL(signalClicked(WIconWidget *)), SLOT(slotClicked(WIconWidget *)) );
    vectorIcons.append( p );
    pLayout->addWidget( p );
}

QString SANewWidget::getSelected()
{
    WIconWidget *pIcon;
    foreach( pIcon, vectorIcons )
    {
        if ( pIcon->isSelected() ) return pIcon->getText();
    }
    return QString();
}


void SANewWidget::slotClicked( WIconWidget *p )
{
    WIconWidget *pIcon;
    foreach( pIcon, vectorIcons )
    {
        if ( pIcon != p && pIcon->isSelected() ) pIcon->setSelected( false ); // not p should be unselected
        else if ( pIcon == p ) pIcon->setSelected( !pIcon->isSelected() ); // toggle p
    }
}

SANewDialog::SANewDialog( QWidget *pWidgetParent, bool bPhysicalModels )
    : QDialog( pWidgetParent )
{
    setModal( true );
    setSizeGripEnabled( true );
    setObjectName( metaObject()->className() );
    setWindowIcon( QPixmap( ":New" ) );
    setWindowTitle( "New..." );

    QVBoxLayout *playoutTop = new QVBoxLayout;
    // top bit (widget)
    pNewWidget = new SANewWidget( this, bPhysicalModels );
    playoutTop->addWidget( pNewWidget );
    // bottom bit (button)
    QDialogButtonBox *pButtonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this );
    playoutTop->addWidget( pButtonBox );

    connect( pButtonBox, SIGNAL(accepted()), this, SLOT(slotOk()) );
    connect( pButtonBox, SIGNAL(rejected()), this, SLOT(slotRejected()) );

    setLayout( playoutTop );

    doLoadState();
}

void SANewDialog::doLoadState()
{
    QString s = objectName() + "/";
    QSettings settings;
    restoreGeometry( settings.value( s + "geometry" ).toByteArray() );
}

void SANewDialog::doSaveState()
{
    QString s = objectName() + "/";
    QSettings settings;
    settings.setValue( s + "geometry", saveGeometry() );
}

void SANewDialog::slotOk()
{
    // save changes here

    doSaveState();
    done( QDialog::Accepted );
}

void SANewDialog::slotRejected()
{
    doSaveState();
    done( QDialog::Rejected );
}


