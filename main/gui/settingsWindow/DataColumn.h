#ifndef SETTINGS_COLUMN_H
#define SETTINGS_COLUMN_H

#include <QtCore/QVariant>
#include <string>

#include "EnumData.h"

using namespace std;

/// Column storing the data of the item tree
/// Base class for name, value and unit column
class DataColumn {

	public:

		/// Ctor
		DataColumn();

		/// Dtor
		virtual ~DataColumn();

		/// Clone this item, which is basically equivalent to calling the copy ctor
		/// This must be pure virtual, because the class is abstract and I cannot call
		/// the copy ctor from here. So I am obliged to issue the job to child classes
		virtual DataColumn* clone() const=0;

		std::string getDisplayName() const;

		/// Get the data stored in this column
		QVariant getData(const int role=0) const;

		/// Set the data stored in this column
		void setData(const QVariant&);

		/// Is this column editable in the view?
		virtual Qt::ItemFlag editable() const =0;

		/// Get the backGround color for the items of this
		/// column
		virtual QColor getBackGroundColor() const;

		/// Comparison operator
		bool operator==(const DataColumn&);

		/// Inverse Comparison operator
		bool operator!=(const DataColumn&);

	protected:

		/// Copy Ctor
		DataColumn(const DataColumn&);

		/// Container that stores the actual data to be visualized
		QVariant data_;

	protected:

		/// Name of the column as it is visualized in the
		/// settingsWindow
		std::string columnDisplayName_;

};

///-----------------------------------------

/// Store the name related data for this item
class NameColumn : public DataColumn {

	public :

		/// Ctor
		NameColumn();

		/// Value Ctor
		NameColumn(QVariant value);

		/// Dtor
		virtual ~NameColumn();

		/// Clone this item, which is basically equivalent to calling the copy ctor
		virtual NameColumn* clone() const;

		/// Is this column editable in the view?
		virtual Qt::ItemFlag editable() const;

	private:

		/// Copy Ctor
		NameColumn(const NameColumn&);

};

///-----------------------------------------

/// Store the value related data for this item
class ValueColumn : public DataColumn {

	public :

		/// Default Ctor
		ValueColumn();

		/// Value Ctor
		ValueColumn(QVariant value);

		/// Dtor
		virtual ~ValueColumn();

		/// Clone this item, which is basically equivalent to calling the copy ctor
		virtual ValueColumn* clone() const;

		/// Is this column editable in the view?
		virtual Qt::ItemFlag editable() const;

		/// Get the backGround color for the items of this
		/// column
		virtual QColor getBackGroundColor() const;

	private:

		/// Copy Ctor
		ValueColumn(const ValueColumn&);

};

///-----------------------------------------

/// Store the unit related data for this item
class UnitColumn : public DataColumn {

	public :

		/// Ctor
		UnitColumn();

		/// Dtor
		virtual ~UnitColumn();

		/// Clone this item, which is basically equivalent to calling the copy ctor
		virtual UnitColumn* clone() const;

		/// Is this column editable in the view?
		virtual Qt::ItemFlag editable() const;

	private:

		/// Copy Ctor
		UnitColumn(const UnitColumn&);

};


#endif
