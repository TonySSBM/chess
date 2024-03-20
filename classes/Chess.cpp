#include "Chess.h"
#include "Evaluate.h"
#include <map>

std::string Chess::pieceNotation(std::string state, int row, int column) const
{
    return std::to_string(state.at(row * 8 + column));
}

Chess::Chess()
{
}  

Chess::~Chess()
{
}

Bit* Chess::PieceForPlayer(const int playerNumber, ChessPiece piece)
{
    const char *pieces[] = {"pawn.png", "knight.png", "bishop.png", "rook.png", "queen.png", "king.png"};

    Bit *bit = new Bit();
    const char *pieceName = pieces[piece-1];
    std::string spritePath = std::string("chess/") + (playerNumber == 0 ? "b_": "w_") + pieceName;
    bit->LoadTextureFromFile(spritePath.c_str());
    bit->setOwner(getPlayerAt(playerNumber));
    bit->setSize(pieceSize, pieceSize);

    return bit;
}

void Chess::setUpBoard()
{
    const ChessPiece initialBoard[2][8] = {
        {Rook, Knight, Bishop, Queen, King, Bishop, Knight, Rook},
        {Pawn, Pawn, Pawn, Pawn, Pawn, Pawn, Pawn, Pawn}
    };

    setNumberOfPlayers(2);
    _gameOptions.rowX = 8;
    _gameOptions.rowY = 8;

    currentState = GameState();

    for (int y = 0; y < _gameOptions.rowY; y++){
        for (int x = 0; x < _gameOptions.rowX; x++){
            ImVec2 position((float)(pieceSize * x + pieceSize), (float)(pieceSize * y + pieceSize));
            currentState._grid[y][x].initHolder(position, "boardsquare.png", x, y);
            currentState._grid[y][x].setGameTag(0);
            currentState._grid[y][x].setNotation(indexToNotation(y, x));
        }
    }

    FENtoBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");   //starting position

    //FENtoBoard("k2qq3/8/8/8/8/8/4R3/4K3"); //check stuff
    //FENtoBoard("k2qq3/8/8/8/8/8/8/4K2R w KQkq - 0 1"); //castling in check
    //FENtoBoard("k2q1q2/8/8/8/8/8/8/4K2R w KQkq - 0 1"); //castling through check
    //FENtoBoard("k5q1/8/8/8/8/8/8/4K2R w KQkq - 0 1"); //castling into check
    //FENtoBoard("7k/8/8/8/8/5Q2/6Q1/K7"); //white checkmates black
    //FENtoBoard("1q5k/2q5/8/8/8/8/8/K7 b ---- - 0 2"); //black checkmates white
    //FENtoBoard("7k/8/8/5Q2/8/8/8/K7 w ---- - 0 1"); //stalemate 1
    //FENtoBoard("7k/8/8/8/2q5/8/8/K7 b ---- - 0 2"); //stalemate 2

    //FENtoBoard("3k4/4p3/8/8/8/8/4P3/3K4");
    //FENtoBoard("5k2/8/8/8/8/8/8/4K2R w K--- - 0 1");                           // white can castle
    //FENtoBoard("3k4/8/8/8/8/8/8/R3K3 w -Q-- ---- 0 1");                           // white can castle queen side
    //FENtoBoard("r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1");                // white can castle both sides
    //FENtoBoard("2K2r2/4P3/8/8/8/8/8/3k4 w ---- - 0 1");                        // white can promote to queen
    //FENtoBoard("4k3/1P6/8/8/8/8/K7/8 w ---- - 0 1");                           // white can promote to queen

    if (gameHasAI()){
        setAIPlayer(1);
    }

    if(currentState.sideToMove.at(0) == 'B' || currentState.sideToMove.at(0) == 'b'){
        currentState._moves = generateMoves(gridStringState(), ((currentState.fullMoveClock - 1)&1) ? 'B' : 'W', true);
    }else{
        _gameOptions.currentTurnNo = 0;
        currentState._moves = generateMoves(gridStringState(), toupper(currentState.sideToMove.at(0)), true);
    }

    startGame();
}

