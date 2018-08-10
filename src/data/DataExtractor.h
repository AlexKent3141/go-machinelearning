#ifndef __DATA_EXTRACTOR_H__
#define __DATA_EXTRACTOR_H__

#include "SGFParser.h"
#include "../core/Board.h"
#include <cassert>
#include <fstream>
#include <string>

// This object extracts the data from a game of Go.
class DataExtractor
{
public:
    DataExtractor() = delete;

    DataExtractor(const std::string& inputDataFile, const std::string& outputDataFile)
    {
        inputs.open(inputDataFile);
        outputs.open(outputDataFile);
    }

    ~DataExtractor()
    {
        inputs.close();
        outputs.close();
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
    std::ofstream inputs, outputs;

    void Save(const Board& board, const Move& nextMove)
    {
        SaveInputs(board, nextMove.Col);
        SaveOutputs(nextMove);
    }

    void SaveInputs(const Board& board, int colourToMove)
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

            inputs << row;
        }

        inputs << std::endl;
    }

    void SaveOutputs(const Move& move)
    {
        outputs << move.Coord << std::endl;
    }
};

#endif // __DATA_EXTRACTOR_H__
