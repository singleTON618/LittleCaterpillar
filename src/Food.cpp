#include "Food.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <random>

void Food::spawnMultiple(int count, int gridWidth, int gridHeight) {
    m_apples.clear();
    for (int i = 0; i < count; ++i) {
        Apple apple;
        // Генерируем случайную позицию в пределах сетки мира
        apple.position.x = rand() % gridWidth;
        apple.position.y = rand() % gridHeight;

        // Случайный выбор типа яблока (с разным шансом появления)
        int roll = rand() % 100;
        if (roll < 15) { // 15% шанс
            apple.type = Type::SpeedBoost;
            apple.color = sf::Color(255, 220, 0); // Насыщенный желтый
        } else if (roll < 30) { // 15% шанс
            apple.type = Type::SlowDown;
            apple.color = sf::Color(255, 140, 0); // Оранжевый
        } else if (roll < 45) { // 15% шанс
            apple.type = Type::Shrink;
            apple.color = sf::Color(139, 0, 0);   // Бордовый / Темно-красный
        } else { // 55% шанс
            apple.type = Type::Normal;
            apple.color = sf::Color(255, 0, 0);   // Ярко-красный
        }
        m_apples.push_back(apple);
    }
}

void Food::replaceApple(size_t index, int gridWidth, int gridHeight) {
    if (index >= m_apples.size()) return;

    m_apples[index].position.x = rand() % gridWidth;
    m_apples[index].position.y = rand() % gridHeight;

    int roll = rand() % 100;
    if (roll < 15) {
        m_apples[index].type = Type::SpeedBoost;
        m_apples[index].color = sf::Color(255, 220, 0);
    } else if (roll < 30) {
        m_apples[index].type = Type::SlowDown;
        m_apples[index].color = sf::Color(255, 140, 0);
    } else if (roll < 45) {
        m_apples[index].type = Type::Shrink;
        m_apples[index].color = sf::Color(139, 0, 0);
    } else {
        m_apples[index].type = Type::Normal;
        m_apples[index].color = sf::Color(255, 0, 0);
    }
}

void Food::render(sf::RenderWindow& window) {
    // В SFML 3 уменьшаем размер отрисовки яблока на 2 пикселя, чтобы оно красиво ложилось в тайл
    sf::RectangleShape shape({m_tileSize - 2.f, m_tileSize - 2.f});

    for (const auto& apple : m_apples) {
        shape.setFillColor(apple.color);
        shape.setPosition({apple.position.x * m_tileSize + 1.f, apple.position.y * m_tileSize + 1.f});
        window.draw(shape);
    }
}
