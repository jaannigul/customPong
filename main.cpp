#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include "Menu.h"
#include "imgui.h"
#include "imgui\imgui-SFML.h"

Menu::BallProperties ballProperties;
Menu menu(ballProperties);

//ball and game properties
const float paddleWidth = 20.0f;
const float paddleHeight = 128.0f;
const float paddleSpeed = 300.0f;
float ballRadius = ballProperties.radius;
const float deltaTime = 0.016f; // Adjust this value based on your frame rate
float minBallVelocity = ballProperties.minVelocity;
float maxBallVelocity = ballProperties.maxVelocity;
const float ballSpeedX = 250.0f;
const float ballSpeedY = 0.0f;

const float ballSpeedXmultiplier = 0.095f;
const float ballSpeedYmultiplier = 1.25f;

float absXVelocity;
float absYVelocity;
float hitPosition;

//window size properties
const unsigned int windowSizeX = 1000;
const unsigned int windowSizeY = 600;
//other
unsigned int maxFramerate = 144;
sf::Font scoreFont; //https://www.fontspace.com/category/scoreboard
sf::Vector2f ballVelocity(ballSpeedX,ballSpeedY);
unsigned int rightPts = 0;
unsigned int leftPts = 0;

sf::VertexArray createDottedLine(sf::RenderWindow& window) {
    const int numSegments = 50; // Number of line segments to create the dotted effect
    sf::VertexArray dottedLine(sf::Lines, numSegments * 2);

    sf::Vector2f topCenter(window.getSize().x / 2.0f, 0.0f);
    sf::Vector2f bottomCenter(window.getSize().x / 2.0f, window.getSize().y);
    sf::Vector2f lineSegment;

    float gap = windowSizeY / (2 * numSegments); //Gap between each line segment

    for (int i = 0; i < numSegments; ++i)
    {
        lineSegment = topCenter + sf::Vector2f(0.0f, 2 * i * gap);
        dottedLine[i * 2].position = lineSegment;

        lineSegment = topCenter + sf::Vector2f(0.0f, (2 * i + 1) * gap);
        dottedLine[i * 2 + 1].position = lineSegment;
    }

    sf::Color dottedLineColor = sf::Color::White;
    for (int i = 0; i < numSegments * 2; ++i)
    {
        dottedLine[i].color = dottedLineColor;
    }
    return dottedLine;
}

void handlePaddleMovement(sf::RectangleShape& paddle, sf::Keyboard::Key upKey, sf::Keyboard::Key downKey,sf::RenderWindow &window)
{
    if (sf::Keyboard::isKeyPressed(upKey))
    {
        if (paddle.getPosition().y > 0.0f)
        paddle.move(0.0f, -paddleSpeed * deltaTime); // Move up
    }
    if (sf::Keyboard::isKeyPressed(downKey))
    {
        if (paddle.getPosition().y + paddleHeight < window.getSize().y)
        paddle.move(0.0f, paddleSpeed * deltaTime); // Move down
    }
}

void resetBallSpeed() {
    ballVelocity.x = 250.0f; // Reset ball speed and direction
    ballVelocity.y = 0.0f;
}

float calcVelocityX(float hitPosition) {
    return  1.0f + std::abs(hitPosition) * ballSpeedXmultiplier;
}

float calcVelocityY(float hitPosition) {
    return hitPosition * ballSpeedYmultiplier * maxBallVelocity;
}

enum class Serve {
   right, left
};

enum class GameState {
    Start, Serve, Play, Paused 
};

sf::Text createLeftScoreText() {
    sf::Text leftScoreText;
    leftScoreText.setFont(scoreFont);
    leftScoreText.setCharacterSize(30);
    leftScoreText.setFillColor(sf::Color::White);
    leftScoreText.setPosition(50.0f, 20.0f); 
    return leftScoreText;
}

sf::Text createRightScoreText() {
    sf::Text rightScoreText;
    rightScoreText.setFont(scoreFont);
    rightScoreText.setCharacterSize(30);
    rightScoreText.setFillColor(sf::Color::White);
    rightScoreText.setPosition(windowSizeX - 100.0f, 20.0f);
    return rightScoreText;
}

Serve currentServer = Serve::right;

