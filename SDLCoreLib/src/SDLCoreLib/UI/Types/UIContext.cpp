#include "Application.h"
#include "UI/UINode.h"
#include "UI/Types/UIContext.h"

namespace SDLCore::UI {

	UIContext* UIContext::CreateContext() {
		return new UIContext();
	}

    uint16_t UIContext::GetCurrentStackPosition() const {
        return m_stackPosition;
    }

    void UIContext::IncreaseStackCounter() {
        m_stackPosition++;
    }

    void UIContext::ResetStackCounter() {
        m_stackPosition = 0;
    }

    bool UIContext::IsNewKey(uintptr_t newId) {
        if (m_stackPosition < m_nodeIDs.size()) {
            uintptr_t id = m_nodeIDs[m_stackPosition];
            return id != newId;
        }
        Log::Error("SDLCore::UI::UIContext::IsNewKey: stack was outOfBounds");
        return false;
    }


    FrameNode* UIContext::BeginFrame(uintptr_t id) {
        if (!m_rootNode) {
            m_rootNode = std::make_shared<FrameNode>(id);
            FrameNode* frame = m_rootNode.get();
            m_nodeStack.push_back(reinterpret_cast<UINode*>(frame));
            return frame;
        }

        if (!m_nodeStack.empty()) {
            UINode* node = m_nodeStack.back();
            if (!node)
                return nullptr;

            auto sharedFrame = std::make_shared<FrameNode>(id);
            FrameNode* frame = sharedFrame.get();
            m_nodeStack.push_back(reinterpret_cast<UINode*>(frame));

            node->AddChild(sharedFrame);
            return frame;
        }
        else {
            UINode* node = (m_lastNode) ? m_lastNode : m_rootNode.get();
            if (node->ContainsChildAtPos(m_lastChildPosition, id)) {
                // element with id exists at position
            }
            else {
                // element does not exist
            }

        }
    }

    void UIContext::EndFrame() {
        if(!m_nodeStack.empty())
            m_nodeStack.pop_back();
    }

    void UIContext::SetWindowParams(WindowID id) {
        m_windowID = id;

        auto* app = Application::GetInstance();
        if (!app) {
            Log::Error("SDLCore::UI::SetWindowParams: Failed to resolve Window '{}': application instance not available", id);
            return;
        }

        Window* win = app->GetWindow(id);
        if (!win) {
            Log::Error("SDLCore::UI::SetWindowParams: Failed to initialize window parameters for '{}': window not found", id);
            return;
        }

        m_windowContentScale = win->GetContentScale();
        m_windowSize = win->GetSize();
    }

}