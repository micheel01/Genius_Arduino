/*******************************************************************************
*
Jogo Genius adaptado para arduino
Para fins acadêmicos

Unibrasil - 5 periodo Engenharia de software

Professor: Fabio Bettio

Criado por:
Michel de Oliviera
Paulo Grunte

*
*******************************************************************************/
#define NOTE_D4  294
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_A5  880

// Variáveis globais
// Nomes de arrays já são ponteiros para o primeiro elemento, mas vamos usar explicitamente em algumas funções.
int tons[4] = { NOTE_A5, NOTE_A4, NOTE_G4, NOTE_D4 };
int sequencia[100] = {}; // Array principal da sequência
int rodada_atual = 0;
int passo_atual_na_sequencia = 0;

int pinoAudio = 12;
int pinosLeds[4] = { 2, 4, 6, 8 };
int pinosBotoes[4] = { 3, 5, 7, 9 };

int botao_pressionado = 0;
int perdeu_o_jogo = false;
int score = 0;

bool jogoIniciado = false;
bool jogoPausado = false;

// --- Protótipos das Funções (necessários quando as funções chamam umas as outras) ---
// Adicionei protótipos para todas as funções para garantir que o compilador as conheça
// antes de serem chamadas, resolvendo problemas de "not declared in this scope".
void resetarJogo();
void exibirMenu();
void tocarSomDeInicio();
void proximaRodada();
void reproduzirSequencia();
void aguardarJogador();
void aguardarJogada();
void gameOver();
void processarComandoSerial(char key); // Declarando esta função antes de ser usada

// --- Funções Auxiliares para o Jogo ---

// Função para resetar o estado do jogo (usando ponteiros para score e rodada_atual)
void resetarJogo() {
  // Ponteiro para o primeiro elemento do array 'sequencia'
  int *ptrSequencia = sequencia;
  for (int i = 0; i < 100; i++) {
    *ptrSequencia = 0; // Zera o valor apontado pelo ponteiro
    ptrSequencia++;    // Move o ponteiro para o próximo elemento
  }
  
  rodada_atual = 0;
  passo_atual_na_sequencia = 0;
  perdeu_o_jogo = false;
  score = 0; // Resetar pontuação
  jogoIniciado = false;
  jogoPausado = false;
  noTone(pinoAudio);
  
  // Ponteiro para o primeiro elemento do array 'pinosLeds'
  int *ptrLeds = pinosLeds;
  for(int i = 0; i < 4; i++) {
    digitalWrite(*ptrLeds, LOW); // Apaga o LED apontado
    ptrLeds++; // Move o ponteiro
  }
}

// Função para exibir o menu no Serial Monitor
void exibirMenu() {
  Serial.println("\n--- Jogo Genius ---");
  Serial.println("Menu:");
  Serial.println("1 - Iniciar Jogo");
  Serial.println("2 - Pausar/Despausar Jogo");
  Serial.println("3 - Finalizar Jogo (resetar)");
  Serial.println("4 - Mostrar Pontuacao Atual");
  Serial.println("-------------------");
}

// Função para processar comandos do serial
void processarComandoSerial(char key) {
  delay(10);
  while (Serial.available()) {
    Serial.read();
  }

  switch (key) {
    case '1':
      if (!jogoIniciado) {
        Serial.println("Comando: Iniciar Jogo");
        resetarJogo();
        jogoIniciado = true;
        tocarSomDeInicio();
        Serial.println("Jogo iniciado! Boa sorte!");
      } else {
        Serial.println("O jogo já está em andamento. Para reiniciar, use '3' e depois '1'.");
      }
      break;
    case '2':
      if (jogoIniciado) {
        jogoPausado = !jogoPausado;
        if (jogoPausado) {
          Serial.println("Comando: Jogo Pausado.");
          noTone(pinoAudio);
          // Usando ponteiro para apagar LEDs
          int *ptrLeds = pinosLeds;
          for(int i = 0; i < 4; i++) {
            digitalWrite(*ptrLeds, LOW);
            ptrLeds++;
          }
          digitalWrite(pinosLeds[0], HIGH); // Feedback visual de pausa
        } else {
          Serial.println("Comando: Jogo Despausado.");
          digitalWrite(pinosLeds[0], LOW);
        }
      } else {
        Serial.println("O jogo não está em andamento para ser pausado/despausado.");
      }
      break;
    case '3':
      if (jogoIniciado || perdeu_o_jogo) {
        Serial.println("Comando: Finalizar Jogo (resetar).");
        gameOver(); // Executa rotina de Game Over para feedback visual/sonoro
        resetarJogo(); // Reseta todas as variáveis
        Serial.println("Jogo finalizado. Pressione '1' para iniciar uma nova partida.");
      } else {
        Serial.println("O jogo não está em andamento para ser finalizado.");
      }
      break;
    case '4':
      Serial.print("Comando: Mostrar Pontuacao Atual. Pontuação: ");
      Serial.println(score);
      break;
    default:
      Serial.println("Opção inválida. Por favor, digite 1, 2, 3 ou 4.");
      break;
  }
  exibirMenu();
}