std::vector<std::string> Chess::splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void Chess::FENtoBoard(const std::string& fenString)
{
    // Split the FEN string into its components

    std::vector<std::string> fenParts = splitString(fenString, ' ');

    // Extract the board position from the first part
    std::string boardPosition = fenParts[0];

    // Extract additional FEN components

    if(fenParts.size() > 1){
        currentState.sideToMove = fenParts[1][0]; // First character indicates the side to move
        currentState.castlingAbility = fenParts[2]; // Second part indicates castling availability
        currentState.enPassantTarget = fenParts[3]; // Third part indicates en passant target square
        currentState.halfMoveClock = std::stoi(fenParts[4]); // Fourth part indicates half move clock
        currentState.fullMoveClock = std::stoi(fenParts[5]); // Fifth part indicates full move clock

        if(currentState.stateStack.size() == 0){
            _gameOptions.currentTurnNo = currentState.fullMoveClock;
        }
    }

    // Loop through each row of the board position
    int row = 0;
    int col = 0;
    for (char c : boardPosition)
    {
        if (c == '/')
        {
            row++;
            col = 0;
        }
        else if (isdigit(c))
        {
            int emptySquares = c - '0';
            for (int i = 0; i < emptySquares; i++)
            {
                currentState._grid[row][col].setBit(nullptr);
                col++;
            }
        }
        else
        {
            // Determine piece color and type
            char pieceColor = islower(c) ? 'W' : 'B';
            ChessPiece pieceType = convertCharToPiece(c);

            // Create and place the piece on the board
            Bit* piece = PieceForPlayer(pieceColor == 'W' ? 0 : 1, pieceType);
            
            piece->setPosition(currentState._grid[row][col].getPosition());
            piece->setParent(&(currentState._grid[row][col]));
            piece->setGameTag(pieceColor == 'B' ? pieceType + 128 : pieceType);
            currentState._grid[row][col].setBit(piece);

            col++;
        }
    }
}


ChessPiece Chess::convertCharToPiece(char c) {
    switch (toupper(c)) {
        case 'P': return Pawn;
        case 'N': return Knight;
        case 'B': return Bishop;
        case 'R': return Rook;
        case 'Q': return Queen;
        case 'K': return King;
        default: return NoPiece;
    }
}

bool Chess::canBitMoveFrom(GameState* state, Bit &bit, BitHolder &src)
{
    for(int y = 0; y < _gameOptions.rowY; y++){
        for(int x = 0; x < _gameOptions.rowX; x++){
            ChessSquare &square = currentState._grid[y][x];
            square.setMoveHighlighted(false);
        }
    }
    
    ChessSquare &srcSquare = static_cast<ChessSquare&>(src);
    for (auto move : (*state)._moves){
        if (move.from == srcSquare.getNotation()){
            for(int y = 0; y < _gameOptions.rowY; y++){
                for(int x = 0; x < _gameOptions.rowX; x++){
                    //move highlighted if can bit move from to, and if the piece is not the same color
                    currentState._grid[y][x].setMoveHighlighted(canBitMoveFromTo(state, bit, src, currentState._grid[y][x]) && (currentState._grid[y][x].bit() == nullptr || currentState._grid[y][x].bit()->getOwner() != bit.getOwner()));
                }
            }
            return true;
        }
    }
    return false;
}

bool Chess::canBitMoveFromTo(GameState *state, Bit &bit, BitHolder &src, BitHolder &dst)
{
    ChessSquare &srcSquare = static_cast<ChessSquare&>(src);
    ChessSquare &dstSquare = static_cast<ChessSquare&>(dst);
    for (auto move : (*state)._moves){
        if (move.from == srcSquare.getNotation() && move.to == dstSquare.getNotation()){
            return true;
        }
    }
    return false;
}

