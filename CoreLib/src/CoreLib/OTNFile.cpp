#include <algorithm>
#include <functional>
#include <unordered_map>
#include "OTNFile.h"

namespace OTN {

	inline void HashCombine(size_t& seed, size_t value) {
		seed ^= value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
	}

	constexpr std::string_view OTNValueTypeToString(OTNValueType type) noexcept {
		switch (type)
		{
		case OTN::OTNValueType::INT:     return "int";
		case OTN::OTNValueType::FLOAT:   return "float";
		case OTN::OTNValueType::DOUBLE:  return "double";
		case OTN::OTNValueType::BOOL:    return "bool";
		case OTN::OTNValueType::STRING:  return "string";
		case OTN::OTNValueType::OBJECT:  return "object";
		case OTN::OTNValueType::LIST:    return "list";
		case OTN::OTNValueType::UNKNOWN:
		default:                         return "UNKNOWN";
		}
	}

	constexpr uint32_t OTNValueTypeCharLength(OTNValueType type) noexcept {
		return static_cast<uint32_t>(OTNValueTypeToString(type).size());
	}

#pragma region OTNObject


	// ======== OTNObject ========
	OTNObject::OTNObject(const std::string& name)
		: m_name(name) {
	}

	OTNObject::OTNObject(const OTNObject& other)
		: m_name(other.m_name),
		m_names(other.m_names),
		m_rows(other.m_rows),
		m_error(other.m_error),
		m_valid(other.m_valid) {
	}

	OTNObject& OTNObject::operator=(const OTNObject& other) {
		m_name = other.m_name;
		m_names = other.m_names;
		m_rows = other.m_rows;
		m_error = other.m_error;
		m_valid = other.m_valid;
		return *this;
	}

	bool OTNObject::IsValid() const {
		return m_valid;
	}

	bool OTNObject::TryGetError(std::string& outError) {
		if (m_error.empty())
			return false;
		outError = m_error;
		return true;
	}

	std::string OTNObject::GetError() const {
		return m_error;
	}

	std::string OTNObject::GetName() const {
		return m_name;
	}

	const std::vector<std::string>& OTNObject::GetColumnNames() const {
		return m_names;
	}

	const std::vector<OTNObject::OTNRow>& OTNObject::GetDataRows() const {
		return m_rows;
	}

	void OTNObject::SetInvalid(const std::string& error) {
		m_valid = false;
		if (!m_error.empty())
			m_error += "\n";
		m_error += error;
	}

	void OTNObject::SetNamesFromBuilder(std::vector<std::string>&& names) {
		if (!m_rows.empty()) {
			SetInvalid(
				"SetNames must be called before AddData in object '" + m_name + "'!"
			);
			return;
		}

		m_names = std::move(names);
	}

	void OTNObject::AddRowInternal(OTNRow&& row) {
		m_rows.emplace_back(std::move(row));
	}

	bool OTNObject::DebugValidateNamesDistinct() {
#ifdef NDEBUG
		return true;
#else
		bool allUnique = true;
		std::unordered_map<std::string, int> nameCounts;

		for (const auto& name : m_names) {
			++nameCounts[name];
		}

		std::vector<std::string> duplicates;
		for (const auto& [name, count] : nameCounts) {
			if (count > 1) {
				duplicates.push_back(name);
				allUnique = false;
			}
		}

		if (!duplicates.empty()) {
			std::string msg = "Duplicate names found: ";
			for (size_t i = 0; i < duplicates.size(); ++i) {
				msg += "'" + duplicates[i] + "'";
				if (i + 1 < duplicates.size()) {
					msg += ", ";
				}
			}
			SetInvalid(msg + "!");
		}

		return allUnique;
#endif
	}

