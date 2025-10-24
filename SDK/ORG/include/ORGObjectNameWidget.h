#ifndef H_ORGObjectNameWidget
#define H_ORGObjectNameWidget

#include "ORGPerson.h" 

class WImageButton;

/*!
 * \class ORGObjectNameWidget 
 *  
 * \brief Properties widget for an ADObject. 
 * 
 * \author pharvey (10/5/19)
 */
class ORGObjectNameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ORGObjectNameWidget( ORGPerson *pPerson, QWidget *pWidgetParent );

protected slots:
    void slotPhoto();
    void slotName()     { pPerson->setName( pLineEditName->text() ); }
    void slotCode()     { pPerson->setCode( pLineEditCode->text() ); }
    void slotComment()  { pPerson->setComment( pTextEditComment->toPlainText() ); }

protected:
    ORGPerson *  pPerson;
    WImageButton *  pPhoto;
    QLabel *    pLabelOID;
    QLineEdit * pLineEditName;
    QLineEdit * pLineEditCode;
    QTextEdit * pTextEditComment;
};

#endif


