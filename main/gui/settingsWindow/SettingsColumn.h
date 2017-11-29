#ifndef SETTINGS_COLUMN_H
#define SETTINGS_COLUMN_H

#include <QtCore/QVariant>

enum columnNames {
	name,
	value,
	unit
};

class SettingsColumn {

	public:

		/// Ctor
		SettingsColumn();

		/// Dtor
		virtual ~SettingsColumn();

		/// Clone this item, which is basically equivalent to calling the copy ctor
		virtual SettingsColumn* clone() const =0;

		/// Get the data stored in this column
		QVariant getData(const int role=0) const;

		/// Set the data stored in this column
		void setData(const QVariant&);

		/// Is this column editable in the view?
		virtual Qt::ItemFlag editable() const =0;

		/// Get the backGround color for the items of this
		/// column
		virtual QColor getBackGroundColor() const;

	protected:

		/// Copy Ctor
		SettingsColumn(const SettingsColumn&);

		/// Container that stores the actual data to be visualized
		QVariant data_;

	private:



};

///-----------------------------------------

class NameColumn : public SettingsColumn {

	public :

		/// Ctor
		NameColumn();

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

class ValueColumn : public SettingsColumn {

	public :

		/// Ctor
		ValueColumn();

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

class UnitColumn : public SettingsColumn {

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
