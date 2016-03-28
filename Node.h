#ifndef NODE_H_
#define NODE_H_

#include "Location.h"

/*
    struct for representing surrounding locations as nodes on a graph
*/
struct Node
{
    static int newId;
    int m_id;
    int m_predecessor;
    Location m_loc;

    Node(Location loc)
        : m_id(newId++)
    {
        m_loc = loc;
        m_predecessor = -1;
    };

    Node(Location loc, int predecessor)
        : m_id(newId++)
    {
        m_loc = loc;
        m_predecessor = predecessor;
    };

};

#endif //NODE_H_
