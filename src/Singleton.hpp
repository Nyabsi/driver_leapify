#pragma once

template<class T>
class Singleton {
private:
    Singleton() = default;
public:
    static T& get() {
        static auto instance = new T();
        return *instance;
    }

    ~Singleton() = default;
};