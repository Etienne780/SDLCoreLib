#include "OTNFile.h"

#pragma region OTNObject


// ======== OTNObject ========
OTNObject::OTNObject(const std::string& name)
	: m_name(name) {
}

bool OTNObject::IsValid() const {
	return m_valid;
}

bool OTNObject::TryGetError(std::string& outMsg) {
	if (m_error.empty())
		return false;
	outMsg = std::move(m_error);
	m_error.clear();
	return true;
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



#pragma endregion

#pragma region OTNReader



#pragma endregion