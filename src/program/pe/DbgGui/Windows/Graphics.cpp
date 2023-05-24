#include "pe/DbgGui/Windows/Graphics.h"
#include "heap/seadHeapMgr.h"
#include "imgui.h"
#include <sead/heap/seadHeap.h>

void unk_RenderUltrahandVoxel();

namespace agl {
    class DrawContext;
}

namespace gsys {
    class ModelRenderContext;
    class ModelSceneContext;
    class RenderBufferContext;
    class ModelSceneShadow {
        public:
        void drawSSAO(agl::DrawContext *,gsys::ModelSceneContext *,gsys::RenderBufferContext const&) const;
    };
    class ModelSceneGI {
        public:
        void drawLightPrePass(agl::DrawContext*, gsys::ModelSceneContext*, gsys::RenderBufferContext const&) const;
    };
}

namespace pe {
namespace gui {
    namespace GraphicsHooks {
        static bool sPrePassLightState = true;
        static bool sUltraHandVoxel = true;

        void PrePassLight(gsys::ModelSceneGI *x0, agl::DrawContext *x1, gsys::ModelSceneContext *x2, gsys::RenderBufferContext &x3) {
            if (sPrePassLightState)
                x0->drawLightPrePass(x1, x2, x3);
        }

        void UltraHandVoxel() {
            if (sUltraHandVoxel)
                __asm("LDRB W8, [X0,#8]");
            else __asm("MOV W8, #0");
        }
    }

    Graphics::Graphics() {
        using Patcher = exl::patch::CodePatcher;
        using namespace exl::patch::inst;

        Patcher(0x00A5ADDC).BranchLinkInst((void*)GraphicsHooks::PrePassLight);
        Patcher(0x0095E814).BranchLinkInst((void*)GraphicsHooks::UltraHandVoxel);
    }

    void Graphics::update()
    {
        
    }

    void Graphics::draw()
    {
        if (getDbgGuiSharedData().showGraphics && ImGui::Begin("Graphics")) {
            ImGui::Checkbox("Pre-Pass Lights", &GraphicsHooks::sPrePassLightState);
            ImGui::Checkbox("Ultrahand Voxels", &GraphicsHooks::sUltraHandVoxel);
            ImGui::End();
        }
    }

} // namespace gui
} // namespace pe
