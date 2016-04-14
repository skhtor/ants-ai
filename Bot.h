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

    // Initialise Turn
    void SpawnNewAnts();
    void DeleteDeadAnts();
    void PlaceAntsInSquares();
    void ResetAnts();
    void NearbyAllies();

    void GuardBase();
    void GuardBase2();
    void MoveToHighVal(int ant);
    void SearchRadius();
};

#endif //BOT_H_
