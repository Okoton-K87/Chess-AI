# Chess Game Enhancements

This document outlines the implementation details for the enhancements made to the Chess game. Specifically, it covers the logic behind detecting checkmate and stalemate conditions, along with the filtering of illegal moves to ensure game rules are strictly followed.

## Features

- **Checkmate Detection (`checkForWinner`)**: Determines if the current player has no legal moves and their king is in check, indicating a checkmate.
- **Stalemate Detection (`checkForDraw`)**: Identifies a stalemate condition, where the current player has no legal moves but their king is not in check.
- **Draw Conditions**: Includes checks for insufficient material and the specific condition when only the two kings are left on the board, resulting in a draw.
- **Filtering Illegal Moves (`filterOutIllegalMoves`)**: Ensures that moves resulting in the player's king being in check are considered illegal and filtered out from the list of possible moves.

## Implementation Details

### Checkmate Detection

- The `checkForWinner` function evaluates if the current player is in checkmate. This is done by generating all possible moves for the current player's pieces using `generateMoves`. If there are no legal moves and the player's king is in check (`isKingInCheck`), a checkmate condition is confirmed.

### Stalemate and Draw Detection

- The `checkForDraw` function checks for a stalemate by generating all possible moves for the current player. If there are no legal moves and the player's king is not in check, a stalemate is detected.
- Additionally, `checkForDraw` checks for a draw due to insufficient material on the board. This includes a specific check (`isInsufficientMaterial`) for scenarios such as when only the two kings remain, making checkmate impossible.

### Filtering Illegal Moves

- `filterOutIllegalMoves` removes any moves from the possible moves list that would leave or place the player's king in check. This involves simulating each move on a copy of the current board state and checking if the player's king would be in check after the move.
- This function plays a critical role in ensuring the game adheres to chess rules, particularly the rule that a player cannot make a move that places their own king in check.

## Utilities

- **King Check Detection (`isKingInCheck`)**: Determines if the player's king is in check by generating all possible moves for the opponent and checking if any of those moves capture the king.
- **Insufficient Material Detection (`isInsufficientMaterial`)**: Checks the entire board for the number of pieces remaining. If the count indicates that only the two kings are left, or another condition where checkmate is impossible, the function returns true, indicating a draw.

The implementation of these features enhances the gameplay experience by ensuring that all game rules and conditions for ending the game are accurately detected and enforced.
