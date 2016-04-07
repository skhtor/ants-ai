#include "Ant.h"

//constructor
Ant::Ant(Location loc)
{
    m_loc = loc;
    m_nextMove = -1;
    m_dir = 0;
};


// Setters
void Ant::MoveTo(Location loc)
{
    m_loc = loc;
}