void setup() {
  // Configurando pinos com ponteiros
  int *ptrLeds = pinosLeds;
  for (int i = 0; i <= 3; i++) {
    pinMode(*ptrLeds, OUTPUT); // Configura o pino apontado como OUTPUT
    ptrLeds++;                 // Move o ponteiro para o próximo pino
  }

  int *ptrBotoes = pinosBotoes;
  for (int i = 0; i <= 3; i++) {
    pinMode(*ptrBotoes, INPUT_PULLUP); // Configura o pino apontado como INPUT_PULLUP
    ptrBotoes++;                       // Move o ponteiro para o próximo pino
  }
  
  pinMode(pinoAudio, OUTPUT);
  randomSeed(analogRead(0));
  Serial.begin(9600);
  exibirMenu();
}

void loop() {
  // Sempre verifica e processa comandos serial no início do loop
  if (Serial.available()) {
    char key = Serial.read();
    processarComandoSerial(key);
  }

  // --- Lógica Principal do Jogo Genius ---
  if (jogoIniciado && !jogoPausado) {
    if (perdeu_o_jogo) {
      Serial.println("Você perdeu! Pressione '3' para finalizar/resetar, ou '1' para iniciar uma nova partida.");
      jogoIniciado = false;
      delay(2000); 
      return; 
    }
    
    Serial.print("Rodada: ");
    Serial.println(rodada_atual + 1); 

    proximaRodada();
    reproduzirSequencia();
    // Sai se o jogo foi interrompido durante a reprodução da sequência
    if (!jogoIniciado || jogoPausado) return; 

    aguardarJogador();

    // Verifica novamente se o jogo não foi perdido ou finalizado durante a espera da jogada
    if (!perdeu_o_jogo && jogoIniciado && !jogoPausado) {
      score++;
      Serial.print("Parabéns! Pontuação atual: ");
      Serial.println(score);
    }
    
    // Pequeno delay entre as rodadas, mas pode ser interrompido
    unsigned long tempoAtual = millis();
    unsigned long tempoFinal = tempoAtual + 1000;
    while (millis() < tempoFinal && jogoIniciado && !jogoPausado) {
        if (Serial.available()) {
            char key = Serial.read();
            processarComandoSerial(key);
            if (!jogoIniciado || jogoPausado) break; 
        }
        delay(10);
    }

  } else if (jogoIniciado && jogoPausado) {
    // Se o jogo está pausado, podemos dar um feedback visual leve aqui
    digitalWrite(pinosLeds[0], !digitalRead(pinosLeds[0]));
    delay(500);
  } else {
    delay(50);
  }
}

// Sorteia um novo item e adiciona na sequência (rodada_atual é global)
void proximaRodada() {
  int numero_sorteado = random(0, 4);
  // Usa aritmética de ponteiros para adicionar à sequência
  *(sequencia + rodada_atual) = numero_sorteado; 
  rodada_atual++; // Incrementa a rodada
}

// Reproduz a sequência para ser memorizada.
void reproduzirSequencia() {
  // Ponteiros para percorrer os arrays
  int *ptrSequencia = sequencia; // Ponteiro para o elemento atual na sequência a ser reproduzida
  int *ptrTons = tons;           // Ponteiro para o início do array de tons
  int *ptrLeds = pinosLeds;      // Ponteiro para o início do array de LEDs

  for (int i = 0; i < rodada_atual; i++) {
    if (!jogoIniciado || jogoPausado) {
      noTone(pinoAudio);
      for(int j = 0; j < 4; j++) digitalWrite(*(pinosLeds + j), LOW); // Apaga todos os LEDs
      return;
    }

    // Acessa o tom e o LED correto usando desreferência e aritmética de ponteiros
    int indiceDaCor = *(ptrSequencia + i); // Pega o índice da cor na sequência
    tone(pinoAudio, *(ptrTons + indiceDaCor)); // Acessa o tom correspondente
    digitalWrite(*(ptrLeds + indiceDaCor), HIGH); // Acende o LED correspondente
    
    unsigned long tempoFinalNota = millis() + 500;
    while (millis() < tempoFinalNota && !jogoPausado && jogoIniciado) {
        if (Serial.available()) {
            char key = Serial.read();
            processarComandoSerial(key);
            if (!jogoIniciado || jogoPausado) break;
        }
        delay(10);
    }
    
    noTone(pinoAudio);
    digitalWrite(*(ptrLeds + indiceDaCor), LOW); // Apaga o LED
    
    unsigned long tempoFinalPausa = millis() + 100;
    while (millis() < tempoFinalPausa && !jogoPausado && jogoIniciado) {
        if (Serial.available()) {
            char key = Serial.read();
            processarComandoSerial(key);
            if (!jogoIniciado || jogoPausado) break;
        }
        delay(10);
    }
    if (!jogoIniciado || jogoPausado) return;
  }
  noTone(pinoAudio);
}

