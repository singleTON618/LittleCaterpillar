//SoundManager.hpp
#pragma once
#include <SFML/Audio.hpp>
#include <iostream>
#include <unordered_map>
#include <string>

class SoundManager {
private:
    // 1. Закрываем конструкторы, чтобы никто не мог создать объект через `new` или на стеке
    // 1. Приватный конструктор и деструктор
    SoundManager() = default;
    ~SoundManager() = default;

    // 2. Удаляем конструктор копирования и оператор присваивания
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    // Внутренние ресурсы: хранилище буферов и сам объект воспроизведения
    std::unordered_map<std::string, sf::SoundBuffer> buffers;
    // Решение проблемы SFML 3: управляем памятью звука через умный указатель
    std::unique_ptr<sf::Sound> soundDevice;

public:

    // 3. Глобальная точка доступа (Статический метод)
    static SoundManager& getInstance() {
        // Статический объект создастся ровно один раз при первом вызове метода
        static SoundManager instance;
        return instance;
    }

    // Метод для загрузки звука в память
    bool loadSound(const std::string& name, const std::string& filename) {
        sf::SoundBuffer buffer;
        // В SFML 3 метод загрузки возвращает bool (заменил старый loadFromFile)
        if (buffer.loadFromFile(filename)) {
            buffers[name] = buffer;
            return true;
        }
        std::cerr << "Failed to load sound: " << filename << "\n";
        return false;
    }

    // Метод для воспроизведения
    void play(const std::string& name) {
        auto it = buffers.find(name);
        if (it != buffers.end()) {

            // Пересоздаем объект звука в куче, передавая ему ссылку на буфер
            // Теперь это полностью соответствует требованиям конструктора SFML 3
            soundDevice = std::make_unique<sf::Sound>(it->second);
            soundDevice->play();
        }
    }
};
