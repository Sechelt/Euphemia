#ifndef H_AWObjectNameWidget
#define H_AWObjectNameWidget

#include "AWObject.h" 

/*!
 * \class AWObjectNameWidget 
 *  
 * \brief Properties widget for an ADObject. 
 * 
 * \sa AWPropObjectWidget
 * \author pharvey (10/5/19)
 */
class AWObjectNameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AWObjectNameWidget( AWObject *pObject, QWidget *pWidgetParent );

protected slots:
    void slotName()     { pObject->setName( pLineEditName->text() ); }
    void slotCode()     { pObject->setCode( pLineEditCode->text() ); }
    void slotComment()  { pObject->setComment( pTextEditComment->toPlainText() ); }

protected:
    AWObject *  pObject;
    QLabel *    pLabelOID;
    QLineEdit * pLineEditName;
    QLineEdit * pLineEditCode;
    QTextEdit * pTextEditComment;
};

#endif


