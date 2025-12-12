#pragma once
#include "UI/Types/UITypes.h"

namespace SDLCore::UI {

    class UINodeTypeRegistry {
    public:
        UINodeTypeRegistry() = delete;
        static uint32_t GetRegisterTypeCount() {
            return counter;
        }

        static UINodeType RegisterType() {
            return UINodeType(counter++);
        }

    private:
        static inline uint32_t counter = 2;
    };

}