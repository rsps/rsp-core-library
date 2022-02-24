#include "graphics/messaging/Publisher.h"

namespace rsp::graphics
{

void Publisher::registerBroker(BrokerBase &arBroker)
{
    mpBroker = &arBroker;
}

} // namespace rsp::graphics
