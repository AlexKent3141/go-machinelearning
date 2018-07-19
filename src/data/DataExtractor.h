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
        SaveInputs(board);
        SaveOutputs(nextMove);
    }

    void SaveInputs(const Board& board)
    {
        inputs << board.ToString();
        inputs << std::endl;
    }

    void SaveOutputs(const Move& move)
    {
        std::string out;
        for (int i = 0; i < 19; i++) out += std::string(19, '.');

        // Invert the y.
        int x = move.Coord % 19;
        int y = move.Coord / 19;
        int invCoord = x + 19*(18 - y);
        out[invCoord] = move.Col == Black ? 'B' : 'W';

        for (int i = 18; i > 0; i--) out.insert(i*19, "\n");

        outputs << out << std::endl;
        outputs << std::endl;
    }
};

#endif // __DATA_EXTRACTOR_H__
