#include "heap/seadHeapMgr.h"
#include "helpers/InputHelper.h"
#include "hid.h"
#include "hook/trampoline.hpp"
#include "imgui.h"
#include "patch/code_patcher.hpp"
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

        struct DisgustingLookAtCamera {
            char _0[0x50];
            sead::Vector3f mUp;
            int _5C;
            sead::Vector3<double> mPos;
            sead::Vector3<double> mAt;

            void doUpdateMatrix(sead::Matrix34f* out);
        };

        static sead::Vector3f sUp{0, 1, 0}, sPos{0, 0, 0}, sAt{1, 0, 1}, sDirection{1, 0, 1};

        static bool sEnabled = false;

        static float sWheelMoveVel = 0, sCameraSpeed = 1.0;
        static sead::Vector2f sMouseDelta{0, 0}, sLastMousePos{0, 0}, sCameraMoveVel{0, 0};

        static sead::Vector3f rotateVectorY(const sead::Vector3f& vector, float angle) {
            float cosAngle = std::cos(angle);
            float sinAngle = std::sin(angle);
            return {vector.x * cosAngle - vector.z * sinAngle, vector.y, vector.x * sinAngle + vector.z * cosAngle};
        }

        void updateCameraInput() {
            sead::Vector2f mousePos;
            InputHelper::getMouseCoords(&mousePos.x, &mousePos.y);

            sMouseDelta = mousePos - sLastMousePos;
            sLastMousePos = mousePos;

            sead::Vector2f scrollDelta;
            InputHelper::getScrollDelta(&scrollDelta.x, &scrollDelta.y);

            if (InputHelper::isMouseHold(nn::hid::MouseButton::Middle)) {
                sCameraSpeed += scrollDelta.x / 3500;

                if (sCameraSpeed <= 0)
                    sCameraSpeed = .0625;

                sAt = sPos + sDirection;

                sWheelMoveVel /= 1.4;
                sCameraMoveVel /= 1.4;
                return;
            }

            sWheelMoveVel += scrollDelta.x / 100;
            if ((sMouseDelta.x != 0 || sMouseDelta.y != 0) && InputHelper::isMouseHold(nn::hid::MouseButton::Right))
                sCameraMoveVel += sMouseDelta * .0006f;

            sDirection = rotateVectorY(sDirection, sCameraMoveVel.x);
            sDirection.y += -sCameraMoveVel.y;
            sDirection.normalize();

            if (sWheelMoveVel != 0) {
                float degree = (mousePos.x + float(1280) / 2) / 1280 - 1;
                sead::Vector3f dir = rotateVectorY(sDirection, degree);
                dir.y += -(mousePos.y / 720 - 0.5);
                sPos += dir * (sWheelMoveVel * .25) * sCameraSpeed;
            }

            if (InputHelper::isKeyHold(nn::hid::KeyboardKey::W) || InputHelper::isKeyHold(nn::hid::KeyboardKey::S)) {
                float speed = sCameraSpeed / 10;

                if (InputHelper::isKeyHold(nn::hid::KeyboardKey::W))
                    sPos += sDirection * speed;
                else
                    sPos -= sDirection * speed;
            }

            if (InputHelper::isKeyHold(nn::hid::KeyboardKey::A) || InputHelper::isKeyHold(nn::hid::KeyboardKey::D)) {
                float speed = sCameraSpeed / 10;

                sead::Vector3f zaxis = sPos - sAt;
                zaxis.normalize();
                sead::Vector3f xaxis;
                xaxis.setCross(sUp, zaxis);
                xaxis.normalize();

                if (InputHelper::isKeyHold(nn::hid::KeyboardKey::A))
                    sPos -= xaxis * speed;
                else
                    sPos += xaxis * speed;
            }

            if (InputHelper::isKeyHold(nn::hid::KeyboardKey::Space) ||
                InputHelper::isKeyHold(nn::hid::KeyboardKey::LeftShift)) {
                float speed = sCameraSpeed / 10;

                sead::Vector3f zaxis = sPos - sAt;
                zaxis.normalize();
                sead::Vector3f xaxis;
                xaxis.setCross(sUp, zaxis);
                xaxis.normalize();
                sead::Vector3f yaxis;
                yaxis.setCross(zaxis, xaxis);
                yaxis.normalize();

                if (InputHelper::isKeyHold(nn::hid::KeyboardKey::Space))
                    sPos += yaxis * speed;
                else
                    sPos -= yaxis * speed;
            }

            sAt = sPos + sDirection;

            sWheelMoveVel /= 1.4;
            sCameraMoveVel /= 1.4;
        }

        void disgustingCameraHook(DisgustingLookAtCamera* thisPtr, sead::Matrix34f* out) {
            updateCameraInput();

            if (InputHelper::isKeyHold(nn::hid::KeyboardKey::O) ||
                (InputHelper::isButtonHold(nn::hid::NpadButton::ZR) &&
                 InputHelper::isButtonHold(nn::hid::NpadButton::Plus))) {
                sEnabled = !sEnabled;
            }

            if (sEnabled) {
                thisPtr->mPos = {double(sPos.x), double(sPos.y), double(sPos.z)};
                thisPtr->mAt = {double(sAt.x), double(sAt.y), double(sAt.z)};
                thisPtr->mUp = sUp;
            } else {
                sUp = thisPtr->mUp;
                sPos = {float(thisPtr->mPos.x), float(thisPtr->mPos.y), float(thisPtr->mPos.z)};
                sAt = {float(thisPtr->mAt.x), float(thisPtr->mAt.y), float(thisPtr->mAt.z)};
                sDirection = sAt - sPos;
            }
            thisPtr->doUpdateMatrix(out);
        }

        void LayerCalc::Callback(char* thisPtr, sead::Controller* controller, int thing, bool thing2, int thing3) {
            Orig(thisPtr, controller, thing, thing2, thing3);
            InputHelper::updatePadState();
        }

        Camera::Camera() {
            LayerCalc::InstallAtOffset(0x009451d0);
            exl::patch::CodePatcher(0x00cdb894).BranchLinkInst((void*)disgustingCameraHook);
        }

        void Camera::update() {}

        void Camera::draw() {
            if (getDbgGuiSharedData().showCamera && ImGui::Begin("Camera")) {
                ImGui::Checkbox("Enable", &sEnabled);
                ImGui::DragFloat3("Pos", sPos.e.data(), .1);
                ImGui::DragFloat3("At", sAt.e.data(), .1);
                ImGui::DragFloat3("Up", sUp.e.data(), .1);
                ImGui::DragFloat("CameraSpeed", &sCameraSpeed, .1);
                ImGui::End();
            }
        }

    } // namespace gui
} // namespace pe
