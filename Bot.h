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

    std::vector<Ant*> dangeredAnts;
    std::vector<std::vector<bool> > enemyHillsGrid;
    std::vector<Location> enemyHills;

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
    void NearbyEnemies();
    void ClearDangerZones();
    void EnemyHills();

    // Defence
    void GuardBase(Location h);
    bool BaseInDanger(Location h, double maxDist);
    void UltimateGuardBase(Location h);

    // Explore
    void MoveToHighVal(Ant* ant);
    void UpdateGridValues();

    // BFS
    void SearchRadius(std::vector<Location> locations, Mission mission);

    // A*
    void AStar(Ant* ant, Location dest);
    int IndNodeSmallestF(std::deque<Node> queue);

    // Other
    void CheckPath(Ant* ant, int* dir, Location* loc);
};

#endif //BOT_H_
