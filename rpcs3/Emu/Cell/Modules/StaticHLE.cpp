#include "stdafx.h"
#include "StaticHLE.h"

logs::channel static_hle("static_hle");

//For future use
DECLARE(ppu_module_manager::static_hle) ("static_hle", []()
{
});

std::vector<std::array<std::string, 6>> shle_patterns_list
{
	{ "2BA5000778630020788400207C6B1B78419D00702C2500004D82002028A5000F", "FF", "36D0", "05C4", "sys_libc", "memcpy" },
	{ "2BA5000778630020788400207C6B1B78419D00702C2500004D82002028A5000F", "5C", "87A0", "05C4", "sys_libc", "memcpy" },
	{ "2B8500077CA32A14788406207C6A1B78409D009C3903000198830000788B45E4", "B4", "1453", "00D4", "sys_libc", "memset" },
	{ "280500087C661B7840800020280500004D8200207CA903A69886000038C60001", "F8", "F182", "0118", "sys_libc", "memset" },
	{ "2B8500077CA32A14788406207C6A1B78409D009C3903000198830000788B45E4", "70", "DFDA", "00D4", "sys_libc", "memset" },
	{ "7F832000FB61FFD8FBE1FFF8FB81FFE0FBA1FFE8FBC1FFF07C7B1B787C9F2378", "FF", "25B5", "12D4", "sys_libc", "memmove" },
	{ "2F8500007C691B78419E002078A500207CA903A6A004000038840002B0030000", "10", "C255", "0030", "sys_libc", "wmemcpy" },
	{ "2F8500007C601B78419E001878A500207CA903A6B0830000386300024200FFF8", "08", "527B", "0028", "sys_libc", "wmemset" },
	{ "7F832040409D004C54AB083C7D045A147F834040409C003C2F850000419E002C", "60", "62EE", "0080", "sys_libc", "wmemmove" },
	{ "2B850007409D00B07C6923785520077E2F800000409E00ACE8030000E9440000", "FF", "71F1", "0158", "sys_libc", "memcmp" },
	{ "280500007CE32050788B0760418200E028850100786A07607C2A580040840210", "FF", "87F2", "0470", "sys_libc", "memcmp" },
	{ "2B850007409D00B07C6923785520077E2F800000409E00ACE8030000E9440000", "68", "EF18", "0158", "sys_libc", "memcmp" },
	{ "2F850000419E0048A123000078A50020A00400007CA903A67F890000409E0018", "38", "BB15", "0058", "sys_libc", "wmemcmp" },
};

StaticHleHandler::StaticHleHandler()
{
	LoadPatterns();
}

StaticHleHandler::~StaticHleHandler()
{
}

bool StaticHleHandler::LoadPatterns()
{
	for (int i = 0; i < shle_patterns_list.size(); i++)
	{
		auto& pattern = shle_patterns_list[i];

		if (pattern[0].size() != 64)
		{
			static_hle.error("[%d]:Start pattern length != 64", i);
			continue;
		}
		if (pattern[1].size() != 2)
		{
			static_hle.error("[%d]:Crc16_length != 2", i);
			continue;
		}
		if (pattern[2].size() != 4)
		{
			static_hle.error("[d]:Crc16 length != 4", i);
			continue;
		}
		if (pattern[3].size() != 4)
		{
			static_hle.error("[d]:Total length != 4", i);
			continue;
		}

		shle_pattern dapat;

		auto char_to_u8 = [&](u8 char1, u8 char2) -> u16 {
			u8 hv, lv;
			if (char1 == '.' && char2 == '.')
				return 0xFFFF;

			if (char1 == '.' || char2 == '.')
			{
				static_hle.error("[%d]:Broken byte pattern", i);
				return -1;
			}

			hv = char1 > '9' ? char1 - 'A' + 10 : char1 - '0';
			lv = char2 > '9' ? char2 - 'A' + 10 : char2 - '0';

			return (hv << 4) | lv;
		};

		for (int j = 0; j < 32; j++)
			dapat.start_pattern[j] = char_to_u8(pattern[0][j * 2], pattern[0][(j * 2) + 1]);

		dapat.crc16_length = char_to_u8(pattern[1][0], pattern[1][1]);
		dapat.crc16        = (char_to_u8(pattern[2][0], pattern[2][1]) << 8) | char_to_u8(pattern[2][2], pattern[2][3]);
		dapat.total_length = (char_to_u8(pattern[3][0], pattern[3][1]) << 8) | char_to_u8(pattern[3][2], pattern[3][3]);
		dapat.module       = pattern[4];
		dapat.name         = pattern[5];

		dapat.fnid = ppu_generate_id(dapat.name.c_str());

		static_hle.success("Added a pattern for %s(id:0x%X)", dapat.name, dapat.fnid);
		hle_patterns.push_back(std::move(dapat));
	}

	return true;
}

#define POLY 0x8408

uint16_t StaticHleHandler::Gen_CRC16(const uint8_t* data_p, size_t length)
{
	unsigned char i;
	unsigned int data;

	if (length == 0)
		return 0;
	unsigned int crc = 0xFFFF;
	do
	{
		data = *data_p++;
		for (i = 0; i < 8; i++)
		{
			if ((crc ^ data) & 1)
				crc = (crc >> 1) ^ POLY;
			else
				crc >>= 1;
			data >>= 1;
		}
	} while (--length != 0);

	crc  = ~crc;
	data = crc;
	crc  = (crc << 8) | ((data >> 8) & 0xff);
	return (unsigned short)(crc);
}

bool StaticHleHandler::CheckAgainstPatterns(vm::cptr<u8>& data, u32 size, u32 addr)
{
	for (auto& pat : hle_patterns)
	{
		if (size < pat.total_length)
			continue;

		// check start pattern
		int i = 0;
		for (i = 0; i < 32; i++)
		{
			if (pat.start_pattern[i] == 0xFFFF)
				continue;
			if (data[i] != pat.start_pattern[i])
				break;
		}

		if (i != 32)
			continue;

		// start pattern ok, checking middle part
		if (pat.crc16_length != 0)
			if (Gen_CRC16(&data[32], pat.crc16_length) != pat.crc16)
				continue;

		// we got a match!
		static_hle.success("Found function %s at 0x%x", pat.name, addr);

		// patch the code
		const auto smodule = ppu_module_manager::get_module(pat.module);

		if (smodule == nullptr)
		{
			static_hle.error("Couldn't find module: %s", pat.module);
			return false;
		}

		const auto sfunc   = &smodule->functions.at(pat.fnid);
		const u32 target   = ppu_function_manager::addr + 8 * sfunc->index;

		// write stub
		vm::write32(addr, ppu_instructions::LI(0, 0));
		vm::write32(addr+4, ppu_instructions::LIS(0, (target&0xFFFF0000)>>16));
		vm::write32(addr+8, ppu_instructions::ORI(0, 0, target&0xFFFF));
		vm::write32(addr+12, ppu_instructions::MTCTR(0));
		vm::write32(addr+16, ppu_instructions::BCTR());

		return true;
	}

	return false;
}
