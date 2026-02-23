// ============================================================================
// VIRUS: BIOWARFARE - MAIN.CPP
// Ponto de entrada do jogo: inicializa raylib, áudio e loop principal
// ============================================================================

#include "Simulador.h"
#include "raylib.h"

int main() {
    // --- INICIALIZAÇÃO DA JANELA ---
    // Resolução 1280x720 (HD), título do jogo
    InitWindow(1280, 720, "Virus BioWarfare");
    
    // Define target de 60 FPS para movimento suave e consistente
    SetTargetFPS(60);
    
    // --- INICIALIZAÇÃO DO SISTEMA DE ÁUDIO ---
    // Necessário antes de carregar sons ou músicas
    InitAudioDevice();
    SetMasterVolume(0.8f);  // Volume global a 80% para evitar distorção
    
    // --- CRIAÇÃO DO SIMULADOR ---
    // O construtor de Simulador carrega sons, músicas e inicializa o jogo
    Simulador* jogo = new Simulador();
    
    // ========================================================================
    // LOOP PRINCIPAL DO JOGO
    // Executa enquanto a janela estiver aberta (utilizador não fecha)
    // ========================================================================
    while(!WindowShouldClose()) {
        // 1. ATUALIZAR: processa inputs, IA, física, estado do jogo
        jogo->Update();
        
        // 2. RENDERIZAR: desenha todos os elementos no ecrã
        jogo->Draw();
    }
    
    // --- LIMPEZA DE RECURSOS ---
    delete jogo;              // Destrutor liberta memória e recursos de áudio
    CloseAudioDevice();       // Fecha sistema de áudio do raylib
    CloseWindow();            // Fecha a janela do jogo
    
    return 0;                 // Exit code: 0 = sucesso
}