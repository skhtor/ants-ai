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

    Node & operator= (const Node & other)
    {
        if (this != &other) // protect against invalid self-assignment
        {
            m_id = other.m_id;
            m_predecessor = other.m_predecessor;
            m_loc = other.m_loc;
        }
        // by convention, always return *this
        return *this;
    }

};

#endif //NODE_H_
