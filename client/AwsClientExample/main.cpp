#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include "api_caller.hpp"


/*
    JSON lib used: https://github.com/nlohmann/json/tree/v3.9.1
    For OS X install JSON lib
    brew tap nlohmann/json
    brew install nlohmann-json

*/

using namespace std;

static string USER_NAME ("tester");     // default value
static string PASSSWORD ("testing123"); // default value
static string APIGW_ENDPOINT ("sn4h6yk8gg.execute-api.us-west-2.amazonaws.com/dev"); // default value

int main(int argc, char** argv) {
    srand((int)time(NULL));
    int opt;
    bool endpoint_set = false;
    
    while((opt = getopt(argc, argv, "e:u:p:")) != -1){
        switch(opt){
            case 'e':
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
                cout << "Usage: " << argv[0] << " -e API_GW_BASE_ENDPOINT example: " << APIGW_ENDPOINT << endl;
                cout << "        -u USERNAME -p PASSWORD" << endl;
                return -1;
        }
    }
    if(!endpoint_set){
        cout << "Usage: " << argv[0] << " -e API_GW_BASE_ENDPOINT example: " << APIGW_ENDPOINT << endl;
        return -1;
    }
    
    auto ac = new ApiCaller(APIGW_ENDPOINT, USER_NAME, PASSSWORD);
    
    stringstream ss;
    ss << "bestgame_" << rand() % 1000;
    string game_id = ss.str();
    int new_high_score = rand() % 2400 + 1;
    cout << "Adding new high score of: " << new_high_score << endl;
    ac->add_high_score(game_id, new_high_score);
    cout << "High score values: " << ac->get_high_scores() << endl;
    return 0;
}
