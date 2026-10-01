#include "vec2.h"

Vec2 v2add(Vec2 a, Vec2 b) {
	a.x = Fadd(a.x, b.x);
	a.y = Fadd(a.y, b.y);
	return a;
}

Vec2 v2scale(Vec2 v, Fixed s) {
	v.x = Fmul(v.x, s);
	v.y = Fmul(v.y, s);
	return v;
}
