#pragma once
#include "ChessSquare.h"
#include "Chess.h"
#include <iostream>

class GameState
{
public:
	
	struct Move{
        std::string from;
        std::string to;
    };

	std::vector<Move> _moves;
	std::vector<std::string> stateStack;
	ChessSquare _grid[8][8];
    std::string sideToMove = "W";
    std::string castlingAbility = "KQkq";
    std::string enPassantTarget = "-";
    int halfMoveClock = 0;
    int fullMoveClock = 1;
    int totalWhitePieces = 16;
    int totalBlackPieces = 16;
	bool test = false;

	GameState(){};
	~GameState(){};

	GameState(const GameState& other) {
        if (this != &other) {
            sideToMove = other.sideToMove;
			castlingAbility = other.castlingAbility;
			enPassantTarget = other.enPassantTarget;
			halfMoveClock = other.halfMoveClock;
			fullMoveClock = other.fullMoveClock;
			totalWhitePieces = other.totalWhitePieces;
			totalBlackPieces = other.totalBlackPieces;
			test = !other.test;

			std::cout << "Copying game state" << std::endl;

			for(int i = 0; i < 8; i++){
				for(int j = 0; j < 8; j++){
					if (other._grid[i][j].bit() == nullptr) {
						_grid[i][j].setBit(nullptr);
					} else {
						_grid[i][j].setNotation(std::string(1, 'a' + i) + std::string(1, '8' - j));
						//Bit* piece(other._grid[i][j].bit());
						Bit* piece = new Bit();
						// if(other._grid[i][j].bit()->gameTag() > 127){
						// 	piece->setOwner(Chess::getPlayerAt(0));
						// }else{
						// 	piece->setOwner(Chess::getPlayerAt(1));
						// }
						piece->setOwner(other._grid[i][j].bit()->getOwner());
						piece->setGameTag(other._grid[i][j].bit()->gameTag());
						_grid[i][j].setBit(piece);
						//std::cout << "piece: " << piece << std::endl;
						std::cout << "piece: " << _grid[i][j].bit() << std::endl;
					}
				}
			}
        }
    }

	bool operator==(const GameState& other) const {
		std::cout << "Comparing game states" << std::endl;
		if(sideToMove != other.sideToMove) return false;
		if(castlingAbility != other.castlingAbility) return false;
		if(enPassantTarget != other.enPassantTarget) return false;
		if(halfMoveClock != other.halfMoveClock) return false;
		if(fullMoveClock != other.fullMoveClock) return false;
		if(totalWhitePieces != other.totalWhitePieces) return false;
		if(totalBlackPieces != other.totalBlackPieces) return false;
		for(int i = 0; i < 8; i++){
			for(int j = 0; j < 8; j++){
				if(_grid[i][j].bit()->gameTag() != other._grid[i][j].bit()->gameTag()) return false;
			}
		}
		return true;
	}
};