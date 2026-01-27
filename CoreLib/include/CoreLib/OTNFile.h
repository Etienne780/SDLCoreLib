#pragma once
#include <vector>
#include <array>
#include <fstream>
#include <string>
#include <variant>
#include <memory>
#include <optional>
#include <filesystem>
#include <cstdint>
#include <string_view>
#include <unordered_map>
#include <sstream>
#include <iomanip>

/**
* @file OTNFile.h
* @brief OTN (Object Tokenized Notation) - A file format library for structured data serialization
*
* OTN is a text-based file format for storing object data in a tabular style with type safety.
* It supports primitive types (int, float, double, bool, string), nested objects,
* and multi-dimensional arrays.
*
* @section usage Basic Usage
*
* **Writing data:**
* @code
OTN::OTNObject obj("MyData");
obj.SetNames("id", "name", "value")
	.AddDataRow(1, "Item1", 42.5)
    .AddDataRow(2, "Item2", 17.3);

OTN::OTNWriter writer;
writer.AppendObject(obj)
	.Save("data.otn");
* @endcode
*
* **Reading data:**
* @code
OTN::OTNReader reader;
if (reader.ReadFile("data.otn")) {
	auto obj = reader.TryGetObject("MyData");
	if (obj) {
		int id = obj->GetValue<int>(0, "id");
		std::string name = obj->GetValue<std::string>(0, "name");
		// Or
		int id = obj->GetValue<int>(0, "id", 0);
		std::string name = obj->GetValue<std::string>(0, "name", "defaultVal");
		// Or
		std::optional<int> id = obj->TryGetValue<int>(0, "id");
		std::optional<std::string> name = obj->TryGetValue<std::string>(0, "name");

	}
}
* @endcode
*
* @section custom_types Custom Types
*
* This section explains how to define serialization and deserialization
* for custom types using template specialization.
*
* Notes:
* - Objects must provide a default constructor.
* - Loading uses the order in which elements are added.
*   Changing this order will affect save/load compatibility.
*
* Basic example:
* @code
struct Vector2 { float x, y; };

template<>
void OTN::ToOTNDataType<Vector2>(OTN::OTNObjectBuilder& obj, Vector2& v) {
    obj.SetObjectName("Vector2");
    obj.AddNames("x", "y");
    obj.AddData(v.x, v.y);
}
* @endcode
*
* Advanced example using IsBuildingOTNObject():
* This allows independent control over save and load behavior.
*
* @code
template<>
void OTN::ToOTNDataType<Vector2>(OTN::OTNObjectBuilder& obj, Vector2& e) {
    obj.SetObjectName("Vector2");
    obj.AddNames("list");

    if (obj.IsBuildingOTNObject()) {
        // Serialization
        obj.AddData(std::vector<float>{ e.x, e.y });
    } else {
        // Deserialization
        std::vector<float> tmp;
        obj.AddData(tmp);

        if (tmp.size() < 2)
            return;

        e.x = tmp[0];
        e.y = tmp[1];
    }
}
* @endcode
*
* @version 1.0
*/

/**
* @brief Object Tokenized Notation - A structured data serialization library
*
* The OTN namespace provides classes and utilities for:
* - Creating structured tabular data (OTNObject)
* - Serializing to .otn files (OTNWriter)
* - Deserializing from .otn files (OTNReader)
* - Custom type support via template specialization
*
* @section thread_safety Thread Safety
* All classes are NOT thread-safe. Use external synchronization.
*
* @section exceptions Exception Safety
* - Most functions provide basic exception guarantee
* - GetValue<T>() may throw std::out_of_range or std::bad_cast
* - File operations may throw std::filesystem exceptions
*/
namespace OTN {

	/// @brief Current version of the OTN format.
	inline constexpr uint8_t VERSION = 1;

	/// @brief Standard file extension for OTN files.
	inline constexpr std::string_view FILE_EXTENSION = ".otn";

	namespace Syntax {

		inline constexpr char STATEMENT_TERMINATOR	= ';';
		inline constexpr char KEYWORD_PREFIX_CHAR	= '@';
		inline constexpr char KEYWORD_ASSIGN_CHAR	= ':';
		inline constexpr char ASSIGNMENT_CHAR		= '=';
		inline constexpr char SEPARATOR_CHAR		= ',';
		inline constexpr char TYPE_SEPARATOR_CHAR	= '/';
		inline constexpr char BLOCK_BEGIN_CHAR		= '{';
		inline constexpr char BLOCK_END_CHAR		= '}';
		inline constexpr char LIST_BEGIN_CHAR		= '[';
		inline constexpr char LIST_END_CHAR			= ']';
		inline constexpr char REF_BEGIN_CHAR		= '<';
		inline constexpr char REF_END_CHAR			= '>';

		namespace Types {
			inline constexpr std::string_view INT = "int";
			inline constexpr std::string_view FLOAT = "float";
			inline constexpr std::string_view DOUBLE = "double";
			inline constexpr std::string_view BOOL = "bool";
			inline constexpr std::string_view STRING = "String";
			inline constexpr std::string_view OBJECT = "object";
			inline constexpr std::string_view OBJECT_REF = "object_ref";
			inline constexpr std::string_view LIST = "list";
		}
	}

	namespace Keyword {

		inline constexpr std::string_view VERSION_KW	= "version";
		inline constexpr std::string_view DEF_TYPE_KW	= "defType";
		inline constexpr std::string_view DEF_NAME_KW	= "defName";

		inline constexpr std::string_view OBJECT_KW		= "object";
		inline constexpr std::string_view REF_KW		= "Ref";

		inline constexpr std::string_view TRUE_KW		= "true";
		inline constexpr std::string_view FALSE_KW		= "false";

	}

	/// @brief Type alias for file system paths
	using OTNFilePath = std::filesystem::path;

	/// @brief Enum representing the fundamental data types supported by OTN.
	enum class OTNBaseType {
		UNKNOWN = 0,
		INT,
		FLOAT,
		DOUBLE,
		BOOL,
		STRING,
		OBJECT,
		OBJECT_REF,
		LIST
	};

	/**
	* @brief Type descriptor with support for nested arrays
	*
	* Describes a data type including base type, nesting depth for arrays,
	* and optional object type name for custom types.
	*
	* Examples:
	* 
	* - int: baseType=INT, listDepth=0
	* 
	* - int[]: baseType=INT, listDepth=1
	* 
	* - int[][]: baseType=INT, listDepth=2
	* 
	* - CustomObj: baseType=OBJECT, refObjectName="CustomObj"
	*/
	struct OTNTypeDesc {
		OTNBaseType baseType = OTNBaseType::UNKNOWN;
		uint32_t listDepth = 0;
		std::string refObjectName; // only valid if baseType == OBJECT

		OTNTypeDesc() = default;

		/**
		* @brief Construct type descriptor for primitive or list type
		* @param type Base data type
		* @param depth Array nesting depth (default: 0)
		*/
		OTNTypeDesc(OTNBaseType type, uint32_t depth = 0) 
			: baseType(type), listDepth(depth) {
		}

		/**
		* @brief Construct type descriptor for custom object type
		* @param type Object type name
		* @param depth Array nesting depth (default: 0)
		*/
		OTNTypeDesc(const std::string& type, uint32_t depth = 0)
			: baseType(OTNBaseType::OBJECT), refObjectName(type), listDepth(depth) {
		}
	};

