#ifndef BOT_H_
#define BOT_H_

#include "State.h"

/*
    This struct represents your bot in the game of Ants
*/
struct Bot
{
    // Attributes
    State state;
    std::vector<Ant> myAnts;

    // Constructor
    Bot();

    // Methods
    void playGame();    //plays a single game of Ants
    void makeMoves();   //makes moves for a single turn
    void endTurn();     //indicates to the engine that it has made its moves

    void SpawnNewAnts();
    void DeleteDeadAnts(int currentAnt);
    std::vector<Location> GetNeighbours(Location loc);
    void SearchRadius(int ant);
};

#endif //BOT_H_
