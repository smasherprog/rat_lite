#pragma once

#include <algorithm>
#include <math.h>
#include <iostream>

namespace SL {
	namespace Remote_Access_Library {

		class Point {
		public:
			Point() :X(0), Y(0) {}
			Point(const Point& p) :Point(p.X, p.Y) {}
			Point(int x, int y) :X(x), Y(y) {}
			int X, Y;
		};
		class Rect {

		public:
			Rect() :Height(0), Width(0) {}
			Rect(const Rect& rect) : Rect(rect.Origin, rect.Height, rect.Width) {}
			Rect(Point origin, int height, int width) : Origin(origin), Height(height), Width(width) {}
			auto Center() const { return Point(Origin.X + (Width / 2), Origin.Y + (Height / 2)); }

			Point Origin;
			int Height, Width;
			auto top() const { return Origin.Y; }
			auto bottom() const { return Origin.Y + Height; }
			void bottom(int b) { Height = b - Origin.Y; }
			auto left() const { return Origin.X; }
			auto right() const { return Origin.X + Width; }
			void right(int r) { Width = r - Origin.X; }
			auto Contains(const Point& p) const {
				if (p.X < left()) return false;
				if (p.Y < top()) return false;
				if (p.X >= right()) return false;
				if (p.Y >= bottom()) return false;
				return true;
			}
			void Expand_To_Include(const Point& p) {
				if (p.X <= left()) Origin.X = p.X;
				if (p.Y <= top()) Origin.Y = p.Y;
				if (right() <= p.X) Width = p.X - left();
				if (bottom() <= p.Y) Height = p.Y - top();
			}
		};
		inline auto operator==(const Point& p1, const Point& p2) {
			return p1.X == p2.X && p1.Y == p2.Y;
		}
		inline auto operator!=(const Point& p1, const Point& p2) {
			return !(p1 == p2);
		}
		inline auto operator==(const Rect& p1, const Rect& p2) {
			return p1.Origin == p2.Origin && p1.Height == p2.Height && p1.Width == p2.Width;
		}
		inline auto SquaredDistance(const Point& p1, const Point& p2) {
			auto dx = abs(p1.X - p2.X);
			auto dy = abs(p1.Y - p2.Y);
			return dx*dx + dy*dy;
		}
		inline auto SquaredDistance(const Point& p, const Rect& r)
		{
			auto cx = std::max(std::min(p.X, r.right()), r.left());
			auto cy = std::max(std::min(p.Y, r.bottom()), r.top());
			return SquaredDistance(Point(cx, cy), p);
		}
		inline auto Distance(const Point& p1, const Point& p2) { return sqrt(SquaredDistance(p1, p2)); }

		inline auto Distance(const Point& p, const Rect& r) { return Distance(p, r.Center()); }
		inline  std::ostream& operator<<(std::ostream& os, const Point& p)
		{
			os << "X=" << p.X << ", Y=" << p.Y;
			return os;
		}
		inline  std::ostream& operator<<(std::ostream& os, const Rect& r)
		{
			os << "X=" << r.Origin.X << ", Y=" << r.Origin.Y << " Height=" << r.Height << ", Width=" << r.Width;
			return os;
		}

	}
}