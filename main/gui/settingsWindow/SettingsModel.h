#ifndef SETTINGS_MODEL_H
#define SETTINGS_MODEL_H

/* This class implements the Qt item model to be used for
 * creating the settings tree. This is composed by SettingsItems
 */

#include <QtCore/QAbstractItemModel>
#include <QtCore/QModelIndex>
#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtGui/QDoubleValidator>
#include <QtWidgets/QLineEdit>
#include "SettingsItem.h"

class SettingsModel : public QAbstractItemModel {

		Q_OBJECT

	public:

		/// Ctor
		explicit SettingsModel();

		/// Copy Ctor
		explicit SettingsModel(const SettingsModel&);

		/// Virtual Dtor
		virtual ~SettingsModel();

		/// Called by Qt, this method returns the data to visualize or some
		/// display options
		virtual QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

		/// Called by Qt, returns specific flags
		virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

		/// Returns a ptr to the root of this model
		SettingsItemRoot* getRoot() const;

		/// Called by Qt, returns the header
		virtual QVariant headerData(int section,
															Qt::Orientation orientation,
															int role = Qt::DisplayRole
															) const Q_DECL_OVERRIDE;

		/// Returns an index given row and column
		virtual QModelIndex index(int row, int column,
														const QModelIndex& parent= QModelIndex()
														) const Q_DECL_OVERRIDE;

		/// Get my parent
		QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;

		/// How many rows?
		virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

		/// How many cols?
		virtual int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

		/// Append a variable item to the tree
		void append( QList<QVariant>& columnData );

		/// Set some data for a given item - i.e: edit some value
		bool setData(const QModelIndex &index, const QVariant &value, int role) override;

		/// Assignment operator
		const SettingsModel& operator=(const SettingsModel&);

		/// Setup the data of this model
		void setupModelData();

	Q_SIGNALS:

		/// On copy, emit a signal that will notify the view that a
		/// given item must be expanded
		void mustExpand(const QModelIndex& );

		/// On copy, emit a signal that will notify the view that a
		/// given item must be collapsed
		void mustCollapse(const QModelIndex& );

	public slots:

		/// The view notifies the model that an item has been expanded
		/// The model will pass the information to the item itself
		/// Required to remember the current state of the items when
		/// cloning item trees
		void setItemExpanded(const QModelIndex&);

		/// The view notifies the model that an item has been folded
		/// The model will pass the information to the item itself
		/// Required to remember the current state of the items when
		/// cloning item trees
		void setItemCollapsed(const QModelIndex&);

	private:

		/// Get an item of this model
		SettingsItemBase* getItem(const QModelIndex &index) const;

		/// Root of the model tree
		SettingsItemRoot* pRootItem_;

};


#endif
