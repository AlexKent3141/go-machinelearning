#include "BoardRenderer.h"
#include <cmath>
#include <iostream>

void BoardRenderer::Render(SDL_Surface* target)
{
    std::lock_guard<std::mutex> lk(_m);

    auto points = GetCoords(target->w, target->h);
    DrawGrid(target, points);

    if (_hasBoard)
    {
        DrawBoard(target, points);
        DrawOutput(target, points);
    }
}

void BoardRenderer::SetBoard(const Board& board)
{
    std::lock_guard<std::mutex> lk(_m);
    _hasBoard = true;
    _board->CloneFrom(board);
}

void BoardRenderer::SetOutput(const std::vector<double>& output)
{
    std::lock_guard<std::mutex> lk(_m);
    _output = output;
}

std::vector<Coord> BoardRenderer::GetCoords(int w, int h) const
{
    const double BorderSize = 0.1; // The width of the border as a proportion of the rect size.
    const int BoardSize = 19;

    std::vector<Coord> points;

    int minDim = std::min(w, h);
    int borderWidth = minDim * BorderSize;
    int squareSize = (minDim - 2*borderWidth) / (BoardSize - 1);

    short rowLeft = borderWidth;
    short rowTop = borderWidth;
    for (int r = 0; r < BoardSize; r++)
    {
        for (int c = 0; c < BoardSize; c++)
        {
            points.push_back(std::make_pair(rowLeft, rowTop));
            rowLeft += squareSize;
        }

        rowLeft = borderWidth;
        rowTop += squareSize;
    }

    return points;
}

void BoardRenderer::DrawGrid(SDL_Surface* target, const std::vector<Coord>& points) const
{
    const int BoardSize = 19;

    SDL_PixelFormat* fmt = target->format;
    const int BlackCol = SDL_MapRGB(fmt, 0, 0, 0);

    // Draw the grid lines between points.
    for (int r = 0; r < BoardSize; r++)
    {
        DrawLine(target, BlackCol, points[BoardSize*r], points[BoardSize*r + BoardSize - 1]);
    }

    for (int c = 0; c < BoardSize; c++)
    {
        DrawLine(target, BlackCol, points[c], points[BoardSize*(BoardSize-1) + c]);
    }
}

void BoardRenderer::DrawBoard(SDL_Surface* target, const std::vector<Coord>& points) const
{
    const int BoardSize = 19;

    SDL_PixelFormat* fmt = target->format;
    const int BlackCol = SDL_MapRGB(fmt, 0, 0, 0);
    const int WhiteCol = SDL_MapRGB(fmt, 255, 255, 255);

    // Draw the stones.
    Colour col;
    int rad = 0.4 * (points[1].first - points[0].first);
    for (int i = 0; i < BoardSize*BoardSize; i++)
    {
        col = _board->PointColour(i);
        if (col == Black)
        {
            DrawCircle(target, BlackCol, points[i], rad);
        }
        else if (col == White)
        {
            DrawCircle(target, WhiteCol, points[i], rad);
        }
    }
}

void BoardRenderer::DrawOutput(SDL_Surface* target, const std::vector<Coord>& points) const
{
}

// There is no built in method for drawing lines so we do it manually.
void BoardRenderer::DrawLine(SDL_Surface* target, int col, const Coord& p1, const Coord& p2) const
{
    int* pixels = (int*)target->pixels;

    double x = p2.first - p1.first;
    double y = p2.second - p1.second;
    double length = sqrt(x*x + y*y);

    double dx = x/length;
    double dy = y/length;

    x = p1.first;
    y = p1.second;

    int w = target->w;
    for (double i = 0; i < length; i++)
    {
        int ix = (int)x;
        int iy = (int)y;
        pixels[w*iy+ix] = col;
        x += dx;
        y += dy;
    }
}


// There is no built in method for drawing circles so we do it manually.
void BoardRenderer::DrawCircle(SDL_Surface* target, int col, const Coord& c, int r) const
{
    // The idea: iterate over pixels in a square containing the circle.
    // If a pixel is within radius of the circle then colour it.
    int* pixels = (int*)target->pixels;

    int cx = c.first;
    int cy = c.second;

    int dx, dy;

    int w = target->w;
    for (int x = cx - r; x <= cx + r; x++)
    {
        for (int y = cy - r; y <= cy + r; y++)
        {
            dx = x - cx;
            dy = y - cy;

            if (dx*dx + dy*dy < r*r)
            {
                pixels[w*y+x] = col;
            }
        }
    }
}
