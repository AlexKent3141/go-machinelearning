#ifndef __BOARD_RENDERER_H__
#define __BOARD_RENDERER_H__

#include "SDL/SDL.h"
#include "../core/Board.h"
#include <mutex>
#include <utility>
#include <vector>

typedef std::pair<int, int> Coord;

class BoardRenderer
{
public:
    // Render the board state and predictions on the surface.
    void Render(SDL_Surface*);

    // Set the board to render.
    void SetBoard(const Board&);

    // Set the predictions to render.
    void SetOutput(const std::vector<double>&);
    
private:
    std::mutex _m;
    bool _hasBoard = false;
    Board* _board = new Board(19);
    std::vector<double> _output;

    void CleanupBoard();
    std::vector<Coord> GetCoords(int, int) const;
    void DrawGrid(SDL_Surface*, const std::vector<Coord>&) const;
    void DrawBoard(SDL_Surface*, const std::vector<Coord>&) const;
    void DrawOutput(SDL_Surface*, const std::vector<Coord>&) const;

    void DrawLine(SDL_Surface*, int, const Coord&, const Coord&) const;
    void DrawCircle(SDL_Surface*, int, const Coord&, int, int bw = 0, int bcol = 0) const;

    int SmoothCol(const SDL_PixelFormat*, int, int, int, int, int, int, double) const;
};

#endif // __BOARD_RENDERER_H__
