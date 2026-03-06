#pragma once
#include <fstream>
#include <vector>
#include <filesystem>
#include "FormatUtils.h"

// Represents the current open state of the file.
// Helps distinguish between read, write, or closed states.
enum FileState {
    FILE_CLOSE = 0, // No file is open
    FILE_WRITE,     // File is open for writing
    FILE_READ       // File is open for reading
};

enum class FileFlags : uint8_t {
    NONE = 0,
    APPEND = 1 << 0,
    BINARY = 1 << 1
};

inline FileFlags operator|(FileFlags a, FileFlags b) {
    return static_cast<FileFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}
inline bool operator&(FileFlags a, FileFlags b) {
    return static_cast<uint8_t>(a) & static_cast<uint8_t>(b);
}

typedef std::filesystem::path SystemFilePath;

/**
* @class File
* @brief A simple wrapper around std::ifstream and std::ofstream
*        that handles file opening, closing, reading, writing,
*        and some basic file management operations.
*
* Errors and warnings are stored internally and can be retrieved
* via GetError() and GetWarning().
*/
class File {
public:
    /**
    * @brief Constructs a File object with a given path.
    * @param path Path to the file.
    */
    File(const SystemFilePath& path);

    File(const File&) = delete;
    File& operator=(const File&) = delete;

    /**
    * @brief Destructor closes the file if still open.
    */
    ~File();

    /**
    * @brief Opens the file for reading or writing.
    * @param fileState FILE_WRITE or FILE_READ mode.
    * @param flags Optional flags to control open behavior (e.g. FileFlags::BINARY, FileFlags::APPEND).
    * @return True if the file was successfully opened, false otherwise. 
    *         Use GetError() to retrieve the error message.
    */
    bool Open(FileState fileState, FileFlags flags = FileFlags::NONE);

    /**
    * @brief Closes the file if it is open.
    */
    void Close();

    /**
    * @brief Writes a string to the file.
    * @param data The text data to write.
    * @return True on success, false on failure.
    *         Use GetError() to retrieve the error message.
    */
    bool Write(const std::string& data);

    /**
    * @brief Writes raw binary data to the file.
    * @param data Pointer to the data buffer.
    * @param size Size of the data in bytes.
    * @return True on success, false on failure.
    *         Use GetError() to retrieve the error message.
    */
    bool Write(const void* data, size_t size);

    /**
    * @brief Reads the entire file content into a string.
    * @param outContent String where the file content will be stored.
    * @return True on success, false if file could not be read.
    *         Use GetError() to retrieve the error message.
    */
    bool ReadAll(std::string& outContent);

    /**
    * @brief Reads the entire file content and stores it internally.
    *
    * The result can be accessed via GetData() or GetDataRef() after this call.
    * Replaces any previously read data.
    *
    * @return True on success, false if file could not be read.
    *         Use GetError() to retrieve the error message.
    */
    bool ReadAll();

    /**
    * @brief Reads the entire file content as raw binary data into a caller-provided buffer.
    *
    * Useful when you want to manage the buffer yourself instead of storing it
    * inside the File object.
    *
    * note: File must have been opened with FileFlags::BINARY.
    * 
    * @param outData Vector where the binary content will be written.
    * @return True on success, false if file could not be read.
    *         Use GetError() to retrieve the error message. 
    *         If opened without FileFlags::BINARY, a warning is set (see GetWarning()).
    */
    bool ReadAllRaw(std::vector<unsigned char>& outData);

    /**
    * @brief Reads the entire file content as raw binary data and stores it internally.
    *
    * The data can be accessed via GetRawData() after this call.
    * Replaces any previously read binary data.
    * 
    * note: File must have been opened with FileFlags::BINARY.
    *
    * @return True on success, false if file could not be read.
    *         Use GetError() to retrieve the error message.
    *         If opened without FileFlags::BINARY, a warning is set (see GetWarning()).
    */
    bool ReadAllRaw();

