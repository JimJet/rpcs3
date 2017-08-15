#include "gamepads_settings_dialog.h"
#include "emu_settings.h"
#include "../Emu/System.h"

gamepads_settings_dialog::gamepads_settings_dialog(QWidget* parent)
	: QDialog(parent)
{
	setWindowTitle(tr("Gamepads Settings"));
	setAttribute(Qt::WA_DeleteOnClose);

	QVBoxLayout *dialog_layout = new QVBoxLayout();
	QHBoxLayout *all_players = new QHBoxLayout();

	std::shared_ptr<emu_settings> xemu_settings;

	xemu_settings.reset(new emu_settings(""));

	for (int i = 0; i < 7; i++)
	{
		QGroupBox *grp_player = new QGroupBox(QString(tr("Player %1").arg(i+1)));

		QVBoxLayout *ppad_layout = new QVBoxLayout();

		co_inputtype[i] = new QComboBox();
		co_inputtype[i]->addItems(xemu_settings->GetSettingOptions(emu_settings::PadHandler));
		co_inputtype[i]->setFixedWidth(200);
		ppad_layout->addWidget(co_inputtype[i]);

		co_deviceID[i] = new QComboBox();
		co_deviceID[i]->setFixedWidth(200);
		co_deviceID[i]->setEnabled(false);
		ppad_layout->addWidget(co_deviceID[i]);

		QPushButton *config_button = new QPushButton();
		config_button->setText("Config");
		ppad_layout->addWidget(config_button);

		grp_player->setLayout(ppad_layout);
		all_players->addWidget(grp_player);

		connect(co_inputtype[i], &QComboBox::currentTextChanged, [=] { ChangeInputType(i); });
	}

	dialog_layout->addLayout(all_players);
	QPushButton *ok_button = new QPushButton();
	ok_button->setText("OK");
	dialog_layout->addWidget(ok_button);

	setLayout(dialog_layout);
}

void gamepads_settings_dialog::ChangeInputType(int player)
{
	int new_index;
	new_index = co_inputtype[player]->currentIndex();

	switch (new_index)
	{
	case pad_handler::null: return std::make_shared<NullPadHandler>();
	case pad_handler::keyboard:
	{
		keyboard_pad_handler* ret = new keyboard_pad_handler();
		ret->moveToThread(thread());
		ret->SetTargetWindow(gameWindow);
		return std::shared_ptr<PadHandlerBase>(ret);
	}
	case pad_handler::ds4: return std::make_shared<ds4_pad_handler>();
#ifdef _MSC_VER
	case pad_handler::xinput: return std::make_shared<xinput_pad_handler>();
#endif
#ifdef _WIN32
	case pad_handler::mm: return std::make_shared<mm_joystick_handler>();
#endif
#ifdef HAVE_LIBEVDEV
	case pad_handler::evdev: return std::make_shared<evdev_joystick_handler>();
#endif
	default: fmt::throw_exception("Invalid pad handler: %s", type);
	}

}
