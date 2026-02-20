#ifndef UTILS_H
#define UTILS_H
#include "raylib.h"

enum class GameState { MENU, MAPA, PLAYING, PAUSE, INSTRUCTIONS, GAMEOVER, VICTORY, CUTSCENE };
enum class TipoSer { NEUTRO, INFETADO, ANTICORPO, JOGADOR };

struct Gene {
    float velocidade;
    float forca;
    Color cor;
};

struct Particula {
    Vector2 pos;
    Vector2 vel;
    float vida;
    Color cor;
    float tamanho;
};
#endif