    /**
    * @brief Checks whether the file at the current path exists on disk.
    * @return True if the file exists, false otherwise.
    */
    bool Exists() const;

    /**
    * @brief Checks if the file is currently open (either read or write mode).
    * @return True if the file is open, false otherwise.
    */
    bool IsFileOpen() const;

    /**
    * @brief Returns true if a warning was recorded during the last operation.
    * @return True if a warning is present, false otherwise. 
    *         Use GetWarning() to retrieve the warning
    */
    bool HasWarning() const;

    /**
    * @brief Returns true if no error has occurred so far.
    * @return True if no error is present, false otherwise.
    *         Use GetError() to retrieve the error
    */
    bool IsValid() const;

    /**
    * @brief Returns the current open state of the file.
    * @return FileState: FILE_CLOSE, FILE_READ, or FILE_WRITE.
    */
    FileState GetFileState() const;

    /**
    * @brief Returns the flags that were used to open the file.
    * @return The active FileFlags.
    */
    FileFlags GetFileFlags() const;

    /**
    * @brief Returns the text data previously read with ReadAll().
    * @return A copy of the internal data string.
    */
    std::string GetData() const;

    /**
    * @brief Returns the text data previously read with ReadAll().
    * @return A const reference to the internal data string.
    */
    const std::string& GetDataRef() const;

    /**
    * @brief Returns the binary data previously read with ReadAllRaw().
    * @return Const reference to the internal binary data buffer.
    */
    const std::vector<unsigned char>& GetRawData() const;

    /**
    * @brief Returns the binary data previously read with ReadAllRaw().
    * @return A copy of the internal binary data buffer.
    */
    std::vector<unsigned char> GetRawData();

    /**
    * @brief Gets the size of the file in bytes.
    * @return The file size in bytes, or 0 if the file could not be opened.
    */
    size_t GetFileSize() const;

    /**
    * @brief Returns the current file path.
    * @return The stored file path.
    */
    SystemFilePath GetFilePath() const;

    /**
    * @brief Returns the parent directory of the current file path.
    * @return SystemFilePath The path to the parent directory.
    */
    SystemFilePath GetParentPath() const;

    /**
    * @brief Returns the file extension of the current file path, in lowercase without the dot.
    * @return The file extension (e.g. "txt"), or an empty string if there is none.
    */
    std::string GetFileExtension() const;

    /**
    * @brief Returns the filename of the current file path including its extension.
    * @return The filename (e.g. "data.bin"), or an empty string if there is none.
    */
    std::string GetFileName() const;

    /**
    * @brief Returns the last error message, or an empty string if no error occurred.
    * @return The error message string.
    */
    std::string GetError() const;

    /**
    * @brief Returns the last warning message, or an empty string if no warning occurred.
    * @return The warning message string.
    */
    std::string GetWarning() const;

    /**
    * @brief Sets a new file path for this object.
    * @param path The new file path.
    * @return Reference to the current File object for chaining.
    */
    File& SetFilePath(const SystemFilePath& path);

    /**
    * @brief Returns a string representation of this file like 'filename.txt (1024 bytes)'.
    * @return Formatted string with filename and file size.
    */
    std::string ToString() const;

    /**
    * @brief Deletes the file at the current path.
    * @return True if the file was deleted successfully, false otherwise.
    *         Use GetError() or GetWarning() to retrieve more details.
    */
    bool DeleteFile();

    /**
    * @brief Creates the directory at the current path, including any missing parent directories.
    * @return True if the directory was created or already exists, false on error.
    *         Use GetError() to retrieve the error message.
    */
    bool CreateDir();

    /**
    * @brief Opens a native folder selection dialog.
    *
    * Uses tinyfiledialogs to show a system-native folder selection window.
    * If the user cancels, an empty path is returned.
    *
    * @param title       Title of the dialog window.
    * @param defaultPath Optional initial path shown in the dialog. If empty, the system default is used.
    * @return SystemFilePath Absolute path to the selected folder, or empty path if cancelled.
    */
    SystemFilePath SelectFolderDialog(const std::string& title, const SystemFilePath& defaultPath = {});

