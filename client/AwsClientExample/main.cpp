#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include <curl/curl.h>

/*
    JSON lib used: https://github.com/nlohmann/json/tree/v3.9.1
    For OS X install JSON lib
    brew tap nlohmann/json
    brew install nlohmann-json

*/
#include <nlohmann/json.hpp>

using namespace std;

static string USER_NAME ("tester");     // default value
static string PASSSWORD ("testing123"); // default value
static string APIGW_ENDPOINT ("sn4h6yk8gg.execute-api.us-west-2.amazonaws.com/dev"); // default value
static string readBuffer;
static string auth_header;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    readBuffer.append((char *)contents, realsize);
    return realsize;
}

int sign_in(string user, string password){
    CURL *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    int return_val = 1;
    nlohmann::json j_request = {
        {"username", user},
        {"password", password}
    };
    string login_request = j_request.dump();
    if(curl){
        string url = "https://" + APIGW_ENDPOINT + "/create_user";
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, login_request.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK){
            return_val = 0;
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                  curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return return_val;
}

int add_high_score(string game_session_id, int score){
    CURL *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    struct curl_slist *list = NULL;
    int return_val = 1;
    
    nlohmann::json j_request = {
        {"username", USER_NAME},
        {"game_id", game_session_id},
        {"score", score}
    };
    
    string score_request = j_request.dump();
    if(curl){
        string url = "https://" + APIGW_ENDPOINT + "/put_highscore";
        list = curl_slist_append(list, auth_header.c_str());
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, score_request.c_str());
        res = curl_easy_perform(curl);
        if(res != CURLE_OK){
            return_val = 0;
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                  curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return return_val;
}

int get_high_scores(){
    CURL *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    struct curl_slist *list = NULL;
    int return_val = 1;
    
    if(curl){
        string url = "https://" + APIGW_ENDPOINT + "/get_highscore";
        list = curl_slist_append(list, auth_header.c_str());
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK){
            return_val = 0;
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                  curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return return_val;
}

int main(int argc, char** argv) {
    srand((int)time(NULL));
    int opt;
    bool endpoint_set = false;
    
    while((opt = getopt(argc, argv, "e:u:p:")) != -1){
        switch(opt){
            case 'e':
                cout << optarg << endl;
                APIGW_ENDPOINT = optarg;
                endpoint_set = true;
                break;
            case 'u':
                USER_NAME = optarg;
                break;
            case 'p':
                PASSSWORD = optarg;
                break;
            default:
                cout << "Usage: " << argv[0] << " -e API_GW_BASE_ENDPOINT example: sn4h6yk8gg.execute-api.us-west-2.amazonaws.com/dev" << endl;
                cout << "        -u USERNAME -p PASSWORD" << endl;
                return -1;
        }
    }
    if(!endpoint_set){
        cout << "Usage: " << argv[0] << " -e API GW BASE ENDPOINT example: sn4h6yk8gg.execute-api.us-west-2.amazonaws.com/dev" << endl;
        return -1;
    }
    
    sign_in(USER_NAME, PASSSWORD);
    using json = nlohmann::json;
    json j = json::parse(readBuffer);
    if(j["auth"].get<std::string>().compare("success") != 0){
        cout << "Error: Authenication failure." << endl;
        return -1;
    }
    cout << "Authenticated with user: '" << USER_NAME << "'" << endl;
    auth_header = "Authorization: " + j["msg"].get<std::string>();
    stringstream ss;
    ss << "bestgame_" << rand() % 1000;
    string game_id = ss.str();
    int new_high_score = rand() % 2400 + 1;
    cout << "Adding new high score of: " << new_high_score << endl;
    add_high_score(game_id, new_high_score);
    ss.str(string()); // clear this stringstream for reuse.
    readBuffer = "";
    get_high_scores();
    cout << "High score values: " << readBuffer << endl;
    return 0;
}
