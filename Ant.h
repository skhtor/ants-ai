#ifndef ANT_H_
#define ANT_H_

#include "Location.h"

struct Ant
{
    Location m_loc;

    int m_currentDir;

    Ant(Location loc);
};

#endif //ANT_H_
