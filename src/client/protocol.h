#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include "../common/socket.h"

class ClientProtocol{
private:
    Socket socket;

public:
    ClientProtocol();

    //envia al server la tecla presionada
    void send_key_event();



};

#endif //CLIENT_PROTOCOL_H
