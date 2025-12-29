#pragma once
#include <string>

#include "UI/Types/UIPropertyRegistry.h"
#include "UI/Types/UITypes.h"
#include "UI/Types/UIStyleState.h"
#include "IDManager.h"

namespace SDLCore::UI {

	class UIStyle {
	public:
		UIStyle();
		UIStyle(const std::string& name);
		UIStyle(std::string&& name);
		~UIStyle();

		UIStyle(const UIStyle&);
		UIStyle& operator=(const UIStyle&);

		UIStyle(UIStyle&&) noexcept;
		UIStyle& operator=(UIStyle&&) noexcept;
		
		/*
		* not finished
		*/
		std::string ToString() const;

		/*
		* @brief Merges other style on top of this
		*/
		UIStyle& Merge(const UIStyle& other);

		UIStyleID GetID() const;
		std::string GetName() const;
		// number that represents on which frame the style was last modified
		uint64_t GetLastModified() const;
		UIStyleState GetStyleState(UIState state);
		const UIStyleState& GetStyleState(UIState state) const;
		const std::unordered_map<UIState, UIStyleState>& GetAllStates() const;

		/*
		* @brief Sets the state that will be used when setting properties
		*/
		UIStyle& SetActiveState(UIState state);

		// all properties registered by the UIPropertyRegistry should be in the SDLCore::UI::Properties namespace
		UIStyle& SetValue(UIPropertyID attID, PropertyValue value, bool important = false);

	private:
		static inline IDManager m_idManager;
		UIStyleID m_id{ SDLCORE_INVALID_ID };
		std::string m_name = "UNKOWN";
		UIState m_currentState = UIState::NORMAL;
		mutable std::unordered_map<UIState, UIStyleState> m_uiStates;
		uint64_t m_lastModified = 0;

		/*
		* @brief Gets the state or creates a new one
		* @param state state to get
		* @return allways returns a valid ptr
		*/
		UIStyleState* GetState(UIState state) const;

		// updates the last modified to current frame count
		void UpdateLastModified();

		void MoveFrom(UIStyle&& other) noexcept;
	};

}