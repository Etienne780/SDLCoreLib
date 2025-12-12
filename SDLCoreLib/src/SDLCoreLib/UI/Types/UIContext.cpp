#include "Application.h"
#include "UI/UINode.h"
#include "UI/Types/UIContext.h"

namespace SDLCore::UI {

	UIContext* UIContext::CreateContext() {
		return new UIContext();
	}

    FrameNode* UIContext::BeginFrame(uintptr_t id) {
        if (m_lastNodeStack.empty() && m_rootNode) {
            if (m_rootNode->GetID() != id) {
                // Creats new root node
                m_rootNode = nullptr;
            }
            else {
                // pushes root node on to stack
                m_lastNodeStack.push_back(reinterpret_cast<UINode*>(m_rootNode.get()));
                m_lastChildPosition.push_back(0);
                return m_rootNode.get();
            }
        }
        
        if (!m_rootNode) {
            m_rootNode = std::make_shared<FrameNode>(id);
            FrameNode* frame = m_rootNode.get();
            m_nodeStack.push_back(reinterpret_cast<UINode*>(frame));
            m_lastNodeStack.push_back(reinterpret_cast<UINode*>(frame));
            m_lastChildPosition.push_back(0);
            return frame;
        }

        if (!m_nodeStack.empty()) {
            UINode* node = m_nodeStack.back();
            if (!node)
                return nullptr;

            FrameNode* frame = node->AddChild<FrameNode>(id);
            m_nodeStack.push_back(reinterpret_cast<UINode*>(frame));
            m_lastNodeStack.push_back(reinterpret_cast<UINode*>(frame));
            m_lastChildPosition.push_back(0);

            return frame;
        }
        else {
            UINode* node = m_lastNodeStack.back();
            uint16_t pos = (m_lastChildPosition.empty()) ? 0 : m_lastChildPosition.back();
            UINode* currentNode = nullptr;
            if (node->ContainsChildAtPos(pos, id, currentNode)) {
                // element with id exists at position. set it as last position
                m_lastNodeStack.push_back(currentNode);
                m_lastChildPosition.push_back(0);
                return reinterpret_cast<FrameNode*>(currentNode);
            }
            else {
                // remove pos and every entry after
                node->RemoveChildrenFromIndex(pos);

                // element does not exist. create element and create stack
                FrameNode* frame = node->AddChild<FrameNode>(id);
                m_lastNodeStack.push_back(reinterpret_cast<UINode*>(frame));
                m_nodeStack.push_back(reinterpret_cast<UINode*>(frame));
                m_lastChildPosition.push_back(0);
                return frame;
            }
        }

        return nullptr;
    }

    void UIContext::EndFrame() {
        if (!m_lastChildPosition.empty()) {
            UINode* node = m_lastNodeStack.back();
            if (node && m_lastChildPosition.back() < node->GetChildren().size()) {
                node->RemoveChildrenFromIndex(m_lastChildPosition.back());
            }
            
            m_lastChildPosition.pop_back();
            if (!m_lastChildPosition.empty()) {
                m_lastChildPosition.back()++;
            }
        }

        if (!m_nodeStack.empty())
            m_nodeStack.pop_back();

        if (!m_lastNodeStack.empty())
            m_lastNodeStack.pop_back();
    }

    UINode* UIContext::GetRootNode() const {
        return m_rootNode.get();
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