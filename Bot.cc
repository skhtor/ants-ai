#include "Bot.h"

using namespace std;

//constructor
Bot::Bot()
{
};

//plays a single game of Ants.
void Bot::playGame()
{
    //reads the game parameters and sets up
    cin >> state;
    state.setup();
    endTurn();

    //continues making moves while the game is not over
    while(cin >> state)
    {
        state.updateVisionInformation();
        makeMoves();
        endTurn();
    }
};

//makes the bots moves for the turn
void Bot::makeMoves()
{
    state.bug << "turn " << state.turn << ":" << endl;
    state.bug << state << endl;

    SpawnNewAnts();

    for (int ant = 0; ant < myAnts.size(); ant++)
    { // For each ant

        DeleteDeadAnts(ant);

        // Clear all data
        while (myAnts[ant].m_queue.size() > 0)
        {
            myAnts[ant].m_queue.pop();
        }
        myAnts[ant].m_path.clear();
        myAnts[ant].m_visited.clear();

        SearchRadius(ant);

        if (myAnts[ant].m_path.size() > 0)
        {
            for (int dir = 0; dir < 4; dir++)
            {
                Location loc = state.getLocation(myAnts[ant].m_loc, dir);

                if (loc.row == myAnts[ant].m_path[0].m_loc.row && loc.col == myAnts[ant].m_path[0].m_loc.col)
                {
                    state.makeMove(myAnts[ant].m_loc, dir);
                    myAnts[ant].MoveTo(loc);
                    break;
                }
            }
        }
        else
        {
            bool antMoved = false;
            bool looped = false;
            int dir = myAnts[ant].m_dir;

            while (!antMoved)
            { // loop until the ant has moved in a direction
                if (dir >= 4)
                {
                    if (!looped)
                    {
                        dir = 0;
                        looped = true;
                    }
                    else antMoved = true;
                    break;
                }

                Location loc = state.getLocation(myAnts[ant].m_loc, dir);

                if (state.grid[loc.row][loc.col].isWater || state.grid[loc.row][loc.col].ant >= 0)
                { // if water or ant, change dir
                    dir++;
                }
                else // otherwise location is free and ant will move
                {
                    state.makeMove(myAnts[ant].m_loc, dir);
                    myAnts[ant].MoveTo(loc);
                    antMoved = true;
                }
            }

            myAnts[ant].m_dir = dir;
        }
    }

    state.bug << "time taken: " << state.timer.getTime() << "ms" << endl << endl;
};

// Finishes the turn
void Bot::endTurn()
{
    if(state.turn > 0)
        state.reset();
    state.turn++;

    cout << "go" << endl;
};

// --------------------------------------------------------------------Methods//

void Bot::SpawnNewAnts()
{
    // If server returns a location of an ant we don't know about, create an Ant object at that location
    for (Location l: state.myAntLocs)
    { // for each ant location
        bool spawnAnt = true;
        for (int ant = 0; ant < myAnts.size(); ant++)
        { // check that location against all of our stored Ants
            if (myAnts[ant].m_loc.row == l.row && myAnts[ant].m_loc.col == l.col)
            { // if we have an ant at that location, don't spawn it
                 spawnAnt = false;
            }
        }

        // spawn the Ant
        if (spawnAnt)
        {
            myAnts.push_back(Ant(l));
        }
    }
}

void Bot::DeleteDeadAnts(int currentAnt)
{
    bool antDead = true;
    // Delete any ant that doesn't exist anymore (ie server responds with ant not existing in that location anymore)
    for (Location l: state.myAntLocs)
    {
        if (myAnts[currentAnt].m_loc.row == l.row && myAnts[currentAnt].m_loc.col == l.col)
        {
             antDead = false;
        }
    }
    if (antDead)
    {
        myAnts.erase(myAnts.begin() + currentAnt); // Ant location doesn't exist, therefore ant must have died
    }
}

std::vector<Location> Bot::GetNeighbours(Location loc)
{
    std::vector<Location> neighbours;

    for (int dir = 0; dir < 4; dir++)
    {
        Location newLoc = state.getLocation(loc, dir);

        if (!state.grid[newLoc.row][newLoc.col].isWater && state.grid[newLoc.row][newLoc.col].ant < 0)
        {
            neighbours.push_back(newLoc);
        }
    }

    return neighbours;
};

void Bot::SearchRadius(int ant)
{
    Node node = Node(myAnts[ant].m_loc);
    myAnts[ant].m_queue.push(node);

    while (myAnts[ant].m_queue.size() > 0)
    {
        // Dequeue node
        Node currentNode = myAnts[ant].m_queue.front();
        myAnts[ant].m_queue.pop();

        // Add to visited nodes
        myAnts[ant].m_visited.push_back(currentNode);

        // If target cell, break
        if (state.grid[currentNode.m_loc.row][currentNode.m_loc.col].isFood)
        {
            while (currentNode.m_predecessor != -1)
            {
                myAnts[ant].m_path.push_back(currentNode);

                for (Node n: myAnts[ant].m_visited)
                {
                    if (n.m_id == currentNode.m_predecessor)
                    {
                        currentNode = Node(n.m_loc, n.m_predecessor);
                        break;
                    }
                }
            }
            // Reverse the order so path is in order;
            std::reverse(myAnts[ant].m_path.begin(),myAnts[ant].m_path.end());
        }
        else
        {
            // Check available surrounding nodes from current node
            std::vector<Location> neighbours = GetNeighbours(currentNode.m_loc);

            bool visited = false;
            for (Location l: neighbours)
            {
                // Check if not visited
                for (Node n: myAnts[ant].m_visited)
                { // for each stored node
                    if (n.m_loc.row == l.row && n.m_loc.col == l.col)
                    { // if newLoc is already a found location
                        visited = true;
                        break;
                    }
                }

                if (!visited)
                {
                    if (state.grid[l.row][l.col].isVisible == 1)
                    {
                        myAnts[ant].m_queue.push(Node(l, currentNode.m_id));
                    }
                }
            }
        }
    }
}
