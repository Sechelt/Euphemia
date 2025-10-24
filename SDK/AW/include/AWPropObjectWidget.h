#ifndef H_AWPropObjectWidget
#define H_AWPropObjectWidget

#include <CBD.h> 

#include "AWObject.h"

/*!
 * \class AWPropObjectWidget 
 * \brief Widget for editing most basic common AWObject data.  
 *  
 * This is typically added to a \sa AWDataWidget object and shown by a \sa AWDataDialog. 
 * 
 * \sa AWObjectNameWidget
 * \author pharvey (3/27/22)
 */
class AWPropObjectWidget : public QWidget
{
    Q_OBJECT
public:
    AWPropObjectWidget( AWObject *pObject, QWidget *pwidgetParent );

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

protected slots:
    void slotRefresh();
};

#endif


