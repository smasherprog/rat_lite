#pragma once
namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
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
				Rect(Point origin, unsigned int height, unsigned int width) : Origin(origin), Height(height), Width(width) {}
				Point Center() const { return Point(Origin.X + (Width / 2), Origin.Y + (Height / 2)); }
				void Expand_To_Include(const Point& p) {
					if ( p.X<=Origin.X ) Origin.X = p.X;
					if (p.Y  <=Origin.Y) Origin.Y = p.Y;
					auto xextent = Origin.X + Width;
					if (xextent <= p.X) Width += p.X - xextent;
					auto yextent = Origin.Y + Height;
					if (yextent <= p.Y) Height += p.Y - yextent;
				}
				Point Origin;
				unsigned int Height, Width;
			};

			inline unsigned int SquaredDistance(const Point& p1, const Point& p2) {
				auto dx = abs(p1.X - p2.X);
				auto dy = abs(p1.Y - p2.Y);
				return dx*dx + dy*dy;
			}
			inline unsigned int Distance(const Point& p1, const Point& p2) { sqrt(SquaredDistance(p1, p2)); }
			inline unsigned int SquaredDistance(const Point& p, const Rect& r) { return SquaredDistance(p, r.Center()); }
			inline unsigned int Distance(const Point& p, const Rect& r) { return Distance(p, r.Center()); }
		}
	}
}