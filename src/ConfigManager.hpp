#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <json.hpp> // Подключаем библиотеку nlohmann/json

// Для удобства сокращаем пространство имен библиотеки
using json = nlohmann::json;

class ConfigManager {
private:
    // 1. Приватные конструкторы для реализации паттерна Синглтон
    ConfigManager() = default;
    ~ConfigManager() = default;

    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    // Внутренние переменные, которые мы будем хранить в памяти
    int m_highScore = 0;
    const std::string m_configPath = "config.json";

public:
    // 2. Глобальная точка доступа
    static ConfigManager& getInstance() {
        static ConfigManager instance;
        return instance;
    }

    // Загрузка данных из JSON файла при старте игры
    void load() {
        std::ifstream file(m_configPath);
        if (!file.is_open()) {
            // Если файла нет, это нормально для первого запуска. Создаем дефолтные значения.
            m_highScore = 0;
            save();
            return;
        }

        try {
            json data;
            file >> data;
            
            // Читаем значение рекорда. Если поля нет в JSON, вернется 0.
            m_highScore = data.value("high_score", 0);
        } 
        catch (const json::exception& e) {
            std::cerr << "Ошибка чтения JSON конфигурации: " << e.what() << "\n";
            m_highScore = 0;
        }
    }

    // Сохранение данных в JSON файл
    void save() const {
        std::ofstream file(m_configPath);
        if (!file.is_open()) {
            std::cerr << "Ошибка открытия файл для записи конфигурации!\n";
            return; 
        }

        // Создаем структуру JSON
        json data;
        data["high_score"] = m_highScore;

        // Записываем в файл с красивыми отступами в 4 пробела
        file << data.dump(4); 
    }

    // Геттер и сеттер для рекорда
    int getHighScore() const { return m_highScore; }
    
    void setHighScore(int score) { 
        m_highScore = score; 
        save(); // Сразу сохраняем на диск при обновлении рекорда
    }
};
