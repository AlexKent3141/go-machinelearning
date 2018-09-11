#include "SGFParser.h"
#include "../core/Utils.h"
#include <fstream>
#include <iostream>

SGFParser::SGFParser()
{
}

bool SGFParser::Parse(const std::string& sgf)
{
    bool validFile = true;
    _handicap = 0;

    std::ifstream s(sgf);

    // Get all of the data from the file.
    std::string content;
    std::string line;
    while (std::getline(s, line)) content += line;
    s.close();

    content = RemoveBrackets(content);
    content = RemoveLineBreaks(content);

    // Split by delimiters.
    const std::string Delims = ";[]";
    auto tokens = Utils::GetInstance()->Split(content, Delims);

    // Parse each key value pair.
    _moves.clear();

    std::string prevKey;
    for (const auto& token : tokens)
    {
        bool isKey = IsKey(token);

        if (!isKey)
        {
            if (prevKey == KeyAddWhite)
            {
                _whitePlacements.push_back(token);
            }
            else if (prevKey == KeyAddBlack)
            {
                _blackPlacements.push_back(token);
            }
            else
            {
                validFile &= ParseKeyValuePair(prevKey, token);
            }
        }
        else
        {
            prevKey = token;
        }

        // Not interested in handicap games.
        validFile &= _handicap == 0;
    }

    return validFile;
}

std::string SGFParser::RemoveBrackets(const std::string& s) const
{
    return s.substr(1, s.size()-2);
}

std::string SGFParser::RemoveLineBreaks(const std::string& s) const
{
    const std::string Delims = "\r\n";
    auto tokens = Utils::GetInstance()->Split(s, Delims);

    std::string res;
    for (const auto& token : tokens) res += token;

    return res;
}

bool SGFParser::ParseKeyValuePair(const std::string& key, const std::string& value)
{
    bool valid = true;
    if (value == "" || value == "(" || value == ")")
    {
        // Just skip this.
        return valid;
    }

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
        Move m = ParseMove(Black, value);
        if (m.Valid(_boardSize))
        {
            _moves.push_back(m);
        }
        else
        {
            valid = false;
        }
    }
    else if (key == KeyWhiteMove)
    {
        Move m = ParseMove(White, value);
        if (m.Valid(_boardSize))
        {
            _moves.push_back(m);
        }
        else
        {
            valid = false;
        }
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
    else if (key == KeyGameType)
    {
        _gameType = stoi(value);
    }
    else if (key == KeyHandicap)
    {
        _handicap = stoi(value);
    }
    else if (key == KeyApplication)
    {
        _application = value;
    }
    else if (key == KeyComment)
    {
        _comments.push_back(value);
    }
    else
    {
        std::cout << "Unidentified key: " << key << " " << value << std::endl;
    }

    return valid;
}

Move SGFParser::ParseMove(Colour col, const std::string& loc) const
{
    int file = loc[0] - 97;
    int rank = loc[1] - 97;
    int coord = rank*_boardSize + file;
    return {col, coord, 0};
}

// Keys consist of one or two uppercase letters.
bool SGFParser::IsKey(const std::string& key) const
{
    return !key.empty() && key.size() < 3 && isupper(key[0]) && isupper(key[key.size()-1]);
}
