#include "Chess.h"

Chess::Chess() {
}

Chess::~Chess() {
}

std::string Chess::pieceNotation(int row, int column) const
{
    if (row < 0 || row > 7 || column < 0 || column > 7) {
        return "??";
    }
    const char* pieces = "?PNBRQK";
    std::string notation = "";
    Bit* bit = _grid[row][column].bit();
    if (bit) {
        notation += bit->gameTag() >= 128 ? "W" : "B";
        notation += pieces[bit->gameTag()&127];
    } else {
        notation = "00";
    }
    return notation;
}

void Chess::FENtoBoard(const std::string &fen) {
    std::istringstream fenStream(fen);
    std::string placement;
    getline(fenStream, placement, ' ');

    int row = 7; // Start from the top of the board, which corresponds to row 7 in a 0-indexed array
    int col = 0;
    for (char ch : placement) {
        if (ch == '/') {
            row--; // Move to the next row down
            col = 0; // Reset column to the start of the row
            continue;
        }

        if (isdigit(ch)) {
            col += ch - '0'; // Skip empty squares as indicated by the digit
        } else {
            int playerNumber = isupper(ch) ? 0 : 1; // Determine player based on case
            ChessPiece piece = charToChessPiece(ch);

            Bit* bit = PieceForPlayer(playerNumber, piece);
            bit->setPosition(_grid[row][col].getPosition());
            bit->setParent(&_grid[row][col]);
            bit->setGameTag(piece + (playerNumber == 0 ? 128 : 0)); // Use 128 offset for white pieces if needed

            _grid[row][col].setBit(bit);

            col++; // Move to the next column
        }
    }

    // Additional setup based on the rest of the FEN string, if needed
}

ChessPiece Chess::charToChessPiece(char ch) {
    // Convert character to upper case to match the enumeration, since the case indicates color, not piece type
    char upperCh = toupper(ch);
    switch (upperCh) {
        case 'P': return Pawn;
        case 'N': return Knight;
        case 'B': return Bishop;
        case 'R': return Rook;
        case 'Q': return Queen;
        case 'K': return King;
        default: return NoPiece;
    }
}

void Chess::setUpBoard()
{
    const ChessPiece initialBoard[2][8] = {
        { Rook, Knight, Bishop, Queen, King, Bishop, Knight, Rook }, // 1st Rank
        { Pawn, Pawn, Pawn, Pawn, Pawn, Pawn, Pawn, Pawn }           // 2nd Rank
    };

    setNumberOfPlayers(2);
    _gameOptions.rowX = 8;
    _gameOptions.rowY = 8;

    for (int y=0; y<_gameOptions.rowY; y++) {
        for (int x=0; x<_gameOptions.rowX; x++) {
            ImVec2 position((float)(pieceSize * x + pieceSize), (float)(pieceSize * (_gameOptions.rowY - y)));
            _grid[y][x].initHolder(position, "boardsquare.png", x, y);
            _grid[y][x].setGameTag(0);
            _grid[y][x].setNotation( indexToNotation(y, x) );
        }
    }

    FENtoBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"); //Standard

    // FENtoBoard("1r5k/5ppp/8/8/8/8/5PPP/1R3RK1 w - - 0 1"); //checkmate

    // FENtoBoard("7k/5Q2/5K2/8/8/8/8/8 b - - 0 1"); //stalemate

    // FENtoBoard("r1bqkbnr/ppp2ppp/2n5/3pQ3/8/5N1P/PPP1PPP1/RNBQKB1R"); // black in check
    // FENtoBoard("rnbqkbnr/pppppppp/8/P2PP2P/8/8/8/RNBQKBNR"); // en passant
    // FENtoBoard("rnbqkbnr/pppppp1p/8/8/2P1P1p1/8/PP1P1PPP/RNBQKBNR"); // en passant
    // FENtoBoard("rnbqkbnr/8/8/pppppppp/PPPPPPPP/8/8/RNBQKBNR"); // Mid
    // FENtoBoard("5k2/8/8/8/8/8/8/4K2R w K - 0 1" ); // white can castle
    // FENtoBoard("3k4/8/8/8/8/8/8/R3K3 w Q - 0 1"); // white can castle queen side
    // FENtoBoard("r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1"); // white can castle both sides
    // FENtoBoard("2K2r2/4P3/8/8/8/8/8/3k4 w - - 0 1"); // white can promote to queen
    // FENtoBoard("4k3/1P6/8/8/8/8/K7/8 w - - 0 1"); // white can promote to queen

    // After the board is set up, evaluate the board and print the score
    // std::string boardState = stateString(); 
    // std::cout << "Initial boardState: " << boardState << std::endl;
    // int boardScore = evaluateBoard(boardState.c_str());
    // std::cout << "Initial Board Score: " << boardScore << std::endl;

    _lastMove = LastMove(); 

    // if (gameHasAI()) {
    //     setAIPlayer(AI_PLAYER)
    // }

    _moves = generateMoves('W', true);

    startGame();
}

