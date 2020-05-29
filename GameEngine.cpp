#include "GameEngine.h"

#include "Saver.h"
#include "textColour.h"
#include <iostream>
#include <algorithm>
#include <random>

GameEngine::GameEngine(Menu *menu) : players(),
                                     factories(),
                                     centerPileOne(),
                                     centerPileTwo(),
                                     bag(new TileList()),
                                     lid(new TileList()),
                                     menu(menu)

{
    //Create factories
    factories = new Factory *[NUM_FACTORIES];
    for (int i = 0; i < NUM_FACTORIES + factoryModifier; ++i)
    {
        factories[i] = new Factory();
    }
}

GameEngine::~GameEngine()
{
    for (int i = 0; i < NUM_FACTORIES + factoryModifier; ++i)
    {
        if (factories[i] != nullptr)
            delete factories[i];
    }
    if (bag != nullptr)
        delete bag;
    if (lid != nullptr)
        delete lid;
    for (Player *player : players)
        if (player != nullptr)
            delete player;
}

bool GameEngine::playGame(char const *argv)
{
    addPlayers();
    for (int i = 0; i < NUM_FACTORIES + factoryModifier; i++)
    {
        if (factories[i] != nullptr)
        {
            delete factories[i];
        }
    }

    if (numberOfPlayers == 3)
    {
        factoryModifier = 2;
    }
    else if (numberOfPlayers == 4)
    {
        factoryModifier = 4;
    }

    if (factories != nullptr)
    {
        delete[] factories;
    }

    factories = new Factory *[NUM_FACTORIES + factoryModifier];

    for (int i = 0; i < NUM_FACTORIES + factoryModifier; i++)
    {
        factories[i] = new Factory();
    }
    fillBag(argv[0]);
    playerTurnID = players[0];
    return playGame();
}

bool GameEngine::playGame()
{
    //Bool to track exit condition
    bool exit = false;
    while (!exit && !hasPlayerWon())
    {
        exit = playRound();
    }
    //Check if a player has won after each round
    if (hasPlayerWon())
    {
        //Game is a draw
        if (players[0]->getScore() == players[1]->getScore())
        {
            //Print results
            menu->gameOver(players[0]->getName(), players[1]->getName(), players[0]->getScore());
        }
        //Set pointer to winning player
        else
        {
            Player *winningPlayer = playerTurnID;
            for (auto player : players)
            {
                if (player->getScore() > winningPlayer->getScore())
                {
                    winningPlayer = player;
                }
            }
            //Print results
            menu->gameOver(winningPlayer);
        }
    }
    return exit;
}

