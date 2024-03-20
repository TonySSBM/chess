#include "TicTacToe.h"

TicTacToe::TicTacToe()
{
}

TicTacToe::~TicTacToe()
{
}

//
// make a rock, paper, or scissors piece for the player
//
Bit* TicTacToe::PieceForPlayer(const int playerNumber)
{
    const char *textures[] = { "x.png", "o.png"}; //used the correct textures

    Bit *bit = new Bit();
    // should possibly be cached from player class?
    bit->LoadTextureFromFile(textures[playerNumber]);
    bit->setOwner(getCurrentPlayer());
    bit->setGameTag(playerNumber+1);
    bit->setSize(pieceSize, pieceSize);
    return bit;
}

void TicTacToe::setUpBoard()
{
    srand((unsigned int)time(0));
    setNumberOfPlayers(2);
    // this allows us to draw the board correctly
    _gameOptions.rowX = 9;
    _gameOptions.rowY = 9;
    // setup the board
    for (int y = 0; y < 3; y++) { //3 by 3 grid
        for (int x = 0; x < 3; x++) {
            _grid[y * 3 + x].initHolder(ImVec2(100 * (float)x + 100, 100 * (float)y + 100), "square.png", x, y);
        }
    }
    // if we have an AI set it up
    if (gameHasAI())
    {
        setAIPlayer(_gameOptions.AIPlayer);
    }
    // setup up turns etc.
    startGame();
}

//
// about the only thing we need to actually fill out for rock paper scissors
//
bool TicTacToe::actionForEmptyHolder(BitHolder& holder)
{
    if(checkForDraw() || checkForWinner()) //changed to work with tic tac toe rules
    {
        return false;
    }
    if (holder.bit()) {
        return false;
    }
    Bit *bit = PieceForPlayer(getCurrentPlayer()->playerNumber());
    if (bit) {
        bit->setPosition(holder.getPosition());
        holder.setBit(bit);
        endTurn();
        return true;
    }   
    return false;
}

bool TicTacToe::canBitMoveFrom(Bit& bit, BitHolder& src)
{
    // you can't move anything in rock paper scissors
    return false;
}

bool TicTacToe::canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst)
{
    // you can't move anything in rock paper scissors
    return false;
}

//
// free all the memory used by the game on the heap
//
void TicTacToe::stopGame()
{
    for (int x=0; x<9; x++) { //changed to make sure it works with the 3 by 3 grid
        _grid[x].destroyBit();
    }
}

//
// helper function for the winner check
//
Player* TicTacToe::ownerAt(int index )
{
    if (index < 0 || index > 1) {
        return nullptr;
    }
    if (!_grid[index].bit()) {
        return nullptr;
    }
    return _grid[index].bit()->getOwner();
}

Player* TicTacToe::checkForWinner() //changed to work with tic tac toe rules
{
    int ttt0 = _grid[0].bit() ? _grid[0].bit()->gameTag() : 0;
    int ttt1 = _grid[1].bit() ? _grid[1].bit()->gameTag() : 0;
    int ttt2 = _grid[2].bit() ? _grid[2].bit()->gameTag() : 0;
    int ttt3 = _grid[3].bit() ? _grid[3].bit()->gameTag() : 0;
    int ttt4 = _grid[4].bit() ? _grid[4].bit()->gameTag() : 0;
    int ttt5 = _grid[5].bit() ? _grid[5].bit()->gameTag() : 0;
    int ttt6 = _grid[6].bit() ? _grid[6].bit()->gameTag() : 0;
    int ttt7 = _grid[7].bit() ? _grid[7].bit()->gameTag() : 0;
    int ttt8 = _grid[8].bit() ? _grid[8].bit()->gameTag() : 0;
    
    // X = 1, O = 2

    //checking rows
    if((ttt0 == ttt1 && ttt1 == ttt2 && ttt0 == 1) || (ttt3 == ttt4 && ttt4 == ttt5 && ttt3 == 1) || (ttt6 == ttt7 && ttt7 == ttt8 && ttt6 == 1))
    {
        return _players[0];
    }
    if((ttt0 == ttt1 && ttt1 == ttt2 && ttt0 == 2) || (ttt3 == ttt4 && ttt4 == ttt5 && ttt3 == 2) || (ttt6 == ttt7 && ttt7 == ttt8 && ttt6 == 2))
    {
        return _players[1];
    }

    //checking columns
    if((ttt0 == ttt3 && ttt3 == ttt6 && ttt0 == 1) || (ttt1 == ttt4 && ttt4 == ttt7 && ttt1 == 1) || (ttt2 == ttt5 && ttt5 == ttt8 && ttt2 == 1))
    {
        return _players[0];
    }
    if((ttt0 == ttt3 && ttt3 == ttt6 && ttt0 == 2) || (ttt1 == ttt4 && ttt4 == ttt7 && ttt1 == 2) || (ttt2 == ttt5 && ttt5 == ttt8 && ttt2 == 2))
    {
        return _players[1];
    }

    //checking diagonals
    if((ttt0 == ttt4 && ttt4 == ttt8 && ttt0 == 1) || (ttt2 == ttt4 && ttt4 == ttt6 && ttt2 == 1))
    {
        return _players[0];
    }
    if((ttt0 == ttt4 && ttt4 == ttt8 && ttt0 == 2) || (ttt2 == ttt4 && ttt4 == ttt6 && ttt2 == 2))
    {
        return _players[1];
    }

    // no winner
    return nullptr;
}

