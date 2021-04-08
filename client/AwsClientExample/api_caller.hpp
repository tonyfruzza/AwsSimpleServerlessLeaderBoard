//
//  api_caller.hpp
//  AwsClientExample
//
//  Created by Tony Fruzza on 4/7/21.
//

#ifndef api_caller_hpp
#define api_caller_hpp

#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;

static string readBuffer;

class ApiCaller {
    string auth_header;
    bool signed_in = false;
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
    string api_gw_endpoint, username, password;
    CURL *curl;
    CURLcode res;
    struct curl_slist *header_list = NULL;

    bool sign_in();
    bool api_call(string route, bool authenticated = false, string post_data = "");
    void clear_ret_buffer();
  public:
    ApiCaller(string api_ep, string uname, string passwd);
    bool add_high_score(string game_session_id, int score);
    string get_high_scores();
    ~ApiCaller();
};

#endif /* api_caller_hpp */
