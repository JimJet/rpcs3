#include "gamepads_settings_dialog.h"
#include "../Emu/Io/PadHandler.h"
#include "../ds4_pad_handler.h"
#include "../xinput_pad_handler.h"
#include "../mm_joystick_handler.h"
#include "../keyboard_pad_handler.h"
#include "../Emu/Io/Null/NullPadHandler.h"
//#include "emu_settings.h"
#include "../Emu/System.h"

input_config input_cfg;

gamepads_settings_dialog::gamepads_settings_dialog(QWidget* parent)
	: QDialog(parent)
{
	setWindowTitle(tr("Gamepads Settings"));
	//setAttribute(Qt::WA_DeleteOnClose);

	QVBoxLayout *dialog_layout = new QVBoxLayout();
	QHBoxLayout *all_players = new QHBoxLayout();

	input_cfg.load();

	auto fill_device_combo = [](QComboBox *combo)
	{
		std::vector<std::string> str_inputs = input_cfg.player_input[0].to_list();
		for (int index = 0; index < str_inputs.size(); index++)
		{
			combo->addItem(str_inputs[index].c_str());
		}
	};

	for (int i = 0; i < 7; i++)
	{
		QGroupBox *grp_player = new QGroupBox(QString(tr("Player %1").arg(i+1)));

		QVBoxLayout *ppad_layout = new QVBoxLayout();

		co_inputtype[i] = new QComboBox();
		fill_device_combo(co_inputtype[i]);
		ppad_layout->addWidget(co_inputtype[i]);

		co_deviceID[i] = new QComboBox();
		co_deviceID[i]->setEnabled(false);
		ppad_layout->addWidget(co_deviceID[i]);

		QHBoxLayout *button_layout = new QHBoxLayout();
		QPushButton *config_button = new QPushButton();
		config_button->setText("Config");
		button_layout->addSpacing(config_button->sizeHint().width()*0.50f);
		button_layout->addWidget(config_button);
		button_layout->addSpacing(config_button->sizeHint().width()*0.50f);
		ppad_layout->addLayout(button_layout);

		grp_player->setLayout(ppad_layout);
		all_players->addWidget(grp_player);

		connect(co_inputtype[i], &QComboBox::currentTextChanged, [=] { ChangeInputType(i); });
		connect(co_deviceID[i], &QComboBox::currentTextChanged, [=] { ChangeDevice(i); });
	}

	dialog_layout->addLayout(all_players);
	QPushButton *ok_button = new QPushButton();
	ok_button->setText("CELL_OK");
	dialog_layout->addWidget(ok_button);

	setLayout(dialog_layout);

	connect(ok_button, &QPushButton::pressed, this, &gamepads_settings_dialog::SaveExit);

	//Set the values from config
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < co_inputtype[i]->count(); j++)
		{
			if (co_inputtype[i]->itemText(j).toStdString() == input_cfg.player_input[i].to_string())
			{
				co_inputtype[i]->setCurrentIndex(j);
				ChangeInputType(i);
				break;
			}
		}

		for (int j = 0; j < co_deviceID[i]->count(); j++)
		{
			if (co_deviceID[i]->itemText(j).toStdString() == input_cfg.player_device[i].to_string())
			{
				co_deviceID[i]->setCurrentIndex(j);
				ChangeDevice(i);
				break;
			}
		}
	}
}

void gamepads_settings_dialog::SaveExit()
{
	//Check for invalid selection
	for (int i = 0; i < 7; i++)
	{
		if (co_deviceID[i]->currentData() == -1)
		{
			input_cfg.player_input[i].from_default();
			input_cfg.player_device[i].from_default();
		}
	}

	input_cfg.save();

	QDialog::accept();
}

void gamepads_settings_dialog::ChangeDevice(int player)
{
	bool success;

	success = input_cfg.player_device[player].from_string(co_deviceID[player]->currentText().toStdString());

	if (!success)
	{
		//Something went wrong
	}
}

void gamepads_settings_dialog::ChangeInputType(int player)
{
	bool success;

	success = input_cfg.player_input[player].from_string(co_inputtype[player]->currentText().toStdString());

	if (!success)
	{
		//Something went wrong
	}

	std::unique_ptr<PadHandlerBase> cur_pad_handler;

	switch (input_cfg.player_input[player])
	{
	case pad_handler::null:
		cur_pad_handler = std::make_unique<NullPadHandler>();
		break;
	case pad_handler::keyboard:
		cur_pad_handler = std::make_unique<keyboard_pad_handler>();
		break;
	case pad_handler::ds4:
		cur_pad_handler = std::make_unique<ds4_pad_handler>();
		break;
#ifdef _MSC_VER
	case pad_handler::xinput:
		cur_pad_handler = std::make_unique<xinput_pad_handler>();
		break;
#endif
#ifdef _WIN32
	case pad_handler::mm:
		cur_pad_handler = std::make_unique<mm_joystick_handler>();
		break;
#endif
#ifdef HAVE_LIBEVDEV
	case pad_handler::evdev:
		cur_pad_handler = std::make_unique<evdev_joystick_handler>();
		break;
#endif
	}

	std::vector<std::string> list_devices = cur_pad_handler->ListDevices();

	co_deviceID[player]->clear();

	for (int i = 0; i < list_devices.size(); i++) co_deviceID[player]->addItem(list_devices[i].c_str(), i);

	if (list_devices.size() == 0)
	{
		co_deviceID[player]->addItem("No Device Detected", -1);
		co_deviceID[player]->setEnabled(false);
	}
	else
	{
		co_deviceID[player]->setEnabled(true);
	}
}

