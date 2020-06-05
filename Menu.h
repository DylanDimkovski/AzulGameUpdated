#ifndef MENU_H
#define MENU_H

#include <string>
#include <iostream>
#include <vector>
#include "Player.h"
#include "Menu.h"
#include "textColour.h"

class Menu
{
public:
    //Constructor
    Menu();
    //Destructor
    ~Menu();
    //Print main menu
    void printMenu();
    //Get input with getLine
    std::string getInput();
    //Print credits
    void printCredits();
    //Print given string
    void printMessage(std::string message);
    //Print GUI for player turn
    void handStart(std::string playerName, std::vector<Player *> player, int numOfPlayers);
    //Print a factory
    void printFactory(int id, string contents, int playerModifier);
    //Print One centerPile
    void printFactory(std::vector<TileType> *centerPile);
    //Print both centerPiles
    void printFactory(std::vector<TileType> *centerPileOne, std::vector<TileType> *centerPileTwo);
    //Print player's mosaic
    void printMosaic(std::vector<Player *> playerVector, int tempPlayerOneID);
    //Print Multiple Mosaics
    void printMosaic(std::vector<Player *> players, int playerOneID, int playerTwoID);
    //Print player's score
    void printScore(string name, int score);
    //Print Game Over
    void gameOver(std::vector<Player *> winningPlayers);
    //Get master_wall
    TileType getMasterWall(int i, int j);

public:
    int count;

private:
    //Master wall pattern for comparison
    TileType master_wall[NUMBER_OF_LINES][NUMBER_OF_LINES] = {{DARKBLUE, YELLOW, RED, BLACK, LIGTHBLUE},
                                                              {LIGTHBLUE, DARKBLUE, YELLOW, RED, BLACK},
                                                              {BLACK, LIGTHBLUE, DARKBLUE, YELLOW, RED},
                                                              {RED, BLACK, LIGTHBLUE, DARKBLUE, YELLOW},
                                                              {YELLOW, RED, BLACK, LIGTHBLUE, DARKBLUE}};
    std::string colourArray[NUMBER_OF_LINES][NUMBER_OF_LINES] = {{TC_BG_DARKBLUE, TC_BG_YELLOW, TC_BG_RED, TC_BG_BLACK, TC_BG_LIGHTBLUE},
                                                                {TC_BG_LIGHTBLUE, TC_BG_DARKBLUE, TC_BG_YELLOW, TC_BG_RED, TC_BG_BLACK},
                                                                {TC_BG_BLACK, TC_BG_LIGHTBLUE, TC_BG_DARKBLUE, TC_BG_YELLOW, TC_BG_RED},
                                                                {TC_BG_RED, TC_BG_BLACK, TC_BG_LIGHTBLUE, TC_BG_DARKBLUE, TC_BG_YELLOW},
                                                                {TC_BG_YELLOW, TC_BG_RED, TC_BG_BLACK, TC_BG_LIGHTBLUE, TC_BG_DARKBLUE}};
};
#endif // !MENU_H