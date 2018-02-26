/*================================================================
Filename: DebugWin_Main.h
Date: 2018.2.26
Created by AirGuanZ
================================================================*/
#ifndef VW_DEBUG_WIN_MAIN_H
#define VW_DEBUG_WIN_MAIN_H

#include <Utility/Math.h>

#include <Input/InputManager.h>
#include <Screen/GUISystem.h>

class DebugWin_Main
{
public:
    struct Info
    {
        float FPS;
        bool actorOnGround;
        Vector3 actorPos;
        Vector3 camPos;
    };

    DebugWin_Main(void)
    {
        info_.actorOnGround = false;
        info_.FPS = 0.0f;

        openCloseKey_ = VK_F3;
        visible_ = false;
    }

    void SetInfo(const Info &info)
    {
        info_ = info;
    }

    void Update(InputManager &input)
    {
        if(input.IsKeyPressed(openCloseKey_))
            visible_ = !visible_;
    }

    void Render(void)
    {
        if(!visible_)
            return;

        GUISystem &gui = GUISystem::GetInstance();

        ImGui::SetNextWindowSize(ImVec2(400.0f, 150.0f));
        if(ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoResize |
                                          ImGuiWindowFlags_NoMove |
                                          ImGuiWindowFlags_NoCollapse))
        {
            gui.PushFont(0);

            ImGui::Text(("FPS: " + std::to_string(info_.FPS)).c_str());
            ImGui::Text(("Actor on ground: " + std::string(info_.actorOnGround ? "true" : "false")).c_str());
            ImGui::Text(("Actor Position: "  + ToString(info_.actorPos)).c_str());
            ImGui::Text(("Camera Position: " + ToString(info_.camPos)).c_str());

            gui.PopFont();
        }
        ImGui::End();
    }

private:
    Info info_;

    int openCloseKey_;
    bool visible_;
};

#endif //VW_DEBUG_WIN_MAIN_H
