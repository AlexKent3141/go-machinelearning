#ifndef __DATA_EXTRACTOR_H__
#define __DATA_EXTRACTOR_H__

#include "../core/Board.h"
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// This object extracts the data from a game of Go.
class DataExtractor
{
public:
    DataExtractor() = delete;

    DataExtractor(const std::string& targetDir, int numGames)
    {
        // The data extractor potentially needs to split the data over multiple files (because
        // there may not be enough RAM available to load all of the data at once when training).
        const int PositionsPerFile = 100000;
        const int AverageMovesPerGame = 200;
        int numFilesRequired = ceil((8*AverageMovesPerGame*(double)numGames) / PositionsPerFile);

        const std::string& InputFile = "input";
        const std::string& OutputFile = "output";
        const std::string& DataExt = ".dat";
        for (int i = 0; i < numFilesRequired; i++)
        {
            std::cout << targetDir + "/" + InputFile + std::to_string(i) + DataExt << std::endl;
            _inputs.push_back(std::ofstream(targetDir + "/" + InputFile + std::to_string(i) + DataExt));
            _outputs.push_back(std::ofstream(targetDir + "/" + OutputFile + std::to_string(i) + DataExt));
        }
    }

    ~DataExtractor()
    {
        for (size_t i = 0; i < _inputs.size(); i++)
        {
            _inputs[i].close();
            _outputs[i].close();
        }
    }

    void Generate(const std::vector<Move>& moves)
    {
        Board board(19);
        for (size_t i = 0; i < moves.size(); i++)
        {
            const Move& move = moves[i];
            MoveInfo info = board.CheckMove(move.Coord);
            if (info & Legal)
            {
                board.MakeMove(move);
                if (i < moves.size() - 1)
                {
                    const Move& nextMove = moves[i+1];
                    if (nextMove.Coord != PassCoord)
                    {
                        Save(board, nextMove);
                    }
                }
            }
            else
            {
                assert("Illegal move!");
            }
        }
    }

private:
    std::vector<std::ofstream> _inputs;
    std::vector<std::ofstream> _outputs;

    void Save(const Board& board, const Move& nextMove)
    {
        std::string input = GetInputString(board, nextMove.Col);
        int next = nextMove.Coord;
        for (int i = 1; i <= 8; i++)
        {
            int fileToUse = rand() % _inputs.size();
            SaveInputs(input, fileToUse);
            SaveOutputs(next, fileToUse);

            //PrintBoard(input, next);

            if (i % 4)
            {
                input = Rotate(input);
                next = Rotate(next);
            }
            else
            {
                input = Flip(input);
                next = Flip(next);
            }
        }
    }

    std::string Flip(const std::string& input) const
    {
        std::string flipped(input);
        for (int r = 0; r < 19; r++)
        {
            for (int c = 0; c < 19; c++)
            {
                int i = 19*r + c;
                flipped[i] = input[Flip(i)];
            }
        }

        return flipped;
    }

    int Flip(int loc) const
    {
        int x = loc % 19;
        int y = loc / 19;
        return 19*y + (18 - x);
    }

    std::string Rotate(const std::string& input) const
    {
        std::string rotated(input);
        for (int r = 0; r < 19; r++)
        {
            for (int c = 0; c < 19; c++)
            {
                int i = 19*r + c;
                rotated[Rotate(i)] = input[i];
            }
        }

        return rotated;
    }

    int Rotate(int loc) const
    {
        int x = loc % 19;
        int y = loc / 19;
        return 19*(18-x) + y;
    }

    std::string GetInputString(const Board& board, int colourToMove)
    {
        std::string inputs;
        for (int r = 0; r < 19; r++)
        {
            std::string row;
            for (int c = 0; c < 19; c++)
            {
                Colour col = board.PointColour(19*r+c);
                row += col == colourToMove ? 'P'
                    : col != None ? 'O'
                    : (board.CheckMove(19*r + c) & Legal) ? '.'
                    : 'x';
            }

            inputs += row;
        }

        return inputs;
    }

    void SaveInputs(const std::string& input, int targetIndex)
    {
        _inputs[targetIndex] << input << std::endl;
    }

    void SaveOutputs(int coord, int targetIndex)
    {
        _outputs[targetIndex] << coord << std::endl;
    }

    void PrintBoard(const std::string& input, int next) const
    {
        std::string test(input);
        for (int i = 18; i > 0; i--)
        {
            test.insert(19*i, "\n");
        }

        std::cout << test << std::endl;
        std::cout << std::endl;
    }
};

#endif // __DATA_EXTRACTOR_H__