	bool OTNObject::DebugValidateDataTypes(size_t columnIndex) {
#ifdef NDEBUG
		return true;
#else
		if (m_rows.empty() || columnIndex >= m_rows[0].size())
			return true;

		size_t rowCount = m_rows.size();
		size_t rowLength = m_rows[0].size();
		bool valid = true;
		std::string columnName = (m_names.size() > columnIndex) ? m_names[columnIndex] : "-";

		auto getNestedTypeString = [](const OTNValue* val) -> std::string {
			std::string result;
			const OTNValue* current = val;
			while (current->type == OTNValueType::LIST) {
				result += "List of ";
				auto& arr = std::get<OTNArrayPtr>(current->value);
				if (arr->values.empty()) break;
				current = &arr->values[0];
			}
			result += OTNValueTypeToString(current->type);
			return result;
			};

		std::function<bool(const OTNValue&, const OTNValue&, size_t, const std::string&)> validateRecursive;
		validateRecursive = [&](const OTNValue& ref, const OTNValue& val, size_t rowIdx, const std::string& path) -> bool {
			if (ref.type != val.type) {
				SetInvalid(
					"Type mismatch at '" + path + "' (row " + std::to_string(rowIdx) +
					"): expected '" + getNestedTypeString(&ref) +
					"', but found '" + getNestedTypeString(&val) + "'!"
				);
				return false;
			}

			if (ref.type == OTNValueType::OBJECT) {
				auto& refObj = std::get<OTNObjectPtr>(ref.value);
				auto& valObj = std::get<OTNObjectPtr>(val.value);
				if (refObj->GetName() != valObj->GetName()) {
					SetInvalid(
						"Object name mismatch at '" + path + "' (row " + std::to_string(rowIdx) +
						"): expected '" + refObj->GetName() + "', but found '" + valObj->GetName() + "'!"
					);
					return false;
				}
			}

			if (ref.type == OTNValueType::LIST) {
				auto& refArr = std::get<OTNArrayPtr>(ref.value);
				auto& valArr = std::get<OTNArrayPtr>(val.value);

				if (!refArr || !valArr)
					return true; // nothing to validate

				// Reference list must define at least one element to describe the type
				if (refArr->values.empty())
					return true; // dynamic / untyped list

				const OTNValue& refElement = refArr->values.front();

				for (size_t i = 0; i < valArr->values.size(); ++i) {
					std::string newPath = path + "[" + std::to_string(i) + "]";
					if (!validateRecursive(refElement, valArr->values[i], rowIdx, newPath))
						return false;
				}
			}

			return true;
		};

		const OTNValue& refValue = m_rows[0][columnIndex];

		for (size_t i = 0; i < rowCount; i++) {
			auto& row = m_rows[i];

			if (columnIndex >= row.size()) {
				SetInvalid(
					"Row " + std::to_string(i) + " has fewer columns than expected (" +
					std::to_string(row.size()) + " instead of " + std::to_string(rowLength) +
					"). Type validation for column '" + columnName + "' skipped!"
				);
				valid = false;
				continue;
			}

			// Hauptaufruf mit Spaltenname als Pfad
			if (!validateRecursive(refValue, row[columnIndex], i, columnName)) {
				valid = false;
			}
		}

		return valid;
#endif
	}


	// ======== OTNObjectBuilder ========
	void OTNObjectBuilder::SetObjectName(const std::string& name) {
		m_objectName = name;
	}

	bool OTNObjectBuilder::IsValid() const {
		return m_valid;
	}

	std::string OTNObjectBuilder::GetError() const {
		return m_error;
	}

	OTNObject OTNObjectBuilder::ToOTNObject()&& {
		OTNObject obj{ m_objectName };

		if (!m_valid) {
			obj.SetInvalid(m_error);
			return obj;
		}

		if (IsDataOutOfSync()) {
			obj.SetInvalid(
				"Object '" + m_objectName +
				"' has mismatched data and names ( size" +
				std::to_string(m_data.size()) + " != size " +
				std::to_string(m_dataNames.size()) + ")!"
			);
			return obj;
		}

		obj.SetNamesFromBuilder(std::move(m_dataNames));

		OTNObject::OTNRow row;
		row.reserve(m_data.size());

		for (auto& value : m_data) {
			row.emplace_back(std::move(value));
		}

		obj.AddRowInternal(std::move(row));

		return obj;
	}

	void OTNObjectBuilder::SetInvalid(const std::string& error) {
		m_valid = false;
		if (!m_error.empty())
			m_error += "\n";
		m_error += error;
	}

