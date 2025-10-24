#ifndef H_LMRelationshipWidget
#define H_LMRelationshipWidget

#include "LMRelationship.h"

class LMRelationshipDetailsWidget : public QWidget
{
    Q_OBJECT
public:
    LMRelationshipDetailsWidget( LMRelationship *pRelationship, CBD::EOLTypes nEOL, QWidget *pParent );

protected slots:
    void slotDominant( int n );
    void slotIdentified( int n );
    void slotDependent( int n );
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
    void slotGenerate( int n );
    void slotModified();

protected:
    LMRelationship *pRelationship;

    QCheckBox *                     pGenerate;
    LMRelationshipDetailsWidget *   pBegin;
    LMRelationshipDetailsWidget *   pEnd;
};

#endif




