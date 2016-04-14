#include "Bot.h"
#include "time.h"
#include "Node.h"
#include <iterator>
#include <math.h>

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
        //state.updateDangerZones();
        SpawnNewAnts();
        DeleteDeadAnts();
        PlaceAntsInSquares();
        ResetAnts();
        makeMoves();
        endTurn();
    }
};

//makes the bots moves for the turn
void Bot::makeMoves()
{
    state.bug << "turn " << state.turn << ":" << endl;
    state.bug << state << endl;

    SearchRadius();

    if (myAnts.size() > 10)
        GuardBase();
    if (myAnts.size() > 50)
        GuardBase2();

    for (int ant = 0; ant < myAnts.size(); ant++)
    { // For each ant

        if (!myAnts[ant]->m_moved)
        {
            // If path exists --> move along the path
            if (myAnts[ant]->m_nextMove != -1)
            {
                int dir = myAnts[ant]->m_nextMove;
                Location loc = state.getLocation(myAnts[ant]->m_loc, dir);

                if (state.gridValues[loc.row][loc.col] > 0)
                {
                    state.makeMove(myAnts[ant]->m_loc, dir);
                    myAnts[ant]->MoveTo(loc, dir);
                    state.gridValues[loc.row][loc.col] = 0;
                }
            }
            else // otherwise perform default behaviour
            {
                MoveToHighVal(ant);
            }
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
            if (myAnts[ant]->m_loc.row == l.row && myAnts[ant]->m_loc.col == l.col)
            { // if we have an ant at that location, don't spawn it
                 spawnAnt = false;
            }
        }

        // spawn the Ant
        if (spawnAnt)
        {
            myAnts.push_back(new Ant(l));
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
            if (myAnts[ant]->m_loc.row == l.row && myAnts[ant]->m_loc.col == l.col)
            {
                 antDead = false;
            }
        }
        if (antDead)
        {
            delete myAnts[ant];
            myAnts[ant] = NULL;
            myAnts.erase(myAnts.begin() + ant); // Ant location doesn't exist, therefore ant must have died
        }
    } // end myAnts index loop
}

// Assigns a pointer to an ant in each Square that they are in
void Bot::PlaceAntsInSquares()
{
    for (Ant* a: myAnts)
    {
        state.grid[a->m_loc.row][a->m_loc.col].myAnt = a;
    }
}

void Bot::ResetAnts()
{
    for (Ant* a: myAnts)
    {
        a->m_nextMove = -1;
        a->m_distanceToFood = 99999;
        a->m_moved = false;
    }
}

void Bot::SearchRadius()
{

    for (Location f: state.food)
    {
        // Create 2D vector which stores visited locations
        std::vector<std::vector<bool> > visited(state.rows, std::vector<bool>(state.cols, false));

        // Initialise queue
        std::deque<Node> queue;

        // Add initial node to queue
        Node node = Node(f);
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
            if (state.grid[currentNode.m_loc.row][currentNode.m_loc.col].myAnt != NULL)
            {
                Ant* a = state.grid[currentNode.m_loc.row][currentNode.m_loc.col].myAnt;

                if (state.distance(f, currentNode.m_loc) < a->m_distanceToFood)
                {
                    a->m_distanceToFood = state.distance(f, currentNode.m_loc);
                    a->m_nextMove = (currentNode.m_lastMove + 2) % 4;
                }
            }
            else
            {
                // Check available surrounding nodes from current node
                for (int d = 0; d < 4; d++)
                {
                    Location nLoc = state.getLocation(currentNode.m_loc, d);

                    if (!visited[nLoc.row][nLoc.col] && // Node is not visited
                        state.distance(f, nLoc) <= state.viewradius && // Node is within radius
                        !state.grid[nLoc.row][nLoc.col].isWater) // Node does not contain water
                    {
                        Node nNode = Node(nLoc); // Create new node

                        // Pass through the first move made to get to that node
                        nNode.AddFirstMove(d);
                        queue.push_back(nNode); // Add node to queue
                    }
                } // End loop of each diction 0, 1, 2, 3
            }
        } // End while queue size has an element
    } // End loop of each food location
}