	bool OTNObjectBuilder::AddName(std::string name) {
#ifndef NDEBUG
		if (!IsNameUnique(name)) {
			SetInvalid("Name '" + name + "' is not unique in object '" + m_objectName + "'!");
			return false;
		}
#endif

		m_dataNames.push_back(std::move(name));
		return true;
	}

	bool OTNObjectBuilder::IsNameUnique(const std::string& name) const {
		return std::find(m_dataNames.begin(), m_dataNames.end(), name) == m_dataNames.end();
	}

	bool OTNObjectBuilder::IsDataOutOfSync() const {
		return m_data.size() != m_dataNames.size();
	}

#pragma endregion

#pragma region OTNWriter

	OTNWriter& OTNWriter::UseDefName(bool value) {
		m_useDefName = value;
		return *this;
	}

	OTNWriter& OTNWriter::UseDefType(bool value) {
		m_useDefType = value;
		return *this;
	}

	OTNWriter& OTNWriter::UseOptimizations(bool value) {
		m_useOptimizations = value;
		return *this;
	}

	OTNWriter& OTNWriter::AppendObject(const OTNObject& object) {
#ifndef NDEBUG
		for (const auto& obj : m_objects) {
			if (obj.GetName() == object.GetName()) {
				AddError("Could not append object '" + object.GetName() + "', an object with the name already exists!");
				return *this;
			}
		}
#endif 

		m_objects.push_back(object);
		return *this;
	}

	bool OTNWriter::Save(const OTNFilePath& path) {
		if (!IsValid()) {
			AddError("Writer object is invalid!");
			return false;
		}
		
		OTNFilePath newPath;
		if (!ValidateFilePath(path, newPath)) {
			AddError("File path was invalid!");
			return false;
		}

		if (!DebugValidateObjects()) {
			AddError("[Debug] Validation of objects failed!");
			return false;
		}

		if (!WriteToFile(newPath)) {
			AddError("Write to file failed!");
			return false;
		}

		if (!IsValid()) {
			AddError("Writer object is invalid!");
			return false;
		}

		return true;
	}

	bool OTNWriter::GetUseDefName() const {
		return m_useDefName;
	}

	bool OTNWriter::GetUseDefType() const {
		return m_useDefType;
	}

	bool OTNWriter::GetUseOptimizations() const {
		return m_useOptimizations;
	}

	bool OTNWriter::IsValid() const {
		return m_valid;
	}

	bool OTNWriter::TryGetError(std::string& outError) {
		if (m_valid)
			return false;
		outError = m_error;
		return true;
	}

	std::string OTNWriter::GetError() {
		return m_error;
	}

	size_t OTNWriter::SerializedObject::AddOrGetRow(const Row& row) {
		if (row.empty())
			return static_cast<size_t>(-1);
		
		size_t hash = CreateRowHash(columnTypes, row);
		size_t index = 0;

		auto it = rowIndexByHash.find(hash);
		if (it == rowIndexByHash.end()) {
			index = rows.size();
			rowIndexByHash[hash] = index;
			rows.push_back(row);
		}
		else {
			index = it->second;
		}

		return index;
	}

	size_t OTNWriter::SerializedObject::CreateRowHash(const std::vector<ColumnType>& columnTypes, const Row& row) {
		size_t hash = 0;
		
		if (columnTypes.size() != row.size()) {
			throw std::runtime_error("CreateRowHash: columnTypes and row size mismatch");
		}

		for (size_t i = 0; i < row.size(); i++) {
			HashCombine(hash, HashValue(columnTypes[i], row[i]));
		}

		return hash;
	}

