#ifndef H_PMSequenceWidget
#define H_PMSequenceWidget

#include "PMSequence.h"

class PMSequenceWidget : public QWidget
{
    Q_OBJECT
public:
    PMSequenceWidget( PMSequence *pSequence, QWidget *pParent );

protected:
    PMSequence *pSequence;

protected slots:
    void slotIncChanged( const QString &s );
    void slotMinChanged( const QString &s );
    void slotMaxChanged( const QString &s );
    void slotStartChanged( const QString &s );
    void slotCacheChanged( const QString &s );
    void slotCycleChanged( const QString &s );
};

#endif

