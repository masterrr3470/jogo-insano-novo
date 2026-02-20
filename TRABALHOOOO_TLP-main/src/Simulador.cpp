#include "Simulador.h"
#include "raymath.h"
#include <fstream>
#include <cmath>

int nivelAtual = 1;
const char* nomesNiveis[] = {"", "PELE", "PULMOES", "FIGADO", "CEREBRO", "CORACAO (BOSS)"};
Color coresNiveis[] = {WHITE, {255, 200, 180}, {200, 255, 220}, {255, 255, 200}, {220, 200, 255}, {255, 180, 180}};

Simulador::Simulador() : estadoAtual(GameState::MENU), pontosDNA(0), nivelSalvo(1), timerMorte(0), opcaoPausa(0) {
    ResetJogo(1);
}

Simulador::~Simulador() {
    for (auto o : populacao) delete o;
    populacao.clear();
}

void Simulador::SaveGame() {
    std::ofstream file("save.dat", std::ios::binary);
    if(file.is_open()) {
        file.write((char*)&nivelAtual, sizeof(int));
        file.write((char*)&pontosDNA, sizeof(int));
        file.close();
    }
}

void Simulador::LoadGame() {
    std::ifstream file("save.dat", std::ios::binary);
    if (file.is_open()) {
        file.read((char*)&nivelAtual, sizeof(int));
        file.read((char*)&pontosDNA, sizeof(int));
        file.close();
        estadoAtual = GameState::MAPA;
    }
}

void Simulador::ResetJogo(int nivel) {
    nivelAtual = nivel;
    for (auto o : populacao) delete o;
    populacao.clear();
    
    // JOGADOR - MAIS RÁPIDO E MAIOR
    populacao.push_back(new Organismo({640, 360}, {320.0f + (nivel*25), 30.0f, LIME}, TipoSer::JOGADOR));
    
    // AZUIS - Quantidade aumentada e mais rápidos
    int qtdPresas = (25 + nivel * 8);
    for(int i=0; i < qtdPresas; i++) {
        populacao.push_back(new Organismo(
            {(float)GetRandomValue(100, 1180), (float)GetRandomValue(100, 620)}, 
            {180.0f + nivel*15, 8.0f, SKYBLUE}, 
            TipoSer::NEUTRO
        ));
    }
    
    // Anticorpos (roxos)
    int qtdAnticorpos = 2 + nivel;
    for(int i=0; i < qtdAnticorpos; i++) {
        populacao.push_back(new Organismo(
            {(float)GetRandomValue(100, 1180), (float)GetRandomValue(100, 620)}, 
            {170.0f + nivel*12, 18.0f, Color{220, 80, 220, 255}}, 
            TipoSer::ANTICORPO
        ));
    }
}

void Simulador::DrawOrgaoPixelado(int nivel, int x, int y, bool selecionado) {
    Color corBase = selecionado ? GOLD : DARKGRAY;
    Color corDetalhe = selecionado ? YELLOW : GRAY;
    
    switch(nivel) {
        case 1: // PELE
            DrawRectangle(x-40, y-30, 80, 60, corBase);
            DrawRectangle(x-35, y-25, 70, 50, corDetalhe);
            for(int i=0; i<5; i++) {
                for(int j=0; j<4; j++) {
                    DrawRectangle(x-30+i*15, y-20+j*12, 4, 4, corBase);
                }
            }
            DrawText("PELE", x-20, y+35, 12, corBase);
            break;
            
        case 2: // PULMÕES
            DrawCircle(x-20, y-10, 25, corBase);
            DrawCircle(x+20, y-10, 25, corBase);
            DrawRectangle(x-25, y-15, 50, 20, corDetalhe);
            DrawRectangle(x-15, y-20, 6, 25, corDetalhe);
            DrawRectangle(x+9, y-20, 6, 25, corDetalhe);
            DrawText("PULMOES", x-30, y+35, 12, corBase);
            break;
            
        case 3: // FÍGADO
            DrawRectangle(x-35, y-25, 70, 50, corBase);
            DrawCircle(x-25, y-25, 15, corDetalhe);
            DrawCircle(x+25, y-20, 12, corDetalhe);
            DrawRectangle(x-30, y-15, 25, 30, corDetalhe);
            DrawRectangle(x+5, y-10, 25, 25, corDetalhe);
            DrawText("FIGADO", x-25, y+35, 12, corBase);
            break;
            
        case 4: // CÉREBRO
            DrawCircle(x, y-5, 30, corBase);
            DrawCircle(x-15, y-15, 12, corDetalhe);
            DrawCircle(x+15, y-15, 12, corDetalhe);
            DrawCircle(x-10, y+10, 10, corDetalhe);
            DrawCircle(x+10, y+10, 10, corDetalhe);
            DrawCircle(x, y-20, 10, corDetalhe);
            DrawLine(x, y-30, x, y+20, corDetalhe);
            DrawText("CEREBRO", x-30, y+35, 12, corBase);
            break;
            
        case 5: // CORAÇÃO
            DrawCircle(x-12, y-15, 15, corBase);
            DrawCircle(x+12, y-15, 15, corBase);
            DrawTriangle({(float)(x-12), (float)(y-28)}, {(float)(x-25), (float)(y-10)}, {(float)(x), (float)(y+10)}, corBase);
            DrawTriangle({(float)(x+12), (float)(y-28)}, {(float)(x+25), (float)(y-10)}, {(float)(x), (float)(y+10)}, corBase);
            DrawRectangle(x-8, y-5, 16, 20, corDetalhe);
            if (selecionado) {
                float pulse = sinf(GetTime() * 3) * 3;
                DrawCircle(x, (int)(y+25 + pulse), 3, RED);
            }
            DrawText("CORACAO", x-30, y+35, 12, corBase);
            break;
    }
}

