#ifndef __SGF_PARSER_H__
#define __SGF_PARSER_H__

#include "../core/Move.h"
#include <vector>

class SGFParser
{
public:
    SGFParser();

    bool Parse(const std::string&);

    int BoardSize() const { return _boardSize; }
    std::vector<Move> Moves() const { return _moves; }
    std::string Result() const { return _result; }

private:
    const std::string KeyBlackMove = "B";
    const std::string KeyWhiteMove = "W";
    const std::string KeyPlayerBlack = "PB";
    const std::string KeyPlayerWhite = "PW";
    const std::string KeyBlackRank = "BR";
    const std::string KeyWhiteRank = "WR";
    const std::string KeyBlackTeam = "BT";
    const std::string KeyWhiteTeam = "WT";
    const std::string KeyAddBlack = "AB";
    const std::string KeyAddWhite = "AW";
    const std::string KeyApplication = "AP";
    const std::string KeyComment = "C";
    const std::string KeyDate = "DT";
    const std::string KeyEvent = "EV";
    const std::string KeyFileFormatVersion = "FF";
    const std::string KeyCommentary = "GC";
    const std::string KeyGameType = "GM";
    const std::string KeyHandicap = "HA";
    const std::string KeyKomi = "KM";
    const std::string KeyPlace = "PC";
    const std::string KeyResult = "RE";
    const std::string KeyRound = "RO";
    const std::string KeyRulesUsed = "RU";
    const std::string KeySize = "SZ";
    const std::string KeyTimeLimit = "TM";
    const std::string KeyUser = "US";

    int _boardSize;
    int _gameType;
    int _handicap;
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
    std::string _application;

    std::vector<Move> _moves;
    std::vector<std::string> _comments;
    std::vector<std::string> _whitePlacements;
    std::vector<std::string> _blackPlacements;

    std::string RemoveBrackets(const std::string&) const;
    std::string RemoveLineBreaks(const std::string&) const;
    void ParseKeyValuePair(const std::string&, const std::string&);
    Move ParseMove(Colour, const std::string&) const;
    bool IsKey(const std::string&) const;
};

#endif // __SGF_PARSER_H__
