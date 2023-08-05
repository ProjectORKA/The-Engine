#include "NameGenerator.hpp"
#include "Debug.hpp"
#include "Random.hpp"

String NameGenerator::generate() const
{
	String    name;
	const Int sylCount = randomIntSlow(4) + 2;
	for(Int i = 0; i < sylCount; i++)
	{
		const Int randomValueForLetters = randomIntSlow(letterCountProb.size());
		const Int letterCount           = letterCountProb[randomValueForLetters];
		switch(letterCount)
		{
			case 1:
				name += s1[randomIntSlow(s1.size())];
				break;
			case 2:
				name += s2[randomIntSlow(s2.size())];
				break;
			case 3:
				name += s3[randomIntSlow(s3.size())];
				break;
			case 4:
				name += s4[randomIntSlow(s4.size())];
				break;
			case 5:
				name += s5[randomIntSlow(s5.size())];
				break;
			default:
				logError("");
		}
	}
	return name;
}
