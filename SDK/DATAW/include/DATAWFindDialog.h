#ifndef H_DATAWFindDialog
#define H_DATAWFindDialog

#include <QDialog>

class QCheckBox;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class DATASqlWidget;

class DATAWFindDialog : public QDialog
{
    Q_OBJECT
public:
    DATAWFindDialog( QWidget *parent, DATASqlWidget *pSQLWidget, bool bReplace = false );

protected slots:
    void slotFind();
    void slotReplace();
    void slotReplaceAll();
    void slotStateChanged( const QString &s );
    void slotStateChanged( Qt::CheckState n );

private:
    DATASqlWidget *pSQLWidget;

    QLineEdit *         pFind;
    QLineEdit *         pReplace;
    QCheckBox *         pCaseSensitive;
    QCheckBox *         pFromStart;
    QCheckBox *         pWholeWords;
    QCheckBox *         pSelection;
    QCheckBox *         pExpression;
    QDialogButtonBox *  pButtonBox;
    QPushButton *       pButtonFind;
    QPushButton *       pButtonReplace;
    QPushButton *       pButtonReplaceAll;
};

#endif
