#pragma once

#include "pe/DbgGui/IComponent.h"

namespace pe {
    namespace gui {

        class Graphics : public IComponent {
        public:
            Graphics();

            void update() override;
            void draw() override;
        };

    } // namespace gui
} // namespace pe
