#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

class MapManager {
public:
    MapManager() = default;

    // Генерирует камни на карте 100х100 в зависимости от номера уровня
    void generateLevel(int level);

    // Проверяет, занята ли данная клетка камнем (для коллизий змейки и спавна еды)
    bool isCellOccupied(const sf::Vector2i& position) const;

    // Отрисовка всех камней на экране
    void render(sf::RenderWindow& window);

    // Полный сброс карты
    void clear();

private:
    std::vector<sf::Vector2i> m_obstacles; // Массив координат всех камней
    const float m_tileSize = 20.f;
};
