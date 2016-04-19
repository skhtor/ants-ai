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
    std::vector<Ant*> myAnts;

    // Constructor
    Bot();

    // Methods
    void playGame();    //plays a single game of Ants
    void makeMoves();   //makes moves for a single turn
    void endTurn();     //indicates to the engine that it has made its moves

    void SpawnNewAnts();
    void DeleteDeadAnts();
    void PlaceAntsInSquares();

    void MoveToHighVal(Ant* ant);
    void SearchRadius(Ant* ant);

    void GuardBase();
    void GuardBase2();
};

#endif //BOT_H_