void Chess::bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst)
{
    // Check for pawn promotion
    ChessSquare &srcSquare = static_cast<ChessSquare&>(src);
    ChessSquare &dstSquare = static_cast<ChessSquare&>(dst);

    for(int y = 0; y < _gameOptions.rowY; y++){
        for(int x = 0; x < _gameOptions.rowX; x++){
            ChessSquare &square = currentState._grid[y][x];
            square.setMoveHighlighted(false);
        }
    }

    //Promotion to Queen
    if ((bit.gameTag() == Pawn || bit.gameTag() == (Pawn + 128)) && (dstSquare.getRow() == 0 || dstSquare.getRow() == 7)){
        bit.setGameTag(bit.gameTag() + 4);
        if(bit.getOwner() == getPlayerAt(1)){
            bit.LoadTextureFromFile("chess/w_queen.png");
        }else{
            bit.LoadTextureFromFile("chess/b_queen.png");
        }
        bit.setSize(pieceSize, pieceSize);
    }

    // En passant capture
    if(dstSquare.getNotation() == currentState.enPassantTarget){
        if (currentState.sideToMove == "w" || currentState.sideToMove == "W"){
            currentState._grid[dstSquare.getRow() + 1][dstSquare.getColumn()].destroyBit();
        }else if (currentState.sideToMove == "b" || currentState.sideToMove == "B"){
            currentState._grid[dstSquare.getRow() - 1][dstSquare.getColumn()].destroyBit();
        }
    }

    //Setting En passant target
    currentState.enPassantTarget = "-";
    if ((bit.gameTag() == Pawn && (srcSquare.getRow() == 1 && dstSquare.getRow() == 3)) || (bit.gameTag() == (Pawn + 128) && (srcSquare.getRow() == 6 && dstSquare.getRow() == 4))){
        std::string rows[] = {"a", "b", "c", "d", "e", "f", "g", "h"};
        if(dstSquare.getRow() == 3){
            currentState.enPassantTarget = rows[dstSquare.getColumn()] + "6";
        }
        else if(dstSquare.getRow() == 4){
            currentState.enPassantTarget = rows[dstSquare.getColumn()] + "3";
        }
    }

    // Castling target
    if (bit.gameTag() == (King + 128)){
        currentState.castlingAbility[0] = '-';
        currentState.castlingAbility[1] = '-';
    }
    else if(bit.gameTag() == King){
        currentState.castlingAbility[2] = '-';
        currentState.castlingAbility[3] = '-';
    }
    else if(bit.gameTag() == (Rook + 128) && srcSquare.getNotation() == "a1"){
        currentState.castlingAbility[1] = '-';
    }
    else if(bit.gameTag() == (Rook + 128) && srcSquare.getNotation() == "h1"){
        currentState.castlingAbility[0] = '-';
    }
    else if(bit.gameTag() == Rook && srcSquare.getNotation() == "a8"){
        currentState.castlingAbility[3] = '-';
    }
    else if(bit.gameTag() == Rook && srcSquare.getNotation() == "h8"){
        currentState.castlingAbility[2] = '-';
    }

    // Move castling rook
    if (bit.gameTag() == (King + 128) && srcSquare.getNotation() == "e1" && dstSquare.getNotation() == "g1"){
        currentState._grid[7][5].dropBitAtPoint(currentState._grid[7][7].bit(), currentState._grid[7][5].getPosition());
    }
    else if (bit.gameTag() == (King + 128) && srcSquare.getNotation() == "e1" && dstSquare.getNotation() == "c1"){
        currentState._grid[7][3].dropBitAtPoint(currentState._grid[7][0].bit(), currentState._grid[7][3].getPosition());
    }
    else if (bit.gameTag() == King && srcSquare.getNotation() == "e8" && dstSquare.getNotation() == "g8"){
        currentState._grid[0][5].dropBitAtPoint(currentState._grid[0][7].bit(), currentState._grid[0][5].getPosition());
    }
    else if (bit.gameTag() == King && srcSquare.getNotation() == "e8" && dstSquare.getNotation() == "c8"){
        currentState._grid[0][3].dropBitAtPoint(currentState._grid[0][0].bit(), currentState._grid[0][3].getPosition());
    }

    if (currentState.sideToMove == "w" || currentState.sideToMove == "W"){
        currentState.sideToMove = "B";
    }else if (currentState.sideToMove == "b" || currentState.sideToMove == "B"){
        currentState.sideToMove = "W";
    }else{
        std::cerr << "Invalid side to move: " << currentState.sideToMove << std::endl;
    }

    currentState.halfMoveClock += 1;
    currentState.fullMoveClock += 1;

    int currentWhitePieces = currentState.totalWhitePieces;
    int currentBlackPieces = currentState.totalBlackPieces;
    countPieces(gridStringState());
    if(currentWhitePieces > currentState.totalWhitePieces || currentBlackPieces > currentState.totalBlackPieces){
        currentState.halfMoveClock = 0;
    }
    currentState._moves.clear();
    _gameOptions.currentTurnNo++;
    currentState._moves = generateMoves(gridStringState(), ((currentState.fullMoveClock - 1)&1) ? 'B' : 'W', true);
}

std::string Chess::pieceMove(std::string state, int fromRow, int fromCol, int toRow, int toCol)
{
    std::string newState = state;

    //Promotion to Queen
    if (state[fromRow * 8 + fromCol] == 'P' && toRow == 0){
        newState[fromRow * 8 + fromCol] = 'Q';
    }
    else if (state[fromRow * 8 + fromCol] == 'p' && toRow == 7){
        newState[fromRow * 8 + fromCol] = 'q';
    }

    // En passant capture
    if (state[fromRow * 8 + fromCol] == 'P' && fromRow == 1 && toRow == 3){
        newState[(fromRow + 1) * 8 + fromCol] = '-';
    }
    else if (state[fromRow * 8 + fromCol] == 'p' && fromRow == 6 && toRow == 4){
        newState[(fromRow - 1) * 8 + fromCol] = '-';
    }

    // Castling target
    if (state[fromRow * 8 + fromCol] == 'K'){
        newState[4 * 8 + 7] = '-';
        newState[6 * 8 + 7] = 'R';
    }
    else if (state[fromRow * 8 + fromCol] == 'K'){
        newState[4 * 8 + 0] = '-';
        newState[2 * 8 + 0] = 'r';
    }
    else if (state[fromRow * 8 + fromCol] == 'k'){
        newState[4 * 8 + 7] = '-';
        newState[6 * 8 + 7] = 'r';
    }
    else if (state[fromRow * 8 + fromCol] == 'k'){
        newState[4 * 8 + 0] = '-';
        newState[2 * 8 + 0] = 'r';
    }

    return newState;
}

