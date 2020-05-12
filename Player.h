#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include "Mosaic.h"

class Player
{
    public:
        Player(std::string name, int score);
        Player(std::string name, Mosaic* mosaic);
        Player(std::string name, Mosaic* mosaic, int score);
        ~Player();
        std::string getName();
        int getScore();
        void calcScore();

    private:
        std::string name;
        int score;
        Mosaic* mosiac;
};

#endif // !PLAYER_H
