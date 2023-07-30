#include <iostream>
#include <thread>
#include <string>

#include "iothandle.h"
#include "iotclient.h"

using namespace std;


static bool checkArgs(int argc) {
    if(argc - 1 != 6) {
        cerr << "need 6 args but you gave " << argc -1 << endl;
        cerr << "\tendpoint" << endl;
        cerr << "\tca file path" << endl;
        cerr << "\tcert file path" << endl;
        cerr << "\tprivate key file path" << endl;
        cerr << "\ttopic" << endl;
        cerr << "\tclient id" << endl;
        return false;
    }
    return true;
}

static void onMessageReceived(const std::string& message) {
    cout << "message received:" << message << endl;
}


int main(int argc, char *argv[])
{
    if(!checkArgs(argc)) {
        return 1;
    }
    static const char* kEndPoint       = argv[1];
    static const char* kCaFilePath     = argv[2];
    static const char* kCertFilePath   = argv[3];
    static const char* kPrivateKeyFile = argv[4];
    static const char* kTopic          = argv[5];
    static const char* kClientId       = argv[6];
    static const int   kMessageCount   = 2;
    
    IotHandle iotHandle;

    IotClient iotClient(
        kEndPoint,
        kCaFilePath,
        kCertFilePath,
        kPrivateKeyFile,
        kTopic,
        kClientId,
        onMessageReceived
    );

    if(iotClient.Setup() != 0) {
        fprintf(stderr, "mqtt setup failed\n");
        return -1;
    }

    uint32_t publishedCount = 0;
    while (publishedCount < kMessageCount)
    {
        string messagePayload = "Hello World";
        iotClient.Publish(messagePayload);
        publishedCount++;

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    cout << "enter anything to continue" << endl;
    string line;
    cin >> line;

    iotClient.Cleanup();

    return 0;
}
