#ifndef _OBSTACLE_H_
#define _OBSTACLE_H_

class Obstacle
{
	public:
		// Constructor
		Obstacle(bool side[6], unsigned int distance);

		// Move
		void Update(double speed);

		// Which sides of the hexagon the obstacle occupies
		bool side[6];

		// How far away from the player it is
		double distance;
};

#endif
