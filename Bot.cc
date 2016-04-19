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
        makeMoves();
        endTurn();
    }
};

//makes the bots moves for the turn
void Bot::makeMoves()
{
    state.bug << "turn " << state.turn << ":" << endl;
    state.bug << state << endl;

    PlaceAntsInSquares();

    if (myAnts.size() > 10)
    {
        GuardBase();
    }

    for (Ant* ant: myAnts)
    { // For each ant

        if (!ant->m_moved)
        {
            // Reset path to food
            ant->m_nextMove = -1;
            SearchRadius(ant);

            // If path exists --> move along the path
            if (ant->m_nextMove != -1)
            {
                int dir = ant->m_nextMove;
                Location loc = state.getLocation(ant->m_loc, dir);

                state.makeMove(ant->m_loc, dir);
                ant->MoveTo(loc, dir);
                state.gridValues[loc.row][loc.col] = 0;
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
        for (Ant* ant: myAnts)
        { // check that location against all of our stored Ants
            if (ant->m_loc.row == l.row && ant->m_loc.col == l.col)
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

void Bot::SearchRadius(Ant* ant)
{
    // Create 2D vector which stores visited locations
    std::vector<std::vector<bool> > visited(state.rows, std::vector<bool>(state.cols, false));

    // Initialise queue
    std::deque<Node> queue;

    // Add initial node to queue
    Node node = Node(ant->m_loc);
    queue.push_back(node);

    visited[node.m_loc.row][node.m_loc.col] = true;

    //float searchStart = state.timer.getTime();

    while (queue.size() > 0)// && state.timer.getTime() - searchStart < 1)
    {
        // Dequeue node
        Node currentNode = queue.front();
        queue.pop_front();

        // If target cell, break
        if (state.grid[currentNode.m_loc.row][currentNode.m_loc.col].isFood || // If food
            (state.grid[currentNode.m_loc.row][currentNode.m_loc.col].isHill &&
                state.grid[currentNode.m_loc.row][currentNode.m_loc.col].hillPlayer != 0)) // If enemy hill
        {
            ant->m_nextMove = currentNode.m_firstMove;
            break;
        }
        else
        {
            // Check available surrounding nodes from current node
            for (int d = 0; d < TDIRECTIONS; d++)
            {
                Location nLoc = state.getLocation(currentNode.m_loc, d);

                if (!visited[nLoc.row][nLoc.col] && // Node is not visited
                    state.distance(ant->m_loc, nLoc) <= state.viewradius && // Node is within radius
                    !state.grid[nLoc.row][nLoc.col].isWater && // Node does not contain water
                    state.gridValues[nLoc.row][nLoc.col] != 0) // Node does not contain ant
                {
                    Node nNode = Node(nLoc); // Create new node

                    // Pass through the first move made to get to that node
                    if (currentNode.m_firstMove == -1)
                        nNode.AddFirstMove(d);
                    else
                        nNode.AddFirstMove(currentNode.m_firstMove);

                    queue.push_back(nNode); // Add node to queue
                    visited[nNode.m_loc.row][nNode.m_loc.col] = true;
                }
            }
        }
    } // end while queue size has an element
}

void Bot::MoveToHighVal(Ant* ant)
{
    int highestVal = 0; // highest value of neighbours
    int bestDir = 0; // Index of neighbour with highest value

    int waterCount = 0;
    Location deadEnd;

    for (int d = ant->m_dir; d < ant->m_dir + 4; d++)
    {
        int dir = d % 4;
        Location tempLoc = state.getLocation(ant->m_loc, dir);

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
        deadEnd = ant->m_loc;
    }

    // If ant isn't blocked by water or other ants
    if (highestVal > 0)
    {
        state.makeMove(ant->m_loc, bestDir);
        Location newLoc = state.getLocation(ant->m_loc, bestDir);
        ant->MoveTo(newLoc, bestDir);

        state.gridValues[newLoc.row][newLoc.col] = 0;
    }
    else state.gridValues[ant->m_loc.row][ant->m_loc.col] = 0;

    state.gridValues[deadEnd.row][deadEnd.col] = -1;
}

void Bot::PlaceAntsInSquares()
{
    for (int row = 0; row < state.rows; row++)
    {
        for (int col = 0; col < state.cols; col++)
        {

            state.grid[row][col].myAnt = NULL;
        }
    }

    for (Ant* ant: myAnts)
    {
        state.grid[ant->m_loc.row][ant->m_loc.col].myAnt = ant;
    }
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

// Calculating nearby ants

// void Bot::NearbyAllies()
// {
//     // Reset
//     for (Ant a: myAnts)
//         a.m_nearbyAllies = 0;
//
//     // Iterate through and count nearby ants
// 	std::vector<Ant>::iterator a = myAnts.begin();
// 	while (a.next != NULL)
//     {
//
// 		Ant ant1 = a.next();
//
// 		std::vector<Ant>::iterator b = myAnts.rbegin();
// 		Ant ant2 = b.next();
//
// 		while (ant1 != ant2)
//         {
// 			int d = state.distance(ant1.m_loc, ant2.m_loc);
//
// 			if (d <= sqrt(50))
//             {
// 				ant1.m_nearbyAllies++;
// 				ant2.m_nearbyAllies++;
// 			}
//
// 			ant2 = b.next();
// 		}
// 	}
// }

// void Bot::NearbyEnemies()
// {
//
// 	for (Ant ant : myAnts)
//     {
// 		myAnt.tile.oldAnt = myAnt;
// 		for (Ant enemyAnt : enemyAnts)
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
