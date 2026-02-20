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
#endif