#include "App.h"
#include <CoreLib/OTNFile.h>

App::App()
    : Application("NAME", SDLCore::Version(1, 0)) {
}

struct Transform {
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
};

struct Stats {
    int hp;
    int mana;
    float speed;
};

struct Weapon {
    std::string name;
    int damage;
    float range;
};

struct InventoryEntry {
    int weaponIndex;
    int count;
};

struct Entity {
    std::string name;
    Transform transform;
    Stats stats;
};

template<>
void OTN::ToOTNDataType<Vector2>(OTN::OTNObjectBuilder& obj, const Vector2& value) {
    obj.SetObjectName("Vector2");
    obj.AddNames("x", "y");
    obj.AddData(value.x, value.y);
}

template<>
void OTN::ToOTNDataType<Vector3>(OTN::OTNObjectBuilder& obj, const Vector3& v) {
    obj.SetObjectName("Vector3");
    obj.AddNames("x", "y", "z");
    obj.AddData(v.x, v.y, v.z);
}

template<>
void OTN::ToOTNDataType<Transform>(OTN::OTNObjectBuilder& obj, const Transform& t) {
    obj.SetObjectName("Transform");
    obj.AddNames("position", "rotation", "scale");
    obj.AddData(t.position, t.rotation, t.scale);
}

template<>
void OTN::ToOTNDataType<Stats>(OTN::OTNObjectBuilder& obj, const Stats& s) {
    obj.SetObjectName("Stats");
    obj.AddNames("hp", "mana", "speed");
    obj.AddData(s.hp, s.mana, s.speed);
}

template<>
void OTN::ToOTNDataType<Weapon>(OTN::OTNObjectBuilder& obj, const Weapon& w) {
    obj.SetObjectName("Weapon");
    obj.AddNames("name", "damage", "range");
    obj.AddData(w.name, w.damage, w.range);
}

template<>
void OTN::ToOTNDataType<InventoryEntry>(OTN::OTNObjectBuilder& obj, const InventoryEntry& e) {
    obj.SetObjectName("InventoryEntry");
    obj.AddNames("weapon", "count");
    obj.AddData(e.weaponIndex, e.count);
}

template<>
void OTN::ToOTNDataType<Entity>(OTN::OTNObjectBuilder& obj, const Entity& e) {
    obj.SetObjectName("Entity");
    obj.AddNames("name", "transform", "stats");
    obj.AddData(e.name, e.transform, e.stats);
}

void Test_Extreme_OTN_File() {
    // ---------- Weapons ----------
    OTN::OTNObject weapons("Weapons");
    weapons.SetNames("name", "damage", "range");

    weapons.AddDataRow("Sword", 25, 1.5f);
    weapons.AddDataRow("Bow", 15, 15.0f);
    weapons.AddDataRow("Staff", 10, 12.0f);

    // ---------- Inventory ----------
    OTN::OTNObject inventory("Inventory");
    inventory.SetNames("entries");

    inventory.AddDataRow(std::vector<InventoryEntry>{
        {0, 1}, // Sword
        { 1, 20 } // Bow
    });

    inventory.AddDataRow(std::vector<InventoryEntry>{
        {0, 1}, // Sword
        { 1, 20 } // Bow
    });

    inventory.AddDataRow(std::vector<InventoryEntry>{
        {2, 1} // Staff
    });

    // ---------- Entities ----------
    OTN::OTNObject entities("Entities");
    entities.SetNames("name", "transform", "stats", "inventory");

    Transform baseTransform{
        Vector3{ 0,0,0 },
        Vector3{ 0,0,0 },
        Vector3{ 1,1,1 }
    };

    Stats playerStats{ 100, 50, 4.5f };
    Stats enemyStats{ 80, 0, 3.0f };

    entities.AddDataRow(
        "Player",
        baseTransform,
        playerStats,
        0 // Inventory index
    );

    entities.AddDataRow(
        "EnemyA",
        Transform{
            Vector3{ 10,0,5 },
            Vector3{ 0,180,0 },
            Vector3{ 1,1,1 }
        },
        enemyStats,
        1
    );

    entities.AddDataRow(
        "EnemyB",
        Transform{
            Vector3{ 10,0,5 },
            Vector3{ 0,180,0 },
            Vector3{ 1,1,1 }
        },
        enemyStats,
        1
    );

    // ---------- Scene ----------
    OTN::OTNObject scenes("Scenes");
    scenes.SetNames("name", "entities");

    scenes.AddDataRow(
        "Dungeon",
        std::vector<int>{0, 1, 2}
    );

    // ---------- Writer ----------
    OTN::OTNWriter writer;
    writer.UseDefName(true);
    writer.UseDefType(true);
    writer.UseOptimizations(false);
    writer.UseDeduplicateRows(false);

    writer.AppendObject(weapons);
    writer.AppendObject(inventory);
    writer.AppendObject(entities);
    writer.AppendObject(scenes);

    if (!writer.Save("J:/extreme_complex_test.otn")) {
        Log::Error(writer.GetError());
    }
}

void App::OnStart() {
    CreateWindow(&m_winID, "window", 800, 800);

    Test_Extreme_OTN_File();

    int i;
}

void App::OnUpdate() {
    // Quit if no windows remain
    if (GetWindowCount() <= 0)
        Quit();

    if (!m_winID.IsInvalid()) {
        using namespace SDLCore;
        Input::SetWindow(m_winID);

        // =====================
        // Input/Calculations here ...
        // =====================

        namespace RE = SDLCore::Render;
        RE::SetWindowRenderer(m_winID);
        RE::SetColor(0);
        RE::Clear();

        // =====================
        // Render code here ...
        // =====================

        RE::Present();

        if (Input::KeyJustPressed(KeyCode::ESCAPE))
            DeleteWindow(m_winID);
    }
}

struct car {
    std::string name = "";
    int numberOfWheels = 0;
};

void App::OnQuit() {
}