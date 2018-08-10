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

    // Normalise the outputs.
    double largest = 0;
    for (auto out : _output)
    {
        if (out > largest) largest = out;
    }

    for (size_t i = 0; i < _output.size(); i++) _output[i] /= largest;
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
    SDL_PixelFormat* fmt = target->format;
    const int BlackCol = SDL_MapRGB(fmt, 0, 0, 0);
    const int BoardSize = 19;

    // Draw the grid lines between points.
    for (int i = 0; i < BoardSize; i++)
    {
        DrawLine(target, BlackCol, points[BoardSize*i], points[BoardSize*(i+1) - 1]);
        DrawLine(target, BlackCol, points[i], points[BoardSize*(BoardSize-1) + i]);
    }
}

void BoardRenderer::DrawBoard(SDL_Surface* target, const std::vector<Coord>& points) const
{
    SDL_PixelFormat* fmt = target->format;
    const int BlackCol = SDL_MapRGB(fmt, 0, 0, 0);
    const int WhiteCol = SDL_MapRGB(fmt, 255, 255, 255);
    const int BoardSize = 19;

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
            DrawCircle(target, WhiteCol, points[i], rad, 2, BlackCol);
        }
    }
}

void BoardRenderer::DrawOutput(SDL_Surface* target, const std::vector<Coord>& points) const
{
    SDL_PixelFormat* fmt = target->format;
    const int HotColR = 255;
    const int HotColG = 0;
    const int HotColB = 0;

    const int ColdColR = 0;
    const int ColdColG = 0;
    const int ColdColB = 255;

    const int BoardSize = 19;

    SDL_Surface* output = SDL_CreateRGBSurface(0, target->w, target->h, 32, 0, 0, 0, 0);
    SDL_SetAlpha(output, SDL_SRCALPHA, 150);
    SDL_SetColorKey(output, SDL_SRCCOLORKEY, SDL_MapRGB(output->format, 0, 0, 0));

    // Draw the heatmap.
    int col;
    int rad = 0.4 * (points[1].first - points[0].first);
    for (int i = 0; i < BoardSize*BoardSize; i++)
    {
        // Check that the point is empty first.
        if (_board->PointColour(i) == None)
        {
            col = SmoothCol(fmt, HotColR, HotColG, HotColB, ColdColR, ColdColG, ColdColB, _output[i]);
            DrawCircle(output, col, points[i], rad);
        }
    }

    SDL_BlitSurface(output, NULL, target, NULL);

    SDL_FreeSurface(output);
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
void BoardRenderer::DrawCircle(SDL_Surface* target, int col, const Coord& c, int r, int bw, int bcol) const
{
    // The idea: iterate over pixels in a square containing the circle.
    // If a pixel is within radius of the circle then colour it.
    int* pixels = (int*)target->pixels;

    int cx = c.first;
    int cy = c.second;

    int dx, dy;
    double d;

    int w = target->w;
    for (int x = cx - r; x <= cx + r; x++)
    {
        for (int y = cy - r; y <= cy + r; y++)
        {
            dx = x - cx;
            dy = y - cy;

            d = sqrt(dx*dx + dy*dy);
            if (d <= r)
            {
                pixels[w*y+x] = d > r-bw ? bcol : col;
            }
        }
    }
}

int BoardRenderer::SmoothCol(const SDL_PixelFormat* fmt, int r1, int g1, int b1, int r2, int g2, int b2, double s) const
{
    int r = r1*s + r2*(1-s);
    int g = g1*s + g2*(1-s);
    int b = b1*s + b2*(1-s);
    return SDL_MapRGB(fmt, r, g, b);
}