int Chess::getPieceValue(ChessPiece piece)
{
    switch (piece){
        case Pawn: return 1;
        case Knight: return 3;
        case Bishop: return 3;
        case Rook: return 5;
        case Queen: return 9;
        case King: return 100;
        default: return 0;
    }
}

int Chess::evaluatePosition(std::string state, char color){
    int score = 0;
    for (int row = 0; row < 8; row++){
        for (int col = 0; col < 8; col++){
            char piece = state[row*8+col];
            if ((isupper(piece) && color == 'W') || (islower(piece) && color == 'B')){
                score += getPieceValue(convertCharToPiece(piece));
            }
            else if ((isupper(piece) && color == 'B') || (islower(piece) && color == 'W')){
                score -= getPieceValue(convertCharToPiece(piece));
            }
        }
    }
    return score;
}

void Chess::countPieces(std::string state){
    currentState.totalWhitePieces = 0;
    currentState.totalBlackPieces = 0;
    for (int row = 0; row < 8; row++){
        for (int col = 0; col < 8; col++){
            char piece = state[row * 8 + col];
            if(piece == '0' || piece == '-'){
                continue;
            }
            else if (isupper(piece)){
                currentState.totalWhitePieces++;
            }
            else if (islower(piece)){
                currentState.totalBlackPieces++;
            }
        }
    }
}

void Chess::stopGame()
{
    for (int y = 0; y < _gameOptions.rowY; y++){
        for (int x = 0; x < _gameOptions.rowX; x++){
            currentState._grid[y][x].destroyBit();
        }
    }

    FENtoBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    while(currentState.stateStack.size() > 0){
        currentState.stateStack.pop_back();
    }
}

Player* Chess::checkForWinner()
{
    std::vector<GameState::Move> moves = generateMoves(gridStringState(), toupper(currentState.sideToMove.at(0)), true);
    
    if(moves.size() == 0){
        int kingSquare = -1;
        for (int i=0; i<64; i++) {
            if (gridStringState()[i] == 'k' && currentState.sideToMove.at(0) == 'B' ) { kingSquare = i; break; }
            if (gridStringState()[i] == 'K' && currentState.sideToMove.at(0) == 'W' ) { kingSquare = i; break; }
        }

        auto oppositeMoves = generateMoves(gridStringState(), oppositeColor(currentState.sideToMove.at(0)), false);
        for (auto enemyMoves : oppositeMoves) {
            int enemyDst = notationToIndex(enemyMoves.to);
            if (enemyDst == notationToIndex(indexToNotation(kingSquare / 8, kingSquare % 8))){
                if ((currentState.sideToMove == "w" || currentState.sideToMove == "W")){
                    return getPlayerAt(1);
                }else if ((currentState.sideToMove == "b" || currentState.sideToMove == "B")){
                    return getPlayerAt(0);
                }
            }
        }
    }
    return nullptr;
}

bool Chess::checkForDraw()
{
    if (currentState.halfMoveClock >= 50){
        return true;
    }

    std::vector<GameState::Move> moves = generateMoves(gridStringState(), toupper(currentState.sideToMove.at(0)), true);
    if (moves.size() == 0){
        int kingSquare = -1;
        for (int i=0; i<64; i++) {
            if (gridStringState()[i] == 'k' && currentState.sideToMove.at(0) == 'B' ) { kingSquare = i; break; }
            if (gridStringState()[i] == 'K' && currentState.sideToMove.at(0) == 'W' ) { kingSquare = i; break; }
        }

        auto oppositeMoves = generateMoves(gridStringState(), oppositeColor(currentState.sideToMove.at(0)), false);
        for (auto enemyMoves : oppositeMoves) {
            int enemyDst = notationToIndex(enemyMoves.to);
            if (enemyDst == notationToIndex(indexToNotation(kingSquare / 8, kingSquare % 8))){
                return false;
            }
        }
    }else{
        return false;
    }

    return true;
}

std::string Chess::initialStateString()
{
    return stateString();
}