	size_t OTNWriter::SerializedObject::HashValue(const ColumnType& colType, const OTNValue& value) {
		size_t hash = 0;

		// Include value type
		OTNValueType type = (value.type == OTNValueType::LIST) ? OTNValueType::LIST : colType.baseType;
		HashCombine(hash, static_cast<size_t>(type));
		switch (type) {
		case OTNValueType::INT:
			HashCombine(hash, std::hash<int>{}(std::get<int>(value.value)));
			break;

		case OTNValueType::FLOAT:
			HashCombine(hash, std::hash<float>{}(std::get<float>(value.value)));
			break;

		case OTNValueType::DOUBLE:
			HashCombine(hash, std::hash<double>{}(std::get<double>(value.value)));
			break;

		case OTNValueType::BOOL:
			HashCombine(hash, std::hash<bool>{}(std::get<bool>(value.value)));
			break;

		case OTNValueType::STRING:
			HashCombine(hash, std::hash<std::string>{}(std::get<std::string>(value.value)));
			break;

		case OTNValueType::LIST: {
			auto arrayPtr = std::get<OTNArrayPtr>(value.value);
			if (!arrayPtr) {
				HashCombine(hash, 0);
				break;
			}

			// Include list size to distinguish {1,2} from {1,2,3}
			HashCombine(hash, arrayPtr->values.size());
			HashCombine(hash, static_cast<size_t>(colType.listDepth));

			for (const auto& val : arrayPtr->values) {
				HashCombine(hash, HashValue(colType, val));
			}
			break;
		}

		case OTNValueType::OBJECT: {
			if (value.type != OTNValueType::INT) {
				std::runtime_error("HashValue: Object type was not 'int'");
			}
			HashCombine(hash, std::hash<int>{}(std::get<int>(value.value)));
			break;
		}
		case OTNValueType::UNKNOWN:
		default:
			std::runtime_error("HashValue: type for hashing was invalid");
			break;
		}

		return hash;
	}

	bool OTNWriter::ValidateFilePath(const OTNFilePath& path, OTNFilePath& out) {
		namespace fs = std::filesystem;

		fs::path finalPath = path;

		// check file name
		if (finalPath.filename().empty()) {
			AddError("file path has no file name!");
			return false;
		}

		// check if parent file path exists
		fs::path parentDir = finalPath.parent_path();
		if (!parentDir.empty() && !fs::exists(parentDir)) {
			AddError("file path dose not exist!");
			return false;
		}

		// file extension check
		if (finalPath.has_extension()) {
			std::string ext = finalPath.extension().string();
			std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

			if (ext != FILE_EXTENSION) {
				AddError("file extension '" + ext + "' is invalid, valid extensions are .OTN, .otn!");
				return false;
			}
		}
		else {
			// add extension
			finalPath += OTN::FILE_EXTENSION;
		}

		out = finalPath;
		return true;
	}

	bool OTNWriter::DebugValidateObjects() {
#ifdef NDEBUG
		return true;
#else
		bool valid = true;
		for (const auto& obj : m_objects) {
			if (!obj.IsValid()) {
				if (valid) {
					AddError("Objects invalid!");
				}

				std::string msg = obj.GetError();
				// Remove trailing newline if present
				if (!msg.empty() && msg.back() == '\n') {
					msg.pop_back();
				}

				std::string objName = obj.GetName();
				AddError("Object '" + objName + "' is invalid, Error: '" + msg + "'!");
				m_error += "\n";
				valid = false;
			}
		}

		return valid;
#endif
	}

	bool OTNWriter::WriteToFile(const OTNFilePath& path) {
		m_writerData.Reset();

		auto& stream = m_writerData.stream.stream;
		stream.open(path, std::ios::binary);

		if (!stream.is_open()) {
			return false;
		}

		if (!CreateWriteData(m_writerData)) {
			stream.close();
			return false;
		}

		if (!WriteHeader()) {
			stream.close();
			return false;
		}

		if (!WriteBody()) {
			stream.close();
			return false;
		}

		stream.close();
		return true;
	}

	void OTNWriter::CountObjectType(const SerializedObject& obj, std::unordered_map<OTNValueType, uint32_t>& typeUsage) {		
		for (const auto& types : obj.columnTypes) {
			if(types.refObject.empty())
				typeUsage[types.baseType]++;
		}
	}

	OTNWriter::ColumnType OTNWriter::DeduceColumnType(const OTNValue& value) {
		ColumnType result;
		const OTNValue* current = &value;

		while (current->type == OTNValueType::LIST) {
			++result.listDepth;

			const auto& arr = std::get<OTNArrayPtr>(current->value);
			if (!arr || arr->values.empty())
				break;

			current = &arr->values.front();
		}

		result.baseType = current->type;

		if (current->type == OTNValueType::OBJECT) {
			const auto& obj = std::get<OTNObjectPtr>(current->value);
			if (obj)
				result.refObject = obj->GetName();
		}

		return result;
	}

