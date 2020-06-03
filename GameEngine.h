#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "Factory.h"
#include "Player.h"
#include "TileList.h"
#include "Menu.h"
#include "Utils.h"

#include <string>
#include <sstream>
#include <vector>

using std::string;

class GameEngine
{
public:
    //Constructor
    GameEngine(Menu *menu);
    //Destructor
    ~GameEngine();
    //Fill bag with seed and call playGame()
    bool playGame(char const *argv);
    //Play game
    bool playGame();
    //Play a single round
    bool playRound();
    //Add a new player
    Player *addPlayer(string name, bool isAi);
    //Add an existing player from save
    Player *addPlayer(std::string name, int score, Mosaic *mosaic, bool isAi);
    //Helper method for new game
    void addPlayers();
    //Fill bag using seed
    void fillBag(int argc, char **argv);
    //Import already filled bag
    void fillBag(TileList *bag);
    //Import already filled lid
    void fillLid(TileList *lid);
    //Fill center pile with tiles from vector
    void fillCenterPile(std::vector<TileType> centerPileOne);
    //Draw tiles from bag and fill factories
    void fillFactories(Factory *factories[]);
    //Set a pointer to the player whose turn it is
    void setPlayerTurn(int playerIndex);
    //Displays a help menu
    void helpMenu(std::string state);
    //Displays game board
    void outputBoard();
    //Get a random name for the AI
    std::string nameAi();
    //Return AI logic
    std::string returnAiOutput();
    //Get valid factory choice for AI
    std::string getSuitableFactory(std::string check, TileType colour);

    // Getters
    Factory *getFactory(int);
    Player *getPlayer(int playerIndex);
    bool isTurn(Player);
    void fillBag(int seed);
    void shuffleBag();
    bool isPlayer1Turn();
    std::vector<TileType> getCenterPile();
    TileList *getBag();
    TileList *getLid();
    Player *getPlayerTurnID();

public:
    int playerTurnCount = 0;
    int numberOfPlayers;
    int pileNum;
    int factoryModifier = 0;
    int numberOfCenterPiles = 0;
    int passThroughKey = 0;
    int playerCount = 0;
    int numberOfAiPlayers;

private:
    void changePlayerTurn();
    bool hasPlayerWon();
    int drawFromCenter(TileType colour, std::vector<TileType> centerPile);
    bool containsFirstPlayer();
    bool roundOver();
    bool factoriesAreEmpty();
    bool centerPileContains(TileType tileType);
    bool validLineNum(int lineNum);
    bool validFactoryNum(int factoryNum);

    std::vector<Player *> players;
    Factory **factories;
    std::vector<TileType> centerPileOne;
    std::vector<TileType> centerPileTwo;
    Player *playerTurnID;
    TileList *bag;
    TileList *lid;
    Menu *menu;
};

#endif