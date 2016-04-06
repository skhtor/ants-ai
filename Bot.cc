#include "Bot.h"
#include "time.h"

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
        state.updateGridValues();
        SpawnNewAnts();
        DeleteDeadAnts();
        makeMoves();
        endTurn();
    }
};

//makes the bots moves for the turn
void Bot::makeMoves()
{
    state.bug << "turn " << state.turn << ":" << endl;
    state.bug << state << endl;

    for (int ant = 0; ant < myAnts.size(); ant++)
    { // For each ant

        // Clear all data
        while (myAnts[ant].m_queue.size() > 0)
        {
            myAnts[ant].m_queue.pop_front();
        }
        myAnts[ant].m_path.clear();
        myAnts[ant].m_visited.clear();

        SearchRadius(ant);

        if (myAnts[ant].m_path.size() > 0)
        {
            state.bug << "Ant " << ant << " is going to food" << endl;
            for (int dir = 0; dir < 4; dir++)
            {
                Location loc = state.getLocation(myAnts[ant].m_loc, dir);

                if (loc.row == myAnts[ant].m_path[0].m_loc.row && loc.col == myAnts[ant].m_path[0].m_loc.col)
                {
                    state.makeMove(myAnts[ant].m_loc, dir);
                    myAnts[ant].MoveTo(loc);
                    state.gridValues[loc.row][loc.col] = 0;
                    break;
                }
            }
        }
        else
        {
            int highestVal = 0; // highest value of neighbours
            int highestNeighbour = 0; // Index of neighbour with highest value

            for (int dir = 0; dir < 4; dir++)
            {
                Location tempLoc = state.getLocation(myAnts[ant].m_loc, dir);

                if (state.gridValues[tempLoc.row][tempLoc.col] > highestVal)
                {
                    highestVal = state.gridValues[tempLoc.row][tempLoc.col];
                    highestNeighbour = dir;
                }
            }

            // If ant isn't blocked by water or other ants
            if (highestVal > 0)
            {
                state.makeMove(myAnts[ant].m_loc, highestNeighbour);
                Location newLoc = state.getLocation(myAnts[ant].m_loc, highestNeighbour);
                myAnts[ant].MoveTo(newLoc);

                state.gridValues[newLoc.row][newLoc.col] = 0;
            }
            else state.gridValues[myAnts[ant].m_loc.row][myAnts[ant].m_loc.col] = 0;
        }
    } // end myAnts loop

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
    } // end state.myAntLocs loop
}

void Bot::DeleteDeadAnts()
{
    for (int ant = 0; ant < myAnts.size(); ant++)
    {
        bool antDead = true;
        // Delete any ant that doesn't exist anymore (ie server responds with ant not existing in that location anymore)

        for (Location l: state.myAntLocs)
        {
            if (myAnts[ant].m_loc.row == l.row && myAnts[ant].m_loc.col == l.col)
            {
                 antDead = false;
            }
        }
        if (antDead)
        {
            myAnts.erase(myAnts.begin() + ant); // Ant location doesn't exist, therefore ant must have died
        }
    } // end myAnts index loop
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
    state.bug << "Ant " << ant << " is searching for food" << endl;
    Node node = Node(myAnts[ant].m_loc);
    myAnts[ant].m_queue.push_back(node);

    while (myAnts[ant].m_queue.size() > 0)
    {

        // Dequeue node
        Node currentNode = myAnts[ant].m_queue.front();
        myAnts[ant].m_queue.pop_front();

        // Add to visited nodes
        myAnts[ant].m_visited.push_back(currentNode);

        // If target cell, break
        if (state.grid[currentNode.m_loc.row][currentNode.m_loc.col].isFood)
        {
            for (Node n: myAnts[ant].m_visited)
            {
                if (n.m_id == currentNode.m_predecessor)
                {
                    currentNode = Node(n.m_loc, n.m_predecessor);
                    break;
                }
            }

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
            break;
        }
        else
        {
            // Check available surrounding nodes from current node
            std::vector<Location> neighbours = GetNeighbours(currentNode.m_loc);

            for (Location l: neighbours)
            {
                bool visited = false;
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
                    if (state.distance(myAnts[ant].m_loc, l) <= state.viewradius)
                    {
                        state.bug << ".";
                        myAnts[ant].m_queue.push_back(Node(l, currentNode.m_id));
                    }
                }
            }
        }
    } // end while queue size has an element

    state.bug << "Ant " << ant << " has completed searching for food" << endl;
}