	bool OTNWriter::CreateWriteData(WriterData& data) {
		if (data.created)
			data.Reset();

		// Convert objects
		for (OTNObject& obj : m_objects) {
			AddObject(data, obj);
		}

		// Count used types (read-only)
		for (const auto& [name, obj] : data.objects) {
			CountObjectType(obj, data.typeUsage);
		}

		data.created = true;
		m_objects.clear();

		if (m_useDefName) {
			if (!CreateDefName())
				return false;
		}

		if (m_useDefType) {
			if (!CreateDefType())
				return false;
		}

		return true;
	}

	size_t OTNWriter::AddObject(WriterData& data, OTNObject& object) {
		auto& objectMap = data.objects;

		// Create or get SerializedObject
		auto [it, inserted] = objectMap.try_emplace(object.GetName());
		SerializedObject& serObj = it->second;

		if (inserted) {
			serObj.columnNames = object.GetColumnNames();
		}

		std::function<void(const ColumnType& colType, OTNValue&, const OTNValue&)> convertToSerValue;
		convertToSerValue = [&](const ColumnType& colType, OTNValue& outVal, const OTNValue& val) {
			if (val.type == OTNValueType::LIST) {
				// resolve objects
				if (!colType.refObject.empty()) {
					const OTNArrayPtr& arrayPtr = std::get<OTNArrayPtr>(val.value);
					OTNArrayPtr newArray = std::make_shared<OTNArray>();
					if (!arrayPtr || !newArray)
						return;

					newArray->values.reserve(arrayPtr->values.size());
					for (const auto& v : arrayPtr->values) {
						OTNValue newValue;
						convertToSerValue(colType, newValue, v);
						newArray->values.emplace_back(std::move(newValue));
					}
					outVal = OTNValue(newArray);
				}
				else {
					outVal = val;
				}
			}
			else if (val.type == OTNValueType::OBJECT) {
				const OTNObjectPtr& objPtr = std::get<OTNObjectPtr>(val.value);
				if (!objPtr)
					return;

				// Ensure referenced object exists
				size_t refIndex = AddObject(data, *objPtr);
				outVal = OTNValue(static_cast<int>(refIndex));
			}
			else {
				outVal = val;
			}
		};

		size_t lastIndex = 0;
		// Convert rows
		for (const OTNObject::OTNRow& row : object.GetDataRows()) {
			SerializedObject::Row serRow;
			serRow.reserve(row.size());

			if (serObj.columnTypes.empty()) {
				serObj.columnTypes.reserve(row.size());
				for (const OTNValue& val : row) {
					serObj.columnTypes.push_back(DeduceColumnType(val));
				}
			}

			if (row.size() != serObj.columnTypes.size()) {
				throw std::runtime_error("AddObject: SerializedObject::Row and SerializedObject::ColumnType size mismatch");
			}

			size_t columnTypeIndex = 0;
			for (const OTNValue& val : row) {
				OTNValue outVal;
				convertToSerValue(serObj.columnTypes[columnTypeIndex], outVal, val);
				serRow.emplace_back(std::move(outVal));
				columnTypeIndex++;
			}

			lastIndex = serObj.AddOrGetRow(serRow);
		}

		return lastIndex;
	}

	bool OTNWriter::CreateDefName() {
		auto& defNameMap = m_writerData.defName;
		defNameMap.clear();

		// Count name usage
		std::unordered_map<std::string, uint32_t> nameUsage;

		for (const auto& [_, serObj] : m_writerData.objects) {
			for (const std::string& colName : serObj.columnNames) {
				++nameUsage[colName];
			}
		}

		uint32_t indexCount = 0;

		// skip if there are not enough names
		if (nameUsage.size() < 2)
			return true;

		for (const auto& [name, used] : nameUsage) {
			// Heuristic: only replace if it actually saves space
			// Example rule: more than once and longer than 3 char
			if (used > 1 && name.size() > 3) {
				defNameMap.emplace(name, indexCount++);
			}
		}

		// clear if not enough names meet the conditions
		if (defNameMap.size() <= 1)
			defNameMap.clear();

		return true;
	}

