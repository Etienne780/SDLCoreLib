#include "Internal/FontManager.h"
#include "Types/Font/FontResource.h"
#include "Application.h"

namespace SDLCore {

	FontResource::FontResource(TTF_Font* font) {
		if(!IsApplicationQuit())
			m_id = FontManager::GetInstance().RegisterFont(font);
	}

	FontResource::FontResource(const FontResource& other) 
		: m_id(other.m_id) {
		if(!IsApplicationQuit())
			FontManager::GetInstance().IncreaseRef(m_id);
	}

	FontResource::FontResource(FontResource&& other) noexcept
		: m_id(other.m_id) {
		other.m_id.SetInvalid();
	}

	FontResource::~FontResource() {
		if (!m_id.IsInvalid() && !IsApplicationQuit())
			FontManager::GetInstance().DecreaseRef(m_id);
	}

	FontResource& FontResource::operator=(const FontResource& other) {
		if (this == &other)
			return *this;

		if (!m_id.IsInvalid() && !IsApplicationQuit())
			FontManager::GetInstance().DecreaseRef(m_id);

		m_id = other.m_id;

		if (!m_id.IsInvalid() && !IsApplicationQuit())
			FontManager::GetInstance().IncreaseRef(m_id);

		return *this;
	}

	FontResource& FontResource::operator=(FontResource&& other) noexcept {
		if (this == &other)
			return *this;

		if (!m_id.IsInvalid() && !IsApplicationQuit())
			FontManager::GetInstance().DecreaseRef(m_id);

		m_id = other.m_id;
		other.m_id.SetInvalid();

		return *this;
	}

	bool FontResource::IsInvalid() {
		return (m_id.IsInvalid() || GetFont() == nullptr);
	}

	FontID FontResource::GetID() const {
		return m_id;
	}

	TTF_Font* FontResource::GetFont() const {
		if (IsApplicationQuit())
			return nullptr;
		return FontManager::GetInstance().GetFont(m_id);
	}

}