#pragma once
#include <memory>
#include <optional>
#include <SFML/Window/Event.hpp>

namespace sf {
    class RenderWindow;
}

class GameWindow {
public:
    GameWindow(unsigned int width, unsigned int height, const std::string& title);
    ~GameWindow();

    bool isOpen() const;
    void close();
    
    // Возвращает событие, если оно есть (стиль SFML 3)
    std::optional<sf::Event> pollEvent();
    
    void clear(uint8_t r, uint8_t g, uint8_t b);
    void display();

    // Предоставляем доступ к сырому окну для функций отрисовки других классов
    sf::RenderWindow& getRenderWindow();

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