	/**
	* @brief Internal reference to an object instance
	* Used during deserialization to resolve object references
	*/
	struct OTNObjectRef {
		std::string refObjectName;
		int index = 0;
		
		OTNObjectRef() = default;

		/**
		* @brief Construct object reference
		* @param _objName Name of referenced object type
		* @param _index Instance index
		*/
		OTNObjectRef(const std::string& _objName, int _index) 
			: refObjectName(_objName), index(_index) {
		}
	};

	/**
	* @brief Convert string to OTNBaseType
	* @param type Type name (e.g., "int", "float", "String")
	* @return Corresponding OTNBaseType or UNKNOWN if not recognized
	*/
	OTNBaseType StringToOTNBaseType(const std::string& type);

	/**
	* @brief Convert OTNBaseType to string representation
	* @param type Base type enum value
	* @return String representation (e.g., "int", "float")
	*/
	constexpr std::string_view OTNValueTypeToString(OTNBaseType type) noexcept;

	/**
	* @brief Get character length of type name
	* @param type Base type enum value
	* @return Length of type name string
	*/
	constexpr uint32_t OTNValueTypeCharLength(OTNBaseType type) noexcept;

	/**
	* @brief Convert type descriptor to string representation
	* @param type Type descriptor
	* @return String like "int", "float[][]", or "CustomType"
	*/
	std::string TypeDescToString(const OTNTypeDesc& type);

	#pragma region OTNObject

	class OTNObject;
	using OTNObjectPtr = std::shared_ptr<OTNObject>;
	
	class OTNArray;
	using OTNArrayPtr = std::shared_ptr<OTNArray>;
	
	using OTNValueVariant = std::variant<
		int,
		float,
		double,
		bool,
		std::string,
		OTNObjectPtr,
		OTNArrayPtr,
		OTNObjectRef
	>;
	
	template<typename T>
	struct OTNTypeOf {
		static constexpr OTNBaseType value = OTNBaseType::UNKNOWN;
	};

	template<> struct OTNTypeOf<int> {
		static constexpr OTNBaseType value = OTNBaseType::INT;
	};

	template<> struct OTNTypeOf<float> {
		static constexpr OTNBaseType value = OTNBaseType::FLOAT;
	};

	template<> struct OTNTypeOf<double> {
		static constexpr OTNBaseType value = OTNBaseType::DOUBLE;
	};

	template<> struct OTNTypeOf<bool> {
		static constexpr OTNBaseType value = OTNBaseType::BOOL;
	};

	template<> struct OTNTypeOf<std::string> {
		static constexpr OTNBaseType value = OTNBaseType::STRING;
	};

	template<> struct OTNTypeOf<OTNObjectPtr> {
		static constexpr OTNBaseType value = OTNBaseType::OBJECT;
	};

	template<> struct OTNTypeOf<OTNObjectRef> {
		static constexpr OTNBaseType value = OTNBaseType::OBJECT_REF;
	};

	template<> struct OTNTypeOf<OTNArrayPtr> {
		static constexpr OTNBaseType value = OTNBaseType::LIST;
	};

	template<typename T>
	inline constexpr OTNBaseType GetType() {
		return OTNTypeOf<std::decay_t<T>>::value;
	}

	inline OTNBaseType GetTypeFromVariant(const OTNValueVariant& variant) {
		return std::visit(
			[](const auto& value) constexpr -> OTNBaseType {
				using T = std::decay_t<decltype(value)>;
				return GetType<T>();
			},
			variant
		);
	}

	/**
	* @brief Container for a single data value of any supported type
	*/
	struct OTNValue {
		OTNValueVariant value;
		OTNBaseType type = OTNBaseType::UNKNOWN;

		OTNValue() = default;

		/**
		* @brief Construct OTNValue from variant
		* @param v Value variant
		*/
		explicit OTNValue(OTNValueVariant v)
			: value(std::move(v)),
			type(GetTypeFromVariant(value)) {
		}
	};

	/**
	* @brief Container for array/list values
	* Used internally to represent lists
	*/
	class OTNArray {
	public:
		std::vector<OTNValue> values;
	};
	
	template<typename>
	inline constexpr bool otn_always_false_v = false;
	
	template<typename T>
	struct otn_is_std_vector : std::false_type {};

	template<typename U, typename Alloc>
	struct otn_is_std_vector<std::vector<U, Alloc>> : std::true_type {};

	template<typename T>
	static constexpr bool is_otn_variant_v =
	std::is_same_v<std::decay_t<T>, OTNValueVariant>;

	template<typename T>
	struct is_otn_base_type : std::false_type {};
	
	template<> struct is_otn_base_type<int> : std::true_type {};
	template<> struct is_otn_base_type<float> : std::true_type {};
	template<> struct is_otn_base_type<double> : std::true_type {};
	template<> struct is_otn_base_type<bool> : std::true_type {};
	template<> struct is_otn_base_type<std::string> : std::true_type {};
	template<> struct is_otn_base_type<const char*> : std::true_type {};
	template<size_t N> struct is_otn_base_type<const char[N]> : std::true_type {};
	template<> struct is_otn_base_type<OTNObjectRef> : std::true_type {};
	
	template<typename T>
	inline constexpr bool is_otn_base_type_v = is_otn_base_type<T>::value;
	
	template<typename T>
	struct is_otn_list : std::false_type {};
	
	template<typename T, typename Alloc>
	struct is_otn_list<std::vector<T, Alloc>> : std::true_type {};
	template<typename T, std::size_t N>
	struct is_otn_list<std::array<T, N>> : std::true_type {};
	
	template<typename T>
	inline constexpr bool is_otn_list_v = is_otn_list<T>::value;

	/// Type alias for a row of data values
	using OTNRow = std::vector<OTNValue>;

	/**
	* @brief Represents a structured data object in the OTN format.
	* 
	* An OTNObject works like a table or a dataframe. It has a name, a set of column names,
	* column types, and a list of rows containing data.
	* It is the primary data structure for manipulating data in memory before saving or after loading.
	* 
	* notes:
	* 
	* - Column names must be set before adding data rows.
	* 
	* - Types can be auto-deduced from the first data row if not explicitly set.
	* 
	* - Object can become invalid while build use IsValid, GetError or TryGetError for more information
	*/
	class OTNObject {
		friend class OTNObjectBuilder;
	public:
		/**
		* @brief Construct a new OTN object with given name
		* 
		* note:
		*
		* - Name must not start with a digit and must not contain special characters
		* 
		* @param name Unique identifier for this object type
		*/
		explicit OTNObject(const std::string& name);

		OTNObject(const OTNObject& other) noexcept;
		OTNObject& operator=(const OTNObject& other) noexcept;

		OTNObject(OTNObject&& other) noexcept;
		OTNObject& operator=(OTNObject&& other) noexcept;

		~OTNObject() = default;
	
