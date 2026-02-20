#include "Organismo.h"
#include "raymath.h"
#include <vector>
#include <cmath>

Organismo::Organismo(Vector2 p, Gene g, TipoSer t) 
    : pos(p), dna(g), tipo(t), energia(100.0f) {
    vel = {(float)GetRandomValue(-100, 100), (float)GetRandomValue(-100, 100)};
}

void Organismo::Update(float dt, Vector2 playerPos, std::vector<Organismo*>& todos) {
    if (tipo == TipoSer::JOGADOR) {
        // Jogador regenera energia lentamente
        if (energia < 100.0f) energia += 3.0f * dt;
        return;
    }

    float distAoPlayer = Vector2Distance(pos, playerPos);

    if (tipo == TipoSer::NEUTRO) {
        // AZUIS - Movimento mais natural e fluido
        if (distAoPlayer < 180.0f) {
            // Foge do jogador de forma mais inteligente
            Vector2 fuga = Vector2Normalize(Vector2Subtract(pos, playerPos));
            vel = Vector2Lerp(vel, Vector2Scale(fuga, dna.velocidade * 1.8f), dt * 4.0f);
        } else {
            // Movimento aleatório mais suave e orgânico
            if (GetRandomValue(0, 100) < 3) {
                // Muda direção gradualmente
                float angle = atan2f(vel.y, vel.x);
                angle += (GetRandomValue(-50, 50) * DEG2RAD);
                float speed = Vector2Length(vel);
                if (speed < 0.1f) speed = dna.velocidade;
                vel = {cosf(angle) * speed, sinf(angle) * speed};
            }
            // Adiciona pequena variação contínua
            vel.x += (float)GetRandomValue(-10, 10) * dt;
            vel.y += (float)GetRandomValue(-10, 10) * dt;
            
            // Normaliza velocidade máxima
            float currentSpeed = Vector2Length(vel);
            if (currentSpeed > dna.velocidade) {
                vel = Vector2Scale(Vector2Normalize(vel), dna.velocidade);
            }
        }
        
        // Suaviza movimento
        vel = Vector2Lerp(vel, Vector2Scale(Vector2Normalize(vel), dna.velocidade), dt * 2.0f);
    } 
    else if (tipo == TipoSer::ANTICORPO) {
        // ROXOS - Perseguem o jogador e VERDES
        Organismo* alvoVerde = nullptr;
        float dMinVerde = 200.0f;
        
        // Procura verdes próximos para atacar
        for(auto& outro : todos) {
            if(outro->tipo == TipoSer::INFETADO) {
                float d = Vector2Distance(pos, outro->pos);
                if(d < dMinVerde) { 
                    dMinVerde = d; 
                    alvoVerde = outro; 
                }
            }
        }
        
        Vector2 target;
        if(alvoVerde && dMinVerde < 150.0f) {
            // Ataca verde se estiver perto
            target = alvoVerde->pos;
        } else {
            // Senão persegue jogador
            target = playerPos;
        }
        
        Vector2 direcao = Vector2Normalize(Vector2Subtract(target, pos));
        vel = Vector2Lerp(vel, Vector2Scale(direcao, dna.velocidade * 1.4f), dt * 3.0f);
    } 
    else if (tipo == TipoSer::INFETADO) {
        // VERDES - Atacam os ROXOS mas também podem morrer
        Organismo* alvo = nullptr;
        float dMin = 250.0f;
        
        for(auto& outro : todos) {
            if(outro->tipo == TipoSer::ANTICORPO) {
                float d = Vector2Distance(pos, outro->pos);
                if(d < dMin) { 
                    dMin = d; 
                    alvo = outro; 
                }
            }
        }
        
        if(alvo) {
            // Perseguir e atacar o roxo
            Vector2 ataque = Vector2Normalize(Vector2Subtract(alvo->pos, pos));
            vel = Vector2Lerp(vel, Vector2Scale(ataque, dna.velocidade * 1.6f), dt * 4.0f);
            
            // COMBATE BILATERAL: Verde causa dano no roxo
            if(dMin < 22.0f) {
                alvo->energia -= 20.0f * dt;
                // Roxo também causa dano no verde!
                this->energia -= 15.0f * dt;
            }
        } else if (distAoPlayer > 80.0f) {
            // Segue o jogador se não houver roxos
            Vector2 segue = Vector2Normalize(Vector2Subtract(playerPos, pos));
            vel = Vector2Lerp(vel, Vector2Scale(segue, dna.velocidade), dt * 5.0f);
        }
    }

    // Atualizar posição
    pos = Vector2Add(pos, Vector2Scale(vel, dt));
    
    // Manter na tela com margem
    if (pos.x < 25) { pos.x = 25; vel.x *= -0.8f; }
    if (pos.x > 1255) { pos.x = 1255; vel.x *= -0.8f; }
    if (pos.y < 25) { pos.y = 25; vel.y *= -0.8f; }
    if (pos.y > 695) { pos.y = 695; vel.y *= -0.8f; }
}