Bit* Chess::PieceForPlayer(const int playerNumber, ChessPiece piece)
{
    const char *pieces[] = { "pawn.png", "knight.png", "bishop.png", "rook.png", "queen.png", "king.png" };
    
    // depending on playerNumber load the "x.png" or the "o.png" graphic
    Bit *bit = new Bit();
    // should possibly be cached from player class?
    const char *pieceName = pieces[piece - 1];
    std::string spritePath = std::string("chess/") + (playerNumber == 0 ? "w_" : "b_") + pieceName;
    bit->LoadTextureFromFile(spritePath.c_str());
    bit->setOwner(getPlayerAt(playerNumber));
    bit->setSize(pieceSize, pieceSize);
    
    return bit;
}

std::string Chess::initialStateString() {
    // Serialize the initial state of the game
    return stateString(); // Placeholder, modify as needed
}

std::string Chess::stateString() {
    std::string s;
    for (int y=0; y<_gameOptions.rowY; y++) {
        for (int x=0; x<_gameOptions.rowX; x++) {
            s += pieceNotation(y, x);
        }
    }
    return s;
}

void Chess::clearBoardHighlights() {
    for (int y = 0; y < _gameOptions.rowY; y++) {
        for (int x = 0; x < _gameOptions.rowX; x++) {
            ChessSquare& dstSquare = _grid[y][x];
            ;
            dstSquare.setMoveHighlighted(false);
        }
    }
}

bool Chess::canBitMoveFrom(Bit& bit, BitHolder& src) {
    ChessSquare& srcSquare = static_cast<ChessSquare&>(src);
    bool canMove = false;
    for (auto move : _moves) {
        if (move.from == srcSquare.getNotation()) {
            canMove = true;
            for (int y = 0; y < _gameOptions.rowY; y++) {
                for (int x = 0; x < _gameOptions.rowX; x++) {
                    ChessSquare& dstSquare = _grid[y][x];
                    ;
                    if (move.to == dstSquare.getNotation()) {
                        dstSquare.setMoveHighlighted(true);
                    }
                }
            }
        }
    }
    return canMove;
}

bool Chess::canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst)
{
    ChessSquare &srcSquare = static_cast<ChessSquare&>(src);
    ChessSquare &dstSquare = static_cast<ChessSquare&>(dst);
    for (auto move : _moves) {
        if (move.from == srcSquare.getNotation() && move.to == dstSquare.getNotation()) {
            return true;
        }
    }
    return false;
}