		/**
		* @brief Set column names using variadic arguments
		* 
		* note:
		* 
		* - Must be called before AddDataRow()
		* 
		* - Names must not contain special characters or start with digits
		* 
		* @tparam Args String-convertible types
		* @param names Column names (must be unique)
		* @return Reference to this object for method chaining
		*/
		template<typename ...Args>
		OTNObject& SetNames(Args&&... names) {
			static_assert((std::is_convertible_v<Args, std::string> && ...),
				"All names must be convertible to std::string");

#ifndef NDEBUG
			if (!m_dataRows.empty()) {
				AddError(
					"SetNames must be called before AddData in object '" + m_name + "'!"
				);
				return *this;
			}
#endif
			auto addName = [&](const std::string& n) {
				if (DebugIsNameValid(n))
					m_columnNames.emplace_back(n);
				};

			constexpr size_t ArgCount = sizeof...(Args);
			m_columnNames.clear();
			m_columnNames.reserve(ArgCount);

			(addName(std::forward<Args>(names)), ...);

			DebugValidateNamesDistinct();
			return *this;
		}

		/**
		* @brief Set column names from a vector
		* 
		* note:
		* 
		* - Must be called before AddDataRow()
		* 
		* @param names Vector of column names
		* @return Reference to this object for method chaining
		*/
		OTNObject& SetNamesList(const std::vector<std::string>& names);

		/**
		* @brief Explicitly sets the data types for the columns.
		*
		* If this function is not called, column types are deduced from
		* the first row of data added.
		*
		* Supported type strings include for example:
		* "int", "float", "String", "objectName", "int[]".
		*
		* Use "-", "", or "_" to indicate skip the definition of this type.
		* Type will automaticly be deduced if a row is inserted.
		*
		* notes:
		* 
		* - This function must be called before AddData().
		*       Calling it after data has been added will result in an error.
		*
		* - On failure, this object will contain an error state.
		*       More information can be retrieved via GetError() or TryGetError().
		* 
		* @tparam Args Variadic list of type strings.
		* @param types One type string per column.
		*
		* @return Reference to this object for method chaining
		*/
		template<typename ...Args>
		OTNObject& SetTypes(Args&&... types) {
			static_assert((std::is_convertible_v<Args, std::string> && ...),
				"All types must be convertible to std::string");

#ifndef NDEBUG
			if (!m_dataRows.empty()) {
				AddError(
					"SetTypes must be called before AddData in object '" + m_name + "'!"
				);
				return *this;
			}
#endif
			constexpr size_t ArgCount = sizeof...(Args);
			
			std::vector<OTNTypeDesc> tmp;
			tmp.reserve(ArgCount);

			auto addType = [&](const std::string& t) {
				return AddSingleType(tmp, t);
			};

			if (!(addType(std::forward<Args>(types)) && ...)) {
				return *this; // Do not modify m_types on failure
			}

			m_columnTypes = std::move(tmp);
			return *this;
		}

		/**
		* @brief Explicitly sets the data types for the columns.
		*
		* If this function is not called, column types are deduced from
		* the first row of data added.
		*
		* Supported type strings include for example:
		* "int", "float", "String", "objectName", "int[]".
		*
		* Use "-", "", or "_" to indicate skip the definition of this type.
		* Type will automaticly be deduced if a row is inserted.
		*
		* notes:
		*
		* - This function must be called before AddData().
		*       Calling it after data has been added will result in an error.
		*
		* - On failure, this object will contain an error state.
		*       More information can be retrieved via GetError() or TryGetError().
		* 
		* @param types List of type strings, one entry per column.
		*
		* @return Reference to this object for method chaining
		*/
		OTNObject& SetTypesList(const std::vector<std::string>& types);

		/**
		* @brief Add a data row using variadic arguments
		* 
		* notes:
		* 
		* - Number of arguments must match number of columns
		* 
		* - First row is used for type deduction if types weren't explicitly set
		* 
		* @tparam Args Value types (must match column types)
		* @param args Values for each column
		* @return Reference to this object for method chaining
		*/
		template<typename... Args>
		OTNObject& AddDataRow(Args&&... args) {
			constexpr size_t ArgCount = sizeof...(Args);

#ifndef NDEBUG
			if (m_columnNames.empty()) {
				AddError("AddDataRow called before SetNames in object '" + m_name + "'!");
				return *this;
			}

			if (m_columnTypes.size() > m_columnNames.size()) {
				AddError("Cant have more types (count: "
					+ std::to_string(m_columnTypes.size())
					+ ") defined as names (count: "
					+ std::to_string(m_columnNames.size()) + ")!");
				return *this;
			}

			if (ArgCount != m_columnNames.size()) {
				AddError("AddDataRow argument count (" + std::to_string(ArgCount) +
					") does not match name count (" +
					std::to_string(m_columnNames.size()) +
					") in object '" + m_name + "'!");
				return *this;
			}
#endif

			OTNRow row;
			row.reserve(ArgCount);

			bool rowValid = true;
			auto addValue = [&](auto&& value) { 
				AddSingleValueToRow(row, std::forward<decltype(value)>(value), rowValid); 
			};
			(addValue(std::forward<Args>(args)), ...);

			if (rowValid) {
				m_dataRows.emplace_back(std::move(row));
			}

			if (!m_deducedColumnTypes) {
				if (!DeduceTypesFromRow(m_dataRows.back()))
					return *this;
			}

#ifndef NDEBUG
			DebugValidateDataTypes(m_dataRows.size() - 1);
#endif
			return *this;
		}

		/**
		* @brief Add a data row using a OTNROW
		*
		* notes:
		*
		* - Number of arguments must match number of columns
		*
		* - First row is used for type deduction if types weren't explicitly set
		*
		* @param values Values for each column
		* @return Reference to this object for method chaining
		*/
		OTNObject& AddDataRowList(const OTNRow& values);

		/**
		* @brief Pre-allocate storage for data rows
		* @param amount Number of rows to reserve space for
		* @return Reference to this object for method chaining
		*/
		OTNObject& ReserveDataRows(size_t amount);
	
		/**
		* @brief Check if object is in a valid state
		* @return true if no errors occurred during construction
		*/
		bool IsValid() const;

		/**
		* @brief Try to retrieve error message
		* @param[out] outError Error message if any
		* @return true if an error exists
		*/
		bool TryGetError(std::string& outError);

		/**
		* @brief Get accumulated error messages
		* @return Error message string (empty if no errors)
		*/
		std::string GetError() const;

		/**
		* @brief Get the objects name
		* @return Object name/identifier
		*/
		std::string GetName() const;

		/**
		* @brief Returns the number of columns based on the defined column names.
		*
		* This reflects the logical column count of the object schema,
		* 
		* independent of any stored data rows.
		*
		* @return Number of columns defined by column names.
		*/
		size_t GetColumnCount() const;

		/**
		* @brief Returns the number of columns for a specific data row.
		*
		* This reflects the actual number of values stored in the given row.
		*
		* @param rowIndex Index of the data row.
		* @return Number of columns in the specified row.
		*
		* @throws std::out_of_range if the row index is invalid.
		*/
		size_t GetColumnCount(size_t rowIndex) const;

		/**
		* @brief Get number of data rows
		* @return Row count
		*/
		size_t GetRowCount() const;

		/**
		* @brief Get column names (read-only)
		* @return Const reference to column names vector
		*/
		const std::vector<std::string>& GetColumnNames() const;

