#include "stdafx.h"
#include "bpmanager_panel.h"

bpmanager_panel::bpmanager_panel(QWidget* parent)
	: QDialog(parent)
{
	setWindowTitle(tr("Breakpoints Manager"));
	setAttribute(Qt::WA_DeleteOnClose);

	//Layout
	QVBoxLayout *vbox_panel = new QVBoxLayout();

	//Search edit&Keep breakpoints checkbox
	QHBoxLayout *hbox_top = new QHBoxLayout();
	t_search = new QLineEdit(this);
	t_search->setPlaceholderText("Search here");
	t_search->setFocus();
	hbox_top->addWidget(t_search);
	c_keepbps = new QCheckBox();
	c_keepbps->setText("Keep BPs on restart");
	hbox_top->addWidget(c_keepbps, Qt::AlignRight);
	vbox_panel->addLayout(hbox_top);

	//List of breakpoints
	list_bps = new QTableWidget();
	list_bps->setShowGrid(true);
	//list_bps->setItemDelegate(new table_item_delegate(this));
	list_bps->setSelectionBehavior(QAbstractItemView::SelectRows);
	list_bps->setSelectionMode(QAbstractItemView::SingleSelection);
	list_bps->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
	list_bps->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	list_bps->verticalHeader()->setVisible(false);
	list_bps->horizontalHeader()->setContextMenuPolicy(Qt::NoContextMenu);
	list_bps->horizontalHeader()->setHighlightSections(false);
	list_bps->horizontalHeader()->setSortIndicatorShown(true);
	list_bps->horizontalHeader()->setStretchLastSection(true);
	list_bps->horizontalHeader()->setDefaultSectionSize(100);
	list_bps->setAlternatingRowColors(true);
	//Define columns
	list_bps->setColumnCount(3);
	list_bps->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Type")));
	list_bps->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Address")));
	list_bps->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Note")));
	list_bps->setRowCount(10);
	vbox_panel->addWidget(list_bps);

	//Breakpoint Options
	QGroupBox *bp_opt = new QGroupBox(tr("Breakpoint Options"));
	QHBoxLayout* hbox_bp_opt = new QHBoxLayout();
	co_bptype = new QComboBox();
	QStringList breakpoint_types;
	breakpoint_types << "BPX" << "BPMB" << "BPMH" << "BPMW" << "BPMD";
	co_bptype->addItems(breakpoint_types);
	hbox_bp_opt->addWidget(co_bptype);
	t_address = new QLineEdit();
	t_address->setPlaceholderText("Address");
	hbox_bp_opt->addWidget(t_address);
	t_bpnote = new QLineEdit();
	t_bpnote->setPlaceholderText("Note");
	hbox_bp_opt->addWidget(t_bpnote);
	QPushButton* b_addbp = new QPushButton(tr("Add"), this);
	hbox_bp_opt->addWidget(b_addbp);
	bp_opt->setLayout(hbox_bp_opt);
	vbox_panel->addWidget(bp_opt);

	setLayout(vbox_panel);

	QSize tableSize = QSize(
		list_bps->verticalHeader()->width() + list_bps->horizontalHeader()->length() + list_bps->frameWidth() * 2,
		list_bps->horizontalHeader()->height() + list_bps->verticalHeader()->length() + list_bps->frameWidth() * 2);
	resize(minimumSize().expandedTo(sizeHint() - list_bps->sizeHint() + tableSize));


}
