// ============================================================================
// ORGANISMO.CPP - IMPLEMENTAÇÃO DA CLASSE ORGANISMO
// Contém: comportamento de IA, movimento, colisões, renderização e sons
// ============================================================================

#include "Organismo.h"
#include "raymath.h"
#include <vector>
#include <cmath>

// Cooldown global para dano do jogador (evita dano por frame)
float cooldownDano = 0.3f;

// ============================================================================
// CONSTRUTOR: INICIALIZA ATRIBUTOS DO ORGANISMO
// ============================================================================
Organismo::Organismo(Vector2 p, Gene g, TipoSer t) 
    : pos(p), dna(g), tipo(t), energia(100.0f), kills(0) {
    
    // Define tamanho base conforme tipo: jogador é maior, anticorpo médio
    tamanhoBase = 10.0f;
    if (tipo == TipoSer::JOGADOR) tamanhoBase = 18.0f;
    if (tipo == TipoSer::ANTICORPO) tamanhoBase = 13.0f;
    
    // Velocidade inicial aleatória para movimento natural
    vel = {(float)GetRandomValue(-100, 100), (float)GetRandomValue(-100, 100)};
}

// ============================================================================
// GETRAIO: CALCULA RAIO ATUAL COM BÓNUS DE KILLS (ANTICORPOS)
// ============================================================================
float Organismo::GetRaio() {
    // Anticorpos crescem visualmente conforme abatem vírus
    float extraSize = (tipo == TipoSer::ANTICORPO) ? (kills * 0.3f) : 0.0f;
    return tamanhoBase + extraSize;
}

// ============================================================================
// UPDATE: LÓGICA DE MOVIMENTO E COMPORTAMENTO POR TIPO
// ============================================================================
void Organismo::Update(float dt, Vector2 playerPos, std::vector<Organismo*>& todos) {
    // --- JOGADOR: apenas regenera energia, movimento é controlado por input ---
    if (tipo == TipoSer::JOGADOR) {
        if (energia < 100.0f) energia += 3.0f * dt;  // Regeneração lenta
        return;
    }

    float distAoPlayer = Vector2Distance(pos, playerPos);

    // --- NEUTRO (AZUL): Movimento aleatório com "wandering" ---
    if (tipo == TipoSer::NEUTRO) {
        // Pequena chance de mudar direção aleatoriamente
        if (GetRandomValue(0, 100) < 4) {
            float angle = atan2f(vel.y, vel.x);
            angle += (GetRandomValue(-60, 60) * DEG2RAD);  // Variação ±60 graus
            float speed = dna.velocidade * (0.8f + GetRandomValue(0, 40) / 100.0f);
            vel = {cosf(angle) * speed, sinf(angle) * speed};
        }
        
        // "Jitter" aleatório para movimento orgânico
        vel.x += (float)GetRandomValue(-15, 15) * dt;
        vel.y += (float)GetRandomValue(-15, 15) * dt;
        
        // Limita velocidade máxima definida no DNA
        float currentSpeed = Vector2Length(vel);
        if (currentSpeed > dna.velocidade) {
            vel = Vector2Scale(Vector2Normalize(vel), dna.velocidade);
        }
    } 
    
    // --- ANTICORPO (BRANCO): Persegue jogador + evita aliados (flocking) ---
    else if (tipo == TipoSer::ANTICORPO) {
        // Direção principal: em direção ao jogador
        Vector2 direcao = Vector2Normalize(Vector2Subtract(playerPos, pos));
        
        // Separação: evita aglomeração com outros anticorpos (comportamento de bando)
        Vector2 separacao = {0, 0};
        int vizinhos = 0;
        
        for(auto& outro : todos) {
            if(outro->tipo == TipoSer::ANTICORPO && outro != this) {
                float dist = Vector2Distance(pos, outro->pos);
                if(dist < 60.0f && dist > 0) {
                    // Empurra para longe de vizinhos muito próximos
                    Vector2 afastamento = Vector2Normalize(Vector2Subtract(pos, outro->pos));
                    separacao = Vector2Add(separacao, Vector2Scale(afastamento, 1.0f / dist));
                    vizinhos++;
                }
            }
        }
        
        // Combina perseguição ao jogador + separação de aliados
        if(vizinhos > 0) {
            separacao = Vector2Scale(separacao, 1.0f / vizinhos);
            separacao = Vector2Normalize(separacao);
            // 60% persegue jogador, 40% evita aliados
            direcao = Vector2Add(Vector2Scale(direcao, 0.6f), Vector2Scale(separacao, 0.4f));
            direcao = Vector2Normalize(direcao);
        }
        
        // Suaviza mudança de direção (lerp para movimento fluido)
        vel = Vector2Lerp(vel, Vector2Scale(direcao, dna.velocidade * 1.5f), dt * 3.0f);
    } 
    
    // --- INFETADO (VERDE): Prioriza atacar anticorpos, senão segue jogador ---
    else if (tipo == TipoSer::INFETADO) {
        Organismo* alvo = nullptr;
        float dMin = 250.0f;  // Raio de deteção de anticorpos
        
        // Procura o anticorpo mais próximo para atacar
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
            // Ataca anticorpo: movimento agressivo e rápido
            Vector2 ataque = Vector2Normalize(Vector2Subtract(alvo->pos, pos));
            vel = Vector2Lerp(vel, Vector2Scale(ataque, dna.velocidade * 1.5f), dt * 4.0f);
        } else if (distAoPlayer > 80.0f) {
            // Segue jogador se não houver anticorpos por perto
            Vector2 segue = Vector2Normalize(Vector2Subtract(playerPos, pos));
            vel = Vector2Lerp(vel, Vector2Scale(segue, dna.velocidade), dt * 5.0f);
        }
    }

    // --- APLICA MOVIMENTO E LIMITES DO ECRÃ ---
    pos = Vector2Add(pos, Vector2Scale(vel, dt));
    
    // Colisão com bordas: rebate com amortecimento (0.8 = perde 20% da velocidade)
    if (pos.x < 25) { pos.x = 25; vel.x *= -0.8f; }
    if (pos.x > 1255) { pos.x = 1255; vel.x *= -0.8f; }
    if (pos.y < 25) { pos.y = 25; vel.y *= -0.8f; }
    if (pos.y > 695) { pos.y = 695; vel.y *= -0.8f; }
}