bool GameEngine::playRound()
{
    //Track whether all tiles have been drawn
    if (roundOver())
    {
        //Return FP tile to center
        centerPileOne.push_back(FIRSTPLAYER);
        centerPileTwo.push_back(FIRSTPLAYER);
        for (int i = 0; i < NUM_FACTORIES + factoryModifier; i++)
        {
            TileType temp[4] = {NOTILE, NOTILE, NOTILE, NOTILE};

            for (int j = 0; j < FACTORY_SIZE; j++)
            {
                if (bag->size() == 0)
                {
                    TileList *tmp = bag;
                    bag = lid;
                    lid = tmp;
                    tmp = nullptr;
                    delete tmp;
                }

                temp[j] = bag->removeFront();
            }
            //Refill factories

            factories[i]->fill(temp);
        }
    }

    bool exit = false;

    while (!exit && !roundOver())
    {
        menu->printMessage("===========================================================================");

        for (int i = 0; i < 1; i++)
        {
            playerTurnID = players[i];
            if (numberOfPlayers == 4)
            {
                menu->printMosaic(players, 0, 1);
                menu->printMosaic(players, 2, 3);
            }
            else if (numberOfPlayers == 3)
            {
                menu->printMosaic(players, 0, 1);
                menu->printMosaic(players, 2);
            }
            else
            {
                menu->printMosaic(players, 0, 1);
            }
        }

        //Print player turn GUI
        playerTurnID = players[playerTurnCount];
        menu->handStart(playerTurnID->getName());
        if (numberOfCenterPiles == 1)
        {
            menu->printFactory(&centerPileOne);
        }
        else
        {
            menu->printFactory(&centerPileOne, &centerPileTwo);
        }

        for (int i = 0; i < NUM_FACTORIES + factoryModifier; i++)
        {
            menu->printFactory(i + 1, factories[i]->toString(), numberOfPlayers);
        }
        std::cout << "Broken: "
                  << playerTurnID->getMosaic()->getBrokenTiles()->toString()
                  << std::endl;

        std::cout << "==========" << std::endl;
        for (int i = 0; i < numberOfPlayers; i++)
        {
            Player *player = players[i];
            menu->printScore(player->getName(), player->getScore());
        }
        std::cout << "==========" << std::endl;

        //Bool to check whether input command has been executed
        bool inputDone = false;

        //Handle user input
        do
        {
            std::stringstream ss(menu->getInput());
            if (!std::cin.eof())
            {
                string errorMessage = "Command not recognised";
                string command;

                ss >> command;

                if (command == "turn")
                {
                    errorMessage = "Invalid syntax for turn command";
                    unsigned int factoryNum = NUM_FACTORIES + 1, lineNum = NUMBER_OF_LINES + 1;
                    char colour = '\0';

                    ss >> factoryNum >> colour >> lineNum;
                    TileType tileType = charToTileType(colour);
                    --lineNum;

                    //Error checking with helpful messages
                    if (validFactoryNum(factoryNum))
                    {
                        if (selectableTile(colour))
                        {
                            if (validLineNum(lineNum))
                            {
                                //If drawing from center pile
                                if (factoryNum == 10)
                                {
                                    if (!centerPileOne.empty() && centerPileContains(tileType))
                                    {
                                        //If moving tiles straight to broken line
                                        if (lineNum == 5)
                                        {
                                            playerTurnID->getMosaic()->addToBrokenTiles(drawFromCenter(tileType, centerPileOne), tileType);
                                            inputDone = true;
                                        }
                                        //If moving tiles to pattern line
                                        else if (!playerTurnID->getMosaic()->isFilled(lineNum, tileType) &&
                                                 playerTurnID->getMosaic()->getLine(lineNum)->canAddTiles(tileType))
                                        {
                                            if (containsFirstPlayer())
                                            {
                                                playerTurnID->getMosaic()->getBrokenTiles()->addFront(FIRSTPLAYER);
                                            }
                                            playerTurnID->getMosaic()->insertTilesIntoLine(lineNum, drawFromCenter(tileType, centerPileOne), tileType);
                                            inputDone = true;
                                        }
                                        else
                                            errorMessage = "Specified line cannot add tile";
                                    }
                                    else
                                        errorMessage = "Center pile does not contain specified tile";
                                }
                                if (factoryNum == 11)
                                {
                                    if (!centerPileTwo.empty() && centerPileContains(tileType))
                                    {
                                        //If moving tiles straight to broken line
                                        if (lineNum == 5)
                                        {
                                            playerTurnID->getMosaic()->addToBrokenTiles(drawFromCenter(tileType, centerPileTwo), tileType);
                                            inputDone = true;
                                        }
                                        //If moving tiles to pattern line
                                        else if (!playerTurnID->getMosaic()->isFilled(lineNum, tileType) &&
                                                 playerTurnID->getMosaic()->getLine(lineNum)->canAddTiles(tileType))
                                        {
                                            if (containsFirstPlayer())
                                            {
                                                playerTurnID->getMosaic()->getBrokenTiles()->addFront(FIRSTPLAYER);
                                            }
                                            playerTurnID->getMosaic()->insertTilesIntoLine(lineNum, drawFromCenter(tileType, centerPileTwo), tileType);
                                            inputDone = true;
                                        }
                                        else
                                            errorMessage = "Specified line cannot add tile";
                                    }
                                    else
                                        errorMessage = "Center pile does not contain specified tile";
                                }
                                //If drawing from factory
                                else
                                {
                                    //Check which center factory to discard too
                                    menu->printMessage("Please Enter Which Center Factory to add excess tiles: ");

                                    std::string numberConversion;
                                    bool inputCheck = false;
                                    while (!inputCheck)
                                    {
                                        numberConversion = menu->getInput();

                                        if (numberConversion == "11" && numberOfCenterPiles == 1)
                                        {
                                            std::cout << "Invalid Center Pile, Try Again!"
                                                      << std::endl;
                                        }
                                        else if (numberConversion == "10" || numberConversion == "11")
                                        {
                                            inputCheck = true;
                                            pileNum = std::stoi(numberConversion);
                                        }
                                        else
                                        {
                                            std::cout << "Invalid Center Pile, Try Again!"
                                                      << std::endl;
                                        }
                                    }

                                    //-1 from factory num input as index starts at 0
                                    --factoryNum;
                                    if (!factories[factoryNum]->isEmpty())
                                    {
                                        if (factories[factoryNum]->contains(tileType))
                                        {
                                            //if Adding straight to broken tiles
                                            if (lineNum == 5)
                                            {
                                                playerTurnID->getMosaic()->addToBrokenTiles(factories[factoryNum]->draw(tileType), tileType);
                                                for (TileType tile : factories[factoryNum]->empty())
                                                {
                                                    if (pileNum == 10)
                                                    {
                                                        centerPileOne.push_back(tile);
                                                    }
                                                    else if (pileNum == 11)
                                                    {
                                                        centerPileTwo.push_back(tile);
                                                    }
                                                }
                                                inputDone = true;
                                            }
                                            //Adding to pattern line
                                            else if (!playerTurnID->getMosaic()->isFilled(lineNum, tileType) &&
                                                     playerTurnID->getMosaic()->getLine(lineNum)->canAddTiles(tileType))
                                            {
                                                playerTurnID->getMosaic()->insertTilesIntoLine(lineNum, factories[factoryNum]->draw(tileType), tileType);

                                                for (TileType tile : factories[factoryNum]->empty())
                                                {
                                                    if (pileNum == 10)
                                                    {
                                                        centerPileOne.push_back(tile);
                                                    }
                                                    else if (pileNum == 11)
                                                    {
                                                        centerPileTwo.push_back(tile);
                                                    }
                                                }

                                                inputDone = true;
                                            }
                                            else
                                                errorMessage = "Specified line cannot add tile";
                                        }
                                        else
                                            errorMessage = "Selected factory does not contain specified tile";
                                    }
                                    else
                                        errorMessage = "Selected factory is empty";
                                }

                                // If the input has been executed
                                if (inputDone)
                                {
                                    //Change player turn
                                    changePlayerTurn();
                                    system("clear");
                                    std::cout << "Turn Successful!";
                                }
                            }
                            else
                                errorMessage = "Invalid line number";
                        }
                        else
                            errorMessage = "Invalid colour";
                    }
                    else
                        errorMessage = "Invalid factory number";
                }
                // Command to save game to file
                else if (command == "save")
                {
                    errorMessage = "Invalid syntax for save command";
                    std::string fileName;
                    if (!ss.eof())
                    {
                        ss.get(); // removes initial whitespace between save and filename
                        std::getline(ss, fileName);
                        if (isNotWhiteSpace(fileName))
                        {
                            Saver saver;
                            saver.save(this, fileName);
                            inputDone = true;
                        }
                    }

                    if (inputDone)
                        menu->printMessage("Game successfully saved to '" + fileName + "'");
                }
                if (!inputDone)
                    menu->printMessage("Invalid input: " + errorMessage + ", try again");
            }
            else
            {
                exit = true;
            }
        } while (!exit && !inputDone);
    }

    if (!exit)
    {
        //Distribute tiles to walls
        for (auto player : players)
        {
            if (player->hasFirstPlayer())
            {
                playerTurnID = player;
            }

            for (TileType tile : player->calcScore())
            {
                lid->addBack(tile);
            }
        }
    }
    return exit;
}

