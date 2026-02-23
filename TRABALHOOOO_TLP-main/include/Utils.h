#ifndef UTILS_H
#define UTILS_H

#include "raylib.h"

// ============================================================================
// ESTADOS DO FLUXO DO JOGO
// Define as diferentes telas/fases que o jogo pode estar
// ============================================================================
enum class GameState { 
    MENU,           // Ecrã principal com opções
    MAPA,           // Seleção de órgãos para infectar
    PLAYING,        // Gameplay principal
    PAUSE,          // Menu de pausa
    INSTRUCTIONS,   // Tela de instruções
    GAMEOVER,       // Derrota do jogador
    VICTORY,        // Vitória (não implementado, usa CUTSCENE)
    CUTSCENE        // Cinemática final de vitória
};

// ============================================================================
// TIPOS DE ENTIDADES NO SIMULADOR
// Cada organismo pertence a uma destas categorias
// ============================================================================
enum class TipoSer { 
    NEUTRO,         // Células azuis - podem ser infectadas
    INFETADO,       // Vírus verdes - controlados pelo jogador
    ANTICORPO,      // Células brancas - inimigos que atacam o vírus
    JOGADOR         // O vírus principal controlado pelo utilizador
};

// ============================================================================
// ESTRUTURA DE ATRIBUTOS GENÉTICOS (DNA)
// Define as características físicas de cada organismo
// ============================================================================
struct Gene {
    float velocidade;   // Velocidade máxima de movimento (pixels/segundo)
    float forca;        // Força de ataque/colisão (não usado diretamente)
    Color cor;          // Cor visual do organismo no ecrã
};

// ============================================================================
// CONFIGURAÇÕES DE ÁUDIO (novas)
// Paths para ficheiros de som e música
// ============================================================================
#define SOM_INFETAR     "resources/sounds/infect.wav"      // Som ao infectar célula
#define SOM_DANO        "resources/sounds/hurt.wav"        // Som ao receber dano
#define SOM_ANTICORPO_MORTE "resources/sounds/kill.wav"    // Som ao destruir anticorpo
#define SOM_NITRO_ON    "resources/sounds/nitro_on.wav"    // Ativar nitro
#define SOM_NITRO_OFF   "resources/sounds/nitro_off.wav"   // Desativar nitro
#define SOM_MENU_SELECT "resources/sounds/select.wav"      // Navegar menu
#define SOM_MENU_CONFIRM "resources/sounds/confirm.wav"    // Confirmar seleção
#define MUSICA_MENU     "resources/music/menu.ogg"         // Música do menu
#define MUSICA_GAMEPLAY "resources/music/gameplay.ogg"     // Música durante o jogo
#define MUSICA_BOSS     "resources/music/boss.ogg"         // Música de tensão

#endif // UTILS_H