void Chess::bitMovedFromTo(Bit& bit, BitHolder& src, BitHolder& dst) {
    Game::bitMovedFromTo(bit, src, dst);

    // Update lastMove details
    ChessSquare& srcSquare = static_cast<ChessSquare&>(src);
    ChessSquare& dstSquare = static_cast<ChessSquare&>(dst);
    _lastMove.from = srcSquare.getNotation();
    _lastMove.to = dstSquare.getNotation();
    _lastMove.piece = (ChessPiece)(bit.gameTag() & 127);
    _lastMove.playerNumber = bit.gameTag() >= 128 ? 0 : 1;
    // _lastMove.isCapture = dstSquare.bit() != nullptr;
    _lastMove.isCastling = false;
    // _lastMove.isPawnDoubleMove = false;
    // std::cout << "isCapture: " << _lastMove.isCapture << std::endl;

    // CASTLING------------------------------------------------------------
    // Check and update castling flags based on the piece moved
    if (_lastMove.piece == King) {
        if (srcSquare.getDistance(dstSquare) == 2) {
            _lastMove.isCastling = true;
        }
        if (_lastMove.playerNumber == 0) { // White King
            _lastMove.WhiteKingMoved = true;
        } else { // Black King
            _lastMove.BlackKingMoved = true;
        }
    } 
    if (_lastMove.piece == Rook) {
        // std::cout << "Last move from: " << _lastMove.from << std::endl;
        if (_lastMove.playerNumber == 0) { // White Rook
            if (_lastMove.from == "h1") {
                _lastMove.WhiteKingRookMoved = true;
            } else if (_lastMove.from == "a1") {
                _lastMove.WhiteQueenRookMoved = true;
            }
        } else { // Black Rook
            if (_lastMove.from == "h8") {
                _lastMove.BlackKingRookMoved = true;
            } else if (_lastMove.from == "a8") {
                _lastMove.BlackQueenRookMoved = true;
            }
        }
    }

    // Check if the move is a castling move
    if (_lastMove.isCastling) {
        // Determine the side (kingside or queenside) and move the rook accordingly
        int kingFinalCol = dstSquare.getColumn();
        int rookSrcCol, rookDstCol;
        if (kingFinalCol == 6) { // Kingside castling
            rookSrcCol = 7; // Original rook position for kingside castling
            rookDstCol = 5; // Rook's final position for kingside castling
        } else if (kingFinalCol == 2) { // Queenside castling
            rookSrcCol = 0; // Original rook position for queenside castling
            rookDstCol = 3; // Rook's final position for queenside castling
        }

        // Move the rook
        ChessSquare& rookSrcSquare = _grid[dstSquare.getRow()][rookSrcCol];
        ChessSquare& rookDstSquare = _grid[dstSquare.getRow()][rookDstCol];
        Bit* rookBit = rookSrcSquare.bit();
        if (rookBit) {
            // rookSrcSquare.setBit(nullptr); // Remove the rook from its original position
            // rookDstSquare.setBit(rookBit); // Place the rook in its new position
            // // Update the rook's position in the game state if necessary
            rookDstSquare.setBit(rookBit);
            rookSrcSquare.setBit(nullptr);
            rookBit->setPosition(rookDstSquare.getPosition());
        }
    }

    // PROMOTION------------------------------------------------------------
    // Determine the row index for promotion based on the color of the pawn
    int promotionRow = bit.gameTag() >= 128 ? 7 : 0; // 0 for white (moving downwards), 7 for black (moving upwards)

    // Check if the bit is a pawn and has reached the promotion row
    // ChessSquare& srcSquare = static_cast<ChessSquare&>(src);
    // ChessSquare& dstSquare = static_cast<ChessSquare&>(dst);
    if ((bit.gameTag() & 127) == Pawn && dstSquare.getRow() == promotionRow) {
        // Remove the pawn from the board
        dstSquare.destroyBit();

        // Create a new queen bit and place it on the board at the destination square
        int playerNumber = (_gameOptions.currentTurnNo & 1) ? 0 : 1; // Determine the player based on the current turn
        Bit *queenBit = PieceForPlayer(playerNumber, Queen); // Create a queen bit for the player

        // Set the queen's position and parent to the destination square
        queenBit->setPosition(dstSquare.getPosition());
        queenBit->setParent(&dstSquare);
        queenBit->setGameTag(Queen + (playerNumber == 0 ? 128 : 0)); // Set the game tag for the queen, including the color

        // Place the queen on the board
        dstSquare.setBit(queenBit);
    }

    // EN PASSANT------------------------------------------------------------

    // Handling en passant capture
    // std::cout << "dstSquare.getRow(): " << dstSquare.getRow() << std::endl;
    // std::cout << "_lastMove.enPassantRow: " << _lastMove.enPassantRow << std::endl;
    if (_lastMove.piece == Pawn && _lastMove.enPassantRow != -1 && dstSquare.getRow() == _lastMove.enPassantRow) {
        // int currentPlayerNumber = _gameOptions.currentTurnNo % 2;
        // std::cout << "currentPlayerNumber: " << currentPlayerNumber << std::endl;
    
        // The column of the captured pawn is the same as the destination column of the capturing pawn
        int capturedPawnCol = dstSquare.getColumn();
        // std::cout << "capturedPawnCol: " << capturedPawnCol << std::endl;

        // Determine the row of the captured pawn based on the color of the capturing pawn
        int capturedPawnRow = _lastMove.playerNumber == 0 ? 4 : 3; // 4 for white capturing pawn, 3 for black
        // std::cout << "capturedPawnRow: " << capturedPawnRow << std::endl;
        // std::cout << "_lastMove.playerNumber: " << _lastMove.playerNumber << std::endl;

        // Remove the captured pawn from the board
        ChessSquare& capturedSquare = _grid[capturedPawnRow][capturedPawnCol];
        // if (currentPlayerNumber != _lastMove.playerNumber) {
        //     capturedSquare.destroyBit();
        // }
        capturedSquare.destroyBit();
        // if (capturedSquare.bit() && capturedSquare.bit()->gameTag() == Pawn) {
        //     capturedSquare.destroyBit();
        // }
        // dstSquare.destroyBit();

        // Update _lastMove to reflect en passant capture specifics if needed
        // _lastMove.isCapture = false; // Mark this move as a capture
    }
    
    // EN PASSANT------------------------------------------------------------
    // Check for a pawn double move (en passant eligibility condition)
    if (_lastMove.piece == Pawn && srcSquare.getDistance(dstSquare) == 2) {
        _lastMove.isPawnDoubleMove = true;
        _lastMove.enPassantRow = _lastMove.playerNumber == 0 ? 2 : 5; 
    } else {
        _lastMove.isPawnDoubleMove = false;
        _lastMove.enPassantRow = -1;
    }

    _moves = generateMoves((_gameOptions.currentTurnNo & 1) ? 'B' : 'W', true);

    // std::string boardState = stateString(); 
    // std::cout << "movedd State: " << boardState << std::endl;
    // int boardScore = evaluateBoard(boardState.c_str());
    // std::cout << "Initial Board Score: " << boardScore << std::endl;
    // endTurn();
}

