
#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

class Food {
public:
    enum class Type {
        SpeedBoost, // Желтое
        SlowDown,   // Оранжевое
        Normal,     // Красное
        Shrink      // Бордовое
    };

    struct Apple {
        sf::Vector2i position;
        Type type;
        sf::Color color;
    };

    Food() = default;

    // Спавнит заданное количество яблок на карте указанного размера
    void spawnMultiple(int count, int gridWidth, int gridHeight);
    
    // Удаляет съеденное яблоко и спавнит вместо него новое
    void replaceApple(size_t index, int gridWidth, int gridHeight);

    // Возвращает массив всех текущих яблок на карте
    const std::vector<Apple>& getApples() const { return m_apples; }

    void render(sf::RenderWindow& window);

private:
    std::vector<Apple> m_apples;
    const float m_tileSize = 20.f;
};
