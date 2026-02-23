@'
@echo off
cls
echo A compilar com Raylib...
g++ -Iinclude src/*.cpp -o main.exe -lraylib -lopengl32 -lgdi32 -lwinmm
if %errorlevel% equ 0 (
    echo [SUCESSO] A abrir jogo...
    .\main.exe
) else (
    echo [ERRO] Falha na compilacao.
    pause
)
'@ | Out-File -FilePath run.bat -Encoding ascii