void Chess::stopGame() {
    // Implement any cleanup or finalization needed when the game stops
    // Placeholder, modify as needed
    for (int y=0; y<_gameOptions.rowY; y++) {
        for (int x=0; x<_gameOptions.rowX; x++) {
            _grid[y][x].destroyBit();
        }
    }
}

// Helper function to add a move if valid
void Chess::addMoveIfValid(std::vector<Move>& moves, int fromRow, int fromCol, int toRow, int toCol)
{
    if (toRow >= 0 && toRow < 8 && toCol >= 0 && toCol < 8) {
        if (pieceNotation(fromRow, fromCol)[0] != pieceNotation(toRow, toCol)[0]) { // Ensure not capturing own piece
            moves.push_back({indexToNotation(fromRow, fromCol), indexToNotation(toRow, toCol)});
        }
    }
}

// Convert row and column index to chess notation
std::string Chess::indexToNotation(int row, int col)
{
    // return std::string(1, 'a' + col) + std::string(1, '8' - row);
    return std::string(1, 'a' + col) + std::string(1, '1' + row);
}

void Chess::notationToIndex(const std::string& notation, int &row, int &col) {
    // Extract the column from the notation (letter)
    col = notation[0] - 'a';

    // Extract the row from the notation (number)
    // Since your notation uses '1' at the bottom and increases upwards,
    // this conversion may need to reflect how your board's rows are indexed internally.
    row = notation[1] - '1';
}


// Generate moves for a knight
void Chess::generateKnightMoves(std::vector<Move>& moves, int row, int col) 
{
    static const int movesRow[] = {2, 1, -1, -2, -2, -1, 1, 2};
    static const int movesCol[] = {1, 2, 2, 1, -1, -2, -2, -1};

    for (int i = 0; i < 8; ++i) {
        int newRow = row + movesRow[i];
        int newCol = col + movesCol[i];
        addMoveIfValid(moves, row, col, newRow, newCol);
    }
}

void Chess::generatePawnMoves(std::vector<Move>& moves, int row, int col, char color) {
    int direction = (color == 'B') ? -1 : 1;
    int startRow = (color == 'B') ? 6 : 1;

    // One square forward
    if (pieceNotation(row + direction, col) == "00") {
        addMoveIfValid(moves, row, col, row + direction, col);
        // Two squares from start row
        if (row == startRow && pieceNotation(row + 2 * direction, col) == "00") {
            addMoveIfValid(moves, row, col, row + 2 * direction, col);
        }
    }

    // Regular captures
    for (int i = -1; i <= 1; i += 2) { // -1 for left, +1 for right
        if (col + i >= 0 && col + i < 8 && row + direction >= 0 && row + direction < 8 && pieceNotation(row + direction, col + i)[0] == oppositeColor(color)) {
            addMoveIfValid(moves, row, col, row + direction, col + i);
        }
    }

    // En passant captures
    // Adjusting to handle black and white separately
    // std::cout << "color: " << color << std::endl;
    int enPassantTriggerRow = (color == 'B') ? 3 : 4; // Row where the pawn must be to potentially capture en passant
    // std::cout << "enPassantTriggerRow: " << enPassantTriggerRow << std::endl;
    int enPassantCaptureRow = (color == 'B') ? 2 : 5; // The row to move to when capturing en passant
    // std::cout << "enPassantCaptureRow: " << enPassantCaptureRow << std::endl;
    int lastMoveEndRow, lastMoveEndCol;
    notationToIndex(_lastMove.to, lastMoveEndRow, lastMoveEndCol);
    // _lastMove.enPassantRow = -1; // Reset the en passant row to -1
    
    if (row == enPassantTriggerRow && _lastMove.isPawnDoubleMove) {
        // Check if the pawn to be captured moved into position beside this pawn
        if ((lastMoveEndCol == col - 1 || lastMoveEndCol == col + 1)) {
            // Add the en passant capture move
            addMoveIfValid(moves, row, col, enPassantCaptureRow, lastMoveEndCol);
            // std::cout << "row: " << row << std::endl;
            // std::cout << "col: " << col << std::endl;
            // std::cout << "enPassantCaptureRow: " << enPassantCaptureRow << std::endl;
            // _lastMove.enPassantRow = 2;
            // _lastMove.enPassantRow = enPassantCaptureRow;
            // std::cout << "_lastMove.enPassantRow : " << _lastMove.enPassantRow  << std::endl;
        }
    }
}

