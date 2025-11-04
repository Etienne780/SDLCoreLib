#include <unordered_set>
#include <queue>
#include <CoreLib/Log.h>

#include "SDLCoreTypes.h"
#include "IDManager.h"

namespace SDLCore {

	IDManager::IDManager(IDOrder order) {
		SetIDOrder(order);
	}

	bool IDManager::IsIDFallback() {
		return m_idFallback;
	}

	unsigned int IDManager::GetNewUniqueIdentifier() const {
		switch (m_order) {
		case IDOrder::RANDOME: return GetNewUniqueIdentifierRandom();
		case IDOrder::ASCENDING: return GetNewUniqueIdentifierAscending();
		case IDOrder::DESCENDING: return GetNewUniqueIdentifierDescending();
		}

		return SDLCORE_INVALID_ID;
	}

	void IDManager::FreeUniqueIdentifier(unsigned int id) {
		m_freeIDs.push(id);
	}

	unsigned int IDManager::GetNewUniqueIdentifierFallback() const {
		if (!m_idFallback) {
			m_idFallback = true;
			Log::Warn("IDManager: Max ID limit reached, using fallback IDs from free pool");
		}

		if (!m_freeIDs.empty()) {
			auto id = m_freeIDs.front();
			m_freeIDs.pop();
			return id;
		}

		Log::Warn("IDManager: Cant find any free IDs, free pool is empty!");
		return SDLCORE_INVALID_ID;
	}

	void IDManager::Reset(unsigned int startValue) {
		std::queue<unsigned int> empty;
		std::swap(m_freeIDs, empty);

		switch (m_order) {
		case IDOrder::RANDOME:
			m_idCounter = startValue;
			m_idFallback = true;
			break;
		case IDOrder::ASCENDING:
			m_idCounter = startValue;
			m_idFallback = false;
			break;
		case IDOrder::DESCENDING:
			m_idCounter = startValue;
			m_idFallback = false;
			break;
		}
	}

	void IDManager::SetIDOrder(IDOrder value) {
		m_order = value;
		Reset((value == IDOrder::DESCENDING) ? SDLCORE_INVALID_ID : 0);
	}

	unsigned int IDManager::GetNewUniqueIdentifierRandom() const {
		unsigned int id;
		
		if (!m_freeIDs.empty()) {
			id = m_freeIDs.front();
			m_freeIDs.pop();
		}
		else {
			if (m_idCounter != SDLCORE_INVALID_ID) {
				id = m_idCounter++;
			}
			else {
				id = SDLCORE_INVALID_ID;
			}
		}

		return id;
	}

	unsigned int IDManager::GetNewUniqueIdentifierAscending() const {
		unsigned int id;

		if (m_idFallback) {
			id = GetNewUniqueIdentifierFallback();
		}
		else {
			if (m_idCounter != SDLCORE_INVALID_ID) {
				id = m_idCounter++;
			}
			else {
				id = GetNewUniqueIdentifierFallback();
			}
		}

		return id;
	}

	unsigned int IDManager::GetNewUniqueIdentifierDescending() const {
		unsigned int id;

		if (m_idFallback) {
			id = GetNewUniqueIdentifierFallback();
		}
		else {
			if (m_idCounter != 0) {
				id = m_idCounter--;
			}
			else {
				id = GetNewUniqueIdentifierFallback();
			}
		}

		return id;
	}

}