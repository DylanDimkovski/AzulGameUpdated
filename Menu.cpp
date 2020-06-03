#include "Menu.h"
#include "textColour.h"

    Menu::Menu(){
    colourArray = new std::string**[5];
    std::string tempArray[5][5] = {TC_BG_DARKBLUE, TC_BG_YELLOW, TC_BG_RED, TC_BG_BLACK, TC_BG_LIGHTBLUE,
                                    TC_BG_LIGHTBLUE, TC_BG_DARKBLUE, TC_BG_YELLOW, TC_BG_RED, TC_BG_BLACK,
                                    TC_BG_BLACK, TC_BG_LIGHTBLUE, TC_BG_DARKBLUE, TC_BG_YELLOW, TC_BG_RED,
                                    TC_BG_RED, TC_BG_BLACK, TC_BG_LIGHTBLUE, TC_BG_DARKBLUE, TC_BG_YELLOW,
                                    TC_BG_YELLOW, TC_BG_RED, TC_BG_BLACK, TC_BG_LIGHTBLUE, TC_BG_DARKBLUE};
    for (int i = 0; i < 5; i++)
    {
        colourArray[i] = new std::string*[5];
        for (int j = 0; j < 5; j++)
        {
            colourArray[i][j] = new std::string(tempArray[i][j]);
        }   
    }
}

    Menu::~Menu(){
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            delete colourArray[i][j];
        }
        delete[] colourArray[i];
    }
    delete[] colourArray;
}

void Menu::printMenu()
{
    std::cout << TC_LIGHTBLUE << "   AA  " << TC_MAGENTA << " ZZZZZ" << TC_YELLOW << " UU  UU" << TC_WHITE << " LL    "
              << "  ||" << TC_RESET << std::endl
              << TC_LIGHTBLUE << "  A  A " << TC_MAGENTA << "    ZZ" << TC_YELLOW << " UU  UU" << TC_WHITE << " LL    "
              << "  ||" << TC_RESET << std::endl
              << TC_LIGHTBLUE << " AAAAAA" << TC_MAGENTA << "   ZZ " << TC_YELLOW << " UU  UU" << TC_WHITE << " LL    "
              << "  ||" << TC_RESET << std::endl
              << TC_LIGHTBLUE << "AA   AA" << TC_MAGENTA << " ZZ   " << TC_YELLOW << " UUUUUU" << TC_WHITE << " LLLLLL"
              << "  ||" << TC_RESET << std::endl
              << TC_LIGHTBLUE << "AA   AA" << TC_MAGENTA << " ZZZZZ" << TC_YELLOW << " UUUUUU" << TC_WHITE << " LLLLLL"
              << "  OO" << TC_RESET << std::endl
              << TC_LIGHTBLUE << TC_BOLD << "--------------------------------------------" << std::endl
              << "Don't forget to type 'help' if you get lost!"
              << std::endl
              << "--------------------------------------------" << std::endl
              << TC_RESET
              << "Please Enter A Number: " << std::endl
              << "1. NEW GAME" << std::endl
              << "2. LOAD GAME" << std::endl
              << "3. CREDITS" << std::endl
              << "4. QUIT" << std::endl
              << TC_RESET
              << std::endl;
}
std::string Menu::getInput()
{
    std::string input;
    std::cout << "> ";
    std::getline(std::cin, input);
    std::cout << input << std::endl;
    return input;
}

void Menu::printCredits()
{
    std::system("clear");
    std::cout << std::endl
              << "----------------------------------" << std::endl
              << "Name: Luca Cave" << std::endl
              << "Student ID: s3787946" << std::endl
              << "Email: s3787946@student.rmit.edu.au" << std::endl
              << std::endl
              << "Name: Michael Sartorel" << std::endl
              << "Student ID: s3786267" << std::endl
              << "Email: s3786267@student.rmit.edu.au" << std::endl
              << std::endl
              << "Name: Dylan Dimkovski" << std::endl
              << "Student ID: s3717379" << std::endl
              << "Email: s3717379@student.rmit.edu.au" << std::endl
              << "----------------------------------" << std::endl
              << "Please Press Enter to Continue..."
              << std::endl;
    std::cin.ignore();
    std::system("clear");
}

void Menu::printMessage(std::string message)
{
    std::cout << std::endl
              << message
              << std::endl;
}

void Menu::handStart(string playername, std::vector<Player *> player, int numOfPlayers)
{
    std::cout << "TURN FOR PLAYER: " << TC_BOLD << playername << TC_RESET
              << "                                ";
    std::cout << std::endl;
    std::cout << "===========================================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Factories: " << std::endl;
}

void Menu::printFactory(int id, string contents, int playerModifier)
{
    std::cout << id << ": " << contents << std::endl;
    if (playerModifier == 3)
    {
        std::cout << (id == 5 + 2 ? "\n" : "");
    }
    else if (playerModifier == 4)
    {
        std::cout << (id == 5 + 4 ? "\n" : "");
    }
    else
    {
        std::cout << (id == 5 ? "\n" : "");
    }
}

void Menu::printFactory(std::vector<TileType> *centerPile)
{
    std::cout << "10: ";
    for (TileType tile : *centerPile)
    {
        std::cout << (char)tile << " ";
    }
    std::cout << std::endl;
}

void Menu::printFactory(std::vector<TileType> *centerPileOne, std::vector<TileType> *centerPileTwo)
{
    std::cout << "10: ";
    for (TileType tile : *centerPileOne)
    {
        std::cout << (char)tile << " ";
    }
    std::cout << std::endl;
    std::cout << "11: ";
    for (TileType tile : *centerPileTwo)
    {
        std::cout << (char)tile << " ";
    }
    std::cout << std::endl;
}

