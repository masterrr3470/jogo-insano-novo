#include "Simulador.h"
#include "raymath.h"
#include <fstream>
#include <cmath>

int nivelAtual = 1;
const char* nomesNiveis[] = {"", "PELE", "PULMOES", "FIGADO", "CEREBRO", "CORACAO (BOSS)"};
Color coresNiveis[] = {WHITE, {255, 200, 180}, {200, 255, 220}, {255, 255, 200}, {220, 200, 255}, {255, 180, 180}};
float timerMorte = 0;
int opcaoPausa = 0;

Simulador::Simulador() : estadoAtual(GameState::MENU), pontosDNA(0), nivelSalvo(1),
                          nitroTimer(0), nitroAtivoTimer(0), nitroAtivo(false), velocidadeNormal(320.0f) {
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
    nitroTimer = 0;
    nitroAtivoTimer = 0;
    nitroAtivo = false;
    
    for (auto o : populacao) delete o;
    populacao.clear();
    
    velocidadeNormal = 320.0f + (nivel*25);
    populacao.push_back(new Organismo({640, 360}, {velocidadeNormal, 30.0f, LIME}, TipoSer::JOGADOR));
    
    // 50, 100, 150, 200, 250 AZUIS por fase
    int qtdPresas = 50 * nivel;
    for(int i=0; i < qtdPresas; i++) {
        populacao.push_back(new Organismo(
            {(float)GetRandomValue(100, 1180), (float)GetRandomValue(100, 620)}, 
            {180.0f + nivel*15, 8.0f, SKYBLUE}, 
            TipoSer::NEUTRO
        ));
    }
    
    // 20 BRANCOS (sem respawn)
    for(int i=0; i < 20; i++) {
        populacao.push_back(new Organismo(
            {(float)GetRandomValue(100, 1180), (float)GetRandomValue(100, 620)}, 
            {170.0f + nivel*12, 18.0f, WHITE}, 
            TipoSer::ANTICORPO
        ));
    }
}

void Simulador::DrawOrgaoAnimado(int nivel, int x, int y, bool selecionado, float tempo) {
    Color corBase = selecionado ? GOLD : DARKGRAY;
    Color corDetalhe = selecionado ? YELLOW : GRAY;
    float animacao = tempo * 1.5f;
    
    switch(nivel) {
        case 1:
            DrawRectangle(x-40, y-30, 80, 60, Color{255, 220, 180, 255});
            DrawRectangle(x-35, y-25, 70, 50, corDetalhe);
            for(int i=0; i<6; i++) {
                for(int j=0; j<5; j++) {
                    float offset = sinf(tempo + i*0.5f + j*0.3f) * 1.5f;
                    DrawCircle(x-30+i*14, (int)(y-20+j*12+offset), 2, corBase);
                }
            }
            DrawText("PELE", x-20, y+35, 14, corBase);
            break;
            
        case 2:
            {
                float respiracao = sinf(animacao * 0.8f) * 4;
                DrawEllipse(x-25, (int)(y-5+respiracao), 20, 28, corBase);
                DrawEllipse(x+25, (int)(y-5+respiracao), 20, 28, corBase);
                DrawRectangle(x-5, (int)(y-35+respiracao), 10, 25, corDetalhe);
                DrawLine(x-5, (int)(y-10+respiracao), x-25, (int)(y+10+respiracao), corDetalhe);
                DrawLine(x+5, (int)(y-10+respiracao), x+25, (int)(y+10+respiracao), corDetalhe);
                DrawText("PULMOES", x-30, y+35, 14, corBase);
            }
            break;
            
        case 3:
            {
                float pulso = sinf(animacao * 1.2f) * 2;
                DrawEllipse(x, (int)(y-5+pulso), 35, 28, corBase);
                DrawCircle(x+15, (int)(y-10+pulso), 18, corDetalhe);
                DrawCircle(x-20, (int)(y+pulso), 14, corDetalhe);
                DrawCircle(x+25, (int)(y+15+pulso), 6, Color{100, 180, 100, 255});
                DrawText("FIGADO", x-25, y+35, 14, corBase);
            }
            break;
            
        case 4:
            {
                float atividade = sinf(animacao * 2.5f) * 1.5f;
                for(int i=0; i<5; i++) {
                    float offset = sinf(tempo*3 + i) * 2;
                    DrawCircle(x-15 + (int)offset, (int)(y-15+i*8+atividade), 10, corBase);
                }
                for(int i=0; i<5; i++) {
                    float offset = sinf(tempo*3 + i*0.8f) * 2;
                    DrawCircle(x+15 + (int)offset, (int)(y-15+i*8+atividade), 10, corBase);
                }
                DrawLine(x, (int)(y-25+atividade), x, (int)(y+25+atividade), corDetalhe);
                DrawText("CEREBRO", x-30, y+35, 14, corBase);
            }
            break;
            
        case 5:
            {
                float batimento = sinf(animacao * 4) * 5;
                DrawCircle(x-12, (int)(y-20+batimento), 12, corBase);
                DrawCircle(x+12, (int)(y-20+batimento), 12, corBase);
                DrawEllipse(x-10, (int)(y+batimento), 15, 22, corDetalhe);
                DrawEllipse(x+10, (int)(y+batimento), 15, 22, corDetalhe);
                DrawRectangle(x-3, (int)(y-35+batimento), 6, 15, Color{200, 50, 50, 255});
                
                if (selecionado) {
                    float pulse = sinf(tempo * 8) * 6;
                    DrawCircle(x, (int)(y+35+pulse), 5, RED);
                }
                DrawText("CORACAO", x-30, y+35, 14, corBase);
            }
            break;
    }
}

