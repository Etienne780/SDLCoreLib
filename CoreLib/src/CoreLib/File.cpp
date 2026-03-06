#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <unistd.h>
#include <limits.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#include <limits.h>
#endif

#include <algorithm>
#include <fstream>

#include "CoreLib/tinyfiledialogs.h"
#include <CoreLib/Log.h>
#include "CoreLib/File.h" 


File::File(const SystemFilePath& path)
    : m_path(path) {
}

File::~File() {
    Close();
}

void File::AddError(const std::string& msg) {
    if (!m_error.empty()) m_error += "\n";
    m_error += msg;
}

void File::AddWarning(const std::string& msg) {
    if (!m_warning.empty()) m_warning += "\n";
    m_warning += msg;
}

bool File::Open(FileState fileState, FileFlags flags) {
    if (fileState == FileState::FILE_CLOSE) {
        AddError("Open: FileState was set to FILE_CLOSE");
        return false;
    }

    if (m_path.empty()) {
        AddError("Open: Path was empty!");
        return false;
    }

    Close();

    m_fileState = fileState;
    m_flags = flags;

    if (fileState == FileState::FILE_WRITE) {
        std::ios_base::openmode mode = std::ios::out;
        if (flags & FileFlags::BINARY)
            mode |= std::ios::binary;
        if (flags & FileFlags::APPEND)
            mode |= std::ios::app;

        m_ofstream.open(m_path, mode);
        if (!m_ofstream.is_open()) {
            std::filesystem::create_directories(std::filesystem::path(m_path).parent_path());
            m_ofstream.open(m_path, mode);

            if (!m_ofstream.is_open()) {
                AddError("Open: Could not open '" + m_path.string() + "' for writing!");
                m_fileState = FileState::FILE_CLOSE;
                return false;
            }
        }
    }
    else {
        std::ios_base::openmode mode = std::ios::in;
        if (flags & FileFlags::BINARY)
            mode |= std::ios::binary;
        m_ifstream.open(m_path, mode);
        if (!m_ifstream.is_open()) {
            AddError("Open: Could not open '" + m_path.string() + "' for reading!");
            m_fileState = FileState::FILE_CLOSE;
            return false;
        }
    }

    return true;
}

void File::Close() {
    if (m_fileState == FileState::FILE_CLOSE)
        return;

    if (m_fileState == FileState::FILE_WRITE && m_ofstream.is_open()) {
        m_ofstream.close();
    }
    else if (m_fileState == FileState::FILE_READ && m_ifstream.is_open()) {
        m_ifstream.close();
    }

    m_fileState = FileState::FILE_CLOSE;
}

bool File::Write(const std::string& data) {
    if (m_fileState != FileState::FILE_WRITE || !m_ofstream.is_open()) {
        AddError("Write: File '" + m_path.string() + "' not open for writing!");
        return false;
    }

    m_ofstream << data;

    if (m_ofstream.fail()) {
        AddError("Write: Write operation failed on '" + m_path.string() + "'!");
        return false;
    }

    return true;
}

bool File::Write(const void* data, size_t size) {
    if (m_fileState != FileState::FILE_WRITE || !m_ofstream.is_open()) {
        AddError("Write: File '" + m_path.string() + "' not open for writing!");
        return false;
    }

    m_ofstream.write(reinterpret_cast<const char*>(data), size);

    if (m_ofstream.fail()) {
        AddError("Write: Write operation failed on '" + m_path.string() + "'!");
        return false;
    }

    return true;
}

bool File::ReadAll(std::string& outContent) {
    if (m_fileState != FileState::FILE_READ || !m_ifstream.is_open()) {
        AddError("ReadAll: File '" + m_path.string() + "' not open for reading!");
        return false;
    }

    m_ifstream.seekg(0, std::ios::beg);

    std::ostringstream ss;
    ss << m_ifstream.rdbuf();
    outContent = ss.str();

    return true;
}

