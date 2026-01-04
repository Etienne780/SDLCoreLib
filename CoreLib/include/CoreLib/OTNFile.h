#pragma once
#include <vector>
#include <strstream>
#include <string>
#include <variant>
#include <memory>

#include <CoreLib/CoreMath.h>

/*
* 
* ToDo:
* - Create OTN object with X
*	- Create OTN array/vector support
*	- Create Manual type setting
*   - Create OTN objects definitions for specific data types (Vector2, ...) X
*	- Create Manual type setting for custome types
* 
* - Create OTN Writer
*  - NEEDS to set correct version of the writer
* 
* - Create OTN Reader
*	- NEEDS to check version that was used to write the file
* 
*/


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

class OTNArray {
public:
	std::vector<OTNValueVariant> values;
};

template<typename>
inline constexpr bool otn_always_false_v = false;

template<typename T>
static constexpr bool is_otn_variant_v =
std::is_same_v<std::decay_t<T>, OTNValueVariant>;

template<typename T>
inline constexpr bool is_otn_object_v =
std::is_same_v<std::decay_t<T>, OTNObject>;

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
public:
	explicit OTNObject(const std::string& name);
	~OTNObject() = default;

	template<typename ...Args>
	OTNObject& SetNames(Args&&... names) {
		static_assert((std::is_convertible_v<Args, std::string> && ...),
			"All names must be convertible to std::string");

#ifndef NDEBUG
		if (!m_rows.empty()) {
			AppendError(
				"SetNames must be called before AddData in object '" + m_name + "'"
			);
			return *this;
		}
#endif
		constexpr size_t ArgCount = sizeof...(Args);
		m_names.clear();
		m_names.reserve(ArgCount);
		(m_names.emplace_back(std::forward<Args>(names)), ...);
		return *this;
	}

	template<typename... Args>
	OTNObject& AddDataRow(Args&&... args) {
		constexpr size_t ArgCount = sizeof...(Args);

#ifndef NDEBUG
		if (m_names.empty()) {
			AppendError(
				"AddDataRow called before SetNames in object '" + m_name + "'"
			);
			return *this;
		}

		if (ArgCount != m_names.size()) {
			AppendError(
				"AddDataRow argument count (" + std::to_string(ArgCount) +
				") does not match name count (" +
				std::to_string(m_names.size()) +
				") in object '" + m_name + "'"
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
				// Already a valid OTN value -> just store it
				row.emplace_back(std::forward<decltype(value)>(value));
			}
			else {
				OTNDataType<T> data(std::forward<decltype(value)>(value));

				if (!data.m_valid) {
					rowValid = false;
					AppendError(
						"Invalid data in object '" + m_name + "':\n" + data.m_error
					);
				}

				row.emplace_back(std::move(data.m_value));
			}
		};

		(addValue(std::forward<Args>(args)), ...);

		if (rowValid) {
			m_rows.emplace_back(std::move(row));
		}

		return *this;
	}

	bool IsValid() const;
	bool TryGetError(std::string& outMsg);

private:
	friend class OTNObjectBuilder;
	using OTNRow = std::vector<OTNValueVariant>;

	std::string m_name;
	std::string m_error;
	bool m_valid = true;

	std::vector<std::string> m_names;
	std::vector<OTNRow> m_rows;

	void AppendError(const std::string& error);
	void SetNamesFromBuilder(std::vector<std::string>&& names);
	void AddRowInternal(OTNRow&& row);
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
				m_objectName + "'"
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

template<typename T>
class OTNDataType {
public:
	explicit OTNDataType(T value) {
		using DT = std::decay_t<T>;

		if constexpr (is_otn_base_type_v<DT>) {
			m_value = value;
		}
		else if constexpr (is_otn_list_v<DT>) {
			auto otnArray = std::make_shared<OTNArray>();
			auto& array = otnArray->values;
			array.reserve(value.size());

			for (auto& elem : value) {
				if constexpr (is_otn_object_v<std::decay_t<decltype(elem)>>) {
					array.emplace_back(std::make_shared<OTNObject>(elem));
				}
				else {
					OTNDataType elemData(elem);
					if (!elemData.m_valid) {
						SetInvalid(elemData.m_error);
						return;
					}
					array.emplace_back(std::move(elemData.m_value));
				}
			}

			m_value = otnArray;
		}
		else if constexpr (std::is_same_v<DT, const char*>) {
			// Convert C-string to std::string
			m_value = std::string(value);
		}
		else if constexpr (is_otn_object_v<DT>) {
			// Convert Object to shard ptr of object
			m_value = std::make_shared<OTNObject>(std::move(value));
		}
		else {
			OTNObjectBuilder builder;
			ToOTNDataType<T>(builder, value);

			if (!builder.IsValid()) {
				SetInvalid(builder.GetError());
				return;
			}

			m_value = std::make_shared<OTNObject>(
				std::move(builder).ToOTNObject()
			);
		}
	}

	~OTNDataType() = default;

	OTNValueVariant m_value;
	bool m_valid = true;
	std::string m_error;
private:
	void SetInvalid(const std::string& error) {
		m_valid = false;
		m_error += error + "!\n";
	}
};

#pragma endregion


#pragma region OTNWriter

class OTNWriter {
public:
	explicit OTNWriter() = default;
	~OTNWriter() = default;

private:

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