void Menu::printMosaic(std::vector<Player *> playerVector, int tempPlayerOneID)
{
    {
        Player *player = playerVector[tempPlayerOneID];
        std::cout << "Mosaic for " << player->getName() << std::endl;
        for (int j = 0; j < NUMBER_OF_LINES; j++)
        {
            std::cout << j + 1 << ": ";
            std::string output;
            int lineSize = player->getMosaic()->getLine(j)->getMaxSize();
            int numTiles = player->getMosaic()->getLine(j)->getNumTiles();

            for (int i = 0; i < 5 - lineSize; i++)
            {
                output += "  ";
            }

            for (int i = 0; i < lineSize - numTiles; i++)
            {
                output += TC_BG_WHITE + ". " + TC_RESET;
            }
            for (int i = 0; i < numTiles; i++)
            {
                output += " ";
                output += player->getMosaic()->getLine(j)->getTileType();
            }
            output += " || ";

            for (int i = 0; i < NUMBER_OF_LINES; i++)
            {
                if (player->getMosaic()->getWallLine(j)[i] == true)
                {
                    output += *colourArray[j][i] + " ";
                    output += master_wall[j][i];
                    output += TC_RESET;
                } //test comment
                else
                {
                    output += TC_BG_WHITE + " ." + TC_RESET;
                }
            }
            std::cout << output << std::endl;
        }
        std::cout << "==========================================================================="
                  << std::endl;
    }
}

void Menu::printMosaic(std::vector<Player *> playerVector, int tempPlayerOneID, int tempPlayerTwoID)
{
    Player *playerONE = playerVector[tempPlayerOneID];
    Player *playerTWO = playerVector[tempPlayerTwoID];
    std::string gap = "";

    for (int i = 0; i < 36-(int)playerONE->getName().size(); i++)
    {
        gap += " ";
    }
    
    count = 0;
    std::cout << "Mosaic for " << playerONE->getName()
              << gap
              << "Mosaic for " << playerTWO->getName()
              << std::endl;
    for (int j = 0; j < NUMBER_OF_LINES; j++)
    {
        std::cout << j + 1 << ": ";
        std::string outputOne;
        std::string outputTwo;
        int lineSize = playerONE->getMosaic()->getLine(j)->getMaxSize();
        int numTiles = playerONE->getMosaic()->getLine(j)->getNumTiles();

        for (int i = 0; i < 5; i--)
        {
            if (j == i)
            {
            }
        }

        for (int i = 0; i < 5 - lineSize; i++)
        {
            outputOne += "  ";
        }

        for (int i = 0; i < lineSize - numTiles; i++)
        {
            outputOne += TC_BG_WHITE + ". " + TC_RESET;
        }
        for (int i = 0; i < numTiles; i++)
        {
            outputOne += " ";
            outputOne += playerONE->getMosaic()->getLine(j)->getTileType();
        }

        outputOne += " || ";
        
        for (int i = 0; i < NUMBER_OF_LINES; i++)
        {
            if (playerONE->getMosaic()->getWallLine(j)[i] == true)
            {
                outputOne += " ";
                outputOne += *colourArray[j][i];
                outputOne += master_wall[j][i];
                outputOne += TC_RESET;
                count++;
            }
            else
            {
                outputOne += *colourArray[j][i] + " ." + TC_RESET;
                count++;
            }
        }
        std::cout << outputOne
                  << "                    ";
        std::cout << j + 1 << ": ";
        lineSize = playerTWO->getMosaic()->getLine(j)->getMaxSize();
        numTiles = playerTWO->getMosaic()->getLine(j)->getNumTiles();

        for (int i = 0; i < 5 - lineSize; i++)
        {
            outputTwo += "  ";
        }

        for (int i = 0; i < lineSize - numTiles; i++)
        {
            outputTwo += TC_BG_WHITE + ". " + TC_RESET;
        }
        for (int i = 0; i < numTiles; i++)
        {
            outputTwo += " ";
            outputTwo += playerTWO->getMosaic()->getLine(j)->getTileType();
        }

        outputTwo += " || ";

        for (int i = 0; i < NUMBER_OF_LINES; i++)
        {
            if (playerTWO->getMosaic()->getWallLine(j)[i] == true)
            {
                outputTwo += " ";
                outputTwo += *colourArray[j][i];
                outputTwo += master_wall[j][i];
                outputTwo += TC_RESET;
                count++;
            }
            else
            {
                outputTwo += *colourArray[j][i] + " ." + TC_RESET;
                count++;
            }
        }
        std::cout << outputTwo
                  << std::endl;
    }
    std::cout << "==========================================================================="
              << std::endl;
}

void Menu::printScore(string name, int score)
{
    std::cout << name << " score: " << score << std::endl;
}

void Menu::gameOver(Player *player)
{
    std::cout << "=== Game Over ===" << std::endl;
    std::cout << "Player " << player->getName() << " wins!" << std::endl;
    printScore(player->getName(), player->getScore());
}

void Menu::gameOver(string name1, string name2, int score)
{
    std::cout << "=== Game Over ===" << std::endl;
    std::cout << "Draw!" << std::endl;
    std::cout << "Player " << name1 << " score: " << score << std::endl;
    std::cout << "Player " << name2 << " score: " << score << std::endl;
}

TileType Menu::getMasterWall(int i, int j){
    return master_wall[i][j];
}
