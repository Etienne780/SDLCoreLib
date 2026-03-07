/**
 * @file OTNFileTest.cpp
 * @brief Tests für OTNFile – inkl. vollständiger any-Typ-Unterstützung + Performance-Benchmarks
 *
 * Kompilieren & ausführen:
 *   g++ -std=c++17 -O2 OTNFile.cpp OTNFileTest.cpp -o OTNFileTest && ./OTNFileTest
 *
 * Output:
 *   - Alle Unit-Tests mit [PASS] / [FAIL] + Laufzeit in ms
 *   - Slowest-Tests-Übersicht
 *   - Benchmark-Tabelle mit rows/s, MB/s, ms
 */

#include <CoreLib/OTNFile.h>


static OTN::OTNFilePath STREAM_TEST_PATH = "J:/tmp/";
#define STREAM_TEST_FILE (STREAM_TEST_PATH / (std::string(__func__) + ".otn"))

/*
#include <algorithm>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

 // ============================================================
 //  Timing
 // ============================================================

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

static TimePoint Now() { return Clock::now(); }
static double    ElapsedMs(TimePoint a, TimePoint b) { return std::chrono::duration<double, std::milli>(b - a).count(); }

// ============================================================
//  Test-Framework  (mit Zeitmessung pro Test)
// ============================================================

static int s_passed = 0;
static int s_failed = 0;

struct TestResult {
    std::string name;
    bool        passed = true;
    double      elapsedMs = 0.0;
    std::string failMsg;
};
static std::vector<TestResult> s_testResults;

#define TEST(name) \
    static void name(); \
    struct name##_Registrar { name##_Registrar() { \
        auto _t0 = Now(); \
        bool _passed = true; std::string _msg; \
        try { name(); } \
        catch (const std::exception& e) { _passed = false; _msg = e.what(); } \
        catch (...)                     { _passed = false; _msg = "unknown exception"; } \
        double _ms = ElapsedMs(_t0, Now()); \
        s_testResults.push_back({ #name, _passed, _ms, _msg }); \
        if (_passed) { \
            std::cout << "[PASS] " #name \
                      << "  (" << std::fixed << std::setprecision(2) << _ms << " ms)\n"; \
            ++s_passed; \
        } else { \
            std::cout << "[FAIL] " #name " -> " << _msg << "\n"; \
            ++s_failed; \
        } \
    }} name##_instance; \
    static void name()

#define ASSERT_TRUE(expr) \
    do { if (!(expr)) throw std::runtime_error("ASSERT_TRUE failed: " #expr); } while(0)

#define ASSERT_FALSE(expr) \
    do { if ((expr)) throw std::runtime_error("ASSERT_FALSE failed: " #expr); } while(0)

#define ASSERT_EQ(a, b) \
    do { if (!((a) == (b))) { \
        std::ostringstream _ss; \
        _ss << "ASSERT_EQ failed: " #a " == " #b \
            << " (" << (a) << " vs " << (b) << ")"; \
        throw std::runtime_error(_ss.str()); \
    }} while(0)

#define ASSERT_NEAR(a, b, eps) \
    do { if (std::fabs((double)(a) - (double)(b)) > (eps)) { \
        std::ostringstream _ss; \
        _ss << "ASSERT_NEAR failed: |" #a " - " #b "| <= " #eps \
            << " (" << (a) << " vs " << (b) << ")"; \
        throw std::runtime_error(_ss.str()); \
    }} while(0)

// ============================================================
//  Benchmark-Infrastruktur
// ============================================================

struct BenchResult {
    std::string label;
    double      writeMs = 0;
    double      readMs = 0;
    size_t      rowCount = 0;
    size_t      byteSize = 0;   // 0 = in-memory / unbekannt

    void Print() const {
        auto rowsPerSec = [&](double ms) -> double {
            return (ms > 0) ? (rowCount / (ms / 1000.0)) : 0.0;
            };
        auto mbPerSec = [&](double ms) -> std::string {
            if (byteSize == 0 || ms <= 0) return "n/a";
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(1)
                << (byteSize / 1024.0 / 1024.0) / (ms / 1000.0) << " MB/s";
            return ss.str();
            };

        std::cout << "\n  [" << label << "]\n";
        std::cout << "    rows      : " << rowCount << "\n";
        if (byteSize > 0)
            std::cout << "    file size : " << std::fixed << std::setprecision(2)
            << (byteSize / 1024.0 / 1024.0) << " MB\n";
        std::cout << "    write     : " << std::fixed << std::setprecision(2) << writeMs
            << " ms  (" << std::fixed << std::setprecision(0) << rowsPerSec(writeMs)
            << " rows/s, " << mbPerSec(writeMs) << ")\n";
        std::cout << "    read      : " << std::fixed << std::setprecision(2) << readMs
            << " ms  (" << std::fixed << std::setprecision(0) << rowsPerSec(readMs)
            << " rows/s, " << mbPerSec(readMs) << ")\n";
    }
};

static std::vector<BenchResult> s_benchResults;

static constexpr size_t            BENCH_ROWS = 100'000;
static const std::filesystem::path BENCH_DIR = "otn_bench_tmp";
static const std::filesystem::path BENCH_FILE = BENCH_DIR / "bench.otn";

struct BenchRow { int id; float value; std::string label; bool active; };
static BenchRow MakeBenchRow(int i) {
    return { i, static_cast<float>(i) * 0.001f, "item", (i & 1) == 0 };
}

// ============================================================
//  Roundtrip-Hilfe für Unit-Tests
// ============================================================

static std::optional<OTN::OTNObject> RoundTrip(OTN::OTNObject obj) {
    OTN::OTNWriter writer;
    writer.AppendObject(obj);
    std::string otnStr;
    if (!writer.SaveToString(otnStr))
        return std::nullopt;
    OTN::OTNReader reader;
    if (!reader.ReadString(otnStr))
        return std::nullopt;
    return reader.TryGetObject(obj.GetObjectName());
}

// ============================================================
//  Custom-Typ
// ============================================================

struct Vec2 { float x = 0.f, y = 0.f; };
template<>
inline void OTN::ToOTNDataType<Vec2>(OTN::OTNObjectBuilder& b, Vec2& v) {
    b.SetObjectName("Vec2");
    b.AddNames("x", "y");
    b.AddData(v.x, v.y);
}

// ============================================================
// ============================================================
//  UNIT-TESTS
// ============================================================
// ============================================================

// ---- Primitive Typen ----

TEST(Test_Int_Roundtrip) {
    OTN::OTNObject obj("IntTest");
    obj.SetNames("id", "value")
        .AddDataRow(1, 42)
        .AddDataRow(2, -7);
    auto loaded = RoundTrip(obj);
    ASSERT_TRUE(loaded.has_value());
    ASSERT_EQ(loaded->GetRowCount(), 2u);
    ASSERT_EQ(loaded->GetValue<int>(0, "id"), 1);
    ASSERT_EQ(loaded->GetValue<int>(0, "value"), 42);
    ASSERT_EQ(loaded->GetValue<int>(1, "id"), 2);
    ASSERT_EQ(loaded->GetValue<int>(1, "value"), -7);
}

TEST(Test_Float_Roundtrip) {
    OTN::OTNObject obj("FloatTest");
    obj.SetNames("x")
        .AddDataRow(3.14f)
        .AddDataRow(-0.5f);
    auto loaded = RoundTrip(obj);
    ASSERT_TRUE(loaded.has_value());
    ASSERT_NEAR(loaded->GetValue<float>(0, "x"), 3.14f, 1e-4f);
    ASSERT_NEAR(loaded->GetValue<float>(1, "x"), -0.5f, 1e-6f);
}

TEST(Test_Double_Roundtrip) {
    OTN::OTNObject obj("DoubleTest");
    obj.SetNames("d").AddDataRow(1.23456789);
    auto loaded = RoundTrip(obj);
    ASSERT_TRUE(loaded.has_value());
    ASSERT_NEAR(loaded->GetValue<double>(0, "d"), 1.23456789, 1e-7);
}

TEST(Test_Bool_Roundtrip) {
    OTN::OTNObject obj("BoolTest");
    obj.SetNames("flag").AddDataRow(true).AddDataRow(false);
    auto loaded = RoundTrip(obj);
    ASSERT_TRUE(loaded.has_value());
    ASSERT_EQ(loaded->GetValue<bool>(0, "flag"), true);
    ASSERT_EQ(loaded->GetValue<bool>(1, "flag"), false);
}

TEST(Test_String_Roundtrip) {
    OTN::OTNObject obj("StringTest");
    obj.SetNames("name")
        .AddDataRow(std::string("Hello World"))
        .AddDataRow(std::string("Föhn & Co."));
    auto loaded = RoundTrip(obj);
    ASSERT_TRUE(loaded.has_value());
    ASSERT_EQ(loaded->GetValue<std::string>(0, "name"), "Hello World");
    ASSERT_EQ(loaded->GetValue<std::string>(1, "name"), "Föhn & Co.");
}

TEST(Test_Int64_Roundtrip) {
    OTN::OTNObject obj("Int64Test");
    obj.SetNames("big")
        .SetTypes("int64")
        .AddDataRow(static_cast<int64_t>(9'000'000'000LL))
        .AddDataRow(static_cast<int64_t>(-1'000'000'000'000LL));
    auto loaded = RoundTrip(obj);
    ASSERT_TRUE(loaded.has_value());
    ASSERT_EQ(loaded->GetValue<int64_t>(0, "big"), 9'000'000'000LL);
    ASSERT_EQ(loaded->GetValue<int64_t>(1, "big"), -1'000'000'000'000LL);
}

// ---- Listen ----

TEST(Test_IntList_Roundtrip) {
    OTN::OTNObject obj("ListTest");
    obj.SetNames("nums").AddDataRow(std::vector<int>{10, 20, 30});
    auto loaded = RoundTrip(obj);
    ASSERT_TRUE(loaded.has_value());
    auto nums = loaded->GetValue<std::vector<int>>(0, "nums");
    ASSERT_EQ(nums.size(), 3u);
    ASSERT_EQ(nums[0], 10);
    ASSERT_EQ(nums[2], 30);
}

TEST(Test_StringList_Roundtrip) {
    OTN::OTNObject obj("StrListTest");
    obj.SetNames("words").AddDataRow(std::vector<std::string>{"foo", "bar", "baz"});
    auto loaded = RoundTrip(obj);
    ASSERT_TRUE(loaded.has_value());
    auto words = loaded->GetValue<std::vector<std::string>>(0, "words");
    ASSERT_EQ(words.size(), 3u);
    ASSERT_EQ(words[1], "bar");
}

// ---- Gemischte Spalten ----

TEST(Test_Mixed_Columns) {
    OTN::OTNObject obj("Mixed");
    obj.SetNames("id", "name", "score", "active")
        .AddDataRow(1, std::string("Alice"), 99.5f, true)
        .AddDataRow(2, std::string("Bob"), 42.0f, false);
    auto loaded = RoundTrip(obj);
    ASSERT_TRUE(loaded.has_value());
    ASSERT_EQ(loaded->GetValue<int>(1, "id"), 2);
    ASSERT_EQ(loaded->GetValue<std::string>(1, "name"), "Bob");
    ASSERT_NEAR(loaded->GetValue<float>(0, "score"), 99.5f, 1e-4f);
    ASSERT_EQ(loaded->GetValue<bool>(0, "active"), true);
}

TEST(Test_GetValue_Default) {
    OTN::OTNObject obj("DefaultTest");
    obj.SetNames("x").AddDataRow(5);
    auto loaded = RoundTrip(obj);
    ASSERT_TRUE(loaded.has_value());
    ASSERT_EQ(loaded->GetValue<int>(0, "missing_column", -1), -1);
    ASSERT_EQ(loaded->GetValue<int>(99, "x", -99), -99);
}

TEST(Test_TryGetValue) {
    OTN::OTNObject obj("TryGet");
    obj.SetNames("n").AddDataRow(7);
    auto loaded = RoundTrip(obj);
    ASSERT_TRUE(loaded.has_value());
    auto found = loaded->TryGetValue<int>(0, "n");
    auto missing = loaded->TryGetValue<int>(0, "nope");
    ASSERT_TRUE(found.has_value());
    ASSERT_EQ(*found, 7);
    ASSERT_FALSE(missing.has_value());
}

TEST(Test_CustomType_Vec2) {
    OTN::OTNObject obj("Vec2Test");
    obj.SetNames("x", "y").AddDataRow(1.5f, 2.5f);
    auto loaded = RoundTrip(obj);
    ASSERT_TRUE(loaded.has_value());
    ASSERT_NEAR(loaded->GetValue<float>(0, "x"), 1.5f, 1e-5f);
    ASSERT_NEAR(loaded->GetValue<float>(0, "y"), 2.5f, 1e-5f);
}

// ---- any-Typ ----

TEST(Test_Any_Int) {
    OTN::OTNObject obj("AnyInt");
    obj.SetNames("val").SetTypes("any").AddDataRow(42);
    auto loaded = RoundTrip(obj);
    ASSERT_TRUE(loaded.has_value());
    ASSERT_EQ(loaded->GetValue<int>(0, "val"), 42);
}

TEST(Test_Any_Float) {
    OTN::OTNObject obj("AnyFloat");
    obj.SetNames("val").SetTypes("any").AddDataRow(3.14f);
    auto loaded = RoundTrip(obj);
    ASSERT_TRUE(loaded.has_value());
    ASSERT_NEAR(loaded->GetValue<float>(0, "val"), 3.14f, 1e-4f);
}

TEST(Test_Any_String) {
    OTN::OTNObject obj("AnyString");
    obj.SetNames("val").SetTypes("any").AddDataRow(std::string("hello"));
    auto loaded = RoundTrip(obj);
    ASSERT_TRUE(loaded.has_value());
    ASSERT_EQ(loaded->GetValue<std::string>(0, "val"), "hello");
}

TEST(Test_Any_Bool) {
    OTN::OTNObject obj("AnyBool");
    obj.SetNames("flag").SetTypes("any").AddDataRow(true).AddDataRow(false);
    auto loaded = RoundTrip(obj);
    ASSERT_TRUE(loaded.has_value());
    ASSERT_EQ(loaded->GetValue<bool>(0, "flag"), true);
    ASSERT_EQ(loaded->GetValue<bool>(1, "flag"), false);
}

TEST(Test_Any_NegativeInt) {
    OTN::OTNObject obj("AnyNeg");
    obj.SetNames("val").SetTypes("any").AddDataRow(-99);
    auto loaded = RoundTrip(obj);
    ASSERT_TRUE(loaded.has_value());
    ASSERT_EQ(loaded->GetValue<int>(0, "val"), -99);
}

TEST(Test_Any_NegativeFloat) {
    OTN::OTNObject obj("AnyNegF");
    obj.SetNames("val").SetTypes("any").AddDataRow(-1.5f);
    auto loaded = RoundTrip(obj);
    ASSERT_TRUE(loaded.has_value());
    ASSERT_NEAR(loaded->GetValue<float>(0, "val"), -1.5f, 1e-5f);
}

TEST(Test_Any_MixedRows) {
    OTN::OTNObject obj("AnyMixed");
    obj.SetNames("val").SetTypes("any")
        .AddDataRow(10)
        .AddDataRow(std::string("text"))
        .AddDataRow(true)
        .AddDataRow(2.718f);
    auto loaded = RoundTrip(obj);
    ASSERT_TRUE(loaded.has_value());
    ASSERT_EQ(loaded->GetRowCount(), 4u);
    ASSERT_EQ(loaded->GetValue<int>(0, "val"), 10);
    ASSERT_EQ(loaded->GetValue<std::string>(1, "val"), "text");
    ASSERT_EQ(loaded->GetValue<bool>(2, "val"), true);
    ASSERT_NEAR(loaded->GetValue<float>(3, "val"), 2.718f, 1e-3f);
}

TEST(Test_Any_List) {
    OTN::OTNObject obj("AnyList");
    obj.SetNames("nums").SetTypes("any").AddDataRow(std::vector<int>{1, 2, 3});
    auto loaded = RoundTrip(obj);
    ASSERT_TRUE(loaded.has_value());
    auto nums = loaded->GetValue<std::vector<int>>(0, "nums");
    ASSERT_EQ(nums.size(), 3u);
    ASSERT_EQ(nums[0], 1);
    ASSERT_EQ(nums[2], 3);
}

TEST(Test_Any_MultiColumn) {
    OTN::OTNObject obj("AnyMulti");
    obj.SetNames("a", "b", "c").SetTypes("any", "any", "any")
        .AddDataRow(1, std::string("x"), true)
        .AddDataRow(2, std::string("y"), false);
    auto loaded = RoundTrip(obj);
    ASSERT_TRUE(loaded.has_value());
    ASSERT_EQ(loaded->GetValue<int>(0, "a"), 1);
    ASSERT_EQ(loaded->GetValue<std::string>(0, "b"), "x");
    ASSERT_EQ(loaded->GetValue<bool>(0, "c"), true);
    ASSERT_EQ(loaded->GetValue<int>(1, "a"), 2);
}

TEST(Test_Any_MixedWithTyped) {
    OTN::OTNObject obj("AnyWithTyped");
    obj.SetNames("id", "payload", "active").SetTypes("int", "any", "bool")
        .AddDataRow(1, 3.14f, true)
        .AddDataRow(2, std::string("data"), false);
    auto loaded = RoundTrip(obj);
    ASSERT_TRUE(loaded.has_value());
    ASSERT_EQ(loaded->GetValue<int>(0, "id"), 1);
    ASSERT_NEAR(loaded->GetValue<float>(0, "payload"), 3.14f, 1e-4f);
    ASSERT_EQ(loaded->GetValue<bool>(0, "active"), true);
    ASSERT_EQ(loaded->GetValue<int>(1, "id"), 2);
    ASSERT_EQ(loaded->GetValue<std::string>(1, "payload"), "data");
}

// ---- Fehlerfälle ----

TEST(Test_Reader_FileNotFound) {
    OTN::OTNReader reader;
    ASSERT_FALSE(reader.ReadFile("this_file_does_not_exist_12345.otn"));
    ASSERT_FALSE(reader.IsValid());
}

TEST(Test_Reader_InvalidString) {
    OTN::OTNReader reader;
    ASSERT_FALSE(reader.ReadString("this is not valid otn content {{{{"));
}

TEST(Test_MultipleObjects) {
    OTN::OTNObject players("Players");
    players.SetNames("id", "name")
        .AddDataRow(1, std::string("Alice"))
        .AddDataRow(2, std::string("Bob"));
    OTN::OTNObject scores("Scores");
    scores.SetNames("player_id", "score")
        .AddDataRow(1, 100).AddDataRow(2, 200);
    OTN::OTNWriter writer;
    writer.AppendObject(players).AppendObject(scores);
    std::string otnStr;
    ASSERT_TRUE(writer.SaveToString(otnStr));
    OTN::OTNReader reader;
    ASSERT_TRUE(reader.ReadString(otnStr));
    auto pl = reader.TryGetObject("Players");
    auto sc = reader.TryGetObject("Scores");
    ASSERT_TRUE(pl.has_value());
    ASSERT_TRUE(sc.has_value());
    ASSERT_EQ(pl->GetRowCount(), 2u);
    ASSERT_EQ(sc->GetValue<int>(1, "score"), 200);
}

TEST(Test_EmptyObject_WriteFails) {
    OTN::OTNObject obj("Empty");
    obj.SetNames("x");
    OTN::OTNWriter writer;
    writer.AppendObject(obj);
    std::string otnStr;
    ASSERT_FALSE(writer.SaveToString(otnStr));
    ASSERT_FALSE(writer.IsValid());
}

TEST(Test_TypeDescToString_Any) {
    OTN::OTNTypeDesc anyDesc{ OTN::OTNBaseType::ANY };
    ASSERT_EQ(OTN::TypeDescToString(anyDesc), "any");
}

TEST(Test_Version) {
    OTN::OTNObject obj("V");
    obj.SetNames("x").AddDataRow(1);
    OTN::OTNWriter writer;
    writer.AppendObject(obj);
    std::string otnStr;
    ASSERT_TRUE(writer.SaveToString(otnStr));
    OTN::OTNReader reader;
    ASSERT_TRUE(reader.ReadString(otnStr));
    ASSERT_EQ(reader.GetVersion(), OTN::VERSION);
}

// ============================================================
//  STREAMING UNIT-TESTS
// ============================================================

TEST(Stream_Write_Read_WithOTNReader) {
    OTN::OTNStreamWriter w;
    ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
    w.BeginObject("Items", { "id", "name", "score" }, { "int", "String", "float" });
    w.WriteRow(1, std::string("Alpha"), 9.5f);
    w.WriteRow(2, std::string("Beta"), 4.2f);
    w.WriteRow(3, std::string("Gamma"), 7.7f);
    ASSERT_TRUE(w.EndObject());
    ASSERT_TRUE(w.Close());
    ASSERT_TRUE(w.IsValid());

    OTN::OTNReader reader;
    ASSERT_TRUE(reader.ReadFile(STREAM_TEST_FILE));
    auto obj = reader.TryGetObject("Items");
    ASSERT_TRUE(obj.has_value());
    ASSERT_EQ(obj->GetRowCount(), 3u);
    ASSERT_EQ(obj->GetValue<int>(0, "id"), 1);
    ASSERT_EQ(obj->GetValue<std::string>(1, "name"), "Beta");
    ASSERT_NEAR(obj->GetValue<float>(2, "score"), 7.7f, 1e-4f);
}

TEST(Stream_Read_FileWrittenByOTNWriter) {
    OTN::OTNObject obj("Data");
    obj.SetNames("x", "y").AddDataRow(10, 20).AddDataRow(30, 40);
    OTN::OTNWriter writer;
    writer.AppendObject(obj);
    ASSERT_TRUE(writer.Save(STREAM_TEST_FILE));

    OTN::OTNStreamReader r;
    ASSERT_TRUE(r.Open(STREAM_TEST_FILE));
    ASSERT_TRUE(r.HasMoreObjects());
    ASSERT_TRUE(r.NextObject());
    ASSERT_EQ(r.GetCurrentObjectName(), "Data");
    ASSERT_EQ(r.GetRemainingRowCount(), 2u);

    OTN::OTNRow row;
    ASSERT_TRUE(r.ReadRow(row));
    ASSERT_EQ(row.size(), 2u);
    ASSERT_EQ(std::get<int>(row[0].value), 10);
    ASSERT_EQ(std::get<int>(row[1].value), 20);
    ASSERT_TRUE(r.ReadRow(row));
    ASSERT_EQ(std::get<int>(row[0].value), 30);
    ASSERT_FALSE(r.HasMoreRows());
    ASSERT_FALSE(r.NextObject());
}

TEST(Stream_FullRoundtrip) {
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        w.BeginObject("Weights", { "w" }, { "float" });
        for (int i = 0; i < 5; ++i)
            w.WriteRow(static_cast<float>(i) * 0.1f);
        ASSERT_TRUE(w.EndObject());
        ASSERT_TRUE(w.Close());
    }
    OTN::OTNStreamReader r;
    ASSERT_TRUE(r.Open(STREAM_TEST_FILE));
    ASSERT_TRUE(r.NextObject());
    ASSERT_EQ(r.GetCurrentObjectName(), "Weights");
    ASSERT_EQ(r.GetRemainingRowCount(), 5u);
    float expected = 0.0f;
    OTN::OTNRow row;
    while (r.ReadRow(row)) {
        ASSERT_NEAR(std::get<float>(row[0].value), expected, 1e-5f);
        expected += 0.1f;
    }
    ASSERT_FALSE(r.NextObject());
}

TEST(Stream_MultipleObjects) {
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        w.BeginObject("A", { "n" }, { "int" });
        w.WriteRow(1); w.WriteRow(2);
        ASSERT_TRUE(w.EndObject());
        w.BeginObject("B", { "s" }, { "String" });
        w.WriteRow(std::string("hello"));
        ASSERT_TRUE(w.EndObject());
        ASSERT_TRUE(w.Close());
    }
    OTN::OTNStreamReader r;
    ASSERT_TRUE(r.Open(STREAM_TEST_FILE));
    ASSERT_TRUE(r.NextObject());
    ASSERT_EQ(r.GetCurrentObjectName(), "A");
    ASSERT_EQ(r.GetRemainingRowCount(), 2u);
    OTN::OTNRow row;
    ASSERT_TRUE(r.ReadRow(row)); ASSERT_EQ(std::get<int>(row[0].value), 1);
    ASSERT_TRUE(r.ReadRow(row)); ASSERT_EQ(std::get<int>(row[0].value), 2);
    ASSERT_TRUE(r.NextObject());
    ASSERT_EQ(r.GetCurrentObjectName(), "B");
    ASSERT_TRUE(r.ReadRow(row));
    ASSERT_EQ(std::get<std::string>(row[0].value), "hello");
    ASSERT_FALSE(r.NextObject());
}

TEST(Stream_SkipObject) {
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        w.BeginObject("Skip", { "v" }, { "int" });
        w.WriteRow(100); w.WriteRow(200); w.WriteRow(300);
        ASSERT_TRUE(w.EndObject());
        w.BeginObject("Keep", { "v" }, { "int" });
        w.WriteRow(999);
        ASSERT_TRUE(w.EndObject());
        ASSERT_TRUE(w.Close());
    }
    OTN::OTNStreamReader r;
    ASSERT_TRUE(r.Open(STREAM_TEST_FILE));
    ASSERT_TRUE(r.NextObject());
    ASSERT_EQ(r.GetCurrentObjectName(), "Skip");
    ASSERT_TRUE(r.SkipCurrentObject());
    ASSERT_TRUE(r.NextObject());
    ASSERT_EQ(r.GetCurrentObjectName(), "Keep");
    OTN::OTNRow row;
    ASSERT_TRUE(r.ReadRow(row));
    ASSERT_EQ(std::get<int>(row[0].value), 999);
}

TEST(Stream_LargeRowCount) {
    const int N = 10000;
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        w.BeginObject("Big", { "i", "f" }, { "int", "float" });
        for (int i = 0; i < N; ++i)
            w.WriteRow(i, static_cast<float>(i) * 0.5f);
        ASSERT_TRUE(w.EndObject());
        ASSERT_TRUE(w.Close());
    }
    OTN::OTNStreamReader r;
    ASSERT_TRUE(r.Open(STREAM_TEST_FILE));
    ASSERT_TRUE(r.NextObject());
    ASSERT_EQ(r.GetRemainingRowCount(), static_cast<size_t>(N));
    OTN::OTNRow row;
    for (int i = 0; i < N; ++i) {
        ASSERT_TRUE(r.ReadRow(row));
        ASSERT_EQ(std::get<int>(row[0].value), i);
        ASSERT_NEAR(std::get<float>(row[1].value), i * 0.5f, 1e-4f);
    }
    ASSERT_FALSE(r.HasMoreRows());
}

TEST(Stream_TypeVariety) {
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        w.BeginObject("Types", { "b", "d", "i64" }, { "bool", "double", "int64" });
        w.WriteRow(true, 3.141592653589793, static_cast<int64_t>(9'000'000'000LL));
        w.WriteRow(false, -1.0, static_cast<int64_t>(-100LL));
        ASSERT_TRUE(w.EndObject());
        ASSERT_TRUE(w.Close());
    }
    OTN::OTNStreamReader r;
    ASSERT_TRUE(r.Open(STREAM_TEST_FILE));
    ASSERT_TRUE(r.NextObject());
    OTN::OTNRow row;
    ASSERT_TRUE(r.ReadRow(row));
    ASSERT_EQ(std::get<bool>(row[0].value), true);
    ASSERT_NEAR(std::get<double>(row[1].value), 3.141592653589793, 1e-10);
    ASSERT_EQ(std::get<int64_t>(row[2].value), 9'000'000'000LL);
    ASSERT_TRUE(r.ReadRow(row));
    ASSERT_EQ(std::get<bool>(row[0].value), false);
    ASSERT_NEAR(std::get<double>(row[1].value), -1.0, 1e-12);
}

TEST(Stream_ListType) {
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        w.BeginObject("Lists", { "nums" }, { "int[]" });
        w.WriteRow(std::vector<int>{1, 2, 3});
        w.WriteRow(std::vector<int>{10, 20});
        ASSERT_TRUE(w.EndObject());
        ASSERT_TRUE(w.Close());
    }
    OTN::OTNStreamReader r;
    ASSERT_TRUE(r.Open(STREAM_TEST_FILE));
    ASSERT_TRUE(r.NextObject());
    OTN::OTNRow row;
    ASSERT_TRUE(r.ReadRow(row));
    auto& arr = *std::get<OTN::OTNArrayPtr>(row[0].value);
    ASSERT_EQ(arr.values.size(), 3u);
    ASSERT_EQ(std::get<int>(arr.values[0].value), 1);
    ASSERT_TRUE(r.ReadRow(row));
    auto& arr2 = *std::get<OTN::OTNArrayPtr>(row[0].value);
    ASSERT_EQ(arr2.values.size(), 2u);
    ASSERT_EQ(std::get<int>(arr2.values[1].value), 20);
}

TEST(Stream_AnyType) {
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        w.BeginObject("Any", { "val" }, { "any" });
        w.WriteRow(42);
        w.WriteRow(std::string("hello"));
        w.WriteRow(true);
        w.WriteRow(3.14f);
        ASSERT_TRUE(w.EndObject());
        ASSERT_TRUE(w.Close());
    }
    OTN::OTNStreamReader r;
    ASSERT_TRUE(r.Open(STREAM_TEST_FILE));
    ASSERT_TRUE(r.NextObject());
    ASSERT_EQ(r.GetRemainingRowCount(), 4u);
    OTN::OTNRow row;
    ASSERT_TRUE(r.ReadRow(row)); ASSERT_EQ(std::get<int>(row[0].value), 42);
    ASSERT_TRUE(r.ReadRow(row)); ASSERT_EQ(std::get<std::string>(row[0].value), "hello");
    ASSERT_TRUE(r.ReadRow(row)); ASSERT_EQ(std::get<bool>(row[0].value), true);
    ASSERT_TRUE(r.ReadRow(row)); ASSERT_NEAR(std::get<float>(row[0].value), 3.14f, 1e-4f);
}

TEST(Stream_WriteRowList) {
    OTN::OTNRow dataRow;
    dataRow.push_back(OTN::OTNValue(OTN::OTNValueVariant(7)));
    dataRow.push_back(OTN::OTNValue(OTN::OTNValueVariant(std::string("test"))));
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        w.BeginObject("RL", { "n", "s" }, { "int", "String" });
        ASSERT_TRUE(w.WriteRowList(dataRow));
        ASSERT_TRUE(w.EndObject());
        ASSERT_TRUE(w.Close());
    }
    OTN::OTNStreamReader r;
    ASSERT_TRUE(r.Open(STREAM_TEST_FILE));
    ASSERT_TRUE(r.NextObject());
    OTN::OTNRow row;
    ASSERT_TRUE(r.ReadRow(row));
    ASSERT_EQ(std::get<int>(row[0].value), 7);
    ASSERT_EQ(std::get<std::string>(row[1].value), "test");
}

TEST(Stream_Writer_BadPath) {
    OTN::OTNStreamWriter w;
    bool ok = w.Open("/nonexistent_dir_12345/sub/test.otn");
    ASSERT_EQ(ok, w.IsValid());
}

TEST(Stream_Reader_FileNotFound) {
    OTN::OTNStreamReader r;
    ASSERT_FALSE(r.Open("/this_file_does_not_exist_99999.otn"));
    ASSERT_FALSE(r.IsValid());
}

TEST(Stream_Version) {
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        w.BeginObject("V", { "x" }, { "int" });
        w.WriteRow(1);
        ASSERT_TRUE(w.EndObject());
        ASSERT_TRUE(w.Close());
    }
    OTN::OTNStreamReader r;
    ASSERT_TRUE(r.Open(STREAM_TEST_FILE));
    ASSERT_EQ(r.GetVersion(), OTN::VERSION);
}

// ============================================================
//  NESTED OBJECT / REF TESTS
// ============================================================

TEST(Stream_BeginObject_Variadic_Col) {
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        ASSERT_TRUE(w.BeginObject("Items",
            OTN::col("id", "int"),
            OTN::col("name", "String"),
            OTN::col("price", "float")));
        w.WriteRow(1, std::string("Sword"), 9.99f);
        w.WriteRow(2, std::string("Shield"), 14.5f);
        ASSERT_TRUE(w.EndObject());
        ASSERT_TRUE(w.Close());
    }
    OTN::OTNReader r;
    ASSERT_TRUE(r.ReadFile(STREAM_TEST_FILE));
    auto obj = r.TryGetObject("Items");
    ASSERT_TRUE(obj.has_value());
    ASSERT_EQ(obj->GetRowCount(), 2u);
    ASSERT_EQ(obj->GetValue<std::string>(0, "name"), "Sword");
    ASSERT_NEAR(obj->GetValue<float>(1, "price"), 14.5f, 1e-4f);
}

TEST(Stream_GetWrittenRowCount) {
    OTN::OTNStreamWriter w;
    ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
    ASSERT_EQ(w.GetWrittenRowCount("Stats"), 0u);
    w.BeginObject("Stats", { "hp" }, { "int" });
    w.WriteRow(100); w.WriteRow(200);
    ASSERT_TRUE(w.EndObject());
    ASSERT_EQ(w.GetWrittenRowCount("Stats"), 2u);
    w.BeginObject("Stats", { "hp" }, { "int" });
    w.WriteRow(300);
    ASSERT_TRUE(w.EndObject());
    ASSERT_EQ(w.GetWrittenRowCount("Stats"), 3u);
    ASSERT_TRUE(w.Close());
}

TEST(Stream_AppendObject_Flat) {
    OTN::OTNObject obj("Flat");
    obj.SetNames("x", "y").AddDataRow(7, 42);
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        ASSERT_TRUE(w.AppendObject(obj));
        ASSERT_TRUE(w.Close());
    }
    OTN::OTNReader r;
    ASSERT_TRUE(r.ReadFile(STREAM_TEST_FILE));
    auto o = r.TryGetObject("Flat");
    ASSERT_TRUE(o.has_value());
    ASSERT_EQ(o->GetValue<int>(0, "x"), 7);
    ASSERT_EQ(o->GetValue<int>(0, "y"), 42);
}

TEST(Stream_AppendObject_OneLevel_Ref) {
    auto sPtr0 = std::make_shared<OTN::OTNObject>("Stats");
    sPtr0->SetNames("dmg", "spd").SetTypes("float", "float").AddDataRow(10.0f, 1.5f);
    auto sPtr1 = std::make_shared<OTN::OTNObject>("Stats");
    sPtr1->SetNames("dmg", "spd").SetTypes("float", "float").AddDataRow(20.0f, 0.8f);
    OTN::OTNObject weapons("Weapon");
    weapons.SetNames("id", "stats").SetTypes("int", "Stats");
    weapons.AddDataRowList({ OTN::OTNValue(OTN::OTNValueVariant(int(1))), OTN::OTNValue(OTN::OTNValueVariant(sPtr0)) });
    weapons.AddDataRowList({ OTN::OTNValue(OTN::OTNValueVariant(int(2))), OTN::OTNValue(OTN::OTNValueVariant(sPtr1)) });
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        ASSERT_TRUE(w.AppendObject(weapons));
        ASSERT_TRUE(w.Close());
    }
    OTN::OTNReader r;
    ASSERT_TRUE(r.ReadFile(STREAM_TEST_FILE));
    auto statsObj = r.TryGetObject("Stats");
    ASSERT_TRUE(statsObj.has_value());
    ASSERT_EQ(statsObj->GetRowCount(), 2u);
    ASSERT_NEAR(statsObj->GetValue<float>(0, "dmg"), 10.0f, 1e-4f);
    ASSERT_NEAR(statsObj->GetValue<float>(1, "dmg"), 20.0f, 1e-4f);
    auto weaponObj = r.TryGetObject("Weapon");
    ASSERT_TRUE(weaponObj.has_value());
    ASSERT_EQ(weaponObj->GetRowCount(), 2u);
    ASSERT_EQ(weaponObj->GetValue<int>(0, "id"), 1);
}

TEST(Stream_AppendObject_Mixed_With_BeginObject) {
    OTN::OTNObject mat("Material");
    mat.SetNames("r", "g", "b").SetTypes("float", "float", "float");
    mat.AddDataRow(1.0f, 0.0f, 0.0f);
    mat.AddDataRow(0.0f, 1.0f, 0.0f);
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        ASSERT_TRUE(w.AppendObject(mat));
        ASSERT_EQ(w.GetWrittenRowCount("Material"), 2u);
        ASSERT_TRUE(w.BeginObject("Mesh", OTN::col("name", "String"), OTN::col("material", "Material")));
        w.WriteRow(std::string("Cube"), 0);
        w.WriteRow(std::string("Sphere"), 1);
        ASSERT_TRUE(w.EndObject());
        ASSERT_TRUE(w.Close());
    }
    OTN::OTNReader r;
    ASSERT_TRUE(r.ReadFile(STREAM_TEST_FILE));
    auto matObj = r.TryGetObject("Material"); ASSERT_TRUE(matObj.has_value());  ASSERT_EQ(matObj->GetRowCount(), 2u);
    auto meshObj = r.TryGetObject("Mesh");     ASSERT_TRUE(meshObj.has_value()); ASSERT_EQ(meshObj->GetRowCount(), 2u);
    ASSERT_EQ(meshObj->GetValue<std::string>(0, "name"), "Cube");
    ASSERT_EQ(meshObj->GetValue<std::string>(1, "name"), "Sphere");
}

TEST(Stream_AppendObject_DeepNesting) {
    auto cfgPtr = std::make_shared<OTN::OTNObject>("Cfg");
    cfgPtr->SetNames("val").SetTypes("int").AddDataRow(42);
    auto layerPtr = std::make_shared<OTN::OTNObject>("Layer");
    layerPtr->SetNames("index", "cfg").SetTypes("int", "Cfg");
    layerPtr->AddDataRowList({ OTN::OTNValue(OTN::OTNValueVariant(int(0))), OTN::OTNValue(OTN::OTNValueVariant(cfgPtr)) });
    OTN::OTNObject model("Model");
    model.SetNames("name", "layer").SetTypes("String", "Layer");
    model.AddDataRowList({ OTN::OTNValue(OTN::OTNValueVariant(std::string("Net"))), OTN::OTNValue(OTN::OTNValueVariant(layerPtr)) });
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        ASSERT_TRUE(w.AppendObject(model));
        ASSERT_TRUE(w.Close());
        ASSERT_TRUE(w.IsValid());
    }
    OTN::OTNReader r;
    ASSERT_TRUE(r.ReadFile(STREAM_TEST_FILE));
    ASSERT_TRUE(r.TryGetObject("Cfg").has_value());
    ASSERT_TRUE(r.TryGetObject("Layer").has_value());
    auto modelObj = r.TryGetObject("Model");
    ASSERT_TRUE(modelObj.has_value());
    ASSERT_EQ(modelObj->GetValue<std::string>(0, "name"), "Net");
}

TEST(Stream_AppendObject_SharedSubObject) {
    auto sharedStats = std::make_shared<OTN::OTNObject>("Stats");
    sharedStats->SetNames("dmg").SetTypes("int").AddDataRow(50);
    OTN::OTNObject weapons("Weapon");
    weapons.SetNames("name", "stats").SetTypes("String", "Stats");
    weapons.AddDataRowList({ OTN::OTNValue(OTN::OTNValueVariant(std::string("Sword"))), OTN::OTNValue(OTN::OTNValueVariant(sharedStats)) });
    weapons.AddDataRowList({ OTN::OTNValue(OTN::OTNValueVariant(std::string("Axe"))),   OTN::OTNValue(OTN::OTNValueVariant(sharedStats)) });
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        ASSERT_TRUE(w.AppendObject(weapons));
        ASSERT_TRUE(w.Close());
        ASSERT_TRUE(w.IsValid());
    }
    OTN::OTNReader r;
    ASSERT_TRUE(r.ReadFile(STREAM_TEST_FILE));
    auto statsObj = r.TryGetObject("Stats");  ASSERT_TRUE(statsObj.has_value());  ASSERT_EQ(statsObj->GetRowCount(), 1u);
    auto weaponObj = r.TryGetObject("Weapon"); ASSERT_TRUE(weaponObj.has_value()); ASSERT_EQ(weaponObj->GetRowCount(), 2u);
}

TEST(Stream_BeginObject_RefType_ManualIndex) {
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        ASSERT_TRUE(w.BeginObject("Stats", { "hp","mp" }, { "int","int" }));
        w.WriteRow(100, 50); w.WriteRow(200, 80);
        ASSERT_TRUE(w.EndObject());
        ASSERT_TRUE(w.BeginObject("Weapon", OTN::col("id", "int"), OTN::col("stats", "Stats")));
        w.WriteRow(10, 0); w.WriteRow(20, 1);
        ASSERT_TRUE(w.EndObject());
        ASSERT_TRUE(w.Close());
        ASSERT_TRUE(w.IsValid());
    }
    OTN::OTNReader r;
    ASSERT_TRUE(r.ReadFile(STREAM_TEST_FILE));
    auto stats = r.TryGetObject("Stats");  ASSERT_TRUE(stats.has_value());   ASSERT_EQ(stats->GetRowCount(), 2u); ASSERT_EQ(stats->GetValue<int>(1, "hp"), 200);
    auto weapons = r.TryGetObject("Weapon"); ASSERT_TRUE(weapons.has_value()); ASSERT_EQ(weapons->GetRowCount(), 2u); ASSERT_EQ(weapons->GetValue<int>(0, "id"), 10);
}

TEST(Stream_AppendObject_SequentialDifferentTypes) {
    OTN::OTNObject typeA("TypeA"); typeA.SetNames("x").SetTypes("int").AddDataRow(1).AddDataRow(2);
    OTN::OTNObject typeB("TypeB"); typeB.SetNames("y").SetTypes("float").AddDataRow(9.0f).AddDataRow(8.0f);
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        ASSERT_TRUE(w.AppendObject(typeA));
        ASSERT_TRUE(w.AppendObject(typeB));
        ASSERT_TRUE(w.Close());
        ASSERT_TRUE(w.IsValid());
    }
    OTN::OTNReader r;
    ASSERT_TRUE(r.ReadFile(STREAM_TEST_FILE));
    auto a = r.TryGetObject("TypeA"); ASSERT_TRUE(a.has_value()); ASSERT_EQ(a->GetRowCount(), 2u); ASSERT_EQ(a->GetValue<int>(1, "x"), 2);
    auto b = r.TryGetObject("TypeB"); ASSERT_TRUE(b.has_value()); ASSERT_EQ(b->GetRowCount(), 2u); ASSERT_NEAR(b->GetValue<float>(0, "y"), 9.0f, 1e-5f);
}

TEST(Stream_AppendObject_SecondCall_ReusesSharedSubPtr) {
    auto sharedSub = std::make_shared<OTN::OTNObject>("SharedSub");
    sharedSub->SetNames("v").SetTypes("int").AddDataRow(77);
    OTN::OTNObject root1("Root1"); root1.SetNames("s").SetTypes("SharedSub");
    root1.AddDataRowList({ OTN::OTNValue(OTN::OTNValueVariant(sharedSub)) });
    OTN::OTNObject root2("Root2"); root2.SetNames("s").SetTypes("SharedSub");
    root2.AddDataRowList({ OTN::OTNValue(OTN::OTNValueVariant(sharedSub)) });
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        ASSERT_TRUE(w.AppendObject(root1));
        ASSERT_TRUE(w.AppendObject(root2));
        ASSERT_TRUE(w.Close());
        ASSERT_TRUE(w.IsValid());
    }
    OTN::OTNReader r;
    ASSERT_TRUE(r.ReadFile(STREAM_TEST_FILE));
    auto sub = r.TryGetObject("SharedSub"); ASSERT_TRUE(sub.has_value()); ASSERT_EQ(sub->GetRowCount(), 1u); ASSERT_EQ(sub->GetValue<int>(0, "v"), 77);
    ASSERT_TRUE(r.TryGetObject("Root1").has_value());
    auto r2 = r.TryGetObject("Root2"); ASSERT_TRUE(r2.has_value()); ASSERT_EQ(r2->GetRowCount(), 1u);
}

TEST(Stream_AppendObject_SecondCall_SameNameConflict_Errors) {
    auto sub1 = std::make_shared<OTN::OTNObject>("ConflictSub"); sub1->SetNames("v").SetTypes("int").AddDataRow(1);
    auto sub2 = std::make_shared<OTN::OTNObject>("ConflictSub"); sub2->SetNames("v").SetTypes("int").AddDataRow(2);
    OTN::OTNObject root1("Root1C"); root1.SetNames("s").SetTypes("ConflictSub"); root1.AddDataRowList({ OTN::OTNValue(OTN::OTNValueVariant(sub1)) });
    OTN::OTNObject root2("Root2C"); root2.SetNames("s").SetTypes("ConflictSub"); root2.AddDataRowList({ OTN::OTNValue(OTN::OTNValueVariant(sub2)) });
    OTN::OTNStreamWriter w;
    ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
    ASSERT_TRUE(w.AppendObject(root1));
    ASSERT_FALSE(w.AppendObject(root2));
    ASSERT_FALSE(w.IsValid());
    ASSERT_FALSE(w.GetError().empty());
}

TEST(Stream_AppendObject_ListOfObjectRefs) {
    auto item0 = std::make_shared<OTN::OTNObject>("Item"); item0->SetNames("name").SetTypes("String").AddDataRow(std::string("Sword"));
    auto item1 = std::make_shared<OTN::OTNObject>("Item"); item1->SetNames("name").SetTypes("String").AddDataRow(std::string("Shield"));
    auto item2 = std::make_shared<OTN::OTNObject>("Item"); item2->SetNames("name").SetTypes("String").AddDataRow(std::string("Potion"));
    auto listArr = std::make_shared<OTN::OTNArray>();
    listArr->values.push_back(OTN::OTNValue(OTN::OTNValueVariant(item0)));
    listArr->values.push_back(OTN::OTNValue(OTN::OTNValueVariant(item1)));
    listArr->values.push_back(OTN::OTNValue(OTN::OTNValueVariant(item2)));
    OTN::OTNObject inv("Inventory");
    inv.SetNames("owner", "items").SetTypes("String", "Item[]");
    inv.AddDataRowList({ OTN::OTNValue(OTN::OTNValueVariant(std::string("Alice"))), OTN::OTNValue(OTN::OTNValueVariant(listArr)) });
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        ASSERT_TRUE(w.AppendObject(inv));
        ASSERT_TRUE(w.Close());
        ASSERT_TRUE(w.IsValid());
    }
    OTN::OTNReader r;
    ASSERT_TRUE(r.ReadFile(STREAM_TEST_FILE));
    auto itemObj = r.TryGetObject("Item"); ASSERT_TRUE(itemObj.has_value()); ASSERT_EQ(itemObj->GetRowCount(), 3u);
    ASSERT_EQ(itemObj->GetValue<std::string>(0, "name"), "Sword");
    ASSERT_EQ(itemObj->GetValue<std::string>(2, "name"), "Potion");
    auto invObj = r.TryGetObject("Inventory"); ASSERT_TRUE(invObj.has_value()); ASSERT_EQ(invObj->GetRowCount(), 1u);
    ASSERT_EQ(invObj->GetValue<std::string>(0, "owner"), "Alice");
}

TEST(Stream_AppendObject_TwoDistinctSubObjectTypes) {
    auto tex = std::make_shared<OTN::OTNObject>("Texture"); tex->SetNames("path").SetTypes("String").AddDataRow(std::string("diffuse.png"));
    auto shader = std::make_shared<OTN::OTNObject>("Shader");  shader->SetNames("code").SetTypes("String").AddDataRow(std::string("pbr.glsl"));
    OTN::OTNObject mat("Material2");
    mat.SetNames("name", "tex", "shader").SetTypes("String", "Texture", "Shader");
    mat.AddDataRowList({ OTN::OTNValue(OTN::OTNValueVariant(std::string("Gold"))), OTN::OTNValue(OTN::OTNValueVariant(tex)), OTN::OTNValue(OTN::OTNValueVariant(shader)) });
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        ASSERT_TRUE(w.AppendObject(mat));
        ASSERT_TRUE(w.Close());
        ASSERT_TRUE(w.IsValid());
    }
    OTN::OTNReader r;
    ASSERT_TRUE(r.ReadFile(STREAM_TEST_FILE));
    ASSERT_TRUE(r.TryGetObject("Texture").has_value());
    ASSERT_TRUE(r.TryGetObject("Shader").has_value());
    auto m = r.TryGetObject("Material2"); ASSERT_TRUE(m.has_value()); ASSERT_EQ(m->GetValue<std::string>(0, "name"), "Gold");
}

TEST(Stream_AppendObject_SamePtrTwoColumns) {
    auto cfg = std::make_shared<OTN::OTNObject>("Cfg2"); cfg->SetNames("val").SetTypes("int").AddDataRow(42);
    OTN::OTNObject node("Node"); node.SetNames("a", "b").SetTypes("Cfg2", "Cfg2");
    node.AddDataRowList({ OTN::OTNValue(OTN::OTNValueVariant(cfg)), OTN::OTNValue(OTN::OTNValueVariant(cfg)) });
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        ASSERT_TRUE(w.AppendObject(node));
        ASSERT_TRUE(w.Close());
        ASSERT_TRUE(w.IsValid());
    }
    OTN::OTNReader r;
    ASSERT_TRUE(r.ReadFile(STREAM_TEST_FILE));
    auto cfgObj = r.TryGetObject("Cfg2"); ASSERT_TRUE(cfgObj.has_value()); ASSERT_EQ(cfgObj->GetRowCount(), 1u);
    auto nodeObj = r.TryGetObject("Node"); ASSERT_TRUE(nodeObj.has_value());
    ASSERT_EQ(nodeObj->GetValue<OTN::OTNObject>(0, "a").GetValue<int>(0, "val"), 42);
    ASSERT_EQ(nodeObj->GetValue<OTN::OTNObject>(0, "b").GetValue<int>(0, "val"), 42);
}

TEST(Stream_AppendObject_ManyUniqueSubObjects) {
    const int N = 20;
    std::vector<std::shared_ptr<OTN::OTNObject>> subs;
    for (int i = 0; i < N; ++i) {
        auto s = std::make_shared<OTN::OTNObject>("SubN");
        s->SetNames("v").SetTypes("int").AddDataRow(i * 10);
        subs.push_back(s);
    }
    OTN::OTNObject parent("ParentN"); parent.SetNames("idx", "sub").SetTypes("int", "SubN");
    for (int i = 0; i < N; ++i)
        parent.AddDataRowList({ OTN::OTNValue(OTN::OTNValueVariant(i)), OTN::OTNValue(OTN::OTNValueVariant(subs[i])) });
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        ASSERT_TRUE(w.AppendObject(parent));
        ASSERT_TRUE(w.Close());
        ASSERT_TRUE(w.IsValid());
    }
    OTN::OTNReader r;
    ASSERT_TRUE(r.ReadFile(STREAM_TEST_FILE));
    auto subObj = r.TryGetObject("SubN");    ASSERT_TRUE(subObj.has_value());    ASSERT_EQ(subObj->GetRowCount(), static_cast<size_t>(N));
    auto parentObj = r.TryGetObject("ParentN"); ASSERT_TRUE(parentObj.has_value()); ASSERT_EQ(parentObj->GetRowCount(), static_cast<size_t>(N));
    for (int i = 0; i < N; ++i) {
        ASSERT_EQ(parentObj->GetValue<int>(i, "idx"), i);
        ASSERT_EQ(parentObj->GetValue<OTN::OTNObject>(i, "sub").GetValue<int>(0, "v"), i * 10);
        ASSERT_EQ(subObj->GetValue<int>(i, "v"), i * 10);
    }
}

TEST(Stream_AppendObject_FourLevelNesting) {
    auto d = std::make_shared<OTN::OTNObject>("D"); d->SetNames("val").SetTypes("int").AddDataRow(99);
    auto c = std::make_shared<OTN::OTNObject>("C"); c->SetNames("d").SetTypes("D"); c->AddDataRowList({ OTN::OTNValue(OTN::OTNValueVariant(d)) });
    auto b = std::make_shared<OTN::OTNObject>("B"); b->SetNames("c").SetTypes("C"); b->AddDataRowList({ OTN::OTNValue(OTN::OTNValueVariant(c)) });
    OTN::OTNObject a("A"); a.SetNames("b").SetTypes("B"); a.AddDataRowList({ OTN::OTNValue(OTN::OTNValueVariant(b)) });
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        ASSERT_TRUE(w.AppendObject(a));
        ASSERT_TRUE(w.Close());
        ASSERT_TRUE(w.IsValid());
    }
    OTN::OTNReader r;
    ASSERT_TRUE(r.ReadFile(STREAM_TEST_FILE));
    ASSERT_TRUE(r.TryGetObject("D").has_value());
    ASSERT_TRUE(r.TryGetObject("C").has_value());
    ASSERT_TRUE(r.TryGetObject("B").has_value());
    auto aObj = r.TryGetObject("A"); ASSERT_TRUE(aObj.has_value()); ASSERT_EQ(aObj->GetRowCount(), 1u);
}

TEST(Stream_AppendObject_DiamondDependency) {
    auto c = std::make_shared<OTN::OTNObject>("DiamondC"); c->SetNames("v").SetTypes("int").AddDataRow(42);
    auto a = std::make_shared<OTN::OTNObject>("DiamondA"); a->SetNames("c").SetTypes("DiamondC"); a->AddDataRowList({ OTN::OTNValue(OTN::OTNValueVariant(c)) });
    auto b = std::make_shared<OTN::OTNObject>("DiamondB"); b->SetNames("c").SetTypes("DiamondC"); b->AddDataRowList({ OTN::OTNValue(OTN::OTNValueVariant(c)) });
    OTN::OTNObject root("DiamondRoot"); root.SetNames("a", "b").SetTypes("DiamondA", "DiamondB");
    root.AddDataRowList({ OTN::OTNValue(OTN::OTNValueVariant(a)), OTN::OTNValue(OTN::OTNValueVariant(b)) });
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        ASSERT_TRUE(w.AppendObject(root));
        ASSERT_TRUE(w.Close());
        ASSERT_TRUE(w.IsValid());
    }
    OTN::OTNReader r;
    ASSERT_TRUE(r.ReadFile(STREAM_TEST_FILE));
    auto cObj = r.TryGetObject("DiamondC"); ASSERT_TRUE(cObj.has_value()); ASSERT_EQ(cObj->GetRowCount(), 1u); ASSERT_EQ(cObj->GetValue<int>(0, "v"), 42);
    ASSERT_TRUE(r.TryGetObject("DiamondA").has_value());
    ASSERT_TRUE(r.TryGetObject("DiamondB").has_value());
    ASSERT_TRUE(r.TryGetObject("DiamondRoot").has_value());
}

TEST(Stream_AppendObject_ZeroRows) {
    OTN::OTNObject empty("EmptyObj"); empty.SetNames("x").SetTypes("int");
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        ASSERT_TRUE(w.AppendObject(empty));
        ASSERT_TRUE(w.Close());
        ASSERT_TRUE(w.IsValid());
    }
    OTN::OTNReader r;
    ASSERT_TRUE(r.ReadFile(STREAM_TEST_FILE));
    auto obj = r.TryGetObject("EmptyObj"); ASSERT_TRUE(obj.has_value()); ASSERT_EQ(obj->GetRowCount(), 0u);
}

TEST(Stream_AppendObject_RichRow) {
    auto stats = std::make_shared<OTN::OTNObject>("RichStats"); stats->SetNames("hp", "mp").SetTypes("int", "int").AddDataRow(150, 60);
    OTN::OTNObject hero("Hero"); hero.SetNames("name", "level", "tags", "stats").SetTypes("String", "int", "String[]", "RichStats");
    auto tagsArr = std::make_shared<OTN::OTNArray>();
    tagsArr->values.push_back(OTN::OTNValue(OTN::OTNValueVariant(std::string("warrior"))));
    tagsArr->values.push_back(OTN::OTNValue(OTN::OTNValueVariant(std::string("tank"))));
    hero.AddDataRowList({ OTN::OTNValue(OTN::OTNValueVariant(std::string("Aldric"))), OTN::OTNValue(OTN::OTNValueVariant(int(5))), OTN::OTNValue(OTN::OTNValueVariant(tagsArr)), OTN::OTNValue(OTN::OTNValueVariant(stats)) });
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        ASSERT_TRUE(w.AppendObject(hero));
        ASSERT_TRUE(w.Close());
        ASSERT_TRUE(w.IsValid());
    }
    OTN::OTNReader r;
    ASSERT_TRUE(r.ReadFile(STREAM_TEST_FILE));
    auto heroObj = r.TryGetObject("Hero"); ASSERT_TRUE(heroObj.has_value());
    ASSERT_EQ(heroObj->GetValue<std::string>(0, "name"), "Aldric");
    ASSERT_EQ(heroObj->GetValue<int>(0, "level"), 5);
    auto tagVec = heroObj->GetValue<std::vector<std::string>>(0, "tags"); ASSERT_EQ(tagVec.size(), 2u); ASSERT_EQ(tagVec[0], "warrior");
    auto statsObj = r.TryGetObject("RichStats"); ASSERT_TRUE(statsObj.has_value()); ASSERT_EQ(statsObj->GetValue<int>(0, "hp"), 150);
}

TEST(Stream_BeginObject_RefListType) {
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        ASSERT_TRUE(w.BeginObject("Slot", OTN::col("id", "int"), OTN::col("name", "String")));
        w.WriteRow(0, std::string("Head")); w.WriteRow(1, std::string("Body")); w.WriteRow(2, std::string("Feet"));
        ASSERT_TRUE(w.EndObject());
        ASSERT_TRUE(w.BeginObject("Equipment", OTN::col("owner", "String"), OTN::col("slots", "Slot[]")));
        w.WriteRow(std::string("Warrior"), std::vector<int>{0, 1, 2});
        w.WriteRow(std::string("Mage"), std::vector<int>{0, 2});
        ASSERT_TRUE(w.EndObject());
        ASSERT_TRUE(w.Close());
        ASSERT_TRUE(w.IsValid());
    }
    OTN::OTNReader r;
    ASSERT_TRUE(r.ReadFile(STREAM_TEST_FILE));
    auto slot = r.TryGetObject("Slot");      ASSERT_TRUE(slot.has_value());  ASSERT_EQ(slot->GetRowCount(), 3u);
    auto equip = r.TryGetObject("Equipment"); ASSERT_TRUE(equip.has_value()); ASSERT_EQ(equip->GetRowCount(), 2u);
    ASSERT_EQ(equip->GetValue<std::string>(0, "owner"), "Warrior");
}

TEST(Stream_AppendObject_ThenBeginObject_WithCount) {
    auto sa = std::make_shared<OTN::OTNObject>("SA"); sa->SetNames("v").SetTypes("int").AddDataRow(10);
    auto sb = std::make_shared<OTN::OTNObject>("SA"); sb->SetNames("v").SetTypes("int").AddDataRow(20);
    OTN::OTNObject containerA("ContainerA"); containerA.SetNames("s").SetTypes("SA");
    containerA.AddDataRowList({ OTN::OTNValue(OTN::OTNValueVariant(sa)) });
    containerA.AddDataRowList({ OTN::OTNValue(OTN::OTNValueVariant(sb)) });
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        ASSERT_TRUE(w.AppendObject(containerA));
        ASSERT_EQ(w.GetWrittenRowCount("SA"), 2u);
        ASSERT_EQ(w.GetWrittenRowCount("ContainerA"), 2u);
        ASSERT_TRUE(w.BeginObject("ContainerB", OTN::col("ref", "SA")));
        size_t nextIdx = w.GetWrittenRowCount("SA");
        w.WriteRow(0);
        ASSERT_TRUE(w.EndObject());
        ASSERT_TRUE(w.Close());
        ASSERT_TRUE(w.IsValid());
        (void)nextIdx;
    }
    OTN::OTNReader r;
    ASSERT_TRUE(r.ReadFile(STREAM_TEST_FILE));
    auto saObj = r.TryGetObject("SA"); ASSERT_TRUE(saObj.has_value()); ASSERT_EQ(saObj->GetRowCount(), 2u);
    ASSERT_EQ(saObj->GetValue<int>(0, "v"), 10); ASSERT_EQ(saObj->GetValue<int>(1, "v"), 20);
    auto cb = r.TryGetObject("ContainerB"); ASSERT_TRUE(cb.has_value()); ASSERT_EQ(cb->GetRowCount(), 1u);
}

TEST(Stream_CanonicalMix_AppendThenBegin) {
    OTN::OTNObject positions("Position"); positions.SetNames("x", "y").SetTypes("float", "float");
    positions.AddDataRow(0.0f, 0.0f); positions.AddDataRow(10.0f, 5.0f); positions.AddDataRow(-3.0f, 2.0f);
    {
        OTN::OTNStreamWriter w;
        ASSERT_TRUE(w.Open(STREAM_TEST_FILE));
        ASSERT_TRUE(w.AppendObject(positions));
        ASSERT_EQ(w.GetWrittenRowCount("Position"), 3u);
        ASSERT_TRUE(w.BeginObject("Entity", OTN::col("name", "String"), OTN::col("pos", "Position")));
        w.WriteRow(std::string("Player"), 0); w.WriteRow(std::string("Enemy1"), 1); w.WriteRow(std::string("Enemy2"), 2);
        ASSERT_TRUE(w.EndObject());
        ASSERT_TRUE(w.Close());
        ASSERT_TRUE(w.IsValid());
    }
    OTN::OTNReader r;
    ASSERT_TRUE(r.ReadFile(STREAM_TEST_FILE));
    auto posObj = r.TryGetObject("Position"); ASSERT_TRUE(posObj.has_value()); ASSERT_EQ(posObj->GetRowCount(), 3u); ASSERT_NEAR(posObj->GetValue<float>(1, "x"), 10.0f, 1e-5f);
    auto entObj = r.TryGetObject("Entity");   ASSERT_TRUE(entObj.has_value()); ASSERT_EQ(entObj->GetRowCount(), 3u);
    ASSERT_EQ(entObj->GetValue<std::string>(0, "name"), "Player");
    ASSERT_EQ(entObj->GetValue<std::string>(2, "name"), "Enemy2");
}

// ============================================================
// ============================================================
//  BENCHMARKS
// ============================================================
// ============================================================

static void RunBenchmarks() {
    std::filesystem::create_directories(BENCH_DIR);

    std::cout << "\n========================================================\n";
    std::cout << "  Benchmarks  (" << BENCH_ROWS << " rows, Schema: id/value/label/active)\n";
    std::cout << "  Tipp: Mit -O2 kompilieren fuer repraesentative Ergebnisse\n";
    std::cout << "========================================================\n";

    // ---- 1. OTNWriter → string  |  OTNReader ← string ----
    {
        BenchResult res;
        res.label = "OTNWriter  -> string      |  OTNReader <- string";
        res.rowCount = BENCH_ROWS;

        auto t0 = Now();
        OTN::OTNObject obj("BenchData");
        obj.SetNames("id", "value", "label", "active").SetTypes("int", "float", "String", "bool").ReserveDataRows(BENCH_ROWS);
        for (size_t i = 0; i < BENCH_ROWS; ++i) { auto r = MakeBenchRow((int)i); obj.AddDataRow(r.id, r.value, r.label, r.active); }
        OTN::OTNWriter writer; writer.AppendObject(obj);
        std::string outStr; writer.SaveToString(outStr);
        res.writeMs = ElapsedMs(t0, Now());
        res.byteSize = outStr.size();

        auto t1 = Now();
        OTN::OTNReader reader; reader.ReadString(outStr); reader.TryGetObject("BenchData");
        res.readMs = ElapsedMs(t1, Now());

        s_benchResults.push_back(res);
        std::cout << "  [1/6] OTNWriter→string fertig\n";
    }

    // ---- 2. OTNWriter → file  |  OTNReader ← file ----
    {
        BenchResult res;
        res.label = "OTNWriter  -> file        |  OTNReader  <- file";
        res.rowCount = BENCH_ROWS;

        auto t0 = Now();
        OTN::OTNObject obj("BenchData");
        obj.SetNames("id", "value", "label", "active").SetTypes("int", "float", "String", "bool").ReserveDataRows(BENCH_ROWS);
        for (size_t i = 0; i < BENCH_ROWS; ++i) { auto r = MakeBenchRow((int)i); obj.AddDataRow(r.id, r.value, r.label, r.active); }
        OTN::OTNWriter writer; writer.AppendObject(obj); writer.Save(BENCH_FILE);
        res.writeMs = ElapsedMs(t0, Now());
        res.byteSize = std::filesystem::file_size(BENCH_FILE);

        auto t1 = Now();
        OTN::OTNReader reader; reader.ReadFile(BENCH_FILE); reader.TryGetObject("BenchData");
        res.readMs = ElapsedMs(t1, Now());

        s_benchResults.push_back(res);
        std::cout << "  [2/6] OTNWriter→file fertig\n";
    }

    // ---- 3. OTNStreamWriter → file  |  OTNStreamReader ← file ----
    {
        BenchResult res;
        res.label = "OTNStreamWriter -> file   |  OTNStreamReader <- file";
        res.rowCount = BENCH_ROWS;

        auto t0 = Now();
        OTN::OTNStreamWriter w; w.Open(BENCH_FILE);
        w.BeginObject("BenchData", { "id","value","label","active" }, { "int","float","String","bool" });
        for (size_t i = 0; i < BENCH_ROWS; ++i) { auto r = MakeBenchRow((int)i); w.WriteRow(r.id, r.value, r.label, r.active); }
        w.EndObject(); w.Close();
        res.writeMs = ElapsedMs(t0, Now());
        res.byteSize = std::filesystem::file_size(BENCH_FILE);

        auto t1 = Now();
        OTN::OTNStreamReader r; r.Open(BENCH_FILE); r.NextObject();
        OTN::OTNRow row; size_t count = 0;
        while (r.ReadRow(row)) ++count;
        res.readMs = ElapsedMs(t1, Now());

        s_benchResults.push_back(res);
        std::cout << "  [3/6] OTNStreamWriter→file fertig\n";
    }

    // ---- 4. OTNStreamWriter → file  |  OTNReader ← file (batch) ----
    {
        BenchResult res;
        res.label = "OTNStreamWriter -> file   |  OTNReader (batch) <- file";
        res.rowCount = BENCH_ROWS;

        auto t0 = Now();
        OTN::OTNStreamWriter w; w.Open(BENCH_FILE);
        w.BeginObject("BenchData", { "id","value","label","active" }, { "int","float","String","bool" });
        for (size_t i = 0; i < BENCH_ROWS; ++i) { auto r = MakeBenchRow((int)i); w.WriteRow(r.id, r.value, r.label, r.active); }
        w.EndObject(); w.Close();
        res.writeMs = ElapsedMs(t0, Now());
        res.byteSize = std::filesystem::file_size(BENCH_FILE);

        auto t1 = Now();
        OTN::OTNReader reader; reader.ReadFile(BENCH_FILE); reader.TryGetObject("BenchData");
        res.readMs = ElapsedMs(t1, Now());

        s_benchResults.push_back(res);
        std::cout << "  [4/6] StreamWriter + BatchReader fertig\n";
    }

    // ---- 5. OTNWriter (optimized flags) → string  |  OTNReader ← string ----
    {
        BenchResult res;
        res.label = "OTNWriter (opt.) -> string |  OTNReader  <- string";
        res.rowCount = BENCH_ROWS;

        auto t0 = Now();
        OTN::OTNObject obj("BenchData");
        obj.SetNames("id", "value", "label", "active").SetTypes("int", "float", "String", "bool").ReserveDataRows(BENCH_ROWS);
        for (size_t i = 0; i < BENCH_ROWS; ++i) { auto r = MakeBenchRow((int)i); obj.AddDataRow(r.id, r.value, r.label, r.active); }
        OTN::OTNWriter writer;
        writer.UseOptimizations(true).UseDefType(true).UseDefName(true).AppendObject(obj);
        std::string outStr; writer.SaveToString(outStr);
        res.writeMs = ElapsedMs(t0, Now());
        res.byteSize = outStr.size();

        auto t1 = Now();
        OTN::OTNReader reader; reader.ReadString(outStr); reader.TryGetObject("BenchData");
        res.readMs = ElapsedMs(t1, Now());

        s_benchResults.push_back(res);
        std::cout << "  [5/6] OTNWriter (optimized) fertig\n";
    }

    // ---- 6. int-only schema ----
    {
        BenchResult res;
        res.label = "int-only schema: StreamWriter -> file | StreamReader";
        res.rowCount = BENCH_ROWS;
        const auto p = BENCH_DIR / "bench_int.otn";

        auto t0 = Now();
        OTN::OTNStreamWriter w; w.Open(p);
        w.BeginObject("IntData", { "a","b","c","d" }, { "int","int","int","int" });
        for (size_t i = 0; i < BENCH_ROWS; ++i)
            w.WriteRow((int)i, (int)(i * 2), (int)(i * 3), (int)(i & 0xFF));
        w.EndObject(); w.Close();
        res.writeMs = ElapsedMs(t0, Now());
        res.byteSize = std::filesystem::file_size(p);

        auto t1 = Now();
        OTN::OTNStreamReader r; r.Open(p); r.NextObject();
        OTN::OTNRow row; size_t count = 0;
        while (r.ReadRow(row)) ++count;
        res.readMs = ElapsedMs(t1, Now());

        s_benchResults.push_back(res);
        std::cout << "  [6/6] int-only fertig\n";
    }

    // ---- Detailergebnisse ----
    std::cout << "\n--------------------------------------------------------\n";
    std::cout << "  Benchmark-Ergebnisse (Details)\n";
    std::cout << "--------------------------------------------------------\n";
    for (auto& br : s_benchResults)
        br.Print();

    // ---- Zusammenfassung ----
    std::cout << "\n--------------------------------------------------------\n";
    std::cout << "  Zusammenfassung  (write rows/s  |  read rows/s)\n";
    std::cout << "--------------------------------------------------------\n";
    for (auto& br : s_benchResults) {
        double wr = (br.writeMs > 0) ? (br.rowCount / (br.writeMs / 1000.0)) : 0;
        double rd = (br.readMs > 0) ? (br.rowCount / (br.readMs / 1000.0)) : 0;
        std::cout << "  " << std::left << std::setw(50) << br.label
            << "  W: " << std::right << std::setw(10) << std::fixed << std::setprecision(0) << wr
            << "  R: " << std::setw(10) << rd << "\n";
    }

    std::filesystem::remove_all(BENCH_DIR);
}

// ============================================================
//  main
// ============================================================

int main() {
    std::cout << "========================================\n";
    std::cout << "  OTNFile Tests\n";
    std::cout << "========================================\n";

    // Tests laufen automatisch über statische Konstruktoren

    std::cout << "----------------------------------------\n";
    std::cout << "  Ergebnis: " << s_passed << " bestanden, "
        << s_failed << " fehlgeschlagen\n";

    // ---- Slowest Tests ----
    {
        auto sorted = s_testResults;
        std::sort(sorted.begin(), sorted.end(),
            [](const TestResult& a, const TestResult& b) { return a.elapsedMs > b.elapsedMs; });
        const size_t TOP = std::min<size_t>(5, sorted.size());
        std::cout << "\n  Langsamste Unit-Tests:\n";
        for (size_t i = 0; i < TOP; ++i)
            std::cout << "    " << std::fixed << std::setprecision(2) << std::setw(8)
            << sorted[i].elapsedMs << " ms  " << sorted[i].name << "\n";
    }

    std::cout << "========================================\n";

    RunBenchmarks();

    std::cout << "\n========================================\n";
    std::cout << "  Fertig.\n";
    return s_failed > 0 ? 1 : 0;
}*/

/*#include <CoreLib/Log.h>
#include <SDLCoreLib/SDLCore.h>
#include <SDL3/SDL_main.h>

#include "App.h"

int main(int argc, char* argv[]) {
	App* app = new App();
	SDLCore::ApplicationResult result = app->Start();

	std::string msg = SDLCore::GetError(result);
	if(result == 0)
		Log::Info(msg);
	else
		Log::Error(msg);

	delete app;
	return 0;
}*/