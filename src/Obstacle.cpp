#include <algorithm>

#include <Obstacle.h>

// Constructor
Obstacle::Obstacle(bool s[], unsigned int d)
{
	std::copy(s, s+6, this->side);
	this->distance = d;
}