// Generate moves in a linear direction until blocked
void Chess::generateLinearMoves(std::vector<Move>& moves, int row, int col, const std::vector<std::pair<int, int>>& directions)
{
    for (auto& dir : directions) {
        int currentRow = row + dir.first;
        int currentCol = col + dir.second;
        while (currentRow >= 0 && currentRow < 8 && currentCol >= 0 && currentCol < 8) {
            if (pieceNotation(currentRow, currentCol) != "00") {
                addMoveIfValid(moves, row, col, currentRow, currentCol);
                break; // Stop if there's a piece in the way
            }
            addMoveIfValid(moves, row, col, currentRow, currentCol);
            currentRow += dir.first;
            currentCol += dir.second;
        }
    }
}

// Generate moves for a bishop
void Chess::generateBishopMoves(std::vector<Move>& moves, int row, int col)
{
    static const std::vector<std::pair<int, int>> directions = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}; // diagonals
    generateLinearMoves(moves, row, col, directions);
}

// Generate moves for a rook
void Chess::generateRookMoves(std::vector<Move>& moves, int row, int col)
{
    static const std::vector<std::pair<int, int>> directions = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}}; // straight lines
    generateLinearMoves(moves, row, col, directions);
}

void Chess::generateQueenMoves(std::vector<Move>& moves, int row, int col) {
    generateRookMoves(moves, row, col);
    generateBishopMoves(moves, row, col);
}

void Chess::generateKingMoves(std::vector<Move>& moves, int row, int col)
{
    static const std::vector<std::pair<int, int>> directions = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    }; // All 8 directions around the king
    
    for (auto& dir : directions) {
        int newRow = row + dir.first;
        int newCol = col + dir.second;
        if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
            addMoveIfValid(moves, row, col, newRow, newCol);
        }
    }

    // Castling logic
    if ((row == 0 && col == 4 && !_lastMove.WhiteKingMoved) || (row == 7 && col == 4 && !_lastMove.BlackKingMoved)) {
        // White's castling
        // std::cout << "row: " << row << std::endl;
        if (row == 0) {
            // Kingside castling for White
            if (!_lastMove.WhiteKingRookMoved && _grid[0][5].bit() == nullptr && _grid[0][6].bit() == nullptr) {
                moves.push_back({"e1", "g1"}); // Ensure no check conditions along the path
            }
            // Queenside castling for White
            if (!_lastMove.WhiteQueenRookMoved && _grid[0][3].bit() == nullptr && _grid[0][2].bit() == nullptr && _grid[0][1].bit() == nullptr) {
                moves.push_back({"e1", "c1"}); // Ensure no check conditions along the path
            }
        }
        // Black's castling
        else if (row == 7) {
            // Kingside castling for Black
            if (!_lastMove.BlackKingRookMoved && _grid[7][5].bit() == nullptr && _grid[7][6].bit() == nullptr) {
                moves.push_back({"e8", "g8"}); // Ensure no check conditions along the path
            }
            // Queenside castling for Black
            if (!_lastMove.BlackQueenRookMoved && _grid[7][3].bit() == nullptr && _grid[7][2].bit() == nullptr && _grid[7][1].bit() == nullptr) {
                moves.push_back({"e8", "c8"}); // Ensure no check conditions along the path
            }
        }
    }
}

char Chess::oppositeColor(char color)
{
    return (color == 'W') ? 'B' : 'W';
}

std::vector<Chess::Move> Chess::generateMoves(char color, bool filter)
{
    std::vector<Move> moves;

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            std::string piece = pieceNotation(row, col);
            if (!piece.empty() && piece != "00" && piece[0] == color) {
                switch (piece[1]) {
                    case 'N': // Knight
                        generateKnightMoves(moves, row, col);
                        break;
                    case 'P': // Pawn
                        generatePawnMoves(moves, row, col, piece[0]);
                        break;
                    case 'B': // Bishop
                        generateBishopMoves(moves, row, col);
                        break;
                    case 'R': // Rook
                        generateRookMoves(moves, row, col);
                        break;
                    case 'Q': // Queen
                        generateQueenMoves(moves, row, col);
                        break;
                    case 'K': // King
                        generateKingMoves(moves, row, col);
                        break;
                    // ... other cases for other pieces if necessary
                }
            }
        }
    }
    if (filter) {
        filterOutIllegalMoves(moves, color);
    }
    return moves;
}

std::map<std::string, int> scores = {
    {"WP", 100},  {"BP", -100},
    {"WN", 200},  {"BN", -200},
    {"WB", 230},  {"BB", -230},
    {"WR", 400},  {"BR", -400},
    {"WQ", 900},  {"BQ", -900},
    {"WK", 2000}, {"BK", -2000},
    {"00", 0} // Assuming '00' is used for empty squares
};

