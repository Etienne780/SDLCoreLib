#include <algorithm>
#include <unordered_map>
#include "OTNFile.h"

namespace OTN {

	#pragma region OTNObject


	// ======== OTNObject ========
	OTNObject::OTNObject(const std::string& name)
		: m_name(name) {
	}

	bool OTNObject::IsValid() const {
		return m_valid;
	}

	bool OTNObject::TryGetError(std::string& outError) {
		if (m_error.empty())
			return false;
		outError = std::move(m_error);
		m_error.clear();
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
		outError = std::move(m_error);
		m_error.clear();
		return true;
	}

	std::string OTNWriter::GetError() {
		std::string err = m_error;
		m_error.clear();
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

	void OTNWriter::AddError(const std::string& error, bool linebreak) {
		m_error += error;
		if (linebreak)
			m_error += "!\n";
	}

	#pragma endregion

	#pragma region OTNReader



	#pragma endregion

}