bool File::ReadAll() {
    if (m_fileState != FileState::FILE_READ || !m_ifstream.is_open()) {
        AddError("ReadAll: File '" + m_path.string() + "' not open for reading!");
        return false;
    }

    m_ifstream.seekg(0, std::ios::beg);

    std::ostringstream ss;
    ss << m_ifstream.rdbuf();
    m_data = ss.str();

    return true;
}

bool File::ReadAllRaw(std::vector<unsigned char>& outData) {
    if (m_fileState != FileState::FILE_READ || !m_ifstream.is_open()) {
        AddError("ReadAllRaw: File '" + m_path.string() + "' not open for reading!");
        return false;
    }

    if (!(m_flags & FileFlags::BINARY)) {
        AddWarning("ReadAllRaw: File '" + m_path.string() + "' was not opened with FileFlags::BINARY!");
    }

    // Jump to end to get file size
    m_ifstream.seekg(0, std::ios::end);
    std::streamsize size = m_ifstream.tellg();
    if (size <= 0) {
        AddError("ReadAllRaw: File '" + m_path.string() + "' has invalid size!");
        return false;
    }

    // Allocate vector and read back
    outData.resize(static_cast<size_t>(size));
    m_ifstream.seekg(0, std::ios::beg);
    if (!m_ifstream.read(reinterpret_cast<char*>(outData.data()), size)) {
        AddError("ReadAllRaw: Read failed for '" + m_path.string() + "'");
        return false;
    }

    return true;
}

bool File::ReadAllRaw() {
    return ReadAllRaw(m_binaryData);
}

bool File::Exists() const {
    std::ifstream file(m_path);
    return file.good();
}

bool File::IsFileOpen() const {
    if (m_fileState == FileState::FILE_WRITE) {
        return m_ofstream.is_open();
    }
    else if (m_fileState == FileState::FILE_READ) {
        return m_ifstream.is_open();
    }
    return false;
}

bool File::HasWarning() const {
    return !m_warning.empty();
}

bool File::IsValid() const {
    return m_error.empty();
}

FileState File::GetFileState() const {
    return m_fileState;
}

FileFlags File::GetFileFlags() const {
    return m_flags;
}

std::string File::GetData() const {
    return m_data;
}

const std::string& File::GetDataRef() const {
    return m_data;
}

const std::vector<unsigned char>& File::GetRawData() const {
    return m_binaryData;
}

std::vector<unsigned char> File::GetRawData() {
    return m_binaryData;
}

