#pragma once
#include "Game.h"
#include "ChessSquare.h"

const int chessGridSize = 8; // Chess grid size
const int pieceSize = 64; // Size of each piece

enum ChessPiece{
    NoPiece,
    Pawn = 1, 
    Knight, 
    Bishop,
    Rook,
    Queen,
    King
};

class Chess : public Game {
public:
    Chess();
    ~Chess();

    struct Move
    {
        std::string from;
        std::string to;
    };

    struct LastMove {
        std::string from;
        std::string to;
        ChessPiece piece;
        int playerNumber; // 0 for White, 1 for Black
        Bit* capturedPiece; // Pointer to the captured piece, if any
        Bit* movePiece;

        // Other fields remain unchanged
        int enPassantRow;
        bool isPawnDoubleMove; // Specific flag for en passant condition
        bool isCastling;
        //-------Castling
        bool WhiteKingRookMoved;
        bool WhiteQueenRookMoved;
        bool BlackKingRookMoved;
        bool BlackQueenRookMoved;
        bool WhiteKingMoved;
        bool BlackKingMoved;
        // Add more flags as needed for special moves

        LastMove() : piece(NoPiece), playerNumber(-1), capturedPiece(nullptr),
            isPawnDoubleMove(false), isCastling(false),
            WhiteKingRookMoved(false), WhiteQueenRookMoved(false),
            BlackKingRookMoved(false), BlackQueenRookMoved(false),
            WhiteKingMoved(false), BlackKingMoved(false) {}
    };

    void setUpBoard() override;
    Player* checkForWinner() override;
    bool checkForDraw() override;
    bool isKingInCheck(char playerColor);
    bool isInsufficientMaterial();
    std::string initialStateString() override;
    std::string stateString() override;
    void setStateString(const std::string &s) override;
    bool actionForEmptyHolder(BitHolder &holder) override {return false;}
    bool canBitMoveFrom(Bit& bit, BitHolder &src) override;
    bool canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) override;
    void bitMovedFromTo(Bit& bit, BitHolder& src, BitHolder& dst) override;

    void clearBoardHighlights() override;

    void stopGame() override;

    BitHolder &getHolderAt(const int x, const int y) override { return _grid[x][y]; }

    //Fen
    void FENtoBoard(const std::string &fen);
    ChessPiece charToChessPiece(char ch);

    bool gameHasAI() override;
	void updateAI() override;

private:
    Bit* PieceForPlayer(const int playerNumber, ChessPiece piece);
    Player* ownerAt(int index) const;
    void addMoveIfValid(std::vector<Move>& moves, int fromRow, int fromCol, int toRow, int toCol);
    std::string indexToNotation(int row, int col);
    void notationToIndex(const std::string& notation, int &row, int &col); //new
    std::string pieceNotation(int row, int col) const;
    void generateKnightMoves(std::vector<Move>& moves, int row, int col);
    void generatePawnMoves(std::vector<Move>& moves, int row, int col, char color);
    void generateLinearMoves(std::vector<Move>& moves, int row, int col, const std::vector<std::pair<int, int>>& directions);
    void generateBishopMoves(std::vector<Move>& moves, int row, int col);
    void generateRookMoves(std::vector<Move>& moves, int row, int col);
    void generateQueenMoves(std::vector<Move>& moves, int row, int col);
    void generateKingMoves(std::vector<Move>& moves, int row, int col);
    char oppositeColor(char color);

    int evaluateBoard(const char* state);

    int negamax(int depth, int alpha, int beta, int color);
    // int negamax(int depth, int color);
    LastMove applyMove(Move& move);
    void undoMove(LastMove& move);
    void performAIMove();

    void filterOutIllegalMoves(std::vector<Chess::Move>& moves, char color);
    
    std::vector<Chess::Move> generateMoves(char color, bool filter);

    ChessSquare _grid[chessGridSize][chessGridSize];
    std::vector<Move> _moves;
    LastMove _lastMove;
    int counter = 0;
};
