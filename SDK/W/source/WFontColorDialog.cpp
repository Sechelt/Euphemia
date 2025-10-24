#include "LibInfo.h"
#include "WFontColorDialog.h"

WFontColorDialog::WFontColorDialog( const CBD::CBDText &font, QWidget *pParent )
    : QDialog( pParent )
{
    setWindowTitle( tr("Font Color") );

    QVBoxLayout *pLayout = new QVBoxLayout( this );
    pWidget = new WFontColorWidget( font, this );
    pLayout->addWidget( pWidget );

    QDialogButtonBox *pButtonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this );
    pLayout->addWidget( pButtonBox ); 

    connect( pButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept );
    connect( pButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject );
}

CBD::CBDText WFontColorDialog::getValue( bool *pOk, const CBD::CBDText &font, QWidget *pParent )
{
    *pOk = true;
    WFontColorDialog dialog( font, pParent );
    if ( dialog.exec() == QDialog::Accepted )
        return dialog.getValue();

    *pOk = false;
    return font;
}


