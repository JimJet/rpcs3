#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>

#include "../Emu/System.h"
#include "../../Utilities/Config.h"
#include "../../Utilities/File.h"

struct input_config final : cfg::node
{
	const std::string cfg_name = fs::get_config_dir() + "/config_input.yml";

	cfg::_enum<pad_handler> player_input[7] = {
		{ this, "Player 1 Input", pad_handler::keyboard },
		{ this, "Player 2 Input", pad_handler::null },
		{ this, "Player 3 Input", pad_handler::null },
		{ this, "Player 4 Input", pad_handler::null },
		{ this, "Player 5 Input", pad_handler::null },
		{ this, "Player 6 Input", pad_handler::null },
		{ this, "Player 7 Input", pad_handler::null } };

	cfg::string player_device[7] = {
		{ this, "Player 1 Device", "" },
		{ this, "Player 2 Device", "" },
		{ this, "Player 3 Device", "" },
		{ this, "Player 4 Device", "" },
		{ this, "Player 5 Device", "" },
		{ this, "Player 6 Device", "" },
		{ this, "Player 7 Device", "" } };

	bool load()
	{
		if (fs::file cfg_file{ cfg_name, fs::read })
		{
			return from_string(cfg_file.to_string());
		}

		return false;
	}

	void save()
	{
		fs::file(cfg_name, fs::rewrite).write(to_string());
	}

	bool exist()
	{
		return fs::is_file(cfg_name);
	}
};

extern input_config input_cfg;

class gamepads_settings_dialog : public QDialog
{
protected:
	void ChangeInputType(int player);
	void ChangeDevice(int player);
	void SaveExit();

protected:
	QComboBox *co_inputtype[7];
	QComboBox *co_deviceID[7];

	bool exit;

public:
	gamepads_settings_dialog(QWidget* parent);
	~gamepads_settings_dialog()
	{
		exit = true;
	}
};
