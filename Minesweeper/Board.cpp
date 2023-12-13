#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Tile.cpp"
#include <vector>
#include <random>
using namespace sf;

enum GameStatus {
	Paused,
	Normal,
	Leader,
	Victory,
	Loss,
};

class Board {
	GameStatus gameStat = Normal;
	Texture tileText;
	Texture tileRevText;
	Texture flagText;
	Texture mineText;
	Texture digitText;
	int numRow;
	int numCol;
	std::vector<Tile> tiles;
public:
	bool paused = false;
	int numMines;
	int countVic;
	//Constructor
	Board(int cols, int rows, int mines) {
		tileText.loadFromFile("images/tile_hidden.png");
		tileRevText.loadFromFile("images/tile_revealed.png");
		flagText.loadFromFile("images/flag.png");
		mineText.loadFromFile("images/mine.png");
		numRow = rows;
		numCol = cols;
		int count = 1;
		countVic = 0;
		numMines = mines;
		//Populate board
		for (int i = 1; i <= numRow; i++) {
			for (int j = 1; j <= numCol; j++) {
				Tile tile(count, j, i);
				tiles.push_back(tile);
				count++;
			}
		}

		//Randomly place mines
		placeRandMines();

		//Populate the Adjacent vectors for each mine should happen inside of placeRandMines();

		//Set Their locations
		for (int i = 0; i < tiles.size(); i++) {
			setTile(tiles[i].tile, tiles[i].colNum * 32 - 32, tiles[i].rowNum * 32 - 32);
			setTile(tiles[i].Mine, tiles[i].colNum * 32 - 32, tiles[i].rowNum * 32 - 32);
			setTile(tiles[i].Flag, tiles[i].colNum * 32 - 32, tiles[i].rowNum * 32 - 32);
			setTile(tiles[i].Digit, tiles[i].colNum * 32 - 32, tiles[i].rowNum * 32 - 32);
		}
	}
	//End

	//Process and Populate Adjacent tiles for every tile
	void Adjacent(int cols, int rows) {
		for (int i = 0; i < tiles.size(); i++) {
			tiles[i].adjacentTiles.clear();
			std::vector<Tile*> adtiles;
				//Check a max total of 8 tiles
			Tile* tilePtr;
			int temp;
				// 1 Check to the left
			if ((tiles[i].colNum - 1) >= 1) {
				temp = indexFromXY(tiles[i].colNum - 1, tiles[i].rowNum);
				tilePtr = &tiles[temp];
				adtiles.push_back(tilePtr);
			}
				// 2 Check Left Up
			if ((tiles[i].colNum - 1) >= 1 && (tiles[i].rowNum - 1) >= 1) {
				temp = indexFromXY(tiles[i].colNum - 1, tiles[i].rowNum - 1);
				tilePtr = &tiles[temp];
				adtiles.push_back(tilePtr);				
			}
				// 3 Check Straight Up
			if ((tiles[i].rowNum - 1) >= 1) {
				temp = indexFromXY(tiles[i].colNum, tiles[i].rowNum - 1);
				tilePtr = &tiles[temp];
				adtiles.push_back(tilePtr);
			}
				// 4 Check Right Up
			if ((tiles[i].colNum + 1 <= cols) && (tiles[i].rowNum - 1 >= 1)) {
				temp = indexFromXY(tiles[i].colNum + 1, tiles[i].rowNum - 1);
				tilePtr = &tiles[temp];
				adtiles.push_back(tilePtr);
			}
				// 5 Check to the Right
			if ((tiles[i].colNum + 1) <= cols) {
				temp = indexFromXY(tiles[i].colNum + 1, tiles[i].rowNum);
				tilePtr = &tiles[temp];
				adtiles.push_back(tilePtr);
			}
				// 6 Check Right Down
			if ((tiles[i].colNum + 1) <= cols && (tiles[i].rowNum + 1) <= rows) {
				temp = indexFromXY(tiles[i].colNum + 1, tiles[i].rowNum + 1);
				tilePtr = &tiles[temp];
				adtiles.push_back(tilePtr);
			}
				// 7 Check Straight Down
			if ((tiles[i].rowNum + 1) <= rows) {
				temp = indexFromXY(tiles[i].colNum, tiles[i].rowNum + 1);
				tilePtr = &tiles[temp];
				adtiles.push_back(tilePtr);

			}
				// 8 Check Left Down
			if ((tiles[i].colNum - 1) >= 1 && (tiles[i].rowNum + 1) <= rows) {
				temp = indexFromXY(tiles[i].colNum - 1, tiles[i].rowNum + 1);
				tilePtr = &tiles[temp];
				adtiles.push_back(tilePtr);
			}
			tiles[i].insert(adtiles);
		}
	}

