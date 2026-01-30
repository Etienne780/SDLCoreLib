#pragma once
#include <vector>
#include <string>

#include <CoreLib/Math/Vector2.h>
#include <CoreLib/Math/Vector4.h>
#include <CoreLib/FormatUtils.h>

#include "UI/UIStyle.h"
#include "UI/Types/UIEvent.h"

namespace SDLCore::UI {

    namespace Internal {
        
        static std::unordered_map<std::string, uint32_t> nameToID;

    }

    class UIContext;
    class UICTXWrapper;
    class FrameNode;

    class UINode {
        friend class UICTXWrapper;
        friend class UIContext;
    public:
        UINode(int childPos, uintptr_t id, const std::string& typeName);
        virtual ~UINode();
        virtual void RenderNode(UIContext* ctx) const = 0;

        template<typename T, typename... Args>
        T* AddChild(Args&&... args) {
            static_assert(std::is_base_of<UINode, T>::value, "T must derive from UINode");

            auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
            T* child = ptr.get();
            child->m_parent = reinterpret_cast<FrameNode*>(this);
            m_children.push_back(std::move(ptr));
            return child;
        }

        void ClearStyles();

        void ReserveStyles(size_t count);

        // copys style
        void AddStyle(const UIStyle& style);

        void UpdateFinalStyle(UIContext* ctx);

        // sets override style changed to true if value is different
        template<typename... Args>
        UINode& SetOverride(UIPropertyID id, Args&&... args) {
            auto values = std::forward_as_tuple(std::forward<Args>(args)...);

            if (std::apply([&](auto&&... v) {
                return m_overrideState.IsDifferent(id, v...);
                }, values))
            {
                if (std::apply([&](auto&&... v) {
                    return m_overrideState.SetValue(id, std::forward<decltype(v)>(v)...);
                    }, values))
                {
                    m_lastOverrideID = id;
                    m_styleDirty = true;
                }
            }
            return *this;
        }

        UINode& SetImportant(bool value);
        UINode& ClearOverride(UIPropertyID id);

        /*
        * @brief Resets the state of the node to NORMAL
        */
        void ResetState();

        /*
        * @brief checks if a child at a given position has the id, if true outNode is this child
        */
        bool ContainsChildAtPos(uint16_t position, uintptr_t id, UINode*& outNode);

        uintptr_t GetID() const;
        const std::string& GeTypeName() const;
        uint32_t GetTypeID() const;
        UIEvent GetEvent() const;
        UIEvent* GetEventPtr();
        std::string GetName() const;
        UINode* GetParent();
        const std::vector<std::shared_ptr<UINode>>& GetChildren() const;
        UIState GetState() const;
        UIState GetLastState() const;
        bool GetChildHasEvent() const;
        // last style combination
        uint64_t GetAppliedStyleHash() const;
        // max lastModified of applied styles
        uint64_t GetAppliedStyleNode() const;

        bool GetStyleChanged() const;

        /*
        * @brief node will be active after the first frame of creation
        */
        bool IsActive() const;

        bool HasPointerEvents() const;
        bool HasHitTestTransparent() const;
        bool IsStatePropagationEnabled() const;
        bool IsDisabled() const;

        bool IsOverflowHidden() const;
        bool IsHorizontalOverflowHidden() const;
        bool IsVerticalOverflowHidden() const;

        bool IsFlow() const;
        bool IsRelative() const;
        bool IsAbsolute() const;

        /*
        * @brief used internaly to find out what elements have events
        *
        * should not be set manuly
        */
        void SetChildHasEvent(bool value);

        Vector2 GetPosition() const;
        // applys pos + borderWidth if outer border
        Vector2 GetVisiblePosition() const;
        Vector2 GetSize() const;
        // applys size + borderWidth if outer border
        Vector2 GetVisibleSize() const;
        Vector4 GetPadding() const;
        Vector4 GetMargin() const;

        UILayoutDirection GetLayoutDirection() const;
        UIAlignment GetHorizontalAlignment() const;
        UIAlignment GetVerticalAlignment() const;
        UIPositionType GetPositionType() const;

        Vector4 GetBorderLayoutPadding() const;
        Vector4 GetBorderLayoutMargin() const;
        // will always have a valid clipping rect
        Vector4 GetClippingRect() const;

        UIState GetResolvedState() const;

    protected:
        void RemoveChildrenFromIndex(uint16_t position);

        /**
        * @brief Merge all appliedStyles into the final UIStyle object.
        */
        UIStyle CreateStyle();

        void SetNodeStyleDirty();
        void SetNodeActive();
        void SetState(UIState state);
        void SetResolvedState(UIState state);

        bool IsMouseInNode() const;
        bool IsMouseInClipRect(const Vector2& point) const;
        bool IsPointInNode(const Vector2& point) const;
        bool IsPointInClipRect(const Vector2& point) const;

