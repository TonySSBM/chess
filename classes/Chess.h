#pragma once
#include "Game.h"
#include "ChessSquare.h"
#include "GameState.h"

//
// the classic game of Chess
//

const int pieceSize = 64;

enum ChessPiece
{
    NoPiece,
    Pawn = 1,
    Knight,
    Bishop,
    Rook,
    Queen,
    King
};

class Chess : public Game
{
public:
    Chess();
    ~Chess();

    struct Move{
        std::string from;
        std::string to;
    };

    // set up the board
    void setUpBoard() override;

    Player* checkForWinner() override;
    bool checkForDraw() override;
    std::string initialStateString() override;
    std::string stateString() override;
    void setStateString(const std::string &s) override;
    bool actionForEmptyHolder(BitHolder &holder) override { return false; }
    bool canBitMoveFrom(GameState* state, Bit&bit, BitHolder &src);
    bool canBitMoveFromTo(GameState* state, Bit& bit, BitHolder& src, BitHolder& dst);
    void bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;
    void stopGame() override;
    BitHolder &getHolderAt(const int x, const int y) override { return currentState._grid[y][x]; }
	void updateAI() override;
    bool gameHasAI() override { return true; }
    void UCIMove(const std::string& move);

    GameState currentState;

    //These functions are here because C++ moment
    bool canBitMoveFrom(Bit&bit, BitHolder &src) override{ return canBitMoveFrom(&currentState, bit, src);};
    bool canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) override{ return canBitMoveFromTo(&currentState, bit, src, dst);};
    void FENtoBoard(const std::string& fenString);

private:
    Bit *PieceForPlayer(const int playerNumber, ChessPiece piece);
    Player* ownerAt(int index ) const;
    void addMoveIfValid(std::string state, std::vector<GameState::Move> &moves, int fromRow, int fromCol, int toRow, int toCol);
    std::string indexToNotation(int row, int col);
    std::string pieceNotation(std::string state, int row, int column) const;
    void generateKnightMoves(std::string state, std::vector<GameState::Move> &moves, int row, int col);
    void generatePawnMoves(std::string state, std::vector<GameState::Move> &moves, int row, int col, char color);
    void generateLinearMoves(std::string state, std::vector<GameState::Move> &moves, int row, int col, const std::vector<std::pair<int, int>> &directions);
    void generateBishopMoves(std::string state, std::vector<GameState::Move> &moves, int row, int col);
    void generateRookMoves(std::string state, std::vector<GameState::Move> &moves, int row, int col);
    void generateQueenMoves(std::string state, std::vector<GameState::Move> &moves, int row, int col);
    void generateKingMoves(std::string state, std::vector<GameState::Move> &moves, int row, int col);
    char oppositeColor(char color);
    void filterOutIllegalMoves(std::string state, std::vector<GameState::Move>& moves, char color);

    std::vector<GameState::Move> generateMoves(std::string state, char color, bool filter);

    //FEN Stuff
    std::vector<std::string> splitString(const std::string &str, char delimiter);
    ChessPiece convertCharToPiece(char c);

    //Evaluation
    int getPieceValue(ChessPiece piece);
    int evaluatePosition(std::string state, char color);
    void countPieces(std::string state);
    int evaluateBoard(std::string state, char color);
    char boardPieceNotation(int row, int column) const;
    int notationToIndex(std::string notation);
    int negamax(std::string state, int depth, int alpha, int beta, char color);

    std::string pieceMove(std::string state, int fromRow, int fromCol, int toRow, int toCol);
    std::string gridString();
    std::string gridStringState();
    std::string dontworryaboutit(std::string notation);
};

