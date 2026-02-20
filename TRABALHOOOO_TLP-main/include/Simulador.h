#ifndef SIMULADOR_H
#define SIMULADOR_H
#include "Organismo.h"
#include <vector>

class Simulador {
public:
    std::vector<Organismo*> populacao;
    GameState estadoAtual;
    int pontosDNA;
    int nivelSalvo;
    float timerMorte;
    int opcaoPausa;
    
    Simulador();
    ~Simulador();
    void Update();
    void Draw();
    void ResetJogo(int nivel);
    void SaveGame();
    void LoadGame();
    void DesenharBossMorte();
    void DrawOrgaoPixelado(int nivel, int x, int y, bool selecionado); // <-- ADICIONE ESTA LINHA
};
#endif