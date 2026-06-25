#include "MapManager.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <random>
#include <algorithm>

void MapManager::generateLevel(int level) {
    m_obstacles.clear();

    // Формула: базово 40 камней на 1-м уровне, и добавляем по 25 камней за каждый следующий уровень
    int obstacleCount = 40 + (level - 1) * 25;
    
    // Ограничим максимальное количество камней, чтобы на карте оставалось место для ползания
    if (obstacleCount > 400) obstacleCount = 400;

    for (int i = 0; i < obstacleCount; ++i) {
        sf::Vector2i pos;
        bool validPosition = false;

        while (!validPosition) {
            pos.x = rand() % 100;
            pos.y = rand() % 100;

            // Защитная зона: запрещаем спавнить камни в радиусе 5 клеток от стартовой позиции змейки (10, 10)
            if (std::abs(pos.x - 10) < 5 && std::abs(pos.y - 10) < 5) {
                continue; 
            }

            // Проверяем, нет ли уже камня на этих координатах
            if (!isCellOccupied(pos)) {
                validPosition = true;
            }
        }
        m_obstacles.push_back(pos);
    }
}

bool MapManager::isCellOccupied(const sf::Vector2i& position) const {
    return std::find(m_obstacles.begin(), m_obstacles.end(), position) != m_obstacles.end();
}

void MapManager::clear() {
    m_obstacles.clear();
}

void MapManager::render(sf::RenderWindow& window) {
    // В SFML 3 размер камня делаем на 2 пикселя меньше тайла (18x18), чтобы оставался красивый зазор
    sf::RectangleShape block({m_tileSize - 2.f, m_tileSize - 2.f});
    block.setFillColor(sf::Color(90, 100, 110));         // Стильный стальной/каменный цвет
    block.setOutlineColor(sf::Color(140, 150, 160));     // Светло-серый контур для объема
    block.setOutlineThickness(-1.f);                      // Внутрь блока

    for (const auto& pos : m_obstacles) {
        block.setPosition({pos.x * m_tileSize + 1.f, pos.y * m_tileSize + 1.f});
        window.draw(block);
    }
}
