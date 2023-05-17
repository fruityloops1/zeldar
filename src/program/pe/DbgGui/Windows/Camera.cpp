#include "heap/seadHeapMgr.h"
#include "helpers/InputHelper.h"
#include "hid.h"
#include "hook/trampoline.hpp"
#include "imgui.h"
#include "pe/DbgGui/Windows/Camera.h"
#include <sead/controller/seadController.h>
#include <sead/gfx/seadCamera.h>
#include <sead/heap/seadHeap.h>
#include <sead/math/seadMatrix.h>

namespace pe {
    namespace gui {

        HOOK_DEFINE_TRAMPOLINE(LayerCalc) {
            static void Callback(char /* agl::lyr::Layer* */* thisPtr, sead::Controller* controller, int thing,
                                 bool thing2, int thing3);
        };

        sead::Matrix34f makeViewMtx(const sead::Vector3f& eye, const sead::Vector3f& target, const sead::Vector3f& up,
                                    float fov) {
            sead::Vector3f zAxis = (eye - target);
            zAxis.normalize();

            sead::Vector3f xAxis;
            xAxis.setCross(up, zAxis);
            xAxis.normalize();

            sead::Vector3f yAxis;
            yAxis.setCross(zAxis, xAxis);
            yAxis.normalize();

            sead::Matrix34f matrix;
            matrix.setBase(0, xAxis);
            matrix.setBase(1, yAxis);
            matrix.setBase(2, zAxis);

            matrix.m[0][3] = -xAxis.x * eye.x - xAxis.y * eye.y - xAxis.z * eye.z;
            matrix.m[1][3] = -yAxis.x * eye.x - yAxis.y * eye.y - yAxis.z * eye.z;
            matrix.m[2][3] = -zAxis.x * eye.x - zAxis.y * eye.y - zAxis.z * eye.z;

            return matrix;
        }

        static sead::Vector3f sUp{0, 1, 0}, sPos{0, 0, 0}, sAt{1, 0, 1}, sDirection{1, 0, 1};
        static float sFov = 45;

        static bool sEnabled = false;

        static sead::Vector2f sMouseDelta{0, 0}, sLastMousePos{0, 0};

        static sead::Vector3f rotateVectorY(const sead::Vector3f& vector, float angle) {
            float cosAngle = std::cos(angle);
            float sinAngle = std::sin(angle);
            return {vector.x * cosAngle - vector.z * sinAngle, vector.y, vector.x * sinAngle + vector.z * cosAngle};
        }

        void updateCameraInput() {
            sead::Vector2f mousePos;
            InputHelper::getMouseCoords(&mousePos.x, &mousePos.y);

            sead::Vector2f mouseDelta = mousePos - sLastMousePos;

            // MOVE

            sDirection.normalize();

            sLastMousePos = mousePos;

            sAt = sPos + sDirection;
        }

        void LayerCalc::Callback(char* thisPtr, sead::Controller* controller, int thing, bool thing2, int thing3) {
            Orig(thisPtr, controller, thing, thing2, thing3);
            sead::Camera* camera = *(sead::Camera**)(thisPtr + 0x78); // cant be Bothered

            InputHelper::updatePadState();
            updateCameraInput();

            if (camera) {
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::O) ||
                    (InputHelper::isButtonHold(nn::hid::NpadButton::ZR) &&
                     InputHelper::isButtonPress(nn::hid::NpadButton::Plus))) {
                    sEnabled = !sEnabled;
                }

                if (sEnabled)
                    camera->mMatrix = makeViewMtx(sPos, sAt, sUp, sFov);
            }
        }

        Camera::Camera() { LayerCalc::InstallAtOffset(0x009451d0); }

        void Camera::update() {}

        void Camera::draw() {
            if (getDbgGuiSharedData().showCamera && ImGui::Begin("Camera")) {
                ImGui::Checkbox("Enable", &sEnabled);
                ImGui::End();
            }
        }

    } // namespace gui
} // namespace pe
