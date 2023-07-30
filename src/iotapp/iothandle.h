#include <memory>

namespace Aws { namespace Crt { class ApiHandle; }}

class IotHandle {
public:
    IotHandle();
    ~IotHandle();
private:
    std::shared_ptr<Aws::Crt::ApiHandle> _apiHandle;    
};
