#ifndef SETTINGS_MODEL_H
#define SETTINGS_MODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QIcon>

class SettingItem {

	public:

		/// Ctor
		SettingItem(SettingItem* parentItem = 0);

		/// Dtor
		~SettingItem();

		/// Append a child under me
    void appendChild(SettingItem* child);

    /// Get the i-th child
    SettingItem* child(int row);

    /// How many children do I have?
    int childCount() const;

		/// Number of cols required for this item.
		int columnCount() const;

		/// Return the data stored in the ith column of
		/// this item. In this case, either the label
		/// or the numerical value
		QVariant data(int column) const;

		/// Return the parent item
		SettingItem* parentItem();

		/// What child number am I for my parent?
		int row() const;

    /// Returns the associated icon - in this case an empty QVariant
		QVariant getIcon();

	private:

		/// Parent of this item
		SettingItem* pParent_;

		/// Children of this item
    QList<SettingItem*> children_;

		/// Data stored in this item and that will be
		/// shown in the tree
    QList<QVariant> data_;

};

// --

class SettingsModel : public QAbstractItemModel {

		Q_OBJECT

	public:

		/// Ctor
		explicit SettingsModel(QObject* parent = 0);

		/// Virtual Dtor
		virtual ~SettingsModel();

		/// Called by Qt, this method returns the data to visualize or some
		/// display options
		virtual QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

		/// Called by Qt, returns specific flags
		virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

		/// Called by Qt, returns the header
		virtual QVariant headerData(
				int section, Qt::Orientation orientation,
				int role = Qt::DisplayRole
				) const Q_DECL_OVERRIDE;

		/// Returns an index given row and column
		virtual QModelIndex index(
				int row, int column,
				const QModelIndex &parent = QModelIndex()
				) const Q_DECL_OVERRIDE;

		/// Get my parent
		QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;

		/// How many rows?
		virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

		/// How many cols?
		virtual int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

		/// Append a variable item to the tree
		void append( QList<QVariant>& columnData );

	private:

		/// Root of the model tree
		SettingItem* rootItem_;

};


#endif
