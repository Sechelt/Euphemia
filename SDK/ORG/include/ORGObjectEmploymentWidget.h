#ifndef H_ORGObjectEmploymentWidget
#define H_ORGObjectEmploymentWidget

#include "ORGPerson.h" 

/*!
 * \class ORGObjectEmploymentWidget 
 *  
 * \brief Properties widget for an ORGPerson. 
 * 
 * \author pharvey (10/5/19)
 */
class ORGObjectEmploymentWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ORGObjectEmploymentWidget( ORGPerson *pPerson, QWidget *pWidgetParent );

protected slots:
    void slotTitle()        { pPerson->setTitle( pTitle->text() ); }
    void slotRole()         { pPerson->setRole( pRole->text() ); }
    void slotLocation()     { pPerson->setLocation( pLocation->toPlainText() ); }
    void slotEMail()        { pPerson->setEMail( pEMail->text() ); }
    void slotPhone()        { pPerson->setPhone( pPhone->text() ); }

protected:
    ORGPerson *     pPerson;
    QLineEdit *     pTitle;
    QLineEdit *     pRole;
    QTextEdit *     pLocation;
    QLineEdit *     pEMail;
    QLineEdit *     pPhone;
};

#endif