std::string Chess::stateString()
{
    std::string s = "";
    
    s += "\n" + gridString() + "\n";

    for (int i = 0; i < currentState.sideToMove.length(); i++) 
		if (currentState.sideToMove[i] >= 'a' and currentState.sideToMove[i] <= 'z') 
			currentState.sideToMove[i] = currentState.sideToMove[i] & (~(1<<5)); 

    s += "Side To Move: " + currentState.sideToMove + "\n";
    s += "Castling Ability: " + currentState.castlingAbility + "\n";
    s += "En Passant Target: " + currentState.enPassantTarget + "\n";
    s += "Half Move Clock: " + std::to_string(currentState.halfMoveClock) + "\n";
    s += "Full Move Clock: " + std::to_string(currentState.fullMoveClock) + "\n";
    s += "Material Score for White: " + std::to_string(evaluatePosition(gridStringState(), 'W')) + "\n";
    s += "Material Score for Black: " + std::to_string(evaluatePosition(gridStringState(), 'B')) + "\n";
    s += "Board Score for White: " + std::to_string(evaluateBoard(gridStringState(), 'W')) + "\n";
    s += "Board Score for Black: " + std::to_string(evaluateBoard(gridStringState(), 'B')) + "\n";
    s += "Total White Pieces: " + std::to_string(currentState.totalWhitePieces) + "\n";
    s += "Total Black Pieces: " + std::to_string(currentState.totalBlackPieces) + "\n";
    return s;
}

std::string Chess::gridString(){
    std::string gridString = "";
    for (int y = 0; y < _gameOptions.rowY; y++){
        for (int x = 0; x < _gameOptions.rowX; x++){
            gridString += std::string(1, (boardPieceNotation(y, x))) + " ";
        }
        gridString += "\n";
    }
    return gridString;
}

std::string Chess::gridStringState(){
    std::string gridString = "";
    for (int y = 0; y < _gameOptions.rowY; y++){
        for (int x = 0; x < _gameOptions.rowX; x++){
            gridString += std::string(1, (boardPieceNotation(y, x)));
        }
    }
    return gridString;
}

void Chess::setStateString(const std::string &s)
{
    for (int y = 0; y<_gameOptions.rowY; y++){
        for (int x = 0; x<_gameOptions.rowX; x++){
            int index = y * _gameOptions.rowX + x;
            int playerNumber = s[index] - '0';
            if (playerNumber) {
                currentState._grid[y][x].setBit(PieceForPlayer(playerNumber-1, Pawn));
            }else{
                currentState._grid[y][x].setBit(nullptr);
            }
        }
    }
}

void Chess::addMoveIfValid(std::string state, std::vector<GameState::Move> &moves, int fromRow, int fromCol, int toRow, int toCol)
{
    if (toRow >= 0 && toRow < 8 && toCol >= 0 && toCol < 8){
        if (state[toRow * 8 + toCol] == '0' || (state[toRow * 8 + toCol] <= 91 && state[fromRow * 8 + fromCol] >= 97) || (state[toRow * 8 + toCol] >= 97 && state[fromRow * 8 + fromCol] <= 91)){
            moves.push_back({indexToNotation(fromRow, fromCol), indexToNotation(toRow, toCol)});
        }
    }
}

std::string Chess::indexToNotation(int row, int col)
{
    return std::string(1, 'a' + col) + std::string(1, '8' - row);
}

void Chess::generateKnightMoves(std::string state, std::vector<GameState::Move> &moves, int row, int col)
{
    static const int movesRow[] = {2, 1, -1, -2, -2, -1, 1, 2};
    static const int movesCol[] = {1, 2, 2, 1, -1, -2, -2, -1};

    for (int i = 0; i < 8; i++){
        int newRow = row + movesRow[i];
        int newCol = col + movesCol[i];
        addMoveIfValid(state, moves, row, col, newRow, newCol);
    }
}

void Chess::generatePawnMoves(std::string state, std::vector<GameState::Move> &moves, int row, int col, char color)
{
    int direction = (color == 'W') ? -1 : 1;
    int startRow = (color == 'W') ? 6 : 1;

    if ((row + direction) * 8 + col < 64 && ((row + direction) * 8) + col >= 0 && state[(row + direction) * 8 + col] == '0'){
        addMoveIfValid(state, moves, row, col, row + direction, col);
        
        if((row + (2 * direction)) * 8 + col < 64 && (row + (2 * direction)) * 8 + col >= 0 && row == startRow && state[(row + (2 * direction)) * 8 + col] == '0'){
            addMoveIfValid(state, moves, row, col, row + 2 * direction, col);
        }
    }

    for (int i = -1; i <= 1; i += 2){
        if (col + i >= 0 && col + i < 8){
            if(i != 0 && state[(row + direction) * 8 + col + i] != '0' && ((state[(row + direction) * 8 + col + i] <= 91 && color == 'B') || (state[(row + direction) * 8 + col + i] >= 97 && color == 'W'))){
                addMoveIfValid(state, moves, row, col, row + direction, col + i);
            }

            // en passant movement
            else if ((row + direction) * 8 + (col + i) < 64 && (row + direction) * 8 + (col + i) >= 0 && state[(row + direction) * 8 + (col + i)] == '0' && currentState.enPassantTarget != "-" && currentState.enPassantTarget == indexToNotation(row + direction, col + i)){
                addMoveIfValid(state, moves, row, col, row + direction, col + i);
            }
        }
    }
}

