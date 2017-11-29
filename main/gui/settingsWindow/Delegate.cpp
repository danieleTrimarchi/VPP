
/*    delegate.cpp

    A delegate that allows the user to change integer values from the model
    using a spin box widget.*/
#include "delegate.h"

#include <QtWidgets/QSpinBox>
#include <QtWidgets/QLineEdit>
#include "SettingsItem.h"

#include <iostream>

SpinBoxDelegate::SpinBoxDelegate(QObject *parent)
: QStyledItemDelegate(parent) {

}


QWidget *SpinBoxDelegate::createEditor(QWidget* parent,
		const QStyleOptionViewItem& options,
		const QModelIndex& index) const {

	QSpinBox *editor = new QSpinBox(parent);
	editor->setFrame(false);
	editor->setMinimum(0);
	editor->setMaximum(10000);

	editor->setStyleSheet(	"background-color: white;"
			"selection-color: orange;");

	return editor;
}

void SpinBoxDelegate::setEditorData(QWidget *editor,
		const QModelIndex &index) const {

	int value = index.model()->data(index, Qt::EditRole).toInt();

	QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
	spinBox->setValue(value);
}

void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const {
	QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
	spinBox->interpretText();
	int value = spinBox->value();

	model->setData(index, value, Qt::EditRole);
}

void SpinBoxDelegate::updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option, const QModelIndex &/* index */) const {
	editor->setGeometry(option.rect);
}

// -----------------------------------------------------------------


QLineEditDelegate::QLineEditDelegate(QObject *parent)
: QStyledItemDelegate(parent){

}


QWidget* QLineEditDelegate::createEditor(QWidget* parent,
		const QStyleOptionViewItem& option,
		const QModelIndex& index) const {

	if (index.isValid()) {
		SettingsItemBase* pItem = static_cast<SettingsItemBase*>(index.internalPointer());
		if(pItem)
			return pItem->createEditor(parent);
	}

	// This should never happen!
	return new QWidget(parent);
}

void QLineEditDelegate::setEditorData(QWidget *editor,
		const QModelIndex& index) const {

	if (index.isValid()) {
		SettingsItemBase* pItem = static_cast<SettingsItemBase*>(index.internalPointer());
		if(pItem)
			pItem->setEditorData(editor,index);
	}
}

// Set the data in the model
void QLineEditDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
		const QModelIndex &index) const {

	if (index.isValid()) {
		SettingsItemBase* pItem = static_cast<SettingsItemBase*>(index.internalPointer());
		if(pItem){
			pItem->setModelData(editor,model,index);
		}
	}
}

void QLineEditDelegate::updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option, const QModelIndex &/* index */) const {

	editor->setGeometry(option.rect);

}

// Visual options - requests the item what to do
void QLineEditDelegate::paint(QPainter* painter, const QStyleOptionViewItem &option,
		const QModelIndex &index) const{

	if (index.isValid()) {
		SettingsItemBase* pItem = static_cast<SettingsItemBase*>(index.internalPointer());
		if(pItem)
			pItem->paint(painter,option,index);
	}

}
