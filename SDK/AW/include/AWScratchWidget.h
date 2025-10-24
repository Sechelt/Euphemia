#ifndef H_AWScratchWidget
#define H_AWScratchWidget

#include <CBD.h>

#include <QSvgRenderer>

/*!
 * \class AWScratchWidget 
 * \brief A widget for managing 'scratch pads'. To be used in a dock window in similar fashion to AWPanWidget. 
 *  
 * ScratchPads are widgets which can hold bits of a document. The bits are copied from the document and pasted into the ScratchPad. 
 * It is expected that the ScratchPads will be read-only (other than the paste to load it and cut/delete to empty it). 
 * It is also expected that a paste will replace all existing contents (if any). 
 * ScratchPads are NOT saved with the document and have no connection to the document other than its existence. 
 * Closing the document deletes all ScratchPad data. 
 *  
 * \author pharvey (3/25/20)
 */
class AWScratchWidget : public QWidget
{
    Q_OBJECT
public:
    AWScratchWidget( QWidget *pwidgetParent, const QString &stringXML, QXmlStreamReader *pSvg );
    virtual ~AWScratchWidget();

protected:
    QString         stringXML;
    QSvgRenderer *  pRenderer;
};

class AWScratchTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    AWScratchTabWidget( QWidget *pwidgetParent );
    virtual ~AWScratchTabWidget();
    
    virtual void contextMenuEvent( QContextMenuEvent *pEvent );
};

#endif 

