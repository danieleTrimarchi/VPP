#include "VppSettingsXmlReader.h"
#include "VppSettingsXmlWriter.h"
#include "VariableFileParser.h"
#include <QString>
#include "VppTags.h"

template <class TUnit>
const string SettingsItem<TUnit>::className_("SettingsItem");

// Ctor
template <class TUnit>
SettingsItem<TUnit>::SettingsItem(	const QString& displayName,
		const QString& variableName,
		const QVariant& value,
		const QString& tooltip):
		SettingsItemBase(){

		// Instantiates the unit for this item
		pUnit_.reset(new TUnit);
		columns_[colNames::name_.idx_]->setData( displayName );
		columns_[colNames::value_.idx_]->setData( value );
		columns_[colNames::unit_.idx_]->setData( pUnit_->getUnitName().c_str() );

		// Set the tooltip
		tooltip_= tooltip;

		// The editable columns for the settings item are editable
		setEditable(true);

		// Set the variable name that will be used to build
		// the variable in the variableFileParser
		variableName_= variableName;
}

// Ctor from xml. Just call the other constructor
template <class TUnit>
SettingsItem<TUnit>::SettingsItem(const XmlAttributeSet& xmlAttSet):
				SettingsItem<TUnit>(
						xmlAttSet[displayNameTag.c_str()].toQString(),
						xmlAttSet[Variable::variableNameTag_.c_str()].toQString(),
						xmlAttSet[valueTag.c_str()].toQString(),
						xmlAttSet[tooltipTag.c_str()].toQString() ){
}

// Copy Ctor
template <class TUnit>
SettingsItem<TUnit>::SettingsItem(const SettingsItem<TUnit>& rhs) :
					SettingsItemBase(rhs) {

}

// Ctor
template <class TUnit>
SettingsItem<TUnit>::SettingsItem(const varData& data,	//< Variable data
		const QVariant&value): 				//<	Value of this variable
		SettingsItem<TUnit>(data.display_,data.name_,value,data.tootip_) {}

// The item will give the Delegate the editor
// to be properly edited - in this case a QLineEdit with
// double validator
template <class TUnit>
QWidget* SettingsItem<TUnit>::createEditor(QWidget *parent) {

		// Set local settings for the validators
		QLocale localSettings = QLocale::c();
		localSettings.setNumberOptions(QLocale::RejectGroupSeparator | QLocale::OmitGroupSeparator);

		QLineEdit *editor = new QLineEdit(parent);
		QDoubleValidator* doubleValidator= new QDoubleValidator(0.000, 999999.000, 3, editor);
		doubleValidator->setLocale(localSettings);

		editor->setStyleSheet(	"background-color: white;"
				"selection-color: orange;" );

		editor->setValidator(doubleValidator);
		editor->setFrame(false);
		editor->setText( QString::number(0.000) );

		return editor;

}

// Edit the data in the editor
template <class TUnit>
void SettingsItem<TUnit>::setEditorData(QWidget *editor,const QModelIndex& index) {

		double value = index.model()->data(index, Qt::EditRole).toDouble();

		// Set local settings for the validators
		QLocale localSettings = QLocale::c();
		localSettings.setNumberOptions(QLocale::RejectGroupSeparator | QLocale::OmitGroupSeparator);

		QDoubleValidator* doubleValidator= new QDoubleValidator(0.000, 999999.000, 3, editor);
		doubleValidator->setLocale(localSettings);

		QLineEdit* pLineEdit = static_cast<QLineEdit*>(editor);

		pLineEdit->setValidator(doubleValidator);
		pLineEdit->setFrame(false);
		pLineEdit->setText( QString::number(value) );

}

// Set the data in the model
template <class TUnit>
void SettingsItem<TUnit>::setModelData(	QWidget* editor,
		QAbstractItemModel* model,
		const QModelIndex& index) const {

	QLineEdit* pQLineEdit = static_cast<QLineEdit*>(editor);

	model->setData(index, pQLineEdit->text(), Qt::EditRole);

}