		/**
		* @brief Get column type descriptors (read-only)
		* @return Const reference to column types vector
		*/
		const std::vector<OTNTypeDesc>& GetColumnTypes() const;

		/**
		* @brief Get all data rows (read-only)
		* @return Const reference to data rows vector
		*/
		const std::vector<OTNRow>& GetDataRows() const;

		/**
		* @brief Get column names (mutable)
		* 
		* WARNING: Modifying names after adding data may cause inconsistencies
		* 
		* @return Reference to column names vector
		*/
		std::vector<std::string>& GetColumnNames();

		/**
		* @brief Get column type descriptors (mutable)
		* 
		* WARNING: Modifying types after adding data may cause inconsistencies
		* 
		* @return Reference to column types vector
		*/
		std::vector<OTNTypeDesc>& GetColumnTypes();

		/**
		* @brief Get all data rows (mutable)
		* @return Reference to data rows vector
		*/
		std::vector<OTNRow>& GetDataRows();

		/**
		* @brief Get a specific row (read-only)
		* @param index Row index (0-based)
		* @return Const reference to row
		* @throws std::out_of_range if index is invalid
		*/
		const OTNRow& GetRow(size_t index) const;

		/**
		* @brief Get a specific row (mutable)
		* @param index Row index (0-based)
		* @return Reference to row
		* @throws std::out_of_range if index is invalid
		*/
		OTNRow& GetRow(size_t index);

		/**
		* @brief Get a typed value by row and column name
		* @tparam T Expected value type
		* @param row Row index (0-based)
		* @param columnName Name of the column
		* @return Deserialized value of type T
		* @throws std::out_of_range if row or column doesn't exist
		* @throws std::bad_cast if type conversion fails
		*/
		template<typename T>
		T GetValue(size_t row, const std::string& columnName) const {
			auto colId = GetColumnID(columnName);
			if (!colId)
				throw std::out_of_range("column not found");

			return GetValue<T>(row, *colId);
		}

		/**
		* @brief Get a typed value by row and column index
		* @tparam T Expected value type
		* @param row Row index (0-based)
		* @param column Column index (0-based)
		* @return Deserialized value of type T
		* @throws std::out_of_range if row or column doesn't exist
		* @throws std::bad_cast if type conversion fails
		*/
		template<typename T>
		T GetValue(size_t row, size_t column) const {
			if (row >= m_dataRows.size())
				throw std::out_of_range("row out of bounds");

			const OTNRow& r = m_dataRows[row];
			if (column >= r.size())
				throw std::out_of_range("column out of bounds");

			auto opt = TryDeserializeValue<T>(r[column]);
			if (!opt)
				throw std::bad_cast();

			return *opt;
		}

		/**
		* @brief Get a typed value with default fallback by row and column name
		* @tparam T Expected value type
		* @param row Row index
		* @param objName Column name
		* @param defaultValue Value to return if retrieval fails
		* @return Retrieved value or defaultValue
		*/
		template<typename T>
		T GetValue(size_t row, const std::string& objName, const T& defaultValue) const {
			auto colId = GetColumnID(objName);
			if (!colId)
				return defaultValue;
			
			auto opt = TryGetValue<T>(row, *colId);
			return (opt) ? *opt : defaultValue;
		}


		/**
		* @brief Get a typed value with default fallback by row and column index
		* @tparam T Expected value type
		* @param row Row index
		* @param column Column index
		* @param defaultValue Value to return if retrieval fails
		* @return Retrieved value or defaultValue
		*/
		template<typename T>
		T GetValue(size_t row, size_t column, const T& defaultValue) const {
			auto opt = TryGetValue<T>(row, column);
			return (opt) ? *opt : defaultValue;
		}

		/**
		* @brief Try to get a typed value by row and column name
		* @tparam T Expected value type
		* @param row Row index
		* @param columnName Column name
		* @return std::optional containing value if successful, std::nullopt otherwise
		*/
		template<typename T>
		std::optional<T> TryGetValue(size_t row, const std::string& columnName) const {
			auto colId = GetColumnID(columnName);
			if (!colId)
				return std::nullopt;

			return TryGetValue<T>(row, *colId);
		}

		/**
		* @brief Try to get a typed value by row and column index
		* @tparam T Expected value type
		* @param row Row index
		* @param column Column index
		* @return std::optional containing value if successful, std::nullopt otherwise
		*/
		template<typename T>
		std::optional<T> TryGetValue(size_t row, size_t column) const {
			if (row >= m_dataRows.size())
				return std::nullopt;

			const OTNRow& r = m_dataRows[row];
			if (column >= r.size())
				return std::nullopt;

			return TryDeserializeValue<T>(r[column]);
		}

		/**
		* @brief Internal helper to convert an OTNValue variant into a C++ type T.
		*
		* Supports:
		* 
		* - Primitive types (int, float, double, bool, string)
		* 
		* - Vectors/lists (including nested lists)
		* 
		* - Custom objects via ToOTNDataType specialization
		*
		* @tparam T The target type
		* @param val The source OTNValue
		* @return std::optional with the converted value, or std::nullopt on failure
		*/
		template<typename T>
		std::optional<T> TryDeserializeValue(const OTNValue& val) const {
			using DT = std::decay_t<T>;

			// Base types
			if constexpr (is_otn_base_type_v<DT>) {
				try {
					return std::get<DT>(val.value);
				}
				catch (...) {
					return std::nullopt;
				}
			}
			// List
			else if constexpr (otn_is_std_vector<DT>::value) {
				if (val.type != OTNBaseType::LIST)
					return std::nullopt;

				OTNArrayPtr ptr = std::get<OTNArrayPtr>(val.value);
				if (!ptr)
					return std::nullopt;

				DT result;
				using ElemType = typename DT::value_type;
				result.reserve(ptr->values.size());

				for (const OTNValue& elem : ptr->values) {
					auto conv = ConvertOTNValue<ElemType>(elem);
					if (!conv)
						return std::nullopt;
					result.push_back(std::move(*conv));
				}
				return result;
			}
			// Object (custom type)
			else {
				if (val.type != OTNBaseType::OBJECT)
					return std::nullopt;

				OTNObjectPtr ptr = std::get<OTNObjectPtr>(val.value);
				if (!ptr || ptr->GetRowCount() != 1)
					return std::nullopt;

				OTNObjectBuilder builder{ *ptr };
				T result{};
				ToOTNDataType<DT>(builder, result);

				if (!builder.IsValid()) {
					AddError(builder.GetError());
					return std::nullopt;
				}
				return result;
			}
		}
		
	private:
		std::string m_name;
		mutable std::string m_error;
		mutable bool m_valid = true;
	
		bool m_deducedColumnTypes = false;
		std::vector<std::string> m_columnNames;
		std::vector<OTNTypeDesc> m_columnTypes;
		std::vector<OTNRow> m_dataRows;
	
		/**
		* @brief Returns the column index for a given column name.
		*
		* Searches the list of defined column names and returns the index
		* of the first matching entry.
		*
		* @param name Column name to search for.
		* @return Column index if found, otherwise std::nullopt.
		*/
		std::optional<size_t> GetColumnID(const std::string& name) const;
		
