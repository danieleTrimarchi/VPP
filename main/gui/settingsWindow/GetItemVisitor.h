#ifndef GET_SETTINGS_ITEM_VISITOR
#define GET_SETTINGS_ITEM_VISITOR

#include "SettingsItem.h"

/* This visitor is used to get and set values on the settings item tree */
class GetItemVisitor {

	public:

		/// Ctor
		GetItemVisitor(Item* root);

		/// Dtor
		virtual ~GetItemVisitor();

		/// Items are accepting the visitor starting from root
		virtual Item* get(QString varName) const =0;

	protected:

		/// Ptr on the root of the tree this item is iterating onto
		Item* pRoot_;

	private:

		/// Disallowed default Ctor
		GetItemVisitor();

};

///======================================================================


/* This visitor is used to get and set values on the settings item tree */
class GetItemByPathVisitor : public GetItemVisitor {

	public:

		/// Ctor
		GetItemByPathVisitor(Item* root);

		/// Dtor
		~GetItemByPathVisitor();

		/// Items are accepting the visitor starting from root
		Item* get(QString varName) const;

	private:

		/// Disallowed default Ctor
		GetItemByPathVisitor();

};

///======================================================================

/// This visitor is basically a copy of the GetSettingsItemByPathVisitor, but the
/// items will accept it differently - they will search a match with their name,
/// instead of using the path
class GetItemByNameVisitor : public GetItemVisitor {

	public :

		/// Ctor
		GetItemByNameVisitor(Item* root);

		/// Dtor
		~GetItemByNameVisitor();

		/// Items are accepting the visitor starting from root
		Item* get(QString varName) const;

	private:

		/// Disallowed default Ctor
		GetItemByNameVisitor();

};


#endif
