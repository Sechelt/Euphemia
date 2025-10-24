#ifndef H_AWValidationWidget
#define H_AWValidationWidget

#include <QtGui>
#include <QtWidgets>
#include <QStyledItemDelegate>

#include <ADValidation.h>
#include <ADObject.h>

/*!
 * \brief Layer a simple tree structure over our ADValidationDatabase. 
 *  
 * This, tree, structure is a more natural fit for Qt model than the 
 * structure of ADValidationDatabase. 
 *  
 * Each tree item contains a ref to ADValidationDatabase and a copy of 
 * the 3 keys needed to read/write the Severity. 
 *  
 * \author pharvey (4/28/20)
 */
class AWValidationItem
{
public:
    explicit AWValidationItem( const QVariant &v, AWValidationItem *pParent = nullptr );
    virtual ~AWValidationItem();

    void appendChild( AWValidationItem *child );

    AWValidationItem* child( int row );
    int childCount() const;
    int columnCount() const;
    virtual QVariant data( int column ) const;
    int row() const;
    AWValidationItem* parentItem();

private:
    AWValidationItem *          pParent;        // parent item or nullptr for root
    QVariant                    variantColumn0; // column 0 - it can not be changed (static)
    QList<AWValidationItem *>   listChildren;   //
};

/*!
 * \brief Root 
 *  
 * This is special because, as the root, we are representing the column header.
 * 
 * \author pharvey (4/28/20)
 */
class AWValidationItemRoot : public AWValidationItem
{
public:
    explicit AWValidationItemRoot();

    virtual QVariant data( int column ) const;
};

/*!
 * \brief Severity 
 *  
 * This is special because this is where we can edit the value. To do this we need 
 * all the keys and a reference to the data. 
 * 
 * \author pharvey (4/28/20)
 */
class AWValidationItemSeverity : public AWValidationItem
{
public:
    explicit AWValidationItemSeverity( ADValidationDatabase *pData, const QString &stringClass, ADValidation::Strictness nStrictness, const QString &stringCheck, AWValidationItem *pParent = nullptr );

    virtual QVariant data( int column ) const;
    virtual void setData( int n );

private:
    AWValidationItem *          pParent;        // parent item or nullptr for root
    
    ADValidationDatabase *      pData;          // ref to data so we can update 
    QString                     stringClass;    // key1
    ADValidation::Strictness    nStrictness;    // key2
    QString                     stringCheck;    // key3
};


/*!
 * \brief A model for our validation config data so we can use QTreeView.
 *  
 * There is need to add/remove data - just to allow edit of the Severity. 
 *  
 * \author pharvey (4/28/20)
 */
class AWValidationModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit AWValidationModel( ADValidationDatabase *pData, QObject *parent = 0 );
    ~AWValidationModel();

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

private:
    AWValidationItem *pRoot;

    void setupModelData( ADValidationDatabase *pData, AWValidationItem *pRoot );
};

/*!
 * \brief Delegate for editing Severity.
 * 
 * \author pharvey (4/29/20)
 */
class AWValidationSeverityDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    AWValidationSeverityDelegate( QObject *parent = 0 );

    QWidget* createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const override;
    void setEditorData( QWidget *editor, const QModelIndex &index ) const override;
    void setModelData( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const override;
    void updateEditorGeometry( QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index ) const override;
};

/*!
 * \brief Widget for viewing/editing.
 * 
 * \author pharvey (4/29/20)
 */
class AWValidationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AWValidationWidget( ADObject *pObject, ADValidationDatabase *pData, ADValidation::Strictness n, QWidget *pWidgetParent );
    ~AWValidationWidget();

signals:
    void signalStrictness( ADValidation::Strictness n );

protected slots:
    void slotChanged( const QModelIndex &, const QModelIndex & );
    void slotStrictness( const QString &s );

protected:
    ADObject *                      pObject;        // object that owns pData (so we can set modified)
    ADValidationDatabase *          pData;          // validation config data
    QComboBox *                     pStrictness;    // 
    AWValidationModel *             pModel;         // 
    QTreeView *                     pView;          //
    AWValidationSeverityDelegate *  pDelegate;      //

    void doSaveState();
    void doLoadState();
};

#endif
