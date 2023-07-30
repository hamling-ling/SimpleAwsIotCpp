#include "iotclient.h"

#include <iostream>
#include <cstdio>
#include <aws/crt/StlAllocator.h>

#include "mqttcontext.h"

using namespace std;

IotClient::IotClient( const char *endPoint,
                      const char *caFilePath,
                      const char *certFilePath,
                      const char *privateKeyFilePath,
                      const char *topic,
                      const char *clientId,
                      const std::function<void(const std::string&)>& callback)
: _endPoint(endPoint),
  _caFilePath(caFilePath),
  _certFilePath(certFilePath),
  _privateKeyFilePath(privateKeyFilePath),
  _topic(topic),
  _clientId(clientId),
  _messageCallback(callback)
{
}

IotClient::~IotClient()
{
}

int IotClient::Setup()
{
    _context = std::make_shared<MqttContext>();
    _context->client = make_shared<Aws::Iot::MqttClient>();

    if(!_context->client) {
        fprintf( stderr,
                "MQTT Client Creation failed\n");
        return -1;
    }

    Aws::Iot::MqttClient& cl = *(_context->client.get());
    if (!cl)
    {
        fprintf( stderr,
                 "MQTT Client Creation failed with error %s\n",
                _context->getLastClientError());
        return -1;
    }

    _context->connection = MqttContext::BuildMQTTConnection( _context->client.get(),
                                                             _certFilePath.c_str(),
                                                             _privateKeyFilePath.c_str(),
                                                             _endPoint.c_str(),
                                                             _caFilePath.c_str()
                                                             );
    _context->connection->OnConnectionCompleted   = std::bind( &MqttContext::onConnectionCompleted,
                                                               _context.get(),
                                                               std::placeholders::_1,
                                                               std::placeholders::_2,
                                                               std::placeholders::_3,
                                                               std::placeholders::_4);
    _context->connection->OnDisconnect            = std::bind( &MqttContext::onDisconnect,
                                                               _context.get(),
                                                               std::placeholders::_1);
    _context->connection->OnConnectionInterrupted = std::bind( &MqttContext::onInterrupted,
                                                               _context.get(),
                                                               std::placeholders::_1,
                                                               std::placeholders::_2);
    _context->connection->OnConnectionResumed     = std::bind( &MqttContext::onResumed,
                                                               _context.get(),
                                                               std::placeholders::_1,
                                                               std::placeholders::_2,
                                                               std::placeholders::_3);

    fprintf(stdout, "Connecting...\n");
    if (!_context->connection->Connect(_clientId.c_str(), false /*cleanSession*/, 1000 /*keepAliveTimeSecs*/))
    {
        fprintf(stderr, "MQTT Connection failed with error %s\n", _context->getLastConnectionError());
        return -1;
    }

    if (!_context->connectionCompletedPromise.get_future().get())
    {
        fprintf(stderr, "MconnectionCompletedPromise.get_future().get() error\n");
        return -1;
    }

    _context->messageCallback = _messageCallback;
    printf("subscribing %s\n", _topic.c_str());
    _context->connection->Subscribe( _topic.c_str(),
                                     AWS_MQTT_QOS_AT_LEAST_ONCE,
                                     std::bind( &MqttContext::onMessage,
                                                 _context.get(),
                                                 std::placeholders::_1,
                                                 std::placeholders::_2,
                                                 std::placeholders::_3,
                                                 std::placeholders::_4,
                                                 std::placeholders::_5,
                                                 std::placeholders::_6),
                                     std::bind( &MqttContext::onSubAck,
                                                 _context.get(),
                                                 std::placeholders::_1,
                                                 std::placeholders::_2,
                                                 std::placeholders::_3,
                                                 std::placeholders::_4,
                                                 std::placeholders::_5));
    _context->subscribeFinishedPromise.get_future().wait();
    printf("subscribed %s\n", _topic.c_str());

    return 0;
}

void IotClient::Cleanup()
{
    printf("unsubscribing %s\n", _topic.c_str());
    _context->connection->Unsubscribe( _topic.c_str(),
                                       std::bind( &MqttContext::onUnsubscribeFinished,
                                                  _context.get(),
                                                  std::placeholders::_1,
                                                  std::placeholders::_2,
                                                  std::placeholders::_3));

    _context->unsubscribeFinishedPromise.get_future().wait();
    printf("unsubscribed %s\n", _topic.c_str());

    if (_context->connection->Disconnect())
    {
        printf("waiting for connectionClosedPromise\n");
        _context->connectionClosedPromise.get_future().wait();
        printf("connectionClosedPromise done\n");
    }
    _context->connection = nullptr;
}

void IotClient::Publish(std::string &payload)
{
    Aws::Crt::ByteBuf payloadBuf = Aws::Crt::ByteBufFromArray((const uint8_t *)payload.data(), payload.length());

    _context->connection->Publish( _topic.c_str(),
                                   AWS_MQTT_QOS_AT_LEAST_ONCE,
                                   false,
                                   payloadBuf,
                                   std::bind( &MqttContext::onPublishComplete,
                                              _context.get(),
                                              std::placeholders::_1,
                                              std::placeholders::_2,
                                              std::placeholders::_3));
}
