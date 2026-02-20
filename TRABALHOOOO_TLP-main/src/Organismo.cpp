#include "Organismo.h"
#include "raymath.h"
#include <vector>
#include <cmath>

Organismo::Organismo(Vector2 p, Gene g, TipoSer t) 
    : pos(p), dna(g), tipo(t), energia(100.0f), kills(0), jaColidiu(false) {
    tamanhoBase = 10.0f;
    if (tipo == TipoSer::JOGADOR) tamanhoBase = 18.0f;
    if (tipo == TipoSer::ANTICORPO) tamanhoBase = 13.0f;
    vel = {(float)GetRandomValue(-100, 100), (float)GetRandomValue(-100, 100)};
}

float Organismo::GetRaio() {
    float extraSize = (tipo == TipoSer::ANTICORPO) ? (kills * 0.3f) : 0.0f;
    return tamanhoBase + extraSize;
}

void Organismo::Update(float dt, Vector2 playerPos, std::vector<Organismo*>& todos) {
    jaColidiu = false;
    
    if (tipo == TipoSer::JOGADOR) {
        if (energia < 100.0f) energia += 3.0f * dt;
        return;
    }

    float distAoPlayer = Vector2Distance(pos, playerPos);

    if (tipo == TipoSer::NEUTRO) {
        if (GetRandomValue(0, 100) < 4) {
            float angle = atan2f(vel.y, vel.x);
            angle += (GetRandomValue(-60, 60) * DEG2RAD);
            float speed = dna.velocidade * (0.8f + GetRandomValue(0, 40) / 100.0f);
            vel = {cosf(angle) * speed, sinf(angle) * speed};
        }
        
        vel.x += (float)GetRandomValue(-15, 15) * dt;
        vel.y += (float)GetRandomValue(-15, 15) * dt;
        
        float currentSpeed = Vector2Length(vel);
        if (currentSpeed > dna.velocidade) {
            vel = Vector2Scale(Vector2Normalize(vel), dna.velocidade);
        }
    } 
    else if (tipo == TipoSer::ANTICORPO) {
        Organismo* alvoVerde = nullptr;
        float dMinVerde = 200.0f;
        
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
            target = alvoVerde->pos;
        } else {
            target = playerPos;
        }
        
        Vector2 direcao = Vector2Normalize(Vector2Subtract(target, pos));
        vel = Vector2Lerp(vel, Vector2Scale(direcao, dna.velocidade * 1.5f), dt * 3.0f);
    } 
    else if (tipo == TipoSer::INFETADO) {
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
            Vector2 ataque = Vector2Normalize(Vector2Subtract(alvo->pos, pos));
            vel = Vector2Lerp(vel, Vector2Scale(ataque, dna.velocidade * 1.5f), dt * 4.0f);
        } else if (distAoPlayer > 80.0f) {
            Vector2 segue = Vector2Normalize(Vector2Subtract(playerPos, pos));
            vel = Vector2Lerp(vel, Vector2Scale(segue, dna.velocidade), dt * 5.0f);
        }
    }

    pos = Vector2Add(pos, Vector2Scale(vel, dt));
    
    if (pos.x < 25) { pos.x = 25; vel.x *= -0.8f; }
    if (pos.x > 1255) { pos.x = 1255; vel.x *= -0.8f; }
    if (pos.y < 25) { pos.y = 25; vel.y *= -0.8f; }
    if (pos.y > 695) { pos.y = 695; vel.y *= -0.8f; }
}

void ResolverColisoes(std::vector<Organismo*>& todos) {
    for(size_t i = 0; i < todos.size(); i++) {
        for(size_t j = i + 1; j < todos.size(); j++) {
            Organismo* a = todos[i];
            Organismo* b = todos[j];
            
            if (a->energia <= 0 || b->energia <= 0) continue;
            
            Vector2 diff = Vector2Subtract(b->pos, a->pos);
            float dist = Vector2Length(diff);
            
            float raioA = a->GetRaio();
            float raioB = b->GetRaio();
            float minDist = raioA + raioB;
            
            if (dist < minDist && dist > 0) {
                Vector2 normal = Vector2Scale(diff, 1.0f / dist);
                float overlap = minDist - dist;
                
                // BRANCOS ATRAVESSAM AZUIS
                if ((a->tipo == TipoSer::ANTICORPO && b->tipo == TipoSer::NEUTRO) ||
                    (b->tipo == TipoSer::ANTICORPO && a->tipo == TipoSer::NEUTRO)) {
                    continue;
                }
                
                // COMBATE: BRANCO vs VERDE (1 HIT KILL)
                if ((a->tipo == TipoSer::ANTICORPO && b->tipo == TipoSer::INFETADO) ||
                    (b->tipo == TipoSer::ANTICORPO && a->tipo == TipoSer::INFETADO)) {
                    
                    Organismo* branco = (a->tipo == TipoSer::ANTICORPO) ? a : b;
                    Organismo* verde = (a->tipo == TipoSer::INFETADO) ? a : b;
                    
                    verde->energia = 0;
                    branco->energia -= 12.0f;
                    
                    if (branco->energia > 0) {
                        branco->kills++;
                    }
                    
                    branco->pos = Vector2Add(branco->pos, Vector2Scale(normal, 5.0f));
                }
                
                // Colisão Jogador vs Branco
                if ((a->tipo == TipoSer::JOGADOR && b->tipo == TipoSer::ANTICORPO) ||
                    (b->tipo == TipoSer::JOGADOR && a->tipo == TipoSer::ANTICORPO)) {
                    if (a->tipo == TipoSer::JOGADOR) {
                        a->pos = Vector2Subtract(a->pos, Vector2Scale(normal, overlap * 0.5f));
                        b->pos = Vector2Add(b->pos, Vector2Scale(normal, overlap * 0.5f * 0.5f));
                    } else {
                        b->pos = Vector2Subtract(b->pos, Vector2Scale(normal, overlap * 0.5f));
                        a->pos = Vector2Add(a->pos, Vector2Scale(normal, overlap * 0.5f * 0.5f));
                    }
                }
                // Colisão Jogador vs outros (passa através)
                else if (a->tipo == TipoSer::JOGADOR || b->tipo == TipoSer::JOGADOR) {
                    if (a->tipo == TipoSer::JOGADOR) {
                        b->pos = Vector2Add(b->pos, Vector2Scale(normal, overlap * 0.3f));
                    } else {
                        a->pos = Vector2Add(a->pos, Vector2Scale(normal, overlap * 0.3f));
                    }
                }
                // Colisão Brancos vs outros
                else if (a->tipo == TipoSer::ANTICORPO || b->tipo == TipoSer::ANTICORPO) {
                    if (a->tipo == TipoSer::ANTICORPO) {
                        a->pos = Vector2Subtract(a->pos, Vector2Scale(normal, overlap * 0.6f));
                        b->pos = Vector2Add(b->pos, Vector2Scale(normal, overlap * 0.4f));
                    } else {
                        b->pos = Vector2Subtract(b->pos, Vector2Scale(normal, overlap * 0.6f));
                        a->pos = Vector2Add(a->pos, Vector2Scale(normal, overlap * 0.4f));
                    }
                }
                // Outros casos
                else {
                    Vector2 pushA = Vector2Scale(normal, -overlap * 0.5f);
                    Vector2 pushB = Vector2Scale(normal, overlap * 0.5f);
                    a->pos = Vector2Add(a->pos, pushA);
                    b->pos = Vector2Add(b->pos, pushB);
                }
            }
        }
    }
}

