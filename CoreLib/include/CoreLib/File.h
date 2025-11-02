#pragma once
#include <fstream>
#include <vector>

// Represents the current open state of the file.
// Helps distinguish between read, write, or closed states.
enum FileState {
    FILE_CLOSE = 0, // No file is open
    FILE_WRITE,     // File is open for writing
    FILE_READ       // File is open for reading
};

// Constant to improve code readability when passing 'append' mode.
const bool APPEND_TO_FILE = true;

/**
 * @class File
 * @brief A simple wrapper around std::ifstream and std::ofstream
 *        that handles file opening, closing, reading, writing,
 *        and some basic file management operations.
 */
class File {
public:
    /**
     * @brief Constructs a File object with a given path.
     * @param path Path to the file.
     */
    File(const std::string& path);

    /**
     * @brief Destructor closes the file if still open.
     */
    ~File();

    /**
     * @brief Opens the file for reading or writing.
     * @param fileState FILE_WRITE or FILE_READ mode.
     * @param append If true, data will be appended to the file instead of overwriting.
     * @return True if the file was successfully opened, false otherwise.
     */
    bool Open(FileState fileState, bool append = false);

    /**
     * @brief Closes the file if it is open.
     */
    void Close();

    /**
     * @brief Writes a string to the file.
     * @param data The text data to write.
     * @return True on success, false on failure.
     */
    bool Write(const std::string& data);

    /**
     * @brief Writes raw binary data to the file.
     * @param data Pointer to the data buffer.
     * @param size Size of the data in bytes.
     * @return True on success, false on failure.
     */
    bool Write(const void* data, size_t size);

    /**
    * @brief Reads the entire file content into a string.
    * @param outContent String where the file's content will be stored.
    * @return True on success, false if file could not be read.
    */
    bool ReadAll(std::string& outContent);

    /**
    * @brief Reads the entire file content into a string called Data. (Replaces the old Data that was previously read)
    * @return True on success, false if file could not be read.
    */
    bool ReadAll();

    /**
    * @brief Reads the entire file content as raw binary data into the provided vector.
    *
    * This variant stores the result in the caller-provided buffer `outData`.
    * Useful when you want to manage the buffer yourself instead of storing it inside the File object.
    *
    * @param outData Vector where the binary content will be written.
    * @return True on success, false if file could not be read.
    */
    bool ReadAllBinary(std::vector<unsigned char>& outData);

    /**
    * @brief Reads the entire file content as raw binary data and stores it internally.
    *
    * The data will be stored in the member variable `m_binaryData`.
    * Use GetBinaryData() to access the loaded binary content after calling this function.
    *
    * @return True on success, false if file could not be read.
    */
    bool ReadAllBinary();


    /**
     * @brief Checks if the file exists (non-static version).
     * @return True if file exists, false otherwise.
     */
    bool Exists() const;

    /**
     * @brief Checks if the file is currently open (either read or write mode).
     * @return True if file is open, false otherwise.
     */
    bool IsFileOpen() const;

    /**
    * @brief Gets the Data that was previously read with ReadAll()
    * @return a copy of the data string
    */
    std::string GetData() const;

    /**
    * @brief Gets the Data that was previously read with ReadAll()
    * @return a ptr to the data string
    */
    const std::string& GetDataPtr();

    /**
    * @brief Returns the binary data previously read with ReadAllBinary().
    *
    * @return Const reference to the internal binary data buffer `m_binaryData`.
    */
    const std::vector<unsigned char>& GetBinaryData() const;

    /**
     * @brief Gets the size of the file in bytes.
     * @return The file size, or 0 if the file does not exist.
     */
    size_t GetFileSize() const;

    /**
     * @brief Gets the current file path.
     * @return The stored file path.
     */
    std::string GetFilePath() const;

    /*
    * @brief Gets the file Extension of a given path or filename
    * @param path path/filename
    * @return returns the file extension without the dot or nothing
    */
    std::string GetFileExtension() const;

    /*
    * @brief Gets the file name of a given path or filename
    * @param path path/filename
    * @return returns the file name or nothing
    */
    std::string GetFileName() const;

