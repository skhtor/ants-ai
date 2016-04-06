#ifndef NODE_H_
#define NODE_H_

#include "Location.h"

/*
    struct for representing surrounding locations as nodes on a graph
*/
struct Node
{
    int m_firstMove;
    Location m_loc;

    Node(Location loc)
    {
        m_loc = loc;
        m_firstMove = -1;
    };

    void AddFirstMove(int firstMove)
    {
        m_firstMove = firstMove;
    };
};

#endif //NODE_H_