// Visual options - requested by the Delegate - this directly derives from MEMS+
// Decorates the base class method paint
template <class TUnit>
void SettingsItem<TUnit>::paint(QPainter* painter, const QStyleOptionViewItem &option,
		const QModelIndex &index) const {

	painter->save();

	// If the element is editable we draw a background and a box around the text
	if (index.model()->flags(index).testFlag(Qt::ItemIsEditable)) {

		// Gets a rectangle to draw the background and adjust it
		QRect boxRect(option.rect);
		boxRect.adjust(1, 1, -1, -1);

		// Sets the color of the border depending on the selected or hovered state
		if ( (option.state & QStyle::State_Selected) || (option.state & QStyle::State_MouseOver) )
			painter->setPen(QApplication::palette().highlight().color());
		else
			painter->setPen(QPen(Qt::lightGray));

		// Sets the brush depending on the selected state
		if (option.state & QStyle::State_Selected)
			painter->setBrush(QBrush(QApplication::palette().highlight().color(), Qt::Dense6Pattern));
		else
			painter->setBrush(QColor("white"));

		// Draws the rectangle around the text
		painter->drawRect(boxRect);
	}

	// Call base-class method
	SettingsItemBase::paint(painter,option,index);

	painter->restore();
}

// Accept a visitor that will write this item to XML
template <class TUnit>
void SettingsItem<TUnit>::accept( VariableParserGetVisitor& v) {

		// Visit me
		v.visit(this);
}


// Accept a visitor that will write this item to XML
template <class TUnit>
void SettingsItem<TUnit>::accept( VPPSettingsXmlWriterVisitor& v ) {

		// Visit me
		v.visit(this);
}

// Clone this item, which is basically equivalent to calling the copy ctor
template <class TUnit>
SettingsItem<TUnit>* SettingsItem<TUnit>::clone() const {
	return new SettingsItem<TUnit>(*this);
}

// Returns a copy of this item, the value of which
// has been converted to SI units. The default behavior
// is to return a clone of myself, because I am already
// in the correct unit. Warning : this is specialized for
// Degrees!
template <class TUnit>
SettingsItemBase* SettingsItem<TUnit>::convertToSI() {
		return this->clone();
}

// Return the backGround color for this item based on the column
template <class TUnit>
QColor SettingsItem<TUnit>::getBackGroundColor(int iColumn) const {

	return columns_[iColumn]->getBackGroundColor();

}

// Dtor
template <class TUnit>
SettingsItem<TUnit>::~SettingsItem(){

}

////////////////////////////////////////////////////////////////

template <class TUnit>
const string SettingsItemInt<TUnit>::className_("SettingsItemInt");

/// Ctor
template <class TUnit>
SettingsItemInt<TUnit>::SettingsItemInt(
		const QString& displayName,
		const QString& variableName,
		const QVariant& value,
		const QString& tooltip):
		SettingsItem<TUnit>(displayName,variableName,value,tooltip){

}

template <class TUnit>
SettingsItemInt<TUnit>::SettingsItemInt(const XmlAttributeSet& xmlAttSet):
	SettingsItem<TUnit>(xmlAttSet) {

}

// Ctor
template <class TUnit>
SettingsItemInt<TUnit>::SettingsItemInt(	const varData& data, 	//< Name visualized in the UI
		const QVariant& value):				//<	Value of this variable
		SettingsItem<TUnit>(data.display_, data.name_,value, data.tootip_)	{}


// Copy ctor, called by clone()
template <class TUnit>
SettingsItemInt<TUnit>::SettingsItemInt(const SettingsItemInt<TUnit>& rhs) :
						SettingsItem<TUnit>(rhs) {
	// There is nothing else to do, as there are no owned class
	// members
}

// The item will give the Delegate the editor
// to be properly edited - in this case a spinbox
template <class TUnit>
QWidget* SettingsItemInt<TUnit>::createEditor(QWidget *parent) {

	QSpinBox* editor = new QSpinBox(parent);
	editor->setFrame(false);
	editor->setMinimum(0);
	editor->setMaximum(1000);

	editor->setStyleSheet(	"background-color: white;"
			"selection-color: orange;");

	return editor;

}

// Edit the data in the editor
template <class TUnit>
void SettingsItemInt<TUnit>::setEditorData(QWidget *editor,const QModelIndex& index) {

	// Get the value stored in the model for this object
	int value = index.model()->data(index, Qt::EditRole).toInt();

	QSpinBox* pSpinBox = static_cast<QSpinBox*>(editor);
	pSpinBox->setValue(value);

}