    /**
    * @brief Opens a native file open dialog with an optional file filter.
    *
    * Example:
    * @code
    * SystemFilePath path = file.OpenFileDialog(
    *     "Select document",
    *     "Text Files\0*.txt;*.md\0"
    * );
    * @endcode
    *
    * @param title  Title of the dialog window.
    * @param filter Optional null-separated filter string. Use ConvertFilterString() to build one easily.
    * @return SystemFilePath The selected file path, or empty path if cancelled.
    */
    SystemFilePath OpenFileDialog(const std::string& title, std::string filter = "All Files\0*.*\0");

    /**
    * @brief Opens a native file open dialog with a filter and a default directory.
    *
    * Example:
    * @code
    * SystemFilePath path = file.OpenFileDialog(
    *     "Select Image",
    *     "Image Files\0*.png;*.jpg;*.bmp\0",
    *     "C:/Users/User/Pictures"
    * );
    * @endcode
    *
    * @param title       Title of the dialog window.
    * @param filter      Null-separated filter string (see tinyfiledialogs docs).
    * @param defaultPath Optional initial directory. If empty, the system default is used.
    * @return SystemFilePath The selected file path, or empty path if cancelled.
    */
    SystemFilePath OpenFileDialog(const std::string& title, const char* filter, const SystemFilePath& defaultPath);

    /**
    * @brief Opens a native save file dialog with an optional file filter.
    *
    * Example:
    * @code
    * SystemFilePath savePath = file.SaveFileDialog(
    *     "Save image as",
    *     "Image Files\0*.png;*.jpg;*.bmp\0",
    *     "C:/Users/User/Pictures"
    * );
    * if (!savePath.empty()) {
    *     // Save to savePath
    * }
    * @endcode
    *
    * @param title       Title of the dialog window.
    * @param filter      Null-separated filter string (e.g. "Text Files\0*.txt;*.md\0").
    * @param defaultPath Optional initial directory or filename to display.
    * @return SystemFilePath The selected file path, or empty path if cancelled.
    */
    SystemFilePath SaveFileDialog(const std::string& title, const char* filter = "All Files\0*.*\0", const SystemFilePath& defaultPath = {});

    /**
    * @brief Converts a simplified comma-separated extension list into a
    *        null-separated filter string as required by tinyfiledialogs.
    *
    * Example: ".txt, .png" -> "Custom Files\0*.txt;*.png\0"
    *
    * @param extensions Comma-separated extension list, e.g. ".txt, .png, .jpg"
    * @return std::string Filter string in tinyfiledialogs format.
    */
    std::string ConvertFilterString(const std::string& extensions);

    /**
    * @brief Returns the directory path where the application executable resides.
    *
    * Works on Windows, Linux, and macOS.
    *
    * note: Always check whether the returned path is empty before using it.
    * 
    * @return Absolute path to the executable's directory, with a trailing slash.
    *         Returns an empty path if the executable path could not be determined.
    */
    SystemFilePath GetExecutableDir();

private:
    SystemFilePath m_path;
    std::ifstream m_ifstream;
    std::ofstream m_ofstream;
    FileState m_fileState = FileState::FILE_CLOSE;
    FileFlags m_flags = FileFlags::NONE;
    std::string m_data;
    std::vector<unsigned char> m_binaryData;

    std::string m_error;
    std::string m_warning;

    static SystemFilePath GetExecutablePathInternal();

    void AddError(const std::string& msg);
    void AddWarning(const std::string& msg);
};

template<>
inline std::string FormatUtils::toString<SystemFilePath>(SystemFilePath path) {
    return path.string();
}

template<>
inline std::string FormatUtils::toString<FileState>(FileState state) {
    switch (state) {
    case FILE_CLOSE: return "FILE_CLOSE";
    case FILE_WRITE: return "FILE_WRITE";
    case FILE_READ:  return "FILE_READ";
    default:         return "UNKNOWN";
    }
}