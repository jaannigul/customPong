#pragma once
#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>

class Menu
{
public:
    
    enum MenuState {
        Closed, MainMenu, SettingsMenu
    };
    MenuState showMenu(sf::RenderWindow& window);
    MenuState showSettingsMenu(sf::RenderWindow& window, float& ballRadius, float& minBallVelocity, float& maxBallVelocity);

    MenuState menustate = Closed;
    bool settingsOpen;

    struct BallProperties {
        float radius = 25.0f;
        float minVelocity = 1.0f;
        float maxVelocity = 500.0f;
    };

    
    Menu(BallProperties& ballProps);
    BallProperties& ballProperties;
private:
    
};
#endif