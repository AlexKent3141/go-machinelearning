#ifndef __DATA_EXTRACTOR_H__
#define __DATA_EXTRACTOR_H__

#include "SGFParser.h"
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
        const int PositionsPerFile = 500000;
        const int AverageMovesPerGame = 200;
        int numFilesRequired = ceil((AverageMovesPerGame * (double)numGames) / PositionsPerFile);

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
        int i = rand() % _inputs.size();
        SaveInputs(board, nextMove.Col, i);
        SaveOutputs(nextMove, i);
    }

    void SaveInputs(const Board& board, int colourToMove, int targetIndex)
    {
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

            _inputs[targetIndex] << row;
        }

        _inputs[targetIndex] << std::endl;
    }

    void SaveOutputs(const Move& move, int targetIndex)
    {
        _outputs[targetIndex] << move.Coord << std::endl;
    }
};

#endif // __DATA_EXTRACTOR_H__
