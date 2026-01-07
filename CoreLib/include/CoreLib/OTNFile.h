#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <variant>
#include <memory>
#include <filesystem>
#include <cstdint>
#include <string_view>
#include <unordered_map>

#include <CoreLib/CoreMath.h>

namespace std::filesystem {
	class path;
}

namespace OTN {

	// Version of the OTN format
	inline constexpr uint8_t VERSION = 1;

	// File extension for OTN files
	inline constexpr std::string_view FILE_EXTENSION = ".otn";

	/*
	* 
	* ToDo:
	* - Create OTN object with X
	*	- Create OTN array/vector support X
	*	- Create Manual type  ---------- na
	*   - Create OTN objects definitions for specific data types (Vector2, ...) X
	*	- Create Manual type setting for custome types -------------- na
	*	- Validate if name is allowed
	*	- @index and @indexOffset to know where in the file specific data is located
	* 
	* - Create OTN Writer
	*  - NEEDS to set correct version of the writer
	* 
	* - Create OTN Reader
	*	- NEEDS to check version that was used to write the file
	* 
	*/

	enum class OTNValueType {
		UNKNOWN = 0,
		INT,
		FLOAT,
		DOUBLE,
		BOOL,
		STRING,
		OBJECT,
		LIST,
	};

	constexpr std::string_view OTNValueTypeToString(OTNValueType type) noexcept;
	constexpr uint32_t OTNValueTypeCharLength(OTNValueType type) noexcept;
	
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
		OTNArrayPtr
	>;
	
	template<typename T>
	struct OTNTypeOf {
		static constexpr OTNValueType value = OTNValueType::UNKNOWN;
	};

	template<> struct OTNTypeOf<int> {
		static constexpr OTNValueType value = OTNValueType::INT;
	};

	template<> struct OTNTypeOf<float> {
		static constexpr OTNValueType value = OTNValueType::FLOAT;
	};

	template<> struct OTNTypeOf<double> {
		static constexpr OTNValueType value = OTNValueType::DOUBLE;
	};

	template<> struct OTNTypeOf<bool> {
		static constexpr OTNValueType value = OTNValueType::BOOL;
	};

	template<> struct OTNTypeOf<std::string> {
		static constexpr OTNValueType value = OTNValueType::STRING;
	};

	template<> struct OTNTypeOf<OTNObjectPtr> {
		static constexpr OTNValueType value = OTNValueType::OBJECT;
	};

	template<> struct OTNTypeOf<OTNArrayPtr> {
		static constexpr OTNValueType value = OTNValueType::LIST;
	};

	template<typename T>
	inline constexpr OTNValueType GetType() {
		return OTNTypeOf<std::decay_t<T>>::value;
	}

	inline OTNValueType GetTypeFromVariant(const OTNValueVariant& variant) {
		return std::visit(
			[](const auto& value) constexpr -> OTNValueType {
				using T = std::decay_t<decltype(value)>;
				return GetType<T>();
			},
			variant
		);
	}

	struct OTNValue {
		OTNValueVariant value;
		OTNValueType type = OTNValueType::UNKNOWN;

		OTNValue() = default;

		explicit OTNValue(OTNValueVariant v)
			: value(std::move(v)),
			type(GetTypeFromVariant(value)) {
		}
	};

	class OTNArray {
	public:
		std::vector<OTNValue> values;
	};
	
	template<typename>
	inline constexpr bool otn_always_false_v = false;
	
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

	class OTNObject {
		friend class OTNObjectBuilder;
	public:
		using OTNRow = std::vector<OTNValue>;
		explicit OTNObject(const std::string& name);

		OTNObject::OTNObject(const OTNObject& other);
		OTNObject& OTNObject::operator=(const OTNObject& other);

		~OTNObject() = default;
	
		template<typename ...Args>
		OTNObject& SetNames(Args&&... names) {
			static_assert((std::is_convertible_v<Args, std::string> && ...),
				"All names must be convertible to std::string");
	
	#ifndef NDEBUG
			if (!m_rows.empty()) {
				SetInvalid(
					"SetNames must be called before AddData in object '" + m_name + "'!"
				);
				return *this;
			}
	#endif
			constexpr size_t ArgCount = sizeof...(Args);
			m_names.clear();
			m_names.reserve(ArgCount);
			(m_names.emplace_back(std::forward<Args>(names)), ...);

			DebugValidateNamesDistinct();
			return *this;
		}
	
