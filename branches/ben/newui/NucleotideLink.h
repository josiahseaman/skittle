#ifndef NLINKAGEISM
#define NLINKAGEISM

class NucleotideLink
{
public:
	unsigned int match_dist;
	unsigned int start;
	
	NucleotideLink()
	{	
		match_dist = 0;
		start = 0;
	}
	NucleotideLink( unsigned int Start, unsigned int match)
	{	
		match_dist = match;
		start = Start;
	}
	/*NucleotideLink(const NucleotideLink& b)
	{	
		match_dist = b.match_dist;
	}*/
	~NucleotideLink()	{	}
	bool operator == (NucleotideLink b)
	{ return true;}
};

#endif
