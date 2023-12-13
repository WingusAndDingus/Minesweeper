#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
using namespace sf;

class Tile {
    bool hasmine = false;
    bool flagged = false;
public:
    int tilenum;
    int colNum;
    int rowNum;
    int adjMines;
    std::vector<Tile*> adjacentTiles;
    bool revealed = false;
    bool blown = false;
    Sprite tile;
    Sprite Flag;
    Sprite Mine;
    Sprite Digit;
    //Constructor
    Tile(int num, int colNu, int rowNu) {
        tilenum = num;
        colNum = colNu;
        rowNum = rowNu;
        adjMines = 0;
        tile.setOrigin(tile.getLocalBounds().width / 2.0f, tile.getLocalBounds().height / 2.0f);
        Flag.setOrigin(Flag.getLocalBounds().width / 2.0f, Flag.getLocalBounds().height / 2.0f);
        Mine.setOrigin(Mine.getLocalBounds().width / 2.0f, Mine.getLocalBounds().height / 2.0f);
    }
    
    //Place mine
    void minePlace() {
        hasmine = true;
    }
    //Remove Mine
    void removeMine() {
        hasmine = false;
    }
    //Flag this tile
    void flag() {
        flagged = true;
    }
    //Unflag this tile
    void unFlag() {
        flagged = false;
    }
    //Insert Data
    void insert(std::vector<Tile*> adtiles) {
        adjacentTiles = adtiles;
        for (int i = 0; i < adjacentTiles.size(); i++) {
            if (adjacentTiles[i]->hasMine()) {
                adjMines = adjMines + 1;
            }
        }
    }
    //Get wether it is flagged
    bool isFlagged() {
        return flagged;
    }
    //Get whether it has a mine
    bool hasMine() {
        return hasmine;
    }
    //Get Tile number
    int number() {
        return tilenum;
    }
};