		template<typename... Args>
		OTNObject& AddDataRow(Args&&... args) {
			constexpr size_t ArgCount = sizeof...(Args);
	
	#ifndef NDEBUG
			if (m_names.empty()) {
				SetInvalid(
					"AddDataRow called before SetNames in object '" + m_name + "'!"
				);
				return *this;
			}
	
			if (ArgCount != m_names.size()) {
				SetInvalid(
					"AddDataRow argument count (" + std::to_string(ArgCount) +
					") does not match name count (" +
					std::to_string(m_names.size()) +
					") in object '" + m_name + "'!"
				);
				return *this;
			}
	#endif
	
			OTNRow row;
			row.reserve(ArgCount);
	
			bool rowValid = true;
	
			auto addValue = [&](auto&& value) {
				using T = std::decay_t<decltype(value)>;

				if constexpr (is_otn_variant_v<T>) {
					row.emplace_back(std::forward<decltype(value)>(value));
				}
				else {
					OTNDataType<T> data(std::forward<decltype(value)>(value));

					if (!data.m_valid) {
						rowValid = false;
						SetInvalid(
							"Invalid data in object '" + m_name + "':\n" + data.m_error
						);
						return;
					}

					row.emplace_back(std::move(data.m_value));
				}
			};
	
			(addValue(std::forward<Args>(args)), ...);
	
			if (rowValid) {
				m_rows.emplace_back(std::move(row));
			}

#ifndef NDEBUG
			if (!m_rows.empty()) {
				for (size_t i = 0; i < m_rows[0].size(); i++) {
					DebugValidateDataTypes(i);
				}
			}
#endif

			return *this;
		}
	
		bool IsValid() const;
		bool TryGetError(std::string& outError);
		std::string GetError() const;
		std::string GetName() const;
		const std::vector<std::string>& GetColumnNames() const;
		const std::vector<OTNRow>& GetDataRows() const;
	
	private:
		std::string m_name;
		std::string m_error;
		bool m_valid = true;
	
		std::vector<std::string> m_names;
		std::vector<OTNRow> m_rows;
	
		void SetInvalid(const std::string& error);
		void SetNamesFromBuilder(std::vector<std::string>&& names);
		void AddRowInternal(OTNRow&& row);

		// validates if each column name is distinct
		bool DebugValidateNamesDistinct();
		// validates if each data type of each row matchs the type of the first row at a specifc column
		bool DebugValidateDataTypes(size_t columnIndex);
	};
	
	class OTNObjectBuilder {
	public:
		OTNObjectBuilder() = default;
		~OTNObjectBuilder() = default;
	
		void SetObjectName(const std::string& name);
	
		template<typename... Args>
		OTNObjectBuilder& AddNames(Args&&... names) {
			static_assert((std::is_convertible_v<Args, std::string> && ...),
				"All names must be convertible to std::string");
	
			(AddName(std::forward<Args>(names)) && ...);
			return *this;
		}
	
		template<typename... Args>
		OTNObjectBuilder& AddData(Args&&... args) {			
			(AddSingleData(std::forward<Args>(args)) && ...);
			return *this;
		}
	
