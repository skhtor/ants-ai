#include "Bot.h"
#include "time.h"
#include "Node.h"

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

        // Reset path to food
        myAnts[ant].m_nextMove = -1;
        SearchRadius(ant);

        // If path exists --> move along the path
        if (myAnts[ant].m_nextMove != -1)
        {
            int dir = myAnts[ant].m_nextMove;
            Location loc = state.getLocation(myAnts[ant].m_loc, dir);

            state.makeMove(myAnts[ant].m_loc, dir);
            myAnts[ant].MoveTo(loc);
            state.gridValues[loc.row][loc.col] = 0;
        }
        else // otherwise perform default behaviour
        {
            MoveToHighVal(ant);
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

// --------------------------------------------------------------------Methods

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

void Bot::SearchRadius(int ant)
{
    state.bug << "Ant " << ant << " is searching for food" << endl;

    // Create 2D vector which stores visited locations
    std::vector<std::vector<bool> > visited(state.rows, std::vector<bool>(state.cols, false));

    // Initialise queue
    std::deque<Node> queue;

    // Add initial node to queue
    Node node = Node(myAnts[ant].m_loc);
    queue.push_back(node);

    float searchStart = state.timer.getTime();

    while (queue.size() > 0 && state.timer.getTime() - searchStart < 1)
    {
        // Dequeue node
        Node currentNode = queue.front();
        queue.pop_front();

        // Add to visited nodes
        visited[currentNode.m_loc.row][currentNode.m_loc.col] = true;

        // If target cell, break
        if (state.grid[currentNode.m_loc.row][currentNode.m_loc.col].isFood)
        {
            myAnts[ant].m_nextMove = currentNode.m_firstMove;
            break;
        }
        else
        {
            // Check available surrounding nodes from current node
            for (int d = 0; d < 4; d++)
            {
                Location nLoc = state.getLocation(currentNode.m_loc, d);

                if (!visited[nLoc.row][nLoc.col] && // Node is not visited
                    state.distance(myAnts[ant].m_loc, nLoc) <= state.viewradius && // Node is within radius
                    !state.grid[nLoc.row][nLoc.col].isWater && // Node does not contain water
                    state.gridValues[nLoc.row][nLoc.col] != 0) // Node does not contain ant
                {
                    Node nNode = Node(nLoc); // Create new node

                    // Pass through the first move made to get to that node
                    if (currentNode.m_firstMove == -1)
                        nNode.AddFirstMove(d);
                    else
                        nNode.AddFirstMove(currentNode.m_firstMove);

                    queue.push_back(nNode); // Add node to queue.
                }
            }
        }
    } // end while queue size has an element

    state.bug << "Ant " << ant << " has completed searching for food" << endl;
}

void Bot::MoveToHighVal(int ant)
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