int Chess::evaluateBoard(const char* state) {
    // std::map<char, int> scores = {
    //     {'p', 100},  {'P', -100},
    //     {'n', 200},  {'N', -200},
    //     {'b', 230},  {'B', -230},
    //     {'r', 400},  {'R', -400},
    //     {'q', 900},  {'Q', -900},
    //     {'k', 2000}, {'K', -2000},
    //     {'e', 0}
    // };

    // std::map<std::string, int> scores = {
    //     {"WP", 100},  {"BP", -100},
    //     {"WN", 200},  {"BN", -200},
    //     {"WB", 230},  {"BB", -230},
    //     {"WR", 400},  {"BR", -400},
    //     {"WQ", 900},  {"BQ", -900},
    //     {"WK", 2000}, {"BK", -2000},
    //     {"00", 0} // Assuming '00' is used for empty squares
    // };

    int score = 0;
    for (int i = 0; i < 128; i += 2) { // Increment by 2 because each piece is represented by two characters
        // std::cout << "i: " << i << std::endl;
        std::string piece(state + i, state + i + 2); // Create a substring for each piece
        // std::cout << "boardState: " << piece << std::endl;
        score += scores[piece];
        // std::cout << "current score: " << score << std::endl;
    }

    return score;
}

Chess::LastMove Chess::applyMove(Move& move) {
    // Convert move notation to board indices
    int fromRow, fromCol, toRow, toCol;
    notationToIndex(move.from, fromRow, fromCol);
    notationToIndex(move.to, toRow, toCol);

    Bit* movedBit = _grid[fromRow][fromCol].bit();
    Bit* capturedBit = _grid[toRow][toCol].bit();

    LastMove savedMove;

    // Update the LastMove with details of this move
    savedMove.from = move.from;
    savedMove.to = move.to;
    // savedMove.piece = (ChessPiece)(movedBit.gameTag() & 127);
    savedMove.piece = movedBit ? (ChessPiece)(movedBit->gameTag() & 127) : NoPiece;
    savedMove.movePiece = movedBit;
    savedMove.playerNumber = movedBit && (movedBit->gameTag() >= 128) ? 0 : 1;
    savedMove.capturedPiece = capturedBit; // Store the captured piece

    // Simulate the move
    if (movedBit) {
        // std::cout << "did it" << std::endl;
        _grid[toRow][toCol].setBitOverride(movedBit); // Move the piece to the new location
        movedBit->moveTo(_grid[toRow][toCol].getPosition());
        movedBit->setParent(&_grid[toRow][toCol]);
        _grid[fromRow][fromCol].setBitOverride(nullptr); // Clear the old location
    }
    return savedMove;
}

void Chess::undoMove(LastMove& lastMove) {
    // Convert move notation back to board indices
    int fromRow, fromCol, toRow, toCol;
    notationToIndex(lastMove.from, fromRow, fromCol);
    notationToIndex(lastMove.to, toRow, toCol);

    // Retrieve the moved and potentially captured pieces from the LastMove object
    Bit* movedBit = lastMove.movePiece;
    Bit* capturedBit = lastMove.capturedPiece;

    // Revert the move
    if (movedBit) {
        // Move the piece back to its original location
        _grid[fromRow][fromCol].setBitOverride(movedBit);
        movedBit->moveTo(_grid[fromRow][fromCol].getPosition());
        movedBit->setParent(&_grid[fromRow][fromCol]);

        // Restore the captured piece, if there was one
        if (capturedBit) {
            _grid[toRow][toCol].setBitOverride(capturedBit);
            capturedBit->moveTo(_grid[toRow][toCol].getPosition());
            capturedBit->setParent(&_grid[toRow][toCol]);
        } else {
            // If there was no captured piece, ensure the destination is cleared
            _grid[toRow][toCol].setBitOverride(nullptr);
        }
    }
}

bool Chess::gameHasAI() {
    return true; // Indicate that this game supports AI
}

void Chess::updateAI() {
    if (getCurrentPlayer()->isAIPlayer()) {
        performAIMove(); // Make the best move for the AI
    }
}