void Organismo::Draw() {
    float animacao = GetTime() * 2.0f;
    float raioAtual = GetRaio();
    
    if (tipo == TipoSer::JOGADOR) {
        float glow = 5.0f + sinf(animacao) * 2.0f;
        DrawCircleV(pos, raioAtual + glow, LIME);
        DrawCircleV(pos, raioAtual, GREEN);
        DrawCircleV(pos, raioAtual - 4, DARKGREEN);
        DrawCircleLines((int)pos.x, (int)pos.y, raioAtual + 6, WHITE);
        
        for(int i = 0; i < 10; i++) {
            float angle = i * PI / 5 + animacao * 0.5f;
            Vector2 spike = {pos.x + cosf(angle) * (raioAtual + 8), pos.y + sinf(angle) * (raioAtual + 8)};
            DrawCircleV(spike, 4, DARKGREEN);
        }
        
        DrawCircleV({pos.x - 5, pos.y - 4}, 4, WHITE);
        DrawCircleV({pos.x + 5, pos.y - 4}, 4, WHITE);
        DrawCircleV({pos.x - 5, pos.y - 4}, 2, BLACK);
        DrawCircleV({pos.x + 5, pos.y - 4}, 2, BLACK);
    }
    else if (tipo == TipoSer::INFETADO) {
        DrawCircleV(pos, raioAtual, LIME);
        DrawCircleV(pos, raioAtual - 3, GREEN);
        for(int i = 0; i < 8; i++) {
            float angle = i * PI / 4 + animacao * 0.3f;
            Vector2 spike = {pos.x + cosf(angle) * (raioAtual + 4), pos.y + sinf(angle) * (raioAtual + 4)};
            DrawCircleV(spike, 3, DARKGREEN);
        }
        DrawCircleV({pos.x - 3, pos.y - 2}, 2.5f, WHITE);
        DrawCircleV({pos.x + 3, pos.y - 2}, 2.5f, WHITE);
        DrawRectangle((int)(pos.x - 4), (int)(pos.y + 3), 8, 3, RED);
    }
    else if (tipo == TipoSer::NEUTRO) {
        DrawCircleV(pos, raioAtual, SKYBLUE);
        DrawCircleV(pos, raioAtual - 3, BLUE);
        DrawCircleV({pos.x - 3, pos.y - 2}, 2, WHITE);
        DrawCircleV({pos.x + 3, pos.y - 2}, 2, WHITE);
        DrawCircleV({pos.x - 3, pos.y - 2}, 1, BLACK);
        DrawCircleV({pos.x + 3, pos.y - 2}, 1, BLACK);
    }
    else if (tipo == TipoSer::ANTICORPO) {
        float pulse = sinf(animacao * 3) * 2;
        
        DrawCircleV(pos, raioAtual + pulse*0.2f, WHITE);
        DrawCircleV(pos, raioAtual - 3, Color{220, 220, 220, 255});
        DrawCircleV({pos.x - raioAtual - 2, pos.y - 4}, 3, WHITE);
        DrawCircleV({pos.x + raioAtual + 2, pos.y - 4}, 3, WHITE);
        DrawRectangle((int)(pos.x - 5), (int)(pos.y - 4), 4, 3, BLACK);
        DrawRectangle((int)(pos.x + 1), (int)(pos.y - 4), 4, 3, BLACK);
        
        if (kills > 0) {
            DrawText(TextFormat("%d", kills), (int)(pos.x - 5), (int)(pos.y - raioAtual - 15), 10, RED);
        }
        
        if (energia < 100.0f) {
            DrawRectangle((int)(pos.x - 12), (int)(pos.y - raioAtual - 10), 24, 3, DARKGRAY);
            DrawRectangle((int)(pos.x - 12), (int)(pos.y - raioAtual - 10), 24 * (energia/100.0f), 3, RED);
        }
    }
}