#include "mqttcontext.h"

void MqttContext::onConnectionCompleted( Aws::Crt::Mqtt::MqttConnection &,
                                         int                            errorCode,
                                         Aws::Crt::Mqtt::ReturnCode     returnCode,
                                         bool) {
    if (errorCode)
    {
        fprintf(stdout, "Connection failed with error %s\n", Aws::Crt::ErrorDebugString(errorCode));
        connectionCompletedPromise.set_value(false);
    }
    else
    {
        fprintf(stdout, "Connection completed with return code %d\n", returnCode);
        connectionCompletedPromise.set_value(true);
    }
}

void MqttContext::onInterrupted( Aws::Crt::Mqtt::MqttConnection &,
                    int                                         error) {
    fprintf(stdout, "Connection interrupted with error %s\n", Aws::Crt::ErrorDebugString(error));
}

void MqttContext::onResumed( Aws::Crt::Mqtt::MqttConnection &,
                             Aws::Crt::Mqtt::ReturnCode,    bool) {
    fprintf(stdout, "Connection resumed\n");
}

void MqttContext::onDisconnect( Aws::Crt::Mqtt::MqttConnection &) {
    fprintf(stdout, "Disconnect completed\n");
    connectionClosedPromise.set_value();
}

void MqttContext::onMessage( Aws::Crt::Mqtt::MqttConnection &,
                             const Aws::Crt::String         &topic,
                             const Aws::Crt::ByteBuf        &byteBuf,
                             bool                           /*dup*/,
                             Aws::Crt::Mqtt::QOS            /*qos*/,
                             bool                           /*retain*/) {
    std::string message((char*)byteBuf.buffer, byteBuf.len);
    messageCallback(message);
}

void MqttContext::onSubAck( Aws::Crt::Mqtt::MqttConnection &,
                            uint16_t                       packetId,
                            const Aws::Crt::String         &topic,
                            Aws::Crt::Mqtt::QOS            QoS,
                            int                            errorCode) {
    if (errorCode)
    {
        fprintf(stderr, "Subscribe failed with error %s\n", aws_error_debug_str(errorCode));
        exit(-1);
    }
    else
    {
        if (!packetId || QoS == AWS_MQTT_QOS_FAILURE)
        {
            fprintf(stderr, "Subscribe rejected by the broker.");
            exit(-1);
        }
        else
        {
            fprintf(stdout, "Subscribe on topic %s on packetId %d Succeeded\n", topic.c_str(), packetId);
        }
    }
    subscribeFinishedPromise.set_value();
}

void MqttContext::onPublishComplete( Aws::Crt::Mqtt::MqttConnection &,
                                     uint16_t,
                                     int) {
    printf("onPublishComplete\n");
}

void MqttContext::onUnsubscribeFinished( Aws::Crt::Mqtt::MqttConnection &,
                                         uint16_t,
                                         int) {
    unsubscribeFinishedPromise.set_value();
}

const char *MqttContext::getLastConnectionError()
{
    return Aws::Crt::ErrorDebugString( connection->LastError());
}

const char *MqttContext::getLastClientError()
{
    return Aws::Crt::ErrorDebugString( client->LastError());
}

const char *MqttContext::errorDebugString(int errorCode)
{
    return Aws::Crt::ErrorDebugString(errorCode);
}

std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> MqttContext::GetClientConnectionForMQTTConnection(
    Aws::Iot::MqttClient                        *client,
    Aws::Iot::MqttClientConnectionConfigBuilder *clientConfigBuilder)
{
    auto clientConfig = clientConfigBuilder->Build();
    if (!clientConfig)
    {
        fprintf( stderr,
                 "Client Configuration initialization failed with error %s\n",
                 Aws::Crt::ErrorDebugString(clientConfig.LastError()));
        return nullptr;
    }

    auto connection = client->NewConnection(clientConfig);
    if (!*connection)
    {
        fprintf( stderr,
                 "MQTT Connection Creation failed with error %s\n",
                 Aws::Crt::ErrorDebugString(connection->LastError()));
        return nullptr;
    }
    return connection;
}

std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> MqttContext::BuildMQTTConnection(
    Aws::Iot::MqttClient *client,
    const char           *certFilePath,
    const char           *privateKeyFilePath,
    const char           *endpoint,
    const char           *caFilePath
    )
{
    auto builder = Aws::Iot::MqttClientConnectionConfigBuilder( certFilePath,
                                                                privateKeyFilePath);
    builder.WithEndpoint(endpoint);
    builder.WithCertificateAuthority(caFilePath);

    return GetClientConnectionForMQTTConnection(client, &builder);
}
