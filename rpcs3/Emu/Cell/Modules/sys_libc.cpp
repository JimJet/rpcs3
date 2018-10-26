#include "stdafx.h"
#include "Emu/System.h"
#include "Emu/Cell/PPUModule.h"

LOG_CHANNEL(sys_libc);

vm::ptr<void> sys_libc_memcpy(vm::ptr<void> dst, vm::cptr<void> src, u32 size)
{
	sys_libc.trace("memcpy(dst=*0x%x, src=*0x%x, size=0x%x)", dst, src, size);

	::memcpy(dst.get_ptr(), src.get_ptr(), size);
	return dst;
}
vm::ptr<void> sys_libc_memset(vm::ptr<void> dst, s32 value, u32 size)
{
	sys_libc.trace("memset(dst=*0x%x, value=0x%x, size=0x%x)", dst, value, size);

	::memset(dst.get_ptr(), value, size);
	return dst;
}

vm::ptr<void> sys_libc_memmove(vm::ptr<void> dst, vm::ptr<void> src, u32 size)
{
	sys_libc.trace("memmove(dst=*0x%x, src=*0x%x, size=0x%x)", dst, src, size);

	::memmove(dst.get_ptr(), src.get_ptr(), size);
	return dst;
}

u32 sys_libc_memcmp(vm::ptr<void> buf1, vm::ptr<void> buf2, u32 size)
{
	sys_libc.trace("memcmp(buf1=*0x%x, buf2=*0x%x, size=0x%x)", buf1, buf2, size);

	return ::memcmp(buf1.get_ptr(), buf2.get_ptr(), size);
}

vm::ptr<void> sys_libc_wmemcpy(vm::ptr<void> dst, vm::cptr<void> src, u32 size)
{
	sys_libc.trace("wmemcpy(dst=*0x%x, src=*0x%x, size=0x%x)", dst, src, size);

	::wmemcpy((wchar_t*)dst.get_ptr(), (wchar_t*)src.get_ptr(), size);
	return dst;
}

vm::ptr<void> sys_libc_wmemset(vm::ptr<void> dst, s32 value, u32 size)
{
	sys_libc.trace("wmemset(dst=*0x%x, value=0x%x, size=0x%x)", dst, value, size);

	::wmemset((wchar_t*)dst.get_ptr(), value, size);
	return dst;
}

vm::ptr<void> sys_libc_wmemmove(vm::ptr<void> dst, vm::ptr<void> src, u32 size)
{
	sys_libc.trace("wmemmove(dst=*0x%x, src=*0x%x, size=0x%x)", dst, src, size);

	::wmemmove((wchar_t*)dst.get_ptr(), (wchar_t*)src.get_ptr(), size);
	return dst;
}

u32 sys_libc_wmemcmp(vm::ptr<void> buf1, vm::ptr<void> buf2, u32 size)
{
	sys_libc.trace("wmemcmp(buf1=*0x%x, buf2=*0x%x, size=0x%x)", buf1, buf2, size);

	return ::wmemcmp((wchar_t*)buf1.get_ptr(), (wchar_t*)buf2.get_ptr(), size);
}


DECLARE(ppu_module_manager::sys_libc)("sys_libc", []()
{
	REG_FNID(sys_libc, "memcpy", sys_libc_memcpy)/*.flag(MFF_FORCED_HLE)*/;
	REG_FNID(sys_libc, "memset", sys_libc_memset);
	REG_FNID(sys_libc, "memmove", sys_libc_memmove);
	REG_FNID(sys_libc, "memcmp", sys_libc_memcmp);
	REG_FNID(sys_libc, "wmemcpy", sys_libc_wmemcpy);
	REG_FNID(sys_libc, "wmemset", sys_libc_wmemset);
	REG_FNID(sys_libc, "wmemmove", sys_libc_wmemmove);
	REG_FNID(sys_libc, "wmemcmp", sys_libc_wmemcmp);
});
