#ifndef ANT_H_
#define ANT_H_

#include "Location.h"

struct Ant
{
    // Attributes
    Location m_loc;
    int m_dir;

    // Constructor
    Ant(Location loc);

    // Setter
    void MoveTo(Location loc);
};

#endif //ANT_H_
