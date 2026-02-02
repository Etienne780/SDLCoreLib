#pragma once

class App;
class Layer {
public:
    Layer(App* app) 
        : m_app(app) {
    }
    virtual ~Layer() = default;

    virtual void Update() = 0;
    virtual void Render() = 0;

protected:
    App* m_app = nullptr;
};