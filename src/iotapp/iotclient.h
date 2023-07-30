#include <memory>
#include <string>
#include <functional>

struct MqttContext;

class IotClient {
public:
    IotClient(
        const char* endPoint,
        const char* caFilePath,
        const char* certFilePath,
        const char* privateKeyFilePath,
        const char* topic,
        const char* clientId,
        const std::function<void(const std::string&)>& callback
    );
    
    virtual ~IotClient();
    int Setup();
    void Cleanup();
    void Publish(std::string& payload);

private:
    const std::string _endPoint;
    const std::string _caFilePath;
    const std::string _certFilePath;
    const std::string _privateKeyFilePath;
    const std::string _topic;
    const std::string _clientId;
    
    std::shared_ptr<MqttContext> _context;
    const std::function<void(const std::string&)>& _messageCallback;

    // for debug
    int _messageDebug;
};
