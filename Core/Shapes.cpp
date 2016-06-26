#include "stdafx.h"
#include "Shapes.h"
#include <ostream>


std::ostream& operator<<(std::ostream& os, const SL::Remote_Access_Library::Utilities::Point& p)
{
	os << "X=" << p.X << ", Y=" << p.Y;
	return os;
}
std::ostream& operator<<(std::ostream& os, const SL::Remote_Access_Library::Utilities::Rect& r)
{
	os << "X=" << r.Origin.X << ", Y=" << r.Origin.Y << " Height=" << r.Height << ", Width=" << r.Width;
	return os;
}