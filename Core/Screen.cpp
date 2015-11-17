#include "stdafx.h"
#include "Screen.h"
#include "Internal_Impls.h"

void SL::Remote_Access_Library::Screen_Capture::Reorder(std::vector<Screen_Info>& screens)
{
	//organize the monitors so that the ordering is left to right for displaying purposes
	std::sort(begin(screens), end(screens), [](const Screen_Info& i, const Screen_Info& j) { return i.Offsetx < j.Offsetx; });
	auto index = 0;
	for (auto& x : screens) x.Index = index++;
}
