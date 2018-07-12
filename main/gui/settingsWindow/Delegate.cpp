
/*    delegate.cpp

    A delegate that allows the user to change integer values from the model
    using a spin box widget.*/
#include "Delegate.h"

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


VPPSettingsDelegate::VPPSettingsDelegate(QObject *parent)
: QStyledItemDelegate(parent) {

}


QWidget* VPPSettingsDelegate::createEditor(QWidget* parent,
		const QStyleOptionViewItem& option,
		const QModelIndex& index) const {

	if (index.isValid()) {
		SettingsItemBase* pItem = static_cast<SettingsItemBase*>(index.internalPointer());
		if(pItem){
			QLineEdit* pCurrentEditor= dynamic_cast<QLineEdit*>(pItem->createEditor(parent));
			if(!pCurrentEditor)
				throw VPPException(HERE,"cannot cast editor to QLineEdit!");

			// This **should** guarantee that the text is registered into the model while
			// typed in. No need to close the editor. From MEMS+ LineEditDelegateHelper::createEditor
			connect(pCurrentEditor, SIGNAL(textChanged(QString)), this, SLOT(textEdited(QString)));
			return pCurrentEditor;
		}
	}

	// This should never happen!
	return new QWidget(parent);
}

void VPPSettingsDelegate::textEdited(QString dummy) {

	QLineEdit* pCurrentEditor = static_cast<QLineEdit*>(sender());

	// keep the position of the cursor
	int cursorPosition = pCurrentEditor->cursorPosition();
	emit commitData(pCurrentEditor);
	//emit editorTextEdited();

	QRect initialGeometry(pCurrentEditor->geometry());
	if((pCurrentEditor->fontMetrics().width(pCurrentEditor->text()) +
			pCurrentEditor->fontMetrics().width("0") -
			initialGeometry.width())>0){
		initialGeometry.setWidth(	pCurrentEditor->fontMetrics().width("0")+
														pCurrentEditor->fontMetrics().width(pCurrentEditor->text()));
	}

	pCurrentEditor->setGeometry(initialGeometry);
	pCurrentEditor->setCursorPosition(cursorPosition);
}

void VPPSettingsDelegate::setEditorData(QWidget *editor,const QModelIndex& index) const {

	if (index.isValid()) {
		SettingsItemBase* pItem = static_cast<SettingsItemBase*>(index.internalPointer());
		if(pItem)
			pItem->setEditorData(editor,index);
	}
}

// Set the data in the model
void VPPSettingsDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
		const QModelIndex &index) const {

	if (index.isValid()) {
		SettingsItemBase* pItem = static_cast<SettingsItemBase*>(index.internalPointer());
		if(pItem){
			pItem->setModelData(editor,model,index);
		}
	}
}

void VPPSettingsDelegate::updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option, const QModelIndex &/* index */) const {

	editor->setGeometry(option.rect);

}

// Visual options - requests the item what to do
void VPPSettingsDelegate::paint(QPainter* painter, const QStyleOptionViewItem &option,
		const QModelIndex &index) const{

	if (index.isValid()) {
		SettingsItemBase* pItem = static_cast<SettingsItemBase*>(index.internalPointer());
		if(pItem)
			pItem->paint(painter,option,index);
	}

}
