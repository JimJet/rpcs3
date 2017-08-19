#include "pad_thread.h"
#include "rpcs3qt\gamepads_settings_dialog.h"
#include "../ds4_pad_handler.h"
#include "../xinput_pad_handler.h"
#include "../mm_joystick_handler.h"
#include "../keyboard_pad_handler.h"
#include "../Emu/Io/Null/NullPadHandler.h"


pad_thread::pad_thread(void *_curthread, void *_curwindow) : curthread(_curthread), curwindow(_curwindow)
{

}

pad_thread::~pad_thread()
{
	handlers.clear();
}

void pad_thread::Init(const u32 max_connect)
{
//	std::memset(&m_info, 0, sizeof(m_info));
//	m_info.max_connect = max_connect;
//	m_info.now_connect = std::min(max_connect, (u32)7);
//
//	input_cfg.load();
//
//	keyboard_pad_handler *ret;
//
//	for (u32 i = 0; i < m_info.now_connect; i++)
//	{
//		PadHandlerBase *cur_pad_handler;
//		if (i < 7)
//		{
//			if (handlers.count(input_cfg.player_input[i]) != 0)
//			{
//				cur_pad_handler = &handlers[input_cfg.player_input[i]];
//			}
//			else
//			{
//				switch (input_cfg.player_input[i])
//				{
//				case pad_handler::null:
//					cur_pad_handler = new NullPadHandler();
//					break;
//				case pad_handler::keyboard:
//					ret = new keyboard_pad_handler();
//					ret->moveToThread((QThread *)curthread);
//					ret->SetTargetWindow((QWindow *)curwindow);
//					cur_pad_handler = ret;
//					break;
//				case pad_handler::ds4:
//					cur_pad_handler = new ds4_pad_handler();
//					break;
//#ifdef _MSC_VER
//				case pad_handler::xinput:
//					cur_pad_handler = new xinput_pad_handler();
//					break;
//#endif
//#ifdef _WIN32
//				case pad_handler::mm:
//					cur_pad_handler = new mm_joystick_handler();
//					break;
//#endif
//#ifdef HAVE_LIBEVDEV
//				case pad_handler::evdev:
//					cur_pad_handler = new evdev_joystick_handler();
//					break;
//#endif
//				}
//				handlers.emplace(input_cfg.player_input[i], *cur_pad_handler);
//			}
//		}
//		else
//		{
//			//null handler for the rest
//			if (handlers.count(pad_handler::null) != 0)
//			{
//				cur_pad_handler = &handlers[pad_handler::null];
//			}
//			else
//			{
//				cur_pad_handler = new NullPadHandler();
//				handlers.emplace(input_cfg.player_input[i], *cur_pad_handler);
//			}
//		}
//		cur_pad_handler->Init();
//		list_pads.push_back(cur_pad_handler);
//		cur_pad_handler->bindPadToDevice(m_pads, input_cfg.player_device[i].to_string());
//	}
//
//	thread = new std::thread(&pad_thread::ThreadFunc, this);
}

void pad_thread::SetRumble(const u32 pad, u8 largeMotor, bool smallMotor) {
	if (pad > m_pads.size())
		return;

	m_pads[pad].m_vibrateMotors[0].m_value = largeMotor;
	m_pads[pad].m_vibrateMotors[1].m_value = smallMotor ? 255 : 0;
}

void pad_thread::ThreadFunc()
{
	active = true;
	while (active)
	{
		for (auto& cur_pad_handler : handlers)
		{
			//cur_pad_handler.second.ThreadProc();
		}
		std::this_thread::sleep_for(1ms);
	}
}
