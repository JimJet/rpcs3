#pragma once

#include "Emu/Io/PadHandler.h"

class NullPadHandler final : public PadHandlerBase
{
public:
	void Init() override
	{
		//memset(&m_info, 0, sizeof(PadInfo));
		//m_info.max_connect = 7;
		//m_pads.clear();
	}

	std::vector<std::string> ListDevices() override
	{
		std::vector<std::string> nulllist;
		nulllist.push_back("Default Null Device");
		return nulllist;
	}
	void bindPadToDevice(std::vector<Pad> &pads, std::string& device) override
	{
	}

	void ThreadProc() override
	{
	}

};
