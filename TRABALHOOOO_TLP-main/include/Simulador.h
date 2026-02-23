#ifndef SIMULADOR_H
#define SIMULADOR_H

#include "Organismo.h"
#include <vector>

// ============================================================================
// CLASSE SIMULADOR
// Gerencia o estado global do jogo, entidades, áudio e progresso
// ============================================================================
class Simulador {
public:
    // --- GERENCIAMENTO DE ENTIDADES ---
    std::vector<Organismo*> populacao;  // Lista dinâmica de todos os organismos
    
    // --- ESTADO DO JOGO ---
    GameState estadoAtual;              // Estado corrente (MENU, PLAYING, etc)
    int pontosDNA;                      // Moeda do jogo para upgrades (futuro)
    int nivelSalvo;                     // Nível guardado no save file
    float timerMorte;                   // Temporizador para cinemáticas
    int opcaoPausa;                     // Índice da opção selecionada no pause
    bool jogoComecou;                   // Flag para evitar reset acidental
    
    // --- SISTEMA DE NITRO (boost de velocidade) ---
    float nitroTimer;                   // Tempo até poder usar nitro novamente (cooldown)
    float nitroAtivoTimer;              // Duração restante do boost ativo
    bool nitroAtivo;                    // Estado atual do nitro (ligado/desligado)
    float velocidadeNormal;             // Velocidade base do jogador por nível
    
    // --- SISTEMA DE IMUNIDADE (invencibilidade inicial) ---
    float imunidadeTimer;               // Tempo restante de imunidade (5s iniciais)
    bool imunidadeAtiva;                // Se true, jogador não sofre dano
    
    // --- SISTEMA DE ÁUDIO (NOVO) ---
    Sound somInfetar;                   // Som: infectar célula neutra
    Sound somDano;                      // Som: jogador recebe dano
    Sound somAnticorpoMorte;            // Som: anticorpo destruído
    Sound somNitroOn;                   // Som: ativar nitro
    Sound somNitroOff;                  // Som: desativar nitro
    Sound somMenuSelect;                // Som: navegar opções do menu
    Sound somMenuConfirm;               // Som: confirmar seleção
    Music musicaMenu;                   // Música de fundo: menu principal
    Music musicaGameplay;               // Música de fundo: gameplay
    bool audioInicializado;             // Flag para evitar inicialização duplicada

    // --- MÉTODOS DO CICLO DE VIDA ---
    Simulador();                        // Construtor: inicializa áudio, entidades, estado
    ~Simulador();                       // Destrutor: liberta memória e recursos de áudio
    
    // --- MÉTODOS PRINCIPAIS DO LOOP ---
    void Update();                      // Atualiza lógica: input, IA, colisões, estado
    void Draw();                        // Renderiza tudo: entidades, HUD, menus, efeitos
    
    // --- GERENCIAMENTO DE PROGRESSO ---
    void ResetJogo(int nivel);          // Reinicia o jogo para um nível específico
    void SaveGame();                    // Guarda estado atual em ficheiro binário
    void LoadGame();                    // Carrega estado guardado do ficheiro
    
    // --- MÉTODOS AUXILIARES DE RENDERIZAÇÃO ---
    void DesenharBossMorte();           // Efeito visual de morte do boss (coração)
    void DrawOrgaoAnimado(int nivel, int x, int y, bool selecionado, float tempo);
    void DrawMenuAnimado(float tempo);  // Efeitos animados do menu principal
    
    // --- MÉTODOS DE ÁUDIO (NOVOS) ---
    void TocarSom(Sound som, float volume = 1.0f);  // Toca som com volume opcional
    void TocarMusica(Music& musica);    // Inicia música com fade-in suave
    void PararMusica(Music& musica);    // Para música com fade-out
    void AtualizarAudio();              // Atualiza estado do áudio (músicas em loop)
};

// Declaração da função global de colisões (implementada em organismo.cpp)
void ResolverColisoes(std::vector<Organismo*>& todos, bool imunidadeAtiva);

#endif // SIMULADOR_H