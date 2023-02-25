#include "String.hpp"

String toString(Bool b) {
	if (b) return "true";
	else return "false";
}

String toString(Vec3 v) {
	String s = "(";
	s.append(toString(v.x));
	s.append("|");
	s.append(toString(v.y));
	s.append("|");
	s.append(toString(v.z));
	s.append(")");
	return s;
}