void Chess::generateLinearMoves(std::string state, std::vector<GameState::Move> &moves, int row, int col, const std::vector<std::pair<int, int>> &directions)
{
    for (auto& dir : directions){
        int currentRow = row + dir.first;
        int currentCol = col + dir.second;
        while (currentRow >= 0 && currentRow < 8 && currentCol >= 0 && currentCol < 8){
            if (state[currentRow * 8 + currentCol] != '0'){
                addMoveIfValid(state, moves, row, col, currentRow, currentCol);
                break;
            }
            addMoveIfValid(state, moves, row, col, currentRow, currentCol);
            currentRow += dir.first;
            currentCol += dir.second;
        }
    }
}

void Chess::generateBishopMoves(std::string state, std::vector<GameState::Move> &moves, int row, int col)
{
    std::vector<std::pair<int, int>> directions = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    generateLinearMoves(state, moves, row, col, directions);
}

void Chess::generateRookMoves(std::string state, std::vector<GameState::Move> &moves, int row, int col)
{
    std::vector<std::pair<int, int>> directions = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
    generateLinearMoves(state, moves, row, col, directions);
}

void Chess::generateQueenMoves(std::string state, std::vector<GameState::Move> &moves, int row, int col)
{
    generateBishopMoves(state, moves, row, col);
    generateRookMoves(state, moves, row, col);
}

void Chess::generateKingMoves(std::string state, std::vector<GameState::Move> &moves, int row, int col)
{
    static const std::vector<std::pair<int, int>> directions = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };
    for (auto dir : directions){
        int newRow = row + dir.first;
        int newCol = col + dir.second;
        if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8){
            addMoveIfValid(state, moves, row, col, newRow, newCol);
        }
    }

    // Castling
    if (currentState.sideToMove == "w" || currentState.sideToMove == "W"){
        if (currentState.castlingAbility.find('K') != std::string::npos){
            if (state[7 * 8 + 5] == '0' && state[7 * 8 + 6] == '0'){
                if (state[7 * 8 + 7] == 'R'){
                    addMoveIfValid(state, moves, 7, 4, 7, 6);
                }
            }
        }
        if (currentState.castlingAbility.find('Q') != std::string::npos){
            if (state[7 * 8 + 1] == '0' && state[7 * 8 + 2] == '0' && state[7 * 8 + 3] == '0'){
                if (state[7 * 8 + 0] == 'R'){
                    addMoveIfValid(state, moves, 7, 4, 7, 2);
                }
            }
        }
    }
    else if (currentState.sideToMove == "b" || currentState.sideToMove == "B"){
        if (currentState.castlingAbility.find('k') != std::string::npos){
            if (state[0 * 8 + 5] == '0' && state[0 * 8 + 6] == '0'){
                if (state[7 * 0 + 7] == 'r'){
                    addMoveIfValid(state, moves, 0, 4, 0, 6);
                }
            }
        }
        if (currentState.castlingAbility.find('q') != std::string::npos){
            if (state[0 * 8 + 1] == '0' && state[0 * 8 + 2] == '0' && state[0 * 8 + 3] == '0'){
                if (state[7 * 0 + 0] == 'r'){
                    addMoveIfValid(state, moves, 0, 4, 0, 2);
                }
            }
        }
    }
}

char Chess::oppositeColor(char color)
{
    return (color == 'W') ? 'B' : 'W';
}

