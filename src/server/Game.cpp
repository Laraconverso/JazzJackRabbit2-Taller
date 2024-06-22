#include "Game.h"

#include <utility>
#include "./loader/config.h"

Game::Game(std::string name, Map map)
    : Thread("Game server"),
      status(Game_status::WAITING),
      map(std::move(map)),
      name(name),
      next_id(0) {}

// Agregado para poder parar el loop del servidor, antes de joinear este thread
void Game::stop_custom() { status = Game_status::FINISHED; }

void Game::run() {
    status = Game_status::RUNNING;
    std::chrono::steady_clock::time_point current_tick_start = std::chrono::steady_clock::now();
    int time_left = Config::get_game_time();
    std::chrono::steady_clock::time_point next_second_update = current_tick_start + std::chrono::seconds(1);

    send_initial_values();

    while (status == Game_status::RUNNING) {
        std::chrono::steady_clock::time_point current_tick_end = current_tick_start + TICK_DURATION;

        run_iteration();

        // Calculate the start time for the next tick
        current_tick_start += TICK_DURATION;

        // Sleep until the next tick
        std::this_thread::sleep_until(current_tick_end);

        if (time_left < 0) {
            status = Game_status::FINISHED;
            sendAll({Update::Update_new::create_value(0, Update::MatchEnded, 0)});
        } else if (std::chrono::steady_clock::now() >= next_second_update) {
            time_left--;
            next_second_update += std::chrono::seconds(1);
            sendAll({Update::Update_new::create_value(
                static_cast<uint16_t>(0), Update::RemainingSeconds,
                static_cast<uint8_t>(time_left))});
        }
    }

    // TODO: Send final stats
}

void Game::run_iteration() {
    std::vector<Update::Update_new> total_updates;
    std::vector<Update::Update_new> tick_updates;

    for (auto& client : clients) {
        uint8_t action = client->getReceiver().get_next_action();
        auto* player = dynamic_cast<Player*>(
            entity_pool[findEntityPositionById(client->get_id())].get());
        tick_updates = player->process_action(action, entity_pool, next_id);
        total_updates.insert(total_updates.end(), tick_updates.begin(),
                             tick_updates.end());
    }

    for (std::unique_ptr<Dynamic_entity>& entity_ptr : entity_pool) {
        tick_updates = entity_ptr->tick(map, entity_pool, next_id);
        total_updates.insert(total_updates.end(), tick_updates.begin(),
                             tick_updates.end());
    }
    sendAll(total_updates);
}



void Game::send_initial_values() {

    std::vector<Update::Update_new> creation_updates;
    std::vector<Update::Update_new> general_updates;

    std::array<Update::EntitySubtype, 3> player_subtypes = {
        Update::EntitySubtype::Jazz,
        Update::EntitySubtype::Spaz,
        Update::EntitySubtype::Lori
    };

    std::array<Update::EntitySubtype, 3> enemy_subtypes = {
        Update::EntitySubtype::Enemy1,
        Update::EntitySubtype::Enemy2,
        Update::EntitySubtype::Enemy3
    };

    std::array<Update::EntitySubtype, 5> pickup_subtypes = {
        Update::EntitySubtype::Coin, 
        Update::EntitySubtype::Carrot,
        Update::EntitySubtype::Light,
        Update::EntitySubtype::Heavy,
        Update::EntitySubtype::Power
    };

    //player
    Coordinate rand_spawn = map.get_player_spawns()[rand() % map.get_player_spawns().size()];
   
    Player player(next_id++, rand_spawn.x, rand_spawn.y, player_subtypes[rand() % player_subtypes.size()]);
    entity_pool.push_back(std::make_unique<Player>(player));

    creation_updates.push_back(Update::Update_new::create_create_entity(
        player.get_id(),
        Update::EntityType::Player,
        player.get_player_subtype()
    ));

    general_updates.push_back(Update::Update_new::create_position(
        player.get_id(), // usar mismo id que en la creación
        rand_spawn.x,
        rand_spawn.y
    ));
    

    //enemies
    Coordinate rand_enemy_spawn = map.get_enemy_spawns()[rand() % map.get_enemy_spawns().size()];
    /*
    Falta enemigos: 
    //se deberia chequear al cantidad de enemigos. Config::get_enemy_count...
    Enemy enemy(next_id++, rand_enemy_spawn.x, rand_enemy_spawn.y);
    creation_updates.push_back(Update::Update_new::create_create_entity(
        enemy.get_id(),
        Update::EntityType::Enemy,
        player_subtypes[rand() % player_subtypes.size()]
    
    ));
    general_updates.push_back(Update::Update_new::create_position(
        enemy.get_id(), // usar mismo id que en la creación
        rand_spawn.x,
        rand_spawn.y
    ));
    */
   
    //pickups
    for (const auto& pickup_spawn : map.get_items_spawns()) {
        Update::EntitySubtype pickup_subtype = pickup_subtypes[rand() % pickup_subtypes.size()];
        Pickup pickup(next_id++, pickup_spawn.x, pickup_spawn.y, (pickup_subtype));
        entity_pool.push_back(std::make_unique<Pickup>(pickup));

        creation_updates.push_back(Update::Update_new::create_create_entity(
            pickup.get_id(),
            Update::EntityType::Item,
            pickup_subtype 
        ));

        general_updates.push_back(Update::Update_new::create_position(
            pickup.get_id(),
            pickup_spawn.x,
            pickup_spawn.y
        ));
    }

    sendAll(creation_updates);
    sendAll(general_updates);
}


