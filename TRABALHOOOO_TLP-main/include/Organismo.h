#ifndef ORGANISMO_H
#define ORGANISMO_H

#include "Utils.h"
#include <vector>

// ============================================================================
// CLASSE ORGANISMO
// Representa qualquer entidade viva no simulador (vírus, célula, anticorpo)
// ============================================================================
class Organismo {
public:
    // --- ATRIBUTOS PÚBLICOS ---
    Vector2 pos;              // Posição atual no mundo 2D (centro do círculo)
    Vector2 vel;              // Vetor velocidade: direção e magnitude do movimento
    float energia;            // "Vida" do organismo; 0 = morto/removido
    Gene dna;                 // Atributos genéticos que definem comportamento
    TipoSer tipo;             // Categoria: JOGADOR, INFETADO, ANTICORPO ou NEUTRO
    int kills;                // Contador de abates (anticorpos crescem ao matar)
    float tamanhoBase;        // Raio base do organismo em pixels

    // --- CONSTRUTOR ---
    // Inicializa posição, DNA e tipo; define tamanho conforme categoria
    Organismo(Vector2 p, Gene g, TipoSer t);

    // --- MÉTODOS PRINCIPAIS ---
    
    // Atualiza lógica do organismo: IA, movimento, limites do ecrã
    // @param dt: delta time em segundos para movimento frame-independent
    // @param playerPos: posição do jogador para IA de perseguição/fuga
    // @param todos: lista de todos os organismos para deteção de vizinhos
    void Update(float dt, Vector2 playerPos, std::vector<Organismo*>& todos);
    
    // Renderiza o organismo com efeitos visuais: glow, spikes, olhos, barras de vida
    void Draw();
    
    // Calcula o raio atual considerando bónus por kills (anticorpos)
    float GetRaio();
};

// ============================================================================
// FUNÇÃO GLOBAL: RESOLVER COLISÕES
// Deteta e processa interações físicas entre todos os organismos
// @param todos: vetor com ponteiros para todos os organismos ativos
// @param imunidadeAtiva: se true, jogador não sofre dano de anticorpos
// ============================================================================
void ResolverColisoes(std::vector<Organismo*>& todos, bool imunidadeAtiva);

#endif // ORGANISMO_H