        virtual void ApplyStyleCalled(UIContext* context, const UIStyleState& styleState) = 0;
        virtual Vector2 CalculateSize(UIContext* context, UISizeUnit unitW, UISizeUnit unitH, float w, float h);
        virtual void ProcessEvent(UIEvent* event) {};

        uintptr_t m_id = 0;
        int m_childPos = -1;/*< position inside of the children */
        std::string m_typeName = "-";
        uint32_t m_typeID = SDLCORE_INVALID_ID;
        FrameNode* m_parent = nullptr;

        std::vector<std::shared_ptr<UINode>> m_children;
        std::vector<UIStyle> m_appliedStyles;
        UIStyle m_finalStyle;
        UIEvent m_eventState;
        float m_borderWidth = 0.0f;
        float m_transitionDuration = 0.0f;/*< is in milliseconds */
        UIEasing m_transitionEasing = UIEasing::Linear;
        bool m_innerBorder = false;
        bool m_childHasEvent = false;
        bool m_isActive = false;
        bool m_hasPointerEvents = true;
        bool m_hasHitTestTransparent = false;
        bool m_propagateStateToChildren = false;
        bool m_isDisabled = false;
        bool m_borderAffectsLayout = true;
        bool m_isHorizontalOverflowHidden = true;
        bool m_isVerticalOverflowHidden = true;

        Vector2 m_position{ 0.0f, 0.0f };
        Vector2 m_size{ 0.0f, 0.0f };
        Vector4 m_padding{ 0.0f, 0.0f, 0.0f, 0.0f };/*< Top, Left, Bottom, Right */
        Vector4 m_margin{ 0.0f, 0.0f, 0.0f, 0.0f };/*< Top, Left, Bottom, Right */

        UILayoutDirection m_layoutDir = UILayoutDirection::ROW;
        UIAlignment m_horizontalAligment = UIAlignment::START;
        UIAlignment m_verticalAligment = UIAlignment::START;
        UIPositionType m_positionType = UIPositionType::FLOW;
        Vector4 m_absolutePositionOffset{ 0.0f, 0.0f, 0.0f, 0.0f }; /*< Top, Left, Bottom, Right */
        std::array<UISizeUnit, 4> m_absolutePositionUnits{ /*< Top, Left, Bottom, Right */
            UISizeUnit::PX,
            UISizeUnit::PX,
            UISizeUnit::PX,
            UISizeUnit::PX
        };
    private:
        UIPropertyID m_lastOverrideID;
        UIStyleState m_overrideState;
        UIStyleState m_renderedStyleState;
        UIStyleState m_transitionFrom;
        UIStyleState m_transitionTo;

        UIState m_state = UIState::NORMAL;
        UIState m_resolvedState = UIState::NORMAL;
        UIState m_lastState = UIState::NORMAL;
        uint64_t m_appliedStyleHash = 0;    // last style combination
        uint64_t m_appliedStyleNode = 0;    // max lastModified of applied styles
        float m_currentTransitionEnd = 0.0f;
        float m_currentTransition = 0.0f;
        bool m_transitionActive = false;
        bool m_styleDirty = false;
        Vector4 m_clippingMask;

        static inline uint32_t m_typeIDCounter = 0;
        static uint32_t GetUITypeID(const std::string& name);

        /*
        * @brief Create and applys the style from all of the added styles
        */
        void ApplyStyle(UIContext* context);
        UIStyleState ComputeTargetStyleState();
        void OnStyleStateChanged();
        void Update(UIContext* ctx, float dt);
        void UpdateStyle(UIContext* ctx, float dt);

        template<typename T>
        bool GetResolvedValue(UIPropertyID id, T& out, const T& fallback) const {
            if (m_overrideState.TryGetValueIfSet<T>(id, out))
                return true;

            if (m_renderedStyleState.TryGetValue<T>(id, out, fallback))
                return true;

            out = fallback;
            return false;
        }

        // return true if last state is diff to current state
        bool HasStateChanged() const;

        void SetAppliedStyleHash(uint64_t newHash);
        void SetAppliedStyleNode(uint64_t node);
        void SetTransitionTime(float time, UITimeUnit unit);
        // is set in UI context begin call
        void SetClippingRect(const Vector4& clipRect);

        // is size + margin
        float GetAccumulatedChildSize(bool horizontal, int upToIndex) const;

        // is size + margin
        float GetTotalChildrenSize(bool horizontal) const;
        float AlignOffset(bool isHor, UIAlignment align, float freeSpace);
        float ResolveAbsoluteValue(const UINode* reference, 
            bool horizontal,UISizeUnit unit, float value);
        static bool IsRow(UILayoutDirection d);
        static bool IsReverse(UILayoutDirection d);
        void CalculateLayout(const UIContext* uiContext);
        void CalculateLayoutAbsolute(const UIContext* uiContext);
        void CalculateLayoutFlow(const UIContext* uiContext);

        void ProcessEventInternal(UIContext* ctx, UIEvent* event);
    };

}