int Chess::negamax(int depth, int alpha, int beta, int color) {
    // if (depth == 0 || checkForWinner() || checkForDraw()) {
    //     return color * evaluateBoard(stateString().c_str());
    // }

    if (depth == 0) {
        return color * evaluateBoard(stateString().c_str());
    }

        // counter++;
        // std::cout << "negamax:" << counter << std::endl;

    std::vector<Move> moves = generateMoves(color == 1 ? 'W' : 'B', true);
    if (moves.empty()) {
        // Adjust scoring for checkmate and stalemate if necessary
        return color * -10000; // Placeholder score for checkmate or specific terminal condition
    }

    int maxScore = INT_MIN;
    for (auto move : moves) {
        LastMove savedMove = applyMove(move);
        
        // Now using alpha and beta for pruning
        int score = -negamax(depth - 1, -beta, -alpha, -color);

        undoMove(savedMove);

        if (score > maxScore) maxScore = score;
        alpha = std::max(alpha, score);
        if (alpha >= beta) break; // Alpha-beta pruning
    }

    return maxScore;
}

void Chess::performAIMove() {
    const int myInfinity = 1000000; // Use a large value for infinity
    int bestScore = -myInfinity;
    Move bestMove;

    int alpha = -myInfinity;
    int beta = myInfinity;
    int color = -1; // Assuming AI plays as Black (-1), use 1 for White

    std::vector<Move> moves = generateMoves('B', true); // Assuming AI plays as Black
    for (auto move : moves) {
        LastMove savedMove = applyMove(move);

        // Negamax search with Alpha-Beta pruning, flipping color (-color)
        int score = -negamax(3, -beta, -alpha, -color); // Depth set to 3 or another value based on your requirement
        
        undoMove(savedMove);

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
        alpha = std::max(alpha, score);
        if (alpha >= beta) break; // Alpha-beta pruning
    }

    // Perform the best move found
    if (!bestMove.from.empty()) {
        int fromRow, fromCol, toRow, toCol;
        notationToIndex(bestMove.from, fromRow, fromCol);
        notationToIndex(bestMove.to, toRow, toCol);

        BitHolder& src = getHolderAt(fromRow, fromCol);
        BitHolder& dst = getHolderAt(toRow, toCol);
        Bit* bit = src.bit();
        dst.dropBitAtPoint(bit, ImVec2(0, 0));
        src.setBit(nullptr);
        bitMovedFromTo(*bit, src, dst);
    }
}

// void Chess::filterOutIllegalMoves(std::vector<Chess::Move>& moves, char color) {
//     char baseState[65];
//     std::string copyState = stateString();

//     // stateString().c_str()
//     //
//     // find out where the king is
//     //
//     int kingSquare = -1;
//     for (int i=0; i<64; i++) {
//         if (copyState[i] == 'k' && color == 'B') { kingSquare = i; break; }
//         if (copyState[i] == 'K' && color == 'W') { kingSquare = i; break; }
//     }

//     for (auto it = moves.begin(); it != moves.end(); ) {
//         bool moveBad = false;
//         strcpy(&baseState[0], copyState.c_str());
//         int srcSquare = notationToIndex(it->from);
//         int dstSquare = notationToIndex(it->to);
//         baseState[dstSquare] = baseState[srcSquare];
//         baseState[srcSquare] = '0';
//         auto oppositeMoves = generateMoves(baseState, color == 'W' ? 'B' : 'W', false);
//         for (auto enemyMoves : oppositeMoves) {
//             int enemyDst = notationToIndex(enemyMoves.to);
//             if (enemyDst == kingSquare) {
//                 moveBad = true;
//                 break;
//             }
//         }
//         if (moveBad) {
//             it = moves.erase(it);
//         } else {
//             ++it;
//         }
//     }
// }

static std::map<char, ChessPiece> ChessPieces = {
        {'P', Pawn},
        {'N', Knight},
        {'B', Bishop},
        {'R', Rook},
        {'Q', Queen},
        {'K', King},
        {'0', NoPiece}
}; 

void Chess::setStateString(const std::string &s)
{
    for (int y=0; y<_gameOptions.rowY; y++) {
        for (int x=0; x<_gameOptions.rowX; x++) {
            int index = y*_gameOptions.rowX + x;
            int playerNumber = s[index * 2] == 'W' ? 0 : 1;
            ChessPiece piece =ChessPieces[s[index * 2 + 1]];
            if (piece != NoPiece) {
                Bit* bit = PieceForPlayer(playerNumber, piece);
                bit->setPosition(_grid[y][x].getPosition());
                bit->setParent(&_grid[y][x]);
                bit->setGameTag(piece + (playerNumber == 0 ? 128 : 0)); // Use 128 offset for white pieces if needed

                _grid[y][x].setBitOverride(bit);
                // _grid[y][x].setBit( PieceForPlayer(playerNumber, piece) );
            } else {
                _grid[y][x].setBitOverride( nullptr );
            }
        }
    }
}

