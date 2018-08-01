// This program provides a way to view the output of the Go prediction neural network in realtime.
// The aim is to be able to step through an SGF file and apply the network to each board state to
// see what it predicts for the next move.

#include "BoardRenderer.h"
#include "../data/SGFParser.h"
#include "../core/Move.h"
#include "SDL/SDL.h"
#include <iostream>
#include <string>
#include <vector>

BoardRenderer renderer;
std::vector<Move> moves;
int moveIndex = 0;

void UpdateBoard(Board* board)
{
    for (int i = 0; i < moveIndex; i++)
    {
        board->MakeMove(moves[i]);
    }
}

// This method initialises the window.
void ShowWindow()
{
    // Initialise SDL.
    SDL_Init(SDL_INIT_VIDEO);

    // Set the title.
    SDL_WM_SetCaption("Neural network output test", "NN");

    // Create window.
    SDL_Surface* screen = SDL_SetVideoMode(800, 800, 0, 0);

    // Set the back colour.
    SDL_PixelFormat* fmt = screen->format;
    const int BoardCol = SDL_MapRGB(fmt, 255, 222, 173);

    // Handle input from the window.
    SDL_Event event;
    bool quit = false;
    while (!quit)
    {
        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_LEFT && moveIndex > 0)
                    {
                        --moveIndex;
                        Board board(19);
                        UpdateBoard(&board);
                        renderer.SetBoard(board);
                    }
                    else if (event.key.keysym.sym == SDLK_RIGHT && moveIndex < moves.size() - 1)
                    {
                        ++moveIndex;
                        Board board(19);
                        UpdateBoard(&board);
                        renderer.SetBoard(board);
                    }

                    break;

                case SDL_QUIT:
                    quit = true;
                    break;
            }
        }

        SDL_Flip(screen);

        SDL_FillRect(screen, NULL, BoardCol);

        // Show the current board state.
        renderer.Render(screen);

        // Wait for a short period.
        const int TimeDelay = 100;
        SDL_Delay(TimeDelay);
    }

    SDL_FreeSurface(screen);
    SDL_Quit();
}

int main(int argc, char** argv)
{
    SGFParser parser;
    parser.Parse("test.sgf");
    moves = parser.Moves();

    ShowWindow();

    return 0;
}
