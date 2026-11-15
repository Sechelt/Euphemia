#ifndef H_LMRelationshipWidget
#define H_LMRelationshipWidget

#include "LMRelationship.h"

class LMRelationshipDetailsWidget : public QWidget
{
    Q_OBJECT
public:
    LMRelationshipDetailsWidget( LMRelationship *pRelationship, CBD::EOLTypes nEOL, QWidget *pParent );

protected slots:
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    void slotDominant( Qt::CheckState n );
    void slotIdentified( Qt::CheckState n );
    void slotDependent( Qt::CheckState n );
#else
    void slotDominant( int n );
    void slotIdentified( int n );
    void slotDependent( int n );
#endif
    void slotCardinalityMin( const QString &s );
    void slotCardinalityMax( const QString &s );
    void slotRole( const QString &s );

    void slotModified();

protected:
    LMRelationship *pRelationship;
    CBD::EOLTypes   nEOL;

    QCheckBox *pCheckBoxDominant; 
    QCheckBox *pCheckBoxIdentified;
    QCheckBox *pCheckBoxDependent;
    QComboBox *pComboBoxCardinalityMin;
    QComboBox *pComboBoxCardinalityMax;
    QLineEdit *pLineEditRole;

};

class LMRelationshipWidget : public QWidget
{
    Q_OBJECT
public:
    LMRelationshipWidget( LMRelationship *pRelationship, Qt::Orientation nOrientation, QWidget *pParent );

protected slots:
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    void slotGenerate( Qt::CheckState n );
#else
    void slotGenerate( int n );
#endif
    void slotModified();

protected:
    LMRelationship *pRelationship;

    QCheckBox *                     pGenerate;
    LMRelationshipDetailsWidget *   pBegin;
    LMRelationshipDetailsWidget *   pEnd;
};

#endif




