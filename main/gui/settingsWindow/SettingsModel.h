#ifndef SETTINGS_MODEL_H
#define SETTINGS_MODEL_H

/* This class implements the Qt item model to be used for
 * creating the settings tree. This is composed by SettingsItems
 */

#include <QtCore/QModelIndex>
#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtGui/QDoubleValidator>
#include <QtWidgets/QLineEdit>

#include "VppItemModel.h"
#include "SettingsItem.h"

class SettingsModel : public VppItemModel {

		Q_OBJECT

	public:

		/// Ctor
		explicit SettingsModel();

		/// Copy Ctor
		explicit SettingsModel(const SettingsModel&);

		/// Virtual Dtor
		virtual ~SettingsModel();

		/// Called by Qt, returns specific flags
		virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

		// Returns a ptr to the root of this model
		SettingsItemBase* getRoot() const;

		/// Assignment operator
		const SettingsModel& operator=(const SettingsModel&);

		/// Comparison operator
		bool operator==(const SettingsModel&);

		/// Inverse Comparison operator
		bool operator!=(const SettingsModel&);

		/// Diagnostics print
		void print();

		/// Set some data for a given item - i.e: edit some value
		bool setData(const QModelIndex &index, const QVariant &value, int role) override;

		/// Setup the data of this model
		void setupModelData();

	Q_SIGNALS:

		/// On copy, emit a signal that will notify the view that a
		/// given item must be collapsed
		void mustCollapse(const QModelIndex& );

		/// On copy, emit a signal that will notify the view that a
		/// given item must be expanded
		void mustExpand(const QModelIndex& );

	public slots:

		/// The view notifies the model that an item has been folded
		/// The model will pass the information to the item itself
		/// Required to remember the current state of the items when
		/// cloning item trees
		void setItemCollapsed(const QModelIndex&);

		/// The view notifies the model that an item has been expanded
		/// The model will pass the information to the item itself
		/// Required to remember the current state of the items when
		/// cloning item trees
		void setItemExpanded(const QModelIndex&);

	private:

};


#endif