    /**
     * @brief Sets a new file path for this object.
     * @param path The new file path.
     * @return Reference to the current File object for chaining.
     */
    File& SetFilePath(const std::string& path);

    /*
    * @brief Gets the file Extension of a given path or filename (static version).
    * @param path path/filename
    * @return returns the file extension without the dot or nothing
    */
    static std::string GetFileExtension(const std::string& path);

    /*
    * @brief Gets the file name of a given path or filename (static version).
    * @param path path/filename
    * @return returns the file name or nothing
    */
    static std::string GetFileName(const std::string& path);

    /**
     * @brief Checks if a file exists at the given path (static version).
     * @param path File path to check.
     * @return True if the file exists, false otherwise.
     */
    static bool Exists(const std::string& path);

    /**
     * @brief Deletes a file at the given path
     * @param path File path to delete.
     * @return True if file was deleted successfully, false otherwise.
     */
    static bool DeleteFile(const std::string& path);

    /**
     * @brief Creates a directory (and any missing parent directories).
     * @param dir Directory path to create.
     * @return True if directory was created or already exists, false on error.
     */
    static bool CreateDir(const std::string& dir);

    /**
    * @brief Opens a native folder selection dialog.
    *
    * This uses tinyfiledialogs to show a system-native folder selection window.
    * If the user cancels, an empty string is returned.
    *
    * @param title       Title of the dialog window.
    * @param defaultPath Optional initial path shown in the dialog. If empty, the system default is used.
    * @return std::string Absolute path to the selected folder, or empty string if cancelled.
    */
    static std::string SelectFolderDialog(const std::string& title, const std::string& defaultPath = "");

    /**
    * @brief Opens a native file open dialog with an optional file filter.
    *        By default, the filter is set to "All Files".
    * 
    * Example:
    * @code
    * std::string path = File::OpenFileDialog(
    *     "Select document",
    *     "Text Files\0*.txt;*.md\0"
    * );
    * @endcode
    * 
    * @param title The title of the file open dialog window.
    * @param filter A null-separated C-string defining file filters. See tinyfiledialogs documentation for details, or use File::ConvertFilterString().
    * @return std::string The full path of the selected file, or empty string if cancelled.
    */
    static std::string OpenFileDialog(const std::string& title, const char* filter = "All Files\0*.*\0");

    /**
    * @brief Opens a native file open dialog with an initial directory and file filter.
    *
    * Example:
    * @code
    * std::string path = File::OpenFileDialog(
    *     "Select document",
    *     "Text Files\0*.txt;*.md\0",
    *     "C:/Users/Documents"
    * );
    * @endcode
    *
    * @param title The title of the file open dialog window.
    * @param defaultPath The initial directory or file path shown in the dialog.
    * @param filter A null-separated C-string defining file filters. Can be nullptr to show all files. See tinyfiledialogs documentation for details, or use File::ConvertFilterString().
    * @return std::string The full path of the selected file, or empty string if cancelled.
    */
    static std::string OpenFileDialog(const std::string& title, const char* filter, const std::string& defaultPath);

    /**
    * @brief Converts a simplified filter string like ".txt, .png" into the null-separated
    *        filter string required by tinyfiledialogs.
    *
    * This creates a filter description "Custom Files" with all extensions combined.
    * Example: ".txt, .png" -> "Custom Files\0*.txt;*.png\0"
    *
    * @param extensions Comma-separated extensions, e.g. ".txt, .png, .jpg"
    * @return std::string Filter string in tinyfiledialogs format.
    */
    static std::string ConvertFilterString(const std::string& extensions);

    /**
     * @brief Returns the directory path where the application executable resides.
     *
     * This function works across Windows, Linux, and macOS.
     * @return Absolute path to the executable's directory, without a trailing slash.
     */
    static std::string GetExecutableDir();

private:
    std::string m_path;         // Path to the file
    std::ifstream m_ifstream;   // Input file stream (for reading)
    std::ofstream m_ofstream;   // Output file stream (for writing)
    FileState m_fileState = FileState::FILE_CLOSE; // Current state of the file
    std::string m_data;
    std::vector<unsigned char> m_binaryData;

    static std::string GetExecutablePath();
};