template <class TUnit>
void SettingsItemInt<TUnit>::setModelData(	QWidget *editor,
		QAbstractItemModel *model,
		const QModelIndex &index) const {

	QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
	spinBox->interpretText();
	int value = spinBox->value();

	//std::cout<<"Setting \'"<<value<<"\' to item "<<this<<" ; model "<<model<<std::endl;
	model->setData(index, value, Qt::EditRole);
}

// Accept a visitor that will write this item to XML
template <class TUnit>
void SettingsItemInt<TUnit>::accept( VPPSettingsXmlWriterVisitor& v ) {

	// Visit me
	v.visit(this);
}

// Dtor
template <class TUnit>
SettingsItemInt<TUnit>::~SettingsItemInt() {

}

// Clone this item, which is basically equivalent to calling the copy ctor
template <class TUnit>
SettingsItemInt<TUnit>* SettingsItemInt<TUnit>::clone() const {
	return new SettingsItemInt<TUnit>(*this);
}

////////////////////////////////////////////////////////////////

template <class TUnit>
const string SettingsItemComboBox<TUnit>::className_("SettingsItemComboBox");
template <class TUnit>
const string SettingsItemComboBox<TUnit>::numOptsTag_("numOpts");
template <class TUnit>
const string SettingsItemComboBox<TUnit>::activeIndexTag_("ActiveIndex");


// Ctor
template <class TUnit>
SettingsItemComboBox<TUnit>::SettingsItemComboBox(
		const QString& displayName,
		const QString& variableName,
		const QList<QString>& options,
		const QString& tooltip):
		SettingsItem<TUnit>(displayName,variableName,options[0],tooltip),
		opts_(options),
		activeIndex_(0) {

}

// Ctor from xml
// todo : the options are known by the visitor, not the item. All the
// logics here should be displaced to the XmlReadVisitor!
template <class TUnit>
SettingsItemComboBox<TUnit>::SettingsItemComboBox(const XmlAttributeSet& xmlAttSet) :
	SettingsItem<TUnit>(	xmlAttSet[displayNameTag.c_str()].toQString(),
											xmlAttSet[Variable::variableNameTag_.c_str()].toQString(),
											xmlAttSet["Option0"].toQString(),
											xmlAttSet[tooltipTag.c_str()].toQString() ){

	// Populate the options
	for(size_t i=0; i<int((xmlAttSet[SettingsItemComboBox<TUnit>::numOptsTag_.c_str()]).getInt()); i++){
		char msg[256];
		sprintf(msg,"Option%zu",i);
		opts_.push_back(xmlAttSet[string(msg)].toQString());
	}

	activeIndex_= xmlAttSet[activeIndexTag_.c_str()].getInt();

}

// Ctor
template <class TUnit>
SettingsItemComboBox<TUnit>::SettingsItemComboBox(const varData& data,	//< Data of the variable
		const QList<QString>& options)://<	List of options available in this combo-box
		SettingsItemComboBox<TUnit>(data.display_,data.name_,options,data.tootip_) {
		activeIndex_=0;
};

// The item will give the Delegate the editor
// to be properly edited - in this case a spinbox
template <class TUnit>
QWidget* SettingsItemComboBox<TUnit>::createEditor(QWidget *parent) {

	QComboBox* editor = new QComboBox(parent);

	for(size_t i=0; i<opts_.size(); i++)
		editor->addItem(opts_[i]);

	editor->setCurrentIndex(activeIndex_);

	editor->setStyleSheet(	"background-color: white;"
			"selection-color: orange;");

	return editor;
}

// Dtor
template <class TUnit>
SettingsItemComboBox<TUnit>::~SettingsItemComboBox() {
	/* do nothing */
}

// Accept a visitor that will write this item to XML
template <class TUnit>
void SettingsItemComboBox<TUnit>::accept( VPPSettingsXmlWriterVisitor& v ) {

	// Visit me
	v.visit(this);
}

