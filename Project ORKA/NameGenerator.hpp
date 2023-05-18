#pragma once

#include "Basics.hpp"
#include "Debug.hpp"
#include "Random.hpp"

//usage
// {
//NameGenerator names;
//randomizeSeed();
//while (true) {
//	names.generate();
//	pause();
// }
//}

struct NameGenerator {
	Vector<String> s1 = {
		"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V",
		"W", "X", "Y", "Z"
	};
	Vector<String> s2 = {
		"aa",
		"AB",
		"AC",
		"AD",
		"ae",
		"ah",
		"AN",
		"ao",
		"AS",
		"aw",
		"ax",
		"ay",
		"AZ",
		"BI",
		"DE",
		"DU",
		"ea",
		"eh",
		"er",
		"ES",
		"EX",
		"ey",
		"FE",
		"FI",
		"FU",
		"GA",
		"GE",
		"GI",
		"GO",
		"ia",
		"ID",
		"ih",
		"iy",
		"LO",
		"LU",
		"MA",
		"ME",
		"MI",
		"MO",
		"MU",
		"NA",
		"NE",
		"NU",
		"oh",
		"ow",
		"oy",
		"PO",
		"RA",
		"RE",
		"RI",
		"RY",
		"SI",
		"SU",
		"TA",
		"TE",
		"TI",
		"TO",
		"TU",
		"TY",
		"ua",
		"uh",
		"uw",
		"VI",
	};
	Vector<String> s3 = {
		"ATE",
		"BEL",
		"CHI",
		"CRE",
		"CUS",
		"DAM",
		"DAN",
		"DES",
		"DIS",
		"DON",
		"DRU",
		"FOR",
		"GAH",
		"GAR",
		"FAC",
		"JAN",
		"KER",
		"LAR",
		"MAH",
		"MUS",
		"NAS",
		"NOS",
		"RUS",
		"SEI",
		"PLE",
		"SKY",
		"SYM",
		"TAN",
		"TEM",
		"TAS",
		"TER",
		"TOR",
		"TRO",
		"TUS",
		"VAL",
		"VEN",
		"VER",
		"WAL",
		"WAR",
		"ZEN",
	};
	Vector<String> s4 = {
		"BELL",
		"BUST",
		"CRAY",
		"DISM",
		"DISS",
		"PORT",
		"KING",
		"LATE",
		"LYZE",
		"TION",
		"LOCK",
		"SPAR",
		"TERR",
		"TOON",
		"TURE",
		"TRIX",
		"FREE",
	};
	Vector<String> s5 = {
		"BLOCK",
		"CRYPT",
		"DOYLE",
		"TRACE",
		"PEACE",
	};
	//Vector<String> s6 = {
	//	"SPHERE"
	//};

	Vector<Int> letterCountProb = {
		1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4
	};

	void generate() const {
		String name = "E";
		const Int sylCount = randomInt(4) + 2;
		for (Int i = 0; i < sylCount; i++) {
			const Int letterCount = letterCountProb[randomInt(letterCountProb.size())];
			switch (letterCount) {
				case 1: name += "-" + s1[randomInt(s1.size())];
					break;
				case 2: name += "-" + s2[randomInt(s2.size())];
					break;
				case 3: name += "-" + s3[randomInt(s3.size())];
					break;
				case 4: name += "-" + s4[randomInt(s4.size())];
					break;
				case 5: name += "-" + s5[randomInt(s5.size())];
					break;
				default: logError("");
			}
		}
		logDebug(name);
	}
};
