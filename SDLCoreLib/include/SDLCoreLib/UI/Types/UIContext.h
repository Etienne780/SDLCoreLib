#pragma once
#include <deque>

#include <CoreLib/Math/Vector2.h>

#include "IDManager.h"
#include "Types/Types.h"
#include "UI/Types/UITypes.h"

namespace SDLCore::UI {

	class UICTXWrapper;
	class UINode;
	class FrameNode;
	class UIContext {
		friend class UICTXWrapper;
	public:
		static UIContext* CreateContext();

	private:
		UIContext() = default;

		FrameNode* BeginFrame(uintptr_t id);
		void EndFrame();
		
		template<typename T, typename ...Args>
		T* AddNode(uintptr_t id, Args&&... args) {
			if (!m_rootNode) {
				Log::Error("SDLCore::UI::UICTXWrapper::AddNode: Could not add not. no valid root element was found!");
				return nullptr;
			}

			UINode* parentNode = m_lastNodeStack.back();
			uint16_t pos = (m_lastChildPosition.empty()) ? 0 : m_lastChildPosition.back();
			UINode* currentNode = nullptr;

			m_lastChildPosition.back()++;
			if (parentNode->ContainsChildAtPos(pos, id, currentNode)) {
				// element with id exists at position. set it as last position
				// no stack increas cause add node has no end func
				return reinterpret_cast<T*>(currentNode);
			}
			
			// remove pos and every entry after
			parentNode->RemoveChildrenFromIndex(pos);

			// element does not exist. create element and create stack
			T* childNode = parentNode->AddChild<T>(id, std::forward<Args>(args)...);
			// no stack increas cause add node has no end func
			return childNode;
			
		}

		UINode* GetRootNode() const;
		void SetWindowParams(WindowID id);

		std::deque<uint16_t> m_lastChildPosition;/*< is the position of the current child inside of last node*/
		std::deque<UINode*> m_nodeStack;
		std::deque<UINode*> m_lastNodeStack;
		std::shared_ptr<FrameNode> m_rootNode = nullptr;
		WindowID m_windowID;
		float m_windowContentScale = 1.0f;
		Vector2 m_windowSize{ 0.0f, 0.0f };
	};

}