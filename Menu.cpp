#include "Menu.h"
#include <iostream>
#include "imgui.h"
#include "imgui/imgui-SFML.h"

Menu::Menu(BallProperties& ballProps) : ballProperties(ballProps)
{
    // ...
}

Menu::MenuState Menu::showSettingsMenu(sf::RenderWindow& window, float& ballRadius, float& minBallVelocity, float& maxBallVelocity)
{
    ImGui::SFML::Init(window);
    bool settingsOpen = true;

    while (settingsOpen)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                settingsOpen = false;
            }
            ImGui::SFML::ProcessEvent(event);
        }

        ImGui::SFML::Update(window, sf::seconds(1.0f / 144.0f));
        ImGui::SetNextWindowSize(ImVec2(window.getSize().x, window.getSize().y));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin("Settings", &settingsOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);

        // Slider to change ball radius
        ImGui::SliderFloat("Ball Radius", &ballProperties.radius, 10.0f, 50.0f);
        ballRadius = ballProperties.radius; // Update ballRadius variable

        // Slider to change ball min velocity
        ImGui::SliderFloat("Ball Min Velocity", &ballProperties.minVelocity, 0.1f, 10.0f);
        minBallVelocity = ballProperties.minVelocity; // Update minBallVelocity variable

        // Slider to change ball max velocity
        ImGui::SliderFloat("Ball Max Velocity", &ballProperties.maxVelocity, 100.0f, 1000.0f);
        maxBallVelocity = ballProperties.maxVelocity; // Update maxBallVelocity variable

        if (ImGui::Button("Back"))
        {
            settingsOpen = false;
            menustate = Menu::MenuState::MainMenu;
        }

        ImGui::End();

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return menustate;
}



Menu::MenuState Menu::showMenu(sf::RenderWindow& window)
{
    ImGui::SFML::Init(window);
    bool resumeClicked = false;
    bool menuOpen = true;
    menustate = Menu::MenuState::MainMenu;
    while (menustate == Menu::MenuState::MainMenu)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                menuOpen = false;
            }
            ImGui::SFML::ProcessEvent(event);
        }

        ImGui::SFML::Update(window, sf::seconds(1.0f / 144.0f));
        ImGui::SetNextWindowSize(ImVec2(window.getSize().x, window.getSize().y));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin("Menu", &menuOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);

        if (ImGui::Button("Resume Game"))
        {
            resumeClicked = true;
            menuOpen = false;
            menustate = Menu::MenuState::Closed;
        }

        if (ImGui::Button("Settings"))
        {
            menuOpen = false; 
            settingsOpen = true;
            menustate = Menu::MenuState::SettingsMenu;

        }

        if (ImGui::Button("Exit"))
        {
            window.close();
            menuOpen = false;
            menustate = Menu::MenuState::Closed;
        }

        ImGui::End();

        ImGui::SFML::Render(window); // Render ImGui UI for main menu
        window.display();
    }

    ImGui::SFML::Shutdown();
    return menustate;
}




