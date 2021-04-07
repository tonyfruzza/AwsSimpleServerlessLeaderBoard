//
//  main.cpp
//  AwsClientExample
//
//  Created by Daniel Connery on 4/05/21.
//

#include <iostream>
#include <thread>
#include <time.h>
#include <chrono>
#include <stdlib.h>
#include <aws/core/Aws.h>
#include <aws/core/utils/logging/ConsoleLogSystem.h>
#include <aws/core/utils/logging/AWSLogging.h>
#include <aws/core/utils/Outcome.h>
#include <aws/apigatewayv2/ApiGatewayV2Client.h>


using namespace std;

const char USER_NAME[] = "tester";
const char PASSSWORD[] = "testing123";

#include "AwsClientSampleSettings.h"

int main(int argc, char** argv) {
    Aws::SDKOptions options;

    Aws::Utils::Logging::LogLevel logLevel{ Aws::Utils::Logging::LogLevel::Error };
    options.loggingOptions.logger_create_fn = [logLevel] {return make_shared<Aws::Utils::Logging::ConsoleLogSystem>(logLevel); };

    Aws::InitAPI(options);

    Aws::Client::ClientConfiguration clientConfig;
    clientConfig.region = REGION;    // region must be set for Cognito operations
    Aws::ApiGatewayV2::ApiGatewayV2Client(clientConfig);

    Aws::ShutdownAPI(options);
    return 0;
}
