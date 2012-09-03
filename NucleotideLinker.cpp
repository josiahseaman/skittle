//NucleotideLinker.cpp
#include "NucleotideLinker.h"

NucleotideLinker::NucleotideLinker()
{	
	F_width = 250;
	min_width = 200;
}

void NucleotideLinker::display()
{
	vector<NucleotideLink>::iterator iter;
	for(iter = links.begin(); iter != links.end() && iter->start < min_width; ++iter)
	{
			NucleotideLink link = *iter;
			
			glPushMatrix();
				glLineWidth(2);
				glColor3i(0,0,0);
				glTranslated(link.start + link.match_dist / 2.0, 0, 0);
				//draw_arc(link.match_dist);
				DrawArc(link.match_dist /2.0);
			glPopMatrix();
	}
}

void NucleotideLinker::draw_arc(int distance)
{
	double h = distance / 1.4;
	glBegin(GL_LINE_STRIP);
		glVertex3d(0, 0, 0);
		glVertex3d(distance / 2.0, -h, 0);
		glVertex3d(distance, 0, 0);
	glEnd();
}

/** Thanks to SiegeLord Abode at http://slabode.exofire.net/circle_draw.shtml
 * for the example code on drawing arcs.
*/
void NucleotideLinker::DrawArc(float r, float cx, float cy, float start_angle, float arc_angle, int num_segments) 
{ 
	if( num_segments == 0)
		num_segments = int(2.0f * 3.1415926f / (acosf(1 - 0.5 / r)));//change the 0.25 to a smaller/bigger number as needed 

	float theta = arc_angle / float(num_segments - 1);//theta is now calculated from the arc angle instead, the - 1 bit comes from the fact that the arc is open

	float tangetial_factor = tanf(theta);

	float radial_factor = cosf(theta);

	
	float x = r * cosf(start_angle);//we now start at the start angle
	float y = r * sinf(start_angle); 
    
	glBegin(GL_LINE_STRIP);//since the arc is not a closed curve, this is a strip now
	for(int ii = 0; ii < num_segments; ii++)
	{ 
		glVertex2f(x + cx, y + cy);

		float tx = -y; 
		float ty = x; 

		x += tx * tangetial_factor; 
		y += ty * tangetial_factor; 

		x *= radial_factor; 
		y *= radial_factor; 
	} 
	glEnd(); 
}

void NucleotideLinker::calculate(string sequence, int minimum_width)
{
	min_width = minimum_width;
	
	int consensus_width = 200;
	int consensus_strength = 0;

	links.clear();
	const char* word;
	for(int i = 0; i+9+F_width < sequence.size(); i+=1)
	{
		NucleotideLink match = NucleotideLink();
		word = sequence.c_str() + i;

		if(consensus_strength > 0)
		{
			match = localized_search(sequence, i, consensus_width, 6 );
		}
		//only do this part if an entry was not already added
		if(match.match_dist == 0)
		{
			for(int k = min_width; k <= F_width; k++)
			{
				const char* neighbor = sequence.c_str()+i+k;
				if(*(word  ) == *(neighbor  ) && //this statement will get shortcutted 99.9% of the time
				   *(word+1) == *(neighbor+1) && 
				   *(word+2) == *(neighbor+2) && 
				   *(word+3) == *(neighbor+3) && 
				   *(word+4) == *(neighbor+4) && 
				   *(word+5) == *(neighbor+5) && 
				   *(word+6) == *(neighbor+6) && 
				   *(word+7) == *(neighbor+7) ) 
				{
					match = NucleotideLink(i,k);
					//cout << "  : " << i << ", " << k << endl;
					break;
				}
			}// end for k		
		}// end if lowered_reqs
		if(match.match_dist != 0)
		{
			if(match.match_dist < min_width)
				match.match_dist *= (int)((float)min_width / match.match_dist + 0.99999);
			match = multiples_check(sequence, match, consensus_width);
			links.push_back(match);
		}
		
		//consensus handling
		if(match.match_dist != consensus_width)//didn't add something that matched
		{
			--consensus_strength;
		}
		if(match.match_dist != 0)//I did add something
		{
			if(match.match_dist == consensus_width)
				++consensus_strength;						
			if(consensus_strength <= 0)
			{
				consensus_width = match.match_dist;
				consensus_strength = 10;
			}
		}	
		
	}
}

