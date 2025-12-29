#include "SDLCoreTime.h"
#include "UI/UIStyle.h"
#include "UI/Types/UIPropertyRegistry.h"

namespace SDLCore::UI {

	UIStyle::UIStyle()
		: m_id(m_idManager.GetNewUniqueIdentifier()) {
		UIPropertyRegistry::RegisterBaseProperties();
	}

	UIStyle::UIStyle(const std::string& name)
		: m_name(name), m_id(m_idManager.GetNewUniqueIdentifier()) {
		UIPropertyRegistry::RegisterBaseProperties();
	}

	UIStyle::UIStyle(std::string&& name)
		: m_name(name), m_id(m_idManager.GetNewUniqueIdentifier()) {
		UIPropertyRegistry::RegisterBaseProperties();
	}

	UIStyle::~UIStyle() {
		m_idManager.FreeUniqueIdentifier(m_id.value);
	}

	UIStyle::UIStyle(const UIStyle& other)
		: m_id(m_idManager.GetNewUniqueIdentifier()),
		m_name(other.m_name),
		m_currentState(other.m_currentState),
		m_uiStates(other.m_uiStates),
		m_lastModified(other.m_lastModified) {
	}

	UIStyle& UIStyle::operator=(const UIStyle& other) {
		if (this == &other)
			return *this;

		if (!m_id.IsInvalid())
			m_idManager.FreeUniqueIdentifier(m_id.value);

		m_id = UIStyleID(m_idManager.GetNewUniqueIdentifier());
		m_name = other.m_name;
		m_currentState = other.m_currentState;
		m_uiStates = other.m_uiStates;
		m_lastModified = other.m_lastModified;

		return *this;
	}

	UIStyle::UIStyle(UIStyle&& other) noexcept {
		MoveFrom(std::move(other));
	}

	UIStyle& UIStyle::operator=(UIStyle&& other) noexcept {
		if (this != &other)
			MoveFrom(std::move(other));
		return *this;
	}

	std::string UIStyle::ToString() const {
		return m_name;
	}

	UIStyle& UIStyle::Merge(const UIStyle& other) {
		for (const auto& [state, styleState] : other.GetAllStates()) {
			UIStyleState* outStyleState = this->GetState(state);
			outStyleState->Merge(styleState);
		}
		return *this;
	}

	UIStyleID UIStyle::GetID() const {
		return m_id;
	}

	std::string UIStyle::GetName() const {
		return m_name;
	}

	uint64_t UIStyle::GetLastModified() const {
		return m_lastModified;
	}

	UIStyleState UIStyle::GetStyleState(UIState state) {
		UIStyleState* styleState = GetState(state);
		return *styleState;
	}

	const UIStyleState& UIStyle::GetStyleState(UIState state) const {
		UIStyleState* styleState = GetState(state);
		return *styleState;
	}

	const std::unordered_map<UIState, UIStyleState>& UIStyle::GetAllStates() const {
		return m_uiStates;
	}

	UIStyle& UIStyle::SetActiveState(UIState state) {
		m_currentState = state;
		return *this;
	}

	UIStyle& UIStyle::SetValue(UIPropertyID attID, PropertyValue value, bool important) {
		UIStyleState* state = GetState(m_currentState);
		if (state->SetValue(attID, value, important))
			UpdateLastModified();// if value could be set update last modifed
		return *this;
	}

	UIStyleState* UIStyle::GetState(UIState state) const {
		auto it = m_uiStates.find(state);
		if (it == m_uiStates.end()) {
			m_uiStates[state] = UIStyleState();
			return &m_uiStates[state];
		}
		return &it->second;
	}

	void UIStyle::UpdateLastModified() {
		m_lastModified = Time::GetFrameCount();
	}

	void UIStyle::MoveFrom(UIStyle&& other) noexcept {
		if (this == &other)
			return;

		if (m_id.value != SDLCORE_INVALID_ID)
			m_idManager.FreeUniqueIdentifier(m_id.value);

		m_id = other.m_id;
		m_name = std::move(other.m_name);
		m_currentState = other.m_currentState;
		m_uiStates = std::move(other.m_uiStates);
		m_lastModified = other.m_lastModified;

		other.m_id.SetInvalid();
		other.m_currentState = UIState::NORMAL;
		other.m_uiStates.clear();
		other.m_lastModified = 0;
	}

}