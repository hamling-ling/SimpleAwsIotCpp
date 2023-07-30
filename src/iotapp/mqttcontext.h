
#include <memory>
#include <algorithm>
#include <thread>
#include <mutex>
#include <future>

#include <aws/crt/Api.h>
#include <aws/crt/StlAllocator.h>
#include <aws/crt/auth/Credentials.h>
#include <aws/crt/io/TlsOptions.h>

#include <aws/iot/MqttClient.h>

struct MqttContext {
    std::shared_ptr<Aws::Iot::MqttClient> client;
    std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> connection;

    std::promise<bool> connectionCompletedPromise;
    std::promise<void> connectionClosedPromise;
    std::promise<void> subscribeFinishedPromise;
    std::promise<void> unsubscribeFinishedPromise;

    std::mutex                              receiveMutex;
    std::function<void(const std::string&)> messageCallback;

    void onConnectionCompleted( Aws::Crt::Mqtt::MqttConnection &,
                                int                            errorCode,
                                Aws::Crt::Mqtt::ReturnCode     returnCode,
                                bool);

    void onInterrupted( Aws::Crt::Mqtt::MqttConnection &,
                        int                            error);

    void onResumed( Aws::Crt::Mqtt::MqttConnection &,
                    Aws::Crt::Mqtt::ReturnCode,    bool);

    void onDisconnect( Aws::Crt::Mqtt::MqttConnection &);

    void onMessage( Aws::Crt::Mqtt::MqttConnection &,
                    const Aws::Crt::String         &topic,
                    const Aws::Crt::ByteBuf        &byteBuf,
                    bool                           /*dup*/,
                    Aws::Crt::Mqtt::QOS            /*qos*/,
                    bool                           /*retain*/);

    void onSubAck( Aws::Crt::Mqtt::MqttConnection &,
                   uint16_t                       packetId,
                   const Aws::Crt::String         &topic,
                   Aws::Crt::Mqtt::QOS            QoS,
                   int                            errorCode);

    void onPublishComplete( Aws::Crt::Mqtt::MqttConnection &,
                            uint16_t,
                            int) ;

    void onUnsubscribeFinished( Aws::Crt::Mqtt::MqttConnection &,
                                uint16_t,
                                int);

    const char* getLastConnectionError();
    const char* getLastClientError();
    const char* errorDebugString(int errorCode);

public:
    static std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> GetClientConnectionForMQTTConnection(
        Aws::Iot::MqttClient                        *client,
        Aws::Iot::MqttClientConnectionConfigBuilder *clientConfigBuilder);

    static std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> BuildMQTTConnection(
        Aws::Iot::MqttClient *client,
        const char           *certFilePath,
        const char           *privateKeyFilePath,
        const char           *endpoint,
        const char           *caFilePath);
};
