#include "stdafx.h"
#include "bpmanager_panel.h"
#include "breakpoint.h"
#include "emu_settings.h"

std::array<std::vector<breakpoint>, 5> breakpoints_list;


static const std::string bp_names[] = { "BPX", "BPMB", "BPMH", "BPMW", "BPMD" };

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
	t_addr = new QLineEdit();
	t_addr->setPlaceholderText("Address");
	hbox_bp_opt->addWidget(t_addr);
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

	connect(b_addbp, &QAbstractButton::clicked, add_breakpoint);
}

void bpmanager_panel::refresh_list()
{
	u32 index = 0, type=0;
	list_bps->clear();

	auto l_GetItem = [](const std::string& text)
	{
		QTableWidgetItem* curr = new QTableWidgetItem;
		curr->setFlags(curr->flags() & ~Qt::ItemIsEditable);
		curr->setText(qstr(text));
		return curr;
	};

	for (auto& bp_list : breakpoints_list)
	{
		for (auto& breakpoint : bp_list)
		{
			list_bps->setItem(index, 0, l_GetItem(bp_names[type]));
			list_bps->setItem(index, 1, l_GetItem(fmt::format("%08x", breakpoint.addr)));
			list_bps->setItem(index, 2, l_GetItem(breakpoint.s_note));
		}
	}
}

void bpmanager_panel::add_breakpoint()
{
	bool ok;
	u32 addr = t_addr->text().toULong(&ok, 16);
	if (ok == false || addr==0)
	{
		t_addr->setText("");
		return;
	}
	u32 type = co_bptype->currentIndex();

	//Check if it already exist before adding
	for (auto& bp : breakpoints_list[type])
	{
		if (bp.addr == addr)
		{
			//Update the note if it exists
			bp.s_note = t_bpnote->text().toStdString();
			refresh_list();
			return;
		}
	}

	breakpoint new_bp;
	new_bp.addr = addr;
	new_bp.s_note = t_bpnote->text().toStdString();
	breakpoints_list[type].push_back(std::move(new_bp));
	refresh_list();
}