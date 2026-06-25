#include "Portal.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <random>

Portal::Portal() : m_position({0, 0}), m_isActive(false) {}

void Portal::spawn(int gridWidth, int gridHeight) {
    m_position.x = rand() % gridWidth;
    m_position.y = rand() % gridHeight;
    m_isActive = true;
}

void Portal::deactivate() {
    m_isActive = false;
}

void Portal::render(sf::RenderWindow& window) {
    if (!m_isActive) return;

    // Рисуем портал как красивую круглую энергетическую воронку
    // Радиус 10.f идеально вписывается в тайл 20x20
    sf::CircleShape energyRing(10.f);
    energyRing.setOrigin({10.f, 10.f});
    
    // Вычисляем центр нужной клетки на сетке
    float pixelX = m_position.x * m_tileSize + m_tileSize / 2.f;
    float pixelY = m_position.y * m_tileSize + m_tileSize / 2.f;
    energyRing.setPosition({pixelX, pixelY});

    // 1. Внешнее фиолетовое кольцо
    energyRing.setFillColor(sf::Color::Transparent);
    energyRing.setOutlineColor(sf::Color(157, 78, 221)); // Ярко-фиолетовый
    energyRing.setOutlineThickness(2.f);
    window.draw(energyRing);

    // 2. Внутреннее светящееся ядро (чуть меньше размером)
    sf::CircleShape core(6.f);
    core.setOrigin({6.f, 6.f});
    core.setPosition({pixelX, pixelY});
    core.setFillColor(sf::Color(224, 170, 255, 200)); // Неоново-розово-белый
    window.draw(core);
}
