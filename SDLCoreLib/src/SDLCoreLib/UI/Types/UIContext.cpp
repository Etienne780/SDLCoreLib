#include <functional>

#include "Application.h"
#include "UI/UINode.h"
#include "UI/Nodes/FrameNode.h"
#include "UI/Types/UIContext.h"

namespace SDLCore::UI {

	UIContext* UIContext::CreateContext() {
		return new UIContext();
	}

    WindowID UIContext::GetWindowID() const {
        return m_windowID;
    }

    float UIContext::GetWindowScale() const {
        return m_windowContentScale;
    }

    Vector2 UIContext::GetWindowSize() const {
        return m_windowSize;
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
                m_rootNode->SetNodeActive();
                return m_rootNode.get();
            }
        }
        
        if (!m_rootNode) {
            m_rootNode = std::make_shared<FrameNode>(-1, id);
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

            int currentChildPos = (!m_lastChildPosition.empty()) ? 
                static_cast<int>(m_lastChildPosition.back()) : 0;

            FrameNode* frame = node->AddChild<FrameNode>(currentChildPos, id);
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
                currentNode->SetNodeActive();
                return reinterpret_cast<FrameNode*>(currentNode);
            }
            else {
                // remove pos and every entry after
                node->RemoveChildrenFromIndex(pos);

                // element does not exist. create element and create stack
                FrameNode* frame = node->AddChild<FrameNode>(static_cast<int>(pos), id);
                m_lastNodeStack.push_back(reinterpret_cast<UINode*>(frame));
                m_nodeStack.push_back(reinterpret_cast<UINode*>(frame));
                m_lastChildPosition.push_back(0);
                return frame;
            }
        }

        return nullptr;
    }

#include "SDLCoreTime.h"
    UIEvent UIContext::EndFrame() {
        if (!m_lastChildPosition.empty() && !m_lastNodeStack.empty()) {
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

        if (!m_lastNodeStack.empty()) {
            UIEvent* uiEvent = ProcessEvent(m_lastNodeStack.back());
            m_lastNodeStack.pop_back();
            // last node poped
            if (m_lastNodeStack.empty()) {
                RenderNodes(m_rootNode.get());
            }

            return *uiEvent;
        }

#ifndef NDEBUG
        Log::Warn("SDLCore::UI::UIContext::EndFrame: EndFrame called without a fitting BeginFrame!");
#endif 
        return UIEvent{};
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

    UIEvent* UIContext::ProcessEvent(UINode* node) {
        // skip inactive nodes
        if (!node || !node->IsActive()) {
            static UIEvent dummy;
            return &dummy;
        }

        // skips this element if a child has a event
        const auto& children = node->GetChildren();
        for (const auto& child : children) {
            if (!child)
                continue;

            if (child->GetChildHasEvent()) {
                node->SetChildHasEvent(true);
                return node->GetEventPtr();
            }

            if (UIEvent* childEvent = child->GetEventPtr()) {
                if (childEvent->IsHover()) {
                    node->SetChildHasEvent(true);
                    return node->GetEventPtr();
                }
            }
        }
        
        node->SetChildHasEvent(false);
        UIEvent* event = node->GetEventPtr();
        
        return event;
    }

    void UIContext::RenderNodes(UINode* rootNode) {

        std::function<void(UINode*)> RenderRecursive;
        RenderRecursive = [&](UINode* root) {
            if (!root || !root->IsActive()) 
                return;

            root->RenderNode()

            for (const std::shared_ptr<UINode>& child : root->GetChildren()) {
                RenderRecursive(child.get());
            }
        };

        RenderRecursive(rootNode);
    }


}