std::vector<GameState::Move> Chess::generateMoves(std::string state, char color, bool filter)
{

    if(gridStringState() == state && !currentState._moves.empty() && filter){
        return currentState._moves;
    }

    std::vector<GameState::Move> moves;

    for (int row = 0; row < 8; ++row){
        for (int col = 0; col < 8; ++col){
            std::string piece = std::string(1, state[row * 8 + col]);
            if(!piece.empty() && piece != "-" && piece != "0" && ((isupper(piece[0]) && color == 'W') || (islower(piece[0]) && color == 'B'))){
                switch (piece[0]){
                    case 'N':
                        generateKnightMoves(state, moves, row, col);
                        break;
                    case 'n':
                        generateKnightMoves(state, moves, row, col);
                        break;
                    case 'P':
                        generatePawnMoves(state, moves, row, col, color);
                        break;
                    case 'p':
                        generatePawnMoves(state, moves, row, col, color);
                        break;
                    case 'B':   
                        generateBishopMoves(state, moves, row, col);
                        break;
                    case 'b':
                        generateBishopMoves(state, moves, row, col);
                        break;
                    case 'R':
                        generateRookMoves(state, moves, row, col);
                        break;
                    case 'r':
                        generateRookMoves(state, moves, row, col);
                        break;
                    case 'Q':
                        generateQueenMoves(state, moves, row, col);
                        break;
                    case 'q':
                        generateQueenMoves(state, moves, row, col);
                        break;
                    case 'K':
                        generateKingMoves(state, moves, row, col);
                        break;
                    case 'k':
                        generateKingMoves(state, moves, row, col);
                        break;
                }
            }
        }
    }

    if(filter){
        filterOutIllegalMoves(state, moves, color);
    }

    return moves;
}

static std::map<char, int> evaluateScores = {
        {'P', 100}, {'p', -100},
        {'N', 200}, {'n', -200},
        {'B', 230}, {'b', -230},
        {'R', 400}, {'r', -400},
        {'Q', 900}, {'q', -900},
        {'K', 2000}, {'k', -2000},
        {'0', 0}
};

char Chess::boardPieceNotation(int row, int column) const
{
    const char *wpieces = { "0PNBRQK" };
    const char *bpieces = { "0pnbrqk" };
    Bit *bit = currentState._grid[row][column].bit();

    char notation = '0';
    if (bit) {
        notation = bit->gameTag() < 128 ? bpieces[bit->gameTag()] : wpieces[bit->gameTag()-128];
    }
    return notation;
}

int Chess::evaluateBoard(std::string state, char color)
{
    int score = 0;

    std::string copyState = state;

    for (int i=0; i<64; i++) {
        score += evaluateScores[copyState[i]];
    }

    for (int i=0; i<64; i++) {
        char piece = copyState[i];
        //int j = FLIP(i);
        int j = i;
        switch (piece) {
            case 'N': // Knight
                score += knightTable[j];
                break;
            case 'n':
                score -= knightTable[FLIP(j)];
                break;
            case 'P': // Pawn
                score += pawnTable[j];
                break;
            case 'p':
                score -= pawnTable[FLIP(j)];
                break;
            case 'K': // King
                score += kingTable[j];
                break;
            case 'k':
                score -= kingTable[FLIP(j)];
                break;
            case 'R': // Rook
                score += rookTable[j];
                break;
            case 'r':
                score -= rookTable[FLIP(j)];
                break;
            case 'Q': // Queen
                score += queenTable[j];
                break;
            case 'q':
                score -= queenTable[FLIP(j)];
                break;
        }
    }
    
    if(color == 'B'){
        score = -score;
    }
    
    return score;
}

void Chess::updateAI(){
    const int myInfinity = 99999999;
    int bestMoveScore = -myInfinity; // Min value
    GameState::Move bestMove;
    std::string copyState = gridStringState();

    currentState._moves = generateMoves(copyState, currentState.sideToMove.at(0), true);

    if(currentState._moves.empty()){
        return;
    }

    for(auto move : currentState._moves) {
        auto state = copyState;
		int srcSquare = 63 - notationToIndex(dontworryaboutit(move.from));
		int dstSquare = 63 - notationToIndex(dontworryaboutit(move.to));
		state[dstSquare] = state[srcSquare];
		state[srcSquare] = '0';
		int bestMoveValue = -negamax(state, 3, -99'999, 99'999, 1);
		if (bestMoveValue > bestMoveScore) {
			bestMoveScore = bestMoveValue;
			bestMove = move;
		}
    }

    if (bestMoveScore != -9999999) {
        std::cout << "Best move: " << bestMove.from << " " << bestMove.to << std::endl;
        int srcSquare = 63 - notationToIndex(dontworryaboutit(bestMove.from));
        int dstSquare = 63 - notationToIndex(dontworryaboutit(bestMove.to));
        BitHolder& src = getHolderAt(srcSquare&7, srcSquare/8);
        BitHolder& dst = getHolderAt(dstSquare&7, dstSquare/8);
        Bit* bit = src.bit();
        bitMovedFromTo(*bit, src, dst);
        dst.dropBitAtPoint(bit, ImVec2(0, 0));
        src.setBit(nullptr);
    }
}