std::vector<Update::Update_new> Game::get_full_game_updates(){
    std::vector<Update::Update_new> updates;
    for(const auto& entity: entity_pool){
        uint16_t entity_id = entity->get_id();
        Update::EntityType entity_type;
        Update::EntitySubtype entity_subtype;
        Coordinate positiion;
        if (auto player = dynamic_cast<Player*>(entity.get())) {
            entity_type = Update::EntityType::Player;
            entity_subtype = player->get_player_subtype();
            position
        } else if (auto pickup = dynamic_cast<Pickup*>(entity.get())) {
            entity_type = Update::EntityType::Item;
            entity_subtype = pickup->get_subtype();
            //FALTA ENEMY
        // } else if (auto enemy = dynamic_cast<Enemy*>(entity.get())) {
        //     entity_type = Update::EntityType::Enemy;
        //     entity_subtype = enemy->get_subtype(); 
        }   else {
            continue;
        }

        updates.push_back(Update::Update_new::create_create_entity(
            entity_id,
            entity_type,
            entity_subtype
        ));

       
    
    }
    return updates;
}

//Otra opcion pero optimizada
/*
std::vector<Update::Update_new> Game::get_full_game_updates() {
    std::vector<Update::Update_new> updates;

    for (const auto& entity : entity_pool) {
        uint16_t entity_id = entity->get_id();
        Update::EntityType entity_type = entity->get_entity_type();
        Update::EntitySubtype entity_subtype = entity->get_entity_subtype();
        Coordinate position = entity->get_position();

        // Create creation update
        updates.push_back(Update::Update_new::create_create_entity(
            entity_id,
            entity_type,
            entity_subtype
        ));

        // Create position update
        updates.push_back(Update::Update_new::create_position(
            entity_id,
            position.x,
            position.y
        ));
    }

    return updates;
}
*/


uint16_t Game::add_player() {
    Coordinate rand_spawn = map.get_player_spawns()[rand() % map.get_player_spawns().size()];
    std::array<Update::EntitySubtype, 3> player_subtypes = {
        Update::EntitySubtype::Jazz,
        Update::EntitySubtype::Spaz,
        Update::EntitySubtype::Lori
    };
    Player player(next_id++, rand_spawn.x, rand_spawn.y, player_subtypes[rand() % player_subtypes.size()]);
    entity_pool.push_back(std::make_unique<Player>(player));

    return player.get_id();
}

uint8_t Game::get_id() { return id; }

void Game::set_id(uint8_t game_id) { this->id = game_id; }

std::string Game::get_match_name() { return this->name; }

std::string Game::get_map_name() { return this->map.get_name(); }

void Game::add_socket_for_player(uint16_t player_id, Socket socket) {
    clients.push_back(std::make_unique<Server_Client>(static_cast<int>(player_id), std::move(socket)));
    
    sendAll({Update::Update_new::create_create_entity(
        player_id, Update::EntityType::Player, Update::EntitySubtype::Jazz)});

    // Send create existing entitys to this player
    std::vector<Update::Update_new> full_updates = get_full_game_updates();
    sendAll(full_updates);
}

void Game::sendAll(std::vector<Update::Update_new> updates) {
    for (auto& i : clients) {
        i->getSender().addToQueue(updates);
    }
}

int Game::findEntityPositionById(int entity_id) {
    auto it = std::find_if(
        entity_pool.begin(), entity_pool.end(),
        [entity_id](const std::unique_ptr<Dynamic_entity>& entity) {
            return entity->get_id() == entity_id;
        });
    if (it != entity_pool.end()) {
        return std::distance(entity_pool.begin(), it);
    }
    return -1;
}