// Accept a visitor that will write this item to XML
template <class TUnit>
void SettingsItemComboBox<TUnit>::accept( VariableParserGetVisitor& v) {

	// Visit me
	v.visit(this);
}

// Clone this item, which is basically equivalent to calling the copy ctor
template <class TUnit>
SettingsItemComboBox<TUnit>* SettingsItemComboBox<TUnit>::clone() const {
	return new SettingsItemComboBox<TUnit>(*this);
}

// Edit the data in the editor
template <class TUnit>
void SettingsItemComboBox<TUnit>::setEditorData(QWidget *editor,const QModelIndex& index) {

	// Set the data with the index of the selected option
	QComboBox* pComboBox = static_cast<QComboBox*>(editor);
	pComboBox->setCurrentIndex(activeIndex_);
}

// Set the data in the model - called by the Delegate
template <class TUnit>
void SettingsItemComboBox<TUnit>::setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const {

	// Get a handle on the combo box
	QComboBox* pComboBox = static_cast<QComboBox*>(editor);

	// Get the selected text
	QString value = pComboBox->currentText();

	// Set the active index. Note that activeIndex_ is declared
	// mutable, as we need to set this inside a const method
	activeIndex_ = pComboBox->currentIndex();

	// Set the underlying model
	model->setData(index, value, Qt::EditRole);

}

// Only meaningful for the combo-box item, returns zero for all the
// other items
template <class TUnit>
size_t SettingsItemComboBox<TUnit>::getActiveIndex() const {
	return activeIndex_;
}

//  Returns the label of the active (selected) item
// Called by the parent 'paint' method
template <class TUnit>
QString SettingsItemComboBox<TUnit>::getActiveText(const QModelIndex &index) const {
	return opts_[activeIndex_];
}

// How many options are available to this combo-box?
template <class TUnit>
size_t SettingsItemComboBox<TUnit>::getNumOpts() const {
	return opts_.size();
}

// Get the i-th option for this Combo-Box
template <class TUnit>
QString SettingsItemComboBox<TUnit>::getOption(size_t i) const {
	return opts_[i];
}

// Assignment operator
template <class TUnit>
const SettingsItemComboBox<TUnit>& SettingsItemComboBox<TUnit>::operator=(const SettingsItemComboBox& rhs) {

	// Call the parent operator=
	SettingsItem<TUnit>::operator=(rhs);

	// Now copy own members
	opts_= rhs.opts_;
	activeIndex_= rhs.activeIndex_;

	return *this;
}

// Visual options - requested by the Delegate - this directly derives from MEMS+
// Decorates the base class method paint
template <class TUnit>
void SettingsItemComboBox<TUnit>::paint(QPainter* painter, const QStyleOptionViewItem &option,
		const QModelIndex &index) const {

//	painter->save();
//
//	// If the element is editable we draw a background and a box around the text
//	if (index.model()->flags(index).testFlag(Qt::ItemIsEditable)) {
//
//		// Gets a rectangle to draw the background and adjust it
//		QRect boxRect(option.rect);
//		boxRect.adjust(1, 1, -1, -1);
//
//		// Sets the color of the border depending on the selected or hovered state
//		if ( (option.state & QStyle::State_MouseOver) ){
//			std::cout<<"Hoovering over me..!\n";
//				painter->setPen(QApplication::palette().highlight().color());
//		}
//		else {
//			std::cout<<"NOT hoovering over me with state "<<option.state<<std::endl;
//			painter->setPen(QPen(Qt::yellow));
//		}
//
//		// Sets the brush depending on the selected state
//		if (option.state & QStyle::State_Selected)
//			painter->setBrush(QBrush(QApplication::palette().highlight().color(), Qt::Dense6Pattern));
//		else
//			painter->setBrush(QColor("red"));
//
//		// Draws the rectangle around the text
//		painter->drawRect(boxRect);
//	}
//
//	// Call base-class method
//	SettingsItemBase::paint(painter,option,index);
//
//	painter->restore();
		QStyleOptionComboBox comboboxoptions;
		comboboxoptions.rect = option.rect;
		comboboxoptions.state = QStyle::State_Active;
		comboboxoptions.frame = true;
		comboboxoptions.currentText = index.model()->data(index).toString();
		QApplication::style()->drawComplexControl(QStyle::CC_ComboBox,&comboboxoptions,painter);
		QApplication::style()->drawControl(QStyle::CE_ComboBoxLabel,&comboboxoptions,painter);

}

