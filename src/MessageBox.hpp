#pragma once
#include <memory>
#include <string>

namespace sf {
    class RenderWindow;
    class Font;
}

class MessageBox {
public:
    // Конструктор принимает ссылку на шрифт, чтобы не загружать его заново с диска
    MessageBox(const sf::Font& font);
    ~MessageBox();

    // Задает текст счета и рекорда для отображения
    void setScores(int currentScore, int highScore);
    
    // Проверяет, кликнул ли пользователь мышкой по кнопке "ОК"
    bool handleMouseClick(int mouseX, int mouseY);

    // Отрисовка окна (рисуется поверх игрового поля, центрируясь по камере)
    void render(sf::RenderWindow& window, float centerX, float centerY);

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