		/**
		* @brief Adds a single value to an output row with type validation.
		*
		* This function accepts either:
		* 
		* - An already constructed OTNValue
		* 
		* - A supported OTN variant type
		* 
		* - A native type that can be wrapped in OTNDataType
		*
		* On failure, the row is marked as invalid and will no be added, also an error is reported.
		*
		* @tparam U Type of the value to add.
		* @param outRow Target row where the value will be appended.
		* @param value Value to add.
		* @param outRowValid Set to false if validation fails.
		*/
		template<typename U>
		void AddSingleValueToRow(OTNRow& outRow, U&& value, bool& outRowValid) {
			using DT = std::decay_t<U>;
			
			if constexpr (std::is_same_v<DT, OTNValue>) {
				outRow.emplace_back(std::forward<U>(value));
			}
			else if constexpr (is_otn_variant_v<DT>) {
				outRow.emplace_back(std::forward<U>(value));
			}
			else {
				OTNDataType<DT> data(std::forward<U>(value));
				if (!data.m_valid) {
					outRowValid = false;
					AddError("Invalid data in object '" + m_name + "':\n" + data.m_error);
					return;
				}
				outRow.emplace_back(std::move(data.m_value));
			}
		}

		void AddError(const std::string& error) const;
		void SetNamesFromBuilder(std::vector<std::string>&& names);
		void AddRowInternal(OTNRow&& row);
		bool AddSingleType(std::vector<OTNTypeDesc>& tempList, const std::string& typeStr);
		bool DeduceTypesFromRow(const OTNRow& row);

		bool DebugIsNameValid(const std::string& name);
		// validates if each column name is distinct
		bool DebugValidateNamesDistinct();
		// validates if each data type of each row matchs the type of the first row at a specifc column
		bool DebugValidateDataTypes(size_t rowIndex);
	};
	
	/**
	* @brief Helper class for serializing and deserializing objects to/from OTNObjects.
	*
	* This class operates in two modes:
	* 
	* 1. Building an OTNObject from an object of type T (serialization).
	* 
	* 2. Building an object of type T from an existing OTNObject (deserialization).
	*
	* The mode is determined by the constructor used.
	*/
	class OTNObjectBuilder {
	public:
		/**
		* @brief Default constructor for building an OTNObject from T.
		*/
		OTNObjectBuilder() = default;

		/**
		* @brief Constructor for building an object of type T from an existing OTNObject.
		* @param obj Source OTNObject to deserialize from.
		*/
		OTNObjectBuilder(const OTNObject& obj);
		~OTNObjectBuilder() = default;
	
		/**
		* @brief Sets the name of the object being built.
		* @param name Name of the object.
		*
		* In serialization mode, sets the object name.
		* 
		* In deserialization mode, checks for name consistency and reports an error if it differs.
		*/
		void SetObjectName(const std::string& name);
	
		/**
		* @brief Adds multiple column names to the object.
		* @tparam Args Variadic list of column name types convertible to std::string.
		* @param names Column names to add.
		* @return Reference to self for method chaining.
		*/
		template<typename... Args>
		OTNObjectBuilder& AddNames(Args&&... names) {
			static_assert((std::is_convertible_v<Args, std::string> && ...),
				"All names must be convertible to std::string");
	
			if (m_otnObjectFromT) {
				(AddName(std::forward<Args>(names)) && ...);
			}
			return *this;
		}
	
		/**
		* @brief Adds multiple data values corresponding to the columns.
		* @tparam Args Variadic list of values to add.
		* @param args Data values.
		* @return Reference to self for method chaining.
		*/
		template<typename... Args>
		OTNObjectBuilder& AddData(Args&&... args) {			
			(AddSingleData(std::forward<Args>(args)) && ...);
			return *this;
		}
	
		/**
		* @brief Adds a single column name and its corresponding value.
		* 
		* Checks that names and data are in sync. Reports errors if not.
		* 
		* @tparam T Type of the value.
		* @param name Name of the column.
		* @param data Value to add.
		* @return Reference to self for chaining.
		*/
		template<typename T>
		OTNObjectBuilder& AddNameData(const std::string& name, T&& data) {		
			if (!m_otnObjectFromT) {
				AddSingleData<T>(data);
				return *this;
			}
			
			if (IsDataOutOfSync()) {
				AddError(
					"Error adding Name and Data (AddNameData): data and names are not in sync (" +
					std::to_string(m_data.size()) + " != " +
					std::to_string(m_dataNames.size()) + ") in object '" +
					m_objectName + "'!"
				);
				return *this;
			}
			
			if (!AddName(name)) {
				return *this;
			}
	
			AddSingleData<T>(data);
			return *this;
		}
		
		/**
		* @brief Returns true if the builder is in serialization mode (T → OTNObject).
		*/
		bool IsBuildingOTNObject() const;

		/**
		* @brief Returns true if the builder is valid (no errors encountered).
		*/
		bool IsValid() const;

		/**
		* @brief Returns the current error string.
		*/
		std::string GetError() const;
	
		/**
		* @brief Converts the builder into an OTNObject (serialization).
		* @return Constructed OTNObject.
		* @throws std::runtime_error if called in deserialization mode.
		*/
		OTNObject ToOTNObject()&&;
	
	private:
		bool m_otnObjectFromT = true;/* determins in which mode the object build ist, build OTNObject from t or build t from OTNObject*/
		bool m_valid = true;
		std::string m_error;
		std::string m_objectName;
	
		std::vector<std::string> m_dataNames;
		std::vector<OTNValue> m_data;
		size_t m_dataPos = 0;/*< is used when T is set from OTNBuilder to detrimend from which position the data should come from*/
	
		void AddError(const std::string& error);

		/**
		* @brief Adds a single column name to the builder.
		* @param name Name to add.
		* @return True if the name was successfully added (unique in serialization mode).
		*/
		bool AddName(std::string name);
		
		/**
		* @brief Adds a single data value to the row, handling serialization or deserialization.
		* @tparam T Type of the value.
		* @param data Value to add.
		* @return True if the operation succeeded.
		*/
		template<typename T>
		bool AddSingleData(T& data) {
			if (m_otnObjectFromT) {
				// Serialize: T -> OTNObject
				OTNDataType<std::decay_t<T>> wrappedData(std::forward<T>(data));
				if (!wrappedData.m_valid) {
					AddError("OTNObjectBuilder: failed to serialize data: " + wrappedData.m_error);
					return false;
				}
				m_data.emplace_back(std::move(wrappedData.m_value));
			}
			else {
				// Deserialize: OTNObject -> T
				if (m_dataPos >= m_data.size()) {
					AddError("OTNObjectBuilder: insufficient data to extract value");
					return false;
				}
				OTNValue& val = m_data[m_dataPos++];
				if (!DeserializeFromOTNValue<T>(val, data)) {
					AddError("OTNObjectBuilder: failed to deserialize value");
					return false;
				}
			}
			return true;
		}

