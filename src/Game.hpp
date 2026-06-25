#pragma once

#include <SFML/System/Time.hpp>
#include <SFML/System/Clock.hpp>
#include <memory>

// Опережающие объявления наших внутренних классов
class GameWindow;
class StatusBar;
class Snake;
class Food;
class SoundManager;


namespace sf {
    class RenderWindow; // [ДОБАВЛЕНО] Опережающее объявление класса окна для аргумента функции
}


enum class GameState {
    Playing,
    GameOver
};

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    void processEvents();
    void update(sf::Time deltaTime);
    void render();
    void resetGame(); // Метод для перезапуска игры
    void renderGrid(sf::RenderWindow& window); // [ДОБАВЛЕНО] Новый метод отрисовки поля

private:
    // Все тяжелые объекты и сущности прячем в Impl

    struct Impl;
    std::unique_ptr<Impl> m_impl;
    GameState m_state;
    int       m_score;
    int       m_currentLevel; // [ДОБАВЛЕНО] Номер текущего уровня

    sf::Clock m_clock;
    sf::Time  m_timeSinceLastUpdate;
    sf::Time  m_timePerFrame; 
    sf::Time  m_baseTimePerFrame;
    sf::Time  m_effectTimer;
};
