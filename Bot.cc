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

    for (int ant = 0; ant < myAnts.size(); ant++)
    { // For each ant

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
            }

            Location loc = state.getLocation(myAnts[ant].m_loc, dir);

            if (state.grid[loc.row][loc.col].isWater || state.grid[loc.row][loc.col].ant >= 0)
            { // if water or ant, change dir
                dir++;
            }
            else // otherwise location is free and ant will move
            {
                for (int i=0; i<state.myAntLocs.size(); i++)
                {
                    if (myAnts[ant].m_loc.row == state.myAntLocs[i].row
                        && myAnts[ant].m_loc.col == state.myAntLocs[i].col)
                    { // Ant location doesn't exist, therefore ant must have died
                        state.makeMove(state.myAntLocs[i], dir);
                        myAnts[ant].MoveTo(loc);
                    }
                }
                antMoved = true;
            }
        }
        myAnts[ant].m_dir = dir;
    }

    state.bug << "time taken: " << state.timer.getTime() << "ms" << endl << endl;
};

//finishes the turn
void Bot::endTurn()
{
    if(state.turn > 0)
        state.reset();
    state.turn++;

    cout << "go" << endl;
};