		/**
		* @brief Deserializes an OTNValue into the provided variable.
		* @tparam T Target type.
		* @param val Source OTNValue.
		* @param data Target variable.
		* @return True if deserialization succeeded.
		*/
		template<typename T>
		bool DeserializeFromOTNValue(const OTNValue& val, T& data) {
			using DT = std::decay_t<T>;

			if constexpr (is_otn_base_type_v<DT>) {
				// Base type
				try {
					data = std::get<DT>(val.value);
				}
				catch (...) {
					return false;
				}
				return true;
			}
			else if constexpr (std::is_same_v<DT, OTNObject>) {
				// Custom object
				if (val.type != OTNBaseType::OBJECT) 
					return false;
				OTNObjectPtr& ptr = std::get<OTNObjectPtr>(val.value);
				if (!ptr) 
					return false;
				data = *(ptr.get());
				return true;
			}
			else if constexpr (is_otn_list_v<DT>) {
				// List or nested list
				data.clear();
				using ElemType = typename DT::value_type;

				std::vector<OTNValue> elements;
				if (val.type == OTNBaseType::LIST) {
					OTNArrayPtr arrayPtr = std::get<OTNArrayPtr>(val.value);
					if (!arrayPtr) 
						return false;
					elements = arrayPtr->values;
				}
				else {
					elements.push_back(val); // single value treated as list
				}

				for (auto& elemVal : elements) {
					ElemType elem;
					if constexpr (is_otn_list_v<ElemType> || !is_otn_base_type_v<ElemType>) {
						if (!DeserializeFromOTNValue<ElemType>(elemVal, elem))
							return false;
						data.push_back(std::move(elem));
					}
					else {
						try {
							data.push_back(std::get<ElemType>(elemVal.value));
						}
						catch (...) {
							return false;
						}
					}
				}
				return true;
			}// if type ist not specified check if it can be made with the object builder
			else if (val.type == OTNBaseType::OBJECT) {
				OTNObjectPtr ptr = std::get<OTNObjectPtr>(val.value);
				if (!ptr)
					return false;

				if (ptr->GetRowCount() > 1)
					return false;

				OTNObjectBuilder builder{ *ptr };
				ToOTNDataType<DT>(builder, data);
				if (!builder.IsValid()) {
					AddError(builder.GetError());
					return false;
				}
				return true;
			}
			return false;
		}
	
		/**
		* @brief Checks if a name is unique in the current object.
		* @param name Name to check.
		* @return True if the name is unique.
		*/
		bool IsNameUnique(const std::string& name) const;

		/**
		* @brief Checks if the number of column names matches the number of data values.
		* @return True if data is out of sync with column names.
		*/
		bool IsDataOutOfSync() const;
	};
	
	/**
	* @brief Template for converting between T and OTNObject.
	* 
	* note:
	* 
	* - Must be specialized for user-defined types.
	* 
	* @tparam T Type to serialize/deserialize.
	*/
	template<typename T>
	void ToOTNDataType(OTNObjectBuilder&, T&) {
		static_assert(otn_always_false_v<T>, "Unsupported type for ToOTNDataType");
	}

	/**
	* @brief Template for converting a value of type U into an OTNValue.
	* @tparam U Type to wrap.
	*
	* Handles:
	* - Base types
	* - Lists (vectors)
	* - Nested objects
	*/
	template<typename U>
	class OTNDataType {
	public:
		// accept both lvalues and rvalues
		explicit OTNDataType(const U& value) {
			Construct(std::forward<const U&>(value));
		}

		explicit OTNDataType(U&& value) {
			Construct(std::forward<U>(value));
		}

	private:
		void Construct(const U& value) {
			using DT = std::decay_t<U>;
			if constexpr (is_otn_list_v<DT>) {
				OTNArrayPtr otnArray = std::make_shared<OTNArray>();
				otnArray->values.reserve(value.size());
				for (auto&& elem : value) {
					OTNDataType<std::decay_t<decltype(elem)>> elemData(elem);
					if (!elemData.m_valid) {
						AddError(elemData.m_error);
						return;
					}
					otnArray->values.emplace_back(std::move(elemData.m_value));
				}
				m_value = otnArray;
			}
			else if constexpr (std::is_same_v<DT, OTNObject>) {
				m_value = std::make_shared<OTNObject>(value); // copy
			}
			else if constexpr (std::is_same_v<DT, const char*>) {
				m_value = std::string(value);
			}
			else if constexpr (is_otn_base_type_v<DT>) {
				m_value = value;
			}
			else {
				OTNObjectBuilder builder;
				// cast const, because ToOTNDataType needs a none const reference
				ToOTNDataType<DT>(builder, static_cast<U>(value));
				if (!builder.IsValid()) {
					AddError(builder.GetError());
					return;
				}
				OTNObjectPtr ptr = std::make_shared<OTNObject>(std::move(builder).ToOTNObject());

				if (!ptr->IsValid())
					AddError(ptr->GetError());

				m_value = ptr;
			}
		}

	public:
		OTNValueVariant m_value;
		bool m_valid = true;
		std::string m_error;

		void OTNDataType::AddError(const std::string& error) {
			m_valid = false;
			if (!m_error.empty())
				m_error += "\n";
			m_error += error;
		}
	};

	#pragma endregion
	
	#pragma region OTNWriter

	/**
	* @brief Serializes and writes OTNObjects to a file.
	*
	* This class provides fine-grained control over the writing process,
	* including optimizations, default name/type usage, and row deduplication.
	*/
	class OTNWriter {
	public:
		/**
		* @brief Default constructor.
		*/
		explicit OTNWriter() = default;
		/**
		* @brief Destructor.
		*/
		~OTNWriter() = default;
	
		/**
		* @brief Replace frequently used strings with numeric IDs to save space.
		* @param value True to enable, false to store full strings.
		* @return Reference to self for chaining.
		*/
		OTNWriter& UseDefName(bool value);

		/**
		* @brief Replace frequently used type names with numeric IDs to save space.
		* @param value True to enable, false to store full type names.
		* @return Reference to self for chaining.
		*/
		OTNWriter& UseDefType(bool value);

		/**
		* @brief Enable or disable optimizations (removes spaces, line breaks).
		* @param value True to enable, false to disable.
		* @return Reference to self for method chaining.
		*/
		OTNWriter& UseOptimizations(bool value);

		/**
		* @brief Enable or disable deduplication of identical rows.
		* @param value True to enable, false to disable.
		* @return Reference to self for method chaining.
		*/
		OTNWriter& UseDeduplicateRows(bool value);
	
		/**
		* @brief Append an OTNObject to the writer.
		* @param object Object to append.
		* @return Reference to self for method chaining.
		*
		* In debug mode, ensures no duplicate object names exist.
		*/
		OTNWriter& AppendObject(const OTNObject& object);
	
		/**
		* @brief Save all appended objects to a file.
		* 
		* Validates the objects and path, writes the data, and stores any errors internally.
		* 
		* @param path Absolute file path including file name (e.g., "file.otn" or "file").
		*             Parent directories will be created automatically if they do not exist.
		* @return True if saving succeeded, false otherwise. Retrieve more information via GetError() or TryGetError() 
		*/
		bool Save(const OTNFilePath& path);
	
		/**
		* @brief Returns whether default names are used.
		*/
		bool GetUseDefName() const;

		/**
		* @brief Returns whether default types are used.
		*/
		bool GetUseDefType() const;

		/**
		* @brief Returns whether optimizations are enabled.
		*/
		bool GetUseOptimizations() const;

		/**
		* @brief Returns whether row deduplication is enabled.
		*/
		bool GetDeduplicateRows() const;
		
		/**
		* @brief Returns true if the writer is valid (no errors occurred).
		*/
		bool IsValid() const;

