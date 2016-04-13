#ifndef NODE_H_
#define NODE_H_

#include "Location.h"

/*
    struct for representing surrounding locations as nodes on a graph
*/
struct Node
{
    int m_lastMove;
    Location m_loc;

    int g;
    int h;
    int f;

    Node(Location loc)
    {
        m_loc = loc;
        m_lastMove = -1;
    };

    void AddFirstMove(int lastMove)
    {
        m_lastMove = lastMove;
    };
};

#endif //NODE_H_
