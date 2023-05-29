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
        static bool sAmbientOverrideBrightness = false;
        static bool sFog = true;

        void PrePassLight(gsys::ModelSceneGI *x0, agl::DrawContext *x1, gsys::ModelSceneContext *x2, gsys::RenderBufferContext &x3) {
            if (sPrePassLightState)
                x0->drawLightPrePass(x1, x2, x3);
        }

        void UltraHandVoxel() {
            if (sUltraHandVoxel)
                __asm("LDRB W8, [X0,#8]");
            else __asm("MOV W8, #0");
        }

        void AmbientOverrideBrightness() {
            __asm("FMOV S7, S0");
            __asm("fmov s0, %s0" : : "w"(sAmbientOverrideBrightness ? 12.0f : 1.0f));
            __asm("FMOV S1, S0");
            __asm("FMOV S0, S7");
        }
        void Fog() {
            __asm("FMOV S7, S0");
            __asm("FMOV S0, %s0" : : "w" (sFog ? 1.0f : 0.0f));
            __asm("FMOV S4, S0");
            __asm("FMOV S0, S7");
        }
    }

    Graphics::Graphics() {
        using Patcher = exl::patch::CodePatcher;
        using namespace exl::patch::inst;

        Patcher(0x00A5ADDC).BranchLinkInst((void*)GraphicsHooks::PrePassLight);
        Patcher(0x01279798).BranchLinkInst((void*)GraphicsHooks::AmbientOverrideBrightness);
        Patcher(0x0096A528).BranchLinkInst((void*)GraphicsHooks::Fog);

    }

    void Graphics::update()
    {
        
    }

    void Graphics::draw()
    {
        if (getDbgGuiSharedData().showGraphics && ImGui::Begin("Graphics")) {
            ImGui::Checkbox("Pre-Pass Lights", &GraphicsHooks::sPrePassLightState);
            if (ImGui::Checkbox("Ultrahand Voxels", &GraphicsHooks::sUltraHandVoxel)) {
                exl::patch::CodePatcher(0x0095D880).Write(0x940003DD);
            } else {
                exl::patch::CodePatcher(0x0095D880).WriteInst(exl::armv8::inst::Nop());
            }

            ImGui::Checkbox("Override Ambient Brightness", &GraphicsHooks::sAmbientOverrideBrightness);
            ImGui::Checkbox("Fog", &GraphicsHooks::sFog);

            ImGui::End();
        }
    }

} // namespace gui
} // namespace pe
