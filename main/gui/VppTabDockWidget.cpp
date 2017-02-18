#include "VppTabDockWidget.h"

VppTabDockWidget::VppTabDockWidget(QWidget* parent /*=Q_NULLPTR*/, Qt::WindowFlags flags/*=0*/) :
			QDockWidget(parent, flags) {

}

VppTabDockWidget::~VppTabDockWidget() {

	// Send a signal : this item is being deleted
	itemDeleted(this);

}
