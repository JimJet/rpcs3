#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>

class gamepads_settings_dialog : public QDialog
{
protected:
	void ChangeInputType(int player);

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
