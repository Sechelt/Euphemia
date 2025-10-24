#ifndef H_AWOutputWidget
#define H_AWOutputWidget

#include <CBD.h>

#include <ADObjectPath.h>
#include <ADValidation.h>
#include <ADObject.h>

class AWOutputMessagesWidget;
class AWOutputSearchWidget;
class AWOutputValidationWidget;

/*!
 * \class AWOutputWidget 
 * \brief Encapsulates the various output widgets. 
 *  
 * A single widget for output that can be used in a dock. 
 * Encapsulates; 'general messages', 'search', and 'validation' output into 
 * a tab widget. Manages message count on tabs. 
 *  
 * \sa AWOutputMessagesWidget
 * \sa AWOutputSearchWidget
 * \sa AWOutputValidationWidget
 *  
 * \author pharvey (1/2/20)
 */
class AWOutputWidget : public QTabWidget
{
    Q_OBJECT
public:
    AWOutputWidget( QWidget *pParent );

    virtual void setFocusFind();
    virtual void setFocusReplace();

    AWOutputMessagesWidget      *getMessages()  { return pMessages;     }
    AWOutputSearchWidget        *getSearch()    { return pSearch;       }
    AWOutputValidationWidget    *getValidation(){ return pValidation;   }

signals:
    void signalFind( const ADFindCriteria &Criteria );
    void signalGoTo( ADObjectPathList listObjectPathItems );
    void signalGoToView( ADObjectPathList listObjectPathItems );

public slots:
    virtual void slotOutput( ADEventOutput *pEvent );
  
protected:
    // tab widgets
    AWOutputMessagesWidget      *pMessages;
    AWOutputSearchWidget        *pSearch;
    AWOutputValidationWidget    *pValidation;
    // new messages can be <= row count (so we can not just use row count)
    int nMessagesCount;
    int nSearchCount;
    int nValidationCount;

protected slots:
    virtual void slotCurrentChanged( int nIndex );
    virtual void slotCurrentClear();
};

#endif