	bool OTNWriter::CreateDefType() {
		auto& defTypeMap = m_writerData.defType;
		defTypeMap.clear();
		uint32_t indexCount = 0;
		
		for (const auto& [type, used] : m_writerData.typeUsage) {
			if (type == OTNValueType::OBJECT || type == OTNValueType::LIST)
				continue;

			uint32_t length = OTNValueTypeCharLength(type);

			if (used > length + 4)
				defTypeMap[std::string(OTNValueTypeToString(type))] = indexCount++;
		}

		return true;
	}

	bool OTNWriter::WriteHeader() {
		auto& stream = m_writerData.stream;

		stream << "@version:";
		AddSpace(stream);
		stream << std::to_string(OTN::VERSION) + GetLineCharEnd();
		AddLineBreak(stream);

		if (m_useDefName) {
			if (!WriteHeaderDefName())
				return false;
		}

		if (m_useDefType) {
			if(!WriteHeaderDefType())
				return false;
		}

		AddLineBreak(stream);
		return true;
	}

	bool OTNWriter::WriteHeaderDefName() {
		auto& defNameMap = m_writerData.defName;
		if (defNameMap.empty())
			return true;

		auto& stream = m_writerData.stream;
		stream << "@defName:";
		AddSpace(stream);

		if (!WirteHeaderDefHelper(stream, defNameMap))
			return false;

		stream << GetLineCharEnd();
		AddLineBreak(stream);

		return true;
	}

	bool OTNWriter::WriteHeaderDefType() {
		auto& defTypeMap = m_writerData.defType;
		if (defTypeMap.empty())
			return true;

		auto& stream = m_writerData.stream;
		stream << "@defType:";
		AddSpace(stream);

		if (!WirteHeaderDefHelper(stream, defTypeMap))
			return false;

		stream << GetLineCharEnd();
		AddLineBreak(stream);
		return true;
	}

	bool OTNWriter::WirteHeaderDefHelper(IndentedStream& stream, const std::unordered_map<std::string, uint32_t>& map) {
		bool first = true;
		for (const auto& [name, id] : map) {
			if (!first) {
				stream << GetSeparatorChar();
				AddSpace(stream);
			}

			stream << name;
			AddSpace(stream);
			stream << "=";
			AddSpace(stream);
			stream << std::to_string(id);

			first = false;
		}
		return true;
	}
	
	bool OTNWriter::WriteBody() {
		auto& stream = m_writerData.stream;

		stream << "@object";
		AddSpace(stream);
		stream << "{";
		AddLineBreak(stream);

		if(!m_useOptimizations)
			stream.IncreaseIndent();

		if (!WriteObject(stream, m_writerData.objects))
			return false;
		stream.DecreaseIndent();

		stream << "}";
		stream << GetLineCharEnd();

		return true;
	}

