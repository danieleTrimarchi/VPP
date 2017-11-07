#ifndef GET_SETTINGS_ITEM_VISITOR
#define GET_SETTINGS_ITEM_VISITOR

#include "SettingsItem.h"

/* This visitor is used to get and set values on the settings item tree */
class GetSettingsItemVisitor {

	public:

		/// Ctor
		GetSettingsItemVisitor(SettingsItemBase* root);

		/// Dtor
		virtual ~GetSettingsItemVisitor();

		/// Items are accepting the visitor starting from root
		virtual SettingsItemBase* get(QString varName) const =0;

	protected:

		/// Ptr on the root of the tree this item is iterating onto
		SettingsItemBase* pRoot_;

	private:

		/// Disallowed default Ctor
		GetSettingsItemVisitor();

};

///======================================================================


/* This visitor is used to get and set values on the settings item tree */
class GetSettingsItemByPathVisitor : public GetSettingsItemVisitor {

	public:

		/// Ctor
		GetSettingsItemByPathVisitor(SettingsItemBase* root);

		/// Dtor
		~GetSettingsItemByPathVisitor();

		/// Items are accepting the visitor starting from root
		SettingsItemBase* get(QString varName) const;

	private:

		/// Disallowed default Ctor
		GetSettingsItemByPathVisitor();

};

///======================================================================

/// This visitor is basically a copy of the GetSettingsItemByPathVisitor, but the
/// items will accept it differently - they will search a match with their name,
/// instead of using the path
class GetSettingsItemByNameVisitor : public GetSettingsItemVisitor {

	public :

		/// Ctor
		GetSettingsItemByNameVisitor(SettingsItemBase* root);

		/// Dtor
		~GetSettingsItemByNameVisitor();

		/// Items are accepting the visitor starting from root
		SettingsItemBase* get(QString varName) const;

	private:

		/// Disallowed default Ctor
		GetSettingsItemByNameVisitor();

};


#endif
