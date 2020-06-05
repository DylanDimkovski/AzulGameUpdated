#include "GameEngine.h"

#include "Saver.h"
#include "textColour.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <unistd.h>

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
    delete menu;
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
        //Check if and store which players have the same score
        Player *winningPlayer = players[playerTurnCount];
        std::vector<Player *> allDrawnPlayers;
        for (int i = 0; i < (int)players.size(); i++)
        {
            if (!((i + 1) == (int)players.size()))
            {
                if (players[i] == players[i + 1])
                {
                    drawnPlayerId.push_back(i);
                    drawnPlayerId.push_back(i);
                }
            }
        }

        //Find who has the highest Score
        for (auto player : players)
        {
            if (player->getScore() > winningPlayer->getScore())
            {
                winningPlayer = player;
            }
        }

        allDrawnPlayers.push_back(winningPlayer);

        //Check if anyone has the same highest score
        for (int i = 0; i < (int)drawnPlayerId.size(); i++)
        {
            Player *playerCheck;
            if (winningPlayer->getScore() == players[drawnPlayerId[i]]->getScore())
            {
                playerCheck = players[drawnPlayerId[i]];
                allDrawnPlayers.push_back(playerCheck);
            }
        }

        menu->gameOver(allDrawnPlayers);
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
        outputBoard();

        //Bool to check whether input command has been executed
        bool inputDone = false;

        //Handle user input
        do
        {
            std::stringstream ss;

            if (players[playerTurnCount]->checkAi() == false)
            {
                ss << (menu->getInput());
            }
            else if (players[playerTurnCount]->checkAi() == true)
            {
                //Ai Functionality
                ss << returnAiOutput();
            }

            if (!std::cin.eof())
            {
                string errorMessage = "Command not recognised";
                string command;

                ss >> command;

                if (command == "help")
                {
                    helpMenu("game");
                }
                else if (command == "turn")
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

                                    if (!centerPileOne.empty() && centerPileContains(tileType, centerPileOne))
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
                                            if (containsFirstPlayer(centerPileOne, centerPileTwo))
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
                                else if (factoryNum == 11)
                                {
                                    if (!centerPileTwo.empty() && centerPileContains(tileType, centerPileTwo))
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
                                            if (containsFirstPlayer(centerPileOne, centerPileTwo))
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
                                    if (numberOfCenterPiles == 1)
                                    {
                                        pileNum = 10;
                                    }
                                    else if (numberOfCenterPiles == 2)
                                    {
                                        menu->printMessage("Please Enter Which Center Factory to add excess tiles: ");

                                        std::string numberConversion;
                                        bool inputCheck = false;

                                        //Check if AI is playing
                                        if (players[playerTurnCount]->checkAi() == true)
                                        {
                                            inputCheck = true;
                                            pileNum = rand() % 2 + 10;
                                        }

                                        while (!inputCheck)
                                        {
                                            numberConversion = menu->getInput();

                                            if (numberConversion == "10" || numberConversion == "11")
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
                                    sleep(2);
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
                if (!inputDone && command != "help")
                {
                    menu->printMessage("Invalid input: " + errorMessage + ", try again");
                    sleep(1);
                    system("clear");
                    outputBoard();
                }
                else if (!inputDone && command == "help")
                {
                    outputBoard();
                }
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
                players[playerTurnCount] = player;
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
    bool factoryCheck;

    if (factoryNum >= 0 && factoryNum >= 0 && factoryNum <= NUM_FACTORIES + factoryModifier)
    {
        factoryCheck = true;
    }
    else if (factoryNum == 10 || factoryNum == 11)
    {
        factoryCheck = true;
    }
    else
    {
        factoryCheck = false;
    }

    return factoryCheck;
}

bool GameEngine::roundOver()
{
    bool returnValue = true;
    if (numberOfCenterPiles == 1)
    {
        returnValue = centerPileOne.empty() && factoriesAreEmpty();
    }
    else if (numberOfCenterPiles == 2)
    {
        returnValue = centerPileOne.empty() && factoriesAreEmpty() && centerPileTwo.empty();
    }
    return returnValue;
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

int GameEngine::drawFromCenter(TileType colour, std::vector<TileType> &centerPile)
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
        centerPile.erase(centerPile.begin() + (remove[i] - decrement));
        // Reduce all future indexes by 1
        decrement++;
    }
    return count;
}

bool GameEngine::containsFirstPlayer(std::vector<TileType> &centerPileOne, std::vector<TileType> &centerPileTwo)
{
    bool contains = false;
    if (numberOfCenterPiles == 2)
    {
        if (centerPileOne[0] == FIRSTPLAYER)
        {
            centerPileOne.erase(centerPileOne.begin());
            centerPileTwo.erase(centerPileTwo.begin());
            contains = true;
        }
        else if (centerPileTwo[0] == FIRSTPLAYER)
        {
            centerPileOne.erase(centerPileOne.begin());
            centerPileTwo.erase(centerPileTwo.begin());
            contains = true;
        }
    }
    else if (numberOfCenterPiles == 1)
    {
        if (centerPileOne[0] == FIRSTPLAYER)
        {
            centerPileOne.erase(centerPileOne.begin());
            contains = true;
        }
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

Player *GameEngine::addPlayer(string name, bool isAi)
{
    //creates a new player
    return addPlayer(name, 0, new Mosaic(), isAi);
}

Player *GameEngine::addPlayer(std::string name, int score, Mosaic *mosaic, bool isAi)
{
    players.push_back(new Player(name, score, mosaic, isAi));
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
        menu->printMessage("Please Enter Number of Total Players: (2-4)");
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

    isValidNum = false;
    numberOfAiPlayers = 0;
    do
    {
        menu->printMessage("How many of those players would you like to be Computer Controlled?");
        strToInt = menu->getInput();
        if (std::stoi(strToInt) <= numberOfPlayers)
        {
            isValidNum = true;
            numberOfAiPlayers = std::stoi(strToInt);
        }

    } while (!isValidNum);

    //Checks for player names and adds them to player vector
    string name;
    if (numberOfPlayers != numberOfAiPlayers)
    {
        for (int i = 0; i < numberOfPlayers - numberOfAiPlayers; i++)
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
            addPlayer(name, false);
        }
    }
    //Adds Computer Players
    for (int i = 0; i < numberOfAiPlayers; i++)
    {
        addPlayer(nameAi(), true);
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
        else if (strToInt == "help")
        {
            helpMenu("new");
        }
        else
        {
            menu->printMessage("Invalid number of CenterPiles, Try Again!");
        }
    }

    system("clear");
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

bool GameEngine::centerPileContains(TileType tileType, std::vector<TileType> centerPile)
{
    bool centerPileContainsTile = false;
    unsigned int index = 0;
    while (!centerPileContainsTile && index < centerPile.size())
    {
        centerPileContainsTile = centerPile[index] == tileType;
        index++;
    }
    return centerPileContainsTile;
}

void GameEngine::helpMenu(std::string state)
{
    system("clear");
    std::cout << ("=================================================== Help Menu ===================================================") << std::endl;
    std::cout << ("Command:                         Example:                Description") << std::endl;

    if (state == "menu")
    {
        std::cout << "<option Number>:                 2                       Selects an option from the menu with the corresponding number." << std::endl;
        std::cout << "4 / ^D:                          Exit Game               Ends the Game." << std::endl;
        std::cout << "help:                            help                    Prints a help menu." << std::endl;
    }
    else if (state == "load")
    {
        std::cout << "saves/<filename>:                saves/testfile          Loads from file called 'testfile'." << std::endl;
        std::cout << "^D :                             Exit Game               Ends the Game." << std::endl;
    }
    else if (state == "new")
    {
        std::cout << "^D :                             Exit Game               Ends the Game." << std::endl;
        std::cout << std::endl;
        std::cout << "Please Follow The Prompt and enter in User Details so the Game can Begin!!" << std::endl;
        std::cout << "Did you know that a hashtag symbol is called an octothorpe?" << std::endl;
    }
    else if (state == "game")
    {
        std::cout << "^D :                             Exit Game               Ends the Game." << std::endl;
        std::cout << "turn <Factory> <Colour> <Line>   turn 2 R 2              Input to determine what move to make E.X." << std::endl;
        std::cout << "                                                         Takes all R tiles from Factory 2 and places into Line 2" << std::endl;
    }

    std::cout << ("=================================================================================================================") << std::endl;
    std::cout << ("Press Enter To Continue...") << std::endl;
    std::cin.ignore();
    system("clear");
}

void GameEngine::outputBoard()
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
    menu->handStart(playerTurnID->getName(), players, numberOfPlayers);
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

    std::cout << ("===========================================================================") << std::endl;
    if (numberOfPlayers == 2)
    {
        passThroughKey = 1;
    }
    else
    {
        passThroughKey = 2;
    }
    playerCount = 0;
    for (int i = 0; i < passThroughKey; i++)
    {
        std::cout << players[playerCount]->getName() << "'s Broken Pile: "
                  << players[playerCount]->getMosaic()->getBrokenTiles()->toString()
                  << "  ";
        if (numberOfPlayers == 4 || playerCount == 0)
        {
            std::cout
                << players[playerCount + 1]->getName() << "'s Broken Pile: "
                << players[playerCount + 1]->getMosaic()->getBrokenTiles()->toString()
                << "  || ";
        }
        else
        {
            std::cout << "                      ";
        }
        std::cout
            << players[playerCount]->getName() << " Score: "
            << players[playerCount]->getScore()
            << "  ";
        if (numberOfPlayers == 4 || playerCount == 0)
        {
            std::cout
                << players[playerCount + 1]->getName() << " Score: "
                << players[playerCount + 1]->getScore();
        }
        std::cout << std::endl;
        if (numberOfPlayers >= 3)
        {
            playerCount = 2;
        }
    }
    std::cout << ("===========================================================================") << std::endl;
}

std::string GameEngine::nameAi()
{
    int num = 0;
    std::string names[] = {
        "Michaele",
        "Deidre",
        "Hoa",
        "Kandy",
        "Tonita",
        "Fonda",
        "Daisey",
        "Kim",
        "Guy",
        "Myrtis",
        "Han",
        "Jenna",
        "Bennett",
        "Pearline",
        "Henriett",
        "Rikki",
        "Corey",
        "Genie",
        "William",
        "Tresa",
        "Patty",
        "Concetta",
        "Alvina",
        "Chanell",
        "Torrie",
        "Matthew",
        "Arnita",
        "Delora",
        "Elfreda",
        "Debra",
    };
    num = (rand() % 30);
    return names[num];
}

std::string GameEngine::returnAiOutput()
{
    //Create check variables
    std::string output, lineCheck, mosaicColour;
    bool lineOneCheck = false;
    bool lineFilledCheck = false;
    int line = 0;
    TileType colour = NOTILE;

    int lineSize = players[playerTurnCount]->getMosaic()->getLine(0)->getMaxSize();
    int numTiles = players[playerTurnCount]->getMosaic()->getLine(0)->getNumTiles();

    for (int i = 0; i < 5 - lineSize; i++)
    {
        lineCheck += "  ";
    }

    for (int i = 0; i < lineSize - numTiles; i++)
    {
        lineCheck += ". ";
    }

    output += "turn ";

    //Check if Top Row is filled. If Not Fill it with valid Colour
    if ("        . " == lineCheck)
    {
        line = 1;
        for (int i = 0; i < 5; i++)
        {
            if (!players[playerTurnCount]->getMosaic()->isFilled(0, i) && !lineOneCheck)
            {
                colour = menu->getMasterWall(0, i);
                output += getSuitableFactory("First Line", colour) + " " + std::string(1, (char)colour) + " " + std::to_string(line);
                lineOneCheck = true;
                std::cout << output << std::endl;
            }
        }
    }

    //If the Top Row is already filled. Or can't be filled this round, then fill Adjacent Tiles from center out.
    if (lineOneCheck == false)
    {
        line = 2;
        int row = 1, column = 0;
        do
        {
            std::cout << players[playerTurnCount]->getMosaic()->getLine(line)->getNumTiles() << ", Line: " << line << std::endl;
            
            //Check for partially filled line
            if (players[playerTurnCount]->getMosaic()->getLine(line)->getNumTiles() > 0)
            {
                colour = players[playerTurnCount]->getMosaic()->getLine(line)->getTileType();
                if (players[playerTurnCount]->getMosaic()->getLine(line)->canAddTiles(colour))
                {
                    lineFilledCheck = true;
                }
            }
            //If line is empty then fill it with colour from wall
            else if (players[playerTurnCount]->getMosaic()->isFilled(row, column) == false)
            {
                colour = menu->getMasterWall(row, column);

                std::string colourAvaliable = getSuitableFactory("First Line", colour);
                if (colourAvaliable == "")
                {
                    lineFilledCheck = false;
                }
                else
                {
                    lineFilledCheck = true;
                }
            }

            //If no line can be filled with the proper colour, move onto next column
            if (lineFilledCheck == false)
            {
                column++;
                line++;

                if (column == 4)
                {
                    row++;
                    column = 0;
                }

                if (row == 4)
                {
                    row = 1;
                    column = 0;
                }

                if (line == 5)
                {
                    line = 2;
                }
            }
        } while (!lineFilledCheck);

        output += getSuitableFactory("First Line", colour) + " " + std::string(1, (char)colour) + " " + std::to_string(line);
        std::cout << output << std::endl;
    }

    //Using a point based system check if the the ai can win at the end of the round. If yes then end game.

    return output;
}

std::string GameEngine::getSuitableFactory(std::string check, TileType colour)
{
    std::string returnVariable;
    //bool logicCheck = false;
    bool emptyFactories = true;
    if (check == "First Line")
    {

        //Check if factories have the colour
        for (int i = 0; i < NUM_FACTORIES + factoryModifier; i++)
        {
            if (factories[i]->contains(colour) && emptyFactories == true)
            {
                returnVariable = std::to_string(i + 1);
                emptyFactories = false;
            }
        }

        //If the factories don't contain the colour check the center piles
        if (emptyFactories == true)
        {
            //If the factories are empty and the game only has 1 Center Pile
            if (numberOfCenterPiles == 1)
            {
                for (int i = 0; i < (int)centerPileOne.size(); i++)
                {
                    if (centerPileOne[i] == colour)
                    {
                        returnVariable = "10";
                    }
                }
            }

            //If factories are empty and the game has 2 Center Piles
            else if (numberOfCenterPiles == 2)
            {
                for (int i = 0; i < (int)centerPileOne.size(); i++)
                {
                    if (centerPileOne[i] == colour)
                    {
                        returnVariable = "10";
                    }
                }
                for (int i = 0; i < (int)centerPileTwo.size(); i++)
                {
                    if (centerPileTwo[i] == colour)
                    {
                        returnVariable = "11";
                    }
                }
            }
        }
    }
    return returnVariable;
}
