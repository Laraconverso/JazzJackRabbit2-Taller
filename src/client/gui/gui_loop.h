#ifndef GUILOOP_H
#define GUILOOP_H

#define FPS 60
#define SECOND_IN_NANO 1000000000

#define TICK_DURATION (std::chrono::nanoseconds{(SECOND_IN_NANO / FPS)})

#define RENDER_DURATION (std::chrono::nanoseconds{((SECOND_IN_NANO / FPS) / 2)})

#include <chrono>

#include "../../common/library/thread.h"
#include "estado_juego.h"
#include "render.h"

class GuiLoop : public Thread {
    std::chrono::steady_clock clock;

    Window& windowForRender;
    Render* render = nullptr;

    UpdatableGameState2 gameState;
    UpdatableGameState updatableGameState;

    int mainId;

    uint32_t currentTick;

   public:
    // GuiLoop();
    explicit GuiLoop(Window& window);
    ~GuiLoop();

   private:
    void run() override;

    void initializeRender();

    void stop_custom() override;

    void runRenderer(MapInfo& mapInfo);

    void updateGameState();
};

#endif
