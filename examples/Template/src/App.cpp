#include "App.h"
#include <CoreLib/Random.h>
#include <CoreLib/OTNFile.h>

#include <SDLCoreLib/Profiler.h>

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

static void Test_Very_Large_OTN_File() {
    constexpr int WEAPON_COUNT = 10'000;
    constexpr int INVENTORY_COUNT = 1'000;
    constexpr int ENTITY_COUNT = 1'000;
    constexpr int SCENE_COUNT = 100;

    Random::SetSeed(1337);


    SDLCore::Debug::Profiler::Begin("Weapons");
    // ---------- Weapons ----------
    OTN::OTNObject weapons("Weapons");
    weapons.SetNames("name", "damage", "range");

    for (int i = 0; i < WEAPON_COUNT; ++i) {
        weapons.AddDataRow(
            "Weapon_" + std::to_string(i),
            Random::GetRangeNumber<int>(5, 100),
            Random::GetRangeNumber<float>(1.0f, 50.0f)
        );
    }

    SDLCore::Debug::Profiler::End("Weapons");

    SDLCore::Debug::Profiler::Begin("Inventory");
    // ---------- Inventory ----------
    OTN::OTNObject inventory("Inventory");
    inventory.SetNames("name", "entries");

    for (int i = 0; i < INVENTORY_COUNT; ++i) {
        int entryCount = Random::GetRangeNumber<int>(1, 10);

        std::vector<InventoryEntry> entries;
        entries.reserve(entryCount);

        for (int e = 0; e < entryCount; ++e) {
            entries.push_back({
                Random::GetRangeNumber<int>(0, WEAPON_COUNT - 1),
                Random::GetRangeNumber<int>(1, 50)
            });
        }

        inventory.AddDataRow("inv_" + std::to_string(i), entries);
    }
    SDLCore::Debug::Profiler::End("Inventory");

    SDLCore::Debug::Profiler::Begin("Entities");
    // ---------- Entities ----------
    OTN::OTNObject entities("Entities");
    entities.SetNames("name", "transform", "stats", "inventory");

    for (int i = 0; i < ENTITY_COUNT; ++i) {
        Transform transform{
            Vector3{
                Random::GetRangeNumber<float>(-1000.f, 1000.f),
                Random::GetRangeNumber<float>(-1000.f, 1000.f),
                Random::GetRangeNumber<float>(-1000.f, 1000.f)
            },
            Vector3{
                0.0f,
                Random::GetRangeNumber<float>(0.f, 360.f),
                0.0f
            },
            Vector3{ 1.0f, 1.0f, 1.0f }
        };

        Stats stats{
            Random::GetRangeNumber<int>(50, 500),
            Random::GetRangeNumber<int>(0, 200),
            Random::GetRangeNumber<float>(1.0f, 10.0f)
        };

        entities.AddDataRow(
            "Entity_" + std::to_string(i),
            transform,
            stats,
            Random::GetRangeNumber<int>(0, INVENTORY_COUNT - 1)
        );
    }
    SDLCore::Debug::Profiler::End("Entities");

    SDLCore::Debug::Profiler::Begin("Scenes");
    // ---------- Scenes ----------
    OTN::OTNObject scenes("Scenes");
    scenes.SetNames("name", "entities");

    for (int s = 0; s < SCENE_COUNT; ++s) {
        int entityPerScene = ENTITY_COUNT / SCENE_COUNT;

        std::vector<int> sceneEntities;
        sceneEntities.reserve(entityPerScene);

        int base = s * entityPerScene;
        for (int i = 0; i < entityPerScene; ++i) {
            sceneEntities.push_back(base + i);
        }

        scenes.AddDataRow(
            "Scene_" + std::to_string(s),
            sceneEntities
        );
    }

    SDLCore::Debug::Profiler::End("Scenes");

    SDLCore::Debug::Profiler::Begin("Writer");
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

    if (!writer.Save("J:/test.otn")) {
        Log::Error(writer.GetError());
    }
    SDLCore::Debug::Profiler::End("Writer");

    SDLCore::Debug::Profiler::PrintAndReset();
}

void App::OnStart() {
    CreateWindow(&m_winID, "window", 800, 800);

    Test_Very_Large_OTN_File();
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

void App::OnQuit() {
}