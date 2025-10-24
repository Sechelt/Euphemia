#ifndef H_AWDocumentNameWidget
#define H_AWDocumentNameWidget

#include "AWFile.h"

/*!
 * \class AWDocumentNameWidget 
 * \brief Combines data from several sources and allows viewing and editing. For those that inherit \sa ADDocument.
 *  
 * This combines the following data; 
 * - Name, Code, Comment and OID from \sa ADObject 
 * - all data from \sa ADDocument 
 * - file name will be displayed if not null
 *  
 * \sa ADDocument 
 *  
 * \author pharvey (12/27/19)
 */
class AWDocumentNameWidget : public QWidget
{
    Q_OBJECT
public:
    AWDocumentNameWidget( ADDocument *pDocument, QWidget *pwidgetParent, const QString &stringFileName = QString() );
    virtual ~AWDocumentNameWidget();

protected slots:                                   
    virtual void slotName( const QString & );   
    virtual void slotCode( const QString & );   
    virtual void slotComment();                 
    virtual void slotAuthor( const QString & ); 
    virtual void slotVersion( const QString & );
                                                
protected:
    ADDocument *pDocument;

    QLabel *    plabelOID;
    QLineEdit * plineeditName;
    QLineEdit * pCode;
    QTextEdit * ptexteditComment;
    QLineEdit * plineeditAuthor;
    QLineEdit * plineeditVersion;
    QLabel *    pCreatedDate;
    QLabel *    pModifiedDate;
    QLabel *    plabelFileName;
};

#endif


