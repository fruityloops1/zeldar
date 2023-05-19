#include "imgui.h"
#include "pe/DbgGui/MenuBar.h"
#include <sead/heap/seadHeap.h>
#include <sead/heap/seadHeapMgr.h>

namespace pe {
    namespace gui {

        void MenuBar::update() {}

        void MenuBar::draw() {
            if (ImGui::BeginMainMenuBar()) {
                if (ImGui::BeginMenu("Windows")) {
                    ImGui::Checkbox("Demo Window", &getDbgGuiSharedData().showDemoWindow);
                    ImGui::Checkbox("Camera", &getDbgGuiSharedData().showCamera);
                    ImGui::Checkbox("HeapViewer", &getDbgGuiSharedData().showHeapViewer);
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }
        }

    } // namespace gui
} // namespace pe
