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
    bool jogoComecou;
    
    // Sistema de Nitro
    float nitroTimer;
    float nitroAtivoTimer;
    bool nitroAtivo;
    float velocidadeNormal;
    
    // Sistema de Imunidade
    float imunidadeTimer;
    bool imunidadeAtiva;
    
    Simulador();
    ~Simulador();
    void Update();
    void Draw();
    void ResetJogo(int nivel);
    void SaveGame();
    void LoadGame();
    void DesenharBossMorte();
    void DrawOrgaoAnimado(int nivel, int x, int y, bool selecionado, float tempo);
    void DrawMenuAnimado(float tempo);
};

void ResolverColisoes(std::vector<Organismo*>& todos, bool imunidadeAtiva);

#endif