#ifndef STATEVECTORDIALOG_H
#define STATEVECTORDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QGridLayout>
#include "boost/shared_ptr.hpp"
#include <Eigen/Core>
#include "VppAeroItem.h"

class StateVectorDialog : public QDialog {

		Q_OBJECT

	public:

		/// Returns the value of V entered by the user
		double getV() const;

		/// Returns the value of Phi entered by the user
		double getPhi() const;

		/// Returns the value of Crew entered by the user
		double getCrew() const;

		/// Returns the value of Flat entered by the user
		double getFlat() const;

		/// Return the state vector under the form of a VectorXd
		Eigen::VectorXd getStateVector() const;

	protected:

		/// Explicit protected Ctor
		explicit StateVectorDialog(QWidget *parent = Q_NULLPTR);

		boost::shared_ptr<QLineEdit> pV_Edit_, pPhi_Edit_, pCrew_Edit_, pFlat_Edit_;

		boost::shared_ptr<QGridLayout> pGridLayout_;
};

//---------------------------------------------------------------

class FullStateVectorDialog : public StateVectorDialog {

		Q_OBJECT

	public:

		/// Explicit Ctor
		explicit FullStateVectorDialog(QWidget *parent = Q_NULLPTR);

};

//---------------------------------------------------------------

class OptimVarsStateVectorDialog : public StateVectorDialog {

		Q_OBJECT

	public:

		/// Explicit Ctor
		explicit OptimVarsStateVectorDialog(QWidget *parent = Q_NULLPTR);

};

//=========================================================================


class WindIndicesDialog : public QDialog {

		Q_OBJECT

	public:

		/// Explicit Ctor with a wind item
		explicit WindIndicesDialog(WindItem*, QWidget *parent = Q_NULLPTR);

		/// Returns the index of the TWV prompted by the user
		int getTWV() const;

		/// Returns the index of the TWA prompted by the user
		int getTWA() const;

		/// Return a ptr to the wind
		WindItem* getWind() const;

	private:

		/// raw ptr to the wind
		WindItem* pWind_;

		boost::shared_ptr<QLineEdit> pTWV_Edit_, pTWA_Edit_;

};
#endif