void Bot::GuardBase()
{
    for (Location h: state.myHills)
    {
        if (state.grid[h.row][h.col - 1].ant >= 0)
            state.grid[h.row][h.col - 1].myAnt->m_moved = true;
        else if (state.grid[h.row][h.col + 1].ant >= 0)
            state.grid[h.row][h.col + 1].myAnt->m_moved = true;
    }
}

void Bot::GuardBase2()
{
    for (Location h: state.myHills)
    {
        if (state.grid[h.row][h.col - 1].ant >= 0)
            state.grid[h.row][h.col - 1].myAnt->m_moved = true;
        if (state.grid[h.row][h.col + 1].ant >= 0)
            state.grid[h.row][h.col + 1].myAnt->m_moved = true;
    }
}

void Bot::MoveToHighVal(int ant)
{
    int highestVal = 0; // highest value of neighbours
    int bestDir = 0; // Index of neighbour with highest value

    int waterCount = 0;
    Location deadEnd;

    for (int d = myAnts[ant]->m_dir; d < myAnts[ant]->m_dir + 4; d++)
    {
        int dir = d % 4;
        Location tempLoc = state.getLocation(myAnts[ant]->m_loc, dir);

        if (state.grid[tempLoc.row][tempLoc.col].isWater)
            waterCount++;

        if (state.gridValues[tempLoc.row][tempLoc.col] > highestVal)
        {
            highestVal = state.gridValues[tempLoc.row][tempLoc.col];
            bestDir = dir;
        }
    }
    if (waterCount == 3)
    {
        deadEnd = myAnts[ant]->m_loc;
    }

    // If ant isn't blocked by water or other ants
    if (highestVal > 0)
    {
        state.makeMove(myAnts[ant]->m_loc, bestDir);
        Location newLoc = state.getLocation(myAnts[ant]->m_loc, bestDir);
        myAnts[ant]->MoveTo(newLoc, bestDir);

        state.gridValues[newLoc.row][newLoc.col] = 0;
    }
    else state.gridValues[myAnts[ant]->m_loc.row][myAnts[ant]->m_loc.col] = 0;

    state.gridValues[deadEnd.row][deadEnd.col] = -1;
}

// Calculating nearby ants

void Bot::NearbyAllies()
{
    // Reset
    // for (Ant* a: myAnts)
    //     a->m_nearbyAllies = 0;
    //
    // // Iterate through and count nearby ants
	// std::vector<Ant*>::iterator a = myAnts.begin();
    //
	// while (a != myAnts.end())
    // {
    //
	// 	std::vector<Ant*>::iterator b = myAnts.end();
    //     std::advance(b, -1);
    //
	// 	while (a != b)
    //     {
	// 		int d = state.distance(*a->m_loc, *b->m_loc);
    //
	// 		if (d <= sqrt(50))
    //         {
	// 			a->m_nearbyAllies++;
	// 			b->m_nearbyAllies++;
	// 		}
    //
    //         std::advance(b, -1);
	// 	}
	// }
}

// void Bot::NearbyEnemies()
// {
//
// 	for (Ant*ant : myAnts)
//     {
// 		myAnt.tile.oldAnt = myAnt;
// 		for (Ant*enemyAnt : enemyAnts)
//         {
// 			int d = state.distance(myAnt.m_loc, enemyAnt.m_loc);
//
// 			if (dist <= 81) {
// 				myAnt.closeEnemyDists.put(dist, enemyAnt);
// 				enemyAnt.closeEnemyDists.put(dist, myAnt);
// 				myAnt.closeEnemyDistsSum += dist;
// 				enemyAnt.closeEnemyDistsSum += dist;
// 				if (dx + dy <= 5 && !((dx == 0 && dy == 5) || (dy == 0 && dx == 5))) {
// 					if (!myAnt.isIndirectlyDangered) myAnt.isIndirectlyDangered = true;
// 					myAnt.gammaDistEnemies.add(enemyAnt);
// 					enemyAnt.gammaDistEnemies.add(myAnt);
// 					if (!myAnt.isDangered && dx + dy <= 4 && !((dx == 0 && dy == 4) || (dy == 0 && dx == 4))) {
// 						myAnt.isDangered = true;
// 						dangeredAnts.add(myAnt);
// 					}
// 				}
// 			}
// 		}
// 		if (!myAnt.closeEnemyDists.isEmpty())
// 			myAnt.closestEnemyTile = myAnt.closeEnemyDists.firstEntry().getValue().tile;
// 	}
// }