void Simulador::Update() {
    if (IsKeyPressed(KEY_F11)) ToggleFullscreen();
    
    if (estadoAtual == GameState::MENU) {
        if (IsKeyPressed(KEY_N)) { nivelAtual = 1; pontosDNA = 0; estadoAtual = GameState::MAPA; }
        if (IsKeyPressed(KEY_L)) LoadGame();
        if (IsKeyPressed(KEY_I)) estadoAtual = GameState::INSTRUCTIONS;
        if (IsKeyPressed(KEY_Q)) CloseWindow();
    }
    else if (estadoAtual == GameState::MAPA) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            ResetJogo(nivelAtual);
            estadoAtual = GameState::PLAYING;
        }
        if (IsKeyPressed(KEY_BACKSPACE)) estadoAtual = GameState::MENU;
    }
    else if (estadoAtual == GameState::PLAYING) {
        if (IsKeyPressed(KEY_P)) { estadoAtual = GameState::PAUSE; opcaoPausa = 0; }
        
        float dt = GetFrameTime();
        Organismo* player = populacao[0];
        
        Vector2 m = {(float)(IsKeyDown(KEY_D)-IsKeyDown(KEY_A)), (float)(IsKeyDown(KEY_S)-IsKeyDown(KEY_W))};
        if (Vector2Length(m) > 0) {
            player->pos = Vector2Add(player->pos, Vector2Scale(Vector2Normalize(m), player->dna.velocidade * dt));
        }
        player->pos.x = Clamp(player->pos.x, 0.0f, 1280.0f);
        player->pos.y = Clamp(player->pos.y, 0.0f, 720.0f);
        
        int neutros = 0;
        int anticorpos = 0;
        
        for(int i = (int)populacao.size() - 1; i >= 1; i--) {
            populacao[i]->Update(dt, player->pos, populacao);
            
            if (populacao[i]->tipo == TipoSer::NEUTRO) neutros++;
            if (populacao[i]->tipo == TipoSer::ANTICORPO) anticorpos++;
            
            if (Vector2Distance(player->pos, populacao[i]->pos) < 25.0f) {
                if (populacao[i]->tipo == TipoSer::NEUTRO) {
                    populacao[i]->tipo = TipoSer::INFETADO;
                    populacao[i]->dna.cor = LIME;
                    populacao[i]->dna.velocidade += 30.0f;
                    pontosDNA += 15;
                } else if (populacao[i]->tipo == TipoSer::ANTICORPO) {
                    player->energia -= (40.0f + nivelAtual * 6) * dt;
                }
            }
            
            if(populacao[i]->tipo == TipoSer::ANTICORPO && populacao[i]->energia <= 0) {
                pontosDNA += 50;
                delete populacao[i];
                populacao.erase(populacao.begin() + i);
            }
        }
        
        if (neutros == 0 && anticorpos == 0) {
            if (nivelAtual < 5) { 
                nivelAtual++; 
                estadoAtual = GameState::MAPA; 
            } else { 
                estadoAtual = GameState::CUTSCENE; 
                timerMorte = 0; 
            }
        }
        
        if (player->energia <= 0) {
            estadoAtual = GameState::GAMEOVER;
        }
        
        if (GetRandomValue(0, 100) < (1 + nivelAtual) && anticorpos < 5 + nivelAtual) {
            populacao.push_back(new Organismo(
                {(float)GetRandomValue(0, 1280), (float)GetRandomValue(0, 100)}, 
                {170.0f + nivelAtual*12, 18.0f, Color{220, 80, 220, 255}}, 
                TipoSer::ANTICORPO
            ));
        }
    }
    else if (estadoAtual == GameState::PAUSE) {
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) opcaoPausa = (opcaoPausa - 1 + 3) % 3;
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) opcaoPausa = (opcaoPausa + 1) % 3;
        if (IsKeyPressed(KEY_ENTER)) {
            if (opcaoPausa == 0) estadoAtual = GameState::PLAYING;
            else if (opcaoPausa == 1) { SaveGame(); estadoAtual = GameState::MENU; }
            else if (opcaoPausa == 2) estadoAtual = GameState::MENU;
        }
        if (IsKeyPressed(KEY_P)) estadoAtual = GameState::PLAYING;
    }
    else if (estadoAtual == GameState::CUTSCENE) {
        timerMorte += GetFrameTime();
        if (timerMorte > 5.0f && IsKeyPressed(KEY_BACKSPACE)) estadoAtual = GameState::MENU;
    }
    else {
        if (IsKeyPressed(KEY_BACKSPACE)) estadoAtual = GameState::MENU;
    }
}

