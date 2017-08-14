#pragma once

#include "Utilities/types.h"
#include <string>
#include <vector>

enum BreakpointType : u8
{
	bp_bpx,
	bp_bpmb,
	bp_bpmh,
	bp_bpmw,
	bp_bpmd,
};

extern std::array<std::map<u32, std::string>, 5> breakpoints_list;
extern bool break_on_bpm;