int Chess::negamax(std::string state, int depth, int alpha, int beta, char color){
    if (depth == 0) return (evaluateBoard(state, color) + (150*evaluatePosition(state, color)));

	int bestValue = -99'999;
    auto moves = generateMoves(state, color == 1 ? 'W' : 'B', true);
    for (const auto& move : moves) {
        if(moves.size() > 12){
            int random = rand() % 10;
            if(random < 3){
                continue;
            }
        }
        
        else if (state[63 - notationToIndex(dontworryaboutit(move.from)) != '0'] && isupper(state[63 - notationToIndex(dontworryaboutit(move.from))]) == isupper(state[63 - notationToIndex(dontworryaboutit(move.to))])){
            continue;
        }

        if(evaluatePosition(state, color) < 0 && evaluatePosition(gridStringState(), color) >= 0 && bestValue != -99'999){
            continue;
        }else if(evaluatePosition(state, color) < 0 && evaluatePosition(gridStringState(), color) >= 0 && bestValue == -99'999){
            bestValue = -99'998;
        }

		auto copiedState = state;
		int srcSquare = 63 - notationToIndex(dontworryaboutit(move.from));
		int dstSquare = 63 - notationToIndex(dontworryaboutit(move.to));
		copiedState[dstSquare] = copiedState[srcSquare];
		copiedState[srcSquare] = '0';
		bestValue = getPieceValue(convertCharToPiece(copiedState[dstSquare])) * std::max(bestValue, -negamax(copiedState, depth - 1, -beta, -alpha, -color));
		alpha = std::max(alpha, bestValue);
		if (alpha >= beta) break;
	}
	return bestValue;
}

int Chess::notationToIndex(std::string notation) 
{
    return (notation[0] - 'a') + (notation[1] - '1') * 8;
}

std::string Chess::dontworryaboutit(std::string notation){
    std::string newNotation = "";
    char letter = notation[0];
    char number = notation[1];
    char newLetter;
    switch (letter) {
        case 'a':
            newLetter = 'h';
            break;
        case 'b':
            newLetter = 'g';
            break;
        case 'c':
            newLetter = 'f';
            break;
        case 'd':
            newLetter = 'e';
            break;
        case 'e':
            newLetter = 'd';
            break;
        case 'f':
            newLetter = 'c';
            break;
        case 'g':
            newLetter = 'b';
            break;
        case 'h':
            newLetter = 'a';
            break;
        default:
            return "";
    }
    std::stringstream ss;
    ss << newLetter << number;
    newNotation = ss.str();
    return newNotation;
}



void Chess::filterOutIllegalMoves(std::string state, std::vector<GameState::Move> &moves, char color)
{
    // find out where the king is
    int kingSquare = -1;
    for (int i=0; i<64; i++) {
        if (state[i] == 'k' && color == 'B' ) { kingSquare = i; break; }
        if (state[i] == 'K' && color == 'W' ) { kingSquare = i; break; }
    }

    for (auto it = moves.begin(); it != moves.end();){
        bool moveBad = false;
        std::string baseState = state;

        int srcSquare = 63 - notationToIndex(dontworryaboutit(it->from));
        int dstSquare = 63 - notationToIndex(dontworryaboutit(it->to));

        baseState[dstSquare] = char(state[srcSquare]);
        baseState[srcSquare] = '0';
        
        // Handle the case in which the king is the piece that moved, and it may have left or remained in check
        int updatedKingSquare = -1;
        for (int i=0; i<64; i++) {
            if (baseState[i] == 'k' && color == 'B' ) { updatedKingSquare = i; break; }
            if (baseState[i] == 'K' && color == 'W' ) { updatedKingSquare = i; break; }
        }

        auto oppositeMoves = generateMoves(baseState, oppositeColor(color), false);
        for (auto enemyMoves : oppositeMoves) {
            int enemyDst = notationToIndex(enemyMoves.to);
            if (enemyDst == notationToIndex(indexToNotation(updatedKingSquare / 8, updatedKingSquare % 8))){
                moveBad = true;
                break;
            }

            if(kingSquare - updatedKingSquare == 2 || updatedKingSquare - kingSquare == 2){
                int checkKingSquare = (kingSquare + updatedKingSquare) / 2;
                if (enemyDst == notationToIndex(indexToNotation(checkKingSquare / 8, checkKingSquare % 8))){
                    moveBad = true;
                    break;
                }
                if (enemyDst == notationToIndex(indexToNotation(kingSquare / 8, kingSquare % 8))){
                    moveBad = true;
                    break;
                }
            }
        }
        if (moveBad){
            it = moves.erase(it);
        }
        else {
            ++it;
        }
    }
}
