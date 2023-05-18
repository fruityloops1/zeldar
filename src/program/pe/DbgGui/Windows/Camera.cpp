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

        sead::Matrix34f makeViewMtx(const sead::Vector3f& pos, const sead::Vector3f& target, const sead::Vector3f& up) {
            sead::Vector3f zaxis = pos - target;
            zaxis.normalize();

            sead::Vector3f xaxis;
            xaxis.setCross(up, zaxis);
            xaxis.normalize();

            sead::Vector3f yaxis;
            yaxis.setCross(zaxis, xaxis);
            yaxis.normalize();

            sead::Matrix34f mtx;
            mtx.m[0][0] = xaxis.x;
            mtx.m[0][1] = xaxis.y;
            mtx.m[0][2] = xaxis.z;
            mtx.m[1][0] = yaxis.x;
            mtx.m[1][1] = yaxis.y;
            mtx.m[1][2] = yaxis.z;
            mtx.m[2][0] = zaxis.x;
            mtx.m[2][1] = zaxis.y;
            mtx.m[2][2] = zaxis.z;
            mtx.setBase(3, {-pos.dot(xaxis), -pos.dot(yaxis), -pos.dot(zaxis)});

            return mtx;
        }

        static sead::Vector3f sUp{0, 1, 0}, sPos{0, 0, 0}, sAt{1, 0, 1}, sDirection{1, 0, 1};

        static bool sEnabled = false;

        static float sWheelMoveVel = 0, sCameraSpeed = 3.2;
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
                sCameraSpeed += scrollDelta.x / 1500;

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

                if (sEnabled) {
                    camera->mMatrix = makeViewMtx(sPos, sAt, sUp);
                } else {
                    // camera->getWorldPosByMatrix(&sPos);
                    // camera->getLookVectorByMatrix(&sAt);
                    // camera->getUpVectorByMatrix(&sUp);
                }
            }
        }

        Camera::Camera() { LayerCalc::InstallAtOffset(0x009451d0); }

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