void Simulador::Draw() {
    BeginDrawing();
    
    if (estadoAtual == GameState::MENU) {
        ClearBackground({10, 5, 5, 255});
        DrawText("VIRUS: BIOWARFARE", 380, 120, 50, LIME);
        DrawText("~~~~~~~~~~~~~~~", 390, 175, 40, DARKGREEN);
        
        DrawRectangle(450, 280, 380, 50, Color{0, 100, 0, 200});
        DrawText("[N] NOVO JOGO", 540, 295, 25, WHITE);
        
        DrawRectangle(450, 340, 380, 50, Color{0, 100, 0, 200});
        DrawText("[L] CARREGAR JOGO", 510, 355, 25, WHITE);
        
        DrawRectangle(450, 400, 380, 50, Color{0, 100, 0, 200});
        DrawText("[I] INSTRUCOES", 520, 415, 25, WHITE);
        
        DrawRectangle(450, 460, 380, 50, Color{100, 0, 0, 200});
        DrawText("[Q] SAIR", 580, 475, 25, WHITE);
        
        DrawText("Infecte os azuis - Destrua os roxos", 420, 600, 20, GRAY);
    }
    else if (estadoAtual == GameState::MAPA) {
        ClearBackground({15, 5, 20, 255});
        for(int i=0; i<50; i++) {
            DrawCircle((i*137)%1280, (i*89)%720, (i%3)+1, Color{200, 200, 255, (unsigned char)(100+(i%155))});
        }
        
        DrawText("SISTEMA DO HOSPEDEIRO", 450, 50, 35, RED);
        DrawText("Selecione o orgao para infectar:", 420, 90, 22, GRAY);
        
        for(int i=1; i<=5; i++) {
            bool selecionado = (i == nivelAtual);
            DrawOrgaoPixelado(i, 350 + i*120, 280, selecionado);
        }
        
        DrawText("Pressione [ENTER] para iniciar", 430, 550, 24, GREEN);
        DrawText("[BACKSPACE] Voltar", 520, 590, 20, GRAY);
    }
    else if (estadoAtual == GameState::PLAYING || estadoAtual == GameState::PAUSE) {
        Color fundoOrgao = coresNiveis[nivelAtual];
        ClearBackground(fundoOrgao);
        
        for(int x=0; x<1280; x+=80) DrawLine(x, 0, x, 720, Color{0, 0, 0, 20});
        for(int y=0; y<720; y+=80) DrawLine(0, y, 1280, y, Color{0, 0, 0, 20});
        
        for(auto& o : populacao) o->Draw();
        
        DrawRectangle(0, 0, 1280, 50, Color{0, 0, 0, 180});
        DrawText(TextFormat("ORGAO: %s", nomesNiveis[nivelAtual]), 20, 12, 24, WHITE);
        
        DrawText("VIDA:", 850, 14, 20, WHITE);
        DrawRectangle(920, 15, 150, 20, DARKGRAY);
        DrawRectangle(920, 15, (int)(1.5f * populacao[0]->energia), 20, 
            populacao[0]->energia > 50 ? LIME : (populacao[0]->energia > 25 ? YELLOW : RED));
        DrawRectangleLines(920, 15, 150, 20, WHITE);
        
        DrawText(TextFormat("DNA: %d", pontosDNA), 1100, 14, 22, GOLD);
        
        int verdes = 0, azuis = 0, roxos = 0;
        for(auto& o : populacao) {
            if(o->tipo == TipoSer::INFETADO) verdes++;
            else if(o->tipo == TipoSer::NEUTRO) azuis++;
            else if(o->tipo == TipoSer::ANTICORPO) roxos++;
        }
        
        DrawText(TextFormat("VERDES: %d", verdes), 20, 700, 18, LIME);
        DrawText(TextFormat("AZUIS: %d", azuis), 150, 700, 18, Color{100, 220, 255, 255});
        DrawText(TextFormat("ROXOS: %d", roxos), 280, 700, 18, Color{220, 80, 220, 255});
        
        if (estadoAtual == GameState::PAUSE) {
            DrawRectangle(0, 0, 1280, 720, {0, 0, 0, 200});
            DrawRectangle(400, 180, 480, 350, Color{30, 30, 30, 255});
            DrawRectangleLines(400, 180, 480, 350, LIME);
            
            DrawText("PAUSA", 560, 200, 45, GOLD);
            
            DrawRectangle(450, 280, 380, 60, opcaoPausa == 0 ? Color{0, 150, 0, 255} : Color{50, 50, 50, 255});
            DrawText("CONTINUAR", 540, 300, 28, WHITE);
            
            DrawRectangle(450, 350, 380, 60, opcaoPausa == 1 ? Color{0, 150, 0, 255} : Color{50, 50, 50, 255});
            DrawText("GUARDAR E SAIR", 480, 370, 28, WHITE);
            
            DrawRectangle(450, 420, 380, 60, opcaoPausa == 2 ? Color{0, 150, 0, 255} : Color{50, 50, 50, 255});
            DrawText("MENU PRINCIPAL", 485, 440, 28, WHITE);
        }
    }
    else if (estadoAtual == GameState::CUTSCENE) {
        float alpha = fminf(timerMorte * 50.0f, 255.0f);
        ClearBackground({(unsigned char)fminf(50 + timerMorte * 20, 150), 0, 0, 255});
        
        for(int i=0; i<30; i++) {
            float angle = (i/30.0f) * PI * 2 + timerMorte;
            float dist = timerMorte * 250;
            DrawCircle(640 + (int)(cosf(angle)*dist), 360 + (int)(sinf(angle)*dist), 6, Color{200, 0, 0, (unsigned char)(255-timerMorte*30)});
        }
        
        DrawText("O CORACAO PAROU.", 400, 280, 50, Color{(unsigned char)alpha, 0, 0, 255});
        if (timerMorte > 2.0f) DrawText("O HOSPEDEIRO FOI DERROTADO.", 330, 380, 35, WHITE);
        if (timerMorte > 4.0f) DrawText("PRESSIONE [BACKSPACE] PARA O MENU", 400, 600, 22, GRAY);
    }
    else if (estadoAtual == GameState::GAMEOVER) {
        ClearBackground({30, 10, 10, 255});
        DrawText("VIRUS ERRADICADO.", 430, 300, 45, RED);
        DrawText("O sistema imunologico venceu.", 410, 370, 28, GRAY);
        DrawText(TextFormat("Pontos de DNA: %d", pontosDNA), 480, 420, 24, GOLD);
        DrawText("[BACKSPACE] PARA VOLTAR AO MENU", 420, 520, 24, WHITE);
    }
    else if (estadoAtual == GameState::INSTRUCTIONS) {
        ClearBackground({10, 20, 10, 255});
        DrawText("COMO JOGAR:", 480, 80, 40, LIME);
        DrawText("1. Use W/A/S/D para mover", 350, 160, 24, WHITE);
        DrawText("2. Toque nos AZUIS para infecta-los", 350, 210, 24, Color{100, 220, 255, 255});
        DrawText("3. Os VERDES atacarao os ROXOS automaticamente", 350, 260, 24, LIME);
        DrawText("4. Evite os ROXOS - eles causam dano!", 350, 310, 24, Color{220, 80, 220, 255});
        DrawText("5. Infecte todos e destrua o sistema imunologico!", 350, 360, 24, GOLD);
        DrawText("[BACKSPACE] Voltar", 520, 550, 24, GRAY);
    }
    
    EndDrawing();
}

void Simulador::DesenharBossMorte() {
    float t = timerMorte;
    for(int i=0; i<20; i++) {
        float angle = t * 3 + i * PI / 10;
        float dist = 100 + t * 150;
        DrawCircle(640 + (int)(cosf(angle)*dist), 360 + (int)(sinf(angle)*dist), 8, RED);
    }
}