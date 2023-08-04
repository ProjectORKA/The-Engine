// ReSharper disable StringLiteralTypo
#pragma once

#include "Basics.hpp"

// usage
// NameGenerator names;
// randomizeSeed();
// logDebug(names.generate());

struct NameGenerator
{
	Vector<Int>    letterCountProb = {1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4};
	Vector<String> s1              = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};
	Vector<String> s2              = {"AA", "AB", "AC", "AD", "AE", "AH", "AN", "AO", "AS", "AW", "AX", "AY", "AZ", "BI", "DE", "DU", "EA", "EH", "ER", "ES", "EX", "EY", "FE", "FI", "FU", "GA", "GE", "GI", "GO", "IA", "ID", "IH", "IY", "LO", "LU", "MA", "ME", "MI", "MO", "MU", "NA", "NE", "NU", "OH", "OW", "OY", "PO", "RA", "RE", "RI", "RY", "SI", "SU", "TA", "TE", "TI", "TO", "TU", "TY", "UA", "UH", "UW", "VI",};
	Vector<String> s3              = {"ATE", "BEL", "CHI", "CRE", "CUS", "DAM", "DAN", "DES", "DIS", "DON", "DRU", "FOR", "GAH", "GAR", "FAC", "JAN", "KER", "LAR", "MAH", "MUS", "NAS", "NOS", "RUS", "SEI", "PLE", "SKY", "SYM", "TAN", "TEM", "TAS", "TER", "TOR", "TRO", "TUS", "VAL", "VEN", "VER", "WAL", "WAR", "ZEN",};
	Vector<String> s4              = {"BELL", "BUST", "CRAY", "DISM", "DISS", "PORT", "KING", "LATE", "LYZE", "TION", "LOCK", "SPAR", "TERR", "TOON", "TURE", "TRIX", "FREE",};
	Vector<String> s5              = {"BLOCK", "CRYPT", "DOYLE", "TRACE", "PEACE",};

	[[nodiscard]] String generate() const;
};