// Aguarda o jogador iniciar sua jogada.
void aguardarJogador() {
  // Ponteiro para o elemento atual na sequência que o jogador deve reproduzir
  int *ptrPassoAtualSequencia = &passo_atual_na_sequencia; 

  for (int i = 0; i < rodada_atual; i++) {
    if (!jogoIniciado || perdeu_o_jogo || jogoPausado) {
      break; 
    }
    
    aguardarJogada();
    
    if (!jogoIniciado || perdeu_o_jogo || jogoPausado) {
      break; 
    }
    
    // Verifica a jogada usando o valor da sequência apontado por ptrSequencia
    // e o valor do botão pressionado (acessado por ponteiro também)
    if (*(sequencia + *ptrPassoAtualSequencia) != botao_pressionado) {
      gameOver();
      break;
    }
    
    (*ptrPassoAtualSequencia)++; // Incrementa o valor apontado pelo ponteiro
  }

  *ptrPassoAtualSequencia = 0; // Reseta o valor apontado
}

void aguardarJogada() {
  boolean jogada_efetuada = false;
  unsigned long tempoLimite = millis() + 5000;

  // Ponteiros para os arrays de LEDs e Botões
  int *ptrLeds = pinosLeds;
  int *ptrBotoes = pinosBotoes;
  int *ptrTons = tons;

  while (!jogada_efetuada && millis() < tempoLimite && jogoIniciado && !jogoPausado) { 
    if (Serial.available()) {
      char key = Serial.read();
      processarComandoSerial(key);
      if (jogoPausado || !jogoIniciado) return; 
    }
    
    for (int i = 0; i <= 3; i++) {
      // Acessa o pino do botão e o pino do LED usando aritmética de ponteiros
      if (!digitalRead(*(ptrBotoes + i))) { 
        delay(50); 
        if (!digitalRead(*(ptrBotoes + i))) { 
          botao_pressionado = i;
          tone(pinoAudio, *(ptrTons + i)); // Toca o tom correspondente ao botão
          digitalWrite(*(ptrLeds + i), HIGH); // Acende o LED correspondente
          delay(300); 
          digitalWrite(*(ptrLeds + i), LOW);
          noTone(pinoAudio);
          
          jogada_efetuada = true;
          while(!digitalRead(*(ptrBotoes + i))); // Espera o botão ser solto
          delay(50);
          break;
        }
      }
    }
    delay(10);
  }

  if (!jogada_efetuada && jogoIniciado && !jogoPausado) {
    Serial.println("Tempo esgotado! Voce nao fez a jogada a tempo.");
    gameOver();
  }
}

void gameOver() {
  Serial.println("GAME OVER!");
  noTone(pinoAudio);
  
  // Ponteiro para o array de LEDs
  int *ptrLeds = pinosLeds;
  int *ptrTons = tons; // Ponteiro para o array de tons

  for(int i=0; i<4; i++) digitalWrite(*(ptrLeds + i), LOW); // Apaga todos os LEDs com ponteiro

  for (int i = 0; i <= 3; i++) {
    tone(pinoAudio, *(ptrTons + i)); // Acessa tons com ponteiro
    digitalWrite(*(ptrLeds + i), HIGH); // Acende LEDs com ponteiro
    delay(200);
    digitalWrite(*(ptrLeds + i), LOW);
    noTone(pinoAudio);
  }

  tone(pinoAudio, *(ptrTons + 3)); // Último tom (ponteiro para tons[3])
  for (int i = 0; i < 5; i++) {
    digitalWrite(*(ptrLeds + 0), HIGH);
    digitalWrite(*(ptrLeds + 1), HIGH);
    digitalWrite(*(ptrLeds + 2), HIGH);
    digitalWrite(*(ptrLeds + 3), HIGH);
    delay(100);
    digitalWrite(*(ptrLeds + 0), LOW);
    digitalWrite(*(ptrLeds + 1), LOW);
    digitalWrite(*(ptrLeds + 2), LOW);
    digitalWrite(*(ptrLeds + 3), LOW);
    delay(100);
  }
  noTone(pinoAudio);
  
  perdeu_o_jogo = true;
} 

void tocarSomDeInicio() {
  // Ponteiro para o array de LEDs
  int *ptrLeds = pinosLeds;
  int *ptrTons = tons; // Ponteiro para o array de tons

  tone(pinoAudio, *ptrTons); // Toca o primeiro tom usando ponteiro (tons[0])
  
  digitalWrite(*ptrLeds, HIGH);     // pinosLeds[0]
  digitalWrite(*(ptrLeds + 1), HIGH); // pinosLeds[1]
  digitalWrite(*(ptrLeds + 2), HIGH); // pinosLeds[2]
  digitalWrite(*(ptrLeds + 3), HIGH); // pinosLeds[3]
  delay(500);
  digitalWrite(*ptrLeds, LOW);
  digitalWrite(*(ptrLeds + 1), LOW);
  digitalWrite(*(ptrLeds + 2), LOW);
  digitalWrite(*(ptrLeds + 3), LOW);
  delay(500);
  noTone(pinoAudio);
}