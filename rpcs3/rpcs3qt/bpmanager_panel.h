#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>

class bpmanager_panel : public QDialog
{
	QTableWidget *list_bps;
	QLineEdit *t_search;
	QCheckBox *c_keepbps;
	QLineEdit *t_address;
	QComboBox *co_bptype;
	QLineEdit *t_bpnote;

	bool exit;
public:
	bpmanager_panel(QWidget* parent);
	~bpmanager_panel()
	{
		exit = true;
	}
};
