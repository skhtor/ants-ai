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
    for (int loc = 0; loc < state.myAntLocs.size(); loc++)
    { // for each ant location
        bool spawnAnt = true;
        for (int ant = 0; ant < myAnts.size(); ant++)
        { // check that location against all of our stored Ants
            if (myAnts[ant].m_loc.row == state.myAntLocs[loc].row && myAnts[ant].m_loc.col == state.myAntLocs[loc].col)
            { // if we have an ant at that location, don't spawn it
                 spawnAnt = false;
            }
        }

        // spawn the Ant
        if (spawnAnt)
        {
            myAnts.push_back(Ant(state.myAntLocs[loc]));
        }
    }
}

void Bot::DeleteDeadAnts()
{
    for (int ant = 0; ant < myAnts.size(); ant++)
    {
        bool antDead = true;
        // Delete any ant that doesn't exist anymore (ie server responds with ant not existing in that location anymore)
        for (int i=0; i<state.myAntLocs.size(); i++)
        {
            if (myAnts[ant].m_loc.row == state.myAntLocs[i].row && myAnts[ant].m_loc.col == state.myAntLocs[i].col)
            {
                 antDead = false;
            }
        }
        if (antDead)
        {
            myAnts.erase(myAnts.begin() + ant); // Ant location doesn't exist, therefore ant must have died
        }
    }
}
