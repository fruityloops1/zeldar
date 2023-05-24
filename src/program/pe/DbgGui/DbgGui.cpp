#include "imgui.h"
#include "pe/DbgGui/DbgGui.h"
#include "pe/DbgGui/MenuBar.h"
#include "pe/DbgGui/Windows/Camera.h"
#include "pe/DbgGui/Windows/Graphics.h"
#include "pe/DbgGui/Windows/HeapViewer.h"
#include "pe/DbgGui/Windows/ImGuiDemo.h"
#include "program/imgui_nvn.h"
#include "util/modules.hpp"
#include <sead/heap/seadDisposer.h>

namespace pe {
    namespace gui {

        static sead::ExpHeap* heap = nullptr;

        sead::ExpHeap*& getDbgGuiHeap() { return heap; }

        SEAD_SINGLETON_DISPOSER_IMPL(DbgGui);

        DbgGui::DbgGui() {
            mComponents[0] = new MenuBar;
            mComponents[1] = new ImGuiDemo;
            mComponents[2] = new Camera;
            mComponents[3] = new Graphics;
            mComponents[4] = new HeapViewer;
        }

        void DbgGui::draw() {
            for (int i = 0; i < sizeof(mComponents) / sizeof(mComponents[0]); i++) {
                IComponent* c = mComponents[i];
                c->draw();
            }
        }

        void DbgGui::update() {
            for (int i = 0; i < sizeof(mComponents) / sizeof(mComponents[0]); i++) {
                IComponent* c = mComponents[i];
                c->update();
            }
        }

    } // namespace gui
} // namespace pe