void Simulador::DrawMenuAnimado(float tempo) {
    for(int y=0; y<720; y+=10) {
        float gradient = sinf(tempo * 0.5f + y * 0.02f) * 0.3f + 0.3f;
        Color c = {
            (unsigned char)(20 + gradient * 40),
            (unsigned char)(10 + gradient * 20),
            (unsigned char)(10 + gradient * 20),
            255
        };
        DrawRectangle(0, y, 1280, 10, c);
    }
    
    for(int i=0; i<30; i++) {
        float x = sinf(tempo * 0.3f + i) * 640 + 640;
        float y = cosf(tempo * 0.2f + i*0.5f) * 360 + 360;
        float size = sinf(tempo + i) * 2 + 3;
        DrawCircle((int)x, (int)y, size, Color{100, 255, 100, (unsigned char)(100 + sinf(tempo*2+i)*50)});
    }
    
    DrawText("VIRUS: BIOWARFARE", 380, 100, 50, LIME);
    DrawText("~~~~~~~~~~~~~~~", 390, 155, 40, DARKGREEN);
    DrawText("Infecte. Domine. Destrua.", 450, 180, 22, GRAY);
    
    int startY = 280;
    float buttonPulse = sinf(tempo * 3) * 3;
    
    DrawRectangle(450, (int)(startY + buttonPulse), 380, 50, Color{0, 100, 0, 200});
    DrawRectangleLines(450, (int)(startY + buttonPulse), 380, 50, LIME);
    DrawText("[N] NOVO JOGO", 540, (int)(startY + 15 + buttonPulse), 25, WHITE);
    
    DrawRectangle(450, (int)(startY + 60 + buttonPulse), 380, 50, Color{0, 100, 0, 200});
    DrawRectangleLines(450, (int)(startY + 60 + buttonPulse), 380, 50, LIME);
    DrawText("[L] CARREGAR JOGO", 510, (int)(startY + 75 + buttonPulse), 25, WHITE);
    
    DrawRectangle(450, (int)(startY + 120 + buttonPulse), 380, 50, Color{0, 100, 0, 200});
    DrawRectangleLines(450, (int)(startY + 120 + buttonPulse), 380, 50, LIME);
    DrawText("[I] INSTRUCOES", 520, (int)(startY + 135 + buttonPulse), 25, WHITE);
    
    DrawRectangle(450, (int)(startY + 180 + buttonPulse), 380, 50, Color{100, 0, 0, 200});
    DrawRectangleLines(450, (int)(startY + 180 + buttonPulse), 380, 50, RED);
    DrawText("[Q] SAIR", 580, (int)(startY + 195 + buttonPulse), 25, WHITE);
}

