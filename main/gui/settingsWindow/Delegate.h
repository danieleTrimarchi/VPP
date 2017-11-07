#ifndef DELEGATE_H
#define DELEGATE_H

#include <QStyledItemDelegate>
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>

class SpinBoxDelegate : public QStyledItemDelegate {

		Q_OBJECT

	public:
		SpinBoxDelegate(QObject *parent = 0);

		QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
				const QModelIndex &index) const override;

		void setEditorData(QWidget *editor, const QModelIndex &index) const override;
		void setModelData(QWidget *editor, QAbstractItemModel *model,
				const QModelIndex &index) const override;

		virtual void updateEditorGeometry(QWidget *editor,
				const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

/// ---------------------------------------------------------------

class QLineEditDelegate : public QStyledItemDelegate {

		Q_OBJECT

	public:

		/// Ctor
		QLineEditDelegate(QObject *parent = 0);

		/// Create an editor for editing the data in the model
		QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
				const QModelIndex &index) const override;

		/// Edit the data in the editor
		void setEditorData(QWidget *editor, const QModelIndex &index) const override;

		/// Set the data in the model
		void setModelData(QWidget *editor, QAbstractItemModel *model,
				const QModelIndex &index) const override;

		/// Make sure the editor matches the size of the contained data
		void updateEditorGeometry(QWidget *editor,
				const QStyleOptionViewItem &option, const QModelIndex &index) const override;

		/// Visual options - this directly derives from MEMS+
		virtual void paint(QPainter* painter, const QStyleOptionViewItem &option,
				const QModelIndex &index) const override;

	private:

};

#endif


