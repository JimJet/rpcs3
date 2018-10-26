#pragma once

#include "../../Utilities/types.h"
#include "Emu/Cell/PPUModule.h"
#include "Emu/Cell/PPUOpcodes.h"
#include <vector>

struct shle_pattern
{
	u16 start_pattern[32];
	u8 crc16_length;
	u16 crc16;
	u16 total_length;
	std::string module;
	std::string name;

	u32 fnid;
};

class StaticHleHandler
{
public:
	StaticHleHandler();
	~StaticHleHandler();

	bool LoadPatterns();
	bool CheckAgainstPatterns(vm::cptr<u8>& data, u32 size, u32 addr);

protected:
	uint16_t Gen_CRC16(const uint8_t* data_p, size_t length);

	std::vector<shle_pattern> hle_patterns;
};
