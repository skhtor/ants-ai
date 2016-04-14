#ifndef ANT_H_
#define ANT_H_

#include "Location.h"
#include "Node.h"
#include <vector>
#include <queue>

struct Ant
{
    // Attributes
    Location m_loc;
    bool m_moved;

    // BFS pathfinding
    int m_nextMove;
    int m_dir;
    double m_distanceToFood;

    // Combat stats
    int m_nearbyAllies;
    int m_nearbyEnemies;

    // Constructor
    Ant(Location loc);

    // Setter
    void MoveTo(Location loc, int dir);
};

#endif //ANT_H_
