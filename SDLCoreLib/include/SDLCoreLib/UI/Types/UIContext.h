#pragma once
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
		uint16_t GetCurrentStackPosition() const;

	private:
		UIContext() = default;

		void IncreaseStackCounter();
		void ResetStackCounter();

		bool IsNewKey(uintptr_t id);

		FrameNode* BeginFrame(uintptr_t id);
		void EndFrame();

		void SetWindowParams(WindowID id);

		// uint16_t m_stackPosition = 0;
		// std::vector<UINode*> m_nodeStack;/*< is created dynamicly*/
		// std::vector<uintptr_t> m_nodeIDs;/*< ids of each UI element liniear*/

		uint16_t m_lastChildPosition = 0;/*< is the position of the current child inside of last node*/
		UINode* m_lastNode = nullptr;
		std::vector<UINode*> m_nodeStack;
		std::shared_ptr<FrameNode> m_rootNode = nullptr;
		WindowID m_windowID;
		float m_windowContentScale = 1.0f;
		Vector2 m_windowSize{ 0.0f, 0.0f };
	};

}