#include "LibInfo.h"
#include "DATAWTableView.h"

#include "DATAWTableModel.h"

DATAWTableView::DATAWTableView( QWidget *pParent )
: QTableView( pParent )
{
    setObjectName( metaObject()->className() );
    QHeaderView *p = horizontalHeader();
    connect( p, SIGNAL(sectionClicked(int)), SLOT(slotHeaderSectionClicked(int)) );
}

void DATAWTableView::keyPressEvent( QKeyEvent *pEvent )
{
    QAbstractItemView::State nState = state();

    // Return = commit cell-edit 
    // Esc = abort cell-edit 
    QTableView::keyPressEvent( pEvent );

    if ( !model()->inherits( "DATAWTableModel" ) )
        printf( "[PAH][%s][%s][%d] ERROR: Was expecting model to be derived from DATAWTableModel.\n", __FILE__, __FUNCTION__, __LINE__ );
    DATAWTableModel *p = (DATAWTableModel*)model();

    if ( pEvent->key() == Qt::Key_Return && nState != QAbstractItemView::EditingState )
    {
        // Return = commit row-edits
        if ( p->isEdit() ) p->doEditCommit();
    }
    else if ( pEvent->key() == Qt::Key_Escape )
    {
        // Esc = abort row-edits 
        if ( p->isEdit() ) p->doEditRevert();
    }
}

void DATAWTableView::slotHeaderSectionClicked( int n )
{
    nIndex = n;
    point = QPoint( horizontalHeader()->sectionViewportPosition( nIndex ), 0 );

    QMenu menu( this );
    QAction *pAction;

    pAction = new QAction( "Filter...", &menu );
    connect( pAction, SIGNAL(triggered()), SLOT(slotFilter()) );

    menu.addAction( pAction );
    menu.addAction( new QAction( "Sort Ascending", &menu) );
    menu.addAction( new QAction( "Sort Descennding", &menu) );
    menu.exec( mapToGlobal( point ) );
}

void DATAWTableView::slotFilter()
{
    bool bOk = false;
    QString s = QInputDialog::getText( this, tr("Filter..."), tr("Value:"), QLineEdit::Normal, "", &bOk );
    if ( !bOk ) return;
    DATAWTableModel *pModel = (DATAWTableModel*)model();
    pModel->setFilter( nIndex, s );
}