void Simulador::Update() {
    float dt = GetFrameTime();
    
    if (nitroAtivo) {
        nitroAtivoTimer -= dt;
        if (nitroAtivoTimer <= 0) {
            nitroAtivo = false;
            nitroTimer = 20.0f;
            populacao[0]->dna.velocidade = velocidadeNormal;
        }
    } else {
        nitroTimer -= dt;
        if (nitroTimer <= 0) nitroTimer = 0;
    }
    
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
        
        if (IsKeyPressed(KEY_LEFT_SHIFT) && nitroTimer <= 0 && !nitroAtivo) {
            nitroAtivo = true;
            nitroAtivoTimer = 5.0f;
            populacao[0]->dna.velocidade = velocidadeNormal * 2.5f;
        }
        
        Organismo* player = populacao[0];
        
        Vector2 m = {(float)(IsKeyDown(KEY_D)-IsKeyDown(KEY_A)), (float)(IsKeyDown(KEY_S)-IsKeyDown(KEY_W))};
        if (Vector2Length(m) > 0) {
            player->pos = Vector2Add(player->pos, Vector2Scale(Vector2Normalize(m), player->dna.velocidade * dt));
        }
        player->pos.x = Clamp(player->pos.x, 0.0f, 1280.0f);
        player->pos.y = Clamp(player->pos.y, 0.0f, 720.0f);
        
        for(int i = (int)populacao.size() - 1; i >= 1; i--) {
            populacao[i]->Update(dt, player->pos, populacao);
        }
        
        ResolverColisoes(populacao);
        
        int neutros = 0;
        int anticorpos = 0;
        
        for(int i = (int)populacao.size() - 1; i >= 1; i--) {
            if (populacao[i]->tipo == TipoSer::NEUTRO) neutros++;
            if (populacao[i]->tipo == TipoSer::ANTICORPO) anticorpos++;
            
            if (Vector2Distance(player->pos, populacao[i]->pos) < 25.0f) {
                if (populacao[i]->tipo == TipoSer::NEUTRO) {
                    populacao[i]->tipo = TipoSer::INFETADO;
                    populacao[i]->dna.cor = LIME;
                    populacao[i]->dna.velocidade += 30.0f;
                    pontosDNA += 15;
                } else if (populacao[i]->tipo == TipoSer::ANTICORPO) {
                    player->energia -= (65.0f + nivelAtual * 12) * dt;
                }
            }
            
            if(populacao[i]->tipo == TipoSer::INFETADO && populacao[i]->energia <= 0) {
                delete populacao[i];
                populacao.erase(populacao.begin() + i);
                continue;
            }
            
            if(populacao[i]->tipo == TipoSer::ANTICORPO && populacao[i]->energia <= 0) {
                pontosDNA += 50;
                delete populacao[i];
                populacao.erase(populacao.begin() + i);
            }
        }
        
        if (neutros == 0) {
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
    }
    else if (estadoAtual == GameState::PAUSE) {
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) opcaoPausa = (opcaoPausa - 1 + 3) % 3;
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) opcaoPausa = (opcaoPausa + 1) % 3;
        
        if (IsKeyPressed(KEY_C) || IsKeyPressed(KEY_P)) estadoAtual = GameState::PLAYING;
        if (IsKeyPressed(KEY_G)) { SaveGame(); estadoAtual = GameState::MENU; }
        if (IsKeyPressed(KEY_M)) estadoAtual = GameState::MENU;
        
        if (IsKeyPressed(KEY_ENTER)) {
            if (opcaoPausa == 0) estadoAtual = GameState::PLAYING;
            else if (opcaoPausa == 1) { SaveGame(); estadoAtual = GameState::MENU; }
            else if (opcaoPausa == 2) estadoAtual = GameState::MENU;
        }
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
    float tempo = GetTime();
    
    if (estadoAtual == GameState::MENU) {
        DrawMenuAnimado(tempo);
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
            DrawOrgaoAnimado(i, 350 + i*120, 280, selecionado, tempo);
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
        
        if (nitroAtivo) {
            DrawText("NITRO ATIVO!", 1080, 12, 20, GOLD);
            DrawRectangle(1080, 35, 100, 15, DARKGRAY);
            DrawRectangle(1080, 35, (int)(100 * (nitroAtivoTimer/5.0f)), 15, GOLD);
        } else {
            if (nitroTimer <= 0) {
                DrawText("NITRO PRONTO [SHIFT]", 1050, 12, 20, LIME);
            } else {
                DrawText(TextFormat("NITRO: %.0fs", nitroTimer), 1080, 12, 18, GRAY);
            }
        }
        
        DrawText(TextFormat("DNA: %d", pontosDNA), 1100, 40, 22, GOLD);
        
        int verdes = 0, azuis = 0, brancos = 0;
        for(auto& o : populacao) {
            if(o->tipo == TipoSer::INFETADO) verdes++;
            else if(o->tipo == TipoSer::NEUTRO) azuis++;
            else if(o->tipo == TipoSer::ANTICORPO) brancos++;
        }
        
        DrawText(TextFormat("VERDES: %d", verdes), 20, 700, 18, LIME);
        DrawText(TextFormat("AZUIS: %d", azuis), 150, 700, 18, SKYBLUE);
        DrawText(TextFormat("BRANCOS: %d", brancos), 280, 700, 18, WHITE);
        
        // MENU DE PAUSA MELHORADO
        if (estadoAtual == GameState::PAUSE) {
            DrawRectangle(0, 0, 1280, 720, Color{0, 0, 0, 200});
            
            DrawRectangle(400, 180, 480, 280, Color{30, 30, 40, 255});
            DrawRectangleLines(400, 180, 480, 280, LIME);
            DrawRectangleLines(402, 182, 476, 276, DARKGREEN);
            
            DrawText("PAUSA", 560, 195, 40, GOLD);
            DrawText("=======", 560, 240, 30, DARKGRAY);
            
            int optionY = 270;
            int spacing = 55;
            
            if (opcaoPausa == 0) {
                DrawRectangle(420, optionY, 440, 45, Color{0, 150, 0, 255});
                DrawRectangleLines(420, optionY, 440, 45, WHITE);
            } else {
                DrawRectangle(420, optionY, 440, 45, Color{50, 50, 60, 255});
            }
            DrawText("[C/P]", 430, optionY + 12, 22, LIME);
            DrawText("CONTINUAR", 520, optionY + 12, 22, WHITE);
            
            optionY += spacing;
            if (opcaoPausa == 1) {
                DrawRectangle(420, optionY, 440, 45, Color{0, 150, 0, 255});
                DrawRectangleLines(420, optionY, 440, 45, WHITE);
            } else {
                DrawRectangle(420, optionY, 440, 45, Color{50, 50, 60, 255});
            }
            DrawText("[G]", 430, optionY + 12, 22, LIME);
            DrawText("GUARDAR E SAIR", 520, optionY + 12, 22, WHITE);
            
            optionY += spacing;
            if (opcaoPausa == 2) {
                DrawRectangle(420, optionY, 440, 45, Color{150, 0, 0, 255});
                DrawRectangleLines(420, optionY, 440, 45, WHITE);
            } else {
                DrawRectangle(420, optionY, 440, 45, Color{50, 50, 60, 255});
            }
            DrawText("[M]", 430, optionY + 12, 22, LIME);
            DrawText("MENU PRINCIPAL", 520, optionY + 12, 22, WHITE);
            
            DrawText("Use W/S ou SETAS para navegar", 450, 420, 16, GRAY);
            DrawText("ENTER para selecionar", 485, 440, 16, GRAY);
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
        DrawText("2. Toque nos AZUIS para infecta-los", 350, 210, 24, SKYBLUE);
        DrawText("3. Os VERDES atacarao os BRANCOS automaticamente", 350, 260, 24, LIME);
        DrawText("4. 1 HIT - Branco mata verde instantaneamente!", 350, 310, 24, GOLD);
        DrawText("5. BRANCOS crescem ao matar verdes!", 350, 350, 24, RED);
        DrawText("6. SHIFT = Nitro (20s cooldown, 5s duracao)", 350, 390, 24, YELLOW);
        DrawText("7. Vitoria: Converta todos os AZUIS!", 350, 430, 24, LIME);
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