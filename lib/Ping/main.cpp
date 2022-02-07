

#include <list>

#include "Receiver/Receiver.h"
#include "common/extension.h"

bool initWSA() {
    WSADATA wsa;
    int32_t status = ::WSAStartup(MAKEWORD(2, 2), &wsa);
    if (status) return false;
    return true;
}

InitExtension(ExtensionInfo *) initModules(void);
