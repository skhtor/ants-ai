#include "Bot.h"

using namespace std;

//constructor
Bot::Bot()
{
  currentDir = 0;
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
    for(int ant=0; ant<(int)state.myAnts.size(); ant++)
    {
        antMoved = false;

        while (!antMoved) { // loop until the ant has moved in a direction

            currentDir = currentDir % 4; // currentDir is either 0, 1, 2, or 3
            Location loc = state.getLocation(state.myAnts[ant], currentDir);

            if(state.grid[loc.row][loc.col].isWater or state.grid[row][col].ant >= 0) // if water or ant, change dir
            {
                currentDir += 1;
            }
            else // otherwise location is free and ant will move
            {
                state.makeMove(state.myAnts[ant], currentDir);
                antMoved = true;
            }
        }
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
