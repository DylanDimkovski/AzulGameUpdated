#ifndef MOSIAC_H
#define MOSIAC_H

#include "Types.h"
#include "Factory.h"
#include "Line.h"
#include "TileList.h"

class Mosaic
{
    public:
        Mosaic();
        ~Mosaic();
        bool isFilled(int row, int col);
        void setFilled(int row, int col, bool filled);
        void insertTilesIntoLine(int row, int quantity, TileType tileType);
        void addToBrokenTiles(int quantity, TileType tileType);
        Line* getLine(int line);
        TileList getBrokenTiles();

private:
    Line *lines[NUMBER_OF_LINES];
    bool wall[NUMBER_OF_LINES][NUMBER_OF_LINES];
    // may need to be pointer
    TileList brokenTiles;
};

#endif
