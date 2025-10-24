#ifndef H_ORGObjectProjectWidget
#define H_ORGObjectProjectWidget

#include "ORGPerson.h" 

/*!
 * \class ORGObjectProjectWidget 
 *  
 * \brief Properties widget for an ORGPerson. 
 * 
 * \author pharvey (10/5/19)
 */
class ORGObjectProjectWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ORGObjectProjectWidget( ORGPerson *pPerson, QWidget *pWidgetParent );

protected slots:
    void slotProjectRole() { pPerson->setProjectRole( pProjectRole->text() ); }

protected:
    ORGPerson *     pPerson;
    QLineEdit *     pProjectRole;
    QTableWidget *  pEvents;
};

#endif


