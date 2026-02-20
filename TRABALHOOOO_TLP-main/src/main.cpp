#include "Simulador.h"

int main() {
    InitWindow(1280, 720, "Virus BioWarfare");
    SetTargetFPS(60);
    Simulador* jogo = new Simulador();
    while(!WindowShouldClose()) {
        jogo->Update();
        jogo->Draw();
    }
    delete jogo;
    CloseWindow();
    return 0;
}