// ============================================================================
// RESOLVERCOLISOES: DETETA E PROCESSA INTERAÇÕES ENTRE ENTIDADES
// ============================================================================
void ResolverColisoes(std::vector<Organismo*>& todos, bool imunidadeAtiva) {
    // Verifica todos os pares de organismos (O(n²) - otimizar para muitos entities)
    for(size_t i = 0; i < todos.size(); i++) {
        for(size_t j = i + 1; j < todos.size(); j++) {
            Organismo* a = todos[i];
            Organismo* b = todos[j];
            
            // Ignora organismos mortos
            if (a->energia <= 0 || b->energia <= 0) continue;
            
            // Calcula distância entre centros
            Vector2 diff = Vector2Subtract(b->pos, a->pos);
            float dist = Vector2Length(diff);
            
            float raioA = a->GetRaio();
            float raioB = b->GetRaio();
            float minDist = raioA + raioB;
            
            // Deteta colisão: distância menor que soma dos raios
            if (dist < minDist && dist > 0) {
                Vector2 normal = Vector2Scale(diff, 1.0f / dist);  // Direção da colisão
                float overlap = minDist - dist;  // Quanto estão sobrepostos
                
                // --- REGRAS DE COLISÃO POR TIPO ---
                
                // Anticorpo + Neutro: sem interação (não se atacam)
                if ((a->tipo == TipoSer::ANTICORPO && b->tipo == TipoSer::NEUTRO) ||
                    (b->tipo == TipoSer::ANTICORPO && a->tipo == TipoSer::NEUTRO)) {
                    continue;
                }
                
                // Anticorpo + Infetado: combate mortal!
                if ((a->tipo == TipoSer::ANTICORPO && b->tipo == TipoSer::INFETADO) ||
                    (b->tipo == TipoSer::ANTICORPO && a->tipo == TipoSer::INFETADO)) {
                    
                    Organismo* branco = (a->tipo == TipoSer::ANTICORPO) ? a : b;
                    Organismo* verde = (a->tipo == TipoSer::INFETADO) ? a : b;
                    
                    // Infetado morre instantaneamente
                    verde->energia = 0;
                    
                    // Anticorpo perde energia ao atacar (balanceamento)
                    branco->energia -= 12.0f;
                    
                    // Se sobreviver, ganha um "kill" e cresce visualmente
                    if (branco->energia > 0) {
                        branco->kills++;
                        // 🎵 TOCAR SOM: anticorpo mata vírus
                        // Nota: som tocado no Simulador para acesso ao sistema de áudio
                    }
                    
                    // Empurra anticorpo para trás (feedback visual de impacto)
                    branco->pos = Vector2Add(branco->pos, Vector2Scale(normal, 5.0f));
                }
                
                // Jogador + Anticorpo: jogador sofre dano (se não imune)
                if ((a->tipo == TipoSer::JOGADOR && b->tipo == TipoSer::ANTICORPO) ||
                    (b->tipo == TipoSer::JOGADOR && a->tipo == TipoSer::ANTICORPO)) {
                    
                    Organismo* player = (a->tipo == TipoSer::JOGADOR) ? a : b;
                    Organismo* branco = (a->tipo == TipoSer::ANTICORPO) ? a : b;
                    
                    if (!imunidadeAtiva) {
                        float danoPorHit = 2.0f;
                        player->energia -= danoPorHit;
                        // 🎵 TOCAR SOM: jogador recebe dano
                    }
                    
                    // Separa entidades para evitar "grudar"
                    player->pos = Vector2Subtract(player->pos, Vector2Scale(normal, overlap * 0.5f));
                    branco->pos = Vector2Add(branco->pos, Vector2Scale(normal, overlap * 0.5f * 0.5f));
                    
                    // Empurra anticorpo para longe com força
                    branco->vel = Vector2Scale(normal, -200.0f);
                }
                // Jogador + Outros: empurrão suave
                else if (a->tipo == TipoSer::JOGADOR || b->tipo == TipoSer::JOGADOR) {
                    if (a->tipo == TipoSer::JOGADOR) {
                        b->pos = Vector2Add(b->pos, Vector2Scale(normal, overlap * 0.3f));
                    } else {
                        a->pos = Vector2Add(a->pos, Vector2Scale(normal, overlap * 0.3f));
                    }
                }
                // Anticorpo + Outros: separação assimétrica
                else if (a->tipo == TipoSer::ANTICORPO || b->tipo == TipoSer::ANTICORPO) {
                    if (a->tipo == TipoSer::ANTICORPO) {
                        a->pos = Vector2Subtract(a->pos, Vector2Scale(normal, overlap * 0.6f));
                        b->pos = Vector2Add(b->pos, Vector2Scale(normal, overlap * 0.4f));
                    } else {
                        b->pos = Vector2Subtract(b->pos, Vector2Scale(normal, overlap * 0.6f));
                        a->pos = Vector2Add(a->pos, Vector2Scale(normal, overlap * 0.4f));
                    }
                }
                // Outros casos: separação simétrica
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

// ============================================================================
// DRAW: RENDERIZAÇÃO VISUAL COM EFEITOS E ANIMAÇÕES
// ============================================================================
void Organismo::Draw() {
    float animacao = GetTime() * 2.0f;  // Tempo para efeitos pulsantes
    float raioAtual = GetRaio();
    
    // --- JOGADOR (VERDE GRANDE): Efeitos de glow, spikes, olhos ---
    if (tipo == TipoSer::JOGADOR) {
        // Glow pulsante ao redor
        float glow = 5.0f + sinf(animacao) * 2.0f;
        DrawCircleV(pos, raioAtual + glow, LIME);
        DrawCircleV(pos, raioAtual, GREEN);
        DrawCircleV(pos, raioAtual - 4, DARKGREEN);
        DrawCircleLines((int)pos.x, (int)pos.y, raioAtual + 6, WHITE);
        
        // Spikes rotativos ao redor 
        for(int i = 0; i < 10; i++) {
            float angle = i * PI / 5 + animacao * 0.5f;
            Vector2 spike = {pos.x + cosf(angle) * (raioAtual + 8), pos.y + sinf(angle) * (raioAtual + 8)};
            DrawCircleV(spike, 4, DARKGREEN);
        }
        
        // Olhos expressivos
        DrawCircleV({pos.x - 5, pos.y - 4}, 4, WHITE);
        DrawCircleV({pos.x + 5, pos.y - 4}, 4, WHITE);
        DrawCircleV({pos.x - 5, pos.y - 4}, 2, BLACK);
        DrawCircleV({pos.x + 5, pos.y - 4}, 2, BLACK);
    }
    // --- INFETADO (VERDE PEQUENO): spikes menores, barra de "raiva" ---
    else if (tipo == TipoSer::INFETADO) {
        DrawCircleV(pos, raioAtual, LIME);
        DrawCircleV(pos, raioAtual - 3, GREEN);
        for(int i = 0; i < 8; i++) {
            float angle = i * PI / 4 + animacao * 0.3f;
            Vector2 spike = {pos.x + cosf(angle) * (raioAtual + 4), pos.y + sinf(angle) * (raioAtual + 4)};
            DrawCircleV(spike, 3, DARKGREEN);
        }
        // Olhos + barra vermelha indicando agressividade
        DrawCircleV({pos.x - 3, pos.y - 2}, 2.5f, WHITE);
        DrawCircleV({pos.x + 3, pos.y - 2}, 2.5f, WHITE);
        DrawRectangle((int)(pos.x - 4), (int)(pos.y + 3), 8, 3, RED);
    }
    // --- NEUTRO (AZUL): aparência amigável, olhos inocentes ---
    else if (tipo == TipoSer::NEUTRO) {
        DrawCircleV(pos, raioAtual, SKYBLUE);
        DrawCircleV(pos, raioAtual - 3, BLUE);
        DrawCircleV({pos.x - 3, pos.y - 2}, 2, WHITE);
        DrawCircleV({pos.x + 3, pos.y - 2}, 2, WHITE);
        DrawCircleV({pos.x - 3, pos.y - 2}, 1, BLACK);
        DrawCircleV({pos.x + 3, pos.y - 2}, 1, BLACK);
    }
    // --- ANTICORPO (BRANCO): aparência "médica", contador de kills ---
    else if (tipo == TipoSer::ANTICORPO) {
        float pulse = sinf(animacao * 3) * 2;  // Pulsação rápida
        
        // Glow branco + corpo cinza claro
        DrawCircleV(pos, raioAtual + pulse*0.2f, WHITE);
        DrawCircleV(pos, raioAtual - 3, Color{220, 220, 220, 255});
        
        // "Braços" laterais que simulam anticorpos reais
        DrawCircleV({pos.x - raioAtual - 2, pos.y - 4}, 3, WHITE);
        DrawCircleV({pos.x + raioAtual + 2, pos.y - 4}, 3, WHITE);
        DrawRectangle((int)(pos.x - 5), (int)(pos.y - 4), 4, 3, BLACK);
        DrawRectangle((int)(pos.x + 1), (int)(pos.y - 4), 4, 3, BLACK);
        
        // Mostra contador de kills acima da cabeça
        if (kills > 0) {
            DrawText(TextFormat("%d", kills), (int)(pos.x - 5), (int)(pos.y - raioAtual - 15), 10, RED);
        }
        
        // Barra de vida vermelha quando ferido
        if (energia < 100.0f) {
            DrawRectangle((int)(pos.x - 12), (int)(pos.y - raioAtual - 10), 24, 3, DARKGRAY);
            DrawRectangle((int)(pos.x - 12), (int)(pos.y - raioAtual - 10), 24 * (energia/100.0f), 3, RED);
        }
    }
}