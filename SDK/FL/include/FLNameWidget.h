#ifndef H_FLNameWidget
#define H_FLNameWidget

#include "FLRectangle.h" 

/*!
 * \class FLNameWidget 
 *  
 * \brief Properties widget for an FLRectangle. 
 * 
 * \author pharvey (10/5/19)
 */
class FLNameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FLNameWidget( FLRectangle *pRectangle, QWidget *pWidgetParent );

protected slots:
    void slotName()     { pRectangle->setName( pName->text() ); }
    void slotCode()     { pRectangle->setCode( pCode->text() ); }
    void slotComment()  { pRectangle->setComment( pComment->toPlainText() ); }
    void slotMacro()    { pRectangle->setMacro( pMacro->toPlainText() ); }

protected:
    FLRectangle *   pRectangle;
    QLabel *        pOID;
    QLineEdit *     pName;
    QLineEdit *     pCode;
    QTextEdit *     pComment;
    QTextEdit *     pMacro;
};

#endif