void Organismo::Draw() {
    float size = 10.0f;
    
    if (tipo == TipoSer::JOGADOR) {
        // JOGADOR - Maior e mais rápido (desenho especial)
        size = 18.0f; // MAIOR
        DrawCircleV(pos, size + 3, LIME);
        DrawCircleV(pos, size, GREEN);
        DrawCircleV(pos, size - 4, DARKGREEN);
        DrawCircleLines((int)pos.x, (int)pos.y, size + 6, WHITE);
        
        // Espinhos mais pronunciados
        for(int i = 0; i < 10; i++) {
            float angle = i * PI / 5;
            Vector2 spike = {pos.x + cosf(angle) * (size + 8), pos.y + sinf(angle) * (size + 8)};
            DrawCircleV(spike, 4, DARKGREEN);
        }
        
        // Olhos
        DrawCircleV({pos.x - 5, pos.y - 4}, 4, WHITE);
        DrawCircleV({pos.x + 5, pos.y - 4}, 4, WHITE);
        DrawCircleV({pos.x - 5, pos.y - 4}, 2, BLACK);
        DrawCircleV({pos.x + 5, pos.y - 4}, 2, BLACK);
    }
    else if (tipo == TipoSer::INFETADO) {
        // VERDES - Design igual ao jogador mas menor
        DrawCircleV(pos, size, LIME);
        DrawCircleV(pos, size - 3, GREEN);
        // Espinhos
        for(int i = 0; i < 8; i++) {
            float angle = i * PI / 4;
            Vector2 spike = {pos.x + cosf(angle) * (size + 4), pos.y + sinf(angle) * (size + 4)};
            DrawCircleV(spike, 3, DARKGREEN);
        }
        // Olhos
        DrawCircleV({pos.x - 3, pos.y - 2}, 2.5f, WHITE);
        DrawCircleV({pos.x + 3, pos.y - 2}, 2.5f, WHITE);
        DrawRectangle((int)(pos.x - 4), (int)(pos.y + 3), 8, 3, RED);
    }
    else if (tipo == TipoSer::NEUTRO) {
        // AZUIS - MESMO DESIGN DOS VERDES (antes de serem infectados)
        DrawCircleV(pos, size, SKYBLUE);
        DrawCircleV(pos, size - 3, BLUE);
        // Espinhos
        for(int i = 0; i < 8; i++) {
            float angle = i * PI / 4;
            Vector2 spike = {pos.x + cosf(angle) * (size + 4), pos.y + sinf(angle) * (size + 4)};
            DrawCircleV(spike, 3, DARKBLUE);
        }
        // Olhos neutros
        DrawCircleV({pos.x - 3, pos.y - 2}, 2, WHITE);
        DrawCircleV({pos.x + 3, pos.y - 2}, 2, WHITE);
        DrawCircleV({pos.x - 3, pos.y - 2}, 1, BLACK);
        DrawCircleV({pos.x + 3, pos.y - 2}, 1, BLACK);
    }
    else if (tipo == TipoSer::ANTICORPO) {
        // ROXOS - Sistema imunológico
        DrawCircleV(pos, 11, Color{220, 80, 220, 255});
        DrawCircleV(pos, 8, Color{180, 50, 180, 255});
        // "Braços" de ataque
        DrawCircleV({pos.x - 14, pos.y - 4}, 3, Color{220, 80, 220, 255});
        DrawCircleV({pos.x + 14, pos.y - 4}, 3, Color{220, 80, 220, 255});
        // Olhos determinados
        DrawRectangle((int)(pos.x - 5), (int)(pos.y - 4), 4, 3, WHITE);
        DrawRectangle((int)(pos.x + 1), (int)(pos.y - 4), 4, 3, WHITE);
        
        // Barra de vida
        if (energia < 100.0f) {
            DrawRectangle((int)(pos.x - 12), (int)(pos.y - 20), 24, 3, DARKGRAY);
            DrawRectangle((int)(pos.x - 12), (int)(pos.y - 20), (int)(24 * (energia/100.0f)), 3, RED);
        }
    }
}