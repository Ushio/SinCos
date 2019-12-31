#include "pr.hpp"
#include <iostream>
#include <memory>

// http://iquilezles.org/www/articles/sincos/sincos.htm
// 
class CircleGenerator {
public:
    CircleGenerator(float stepThetaT)
        :_sinDeltaT(std::sin(stepThetaT)), _cosDeltaT(std::cos(stepThetaT)) {
    }
    CircleGenerator(float stepThetaT, float theta)
        :_sinDeltaT(std::sin(stepThetaT)), _cosDeltaT(std::cos(stepThetaT))
        ,_sinT(std::sin(theta)), _cosT(std::cos(theta)) {
    }

    // sin(theta)
    float sin() const {
        return _sinT;
    }

    // cos(theta)
    float cos() const {
        return _cosT;
    }

    // theta += stepThetaT
    void step() {
        float newSin = _sinT * _cosDeltaT + _cosT * _sinDeltaT;
        float newCos = _cosT * _cosDeltaT - _sinT * _sinDeltaT;
        _sinT = newSin;
        _cosT = newCos;
    }
private:
    float _sinDeltaT = 0.0f;
    float _cosDeltaT = 0.0f;
    float _sinT = 0.0f;
    float _cosT = 1.0f;
};

int main() {
    using namespace pr;

    Config config;
    config.ScreenWidth = 1920;
    config.ScreenHeight = 1080;
    config.SwapInterval = 1;
    Initialize(config);

    Camera3D camera;
    camera.origin = { 4, 4, 4 };
    camera.lookat = { 0, 0, 0 };
    camera.zUp = true;

    int N = 128;
    bool optimized = true;
    float theta_s = 0.0f;

    double e = GetElapsedTime();

    while (pr::NextFrame() == false) {
        if (IsImGuiUsingMouse() == false) {
            UpdateCameraBlenderLike(&camera);
        }

        ClearBackground(0.1f, 0.1f, 0.1f, 1);

        BeginCamera(camera);

        PushGraphicState();

        DrawGrid(GridAxis::XY, 1.0f, 10, { 128, 128, 128 });
        DrawXYZAxis(1.0f);

        LinearTransform i2c(0, N-1, 0, 255);
        float deltaT = glm::pi<float>() * 2.0f / N;
        if (optimized) {
            CircleGenerator circle(deltaT, theta_s);
            for (int i = 0; i < N; ++i) {
                float x = circle.cos();
                float y = circle.sin();
                DrawSphere({ x, y, 0 }, 0.01f, { 255 - i2c(i), 128, i2c(i) });

                circle.step();
            }
        }
        else {
            float t = theta_s;
            for (int i = 0; i < N; ++i) {
                float x = std::cos(t);
                float y = std::sin(t);
                DrawSphere({ x, y, 0 }, 0.01f, { 255 - i2c(i), 128, i2c(i) });
                t += deltaT;
            }
        }

        PopGraphicState();
        EndCamera();

        BeginImGui();

        ImGui::SetNextWindowSize({ 500, 800 }, ImGuiCond_Once);
        ImGui::Begin("Panel");
        ImGui::Text("fps = %f", GetFrameRate());
        ImGui::Checkbox("optimized", &optimized);
        ImGui::SliderInt("N", &N, 3, 256);
        ImGui::SliderFloat("start theta", &theta_s, 0, glm::pi<float>() * 2.0f);
        
        ImGui::End();

        EndImGui();
    }

    CleanUp();
}