bool TicTacToe::checkForDraw() //changed to work with tic tac toe rules
{
    int ttt0 = _grid[0].bit() ? _grid[0].bit()->gameTag() : 0;
    int ttt1 = _grid[1].bit() ? _grid[1].bit()->gameTag() : 0;
    int ttt2 = _grid[2].bit() ? _grid[2].bit()->gameTag() : 0;
    int ttt3 = _grid[3].bit() ? _grid[3].bit()->gameTag() : 0;
    int ttt4 = _grid[4].bit() ? _grid[4].bit()->gameTag() : 0;
    int ttt5 = _grid[5].bit() ? _grid[5].bit()->gameTag() : 0;
    int ttt6 = _grid[6].bit() ? _grid[6].bit()->gameTag() : 0;
    int ttt7 = _grid[7].bit() ? _grid[7].bit()->gameTag() : 0;
    int ttt8 = _grid[8].bit() ? _grid[8].bit()->gameTag() : 0;
    
    // X = 1, O = 2

    //checking if not all spaces are filled
    if(ttt0 == 0 || ttt1 == 0 || ttt2 == 0 || ttt3 == 0 || ttt4 == 0 || ttt5 == 0 || ttt6 == 0 || ttt7 == 0 || ttt8 == 0)
    {
        return false;
    }

    //checking rows
    if((ttt0 == ttt1 && ttt1 == ttt2 && ttt0 != 0) || (ttt3 == ttt4 && ttt4 == ttt5 && ttt3 != 0) || (ttt6 == ttt7 && ttt7 == ttt8 && ttt6 != 0))
    {
        return false;
    }

    //checking columns
    if((ttt0 == ttt3 && ttt3 == ttt6 && ttt0 != 0) || (ttt1 == ttt4 && ttt4 == ttt7 && ttt1 != 0) || (ttt2 == ttt5 && ttt5 == ttt8 && ttt2 != 0))
    {
        return false;
    }

    //checking diagonals
    if((ttt0 == ttt4 && ttt4 == ttt8 && ttt0 != 0) || (ttt2 == ttt4 && ttt4 == ttt6 && ttt2 != 0))
    {
        return false;
    }

    // draw
    return true;
}

//
// state strings
//
std::string TicTacToe::initialStateString()
{
    return "00";
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string TicTacToe::stateString() //correctly displays the state of the board
{
    std::string s;
    for (int x=0; x<9; x++) {
        Bit *bit = _grid[x].bit();
        if (bit) {
            s += std::to_string(bit->gameTag());
        } else {
            s += "0";
        }
    }
    return s;
}

//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void TicTacToe::setStateString(const std::string &s)
{
    for (int x=0; x<9; x++) {
        _grid[x].setBit( PieceForPlayer(0) );
    }
}

/*----------------AI CODE----------------*/

int TicTacToeAI::negamax(TicTacToeAI* state, int depth, int playerColor){
    int score = state->evaluateBoard();

    if(score == 10) return score-depth;
    if(state->isBoardFull()) return 0;

    int bestVal = -1000;
    for (int y = 0; y < 3; y++){
        for (int x = 0; x < 3; x++){
            if(state->_grid[y][x]){
                state->_grid[y][x] = (playerColor == -1) ? (HUMAN_PLAYER + 1) : (AI_PLAYER + 1);
                bestVal = std::max(bestVal, -negamax(state, depth+1, -playerColor));
                std::cout << "looping" << std::endl;
                state->_grid[y][x] = 0;
            }
        }
    }
    return bestVal;
}

void TicTacToe::updateAI(){
    int totalMoves = 0;
    for (int x=0; x<9; x++) {
        if (_grid[x].bit()) {
            totalMoves++;
        }
    }
    
    if(totalMoves % 2 != 1){
        return;
    }

    int bestVal = -1000;
    Square *bestMove = nullptr;

    for (int y = 0; y < 3; y++){
        for(int x = 0; x < 3; x++){
            if(!_grid[y * 3 + x].bit()){
                _grid[y * 3 + x].setBit(PieceForPlayer(AI_PLAYER));
                TicTacToeAI* newState = this->clone();
                int moveVal = -newState->negamax(newState, 0, 1);
                delete newState;

                _grid[y * 3 + x].setBit(nullptr);

                if(moveVal > bestVal){
                    bestMove = &_grid[y * 3 + x];
                    bestVal = moveVal;
                }
            }
        }
    }

    if(bestMove){
        if(actionForEmptyHolder(*bestMove)){
            endTurn();
        }
        endTurn();
    }
}


TicTacToeAI* TicTacToe::clone(){
    TicTacToeAI* newGame = new TicTacToeAI();
    std::string gameState = stateString();
    for(int y = 0; y < 3; y++){
        for(int x = 0; x < 3; x++){
            int index = y * 3 + x;
            int playerNumber = gameState[index] - '0';
            newGame->_grid[y][x] = playerNumber;
        }
    }
    return newGame;
}

int TicTacToeAI::AICheckForWinner(){
    static const int winStates[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8},
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8},
        {0, 4, 8}, {2, 4, 6}
    };

    for(int i = 0; i < 8; i++){
        const int* triple = winStates[i];
        int playerInt = ownerAt(triple[0]);
        if(playerInt != 0 && playerInt == ownerAt(triple[1]) && playerInt == ownerAt(triple[2])){
            std::cout << "WOAH" << std::endl;
            return playerInt;
        }
    }
    return 0;
}

int TicTacToeAI::evaluateBoard(){
    int winner = AICheckForWinner();
    if(winner == AI_PLAYER){
        return 10;
    }
    if(winner == HUMAN_PLAYER){
        return -10;
    }
    return 0;
}

bool TicTacToeAI::isBoardFull() const{
    for(int y = 0; y < 3; y++){
        for(int x = 0; x < 3; x++){
            if(_grid[y][x] == 0){
                return false;
            }
        }
    }
    return true;
}

int TicTacToeAI::ownerAt(int index) const{
    int y = index / 3;
    int x = index % 3;
    return _grid[y][x];
}
