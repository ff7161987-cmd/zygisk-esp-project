# Workflow de Compilação AIDE NDK (ARM64)

Este guia descreve como configurar e compilar este projeto Zygisk usando o **AIDE (Android IDE)** com suporte a NDK no Android.

## Pré-requisitos
1. **AIDE com NDK instalado**: Certifique-se de que você baixou os arquivos do NDK dentro do AIDE.
2. **Arquitetura**: O projeto está configurado para `armeabi-v7a` e `arm64-v8a`.

## Estrutura de Compilação
O projeto utiliza `ndk-build` via arquivos `Android.mk` e `Application.mk`.

### Passos para Compilar no AIDE:
1. Abra o AIDE e navegue até a pasta raiz do projeto (`zygisk-esp-project`).
2. O AIDE deve reconhecer o projeto automaticamente como um projeto Android.
3. Vá em **Settings > Build & Run** e certifique-se de que o NDK está habilitado.
4. Para compilar apenas o NDK (arquivos C++):
   - Abra o arquivo `app/src/main/jni/Android.mk`.
   - Clique no botão de "Build" ou selecione "Rebuild Project".
5. O binário `.so` resultante será gerado em `app/src/main/libs/arm64-v8a/libRAMA_MODZ.so`.

## Configuração ARM64
O arquivo `Application.mk` já define:
```makefile
APP_ABI := arm64-v8a armeabi-v7a
APP_PLATFORM := android-21
APP_STL := c++_static
```
Isso garante que o AIDE compile para ambas as arquiteturas modernas.

## Análise de Universalidade (Unity/IL2CPP)
O projeto utiliza a biblioteca `Il2Cpp.h` e `DobbyHook` para realizar hooks dinâmicos.
- **Universalidade**: O projeto é **semi-universal**. Ele contém funções para obter offsets via `Il2CppGetMethodOffset`, o que permite adaptar para qualquer jogo Unity IL2CPP apenas alterando os nomes das classes e métodos no arquivo `app/src/main/jni/Main.cpp`.
- **Zygisk**: Por ser um módulo Zygisk, ele injeta diretamente no processo da Zygote, permitindo burlar proteções de nível de usuário.

---
*Gerado automaticamente pelo Manus AI 1.6 Max*
