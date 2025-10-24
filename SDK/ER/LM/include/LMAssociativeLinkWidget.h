/*!
 * \file 
 *  
 * This is pretty much the same as LMRelationshipWidget. 
 * However; some changes to this are expected - perhaps using getEntityNormal() and getEntityAssociative(). 
 *  
 * \author pharvey (5/13/20)
 */
#ifndef H_LMAssociativeLinkWidget
#define H_LMAssociativeLinkWidget

#include "LMAssociativeLink.h"

class LMAssociativeLinkDetailsWidget : public QWidget
{
    Q_OBJECT
public:
    LMAssociativeLinkDetailsWidget( LMAssociativeLink *pAssociativeLink, CBD::EOLTypes nEOL, QWidget *pParent );

protected slots:
    void slotDominant( Qt::CheckState n );
    void slotIdentified( Qt::CheckState n );
    void slotDependent( Qt::CheckState n );
    void slotCardinalityMin( const QString &s );
    void slotCardinalityMax( const QString &s );
    void slotRole( const QString &s );

    void slotModified();

protected:
    LMAssociativeLink *pAssociativeLink;
    CBD::EOLTypes   nEOL;

    QCheckBox *pCheckBoxDominant; 
    QCheckBox *pCheckBoxIdentified;
    QCheckBox *pCheckBoxDependent;
    QComboBox *pComboBoxCardinalityMin;
    QComboBox *pComboBoxCardinalityMax;
    QLineEdit *pLineEditRole;

};

class LMAssociativeLinkWidget : public QWidget
{
    Q_OBJECT
public:
    LMAssociativeLinkWidget( LMAssociativeLink *pAssociativeLink, Qt::Orientation nOrientation, QWidget *pParent );

protected slots:
    void slotGenerate( Qt::CheckState n );
    void slotModified();

protected:
    LMAssociativeLink *pAssociativeLink;

    QCheckBox *                     pGenerate;
    LMAssociativeLinkDetailsWidget *pBegin;
    LMAssociativeLinkDetailsWidget *pEnd;
};

#endif