		/**
		* @brief Attempts to get the current error message.
		* @param outError String to store the error.
		* @return True if an error exists, false if the writer is valid.
		*/
		bool TryGetError(std::string& outError) const;

		/**
		* @brief Returns the current error message string.
		*/
		std::string GetError();
	
	private:
		struct SerializedObject {
			using Row = std::vector<OTNValue>;
			std::vector<std::string> columnNames;
			std::vector<OTNTypeDesc> columnTypes;

			// uses map to add only unique rows and vector for stable indices
			std::vector<Row> rows;
			std::unordered_map<std::size_t, std::size_t> rowIndexByHash;

			// Adds row if not already present, returns index
			size_t AddOrGetRow(const Row& row, bool deduplicateRows);
		private:
			static size_t CreateRowHash(const std::vector<OTNTypeDesc>& columnTypes, const Row& row);
			// static size_t HashValue(const SerializedValue& serValue);
			static size_t HashValue(const OTNTypeDesc& colType, const OTNValue& value);
		};

		class BufferedIndentedStream {
		public:
			std::ofstream stream;
			uint32_t indentLevel = 0;
			std::string indentStr = "\t";
			bool newLine = true;
			std::string buffer;

			static constexpr size_t BUFFER_SIZE = 64 * 1024; // 64 KB

			BufferedIndentedStream() = default;
			BufferedIndentedStream(const std::string& path) : stream(path, std::ios::binary) {}

			void NewLine() { newLine = true; }
			void IncreaseIndent() { ++indentLevel; }
			void DecreaseIndent() { if (indentLevel > 0) --indentLevel; }

			void Flush() {
				if (!buffer.empty() && stream.is_open()) {
					stream.write(buffer.data(), buffer.size());
					buffer.clear();
				}
			}

			template<typename T>
			BufferedIndentedStream& operator<<(const T& value) {
				if (newLine) {
					for (uint32_t i = 0; i < indentLevel; ++i)
						buffer += indentStr;
					newLine = false;
				}

				buffer += OTNWriter::ToString(value);
				if (buffer.size() >= BUFFER_SIZE)
					Flush();

				return *this;
			}
		};

		struct WriterData {
			bool created = false;
			BufferedIndentedStream stream;

			std::unordered_map<OTNBaseType, uint32_t> typeUsage;// < map contains which data types are used and how often
			std::unordered_map<std::string, SerializedObject> objects;

			std::unordered_map<std::string, uint32_t> defType;// < used for optimaziations: Replaceses comman used type names with numbers
			std::unordered_map<std::string, uint32_t> defName;// < used for optimaziations: Replaceses comman used names of Objects like position with numbers

			void Reset() {
				if (stream.stream.is_open())
					stream.stream.close();
				created = false;
				typeUsage.clear();
				objects.clear();
			}
		};

		bool m_useDefName = false;// < replaces often used names with number
		bool m_useDefType = false;// < replaces often used type names with numbers
		bool m_useOptimizations = false;// < (Removes spaces, linebreaks)
		bool m_useDeduplicateRows = false;

		std::vector<OTNObject> m_objects;
		std::string m_error;
		bool m_valid = true;

		WriterData m_writerData;
		
		bool DebugValidateObjects();

		bool WriteToFile(const OTNFilePath& path);
		bool CreateWriteData(WriterData& data);
		size_t AddObject(WriterData& data, OTNObject& object);
		void ConvertToSerValue(WriterData& data, OTNValue& result, const OTNTypeDesc& colType, const OTNValue& val);
		bool CreateDefType();
		bool CreateDefName();

		bool WriteHeader();
		bool WriteHeaderDefType();
		bool WriteHeaderDefName();

		template<typename Func>
		void WriteDirective(BufferedIndentedStream& stream, std::string_view keyword, Func&& func) {
			stream << Syntax::KEYWORD_PREFIX_CHAR << keyword << Syntax::KEYWORD_ASSIGN_CHAR;
			AddSpace(stream);
			std::forward<Func>(func)();
			stream << Syntax::STATEMENT_TERMINATOR;
		}

		void WriteHeaderDefHelper(BufferedIndentedStream& stream, const std::unordered_map<std::string, uint32_t>& map);
		bool WriteBody();
		bool WriteObjects(BufferedIndentedStream& stream, const std::unordered_map<std::string, SerializedObject>& objects);

		void WriteOTNValueData(std::string& outStr, const OTNValue& data);

		template<typename T>
		void WriteData(std::string& outStr, const T& data);

		void AddSpace(BufferedIndentedStream& stream) const;
		void AddIndent(BufferedIndentedStream& stream, uint32_t level = 1) const;
		void AddLineBreak(BufferedIndentedStream& stream) const;

		void AddSpace(std::string& outStr) const;
		void AddIndent(std::string& outStr, uint32_t level = 1) const;

		void AddError(const std::string& error, bool linebreak = true);

		void CountObjectType(const SerializedObject& obj, std::unordered_map<OTNBaseType, uint32_t>& typeUsage);
		static OTNTypeDesc DeduceColumnType(const OTNValue& value);

		// Helper: convert basic types to string
		template<typename T>
		static std::string ToString(const T& val) {
			if constexpr (std::is_same_v<T, std::string>) {
				return val;
			}
			else if constexpr (std::is_same_v<T, std::string_view> || 
				std::is_same_v<T, const char*>) {
				return std::string(val);
			}
			else if constexpr (std::is_same_v<T, char>) {
				return std::string(&val, 1); // single char -> string
			}
			else if constexpr (std::is_arithmetic_v<T>) {
				return ToStringNumber<T>(val);
			}
			else {
				return std::string(val);
			}
		}

		template<typename T>
		static std::string ToStringNumber(T value) {
			std::ostringstream oss;

			// Use maximum precision but avoid trailing zeros
			oss << std::setprecision(std::numeric_limits<T>::digits10)
				<< std::defaultfloat
				<< value;

			return oss.str();
		}
	};
	
	#pragma endregion
	
	
	#pragma region OTNReader

	/**
	* @brief Reads OTN files and provides access to stored OTNObjects.
	*
	* Supports file validation, version checking, and error reporting.
	*/
	class OTNReader {
	public:
		/**
		* @brief Default constructor.
		*/
		explicit OTNReader() = default;

		/**
		* @brief Destructor.
		*/
		~OTNReader() = default;
	
		/**
		* @brief Read an OTN file from the specified path.
		* 
		* Validates the path, opens the file, and reads its data.
		* 
		* @param path Absolute path including file name (e.g., "file.otn" or "file").
		* @return True if saving succeeded, false otherwise. Retrieve more information via GetError() or TryGetError()
		*/
		bool ReadFile(const OTNFilePath& path);
		
		/**
		* @brief Returns the version of the OTN file.
		* @return Version number; values <= 0 indicate an invalid version.
		*/
		uint8_t GetVersion() const;

		/**
		* @brief Retrieves a stored OTNObject by name.
		* @param objName Name of the object to retrieve.
		* @return Optional containing the object if found, std::nullopt otherwise.
		*/
		std::optional<OTNObject> TryGetObject(const std::string& objName) const;