	bool OTNWriter::WriteObject(IndentedStream& stream,
		const std::unordered_map<std::string, SerializedObject>& objects) {
		
		std::function<void(const SerializedObject& obj)> writeNames;
		writeNames = [&](const SerializedObject& obj) {
			const auto& defNameMap = m_writerData.defName;
			const auto& defTypeMap = m_writerData.defType;

			bool firstName = true;
			for (size_t i = 0; i < obj.columnNames.size(); i++) {
				if (!firstName) {
					stream << ",";
					AddSpace(stream);
				}
				firstName = false;

				ColumnType colType = obj.columnTypes[i];

				if (colType.refObject.empty()) {
					if (defTypeMap.empty()) {
						stream << OTNValueTypeToString(colType.baseType);
					}
					else {
						std::string typeName{ OTNValueTypeToString(colType.baseType) };
						auto it = defTypeMap.find(typeName);
						if (it != defTypeMap.end())
							stream << it->second;
						else
							stream << typeName;
					}
				}
				else {
					stream << "Ref<";
					stream << colType.refObject;
					stream << ">";
				}

				// < 0 = no List, 1 = [], 2 = [][] ...
				for (size_t j = colType.listDepth; j > 0; j--) {
					stream << "[]";
				}

				stream << "/";


				if (defNameMap.empty()) {
					stream << obj.columnNames[i];
				}
				else {
					std::string name = obj.columnNames[i];
					auto it = defNameMap.find(name);
					if (it != defNameMap.end())
						stream << it->second;
					else
						stream << name;
				}
			}
		};

		bool firstObj = true;
		for (const auto& [name, obj] : objects) {
			if (!firstObj) {
				AddLineBreak(stream);
			}
			firstObj = false;
			stream << name;
			stream << "[";
			stream << obj.rows.size();
			stream << "]";
			AddSpace(stream);
			stream << "{";
			AddLineBreak(stream);

			AddIndent(stream);
			if (obj.columnNames.size() != obj.columnTypes.size()) {
				AddError("Could not body. Section @Object, size of names(" + 
					std::to_string(obj.columnNames.size()) 
					+ ") and types(" + 
					std::to_string(obj.columnTypes.size()) 
					+ ") dose not match in object '" + name + "'!");
				return false;
			}
			writeNames(obj);
			AddLineBreak(stream);
			stream << "}";
			AddLineBreak(stream);

			for (const auto& row : obj.rows) {
				bool first = true;
				for (const auto& serValue : row) {
					if (!first) {
						stream << ",";
						AddSpace(stream);
					}
					first = false;

					WriteData(stream, serValue);
				}
				stream << GetLineCharEnd();
				AddLineBreak(stream);
			}
		}

		return true;
	}

	template<typename T>
	void OTNWriter::WriteData(IndentedStream& stream, const T& data) {
		if constexpr (std::is_same_v<T, OTNValue>) {
			switch (data.type) {
			case OTNValueType::INT:
				WriteData(stream, std::get<int>(data.value));
				break;
			case OTNValueType::FLOAT:
				WriteData(stream, std::get<float>(data.value));
				break;
			case OTNValueType::DOUBLE:
				WriteData(stream, std::get<double>(data.value));
				break;
			case OTNValueType::BOOL:
				WriteData(stream, std::get<bool>(data.value));
				break;
			case OTNValueType::STRING:
				stream << '"';
				stream << std::get<std::string>(data.value);
				stream << '"';
				break;
			case OTNValueType::LIST: {
				auto array = std::get<OTNArrayPtr>(data.value);
				stream << "{";
				if (array) {
					bool first = true;
					for (const auto& val : array->values) {
						if (!first) {
							stream << ","; 
							AddSpace(stream);
						}
						first = false;
						WriteData(stream, val);
					}
				}
				stream << "}";
				break;
			}
			case OTNValueType::OBJECT:
			case OTNValueType::UNKNOWN:
			default:
				AddError("WriteData: unsupported OTNValueType");
				break;
			}
		}
		else if constexpr (std::is_same_v<T, int>) {
			stream << data;
		}
		else if constexpr (std::is_same_v<T, float>) {
			stream << data;
		}
		else if constexpr (std::is_same_v<T, double>) {
			stream << data;
		}
		else if constexpr (std::is_same_v<T, bool>) {
			stream << (data ? "true" : "false");
		}
		else {
			static_assert(otn_always_false_v<T>, "Unsupported type for WriteData");
		}
	}

	constexpr char OTNWriter::GetLineCharEnd() noexcept {
		return ';';
	}

	constexpr char OTNWriter::GetSeparatorChar() noexcept {
		return ',';
	}

	void OTNWriter::AddSpace(IndentedStream& stream) {
		if (!m_useOptimizations)
			stream << ' ';
	}

	void OTNWriter::AddIndent(IndentedStream& stream, uint32_t level) {
		if (m_useOptimizations)
			return;

		for (; level > 0; --level)
			stream << '\t';
	}

	void OTNWriter::AddLineBreak(IndentedStream& m_stream) {
		if (!m_useOptimizations)
			m_stream << '\n';
		m_stream.NewLine();
	}

	void OTNWriter::AddError(const std::string& error, bool linebreak) {
		if (!m_error.empty())
			m_error += "\n";		
		m_error += error;
		m_valid = false;
	}

	#pragma endregion

	#pragma region OTNReader



	#pragma endregion

}