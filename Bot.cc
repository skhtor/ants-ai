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

    //picks out moves for each ant
    for (int ant=0; ant<(int)state.myAnts.size(); ant++)
    {
        if (dirDict.find(ant) == dirDict.end()) {
            dirDict[ant] = 0;
        }

        bool antMoved = false;

        int currentDir = dirDict[ant];

        bool looped = false;

        while (!antMoved) { // loop until the ant has moved in a direction

            Location loc = state.getLocation(state.myAnts[ant].m_loc, currentDir);

            if (state.grid[loc.row][loc.col].isWater or state.grid[loc.row][loc.col].ant >= 0) // if water or ant, change dir
            {

                currentDir += 1;
                if (currentDir == 4) {
                    if (!looped) {
                        currentDir = 0;
                        looped = true;
                    }
                    else antMoved = true;
                }

            }
            else // otherwise location is free and ant will move
            {
                state.makeMove(state.myAnts[ant].m_loc, currentDir);
                antMoved = true;
            }
        }

        dirDict[ant] = currentDir;
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
