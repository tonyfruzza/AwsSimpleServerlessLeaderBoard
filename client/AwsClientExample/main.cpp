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
#include <aws/monitoring/CloudWatchClient.h>
#include <aws/monitoring/model/PutMetricDataRequest.h>


using namespace std;

class MetricMaker {
public:
    // Based on AWS example
    // https://docs.aws.amazon.com/sdk-for-cpp/v1/developer-guide/examples-cloudwatch-publish-custom-metrics.html
    bool publish_ccu_metric(int count){
        bool ret;
        Aws::SDKOptions options;
        
        Aws::InitAPI(options);
        {
            Aws::CloudWatch::CloudWatchClient cw;
            Aws::CloudWatch::Model::Dimension dimension;
            Aws::CloudWatch::Model::MetricDatum datum;
            Aws::CloudWatch::Model::PutMetricDataRequest request;

            dimension.SetName("map");
            dimension.SetValue("capture_the_flag");

            datum.SetMetricName("CCU");
            datum.SetUnit(Aws::CloudWatch::Model::StandardUnit::None);
            datum.SetValue(count);
            datum.AddDimensions(dimension);
            
            request.SetNamespace("dev_gameoftheyear");
            request.AddMetricData(datum);

            auto outcome = cw.PutMetricData(request);
            if (!outcome.IsSuccess())
            {
                cout << "Failed to put sample metric data:" <<
                    outcome.GetError().GetMessage() << endl;
                ret = true;
            }
            else
            {
                cout << "CCU metric set to: " << count << endl;
                ret = false;
            }
        }
        Aws::ShutdownAPI(options);
        return ret;
    }
};


void collect_user_count(){
    srand((long)time(NULL));
    MetricMaker mm;
    while(1){
        mm.publish_ccu_metric(rand() % 20 + 1);
        this_thread::sleep_for(chrono::seconds(5));
    }
}


int main(int argc, const char * argv[]) {
    thread t_handler(collect_user_count);
    t_handler.join();
    return 0;
}
