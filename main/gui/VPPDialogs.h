#ifndef STATEVECTORDIALOG_H
#define STATEVECTORDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>

class StateVectorDialog : public QDialog {

	Q_OBJECT

public:

	/// Explicit Ctor
	explicit StateVectorDialog(QWidget *parent = Q_NULLPTR);

	/// Returns the value of V entered by the user
    double getV() const;

	/// Returns the value of Phi entered by the user
    QString getPhi() const;

	/// Returns the value of Crew entered by the user
    QString getCrew() const;

	/// Returns the value of Flat entered by the user
    QString getFlat() const;

private:

    QLineEdit* pV_Edit_, *pPhi_Edit_, *pCrew_Edit_, *pFlat_Edit_;

};


class WindIndicesDialog : public QDialog {

	Q_OBJECT

public:

	/// Explicit Ctor
	explicit WindIndicesDialog(const int ntwv, const int ntwa, QWidget *parent = Q_NULLPTR);

	/// Returns the index of the TWV prompted by the user
	int getTWV() const;

	/// Returns the index of the TWA prompted by the user
	int getTWA() const;

private:

	QLineEdit* pTWV_Edit_, *pTWA_Edit_;

};
#endif
