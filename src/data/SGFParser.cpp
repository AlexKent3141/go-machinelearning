#include "SGFParser.h"
#include "../core/Utils.h"
#include <fstream>
#include <iostream>

SGFParser::SGFParser()
{
}

void SGFParser::Parse(const std::string& sgf)
{
    std::ifstream s(sgf);

    // Get all of the data from the file.
    std::string content;
    std::string line;
    while (std::getline(s, line)) content += line;
    s.close();

    content = RemoveBrackets(content);

    // Split by delimiters.
    const std::string Delims = ";[]\r\n";
    auto tokens = Utils::GetInstance()->Split(content, Delims);

    // Parse each key value pair.
    _moves.clear();
    for (size_t i = 0; i < tokens.size()/2; i++)
    {
        ParseKeyValuePair(tokens[2*i], tokens[2*i+1]);
    }
}

std::string SGFParser::RemoveBrackets(const std::string& s) const
{
    return s.substr(1, s.size()-2);
}

void SGFParser::ParseKeyValuePair(const std::string& key, const std::string& value)
{
    if (key == KeySize)
    {
        _boardSize = stoi(value);
    }
    else if (key == KeyFileFormatVersion)
    {
        _fileFormatVersion = value;
    }
    else if (key == KeyPlayerBlack)
    {
        _blackPlayer = value;
    }
    else if (key == KeyPlayerWhite)
    {
        _whitePlayer = value;
    }
    else if (key == KeyBlackRank)
    {
        _blackRank = value;
    }
    else if (key == KeyWhiteRank)
    {
        _whiteRank = value;
    }
    else if (key == KeyEvent)
    {
        _event = value;
    }
    else if (key == KeyRound)
    {
        _round = value;
    }
    else if (key == KeyDate)
    {
        _date = value;
    }
    else if (key == KeyPlace)
    {
        _place = value;
    }
    else if (key == KeyKomi)
    {
        if (value != "?") _komi = stof(value);
    }
    else if (key == KeyResult)
    {
        _result = value;
    }
    else if (key == KeyUser)
    {
        _user = value;
    }
    else if (key == KeyBlackMove)
    {
        _moves.push_back(ParseMove(Black, value));
    }
    else if (key == KeyWhiteMove)
    {
        _moves.push_back(ParseMove(White, value));
    }
    else if (key == KeyWhiteTeam)
    {
        _whiteTeam = value;
    }
    else if (key == KeyBlackTeam)
    {
        _blackTeam = value;
    }
    else if (key == KeyRulesUsed)
    {
        _rulesUsed = value;
    }
    else if (key == KeyTimeLimit)
    {
        _timeLimit = value;
    }
    else if (key == KeyCommentary)
    {
        _commentary += value;
    }
    else
    {
        std::cout << "Unidentified key: " << key << " " << value << std::endl;
    }
}

Move SGFParser::ParseMove(Colour col, const std::string& loc) const
{
    int file = loc[0] - 97;
    int rank = loc[1] - 97;
    int coord = rank*_boardSize + file;
    return {col, coord, 0};
}
