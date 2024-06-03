#include "gui_loop.h"

#include <iostream>
#include <thread>

#include "../update_queue.h"

double get_random() {
    static int x = 0;
    double random[] = {0.90, 1.0, 1.7, 5, 2.6};
    x = (x + 1) % (sizeof(random) / sizeof(double));

    return random[x];
}

GuiLoop::GuiLoop(Window& window) : tick_actual(0), render(window) {
    EstadoJugador jugador;
    jugador.direccion = Direccion::Derecha;
    jugador.id = 1;
    jugador.posicion = Posicion{100, 200};
    jugador.puntosDeVida = 10;
    jugador.tipoPersonaje = TipoPersonaje::Jazz;
    jugador.puntaje = 0;

    estadoJuegoActualizable.agregar_jugador_principal(jugador);
};

// std::string GuiLoop::text_description() { return "GuiLoop"; }

// void GuiLoop::stop_custom() {
//     // No tengo que hacer nada, ya que el GuiLoop se va a terminar de
//     ejecutar
//     //  solo cuando termine el proximo tick, y _keep_running sea falso
// }

void GuiLoop::run() {
    using namespace std::chrono;
    time_point INICIO_ABSOLUTO = reloj.now();
    time_point inicio_tick = INICIO_ABSOLUTO;
    while (_keep_running) {
        /*

        inicio_tick     final_tick
            ↓               ↓
            |-------*-------|
                    ↑
                tiempo_actual

        */

        // // Procesado de cosas
        // {
        //     auto duracion_render = (std::chrono::nanoseconds{
        //         (int)((SECOND_IN_NANO / FPS) * get_random())});

        //     std::this_thread::sleep_for(duracion_render);
        // }

        time_point final_tick = inicio_tick + TICK_DURATION;

        actualizar_estado();

        time_point tiempo_actual = reloj.now();
        bool sobra_tiempo_en_tick = tiempo_actual <= final_tick;

        if (sobra_tiempo_en_tick) {
            // Ejecuto el renderer, solamente si todavía me sobra tiempo en el
            // tick actual.
            // En el caso de que ya haya consumido todo el tiempo del tick
            // actual, Decido ni siquiera ejecutar el renderer para , tal vez,
            // llegar al proximo tick a tiempo.
            ejecutar_renderer();
        } else {
            std::cout << "Render cancelled. ";
        }

        tiempo_actual = reloj.now();
        sobra_tiempo_en_tick = tiempo_actual <= final_tick;

        // Tengo otro if, porque podría haber sobrado tiempo para renderizar,
        // pero haberme pasado del tiempo mientras renderizaba.
        // Por lo que tengo que checkear otra vez
        if (!sobra_tiempo_en_tick) {
            // Calculo la cantidad de ticks completos que usé
            uint ticks_extras_transcurridos =
                (tiempo_actual - inicio_tick) / TICK_DURATION;

            // Agrego cantidad de ticks dropeados
            tick_actual += ticks_extras_transcurridos;

            std::cout << ticks_extras_transcurridos << " dropped ticks\n";

            // Agregado cantidad de tiempo salteado ("dropeado")
            inicio_tick += (TICK_DURATION * ticks_extras_transcurridos);
            final_tick = inicio_tick + TICK_DURATION;
        }

        inicio_tick += TICK_DURATION;
        tick_actual += 1;
        std::this_thread::sleep_until(final_tick);
    }
}

void GuiLoop::ejecutar_renderer() {
    // TODO: acá matias debería poner el llamado al renderr
    EstadoJuegoRenderer estado_para_renderer =
        estadoJuegoActualizable.obtener_estado();

    render.presentPlayer(estado_para_renderer.jugadorPrincipal);

    // std::cout << "tick: " << tick_actual << "\n";
    // std::cout << "(" << estado_para_renderer.jugadorPrincipal.posicion.x
    // << ", "
    //           << estado_para_renderer.jugadorPrincipal.posicion.y << ")"
    //           << std::endl;
}

void GuiLoop::actualizar_estado() {
    // TODO: Acá deberia codear la manera de actualizar el estado actual

    std::vector<Update> all_updates;
    std::vector<Update> update_one_tick = Update_queue::try_pop();
    // Juntar todas las updates en un único vector
    while (update_one_tick.size() > 0) {
        all_updates.insert(all_updates.end(), update_one_tick.begin(),
                           update_one_tick.end());

        update_one_tick = Update_queue::try_pop();
    }

    // aplicar de a una las updates en orden
    for (Update update : all_updates) {
        estadoJuegoActualizable.actualizar(update);
    }
}