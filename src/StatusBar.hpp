#pragma once
#include <memory>

namespace sf {
    class RenderWindow;
}

class StatusBar {
public:
    StatusBar();
    ~StatusBar();

    void setScore(int score);
    void setGameOver(bool isGameOver);
    void render(sf::RenderWindow& window);
    void setVolume(float volume); // [ДОБАВЛЕНО] Метод установки громкости            
  

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    int m_score;
    bool m_isGameOver;
    
    
};