void Chess::filterOutIllegalMoves(std::vector<Chess::Move>& moves, char color) {
    // return;
    char baseState[129];
    std::string copyState = std::string(stateString().c_str());

    // std::cout << "copied state: " << copyState << std::endl;

    // find out where the king is
    int kingSquare = -1;
    for (int i=0; i< 128; i+=2) {
        if (copyState[i] == color && copyState[i+1] == 'K') { kingSquare = i/2; break; }
    }

    for (auto it = moves.begin(); it != moves.end(); ) {
        bool moveBad = false;
        memcpy(&baseState[0], copyState.c_str(), 128);

        // Convert source and destination from notation to row and column
        int srcRow, srcCol, dstRow, dstCol;
        notationToIndex(it->from, srcRow, srcCol);
        notationToIndex(it->to, dstRow, dstCol);

        // Convert row and column back to index for baseState
        int srcSquare = srcRow * 8 + srcCol;
        int dstSquare = dstRow * 8 + dstCol;

        baseState[dstSquare * 2 + 0] = baseState[srcSquare * 2 + 0];
        baseState[dstSquare * 2 + 1] = baseState[srcSquare * 2 + 1];
        baseState[srcSquare * 2 + 0] = '0';
        baseState[srcSquare * 2 + 1] = '0';

        setStateString(baseState);

        // Handle the case in which the king is the piece that moved, and it may have left or remained in check
        int updatedKingSquare = kingSquare;
        if (baseState[dstSquare * 2 + 1] == 'K')
        {
            updatedKingSquare = dstSquare;
        }

        auto oppositeMoves = generateMoves(color == 'W' ? 'B' : 'W', false);
        for (auto enemyMove : oppositeMoves) {
            int enemyRow, enemyCol;
            notationToIndex(enemyMove.to, enemyRow, enemyCol);
            int enemyDst = enemyRow * 8 + enemyCol;
            if (enemyDst == updatedKingSquare) {
                moveBad = true;
                break;
            }
        }

        if (moveBad) {
            it = moves.erase(it);
        } else {
            ++it;
        }
    }
    setStateString(copyState);
}

Player* Chess::checkForWinner() {
    char currentPlayerColor = (_gameOptions.currentTurnNo & 1) ? 'B' : 'W';
    std::vector<Move> moves = generateMoves(currentPlayerColor, true); // Generate all legal moves for the current player

    if (moves.empty()) { // No legal moves available
        if (isKingInCheck(currentPlayerColor)) { // Check if the current player's king is in check
            // If the king is in check and there are no legal moves, it's checkmate
            return currentPlayerColor == 'W' ? getPlayerAt(1) : getPlayerAt(0); // Return the winner (opposite player)
        }
    }
    
    // std::cout << "Winnter:" << currentPlayerColor << std::endl;
    return nullptr; // No checkmate detected, no winner yet
}

bool Chess::checkForDraw() {
    char currentPlayerColor = (_gameOptions.currentTurnNo & 1) ? 'B' : 'W';
    std::vector<Move> moves = generateMoves(currentPlayerColor, true); // Generate all legal moves for the current player

    if (moves.empty() && !isKingInCheck(currentPlayerColor)) {
        // If there are no legal moves and the king is not in check, it's a stalemate
        return true;
    }

    // Check for insufficient material - only two kings left
    if (isInsufficientMaterial()) {
        return true; // It's a draw due to insufficient material
    }

    // Additional draw conditions (threefold repetition, 50-move rule) can be checked here

    return false; // No draw condition detected
}

bool Chess::isInsufficientMaterial() {
    int piecesCount = 0;
    for (int y = 0; y < _gameOptions.rowY; ++y) {
        for (int x = 0; x < _gameOptions.rowX; ++x) {
            if (_grid[y][x].bit() != nullptr) { // There's a piece on this square
                piecesCount++;
            }
        }
    }

    // If the count is exactly 2, then only the two kings are left
    return piecesCount == 2;
}


bool Chess::isKingInCheck(char playerColor) {
    // Find the king's position
    std::string kingPosition = "";
    for (int y = 0; y < _gameOptions.rowY; ++y) {
        for (int x = 0; x < _gameOptions.rowX; ++x) {
            std::string pieceNotation = this->pieceNotation(y, x);
            if (pieceNotation[0] == playerColor && pieceNotation[1] == 'K') {
                kingPosition = indexToNotation(y, x);
                break;
            }
        }
    }

    if (kingPosition.empty()) return false; // If the king's position is not found, return false (shouldn't happen)

    // Check if any of the opponent's moves can capture the king
    char opponentColor = (playerColor == 'W') ? 'B' : 'W';
    std::vector<Move> opponentMoves = generateMoves(opponentColor, false); // Generate all moves for the opponent, without filtering for legality
    for (const Move& move : opponentMoves) {
        if (move.to == kingPosition) {
            return true; // The king is in check if any opponent move can capture the king's position
        }
    }

    return false; // The king is not in check
}