//check to make sure we aren't picking up on a multiple of the primary frequency
NucleotideLink NucleotideLinker::multiples_check(const string& seq, NucleotideLink match, int consensus_width)
{
	if(consensus_width != 0)
	{
		if(match.match_dist % consensus_width == 0)
			return NucleotideLink(match.start, consensus_width);
		//if(consensus_width % match.match_dist == 0)
	}
	NucleotideLink two = NucleotideLink();
	NucleotideLink three = NucleotideLink();
	if(match.match_dist/2 >= min_width)
		two = localized_search(seq, match.start, match.match_dist/2, 2);
	if(match.match_dist/3 >= min_width)
		three = localized_search(seq, match.start, match.match_dist/3, 2);
		
	if(three.match_dist != 0)
		match = three;
	if(two.match_dist != 0)
		match = two;
		
	return match;
}

NucleotideLink NucleotideLinker::localized_search(const string& seq, int index, int start_dist, unsigned int range)
{	
	int plus = start_dist;
	int minus = start_dist - 1;
	//int range = 10;
	short int max = 0;
	int best_distance = 0;
	
	while( plus - start_dist <= range && (plus < seq.size()-8 && minus >= 1))
	{
		check_for_max(seq,  plus, index, max, best_distance);
		check_for_max(seq, minus, index, max, best_distance);
		++plus;
		--minus;
	}
	if(max >= 6)
		return NucleotideLink(index, best_distance);
	else
		return NucleotideLink();
}

void NucleotideLinker::check_for_max(const string& seq, int dist, int index, short int& max, int& best_distance)
{
	if(dist < 1 || dist > F_width) 
		return;
		
	short int score = 0;
	for(int l = 0; l < 8; l++)
	{					
		if(seq[index + l] == seq[index + dist + l])				
			score += 1;		
	}				
	if( score > max)
	{
		max = score;
		best_distance = dist;
	}		
}

vector<float> NucleotideLinker::smooth( int default_width, int blur_size)
{
	if(default_width <= 0)
		default_width = 1;
	vector<float> widths;
	if(links.empty())
		return widths;
		
	vector<NucleotideLink>::iterator it;
	int last_pos = links[links.size()-1].start + links[links.size()-1].match_dist;
	vector<map<int,int> > votes;
	votes.resize(last_pos, map<int,int>() );
	for(int i = 0; i < votes.size(); ++i)
		votes[i] = map<int,int>();
		
	//cast votes
	//cout << "Cast Votes" << endl;
	for(it = links.begin(); it != links.end(); ++it)
	{	
		int start = it->start;
		int dist = it->match_dist;
		for(int k = start; k < start + dist + 8 && k < votes.size(); ++k)//vote at all these positions
		{
			votes[k][dist] = votes[k][dist] + 1;
		}
	}
	//tally votes
	//cout << "Tally" << endl;
	int width = default_width;//if there are no votes the width stays the same as the previous winner
	for(int i = 0; i < votes.size(); ++i)
	{//votes and widths have a 1:1 relationship	
		int max_votes = 0;
		for(map<int,int>::iterator tally = votes[i].begin(); tally != votes[i].end(); ++tally)//for each vote
		{
			if( tally->second > max_votes )
			{
				max_votes = tally->second;
				width = tally->first;
			}
		}
		widths.push_back( (float)width );
	}
	return widths;
}

//void NucleotideLinker::tie_up_loose_ends(vector<float>& width_list)

float NucleotideLinker::average( const list<int>& window )
{
	float avg  = 0.0;
	//list<int>::iterator it;
	for(list<int>::const_iterator it = window.begin(); it != window.end(); ++it)
	{
		avg += *it;		
	}
	avg = avg / window.size();
	return avg;
}
