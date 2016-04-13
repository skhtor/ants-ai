#include "Ant.h"

// Constructors
Ant::Ant(Location loc)
{
    m_loc = loc;
    m_nextMove = -1;
    m_dir = 0;
    m_nearbyAllies = 0;
    m_distanceToFood = 0;
};

// Setters
void Ant::MoveTo(Location loc, int dir)
{
    m_loc = loc;
    m_dir = dir;
}
