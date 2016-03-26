#ifndef ANT_H_
#define ANT_H_

#include "Location.h"
#include "Node.h"
#include <map>
#include <vector>
#include <queue>

struct Ant
{
    // Attributes
    Location m_loc;
    int m_dir;

    std::vector<Node> m_nodes;
    std::vector<Node> m_visited;
    std::queue<Node> m_queue;
    std::vector<Node> m_path;

    // Constructor
    Ant(Location loc);

    // Setter
    void MoveTo(Location loc);
};

#endif //ANT_H_
