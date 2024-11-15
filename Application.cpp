#include "Application.h"
#include "imgui/imgui.h"
#include "classes/Chess.h" // Include the Chess class header

namespace ClassGame {
    //
    // Global variables
    //
    Chess *game = nullptr; // Use Chess instead of TicTacToe
    int gameWinner = -1; 
    bool gameGoing = true;

    //
    // Game startup function
    //
    void GameStartUp() {
        game = new Chess(); // Initialize a new Tic Tac Toe game
        game->setUpBoard(); // Set up the game board
        game->setAIPlayer(1); // Set AI as player 2
        gameWinner = -1; // Reset the winner to no winner
        gameGoing = true;
        // game->pauseGame(false); 
    }

    //
    // Game rendering loop
    //
    void RenderGame() {
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        // Game settings/info window
        ImGui::Begin("Settings");

        if (gameGoing) {
            ImGui::Text("Current Player Number: %d", game->getCurrentPlayer()->playerNumber());
            ImGui::Text("Current Board State: %s", game->stateString().c_str());
        } else {
            ImGui::Text("Game Over!");
            if (gameWinner != -1) {
                ImGui::Text("Winner: Player %d", gameWinner);
            } else {
                ImGui::Text("It's a Draw!"); 
            }
            // game->pauseGame(true); 
        }


        if (ImGui::Button("Reset Game")) {
            game = new Chess(); // Initialize a new Tic Tac Toe game
            game->setUpBoard(); // Set up the game board
            game->setAIPlayer(1); // Set AI as player 2
            gameWinner = -1; // Reset the winner to no winner
            gameGoing = true;
            // game->pauseGame(false); 
        }

        ImGui::End();

        if (game->gameHasAI() && game->getCurrentPlayer()->isAIPlayer()) {
            game->updateAI();
            // game->endTurn();
        }

        // Game window where the board and pieces are drawn
        ImGui::Begin("GameWindow");
        game->drawFrame(); // This function should handle drawing the game board and pieces
        ImGui::End();
    }

    //
    // Function called at the end of each turn
    //
    void EndOfTurn() {
        // Implement any end-of-turn logic you might need
        // For a PvP Chess game, you might not need to check for a winner after each turn
        // Instead, you could allow players to declare checkmate themselves and use a button to end the game
        Player *winner = game->checkForWinner(); // Check if there's a winner
        if (winner) {
            gameGoing = false;
            gameWinner = winner->playerNumber(); // Set the winner
            ImGui::Text("Winner is!");
            // game->stopGame(); // Clean up the game
        } else if (game->checkForDraw()) { // Only check for draw if there's no winner
            gameGoing = false;
            // game->stopGame(); // Clean up the game
        }
    }

}
