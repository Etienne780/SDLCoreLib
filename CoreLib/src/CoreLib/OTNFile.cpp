#include <algorithm>
#include <functional>
#include <unordered_map>
#include "OTNFile.h"

namespace OTN {

	std::string OTNValueTypeToString(OTNValueType type) {
		switch (type)
		{
		case OTN::OTNValueType::INT:		return "int";
		case OTN::OTNValueType::FLOAT:		return "float";
		case OTN::OTNValueType::DOUBLE:		return "double";
		case OTN::OTNValueType::BOOL:		return "bool";
		case OTN::OTNValueType::STRING:		return "string";
		case OTN::OTNValueType::OBJECT:		return "object";
		case OTN::OTNValueType::LIST:		return "list";
		case OTN::OTNValueType::UNKNOWN:	
		default:							return "UNKNOWN";
		}
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
		m_valid(other.m_valid) {}

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

	void OTNObject::AppendError(const std::string& error) {
		m_valid = false;
		m_error += error + "!\n";
	}

	void OTNObject::SetNamesFromBuilder(std::vector<std::string>&& names) {
		if (!m_rows.empty()) {
			AppendError(
				"SetNames must be called before AddData in object '" + m_name + "'"
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
			AppendError(msg);
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
			while (current->m_type == OTNValueType::LIST) {
				result += "List of ";
				auto& arr = std::get<OTNArrayPtr>(current->value);
				if (arr->values.empty()) break;
				current = &arr->values[0];
			}
			result += OTNValueTypeToString(current->m_type);
			return result;
		};

		std::function<bool(const OTNValue&, const OTNValue&, size_t, const std::string&)> validateRecursive;
		validateRecursive = [&](const OTNValue& ref, const OTNValue& val, size_t rowIdx, const std::string& path) -> bool {
			if (ref.m_type != val.m_type) {
				AppendError(
					"Type mismatch at '" + path + "' (row " + std::to_string(rowIdx) +
					"): expected '" + getNestedTypeString(&ref) +
					"', but found '" + getNestedTypeString(&val) + "'"
				);
				return false;
			}

			if (ref.m_type == OTNValueType::OBJECT) {
				auto& refObj = std::get<OTNObjectPtr>(ref.value);
				auto& valObj = std::get<OTNObjectPtr>(val.value);
				if (refObj->GetName() != valObj->GetName()) {
					AppendError(
						"Object name mismatch at '" + path + "' (row " + std::to_string(rowIdx) +
						"): expected '" + refObj->GetName() + "', but found '" + valObj->GetName() + "'"
					);
					return false;
				}
			}

			if (ref.m_type == OTNValueType::LIST) {
				auto& refArr = std::get<OTNArrayPtr>(ref.value);
				auto& valArr = std::get<OTNArrayPtr>(val.value);

				if (refArr->values.size() != valArr->values.size()) {
					AppendError(
						"List size mismatch at '" + path + "' (row " + std::to_string(rowIdx) +
						"): expected " + std::to_string(refArr->values.size()) +
						", found " + std::to_string(valArr->values.size())
					);
					return false;
				}

				for (size_t i = 0; i < refArr->values.size(); i++) {
					std::string newPath = path + "[" + std::to_string(i) + "]";
					if (!validateRecursive(refArr->values[i], valArr->values[i], rowIdx, newPath))
						return false;
				}
			}

			return true;
			};

		const OTNValue& refValue = m_rows[0][columnIndex];

		for (size_t i = 0; i < rowCount; i++) {
			auto& row = m_rows[i];

			if (columnIndex >= row.size()) {
				AppendError(
					"Row " + std::to_string(i) + " has fewer columns than expected (" +
					std::to_string(row.size()) + " instead of " + std::to_string(rowLength) +
					"). Type validation for column '" + columnName + "' skipped"
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
			obj.AppendError(m_error);
			return obj;
		}

		if (IsDataOutOfSync()) {
			obj.AppendError(
				"Object '" + m_objectName +
				"' has mismatched data and names (" +
				std::to_string(m_data.size()) + " != " +
				std::to_string(m_dataNames.size()) + ")"
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
		m_error += error + "!\n";
	}

	bool OTNObjectBuilder::AddName(std::string name) {
	#ifndef NDEBUG
		if (!IsNameUnique(name)) {
			SetInvalid("Name '" + name + "' is not unique in object '" + m_objectName + "'");
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
		m_objects.push_back(object);
		return *this;
	}

	bool OTNWriter::Save(const OTNFilePath& path) {
		OTNFilePath newPath;
		if (!ValidateFilePath(path, newPath)) {
			AddError("File path was invalid");
			return false;
		}

		if (!DebugValidateObjects()) {
			return false;
		}

		if (!WriteToFile(newPath)) {
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

	bool OTNWriter::TryGetError(std::string& outError) {
		if (m_error.empty())
			return false;
		outError = m_error;
		return true;
	}

	std::string OTNWriter::GetError() {
		std::string err = m_error;
		return err;
	}

	bool OTNWriter::ValidateFilePath(const OTNFilePath& path, OTNFilePath& out) {
		namespace fs = std::filesystem;

		fs::path finalPath = path;

		// check file name
		if (finalPath.filename().empty()) {
			AddError("file path has no file name");
			return false;
		}

		// check if parent file path exists
		fs::path parentDir = finalPath.parent_path();
		if (!parentDir.empty() && !fs::exists(parentDir)) {
			AddError("file path dose not exist");
			return false;
		}

		// file extension check
		if (finalPath.has_extension()) {
			std::string ext = finalPath.extension().string();
			std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

			if (ext != FILE_EXTENSION) {
				AddError("file extension '" + ext + "' is invalid, valid extensions are .OTN, .otn");
				return false;
			}
		}
		else {
			// add extension
			finalPath += ".";
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
					AddError("Objects invalid");
				}

				std::string msg = obj.GetError();
				// Remove trailing newline if present
				if (!msg.empty() && msg.back() == '\n') {
					msg.pop_back();
				}

				std::string objName = obj.GetName();
				AddError("Object '" + objName + "' is invalid, Error: '" + msg + "'");
				m_error += "\n";
				valid = false;
			}
		}

		return valid;
#endif
	}

	bool OTNWriter::WriteToFile(const OTNFilePath& path) {
		m_writerData.Reset();

		auto& stream = m_writerData.stream;
		stream.open(path, std::ios::binary);

		if (!stream.is_open()) {
			return false;
		}

		if (!CreateObject(m_writerData)) {
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

	void OTNWriter::CountValueType(const OTNValue& value, std::unordered_map<OTNValueType, uint32_t>& typeUsage) {
		switch (value.m_type) {
		case OTNValueType::LIST: {
			OTNArrayPtr array = std::get<OTNArrayPtr>(value.value);
			if (!array || array->values.empty()) {
				// default to INT if empty
				typeUsage[OTNValueType::INT]++;
			}
			else {
				CountArrayType(*array, typeUsage);
			}
			break;
		}
		case OTNValueType::OBJECT: {
			OTNObjectPtr obj = std::get<OTNObjectPtr>(value.value);
			typeUsage[OTNValueType::OBJECT]++;
			break;
		}
		default:
			typeUsage[value.m_type]++;
			break;
		}
	}

	void OTNWriter::CountArrayType(const OTNArray& array, std::unordered_map<OTNValueType, uint32_t>& typeUsage) {
		for (const auto& val : array.values) {
			CountValueType(val, typeUsage);
			break; // Only count type of first element for type usage purposes
		}
	}

	void OTNWriter::CountObjectType(const OTNObject& obj, std::unordered_map<OTNValueType, uint32_t>& typeUsage) {
		for (const auto& row : obj.GetDataRows()) {
			for (const auto& val : row) {
				CountValueType(val, typeUsage);
			}
			break;
		}
	}

	bool OTNWriter::CreateObject(WriterData& data) {
		if (data.created)
			data.Reset();

		for (const auto& obj : m_objects) {
			CountObjectType(obj, data.typeUsage);
		}

		auto& objectList = data.objects;
		objectList.reserve(m_objects.size());

		for (auto& obj : m_objects) {
			objectList.emplace_back(std::move(obj));
		}

		data.created = true;
		m_objects.clear();
		return true;
	}

	bool OTNWriter::WriteHeader() {
		auto& stream = m_writerData.stream;

		stream << "Vers:";
		AddSpace(stream);
		stream << std::to_string(OTN::VERSION) + GetLineCharEnd();
		AddLineBreak(stream);

		if (m_useDefName) {
		
		}

		if (m_useDefType) {
		
		}

		AddLineBreak(stream);
		return true;
	}
	
	bool OTNWriter::WriteBody() {
		auto& stream = m_writerData.stream;

		return true;
	}

	char OTNWriter::GetLineCharEnd() {
		return ';';
	}

	void OTNWriter::AddSpace(std::ofstream& stream) {
		if (!m_useOptimizations)
			stream << ' ';
	}

	void OTNWriter::AddLineBreak(std::ofstream& m_stream) {
		if (!m_useOptimizations)
			m_stream << '\n';
	}

	void OTNWriter::AddError(const std::string& error, bool linebreak) {
		m_error += error;
		if (linebreak)
			m_error += "!\n";
	}

	#pragma endregion

	#pragma region OTNReader



	#pragma endregion

}