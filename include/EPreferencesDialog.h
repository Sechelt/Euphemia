#ifndef H_EPreferencesDialog
#define H_EPreferencesDialog

#include <WColorButton.h>
#include <WBrushStyleComboBox.h>
#include <WImageButton.h>

#include <PContext.h>

class EPreferencesDialog : public QDialog
{
    Q_OBJECT
public:
    EPreferencesDialog( QWidget *pParent = nullptr );

    static void doPreferences( QWidget *pParent );
};

class PGeneralConfig : public QWidget
{
    Q_OBJECT
public:
    PGeneralConfig( QWidget *pParent );

public slots:
    void slotRefresh( const PContextGeneral & );

protected:
    // brushBackground
    WColorButton *          pColor;         /*!< a transparent color (alpha==0) and no texture will implicitly indicate block pattern  */
    WBrushStyleComboBox *   pStyle;
    WImageButton *          pImage;

    QCheckBox *             pRestoreState;      
    QCheckBox *             pAutoCommit;      

protected slots:
    void slotColor( const QColor &color );
    void slotStyle( Qt::BrushStyle n );
    void slotImage();
    void slotRestoreState( int );
    void slotAutoCommit( int );
};

#endif
