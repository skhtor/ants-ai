#include "Bot.h"
#include "time.h"
#include "Node.h"
#include "Mission.h"
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
    enemyHillsGrid = std::vector<std::vector<bool> >(state.rows, std::vector<bool>(state.cols, false));
    endTurn();

    //continues making moves while the game is not over
    while(cin >> state)
    {
        state.updateVisionInformation();
        UpdateGridValues();
        //state.updateDangerZones();
        SpawnNewAnts();
        state.bug << "Spawned new ants" << endl;
        DeleteDeadAnts();
        state.bug << "Killed dead ants" << endl;
        PlaceAntsInSquares();
        state.bug << "Placed ants in squares" << endl;
        NearbyAllies();
        state.bug << "Calced nearby allies" << endl;
        NearbyEnemies();
        state.bug << "Calced nearby enemies" << endl;
        ResetAnts();
        state.bug << "Resetted ants" << endl;
        makeMoves();
        state.bug << "Made moves" << endl;
        endTurn();
    }
};

//makes the bots moves for the turn
void Bot::makeMoves()
{
    state.bug << "turn " << state.turn << ":" << endl;

    for(int row = 0; row < state.rows; row++)
    {
        for(int col = 0; col < state.cols; col++)
        {
            if (state.grid[row][col].value < 0)
                state.bug << " ";
            else if (state.grid[row][col].value <= 50)
                state.bug << "∙";
            else if (state.grid[row][col].value <= 100)
                state.bug << "○";
            else if (state.grid[row][col].value <= 250)
                state.bug << "●";
            else if (state.grid[row][col].value <= 500)
                state.bug << "◎";
            else if (state.grid[row][col].value <= 1000)
                state.bug << "⦿";
            else if (state.grid[row][col].value <= 2500)
                state.bug << "◉";
            else if (state.grid[row][col].value <= 5000)
                state.bug << "%";
            else if (state.grid[row][col].value <= 15000)
                state.bug << "@";
            state.bug << " ";
        }
        state.bug << endl;
    }

    //dangeredAnts.clear();

    //SearchRadius(state.enemyHills, ATTACKHILL);
    SearchRadius(state.food, PICKUPFOOD);
    //EnemyHills();

    state.bug << "Searched for food done" << endl;

    if (myAnts.size() > 35)
        AttackHills();

    if (myAnts.size() > 25)
    {
        AttackAnts();
        state.bug << "Attacked ants" << endl;
    }

    // Defence
    Defence();

    state.bug << "Guarded bases" << endl;

    // Make moves on each ant
    for (Ant* ant: myAnts)
    { // For each ant

        // if (ant->m_nearbyEnemies > ant->m_nearbyAllies)
        //     dangeredAnts.push_back(ant);
        //
        // if (dangeredAnts.size() > 0)
        // {
        //     for (Ant* antInDanger: dangeredAnts)
        //     {
        //         if (state.distance(ant->m_loc, antInDanger->m_loc) <= state.viewradius
        //     }
        //
        //     AStar(ant, dangeredAnts[0]);
        // }

        // Testing purposes
        //ant->m_mission = EXPLORE;

        switch(ant->m_mission)
        {
            case EXPLORE:
            {
                MoveToHighVal(ant);
                break;
            }

            case PICKUPFOOD:
            {
                int dir = ant->m_nextMove;
                Location loc = state.getLocation(ant->m_loc, dir);

                if (state.grid[loc.row][loc.col].ant != 0)
                {
                    state.grid[ant->m_loc.row][ant->m_loc.col].myAnt = NULL;
                    state.makeMove(ant->m_loc, dir);
                    ant->MoveTo(loc, dir);
                    state.grid[ant->m_loc.row][ant->m_loc.col].value *= 0.5;
                    state.grid[ant->m_loc.row][ant->m_loc.col].myAnt = ant;
                }
                break;
            }

            case ATTACK:
            {
                int dir = ant->m_nextMove;
                Location loc = state.getLocation(ant->m_loc, dir);

                CheckPath(ant, &dir, &loc);

                if (state.grid[loc.row][loc.col].ant != 0)
                {
                    state.grid[ant->m_loc.row][ant->m_loc.col].myAnt = NULL;
                    state.makeMove(ant->m_loc, dir);
                    ant->MoveTo(loc, dir);
                    state.grid[ant->m_loc.row][ant->m_loc.col].value *= 0.5;
                    state.grid[ant->m_loc.row][ant->m_loc.col].myAnt = ant;
                }
                break;
            }

            case DEFEND:
            {
                state.grid[ant->m_loc.row][ant->m_loc.col].value *= 0.5;
                break;
            }

            case CONSCRIPTION:
            {
                ant->m_timeSpentDefending++;
                // MoveToLowVal(ant);
                state.grid[ant->m_loc.row][ant->m_loc.col].value *= 0.5;
                break;
            }

            default:
            {
                MoveToHighVal(ant);
                break;
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
            if (ant->m_loc == l)
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
            if (myAnts[ant]->m_loc == l)
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
        a->m_moved = false;
        if (a->m_mission != STORMHILL)
        {
            a->m_mission = EXPLORE;
        }
    }
}

void Bot::Defence()
{
    for (Location base: state.myHills)
    {
        // Make fort
        for (Location e: state.enemyAntLocs)
        {
            if (state.distance(e, base) <= state.viewradius &&
                BaseInDanger(base, state.viewradius))
            {
                GuardBase(base);
                break;
            }
        } // end loop through visible enemies

        // Conscription (ants have to do their time defending their base)
        if (myAnts.size() > 10)
        {
            conscriptedAnts.clear();
            int count = 0;
            for (Ant* ant: myAnts)
            {
                if (state.distance(ant->m_loc, base) <= state.viewradius)
                {
                    count++;
                    conscriptedAnts.push_back(ant);
                }
            }

            if (count < 4)
            {
                for (Ant* ant: conscriptedAnts)
                {
                    if (ant->m_mission == EXPLORE && state.manDistance(ant->m_loc, base) == 2)
                    {
                        if (ant->m_timeSpentDefending > 10)
                            ant->m_timeSpentDefending = 0;
                        else
                            ant->m_mission = CONSCRIPTION;
                    }
                }
            }
        }
    } // end loop through bases
}

void Bot::SearchRadius(std::vector<Location> locations, Mission mission)
{
    for (Ant* a: myAnts)
    {
        a->m_distanceToFood = 99999;
    }

    for (Location l: locations)
    {
        // Create 2D vector which stores visited locations
        std::vector<std::vector<bool> > visited(state.rows, std::vector<bool>(state.cols, false));

        // Initialise queue
        std::deque<Node> queue;

        // Add initial node to queue
        Node node = Node(l);
        queue.push_back(node);
        visited[node.m_loc.row][node.m_loc.col] = true;

        //float searchStart = state.timer.getTime();

        while (queue.size() > 0) // && state.timer.getTime() - searchStart < 1)
        {
            // Dequeue node
            Node currentNode = queue.front();
            queue.pop_front();

            // If target cell, break
            if (state.grid[currentNode.m_loc.row][currentNode.m_loc.col].myAnt != NULL)
            {
                Ant* a = state.grid[currentNode.m_loc.row][currentNode.m_loc.col].myAnt;

                if (state.distance(l, currentNode.m_loc) < a->m_distanceToFood)
                {
                    a->m_distanceToFood = state.distance(l, currentNode.m_loc);
                    a->m_nextMove = (currentNode.m_lastMove + 2) % 4;
                    a->m_mission = mission;
                }
            }
            else
            {
                // Check available surrounding nodes from current node
                for (int d = 0; d < 4; d++)
                {
                    Location nLoc = state.getLocation(currentNode.m_loc, d);

                    if (!visited[nLoc.row][nLoc.col] && // Node is not visited
                        state.distance(l, nLoc) <= state.viewradius && // Node is within radius
                        !state.grid[nLoc.row][nLoc.col].isWater) // Node does not contain water
                    {
                        Node nNode = Node(nLoc); // Create new node

                        // Pass through the first move made to get to that node
                        nNode.AddFirstMove(d);
                        queue.push_back(nNode); // Add node to queue
                        visited[nNode.m_loc.row][nNode.m_loc.col] = true;
                    }
                } // End loop of each diction 0, 1, 2, 3
            }
        } // End while queue size has an element
    } // End loop of each food location
}

bool Bot::BaseInDanger(Location h, double maxDist)
{
    // Create 2D vector which stores visited locations
    std::vector<std::vector<bool> > visited(state.rows, std::vector<bool>(state.cols, false));

    // Initialise queue
    std::deque<Node> queue;

    // Add initial node to queue
    Node node = Node(h);
    queue.push_back(node);
    visited[node.m_loc.row][node.m_loc.col] = true;

    //float searchStart = state.timer.getTime();

    while (queue.size() > 0) // && state.timer.getTime() - searchStart < 1)
    {
        // Dequeue node
        Node currentNode = queue.front();
        queue.pop_front();

        // If target cell, break
        if (state.grid[currentNode.m_loc.row][currentNode.m_loc.col].ant >= 1)
        {
            return true;
        }
        else
        {
            // Check available surrounding nodes from current node
            for (int d = 0; d < 4; d++)
            {
                Location nLoc = state.getLocation(currentNode.m_loc, d);

                if (!visited[nLoc.row][nLoc.col] && // Node is not visited
                    state.distance(h, nLoc) <= maxDist && // Node is within radius
                    !state.grid[nLoc.row][nLoc.col].isWater) // Node does not contain water
                {
                    Node nNode = Node(nLoc); // Create new node

                    // Pass through the first move made to get to that node
                    nNode.AddFirstMove(d);
                    queue.push_back(nNode); // Add node to queue
                    visited[nNode.m_loc.row][nNode.m_loc.col] = true;
                }
            } // End loop of each diction 0, 1, 2, 3
        }
    } // End while queue size has an element
    return false;
}

void Bot::AttackHills()
{
    for (Ant* ant: myAnts)
    {
        if (ant->m_mission == EXPLORE)
        {
            for (Location hill: state.enemyHills)
            {
                if (state.distance(hill, ant->m_loc) <= state.viewradius*2)
                {
                    AStar(ant, hill, ATTACK);
                    break;
                }
            } // end loop through each enemy hill visible
        } // endif ant not currently assigned a mission
    } // end loop through each ant
}

void Bot::AttackAnts()
{
    for (Ant* ant: myAnts)
    {
        if (ant->m_mission == EXPLORE && ant->m_nearbyAllies > 3)
        {
            for (Location enemy: state.enemyAntLocs)
            {
                if (state.distance(enemy, ant->m_loc) <= state.viewradius)
                {
                    AStar(ant, enemy, ATTACK);
                    break;
                }
            } // end loop through each enemy hill visible
        }
    } // end for loop of each ant
}

void Bot::GuardBase(Location h)
{
    int count = 0;

    std::vector<Location> corners = {
        Location(
            min(h.row, state.rows - 1),
            min(h.col, state.cols - 1)), // North West
        Location(
            min(h.row, state.rows - 1),
            min(h.col, state.cols + 1)), // South West
        Location(
            min(h.row, state.rows + 1),
            min(h.col, state.cols - 1)), // North East
        Location(
            min(h.row, state.rows + 1),
            min(h.col, state.rows + 1)) // South East
    };

    std::vector<Location> edges = {
        Location(
            h.row,
            min(h.col, state.cols - 1)), // North
        Location(
            h.row,
            min(h.col, state.cols + 1)), // South
        Location(
            min(h.row, state.rows - 1),
            h.col), // West
        Location(
            min(h.row, state.rows + 1),
            h.col), // East
    };

    for (Location corner: corners)
    {
        if (state.grid[corner.row][corner.col].ant == 0)
        {
            if (state.grid[corner.row][corner.col].myAnt != NULL)
            {
                state.grid[corner.row][corner.col].myAnt->m_mission = DEFEND;
                state.grid[corner.row][corner.col].value = 0;
                count++;
            }
        }
        else if (state.grid[corner.row][corner.col].isWater) count++;
        else state.grid[corner.row][corner.col].value = 500;
    }

    // Corners are filled
    if (count == 4)
    {
        for (Location edge: edges)
        {
            if (state.grid[edge.row][edge.col].ant == 0)
            {
                if (state.grid[edge.row][edge.col].myAnt != NULL)
                {
                    state.grid[edge.row][edge.col].myAnt->m_mission = DEFEND;
                    state.grid[edge.row][edge.col].value = 0;
                }
            }
            else state.grid[edge.row][edge.col].value = 500;
        }
    }
}

void Bot::UltimateGuardBase(Location h)
{
    std::vector<Location> perimeter = {
        // North
        Location(h.row - 1, h.col - 2),
        Location(h.row, h.col - 2),
        Location(h.row + 1, h.col - 2),
        // South
        Location(h.row - 1, h.col + 2),
        Location(h.row, h.col + 2),
        Location(h.row + 1, h.col + 2),
        // East
        Location(h.row + 2, h.col - 1),
        Location(h.row + 2, h.col),
        Location(h.row + 2, h.col + 1),
        // West
        Location(h.row - 2, h.col - 1),
        Location(h.row - 2, h.col),
        Location(h.row - 2, h.col + 1),
    };

    for (Location fort: perimeter)
    {
        if (state.grid[fort.row][fort.col].ant == 0)
        {
            if (state.grid[fort.row][fort.col].myAnt != NULL)
            {
                state.grid[fort.row][fort.col].myAnt->m_moved = true;
            }
        }
        else if (!state.grid[fort.row][fort.col].isWater)
        {
            state.grid[fort.row][fort.col].value = 500;
        }
    }
}

void Bot::CheckPath(Ant* ant, int* dir, Location* loc)
{
    if (ant->m_alliesInLine < ant->m_nearbyEnemies + 1)
    {
        bool safePath = true;

        for (Location enemy: state.enemyAntLocs)
        {
            if (state.distance(*loc, enemy) <= state.attackradius*1.5)
                safePath = false;
        }
        if (!safePath)
        {
            if (ant->m_retreat)
                *dir = ant->m_dir;
            else
                *dir = (ant->m_dir + 2) % 4;
            *loc = state.getLocation(ant->m_loc, *dir);
            ant->m_retreat = true;
        }
        else ant->m_retreat = false;
    }
}

// Move to the highest valued square in the surroundings
void Bot::MoveToHighVal(Ant* ant)
{
    double highestVal = 0; // highest value of neighbours
    int bestDir = 0; // Index of neighbour with highest value

    int waterCount = 0;

    for (int d = ant->m_dir; d < ant->m_dir + 4; d++)
    {
        int dir = d % 4;
        Location tempLoc = state.getLocation(ant->m_loc, dir);

        if (state.grid[tempLoc.row][tempLoc.col].ant == 0)
            continue;
        if (state.grid[tempLoc.row][tempLoc.col].isWater)
            waterCount++;

        if (state.grid[tempLoc.row][tempLoc.col].value > highestVal)
        {
            highestVal = state.grid[tempLoc.row][tempLoc.col].value;
            bestDir = dir;
        }
    }
    if (waterCount == 3)
    {
        state.grid[ant->m_loc.row][ant->m_loc.col].value = -1;
    }

    Location newLoc = state.getLocation(ant->m_loc, bestDir);
    CheckPath(ant, &bestDir, &newLoc);

    // If ant isn't blocked by water or other ants
    if (highestVal > 0)
    {
        state.grid[ant->m_loc.row][ant->m_loc.col].myAnt = NULL;
        state.makeMove(ant->m_loc, bestDir);
        ant->MoveTo(newLoc, bestDir);
        ReduceValue(newLoc.row, newLoc.col);
        state.grid[ant->m_loc.row][ant->m_loc.col].myAnt = ant;
    }
    else state.grid[ant->m_loc.row][ant->m_loc.col].value *= 0.5;
}

// Move to the lowest valued square in the surroundings
void Bot::MoveToLowVal(Ant* ant)
{
    double lowestVal = 0; // highest value of neighbours
    int bestDir = 0; // Index of neighbour with highest value

    for (int d = ant->m_dir; d < ant->m_dir + 4; d++)
    {
        int dir = d % 4;
        Location tempLoc = state.getLocation(ant->m_loc, dir);

        if (state.grid[tempLoc.row][tempLoc.col].ant == 0)
            continue;

        if (state.grid[tempLoc.row][tempLoc.col].value < lowestVal)
        {
            lowestVal = state.grid[tempLoc.row][tempLoc.col].value;
            bestDir = dir;
        }
    }

    Location newLoc = state.getLocation(ant->m_loc, bestDir);
    CheckPath(ant, &bestDir, &newLoc);

    // If ant isn't blocked by water or other ants
    if (lowestVal > 0)
    {
        state.grid[ant->m_loc.row][ant->m_loc.col].myAnt = NULL;
        state.makeMove(ant->m_loc, bestDir);
        ant->MoveTo(newLoc, bestDir);
        state.grid[newLoc.row][newLoc.col].value *= 1.5;
        state.grid[ant->m_loc.row][ant->m_loc.col].myAnt = ant;
    }
    else state.grid[ant->m_loc.row][ant->m_loc.col].value *= 1.5;
}

// Calculating nearby ants

void Bot::NearbyAllies()
{
    for (Ant* a: myAnts)
    {
        a->m_nearbyAllies = 0;
        a->m_alliesInLine = 0;
    }

    // Iterate through and count nearby ants
	std::vector<Ant*>::iterator a = myAnts.begin();

	while (a != myAnts.end())
    {
		std::vector<Ant*>::iterator b = myAnts.end();
        std::advance(b, -1);

		while (a != b)
        {
			int d = state.distance((*a)->m_loc, (*b)->m_loc);

			if (d <= sqrt(50))
            {
				(*a)->m_nearbyAllies++;
				(*b)->m_nearbyAllies++;
                if (d <= state.attackradius * 0.7)
                {
                    (*a)->m_alliesInLine++;
                    (*b)->m_alliesInLine++;
                }
			}

            std::advance(b, -1);
		}
        std::advance(a, 1);
	}
}

void Bot::NearbyEnemies()
{
    for (Ant* a: myAnts)
        a->m_nearbyEnemies = 0;

    // Iterate through and count nearby ants
	std::vector<Ant*>::iterator a = myAnts.begin();

	while (a != myAnts.end())
    {
		std::vector<Location>::iterator b = state.enemyAntLocs.begin();

		while (b != state.enemyAntLocs.end())
        {
			int d = state.distance((*a)->m_loc, *b);

			if (d <= sqrt(50))
            {
				(*a)->m_nearbyEnemies++;
			}

            std::advance(b, 1);
		}
        std::advance(a, 1);
	}
}

void Bot::EnemyHills()
{
    for (Location hill: state.enemyHills)
    {
        enemyHillsGrid[hill.row][hill.col] = true;
    }

    enemyHills.clear();

    for (int row = 0; row < state.rows; row++)
    {
        for (int col = 0; col < state.cols; col++)
        {
            if (enemyHillsGrid[row][col] == true)
            {
                enemyHills.push_back(Location(row, col));
            }
        }
    }
}

void Bot::AStar(Ant* ant, Location dest, Mission mission)
{
    // Create 2D vector which stores visited locations
    std::vector<std::vector<bool> > visited(state.rows, std::vector<bool>(state.cols, false));

    // Initialise queue
    std::deque<Node> queue;

    // Add initial node to queue
    Node node = Node(ant->m_loc);
    node.g = 0;
    node.h = state.manDistance(node.m_loc, dest);
    node.f = node.g + node.h;

    // Add node to queue
    queue.push_back(node);
    visited[node.m_loc.row][node.m_loc.col] = true;

    int maxDist = 3 * state.distance(ant->m_loc, dest);

    while (queue.size() > 0)
    {
        int closestNode = IndNodeSmallestF(queue);

        // Dequeue node
        Node currentNode = queue[closestNode];

        queue.erase(queue.begin() + closestNode);

        // If target cell, break
        if (currentNode.m_loc == dest)
        {
            ant->m_nextMove = currentNode.m_lastMove;
            ant->m_mission = mission;
            break;
        }
        else
        {
            // Check available surrounding nodes from current node
            for (int d = 0; d < 4; d++)
            {
                Location nLoc = state.getLocation(currentNode.m_loc, d);

                if (!visited[nLoc.row][nLoc.col] &&
                    state.grid[nLoc.row][nLoc.col].ant != 0 && // Node does not contain ant
                    !state.grid[nLoc.row][nLoc.col].isWater) // Node does not contain water
                {
                    Node nNode = Node(nLoc); // Create new node

                    nNode.g = currentNode.g + 1;
                    nNode.h = state.manDistance(nNode.m_loc, dest);
                    nNode.f = nNode.g + nNode.h;

                    // Pass through the first move made to get to that node
                    if (currentNode.m_lastMove == -1)
                        nNode.AddFirstMove(d);
                    else
                        nNode.AddFirstMove(currentNode.m_lastMove);

                    if (nNode.f <= maxDist) {
                        int i;
                        int size = queue.size();
                        for (i = 0; i < size; i++)
                        {
    						if (queue[i].f > nNode.f)
                            {
    							queue.insert(queue.begin() + i, nNode);
    							break;
    						}
    					}
                        if (i == size)
                        {
                            queue.push_back(nNode);
                        }
                        visited[nNode.m_loc.row][nNode.m_loc.col] = true;
                    }
                }
            } // End loop of each diction 0, 1, 2, 3
        }
    } // End while queue size has an element
}

int Bot::IndNodeSmallestF(std::deque<Node> queue)
{
    int smallestIndex = 0;
    int smallestF = 999999;

    for (int i = 0; i < queue.size(); i++)
    {
        if (queue[i].f < smallestF)
        {
            smallestF = queue[i].f;
            smallestIndex = i;
        }
    }

    return smallestIndex;
}

void Bot::UpdateGridValues()
{
    for(int row = 0; row < state.rows; row++)
    {
        for(int col = 0; col < state.cols; col++)
        {
            if (state.grid[row][col].isWater) // Never go to water
                state.grid[row][col].value = -1;
            else if (state.grid[row][col].hillPlayer == 0) // Never go to hill
                state.grid[row][col].value = -1;
            else if (state.grid[row][col].ant == 0) // Don't add value to squares with ants
                state.grid[row][col].value += 0;
            else if (state.grid[row][col].value != -1) // Add euclidean distance from hills to squares each turn
            {
                if (state.grid[row][col].isVisible && state.grid[row][col].pathDist == -1)
                {
                    bool pathFound = false;
                    double closestDist = 99999;
                    state.grid[row][col].pathDist = 99999;

                    for (Location base: state.myHills)
                    {
                        double dist = FindPathDist(base, Location(row, col));
                        if (dist != -1 && dist < state.grid[row][col].pathDist)
                        {
                            pathFound = true;
                            closestDist = dist;
                        }
                    }
                    if (pathFound)
                        state.grid[row][col].pathDist = closestDist;
                    else
                        state.grid[row][col].pathDist = -1;
                }
                if (myAnts.size() > 20 && state.grid[row][col].pathDist != -1)
                {
                    state.grid[row][col].value += (state.grid[row][col].pathDist/10);
                }
                else
                    state.grid[row][col].value += 5;
            }
        }
    }
}

double Bot::FindPathDist(Location origin, Location dest)
{
    // Create 2D vector which stores visited locations
    std::vector<std::vector<bool> > visited(state.rows, std::vector<bool>(state.cols, false));

    // Initialise queue
    std::deque<Node> queue;

    // Add initial node to queue
    Node node = Node(origin);
    node.g = 0;
    node.h = state.manDistance(node.m_loc, dest);
    node.f = node.g + node.h;

    // Add node to queue
    queue.push_back(node);
    visited[node.m_loc.row][node.m_loc.col] = true;

    while (queue.size() > 0)
    {
        //int closestNode = IndNodeSmallestF(queue);

        // Dequeue node
        Node currentNode = queue.front();//[closestNode];

        queue.pop_front();//erase(queue.begin() + closestNode);

        // If target cell, break
        if (currentNode.m_loc == dest)
        {
            return currentNode.g;
        }
        else
        {
            // Check available surrounding nodes from current node
            for (int d = 0; d < 4; d++)
            {
                Location nLoc = state.getLocation(currentNode.m_loc, d);

                if (!visited[nLoc.row][nLoc.col] && // Not already visited
                    state.grid[nLoc.row][nLoc.col].isVisible && // Node is visible
                    !state.grid[nLoc.row][nLoc.col].isWater) // Node does not contain water
                {
                    Node nNode = Node(nLoc); // Create new node

                    nNode.g = currentNode.g + 1;
                    nNode.h = state.manDistance(nNode.m_loc, dest);
                    nNode.f = nNode.g + nNode.h;

                    // Pass through the first move made to get to that node
                    if (currentNode.m_lastMove == -1)
                        nNode.AddFirstMove(d);
                    else
                        nNode.AddFirstMove(currentNode.m_lastMove);

                    int i;
                    int size = queue.size();
                    for (i = 0; i < size; i++)
                    {
						if (queue[i].f > nNode.f)
                        {
							queue.insert(queue.begin() + i, nNode);
							break;
						}
					}
                    if (i == size)
                    {
                        queue.push_back(nNode);
                    }
                    visited[nNode.m_loc.row][nNode.m_loc.col] = true;
                }
            } // End loop of each diction 0, 1, 2, 3
        }
    } // End while queue size has an element

    // path not found
    return -1;
}

void Bot::ReduceValue(int row, int col)
{
    state.grid[row][col].value *= 0.5;

    std::vector<Location> corners = {
        Location(
            min(row, state.rows - 1),
            min(col, state.cols - 1)), // North West
        Location(
            min(row, state.rows - 1),
            min(col, state.cols + 1)), // South West
        Location(
            min(row, state.rows + 1),
            min(col, state.cols - 1)), // North East
        Location(
            min(row, state.rows + 1),
            min(col, state.rows + 1)) // South East
    };

    for (Location loc: corners)
    {
        if (!state.grid[loc.row][loc.col].isWater && state.grid[loc.row][loc.col].ant != 0)
            state.grid[loc.row][loc.col].value *= 0.8;
    }

    std::vector<Location> edges = {
        Location(
            row,
            min(col, state.cols - 1)), // North
        Location(
            row,
            min(col, state.cols + 1)), // South
        Location(
            min(row, state.rows - 1),
            col), // West
        Location(
            min(row, state.rows + 1),
            col), // East
    };

    for (Location loc: edges)
    {
        if (!state.grid[loc.row][loc.col].isWater && state.grid[loc.row][loc.col].ant != 0)
            state.grid[loc.row][loc.col].value *= 0.65;
    }
}
