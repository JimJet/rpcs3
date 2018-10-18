#include "stdafx.h"
#include "StaticHLE.h"

logs::channel static_hle("static_hle");

extern void ppu_register_function_at(u32 addr, u32 size, ppu_function_t ptr);

vm::ptr<void> hle_memcpy(vm::ptr<void> dst, vm::cptr<void> src, u32 size)
{
	std::memcpy(dst.get_ptr(), src.get_ptr(), size);

	return dst;
}

vm::ptr<void> hle_memset(vm::ptr<void> dst, s32 value, u32 size)
{
	std::memset(dst.get_ptr(), value, size);

	return dst;
}

vm::ptr<void> hle_memmove(vm::ptr<void> dst, vm::ptr<void> src, u32 size)
{
	std::memmove(dst.get_ptr(), src.get_ptr(), size);

	return dst;
}

vm::ptr<void> hle_wmemcpy(vm::ptr<void> dst, vm::cptr<void> src, u32 size)
{
	std::wmemcpy((wchar_t*)dst.get_ptr(), (wchar_t*)src.get_ptr(), size);

	return dst;
}

vm::ptr<void> hle_wmemset(vm::ptr<void> dst, s32 value, u32 size)
{
	std::wmemset((wchar_t*)dst.get_ptr(), value, size);

	return dst;
}

vm::ptr<void> hle_wmemmove(vm::ptr<void> dst, vm::ptr<void> src, u32 size)
{
	std::wmemmove((wchar_t*)dst.get_ptr(), (wchar_t*)src.get_ptr(), size);
	return dst;
}

int hle_memcmp(vm::ptr<void> buf1, vm::ptr<void> buf2, u32 size)
{
	return std::memcmp(buf1.get_ptr(), buf2.get_ptr(), size);
}

int hle_wmemcmp(vm::ptr<void> buf1, vm::ptr<void> buf2, u32 size)
{
	return std::wmemcmp((wchar_t*)buf1.get_ptr(), (wchar_t*)buf2.get_ptr(), size);
}

DECLARE(ppu_module_manager::static_hle) ("static_hle", []()
{
	REG_FNID(static_hle, "hle_memset", hle_memset);
	REG_FNID(static_hle, "hle_memcpy", hle_memcpy);
	REG_FNID(static_hle, "hle_memmove", hle_memmove);
	REG_FNID(static_hle, "hle_wmemset", hle_wmemset);
	REG_FNID(static_hle, "hle_wmemcpy", hle_wmemcpy);
	REG_FNID(static_hle, "hle_wmemmove", hle_wmemmove);
	REG_FNID(static_hle, "hle_memcmp", hle_memcmp);
	REG_FNID(static_hle, "hle_wmemcmp", hle_wmemcmp);
});

StaticHleHandler::StaticHleHandler()
{
}

StaticHleHandler::~StaticHleHandler()
{
}

bool StaticHleHandler::LoadPatterns(std::string& filename)
{
	fs::file patfile(filename);

	if (!patfile)
		return false;

	static_hle.error("Loading pattern file:%s", filename);

	const auto patterns = fmt::split(patfile.to_string(), {"\r\n"});

	for (int i = 0; i < patterns.size(); i++)
	{
		const auto pattern = fmt::split(patterns[i], {" "});

		if (pattern.size() < 5)
		{
			static_hle.error("[%d]:Invalid pattern with less than 5 elements", i);
			continue;
		}
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
		dapat.name         = pattern[4];

		dapat.fnid = ppu_generate_id(dapat.name.c_str());

		static_hle.success("Added a pattern for %s(id:0x%X)", dapat.name, dapat.fnid);
		hle_patterns.push_back(std::move(dapat));
	}

	return true;
}

#define POLY 0x8408

uint16_t gen_crc16(const uint8_t* data_p, size_t length)
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

		//	check start pattern
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

		//	start pattern ok, checking middle part
		if (pat.crc16_length != 0)
			if (gen_crc16(&data[32], pat.crc16_length) != pat.crc16)
				continue;

		//	we got a match!
		static_hle.success("Found function %s at 0x%x", pat.name, addr);

		//	Patch the code
		const auto smodule = ppu_module_manager::get_module("static_hle");

		if (smodule == nullptr)
		{
			static_hle.error("Couldn't find static_hle module");
			return false;
		}

		const auto sfunc   = &smodule->functions.at(pat.fnid);
		const u32 _entry   = addr;
		const u32 target   = ppu_function_manager::addr + 8 * sfunc->index;

		const auto& hle_funcs = ppu_function_manager::get();

		static_hle.success("Target:0x%x, sfunc->index:0x%x", target, sfunc->index);

		//if ((target <= _entry && _entry - target <= 0x2000000) || (target > _entry && target - _entry < 0x2000000))
		//{
		//	// Use relative branch
		//	vm::write32(_entry, ppu_instructions::B(target - _entry));
		//	ppu_instructions::BCCTR()
		//}
		//else if (target < 0x2000000)
		//{
		//	// Use absolute branch if possible
		//	vm::write32(_entry, ppu_instructions::B(target, true));
		//}
		//else
		//{
		//	static_hle.fatal("Failed to patch function at 0x%x (0x%x)", _entry, target);
		//	return false;
		//}

		//Write stub
		vm::write32(_entry, ppu_instructions::LI(0, 0));
		vm::write32(_entry+4, ppu_instructions::LIS(0, (target&0xFFFF0000)>>16));
		vm::write32(_entry+8, ppu_instructions::ORI(0, 0, target&0xFFFF));
		vm::write32(_entry+12, ppu_instructions::MTCTR(0));
		vm::write32(_entry+16, ppu_instructions::BCTR());

		return true;
	}

	return false;
}
