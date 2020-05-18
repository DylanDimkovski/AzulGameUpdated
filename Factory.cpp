#include "Factory.h"

Factory::Factory(TileType arr[FACTORY_SIZE])
{
    fill(arr);
}

void Factory::fill(TileType arr[FACTORY_SIZE])
{
    for (int i = 0; i < FACTORY_SIZE; i++)
    {
        tiles[i] = arr[i];
    }
}

std::vector<TileType> Factory::empty()
{
    std::vector<TileType> leftovers;

    for (int i = 0; i < FACTORY_SIZE; i++)
    {
        if (tiles[i] != NOTILE)
        {
            leftovers.push_back(tiles[i]);
            tiles[i] = NOTILE;
        }
    }
    clear = true;
    return leftovers;
}

int Factory::draw(TileType tileType)
{
    int count = 0;
    for (int i = 0; i < FACTORY_SIZE; i++)
    {
        if (tiles[i] == tileType)
        {
            count++;
            tiles[i] = NOTILE;
        }
    }
    return count;
}

string Factory::toString()
{
    string output = "";
    for (int i = 0; i < FACTORY_SIZE; i++)
    {
        output += tiles[i];
        output += " ";
    }

    return output;
}

bool Factory::isEmpty()
{
    return clear;
}