#include "NetworkBase.h"

#include "TcpSocket.h"

NetworkBase::NetworkBase()
{
    // TODO: Throw if unable to initialize.
    InitialiseWSA();
}
