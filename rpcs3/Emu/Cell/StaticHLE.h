#pragma once

#include "../../Utilities/types.h"
#include "../Cell/PPUFunction.h"
#include "../../Utilities/File.h"
#include "../../Utilities/StrUtil.h"
#include "../System.h"
//#include "Memory/Memory.h"
#include "Emu/Cell/PPUModule.h"
#include "Emu/Cell/PPUOpcodes.h"
#include <vector>

struct shle_pattern
{
	u16 start_pattern[32];
	u8 crc16_length;
	u16 crc16;
	u16 total_length;
	std::string name;

	u32 fnid;
};

class StaticHleHandler
{
public:
	StaticHleHandler();
	~StaticHleHandler();

	bool LoadPatterns(std::string& filename);
	bool CheckAgainstPatterns(vm::cptr<u8>& data, u32 size, u32 addr);

protected:
	std::vector<shle_pattern> hle_patterns;
};
