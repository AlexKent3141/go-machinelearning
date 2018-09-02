#ifndef __MOVE_DATA_EXTRACTOR_H__
#define __MOVE_DATA_EXTRACTOR_H__

#include "../core/Board.h"
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// This object extracts the data from a game of Go.
class MoveDataExtractor : public DataExtractor
{
public:
    MoveDataExtractor(const std::string& targetDir, int numGames)
        : DataExtractor(targetDir, numGames)
    {
    }

    void Generate(const std::vector<Move>& moves, const std::string& result, const std::string& sgfFilePath)
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

#endif // __MOVE_DATA_EXTRACTOR_H__
