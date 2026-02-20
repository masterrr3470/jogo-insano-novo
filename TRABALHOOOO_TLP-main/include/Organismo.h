#ifndef ORGANISMO_H
#define ORGANISMO_H
#include "Utils.h"
#include <vector>

class Organismo {
public:
    Vector2 pos;
    Vector2 vel;
    float energia;
    Gene dna;
    TipoSer tipo;
    int kills;
    float tamanhoBase;
    
    Organismo(Vector2 p, Gene g, TipoSer t);
    void Update(float dt, Vector2 playerPos, std::vector<Organismo*>& todos);
    void Draw();
    float GetRaio();
};

void ResolverColisoes(std::vector<Organismo*>& todos);

#endif