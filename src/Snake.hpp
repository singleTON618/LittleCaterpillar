#pragma once
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <SFML/Graphics/Color.hpp> // [ДОБАВЛЕНО] Чтобы класс знал тип sf::Color


// Опережающее объявление класса отрисовки из SFML
namespace sf {
    class RenderWindow;
}

// Направления движения змейки
enum class Direction {
    Up,
    Down,
    Left,
    Right
};

class Snake {
public:
    Snake();
    void shrink();                                                  // Уменьшение хвоста при поедании еды
    void handleInput();                                             // Считывание нажатий клавиатуры
    void update();                                                  // Логический шаг движения
    void grow();                                                    // Увеличение хвоста при поедании еды
    void render(sf::RenderWindow& window);                          // Отрисовка змейки на экране
    void setColorScheme(sf::Color headColor, sf::Color bodyColor);  // Установка цветовой схемы змейки
    bool checkSelfCollision() const;                    // Врезалась ли в свой хвост
    sf::Vector2i getHeadPosition() const;               // Получить координаты головы

private:
    std::vector<sf::Vector2i> m_body;   // Массив сегментов тела (m_body[0] — голова)
    Direction m_direction;              // Текущее направление движения
    Direction m_nextDirection;          // Защита от быстрого двойного нажатия
    bool m_growPending;                 // Флаг: нужно ли вырасти на следующем шаге
    int m_tileSize = 20;          // Размер одной клетки в пикселях
    sf::Color m_headColor = sf::Color(0, 190, 0);   // Дефолтный темно-зеленый
    sf::Color m_bodyColor = sf::Color(50, 255, 50); // Дефолтный светло-зеленый
};
