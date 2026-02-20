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
    bool jogoComecou;  // ✅ NOVO: Flag para evitar skip de níveis
    
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

// Função global de colisões
void ResolverColisoes(std::vector<Organismo*>& todos);

#endif