size_t File::GetFileSize() const {
    std::ifstream file(m_path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        return 0;
    return static_cast<size_t>(file.tellg());
}

SystemFilePath File::GetFilePath() const {
    return m_path;
}

SystemFilePath File::GetParentPath() const {
    return m_path.parent_path();
}

std::string File::GetFileExtension() const {
    if (!m_path.has_extension())
        return "";
    std::string ext = m_path.extension().string().substr(1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

std::string File::GetFileName() const {
    return m_path.has_filename() ? m_path.filename().string() : "";
}

std::string File::GetError() const {
    return m_error;
}

std::string File::GetWarning() const {
    return m_warning;
}

File& File::SetFilePath(const SystemFilePath& path) {
    m_path = path;
    return *this;
}

std::string File::ToString() const {
    return GetFileName() + " (" + std::to_string(GetFileSize()) + " bytes)";
}

bool File::DeleteFile() {
    try {
        if (!std::filesystem::exists(m_path)) {
            AddWarning("DeleteFile: '" + m_path.string() + "' does not exist");
            return false;
        }

        if (!std::filesystem::is_regular_file(m_path)) {
            AddWarning("DeleteFile: '" + m_path.string() + "' is not a regular file");
            return false;
        }

        return std::filesystem::remove(m_path);
    }
    catch (const std::filesystem::filesystem_error& e) {
        AddError("DeleteFile: Failed to delete '" + m_path.string() + "': " + e.what());
        return false;
    }
}

bool File::CreateDir() {
    try {
        if (std::filesystem::exists(m_path))
            return std::filesystem::is_directory(m_path);

        return std::filesystem::create_directories(m_path);
    }
    catch (const std::filesystem::filesystem_error& e) {
        AddError("CreateDir: Failed to create '" + m_path.string() + "': " + e.what());
        return false;
    }
}

SystemFilePath File::SelectFolderDialog(const std::string& title, const SystemFilePath& defaultPath) {
    const char* result = tinyfd_selectFolderDialog(
        title.c_str(),
        defaultPath.empty() ? nullptr : defaultPath.string().c_str()
    );

    return result ? SystemFilePath(result) : SystemFilePath{};
}

SystemFilePath File::OpenFileDialog(const std::string& title, std::string filter) {
    return OpenFileDialog(title, filter.c_str(), {});
}

SystemFilePath File::OpenFileDialog(const std::string& title, const char* filter, const SystemFilePath& defaultPath) {
    const char* filterPatterns[1] = { nullptr };
    int patternCount = 0;

    if (filter && std::strlen(filter) > 0) {
        // Extract the second null-separated part of the filter string ("Description\0*.ext\0")
        const char* pattern = filter + std::strlen(filter) + 1;
        if (*pattern != '\0') {
            filterPatterns[0] = pattern;
            patternCount = 1;
        }
    }

    const char* result = tinyfd_openFileDialog(
        title.c_str(),
        defaultPath.empty() ? nullptr : defaultPath.string().c_str(),
        patternCount,
        (patternCount > 0) ? filterPatterns : nullptr,
        nullptr, // Optional description (not needed when using a combined filter string)
        0        // Multiple selection disabled
    );

    return result ? SystemFilePath(result) : SystemFilePath{};
}

SystemFilePath File::SaveFileDialog(const std::string& title, const char* filter, const SystemFilePath& defaultPath) {
    const char* filterPatterns[1] = { nullptr };
    int patternCount = 0;

    if (filter && std::strlen(filter) > 0) {
        // Extract the second null-separated part of the filter string ("Description\0*.ext\0")
        const char* pattern = filter + std::strlen(filter) + 1;
        if (*pattern != '\0') {
            filterPatterns[0] = pattern;
            patternCount = 1;
        }
    }

    const char* result = tinyfd_saveFileDialog(
        title.c_str(),
        defaultPath.empty() ? nullptr : defaultPath.string().c_str(),
        patternCount,
        (patternCount > 0) ? filterPatterns : nullptr,
        nullptr // Optional description (not needed when using a combined filter string)
    );

    return result ? SystemFilePath(result) : SystemFilePath{};
}

std::string File::ConvertFilterString(const std::string& extensions) {
    std::string result = "Custom Files";
    result.push_back('\0');
    std::string pattern;

    size_t start = 0;
    while (start < extensions.size()) {
        size_t end = extensions.find(',', start);
        if (end == std::string::npos) end = extensions.size();

        std::string ext = extensions.substr(start, end - start);
        // Trim whitespace
        ext.erase(0, ext.find_first_not_of(" \t"));
        ext.erase(ext.find_last_not_of(" \t") + 1);

        if (!ext.empty()) {
            if (!pattern.empty()) pattern += ';';
            // Ensure extension starts with '*'
            if (ext[0] != '*')
                pattern += '*';
            pattern += ext;
        }
        start = end + 1;
    }
    result += pattern;
    result.push_back('\0');
    return result;
}

std::filesystem::path File::GetExecutableDir() {
    return std::filesystem::path(GetExecutablePathInternal()).parent_path() / "";
}

std::filesystem::path File::GetExecutablePathInternal() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    DWORD size = GetModuleFileNameA(NULL, buffer, MAX_PATH);
    if (size == 0 || size == MAX_PATH)
        return {};
    return std::string(buffer, size);
#elif __linux__
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        return std::string(buffer);
    }
#elif __APPLE__
    char buffer[PATH_MAX];
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) == 0)
        return std::string(buffer);

    std::vector<char> dynBuffer(size);
    if (_NSGetExecutablePath(dynBuffer.data(), &size) == 0)
        return std::string(dynBuffer.data());
    return {};
#endif
    return {};
}