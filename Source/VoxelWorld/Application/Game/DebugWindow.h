/*================================================================
Filename: DebugWin.h
Date: 2018.2.26
Created by AirGuanZ
================================================================*/
#pragma once

#include <Utility/Math.h>

#include <Input/InputManager.h>
#include <Screen/GUISystem.h>

class DebugWindow
{
public:
    struct Info
    {
        float FPS;
        bool actorOnGround;
        Vector3 actorPos;
        Vector3 camPos;
    };

    DebugWindow(void)
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

        GUI &gui = GUI::GetInstance();

        ImGui::SetNextWindowSize(ImVec2(400.0f, 150.0f));
        if(ImGui::Begin(u8"Debug", nullptr, ImGuiWindowFlags_NoResize |
                                          ImGuiWindowFlags_NoMove |
                                          ImGuiWindowFlags_NoCollapse))
        {
            gui.PushFont(0);

            ImGui::Text((u8"FPS: " + std::to_string(info_.FPS)).c_str());
            ImGui::Text((u8"Actor on ground: " + std::string(info_.actorOnGround ? u8"true" : u8"false")).c_str());
            ImGui::Text((u8"Actor Position: "  + ToString(info_.actorPos)).c_str());
            ImGui::Text((u8"Camera Position: " + ToString(info_.camPos)).c_str());

            gui.PopFont();
        }
        ImGui::End();
    }

private:
    Info info_;

    int openCloseKey_;
    bool visible_;
};