		/**
		* @brief Get all loaded OTNObjects (read-only).
		* 
		* Use this to inspect objects without modifying them.
		* 
		* @return Const reference to the internal map of object names to OTNObjects.
		*/
		const std::unordered_map<std::string, OTNObject>& GetObjects() const;

		/**
		* @brief Get all loaded OTNObjects (modifiable).
		* 
		* Use this to modify or insert objects after reading.
		* 
		* @return Reference to the internal map of object names to OTNObjects.
		*/
		std::unordered_map<std::string, OTNObject>& GetObjects();

		/**
		* @brief Returns true if the reader is in a valid state (no errors).
		*/
		bool IsValid() const;

		/**
		* @brief Returns the current error message string.
		*/
		std::string GetError() const;

		/**
		* @brief Attempts to get the current error message.
		* @param outError Reference to store the error string.
		* @return True if an error exists, false if the reader is valid.
		*/
		bool TryGetError(std::string& outError) const;

	private:
		enum class TokenType {
			UNKNOWN = 0,
			KEYWORD_PREFIX,	// @
			IDENTIFIER,		// object, version, Weapon
			NUMBER,			// 123
			STRING,			// "Metal"
			MINUS,			// -
			COLON,          // :
			EQUALS,         // =
			COMMA,          // ,
			SLASH,			// /
			SEMICOLON,		// ;
			BLOCK_BEGIN,	// {
			BLOCK_END,		// }
			LIST_BEGIN,		// [
			LIST_END,		// ]
			REF_BEGIN,		// <
			REF_END,		// >
			END_OF_FILE
		};

		enum class TokenKeyword {
			UNKNOWN = 0,
			VERSION,
			DEF_TYPE,
			DEF_NAME,
			OBJECT
		};

		class ReaderData {
		public:
			ReaderData() = default;
			~ReaderData() = default;

			std::ifstream stream;

			uint8_t version = 0;
			std::unordered_map<std::string, OTNObject> objects;

			std::unordered_map<uint32_t, std::string> defType;// < used for optimaziations: Replaceses comman used type names with numbers
			std::unordered_map<uint32_t, std::string> defName;// < used for optimaziations: Replaceses comman used names of Objects like position with numbers

			void Reset() {
				if (stream.is_open())
					stream.close();
				
				version = 0;
				objects.clear();
			}
		};

		class Token {
		public:
			Token() = default;
			Token(TokenType t, const std::string& str, uint32_t l, uint32_t c)
				: type(t), text(str), line(l), column(c) {
			}

			TokenType type = TokenType::UNKNOWN;
			std::string text;
			uint32_t line = 0;
			uint32_t column = 0;
		};

		class OTNTokenizer {
		public:
			explicit OTNTokenizer(std::ifstream& stream)
				: m_stream(stream) {
			}

			bool Tokenize();
			const std::vector<Token>& GetTokens() const;
			std::string GetError() const;

		private:
			std::ifstream& m_stream;
			std::vector<Token> m_tokens;
			std::string m_error;
			bool m_valid = false;

			uint32_t m_line = 1;
			uint32_t m_column = 1;

			bool AddToken(TokenType type, char c);
			bool AddToken(TokenType type, const std::string& text);
			bool AddToken(TokenType type, const std::string& text, uint32_t line, uint32_t column);
			
			bool ReadString();
			bool ReadNumber();
			bool ReadIdentifier();

			void Advance(char c);
			void AddError(const std::string& msg);
		};

		#pragma region ReaderV_Num

		class OTNReaderV1 {
		public:
			explicit OTNReaderV1(ReaderData& data, const std::vector<Token>& tokens) 
				: m_data(data), m_tokens(tokens) {
			}

			bool Read();
			std::string GetError() const;
			bool IsValid() const;

		private:
			OTNReader::ReaderData& m_data;
			const std::vector<Token>& m_tokens;
			std::string m_error;
			bool m_valid = true;

			size_t m_pos = 0;

			bool ParseTopLevel();
			bool ParseDefType();
			bool ParseDefName();
			bool ParseObjectBlock();

			bool ParseObject();
			bool ParseHeaderBlock(OTNObject& obj);
			bool ParseDataRows(OTNObject& obj, size_t rowCount);

			bool ResolveOTNObjectRefs();
			bool ResolveObjectRefsInObject(const std::string& objectName, OTNObject& object);
			bool ResolveObjectRefsInRow(const std::string& ownerObjectName, const std::vector<size_t>& refColumns, OTNRow& row);
			bool ResolveValueRecursive(const std::string& ownerObjectName, OTNValue& value);
			bool ResolveSingleObjectRef(const std::string& ownerObjectName, OTNValue& value);
			const OTNObject* GetOTNObject(const std::string& objName) const;
			std::vector<size_t> GetObjectIndieces(const OTNObject& object);

			bool AddIdentifier(const Token& token, const std::unordered_map<uint32_t, std::string>& map, std::vector<std::string>& list);

			const Token& Peek();
			const Token& Next();
			bool NextIf(TokenType type);
			bool Match(TokenType type);
			Token Expect(TokenType type);
			bool IsAtEnd();

			// gets the token for a type/object
			void GetTypeToken(Token& outToken);
			bool IsListToken(uint32_t& outListDepth);
			OTNValue TokenToOTNValue(const Token& token, const OTNTypeDesc& type);
			OTNValue TokenToListOTNValue(const Token& token, const OTNTypeDesc& type);
			OTNValue TokenToObjectOTNValue(const Token& token, const OTNTypeDesc& type);
			OTNValue TokenToPrimitiveOTNValue(const Token& token, OTNBaseType type);

			template<typename T>
			T ParseNumericToken(const Token& token, OTNReaderV1* reader) {
				if (token.type != TokenType::NUMBER && token.type != TokenType::MINUS) {
					reader->AddError(token, "Expected numeric literal");
					return T{};
				}

				std::string text = token.text;
				T value = T{};

				try {
					// Handle leading minus
					if (token.type == TokenType::MINUS) {
						// Expect next token to be a number
						Token next = Next();
						if (next.type != TokenType::NUMBER) {
							reader->AddError(next, "Expected number after minus");
							return T{};
						}
						text = "-" + next.text;
					}

					if constexpr (std::is_same_v<T, int>) {
						value = std::stoi(text);
					}
					else if constexpr (std::is_same_v<T, float>) {
						value = std::stof(text);
					}
					else if constexpr (std::is_same_v<T, double>) {
						value = std::stod(text);
					}
					else {
						reader->AddError(token, "Unsupported numeric type");
						return T{};
					}

					return T(value);
				}
				catch (const std::exception& e) {
					reader->AddError(token, std::string("Invalid numeric literal: ") + e.what());
					return T{};
				}
			}

			TokenKeyword ResolveKeyword(const Token& token);
			std::string ToString(TokenType type);
			bool AddError(const Token& token, const std::string error);
			bool AddError(const std::string error);
		};

		#pragma endregion

		std::string m_error;
		bool m_valid = true;
		ReaderData m_readerData;

		bool OpenFileStream(const OTNFilePath& path);
		bool ReadData(const OTNFilePath& path, ReaderData& data);
		bool SetDataVersion(const std::vector<Token>& tokens, ReaderData& data);

		void AddError(const std::string& error, bool linebreak = true);
	};
	
	#pragma endregion
	
}