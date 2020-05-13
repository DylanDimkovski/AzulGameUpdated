#include "GameEngine.h"

#include <iostream>
#include <algorithm>
#include <random>

GameEngine::GameEngine(Menu *menu) : players(),
                                     factories(),
                                     centerPile(),
                                     bag(new TileList()),
                                     lid(new TileList()),
                                     menu(menu)
{

    centerPile.push_back(FIRSTPLAYER);

    for (int i = 0; i < NUM_FACTORIES; i++)
    {
        TileType temp[4] = {NOTILE};
        factories[i] = new Factory(temp);
    }
}

GameEngine::~GameEngine()
{
}

void GameEngine::playGame(char const *argv)
{
    addPlayers();
    fillBag(argv[0]);
    playerTurnID = players[0];
    int roundsPlayed = 0;
    do
    {
        playRound();
        roundsPlayed++;
    } while (roundsPlayed < 1);
}

void GameEngine::playRound()
{
    for (int i = 0; i < NUM_FACTORIES; i++)
    {
        TileType temp[4] = {NOTILE};

        for (int j = 0; j < FACTORY_SIZE; j++)
        {
            temp[j] = bag->getHead()->getValue();
            bag->removeFront();
        }

        factories[i]->fill(temp);
    }
    for (int i = 0; i < (int)players.size(); i++)
    {
        menu->roundStart(playerTurnID->getName());
        menu->printFactory(&centerPile);
        for (int i = 0; i < NUM_FACTORIES; i++)
        {
            menu->printFactory(i + 1, factories[i]->toString());
        }
        menu->printMosaic(playerTurnID);

        bool inputDone = false;
        do
        {
            string input = menu->getInput();
            if (input.substr(0, 4) == "turn")
            {
                //Handle turn input here and set next player as PlayerTurnID
                setPlayerTurn();
                inputDone = true;
            }
            else
            {
                menu->printMessage("Invalid input, try again");
            }
        } while (!inputDone);
    }
}

void GameEngine::setPlayerTurn()
{
    if (players[0] == playerTurnID)
    {
        playerTurnID = players[1];
    }
    else
    {
        playerTurnID = players[0];
    }
}

Factory *GameEngine::getFactory(int position)
{
    return factories[position];
}

void GameEngine::fillBag(int seed)
{
    //Create both a bag and shuffle vector
    TileType tileTypes[5] = {BLACK, LIGTHBLUE, DARKBLUE, YELLOW, RED};
    TileType temp[100];

    //Store all tiles in temp bag SORTED
    for (int i = 0; i < 100; i++)
    {
        temp[i] = tileTypes[i / 20];
    }

    //Shuffle temp bag
    std::shuffle(std::begin(temp), std::end(temp), std::default_random_engine(seed));

    //Store shuffled tiles into the game bag
    for (int i = 0; i < 100; i++)
    {
        bag->addBack(temp[i]);
    }
}

Player *GameEngine::addPlayer(string name)
{
    //creates a new player
    return new Player(name, 0);
}

void GameEngine::addPlayers()
{
    //Checks for player names and adds them to player vector

    menu->printMessage("Enter the name for player 1: \n");
    string name1 = menu->getInput();
    menu->printMessage("Enter the name for player 2: \n");
    string name2 = menu->getInput();

    players.push_back(addPlayer(name1));
    players.push_back(addPlayer(name2));

    menu->printMessage("Let's Play!");
    ;
}
