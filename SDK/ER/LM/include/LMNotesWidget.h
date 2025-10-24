#ifndef H_LMNotesWidget
#define H_LMNotesWidget

#include "LMObject.h"

class LMNotesWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit LMNotesWidget( QWidget *pParent, const QString &stringDescription, const QString &stringAnnotation );

signals:
    void signalDescriptionChanged( const QString &s );
    void signalAnnotationChanged( const QString &s );

protected slots:
    void slotDescription();
    void slotAnnotation();

protected:
    QTextEdit * pTextEditDescription;
    QTextEdit * pTextEditAnnotation;
};


#endif


