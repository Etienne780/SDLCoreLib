#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <fstream>
#include <atomic>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#endif

#include "CoreLib\Log.h"
#include "CoreLib\TimeUtils.h"

std::vector<Log::Subscriber> Log::m_subscribers;
Log::SubscriberID Log::m_nextId = 0;
bool Log::m_saveLogs = false;
std::unique_ptr<Log::AsyncLogger> Log::m_asyncLogger = nullptr;

Log::SubscriberID Log::Subscribe(LogCallback callback) {
    m_subscribers.push_back({ ++m_nextId, callback });
    return m_nextId;
}

void Log::Unsubscribe(SubscriberID id) {
    m_subscribers.erase(
        std::remove_if(m_subscribers.begin(), m_subscribers.end(),
            [id](auto& sub) { return sub.id == id; }),
        m_subscribers.end()
    );
}

class Log::AsyncLogger::Impl {
public:
    Impl(const std::string& filename)
        : m_exitFlag(false), m_outFile(filename, std::ios::out | std::ios::app) {

        if (!m_outFile.is_open()) {
            std::filesystem::create_directories(std::filesystem::path(filename).parent_path());
            m_outFile.open(filename, std::ios::out | std::ios::app);
        }

        m_workerThread = std::thread(&Impl::ProcessQueue, this);
    }

    ~Impl() {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_exitFlag = true;
            m_cv.notify_all();
        }
        if (m_workerThread.joinable())
            m_workerThread.join();
        if (m_outFile.is_open())
            m_outFile.close();
    }

    void Log(const std::string& message) {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_queue.push(message);
        }
        m_cv.notify_one();
    }

private:
    void ProcessQueue() {
        while (true) {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait(lock, [this]() { return !m_queue.empty() || m_exitFlag; });

            while (!m_queue.empty()) {
                m_outFile << m_queue.front() << std::endl;
                m_queue.pop();
            }

            if (m_exitFlag)
                break;
        }
    }

    std::ofstream m_outFile;
    std::queue<std::string> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::thread m_workerThread;
    std::atomic<bool> m_exitFlag;
};

Log::AsyncLogger::AsyncLogger(const std::string& filename)
    : pImpl(std::make_unique<Impl>(filename)) {
}

Log::AsyncLogger::~AsyncLogger() = default;

void Log::AsyncLogger::Log(const std::string& message) {
    pImpl->Log(message);
}

void Log::ClearLog() {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD count;
    DWORD cellCount;

    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) 
        return;
    cellCount = csbi.dwSize.X * csbi.dwSize.Y;
    if (!FillConsoleOutputCharacter(hConsole, (TCHAR)' ', cellCount, { 0,0 }, &count)) 
        return;
    if (!FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, { 0,0 }, &count))
        return;
    SetConsoleCursorPosition(hConsole, { 0,0 });
#else
    // ANSI Escape Codes for Linux/macOS/Windows 10+
    std::cout << "\033[2J\033[H" << std::flush;
#endif
}

void Log::SaveLogs(const std::string& path) {
    if (path.empty()) return;

    std::string basePath = path;

    size_t pos = basePath.find_last_of('.');
    if (pos != std::string::npos) {
        basePath = basePath.substr(0, pos);
    }
    size_t lastPos = basePath.size() - 1;

    std::string seperater = "_";
    if (basePath[lastPos] == '/' || basePath[lastPos] == '\\') {
        seperater = "";
    }

    std::string fullPath = basePath + seperater + TimeUtils::GetCurrentDateString() + seperater + TimeUtils::GetCurrentTimeString() + ".log";

    if (!m_asyncLogger) {
        m_asyncLogger = std::make_unique<AsyncLogger>(fullPath);
        m_saveLogs = true;
    }
}


void Log::m_print(const Level& logLevel, const std::string& message) {
    std::cout << message << std::endl;

    for (auto& sub : m_subscribers) {
        sub.callback(logLevel, message);
    }

    if (m_saveLogs) {
        m_asyncLogger->Log(message);
    }
}

bool Log::IsLogLevelEnabled(Level level) {
    switch (level) {
    case Level::levelError: return m_levelError;
    case Level::levelWarning: return m_levelWarning;
    case Level::levelInfo: return m_levelInfo;
    case Level::levelDebug: return m_levelDebug;
    default: return true;
    }
}