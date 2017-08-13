#pragma once

#include "Utilities/types.h"
#include <string>
#include <vector>

enum BreakpointType : u8
{
	bpx,
	bpmb,
	bpmh,
	bpmw,
	bpmd,
};

extern std::array<std::map<u32, std::string>, 5> breakpoints_list;