// Copy Ctor
template <class TUnit>
SettingsItemComboBox<TUnit>::SettingsItemComboBox(const SettingsItemComboBox<TUnit>& rhs) :
						SettingsItem<TUnit>(rhs),
						opts_(rhs.opts_),
						activeIndex_(rhs.activeIndex_){

}

////////////////////////////////////////////////////////////////

template <class TUnit>
const string SettingsItemBounds<TUnit>::className_("SettingsItemBounds");

// Ctor
template <class TUnit>
SettingsItemBounds<TUnit>::SettingsItemBounds(
		const QVariant& displayName,
		const QVariant& variableName,
		double min,
		double max,
		const QVariant& tooltip) :
		SettingsItemGroup(displayName){

		appendChild( new SettingsItem<TUnit>(displayName.toString()+QString("_min"),variableName.toString()+QString("_MIN"),min,"min value") );
		appendChild( new SettingsItem<TUnit>(displayName.toString()+QString("_max"),variableName.toString()+QString("_MAX"),max,"max value") );

		// Set the tooltip
		tooltip_= tooltip;

		// Set the variable Name for this item
		variableName_= variableName.toString();
}

template <class TUnit>
SettingsItemBounds<TUnit>::SettingsItemBounds(const varData& data,	//< Data of the variable
		double min,									//<	Min value of this variable bound
		double max):									//<	Max value of this variable bound
SettingsItemBounds(data.display_,data.name_,min,max,data.tootip_){

}

// Ctor from xml
template <class TUnit>
SettingsItemBounds<TUnit>::SettingsItemBounds(const XmlAttributeSet& xmlAttSet) :
	SettingsItemGroup(xmlAttSet){

		// Do not instantiate children, if requested they are instantiated on
		// the fly while reading the rest of the xml
}

// Copy Ctor, called by clone()
template <class TUnit>
SettingsItemBounds<TUnit>::SettingsItemBounds(const SettingsItemBounds& rhs):
SettingsItemGroup(rhs){

		// Do nothing as SettingsItemBounds do not have own members
}

// Dtor
template <class TUnit>
SettingsItemBounds<TUnit>::~SettingsItemBounds() {

}

// Accept a visitor that will write this item to XML
template <class TUnit>
void SettingsItemBounds<TUnit>::accept( VPPSettingsXmlWriterVisitor& v ) {

		// Visit me
		v.visit(this);
}

// Accept a visitor that will write this item to the variableFileParser
template <class TUnit>
void SettingsItemBounds<TUnit>::accept( VariableParserGetVisitor& v) {

		// Visit me
		v.visit(this);
}

// Clone this item, which is basically equivalent to calling the copy ctor
template <class TUnit>
SettingsItemBounds<TUnit>* SettingsItemBounds<TUnit>::clone() const {
		return new SettingsItemBounds<TUnit>(*this);
}

template <class TUnit>
QFont SettingsItemBounds<TUnit>::getFont() const {
		QFont myFont;
		myFont.setBold(true);
		myFont.setUnderline(false);
		int fontSize = myFont.pointSize();
		myFont.setPointSize(fontSize-2);
		return myFont;
}

// Returns a handle on the item that represents the min in this bound
template <class TUnit>
SettingsItem<TUnit>* SettingsItemBounds<TUnit>::getItemMin() {
		return dynamic_cast<SettingsItem<TUnit> * >(child(0));
}

// Returns a handle on the item that represents the max in this bound
template <class TUnit>
SettingsItem<TUnit>* SettingsItemBounds<TUnit>::getItemMax() {
		return dynamic_cast<SettingsItem<TUnit> * >(child(1));
}

// Get the min value of this bound
template <class TUnit>
double SettingsItemBounds<TUnit>::getMin() {
		return getItemMin()->data(colNames::value_.idx_).toDouble();
}

// Get the max value of this bound
template <class TUnit>
double SettingsItemBounds<TUnit>::getMax() {
		return getItemMax()->data(colNames::value_.idx_).toDouble();
}


