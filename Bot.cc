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
        bool antMoved = false;
        bool looped = false;
        int dir = state.myAnts[ant].m_currentDir;

        while (!antMoved)
        { // loop until the ant has moved in a direction
            Location loc = state.getLocation(state.myAnts[ant].m_loc, dir);

            if (state.grid[loc.row][loc.col].isWater || state.grid[loc.row][loc.col].ant >= 0)
            { // if water or ant, change dir
                dir++;
                if (dir >= 4)
                {
                    if (!looped)
                    {
                        dir = 0;
                        looped = true;
                    }
                    else antMoved = true;
                }
            }
            else // otherwise location is free and ant will move
            {
                state.makeMove(state.myAnts[ant].m_loc, dir);
                antMoved = true;
            }
        }

        state.myAnts[ant].m_currentDir = dir;
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
