//
//  api_caller.cpp
//  AwsClientExample
//
//  Created by Tony Fruzza on 4/7/21.
//

#include "api_caller.hpp"

ApiCaller::ApiCaller(string api_ep, string uname, string passwd){
    curl_global_init(CURL_GLOBAL_ALL);

    api_gw_endpoint = api_ep;
    username = uname;
    password = passwd;
}

ApiCaller::~ApiCaller(){
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

size_t ApiCaller::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    readBuffer.append((char *)contents, realsize);
    return realsize;
}

bool ApiCaller::api_call(string route, bool authenticated, string post_data){
    bool return_val = false;
    curl = curl_easy_init();
    if(!curl){
        std::cout << "Error: Issue initilizing libcurl!" << endl;
        return return_val;
    }
    string url = "https://" + api_gw_endpoint + route;
    cout << "Request to: " << url << endl;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    if(post_data != ""){
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
    }
    if(authenticated){
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
    }
    res = curl_easy_perform(curl);
    if(res != CURLE_OK){
        return_val = true;
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    }
    curl_easy_cleanup(curl);
    return return_val;
}

bool ApiCaller::sign_in(){
    if(signed_in){
        return true;
    }
    nlohmann::json j_request = {
        {"username", username},
        {"password", password}
    };
    api_call("/create_user", false, j_request.dump());
    using json = nlohmann::json;
    json j = json::parse(readBuffer);
    try {
        if(j["auth"].get<std::string>().compare("success") != 0){
            cout << "Error: Authenication failure." << endl;
            signed_in = false;
            return false;
        }
    } catch(...) {
        signed_in = false;
        cout << "Error: unable to authenticate" << endl;
        return false;
    }
    
    cout << "Authenticated with user: '" << username << "'" << endl;
    auth_header = "Authorization: " + j["msg"].get<std::string>();
    header_list = curl_slist_append(header_list, auth_header.c_str());
    signed_in = true;
    return true;
}

bool ApiCaller::add_high_score(string game_session_id, int score){
    if(!sign_in()){
        return false;
    }
    nlohmann::json j_request = {
        {"username", username},
        {"game_id", game_session_id},
        {"score", score}
    };
    
    return api_call("/put_highscore", true, j_request.dump());
}

string ApiCaller::get_high_scores(){
    if(!sign_in()){
        return "";
    }
    clear_ret_buffer();
    api_call("/get_highscore", true);
    return readBuffer;
}

void ApiCaller::clear_ret_buffer(){
    readBuffer = "";
}
