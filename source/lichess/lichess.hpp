#pragma once
// Declaration
// I wish I didn't have to put includes in a declaration, but lichess.cpp is too reliant on std::strings and uses std::function as an argument
#include <string> 
#include <functional>
namespace lichess {
    bool authenticate(std::string user, std::string pass);
    std::string getPersonalToken();
    std::string getProfile(std::string token);
    bool makeMove(std::string gameId, std::string move, std::string token);
    bool sendChat(std::string gameId, std::string text, std::string token);
    bool abortGame(std::string gameId, std::string token);
    bool resignGame(std::string gameId, std::string token);
    bool claimVictory(std::string gameId, std::string token);
    bool sendDrawOffer(std::string gameId, std::string token);
    bool declineDrawOffer(std::string gameId, std::string token);
    bool sendTakeback(std::string gameId, std::string token);
    bool declineTakeback(std::string gameId, std::string token);
    bool createChallenge(int timeCtrl, int inc, bool rated, std::string username, std::string token);
    bool acceptChallenge(std::string gameId, std::string token);
    bool declineChallenge(std::string gameId, std::string token);
    void streamBoardState(std::string gameId, std::string token, std::function<void(std::string)> cb);
    bool createSeek(int time, int inc, std::string token);
    void createEventStream(std::string token, std::function<void(std::string)> cb);
    std::string fetchChat(std::string gameId, std::string token);
}