		template<typename T>
		OTNObjectBuilder& AddNameData(const std::string& name, T&& data) {		
			if (IsDataOutOfSync()) {
				SetInvalid(
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
	
		bool IsValid() const;
		std::string GetError() const;
	
		OTNObject ToOTNObject()&&;
	
	private:
		bool m_valid = true;
		std::string m_error;
		std::string m_objectName;
	
		std::vector<std::string> m_dataNames;
		std::vector<OTNValueVariant> m_data;
	
		void SetInvalid(const std::string& error);
		bool AddName(std::string name);
		
		template<typename T>
		bool AddSingleData(T&& data) {
			if constexpr (is_otn_base_type_v<std::decay_t<T>>) {
				m_data.emplace_back(std::forward<T>(data));
			}
			else {
				OTNObjectBuilder builder;
				ToOTNDataType<std::decay_t<T>>(builder, data);
	
				if (!builder.IsValid()) {
					SetInvalid(builder.GetError());
					return false;
				}

				m_data.emplace_back(
					std::make_shared<OTNObject>(
						std::move(builder).ToOTNObject()
					)
				);
			}
			return true;
		}
	
		bool IsNameUnique(const std::string& name) const;
		bool IsDataOutOfSync() const;
	};
	
	template<typename T>
	void ToOTNDataType(OTNObjectBuilder&, const T&) {
		static_assert(otn_always_false_v<T>, "Unsupported type for ToOTNDataType");
	}

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
				auto otnArray = std::make_shared<OTNArray>();
				otnArray->values.reserve(value.size());
				for (auto&& elem : value) {
					OTNDataType<std::decay_t<decltype(elem)>> elemData(elem);
					if (!elemData.m_valid) {
						SetInvalid(elemData.m_error);
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
				ToOTNDataType<DT>(builder, value);
				if (!builder.IsValid()) {
					SetInvalid(builder.GetError());
					return;
				}
				OTNObjectPtr ptr = std::make_shared<OTNObject>(std::move(builder).ToOTNObject());

				if (!ptr->IsValid())
					SetInvalid(ptr->GetError());

				m_value = ptr;
			}
		}

		void SetInvalid(const std::string& error) {
			m_valid = false;
			if (!m_error.empty())
				m_error += "\n";
			m_error += error;
		}

	public:
		OTNValueVariant m_value;
		bool m_valid = true;
		std::string m_error;
	};

	#pragma endregion
	
	#pragma region OTNWriter

	class OTNWriter {
	public:
		using OTNFilePath = std::filesystem::path;
	
		explicit OTNWriter() = default;
		~OTNWriter() = default;
	
		OTNWriter& UseDefName(bool value);
		OTNWriter& UseDefType(bool value);
		OTNWriter& UseOptimizations(bool value);
	
		OTNWriter& AppendObject(const OTNObject& object);
	
		// @param path is absolute
		bool Save(const OTNFilePath& path);
	
		bool GetUseDefName() const;
		bool GetUseDefType() const;
		bool GetUseOptimizations() const;
		
		bool IsValid() const;
		bool TryGetError(std::string& outError);
		std::string GetError();
	
	private:
		struct ColumnType {
			OTNValueType baseType = OTNValueType::UNKNOWN; // < int, object, string, ...
			std::string refObject;                         // < only OBJECT / REF
			uint32_t listDepth = 0;                        // < 0 = no List, 1 = [], 2 = [][] ...
		};

		struct SerializedObject {
			struct SerializedValue {
				OTNValue value;
				std::string refObject;
			};

			using Row = std::vector<SerializedValue>;

			std::vector<std::string> columnNames;
			std::vector<ColumnType> columnTypes;

			// uses map to add only unique rows and vector for stable indices
			std::vector<Row> rows;
			std::unordered_map<std::size_t, std::size_t> rowIndexByHash;

			// Adds row if not already present, returns index
			size_t AddOrGetRow(const Row& row);
		private:
			static size_t CreateRowHash(const Row& row);
			static size_t HashValue(const SerializedValue& serValue);
			static size_t HashValue(const OTNValue& value);
		};

		class IndentedStream {
		public:
			std::ofstream stream;
			uint32_t indentLevel = 0;
			std::string indentStr = "\t";
			bool newLine = true; // true, wenn wir am Zeilenanfang stehen

			IndentedStream() = default;

			void NewLine() { newLine = true; }
			void IncreaseIndent() { ++indentLevel; }
			void DecreaseIndent() { if (indentLevel > 0) --indentLevel; }


			template<typename T>
			IndentedStream& operator<<(const T& value) {
				if (newLine) {
					for (uint32_t i = 0; i < indentLevel; ++i)
						stream << indentStr;
					newLine = false;
				}
				stream << value;
				return *this;
			}
		};

		struct WriterData {
			bool created = false;
			IndentedStream stream;

			std::unordered_map<OTNValueType, uint32_t> typeUsage;// < map contains which data types are used and how often
			std::unordered_map<std::string, SerializedObject> objects;

			std::unordered_map<std::string, uint32_t> defName;// < used for optimaziations: Replaceses comman used names of Objects like position with numbers
			std::unordered_map<std::string, uint32_t> defType;// < used for optimaziations: Replaceses comman used type names with numbers

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
	
		std::vector<OTNObject> m_objects;
		std::string m_error;
		bool m_valid = true;

		WriterData m_writerData;
		
		bool ValidateFilePath(const OTNFilePath& path, OTNFilePath& out);
		bool DebugValidateObjects();

		bool WriteToFile(const OTNFilePath& path);
		bool CreateWriteData(WriterData& data);
		size_t AddObject(WriterData& data, OTNObject& object);
		bool CreateDefName();
		bool CreateDefType();

		bool WriteHeader();
		bool WriteHeaderDefName();
		bool WriteHeaderDefType();
		bool WirteHeaderDefHelper(IndentedStream& stream, const std::unordered_map<std::string, uint32_t>& map);
		bool WriteBody();
		bool WriteObject(IndentedStream& stream, const std::unordered_map<std::string, SerializedObject>& objects);
		
		template<typename T>
		void WriteData(IndentedStream& stream, const T& data);

		static constexpr char GetLineCharEnd() noexcept;
		static constexpr char GetSeparatorChar() noexcept;
		void AddSpace(IndentedStream& stream);
		void AddIndent(IndentedStream& stream, uint32_t level = 1);
		void AddLineBreak(IndentedStream& stream);
		void AddError(const std::string& error, bool linebreak = true);

		void CountObjectType(const SerializedObject& obj, std::unordered_map<OTNValueType, uint32_t>& typeUsage);
		static ColumnType DeduceColumnType(const OTNValue& value);
	};
	
	#pragma endregion
	
	
	#pragma region OTNReader

class OTNReader {
public:
	explicit OTNReader() = default;
	~OTNReader() = default;

private:

};

#pragma endregion
	
}