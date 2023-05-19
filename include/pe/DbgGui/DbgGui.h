#pragma once

#include "container/seadPtrArray.h"
#include "heap/seadDisposer.h"
#include <sead/heap/seadDisposer.h>
#include <sead/heap/seadExpHeap.h>

namespace pe {
    namespace gui {

        class IComponent;

        sead::ExpHeap*& getDbgGuiHeap();

        class DbgGui {
            SEAD_SINGLETON_DISPOSER(DbgGui);

            IComponent* mComponents[4]{nullptr};

            struct {
                bool showDemoWindow = false;
                bool showCamera = false;
                bool showHeapViewer = false;
            } mSharedData;

        public:
            DbgGui();

            void update();
            void draw();

            friend class IComponent;
        };

    } // namespace gui
} // namespace pe
