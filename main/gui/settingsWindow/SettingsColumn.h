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

		/// Get the data stored in this column
		virtual QVariant getData(const int role=0) =0;

		/// Set the data stored in this column
		void setData(const QVariant&);

		/// Is this column editable in the view?
		virtual Qt::ItemFlag editable() const =0;

		/// Get the backGround color for the items of this
		/// column
		virtual QColor getBackGroundColor() const;

	protected:

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

		/// Get the data stored in this column
		virtual QVariant getData(const int role=0);

		/// Is this column editable in the view?
		virtual Qt::ItemFlag editable() const;

	private:

};

///-----------------------------------------

class ValueColumn : public SettingsColumn {

	public :

		/// Ctor
		ValueColumn();

		/// Dtor
		virtual ~ValueColumn();

		/// Get the data stored in this column
		virtual QVariant getData(const int role=0);

		/// Is this column editable in the view?
		virtual Qt::ItemFlag editable() const;

		/// Get the backGround color for the items of this
		/// column
		virtual QColor getBackGroundColor() const;

	private:

};

///-----------------------------------------

class UnitColumn : public SettingsColumn {

	public :

		/// Ctor
		UnitColumn();

		/// Dtor
		virtual ~UnitColumn();

		/// Get the data stored in this column
		virtual QVariant getData(const int role=0);

		/// Is this column editable in the view?
		virtual Qt::ItemFlag editable() const;

	private:

};


#endif
