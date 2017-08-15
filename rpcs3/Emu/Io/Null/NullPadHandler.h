#pragma once

#include "Emu/Io/PadHandler.h"

class NullPadHandler final : public PadHandlerBase
{
public:
	bool Init() override
	{
		return true;
	}

	std::vector<std::string> ListDevices() override
	{
		std::vector<std::string> nulllist;
		nulllist.push_back("Default Null Device");
		return nulllist;
	}

	bool bindPadToDevice(Pad *pad, const std::string& device) override
	{
		return true;
	}

	void ThreadProc() override
	{
	}

};
