#include "String.hpp"

String toString(const Bool b) {
	if (b) return "true";
	return "false";
}

String toString(const Vec3 v) {
	String s = "(";
	s.append(toString(v.x));
	s.append("|");
	s.append(toString(v.y));
	s.append("|");
	s.append(toString(v.z));
	s.append(")");
	return s;
}
