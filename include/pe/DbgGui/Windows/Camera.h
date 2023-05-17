#pragma once

#include "pe/DbgGui/IComponent.h"

namespace pe {
    namespace gui {

        class Camera : public IComponent {
        public:
            Camera();

            void update() override;
            void draw() override;
        };

    } // namespace gui
} // namespace pe
