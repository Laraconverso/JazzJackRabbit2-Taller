#ifndef COMMON_DTOS_H
#define COMMON_DTOS_H

#include <string>
#include <vector>

namespace enums_value_update {
enum Ammo_type {
    NORMAL,  // Standard bullets
    LIGHT,   // High rate of fire, low damage
    HEAVY,   // Low rate of fire, high damage
    POWER    // Lowest rate of fire, highest damage, high speed (almost like a
             // sniper)
};

enum Player_State_Enum : uint8_t {
    Idle,
    Shooting,
    Jumping,
    Falling,
    Running,
    Intoxicated,
    IntoxWalking,
    Dead,  // 3 seg hasta respawn
    TakingDamage,
    SpecialAttack
};

enum Direction : uint8_t { Right = 0, Left = 1 };
}  // namespace enums_value_update

namespace Update {
enum UpdateType : uint8_t {
    CreateEntity = 0,      // type, subtype
    Position = 1,          // x, y
    Direction = 2,         // value
    State = 3,             // value
    Health = 4,            // value
    Score = 5,             // value
    DeleteEntity = 6,      // nothing
    MatchEnded = 7,        // nothing, id irrelevante.
    RemainingSeconds = 8,  // value
    ChangeAmmoType = 9,    // value
    BulletsRemaining = 10  // value
};

enum EntityType : uint8_t { Player = 0, Bullet = 1, Enemy = 2, Item = 3 };

enum EntitySubtype : uint8_t {
    No_subtype,
    Jazz,
    Spaz,
    Lori,
    Enemy1,
    Enemy2,
    Enemy3,
    Coin,
    Weapon,
    Carrot,
    Light, 
    Heavy, 
    Power
};

struct EntityTypeAndSubtype {
    EntityType type;
    EntitySubtype subtype;
};

struct PositionFloat {
    float x;
    float y;
};

// USANDO OBJETO PRACTICAMENTE VACIO
class Update_new {
   private:
    // Estas propiedades tienen valores default para poder crear una instancia
    // "vacia". Luego se completaran las propiedades petrinentes, al tipo de
    // update utilizado.
    uint16_t id = 0;
    Update::UpdateType update_type_value = Update::CreateEntity;

    // Create entity
    Update::EntityType entity_type_value = Update::EntityType::Player;
    Update::EntitySubtype entity_subtype_value = Update::EntitySubtype::Jazz;

    // Position
    float x = 0;
    float y = 0;

    // Value
    uint8_t value = 0;

   public:
    static std::vector<Update_new> deserialize_all(std::vector<uint8_t> data);

    static Update_new deserialize(const std::vector<uint8_t> &data);

    std::vector<uint8_t> serialize() const;

    static Update_new create_create_entity(
        uint16_t id, Update::EntityType entity_type_value,
        Update::EntitySubtype entity_subtype_value);

    static Update_new create_position(uint16_t id, float x, float y);

    static Update_new create_value(uint16_t id, Update::UpdateType key,
                                   uint8_t value);

    static Update_new create_delete_entity(uint16_t id);

    static Update_new create_end_match();

    uint16_t get_id() const;

    Update::UpdateType get_update_type() const;

    uint8_t get_value() const;

    Update::EntityTypeAndSubtype get_entity_type_and_subtype() const;

    Update::PositionFloat get_position() const;

    Update::EntityType getEntityType();

    Update::EntitySubtype getEntitySubType();

    int getPositionX();

    int getPositionY();
};
}  // namespace Update

#endif  // COMMON_DTOS_H