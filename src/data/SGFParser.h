#ifndef __SGF_PARSER_H__
#define __SGF_PARSER_H__

#include "../core/Move.h"
#include <vector>

class SGFParser
{
public:
    SGFParser();

    void Parse(const std::string&);

    int BoardSize() const { return _boardSize; }
    std::vector<Move> Moves() const { return _moves; }

private:
    const std::string KeySize = "SZ";
    const std::string KeyFileFormatVersion = "FF";
    const std::string KeyPlayerBlack = "PB";
    const std::string KeyPlayerWhite = "PW";
    const std::string KeyBlackRank = "BR";
    const std::string KeyWhiteRank = "WR";
    const std::string KeyEvent = "EV";
    const std::string KeyRound = "RO";
    const std::string KeyDate = "DT";
    const std::string KeyPlace = "PC";
    const std::string KeyKomi = "KM";
    const std::string KeyResult = "RE";
    const std::string KeyUser = "US";
    const std::string KeyBlackMove = "B";
    const std::string KeyWhiteMove = "W";
    const std::string KeyWhiteTeam = "WT";
    const std::string KeyBlackTeam = "BT";
    const std::string KeyRulesUsed = "RU";
    const std::string KeyTimeLimit = "TM";
    const std::string KeyCommentary = "GC";

    int _boardSize;
    std::string _fileFormatVersion;
    std::string _blackPlayer, _whitePlayer;
    std::string _blackRank, _whiteRank;
    std::string _event;
    std::string _round;
    std::string _date;
    std::string _place;
    double _komi;
    std::string _result;
    std::string _user;
    std::string _whiteTeam, _blackTeam;
    std::string _rulesUsed;
    std::string _timeLimit;
    std::string _commentary;

    std::vector<Move> _moves;

    std::string RemoveBrackets(const std::string&) const;
    void ParseKeyValuePair(const std::string&, const std::string&);
    Move ParseMove(Colour, const std::string&) const;
};

#endif // __SGF_PARSER_H__