	//Find a tile index based on rowNum nad colNum
	int indexFromXY(int x, int y) {
		for (int i = 0; i < tiles.size(); i++) {
			if (tiles[i].colNum == x && tiles[i].rowNum == y) {
				return i;
			}
		}
		//Should never happen
		return -1;
	}
	//Randomly Place Mines Function
	void placeRandMines() {
		countVic = 0;
		for (int i = 0; i < tiles.size(); i++) {
			tiles[i].revealed = false;
			tiles[i].unFlag();
			tiles[i].adjMines = 0;
			if (tiles[i].hasMine()) {
				tiles[i].removeMine();
			}
		}
		std::random_device rd;     // Only used once to initialise (seed) engine
		std::mt19937 rng(rd());    // Random-number engine used (Mersenne-Twister in this case)
		std::uniform_int_distribution<int> uni(0, tiles.size() - 1); // Guaranteed unbiased
		for (int i = 1; i <= numMines; i++) {
			auto random_integer = uni(rng);
			while (tiles[random_integer].hasMine()) {
				random_integer = uni(rng);
			}
			tiles[random_integer].minePlace();
		}
		Adjacent(numCol, numRow);
	}
	//Set tile Position
	void setTile(sf::Sprite& tile, float x, float y) {
		sf::FloatRect tileRect = tile.getLocalBounds();
		tile.setOrigin(tileRect.left + tileRect.width / 2.0f, tileRect.top + tileRect.height / 2.0f);
		tile.setPosition(sf::Vector2f(x, y));
	}
	//Blow up a mine
	void blowUp() {
		for (int i = 0; i < tiles.size(); i++) {
			if (tiles[i].hasMine()) {
				tiles[i].blown = true;
			}
		}
	}
	//Hide all mines
	void unBlow() {
		for (int i = 0; i < tiles.size(); i++) {
			tiles[i].blown = false;
		}
	}
	//Find which sprite is in those coords
	int findTileIndex(Vector2i coords) {
		for (int i = 0; i < tiles.size(); i++) {
			Vector2f floatCoords(static_cast<float>(coords.x), static_cast<float>(coords.y));
			if (tiles[i].tile.getGlobalBounds().contains(floatCoords)) {
				return i;
			}
		}
		return -1;
	}
	//Function to reveal all adjacent tiles
	void revealBasics(int index) {
		if (!tiles[index].revealed && !tiles[index].isFlagged()) {
			tiles[index].revealed = true;
			countVic++;
			if (tiles[index].adjMines == 0) {
				for (int i = 0; i < tiles[index].adjacentTiles.size(); i++) {
					revealBasics(tiles[index].adjacentTiles[i]->tilenum - 1);
				}
			}
		}
	}
	//Mouse Left click
	GameStatus mouseLeft(Vector2i coords) {
		//Make sure a tile was clicked
		if (findTileIndex(coords) != -1) {
			int temp = findTileIndex(coords);
			if (!tiles[temp].isFlagged()) {
				if (!tiles[temp].revealed) {
					tiles[temp].revealed = true;
					countVic++;
					if (tiles[temp].hasMine()) {
						blowUp();
						return Loss;
					}
					else {
						if (tiles[temp].adjMines == 0) {
							for (int i = 0; i < tiles[temp].adjacentTiles.size(); i++) {
								//Reveal all tiles that are adjacent to this one
								revealBasics(tiles[temp].adjacentTiles[i]->tilenum - 1);
							}
						}
					}
				}
			}
		}
		if (countVic == tiles.size() - numMines) {
			for (int i = 0; i < tiles.size(); i++) {
				if (tiles[i].hasMine()) {
					tiles[i].flag();
				}
			}
			return Victory;
		}
		return gameStat;
	}
	//Mouse Right Click
	int mouseRight(Vector2i coords) {
		//Do stuff for which mine
		if (-1 != findTileIndex(coords)) {
			int temp = findTileIndex(coords);
			if (tiles[temp].isFlagged()) {
				tiles[temp].unFlag();
				return 0;
			}
			else {
				tiles[temp].flag();
				return 1;
			}
		}
		return -1;
	}
	//Draw
	void draw(RenderWindow& window) {
		for (int i = 0; i < tiles.size(); i++) {
			if (paused) {
				tiles[i].tile.setTexture(tileRevText);
				window.draw(tiles[i].tile);
			}
			else {
				if (tiles[i].revealed) {
					//If the tile is revealed, then set the revealed texture and 
					//if not blown, then draw the adjacent numbers
					tiles[i].tile.setTexture(tileRevText);
					window.draw(tiles[i].tile);
					if (tiles[i].hasMine()) {
						tiles[i].Mine.setTexture(mineText);
						window.draw(tiles[i].Mine);
					}
					else if (tiles[i].adjMines != 0) {
						if (tiles[i].adjMines == 1) {
							digitText.loadFromFile("images/number_1.png");
							tiles[i].Digit.setTexture(digitText);
							window.draw(tiles[i].Digit);
						}
						else if (tiles[i].adjMines == 2) {
							digitText.loadFromFile("images/number_2.png");
							tiles[i].Digit.setTexture(digitText);
							window.draw(tiles[i].Digit);
						}
						else if (tiles[i].adjMines == 3) {
							digitText.loadFromFile("images/number_3.png");
							tiles[i].Digit.setTexture(digitText);
							window.draw(tiles[i].Digit);
						}
						else if (tiles[i].adjMines == 4) {
							digitText.loadFromFile("images/number_4.png");
							tiles[i].Digit.setTexture(digitText);
							window.draw(tiles[i].Digit);
						}
						else if (tiles[i].adjMines == 5) {
							digitText.loadFromFile("images/number_5.png");
							tiles[i].Digit.setTexture(digitText);
							window.draw(tiles[i].Digit);
						}
						else if (tiles[i].adjMines == 6) {
							digitText.loadFromFile("images/number_6.png");
							tiles[i].Digit.setTexture(digitText);
							window.draw(tiles[i].Digit);
						}
						else if (tiles[i].adjMines == 7) {
							digitText.loadFromFile("images/number_7.png");
							tiles[i].Digit.setTexture(digitText);
							window.draw(tiles[i].Digit);
						}
						else if (tiles[i].adjMines == 8) {
							digitText.loadFromFile("images/number_8.png");
							tiles[i].Digit.setTexture(digitText);
							window.draw(tiles[i].Digit);
						}
					}
				}
				else {
					tiles[i].tile.setTexture(tileText);
					window.draw(tiles[i].tile);
					if (tiles[i].blown) {
						tiles[i].Mine.setTexture(mineText);
						window.draw(tiles[i].Mine);
					}
					if (tiles[i].isFlagged()) {
						tiles[i].Flag.setTexture(flagText);
						window.draw(tiles[i].Flag);
					}
				}
			}
		}
	}
};