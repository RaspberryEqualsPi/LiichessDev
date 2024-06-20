// Implementation
#include <curl/curl.h>
#include <wiisocket.h>
#include <functional>
#include <string>
#include <vector>
#include <sstream>
size_t writeFunction(void* ptr, size_t size, size_t nmemb, void* data) {
    //printf("|%i, %s", size*nmemb, std::string((char*)ptr, size*nmemb).c_str());
    ((std::string*)data)->append((char*)ptr, size * nmemb);
    return size * nmemb;
}
size_t streamWriteFunction(void* ptr, size_t size, size_t nmemb, void* data) {
    //printf("|%i, %s", size*nmemb, std::string((char*)ptr, size*nmemb).c_str());
    if (size*nmemb > 1){ // don't send the timeout avoiding newlines
        std::function<void(std::string)> cb = *((std::function<void(std::string)>*)data); // yummy
        std::string res((char*)ptr, size*nmemb);
        cb(res);
    }
    return size * nmemb;
}
namespace lichess{
    bool authenticate(std::string user, std::string pass){ // returns whether the login was successful or not
        auto curl = curl_easy_init();
        std::string response_string;
        std::string header_string;
        if (curl) {
            struct curl_slist *chunk = NULL;
 
            chunk = curl_slist_append(chunk, "Accept-Language: en-US,en;q=0.9");
            chunk = curl_slist_append(chunk, "Sec-Ch-Ua: \"Not.A/Brand\";v=\"8\", \"Chromium\";v=\"114\", \"Google Chrome\";v=\"114\"");
            chunk = curl_slist_append(chunk, "Sec-Ch-Ua-Mobile: ?0");
            chunk = curl_slist_append(chunk, "Sec-Ch-Ua-Platform: \"Windows\"");
            chunk = curl_slist_append(chunk, "Sec-Fetch-Dest: empty");
            chunk = curl_slist_append(chunk, "Sec-Fetch-Mode: cors");
            chunk = curl_slist_append(chunk, "Sec-Fetch-Site: same-origin");
            chunk = curl_slist_append(chunk, "X-Requested-With: XMLHttpRequest"); // all these headers are needed because I get 403 otherwise
 
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
 
            char* u = curl_easy_escape(curl, user.c_str(), 0); // length is passed as 0 cuz im lazy (curl finds length on its own)
            char* p = curl_easy_escape(curl, pass.c_str(), 0);
            std::string postdata = "username=" + std::string(u) + "&password=" + std::string(p) + "&remember=true&token=";
            curl_easy_setopt(curl, CURLOPT_URL, "https://lichess.org/login");
            curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "sd:/LichessWii/cookies.txt");
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata.c_str());
		    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // kinda hacky but deals with "server certificate not ok" issue
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            curl_free(u);
            curl_free(p);
        }
        return response_string.find("ok") != std::string::npos && response_string.size() == 4;
    }
    std::string getPersonalToken(){
        auto curl = curl_easy_init();
        std::string response_string;
        std::string header_string;
        if (curl) {
            struct curl_slist *chunk = NULL;
 
            chunk = curl_slist_append(chunk, "Accept-Language: en-US,en;q=0.9");
            chunk = curl_slist_append(chunk, "Sec-Ch-Ua: \"Not.A/Brand\";v=\"8\", \"Chromium\";v=\"114\", \"Google Chrome\";v=\"114\"");
            chunk = curl_slist_append(chunk, "Sec-Ch-Ua-Mobile: ?0");
            chunk = curl_slist_append(chunk, "Sec-Ch-Ua-Platform: \"Windows\"");
            chunk = curl_slist_append(chunk, "Sec-Fetch-Dest: document");
            chunk = curl_slist_append(chunk, "Sec-Fetch-Mode: navigate");
            chunk = curl_slist_append(chunk, "Sec-Fetch-Site: same-origin");
            chunk = curl_slist_append(chunk, "X-Requested-With: XMLHttpRequest"); // all these headers are needed because I get 403 otherwise
 
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_easy_setopt(curl, CURLOPT_URL, "https://lichess.org/account/oauth/token/create");
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
            curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "sd:/LichessWii/cookies.txt");
            curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "sd:/LichessWii/cookies.txt");
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "description=test1&scopes%5B%5D=email%3Aread&scopes%5B%5D=preference%3Aread&scopes%5B%5D=preference%3Awrite&scopes%5B%5D=follow%3Aread&scopes%5B%5D=follow%3Awrite&scopes%5B%5D=msg%3Awrite&scopes%5B%5D=challenge%3Aread&scopes%5B%5D=challenge%3Awrite&scopes%5B%5D=challenge%3Abulk&scopes%5B%5D=tournament%3Awrite&scopes%5B%5D=puzzle%3Aread&scopes%5B%5D=racer%3Awrite&scopes%5B%5D=board%3Aplay&scopes%5B%5D=engine%3Aread&scopes%5B%5D=engine%3Awrite"); // these scopes are the necessary scopes (or planned scopes)
		    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // kinda hacky but deals with "server certificate not ok" issue
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            size_t code = response_string.find("<code>");
            size_t code1 = response_string.find("</code>"); // i am not using an xml parser just for one function
            if (code == std::string::npos || code1 == std::string::npos){
                return "";
            }
            return response_string.substr(code + 6, code1-code-6);
        }
        return "";
    }
    std::string getProfile(std::string token){
        auto curl = curl_easy_init();
        std::string response_string;
        std::string url = "https://lichess.org/api/account";
        std::string auth = "Authorization: Bearer " + token;
        if (curl) {
            struct curl_slist *chunk = NULL;
            chunk = curl_slist_append(chunk, auth.c_str());
 
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // kinda hacky but deals with "server certificate not ok" issue
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
        }
        return response_string;
    }
    bool makeMove(std::string gameId, std::string move, std::string token){
        auto curl = curl_easy_init();
        std::string response_string;
        std::string url = "https://lichess.org/api/board/game/" + gameId + "/move/" + move;
        std::string auth = "Authorization: Bearer " + token;
        if(curl) {
            struct curl_slist *chunk = NULL;
            chunk = curl_slist_append(chunk, auth.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // kinda hacky but deals with "server certificate not ok" issue
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            curl_slist_free_all(chunk);
        }
        return response_string.find("\"ok\":") != std::string::npos;
    }
    bool sendChat(std::string gameId, std::string text, std::string token){
        auto curl = curl_easy_init();
        std::string response_string;
        char* msg = curl_easy_escape(curl, text.c_str(), 0);
        std::string url = "https://lichess.org/api/board/game/" + gameId + "/chat/";
        std::string auth = "Authorization: Bearer " + token;
        std::string postfield = "room=player&text=" + std::string(msg);
        if(curl) {
            struct curl_slist *chunk = NULL;
            chunk = curl_slist_append(chunk, auth.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfield.c_str());
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // kinda hacky but deals with "server certificate not ok" issue
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);      
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            curl_slist_free_all(chunk);
        }
        curl_free(msg);
        return response_string.find("\"ok\":") != std::string::npos;
    }
    bool abortGame(std::string gameId, std::string token){
        auto curl = curl_easy_init();
        std::string response_string;
        std::string url = "https://lichess.org/api/board/game/" + gameId + "/abort";
        std::string auth = "Authorization: Bearer " + token;
        if(curl) {
            struct curl_slist *chunk = NULL;
            chunk = curl_slist_append(chunk, auth.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // kinda hacky but deals with "server certificate not ok" issue
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            curl_slist_free_all(chunk);
        }
        return response_string.find("\"ok\":") != std::string::npos;
    }
    bool resignGame(std::string gameId, std::string token){
        auto curl = curl_easy_init();
        std::string response_string;
        std::string url = "https://lichess.org/api/board/game/" + gameId + "/resign";
        std::string auth = "Authorization: Bearer " + token;
        if(curl) {
            struct curl_slist *chunk = NULL;
            chunk = curl_slist_append(chunk, auth.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // kinda hacky but deals with "server certificate not ok" issue
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            curl_slist_free_all(chunk);
        }
        return response_string.find("\"ok\":") != std::string::npos;
    }
    bool claimVictory(std::string gameId, std::string token){
        auto curl = curl_easy_init();
        std::string response_string;
        std::string url = "https://lichess.org/api/board/game/" + gameId + "/claim-victory";
        std::string auth = "Authorization: Bearer " + token;
        if(curl) {
            struct curl_slist *chunk = NULL;
            chunk = curl_slist_append(chunk, auth.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // kinda hacky but deals with "server certificate not ok" issue
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            curl_slist_free_all(chunk);
        }
        return response_string.find("\"ok\":") != std::string::npos;
    }
    bool sendDrawOffer(std::string gameId, std::string token){
        auto curl = curl_easy_init();
        std::string response_string;
        std::string url = "https://lichess.org/api/board/game/" + gameId + "/draw/yes";
        std::string auth = "Authorization: Bearer " + token;
        if(curl) {
            struct curl_slist *chunk = NULL;
            chunk = curl_slist_append(chunk, auth.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // kinda hacky but deals with "server certificate not ok" issue
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            curl_slist_free_all(chunk);
        }
        return response_string.find("\"ok\":") != std::string::npos;
    }
    bool declineDrawOffer(std::string gameId, std::string token){
        auto curl = curl_easy_init();
        std::string response_string;
        std::string url = "https://lichess.org/api/board/game/" + gameId + "/draw/no";
        std::string auth = "Authorization: Bearer " + token;
        if(curl) {
            struct curl_slist *chunk = NULL;
            chunk = curl_slist_append(chunk, auth.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // kinda hacky but deals with "server certificate not ok" issue
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            curl_slist_free_all(chunk);
        }
        return response_string.find("\"ok\":") != std::string::npos;
    }
    bool sendTakeback(std::string gameId, std::string token){
        auto curl = curl_easy_init();
        std::string response_string;
        std::string url = "https://lichess.org/api/board/game/" + gameId + "/takeback/yes";
        std::string auth = "Authorization: Bearer " + token;
        if(curl) {
            struct curl_slist *chunk = NULL;
            chunk = curl_slist_append(chunk, auth.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // kinda hacky but deals with "server certificate not ok" issue
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            curl_slist_free_all(chunk);
        }
        return response_string.find("\"ok\":") != std::string::npos;
    }
    bool declineTakeback(std::string gameId, std::string token){
        auto curl = curl_easy_init();
        std::string response_string;
        std::string url = "https://lichess.org/api/board/game/" + gameId + "/takeback/no";
        std::string auth = "Authorization: Bearer " + token;
        if(curl) {
            struct curl_slist *chunk = NULL;
            chunk = curl_slist_append(chunk, auth.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // kinda hacky but deals with "server certificate not ok" issue
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);    
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            curl_slist_free_all(chunk);
        }
        return response_string.find("\"ok\":") != std::string::npos;
    }
    bool createChallenge(int timeCtrl, int inc, bool rated, std::string username, std::string token){
        auto curl = curl_easy_init();
        std::string response_string;
        char* u = curl_easy_escape(curl, username.c_str(), 0);
        std::string url = "https://lichess.org/api/challenge/" + std::string(u);
        std::string auth = "Authorization: Bearer " + token;
        if(curl) {
            struct curl_slist *chunk = NULL;
            chunk = curl_slist_append(chunk, auth.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            std::string postdata = "rated=" + std::string((rated) ? "true" : "false") + "&clock.limit=" + std::to_string(timeCtrl * 60) + "&clock.increment=" + std::to_string(inc) + "&variant=standard";
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata.c_str());
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // kinda hacky but deals with "server certificate not ok" issue
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            curl_slist_free_all(chunk);
        }
        curl_free(u);
        return response_string.find("\"error\":") == std::string::npos;
    }
    bool acceptChallenge(std::string gameId, std::string token){
        auto curl = curl_easy_init();
        std::string response_string;
        std::string url = "https://lichess.org/api/challenge/" + gameId + "/accept";
        std::string auth = "Authorization: Bearer " + token;
        if(curl) {
            struct curl_slist *chunk = NULL;
            chunk = curl_slist_append(chunk, auth.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // kinda hacky but deals with "server certificate not ok" issue
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            curl_slist_free_all(chunk);
        }
        return response_string.find("\"ok\":") != std::string::npos;
    }
    bool declineChallenge(std::string gameId, std::string token){
        auto curl = curl_easy_init();
        std::string response_string;
        std::string url = "https://lichess.org/api/challenge/" + gameId + "/decline";
        std::string auth = "Authorization: Bearer " + token;
        if(curl) {
            struct curl_slist *chunk = NULL;
            chunk = curl_slist_append(chunk, auth.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // kinda hacky but deals with "server certificate not ok" issue
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            curl_slist_free_all(chunk);
        }
        return response_string.find("\"ok\":") != std::string::npos;
    }
    void streamBoardState(std::string gameId, std::string token, std::function<void(std::string)> cb){
        auto curl = curl_easy_init();
        std::string url = "https://lichess.org/api/board/game/stream/" + gameId;
        std::string auth = "Authorization: Bearer " + token;
        if (curl) {
            struct curl_slist *chunk = NULL;
            chunk = curl_slist_append(chunk, auth.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // kinda hacky but deals with "server certificate not ok" issue
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, streamWriteFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &cb);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            curl_slist_free_all(chunk);
        }
    }
    bool createSeek(int time, int inc, std::string token){
        auto curl = curl_easy_init();
        std::string response_string;
        std::string url = "https://lichess.org/api/board/seek";
        std::string auth = "Authorization: Bearer " + token;
        std::string tStr = std::to_string(time);
        std::string iStr = std::to_string(inc);
        if(curl) {
            struct curl_slist *chunk = NULL;
            std::string postdata = "rated=true&time=" + tStr + "&increment=" + iStr + "&variant=standard";
            chunk = curl_slist_append(chunk, auth.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata.c_str());
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // kinda hacky but deals with "server certificate not ok" issue
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            curl_slist_free_all(chunk);
        }
        return response_string.find("\"error\":") == std::string::npos;
    }
    void createEventStream(std::string token, std::function<void(std::string)> cb){
        auto curl = curl_easy_init();
        std::string url = "https://lichess.org/api/stream/event";
        std::string auth = "Authorization: Bearer " + token;
        if (curl) {
            struct curl_slist *chunk = NULL;
            chunk = curl_slist_append(chunk, auth.c_str());

            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // kinda hacky but deals with "server certificate not ok" issue
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, streamWriteFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &cb);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            curl_slist_free_all(chunk);
        }
    }
    std::string fetchChat(std::string gameId, std::string token){
        auto curl = curl_easy_init();
        std::string response_string;
        std::string url = "https://lichess.org/api/board/game/" + gameId + "/chat";
        std::string auth = "Authorization: Bearer " + token;
        if (curl) {
            struct curl_slist *chunk = NULL;
            chunk = curl_slist_append(chunk, auth.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // kinda hacky but deals with "server certificate not ok" issue
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            curl_slist_free_all(chunk);
        }
        return response_string;
    }
}