int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(windowSizeX, windowSizeY), "skibidi toilet " , sf::Style::Titlebar | sf::Style::Close, settings);

    window.setFramerateLimit(maxFramerate);

    sf::Texture ballTextureNormal;
    sf::Texture ballTextureFast;
    sf::Texture ballTextureFastest;

    
    if (!ballTextureNormal.loadFromFile("res\\ball_normal3.jpg") ||
        !ballTextureFast.loadFromFile("res\\ball_normal4.jpg") || 
        !ballTextureFastest.loadFromFile("res\\ball_normal5.png") ||
        !scoreFont.loadFromFile("res\\scorefont.ttf"))
    {
        std::cout << "Could not load image(s) or font" << std::endl;
        return -1;
    }
    sf::Text rightScoreText;
    sf::Text leftScoreText;

    sf::CircleShape ballShape(ballRadius);
    ballShape.setOrigin(ballRadius, ballRadius);
    ballShape.setTexture(&ballTextureNormal);

    
    // Paddle 1 (Left paddle)
    sf::RectangleShape paddle1(sf::Vector2f(paddleWidth, paddleHeight));
    paddle1.setFillColor(sf::Color::White);
    paddle1.setPosition(50.0f, window.getSize().y / 2.0f - paddleHeight / 2.0f);

    // Paddle 2 (Right paddle)
    sf::RectangleShape paddle2(sf::Vector2f(paddleWidth, paddleHeight));
    paddle2.setFillColor(sf::Color::White);
    paddle2.setPosition(window.getSize().x - 50.0f - paddleWidth, window.getSize().y / 2.0f - paddleHeight / 2.0f);

    float centerX = window.getSize().x / 2.0f;
    float centerY = window.getSize().y / 2.0f;

    // Set the balls's position to the center of the window
    ballShape.setPosition(centerX, centerY);

    GameState gameState = GameState::Start;
    sf::Vector2f servePosition(centerX, centerY); // Position to start the serve
   
    bool serveReady = true;

    sf::Clock pauseClock;


    ImGui::SFML::Init(window);
    // Main game loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }


        if (menu.menustate == Menu::MenuState::SettingsMenu || menu.menustate == Menu::MenuState::MainMenu) {
            switch (menu.menustate) {
            case (Menu::MenuState::MainMenu):
                menu.menustate = menu.showMenu(window);
                break;
            case (Menu::MenuState::SettingsMenu):
                menu.menustate = menu.showSettingsMenu(window, ballRadius, minBallVelocity, maxBallVelocity);
                ballRadius = ballProperties.radius;
                minBallVelocity = ballProperties.minVelocity;
                maxBallVelocity = ballProperties.maxVelocity;

                // update game properties here
                ballShape.setOrigin(ballRadius, ballRadius);
                ballShape.setRadius(ballRadius);
                break;
            }
            
        } else {

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            menu.menustate = Menu::MenuState::MainMenu;
        }

        // Handle ball reset and serve start
        if ((ballShape.getPosition().x + ballRadius + ballRadius < 0.0f) || // Ball goes past the left border
            (ballShape.getPosition().x - ballRadius + ballRadius > window.getSize().x)) // Ball goes past the right border
        {
            ballShape.setPosition(centerX, centerY);
            resetBallSpeed();
            paddle1.setPosition(50.0f, window.getSize().y / 2.0f - paddleHeight / 2.0f);
            paddle2.setPosition(window.getSize().x - 50.0f - paddleWidth, window.getSize().y / 2.0f - paddleHeight / 2.0f);

            currentServer = (currentServer == Serve::right) ? Serve::left : Serve::right;
            serveReady = true;
            gameState = GameState::Start;
        }

        // Handle serve start
        if (gameState == GameState::Start)
        {
            rightScoreText = createRightScoreText();
            leftScoreText = createLeftScoreText();
            rightScoreText.setString(std::to_string(rightPts));
            leftScoreText.setString(std::to_string(leftPts));
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) // Left player's paddle moved
            {
                gameState = GameState::Serve;
                servePosition = (currentServer == Serve::right) ? paddle2.getPosition() : paddle1.getPosition();
                serveReady = false;
            }
        }

        // Handle serve
        if (gameState == GameState::Serve)
        {
            if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) // Left player's paddle not moving
            {
                ballShape.setPosition(centerX, centerY);
                ballVelocity.x = currentServer == Serve::right ? std::abs(ballVelocity.x) : -std::abs(ballVelocity.x);
                ballVelocity.y = 0.0f;
                gameState = GameState::Play;
            }
        }


        handlePaddleMovement(paddle1, sf::Keyboard::W, sf::Keyboard::S, window);
        handlePaddleMovement(paddle2, sf::Keyboard::Up, sf::Keyboard::Down, window);


        if (gameState == GameState::Play)
        {
            ballShape.move(ballVelocity * deltaTime);

            if (ballShape.getPosition().y - ballRadius < 0.0f) // Ball hits the top edge
            {
                ballVelocity.y = std::abs(ballVelocity.y); // Reverse vertical direction
            }
            else if (ballShape.getPosition().y + ballRadius > window.getSize().y) // Ball hits the bottom edge
            {
                ballVelocity.y = -std::abs(ballVelocity.y); // Reverse vertical direction
            }

            // Ball collision with left and right edges (when the ball goes out of bounds)
            if (ballShape.getPosition().x + ballRadius + ballRadius < 0.0f) // Ball goes past the left border
            {
                rightPts++;
                ballShape.setPosition(centerX, centerY); // Reset the ball position to the center
                resetBallSpeed();
                paddle1.setPosition(50.0f, window.getSize().y / 2.0f - paddleHeight / 2.0f);
                paddle2.setPosition(window.getSize().x - 50.0f - paddleWidth, window.getSize().y / 2.0f - paddleHeight / 2.0f);
                currentServer = Serve::right; 
                serveReady = true; // Set the serveReady flag to true to allow serving again
                gameState = GameState::Start; // Change the game state to start for the next serve
            }
            else if (ballShape.getPosition().x - ballRadius + ballRadius > window.getSize().x) // Ball goes past the right border
            {
                leftPts++;
                ballShape.setPosition(centerX, centerY); // Reset the ball position to the center
                resetBallSpeed();
                paddle1.setPosition(50.0f, window.getSize().y / 2.0f - paddleHeight / 2.0f);
                paddle2.setPosition(window.getSize().x - 50.0f - paddleWidth, window.getSize().y / 2.0f - paddleHeight / 2.0f);
                currentServer = Serve::left; // Change the server
                serveReady = true; // Set the serveReady flag to true to allow serving again
                gameState = GameState::Start; // Change the game state to start for the next serve
            }

            // Ball collision with paddles
            sf::FloatRect ballBounds = ballShape.getGlobalBounds();
            sf::FloatRect paddle1Bounds = paddle1.getGlobalBounds();
            sf::FloatRect paddle2Bounds = paddle2.getGlobalBounds();

            if (ballBounds.intersects(paddle1Bounds))
            {
                // Calculate new ball velocity based on the collision point
                hitPosition = (ballShape.getPosition().y - paddle1.getPosition().y) / paddleHeight - 0.5f;
                //std::cout << hitPosition << std::endl;
                ballVelocity.x = std::abs(ballVelocity.x) * calcVelocityX(hitPosition); // Reverse horizontal direction
                
                // Adjust the vertical velocity based on hit position
                ballVelocity.y = calcVelocityY(hitPosition);
                // Ensure the vertical velocity is at least minBallVelocity
                if (std::abs(ballVelocity.y) < minBallVelocity)
                {
                    ballVelocity.y = ballVelocity.y > 0 ? minBallVelocity : -minBallVelocity;
                }
            }
            else if (ballBounds.intersects(paddle2Bounds))
            {
                // Calculate new ball velocity based on the collision point
                hitPosition = (ballShape.getPosition().y - paddle2.getPosition().y) / paddleHeight - 0.5f;
                ballVelocity.x = -std::abs(ballVelocity.x) * calcVelocityX(hitPosition); // Reverse horizontal direction
                //std::cout << hitPosition << std::endl;
                // Adjust the vertical velocity based on hit position
                ballVelocity.y = calcVelocityY(hitPosition);
                // Ensure the vertical velocity is at least minBallVelocity
                if (std::abs(ballVelocity.y) < minBallVelocity)
                {
                    ballVelocity.y = ballVelocity.y > 0 ? minBallVelocity : -minBallVelocity;
                }
            }
        }
        //std::cout << " x = " << ballVelocity.x << ", y= " << ballVelocity.y << std::endl;
        absXVelocity = std::abs(ballVelocity.x);
        absYVelocity = std::abs(ballVelocity.y);
        //std::cout << " x = " << absXVelocity << ", y= " << absYVelocity << std::endl;
        if (absYVelocity < 100.0f) {
            ballShape.setTexture(&ballTextureNormal); //Normal velocity
        }
        else if ((absYVelocity > 100.0f && absYVelocity < 400.0f)) {
            ballShape.setTexture(&ballTextureFast); //Fast velocity
        }
        
        else if (absYVelocity >= 400.0f ) {
            ballShape.setTexture(&ballTextureFastest); //Fastest velocity
        }
}
        
        //logic end
        window.clear();
        if (gameState == GameState::Start && serveReady)
        {
            window.draw(leftScoreText);
            window.draw(rightScoreText);
        }

        window.draw(ballShape);
        window.draw(paddle1);
        window.draw(paddle2);
        window.draw(createDottedLine(window));
        window.display();
    }

    return 0;
}

