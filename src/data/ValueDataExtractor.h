#ifndef __VALUE_DATA_EXTRACTOR_H__
#define __VALUE_DATA_EXTRACTOR_H__

#include "DataExtractor.h"
#include "SGFParser.h"
#include <iostream>
#include <cstdlib>

class ValueDataExtractor : public DataExtractor
{
public:
    ValueDataExtractor(const std::string& targetDir, int numGames)
        : DataExtractor(targetDir, numGames)
    {
    }

    void Generate(const std::vector<Move>& moves, const std::string& result, const std::string& sgfFilePath)
    {
        // First verify whether the game ended with counting.
        // The expected format in this case is: colour + margin e.g. B+0.5 or W+5.5
        std::cout << "Result: " << result << std::endl;
        if (WentToCounting(result))
        {
            // My plan is to:
            // - Make a temporary copy of the sgf file.
            // - Use GnuGo to play out the game further so position is easier to score.
            // - Call a custom scoring routine which can deal with the "easy" final position.
            const std::string& tempPath1 = "./temp1.sgf";
            const std::string& tempPath2 = "./temp2.sgf";
            CopyFile(sgfFilePath, tempPath1);
            
            const std::string& gnugoAftermath =
                "gnugo --score aftermath --capture-all-dead -l " + tempPath1 + " -o " + tempPath2;

            if (system(gnugoAftermath.c_str()) >= 0)
            {
                SGFParser parser;
                if (parser.Parse(tempPath2))
                {
                    const std::vector<Move>& aftermathMoves = parser.Moves();
                    Colour* territories = GetFinalTerritories(moves, aftermathMoves);

                    // The final ownership map is known. Use this to generate the training data.
                    Board board(19);
                    for (const Move& m : moves)
                    {
                        board.MakeMove(m);
                        Save(board, territories);
                    }

                    delete[] territories;
                }
                else
                {
                    std::cout << "Failed to parse aftermath file." << std::endl;
                }
            }
            else
            {
                std::cout << "Error occurred when running gnugo." << std::endl;
            }
        }
        else
        {
            std::cout << "Did not go to counting." << std::endl;
        }
    }

private:
    void Save(const Board& board, Colour* territories)
    {
        std::string input = GetInputString(board, board.ColourToMove());
        std::string output = GetOutputString(territories, board.ColourToMove());
        for (int i = 0; i <= 8 ; i++)
        {
            int fileToUse = rand() % _inputs.size();
            SaveInputs(input, fileToUse);
            SaveOutputs(output, fileToUse);

          //PrintBoard(input);
          //PrintBoard(output);

            if (i % 4)
            {
                input = Rotate(input);
                output = Rotate(output);
            }
            else
            {
                input = Flip(input);
                output = Flip(output);
            }
        }
    }

    void SaveInputs(const std::string& input, int targetIndex)
    {
        _inputs[targetIndex] << input << std::endl;
    }

    void SaveOutputs(const std::string& output, int targetIndex)
    {
        _outputs[targetIndex] << output << std::endl;
    }

    std::string GetOutputString(Colour* territory, Colour colourToMove)
    {
        std::string output;
        for (int r = 0; r < 19; r++)
        {
            std::string row;
            for (int c = 0; c < 19; c++)
            {
                Colour col = territory[19*r+c];
                row += col == colourToMove ? 'P'
                    : col != None ? 'O'
                    : 'x';
            }

            output += row;
        }

        return output;
    }

    Colour* GetFinalTerritories(const std::vector<Move>& gameMoves, const std::vector<Move>& aftermathMoves) const
    {
        Board board(19);
        for (const Move& m : gameMoves) board.MakeMove(m);
        for (const Move& m : aftermathMoves) board.MakeMove(m);
        return Score(board);
    }

    Colour* Score(const Board& board) const
    {
        std::cout << board.ToString() << std::endl;

        Colour* cols = new Colour[361];

        // Initially set the colours of points which have stones.
        for (int loc = 0; loc < 361; loc++) cols[loc] = board.PointColour(loc);

        // Iteratively fill in the colours of other uncoloured points.
        // A maximum of 19 iterations are required to colour all possible points.
        const int BoardSize = 19;
        int colCount[3] = {};
        for (int i = 0; i < BoardSize; i++)
        {
            for (int loc = 0; loc < 361; loc++)
            {
                if (cols[loc] == None)
                {
                    // Does only one colour neighbour this point?
                    memset(colCount, 0, 3*sizeof(int));
                    
                    int x = loc % BoardSize;
                    int y = loc / BoardSize;
                    if (x > 0)           ++colCount[cols[BoardSize*y + x-1]];
                    if (x < BoardSize-1) ++colCount[cols[BoardSize*y + x+1]];
                    if (y > 0)           ++colCount[cols[BoardSize*(y-1) + x]];
                    if (y < BoardSize-1) ++colCount[cols[BoardSize*(y+1) + x]];

                    if (colCount[Black] > 0 && colCount[White] == 0) cols[loc] = Black;
                    else if (colCount[Black] == 0 && colCount[White] > 0) cols[loc] = White;
                }
            }
        }

        PrintColours(cols);

        return cols;
    }

    void PrintBoard(const std::string& board) const
    {
        std::string out(board);
        for (int i = 18; i > 0; i--)
        {
            out.insert(19*i, "\n");
        }

        std::cout << out << std::endl;
        std::cout << std::endl;
    }

    void PrintColours(Colour* colours) const
    {
        std::string s;

        for (int r = 18; r >= 0; r--)
        {
            for (int c = 0; c < 19; c++)
            {
                Colour col = colours[19*r+c];
                s += col == Black ? 'B' : col == White ? 'W' : 'X';
            }

            s += '\n';
        }

        std::cout << s << std::endl;
        std::cout << std::endl;
    }

    void CopyFile(const std::string& source, const std::string& dest) const
    {
        std::ifstream src(source, std::ios::binary);
        std::ofstream dst(dest, std::ios::binary);
        dst << src.rdbuf();
    }

    bool WentToCounting(const std::string& result) const
    {
        return result.size() > 2 && IsDouble(result.substr(2));
    }

    Colour GetWinner(const std::string& result) const
    {
        return result[0] == 'B' ? Black
             : result[0] == 'W' ? White
             : None;
    }

    bool IsDouble(const std::string& s) const
    {
        bool isDouble = true;
        try
        {
            stod(s);
        }
        catch (...)
        {
            isDouble = false;
        }

        return isDouble;
    }

    std::string GetScores()
    {
        return "";
    }
};

#endif // __VALUE_DATA_EXTRACTOR_H__
