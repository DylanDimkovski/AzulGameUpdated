#include "GameEngine.h"
#include "Saver.h"
#include "Menu.h"
#include "Saver.h"
#include <unistd.h>

int main(int argc, char const *argv[])
{
    system("clear");
    //GameEngine engine = new GameEngine();
    Menu menu;
    bool exit = false;
    std::string input;
    GameEngine *engine = nullptr;
    Saver saver;
    std::string gameState = "menu";
    do
    {
        menu.printMenu();
        input = menu.getInput();
        if (input == "1")
        {
            gameState = "new";
            menu.printMessage("Starting a New Game");
            engine = new GameEngine(&menu);
            exit = engine->playGame(argv[0]);
            gameState = "game";
        }
        else if (input == "2")
        {
            gameState = "load";
            menu.printMessage("Enter the filename from which load a game");
            std::string fileName = menu.getInput();
            if (fileName == "help")
            {
                engine->helpMenu(gameState);
            }
            else if (!std::cin.eof())
            {
                try
                {
                    engine = saver.load(fileName, &menu);
                    if (engine != nullptr)
                    {
                        menu.printMessage("Azul game successfully loaded");
                        menu.printMessage("Press Enter to Play...");
                        std::cin.ignore();
                        std::system("clear");
                        exit = engine->playGame();
                        gameState = "game";
                    }
                }
                catch (const char *errorMessage)
                {
                    std::string message(errorMessage);
                    menu.printMessage("Error reading file - " + message + "\n");
                    std::cout << "Press Enter to Continue... \n";
                    std::cin.ignore();
                }
            }
            else
                exit = true;
            std::system("clear");
        }
        else if (input == "3")
        {
            menu.printCredits();
            gameState = "credits";
        }
        else if (std::cin.eof() || input == "4")
        {
            exit = true;
        }
        else if (input == "help")
        {
            engine->helpMenu(gameState);
        }
        else
        {
            if (gameState == "credits")
            {
                system("clear");
                gameState = "menu";
            }
            else
            {
                menu.printMessage("Invalid input, try again");
                menu.printMessage("Loading New Menu...");
                sleep(2);
                system("clear");
                gameState = "menu";
            }
        }
    } while (!exit);
    menu.printMessage("Goodbye");
    if (engine != nullptr)
        delete engine;

    system("clear");
    return 0;
}
