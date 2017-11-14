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
#include "SettingsWindowView.h"

class SettingsModel : public QAbstractItemModel {

		Q_OBJECT

	public:

		/// Ctor
		explicit SettingsModel(SettingsWindowView*,QObject* parent = 0);

		/// Virtual Dtor
		virtual ~SettingsModel();

		/// Called by Qt, this method returns the data to visualize or some
		/// display options
		virtual QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

		/// Called by Qt, returns specific flags
		virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

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

	public slots:

		/// Call the resizeColumnToContents of the view
		void resizeColumnsToContents(const QModelIndex& );

	private:

		/// Setup the data of this model
		void setupModelData(SettingsItemBase *parent);

		/// Get an item of this model
		SettingsItemBase *getItem(const QModelIndex &index) const;

		/// Root of the model tree
		SettingsItemBase* pRootItem_;

		/// Raw ptr to the current item tree view
		SettingsWindowView* pView_;

};


#endif
