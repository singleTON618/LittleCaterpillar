#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>

class Portal {
public:
    Portal();
    ~Portal() = default;

    // Активирует портал в случайных координатах на сетке
    void spawn(int gridWidth, int gridHeight);
    
    // Деактивирует портал (закрывает его)
    void deactivate();

    // Проверяет, открыт ли портал в данный момент
    bool isActive() const { return m_isActive; }

    // Возвращает текущие координаты портала на сетке
    sf::Vector2i getPosition() const { return m_position; }

    // Отрисовка портала
    void render(sf::RenderWindow& window);

private:
    sf::Vector2i m_position;
    bool m_isActive;
    const float m_tileSize = 20.f;
};