bool GameEngine::validLineNum(int lineNum)
{
    return lineNum >= 0 && lineNum <= NUMBER_OF_LINES;
}

bool GameEngine::validFactoryNum(int factoryNum)
{
    return factoryNum >= 0 && factoryNum <= NUM_FACTORIES + factoryModifier;
}

bool GameEngine::roundOver()
{
    // centerPileOne.empty() && factoriesAreEmpty(); equivalent but less efficient
    return !(!centerPileOne.empty() || !factoriesAreEmpty());
}

// returns true if factories are empty
bool GameEngine::factoriesAreEmpty()
{
    bool factoriesEmpty = true;
    int factoryIndex = 0;
    while (factoriesEmpty && factoryIndex < NUM_FACTORIES + factoryModifier)
    {
        factoriesEmpty &= factories[factoryIndex++]->isEmpty();
    }
    return factoriesEmpty;
}

void GameEngine::setPlayerTurn(int playerIndex)
{
    playerTurnID = players[playerIndex];
}

void GameEngine::changePlayerTurn()
{
    if (playerTurnCount + 1 == numberOfPlayers)
    {
        playerTurnCount = 0;
    }
    else
    {
        playerTurnCount++;
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

int GameEngine::drawFromCenter(TileType colour, std::vector<TileType> centerPile)
{
    int count = 0;
    std::vector<int> remove;
    int decrement = 0;

    //Loop through vector saving indexes of tiles to be erased
    for (int i = 0; i < (int)centerPile.size(); i++)
    {
        if (centerPile[i] == colour)
        {
            count++;
            remove.push_back(i);
        }
    }
    //Loop through indexes
    for (int i = 0; i < (int)remove.size(); i++)
    {
        // erase element at index
        centerPile.erase(centerPileOne.begin() + (remove[i] - decrement));
        // Reduce all future indexes by 1
        decrement++;
    }
    return count;
}

bool GameEngine::containsFirstPlayer()
{
    bool contains = false;
    if (centerPileOne[0] == FIRSTPLAYER)
    {
        centerPileOne.erase(centerPileOne.begin());
        contains = true;
    }
    return contains;
}

void GameEngine::fillBag(TileList *bag)
{
    if (this->bag != nullptr)
        delete this->bag;
    this->bag = bag;
}

void GameEngine::fillLid(TileList *lid)
{
    if (this->lid != nullptr)
        delete this->lid;
    this->lid = lid;
}

void GameEngine::fillCenterPile(std::vector<TileType> centerPileOne)
{
    this->centerPileOne = centerPileOne;
}

void GameEngine::fillFactories(Factory *factories[])
{
    for (int i = 0; i < NUM_FACTORIES + factoryModifier; ++i)
    {
        if (this->factories[i] != nullptr)
            delete this->factories[i];
        this->factories[i] = factories[i];
    }
}

Player *GameEngine::addPlayer(string name)
{
    //creates a new player
    return addPlayer(name, 0, new Mosaic());
}

Player *GameEngine::addPlayer(std::string name, int score, Mosaic *mosaic)
{
    players.push_back(new Player(name, score, mosaic));
    return players.back();
}

void GameEngine::addPlayers()
{
    system("clear");
    //Asks for number of players
    bool isValidNum = false;
    std::string strToInt;
    do
    {
        menu->printMessage("Please Enter Number of Players: (2-4)");
        strToInt = menu->getInput();
        if (strToInt == "2" || strToInt == "3" || strToInt == "4")
        {
            isValidNum = true;
            numberOfPlayers = std::stoi(strToInt);
        }
        else if (strToInt == "help")
        {
            helpMenu("new");
        }
        else
        {
            menu->printMessage("Error - Invalid number of players");
        }
    } while (!isValidNum);

    //Checks for player names and adds them to player vector
    string name;

    for (int i = 0; i < numberOfPlayers; i++)
    {
        bool hasValidName = true;
        do
        {
            std::cout << "Please Enter the Name of Player " << i + 1 << ": " << std::endl;
            name = menu->getInput();
            hasValidName = isNotWhiteSpace(name);
            if (!hasValidName)
                menu->printMessage("Error - Invalid Name");
            if (name == "help")
            {
                helpMenu("new");
                hasValidName = false;
            }
        } while (!hasValidName && !std::cin.eof());
        addPlayer(name);
    }
    bool isValid = false;
    while (!isValid)
    {
        menu->printMessage("Please Choose the number of Center Piles you would Like: (1-2)");
        strToInt = menu->getInput();
        if (strToInt == "1" || strToInt == "2")
        {
            isValid = true;
            numberOfCenterPiles = std::stoi(strToInt);
        }
        else if(strToInt == "help"){
            helpMenu("new");
        }
        else
        {
            menu->printMessage("Invalid number of CenterPiles, Try Again!");
        }
    }

    system("clear");
    std::cout << "Let's Play!";
}

std::vector<TileType> GameEngine::getCenterPile()
{
    return centerPileOne;
}

TileList *GameEngine::getBag()
{
    return bag;
}

TileList *GameEngine::getLid()
{
    return lid;
}

Player *GameEngine::getPlayerTurnID()
{
    return playerTurnID;
}

Player *GameEngine::getPlayer(int playerIndex)
{
    return players.at(playerIndex);
}

bool GameEngine::hasPlayerWon()
{
    bool playerWon = false;
    unsigned int playerIndex = 0;
    while (!playerWon && playerIndex < players.size())
        playerWon = players.at(playerIndex++)->hasWon();
    return playerWon;
}

bool GameEngine::centerPileContains(TileType tileType)
{
    bool centerPileOneContainsTile = false;
    unsigned int index = 0;
    while (!centerPileOneContainsTile && index < centerPileOne.size())
    {
        centerPileOneContainsTile = centerPileOne[index] == tileType;
        index++;
    }
    return centerPileOneContainsTile;
}

void GameEngine::helpMenu(std::string state)
{
    system("clear");
    std::cout << ("=================================================== Help Menu ===================================================") << std::endl;
    std::cout << ("Command:                    Example:                Description") << std::endl;

    if (state == "menu")
    {
        std::cout << "<option Number>:            2                       Selects an option from the menu with the corresponding number." << std::endl;
        std::cout << "4 / ^D:                     Exit Game               Ends the Game." << std::endl;
        std::cout << "help:                       help                    Prints a help menu." << std::endl;
    }
    else if (state == "load")
    {
        std::cout << "saves/<filename>:           saves/testfile         Loads from file called 'testfile'." << std::endl;
        std::cout << "^D :                        Exit Game              Ends the Game." << std::endl;
    }
    else if (state == "new")
    {
        std::cout << "^D :                        Exit Game              Ends the Game." << std::endl;
        std::cout << std::endl;
        std::cout << "Please Follow The Prompt and enter in User Details so the Game can Begin!!" << std::endl;
        std::cout << "Did you know that a hashtag symbol is called an octothorpe?" << std::endl;
    }
    else if(state == "game"){
        
    }

    std::cout << ("=================================================================================================================") << std::endl;
    std::cout << ("Press Enter To Continue...") << std::endl;
    std::cin.ignore();
    system("clear");
}
