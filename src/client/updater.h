#ifndef UPDATER_H
#define UPDATER_H

#include <vector>

#include "../common/library/socket.h"
#include "../common/library/thread.h"
#include "protocol.h"
#include "update_queue.h"

// Escucha el server y actualiza los estados de las entidades del juego.
//
class Updater : public Thread {
   private:
    ClientProtocol protocol;

   public:
    explicit Updater(Socket& socket);

    // envia la informacion actualizada al
    void run() override;

    void stop_custom() override;
};

#endif  // UPDATER_H
