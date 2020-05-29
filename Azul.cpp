#include "GameEngine.h"
#include "Saver.h"
#include "Menu.h"
#include "Saver.h"

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
            menu.printMessage("Starting a New Game");
            engine = new GameEngine(&menu);
            exit = engine->playGame(argv[0]);
        }
        else if (input == "2")
        {
            menu.printMessage("Enter the filename from which load a game");
            std::string fileName = menu.getInput();
            if (!std::cin.eof())
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
                    }
                }
                catch (const char* errorMessage)
                {
                    std::string message(errorMessage);
                    menu.printMessage("Error reading file - "+message+"\n");
                    std::cout << "Press Enter to Continue... \n"; 
                    std::cin.ignore();
                }
            }
            else exit = true;
            std::system("clear");
        }
        else if (input == "3")
        {
            menu.printCredits();
        }
        else if (std::cin.eof() || input == "4")
        {
            exit = true;
        }
        else if(input == "help"){
            engine->helpMenu(gameState);
        }
        else
        {
            menu.printMessage("Invalid input, try again");
        }
    } while (!exit);
    menu.printMessage("Goodbye");
    if (engine != nullptr) delete engine;

    system("clear");
    return 0;
}
