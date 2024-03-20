#include "Application.h"
#include "imgui/imgui.h"
#include "classes/Chess.h"

namespace ClassGame {
        //
        // our global variables
        //
        Chess *game = nullptr;
        int gameWinner = -1; 
        bool gameOver = false;
        bool selectedColor = false;
        int AIPlayer = 1;

        //
        // game starting point
        // this is called by the main render loop in main.cpp
        //
        void GameStartUp() 
        {
            game = new Chess();
            game->setUpBoard();
            gameWinner = -1;
        }

        //
        // game render loop
        // this is called by the main render loop in main.cpp
        //
        void RenderGame() 
        {
#if not defined(UCI_INTERFACE)

            if (!selectedColor)
            {
                AIPlayer = 1;
                selectedColor = true;
                game = new Chess();
                game->_gameOptions.AIPlayer = AIPlayer;
                game->setUpBoard();
            }
            if (game->gameHasAI() && game->getCurrentPlayer()->playerNumber() == game->getAIPlayer())
            {
                game->updateAI();
            }
            game->drawFrame();
#else
                ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

                ImGui::Begin("Settings");
                //ImGui::Text("Current Player Number: %d", game->getCurrentPlayer()->playerNumber());

                ImGui::Text("Current Board State: %s", game->stateString().c_str());
                if (game->checkForDraw())
                {
                    ImGui::Text("Game Over!");
                    ImGui::Text("Draw!");
                }
                else
                {
                    Player* win = game->checkForWinner();
                    if(win != nullptr){
                        gameWinner = win->playerNumber();
                    }
                    if (gameWinner != -1) {
                        ImGui::Text("Game Over!");
                        if(gameWinner == 0){
                            ImGui::Text("Winner: Player 1(White)");
                        } else {
                            ImGui::Text("Winner: Player 2(Black)");
                        }
                    }
                }
                if (ImGui::Button("Reset Game")) {
                    game->stopGame();
                    game->setUpBoard();
                    gameWinner = -1;
                }
                ImGui::End();

                if (game->gameHasAI() && game->getCurrentPlayer()->playerNumber() == game->getAIPlayer())
                {
                    game->updateAI();
                }

                ImGui::Begin("GameWindow");
                game->drawFrame();
                ImGui::End();
#endif
        }

        //
        // end turn is called by the game code at the end of each turn
        // this is where we check for a winner
        //
        void EndOfTurn() 
        {
            std::cout << "End of turn" << std::endl;
            Player *winner = game->checkForWinner();
            if (winner)
            {
                gameWinner = winner->playerNumber();
            }
        }
}
