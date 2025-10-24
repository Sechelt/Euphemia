#ifndef H_WSplash
#define H_WSplash

#include <CBD.h>

/*!
 * \brief A splash panel to show at app startup. 
 *  
 * This is an alternative to QSplashScreen. 
 * 
 * \author pharvey (2/25/23)
 */
class WSplash : public QDialog
{
    Q_OBJECT
public:
    WSplash( QWidget *pwidgetParent, const QPixmap &pixmap, bool bShowCheckBox = true, bool bShowButton = true, int nDelaySeconds = 5 );

protected:
   QCheckBox *  pShow   = nullptr;
   QPushButton *pOk     = nullptr;

protected slots:
    void slotShow( Qt::CheckState );
};

#endif

