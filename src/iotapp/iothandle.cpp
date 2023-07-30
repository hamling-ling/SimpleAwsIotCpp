#include "iothandle.h"
#include <aws/crt/Api.h>

IotHandle::IotHandle()
{
    _apiHandle = std::make_shared<Aws::Crt::ApiHandle>();
    _apiHandle->InitializeLogging(Aws::Crt::LogLevel::Warn, stderr);
}

IotHandle::~IotHandle()
{
    _apiHandle = nullptr;
}
