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
//Vamos começar definindo as notas para os sons
#define NOTE_D4  294
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_A5  880

// criando o array para os 4 sons para sortear um som
int tons[4] = { NOTE_A5, NOTE_A4, NOTE_G4, NOTE_D4 };
// Nossa sequência de até 100 itens vai começar vazia.
int sequencia[100] = {};
// Indica a rodada atual que o jogo se encontra.
int rodada_atual = 0;
// Indica o passo atual dentro da sequência, é usado enquanto a sequência
// está sendo reproduzida.
int passo_atual_na_sequencia = 0;

/*
 * Indica o pino de áudio, leds e botões.
 * Os pinos de leds e botões estão em ordem, relacionados uns aos outros, ou
 * seja, o primeiro led está relacionado ao primeiro botão. A ordem destas
 * sequências também estão relacionadas a ordem dos tons.
 */
int pinoAudio = 12;
int pinosLeds[4] = { 2, 4, 6, 8 };
int pinosBotoes[4] = { 3, 5, 7, 9 };

// Indica se um botão foi pressionado durante o loop principal.
int botao_pressionado = 0;
// Flag indicando se o jogo acabou.
int perdeu_o_jogo = false;
// Variável para armazenar a pontuação.
int score = 0;

// Variáveis de controle para o estado do jogo (novas para o menu)
bool jogoIniciado = false;
bool jogoPausado = false;

// --- Funções Auxiliares para o Jogo ---

// Função para resetar o estado do jogo
void resetarJogo() {
  for (int i = 0; i < 100; i++) {
    sequencia[i] = 0; // Zera a sequência
  }
  rodada_atual = 0;
  passo_atual_na_sequencia = 0;
  perdeu_o_jogo = false;
  score = 0; // Reseta a pontuação
  jogoIniciado = false; // Define que o jogo não está mais iniciado
  jogoPausado = false;  // Garante que não está pausado
  noTone(pinoAudio); // Garante que nenhum som esteja tocando
  for(int i=0; i<4; i++) digitalWrite(pinosLeds[i], LOW); // Garante que todos os LEDs estejam apagados
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

// Função para processar comandos do serial (nova ou modificada para ser reusável)
void processarComandoSerial(char key) {
  // Limpa o buffer serial, caso haja mais caracteres (como \n)
  delay(10); // Pequeno atraso para garantir que apenas um caractere seja lido por vez
  while (Serial.available()) { 
    Serial.read();
  }

  switch (key) {
    case '1':
      if (!jogoIniciado) {
        Serial.println("Comando: Iniciar Jogo");
        resetarJogo(); // Reseta tudo antes de iniciar um novo jogo
        jogoIniciado = true;
        tocarSomDeInicio(); // Toca o som de início
        Serial.println("Jogo iniciado! Boa sorte!");
      } else {
        Serial.println("O jogo já está em andamento. Para reiniciar, use '3' e depois '1'.");
      }
      break;
    case '2':
      if (jogoIniciado) {
        jogoPausado = !jogoPausado; // Alterna o estado de pausa
        if (jogoPausado) {
          Serial.println("Comando: Jogo Pausado.");
          noTone(pinoAudio); // Para qualquer som que esteja tocando
          for(int i=0; i<4; i++) digitalWrite(pinosLeds[i], LOW); // Apaga todos os LEDs
          digitalWrite(pinosLeds[0], HIGH); // Feedback visual de pausa (primeiro LED aceso)
        } else {
          Serial.println("Comando: Jogo Despausado.");
          digitalWrite(pinosLeds[0], LOW); // Apaga o LED de pausa
        }
      } else {
        Serial.println("O jogo não está em andamento para ser pausado/despausado.");
      }
      break;
    case '3':
      if (jogoIniciado || perdeu_o_jogo) { 
        Serial.println("Comando: Finalizar Jogo (resetar).");
        gameOver(); // Executa a rotina de Game Over para feedback visual/sonoro
        resetarJogo(); // Reseta todas as variáveis do jogo
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
  exibirMenu(); // Reexibe o menu após um comando
}

void setup() {
  for (int i = 0; i <= 3; i++) {
    pinMode(pinosLeds[i], OUTPUT);
    pinMode(pinosBotoes[i], INPUT_PULLUP);
  }
  pinMode(pinoAudio, OUTPUT);
  randomSeed(analogRead(0));
  Serial.begin(9600);
  exibirMenu(); // Exibe o menu na inicialização
}

void loop() {
  // Sempre verifica e processa comandos serial no início do loop
  if (Serial.available()) {
    char key = Serial.read();
    processarComandoSerial(key); // Chama a nova função para lidar com os comandos
  }

  // --- Lógica Principal do Jogo Genius ---
  // O jogo só roda se estiver iniciado e não estiver pausado
  if (jogoIniciado && !jogoPausado) {
    if (perdeu_o_jogo) {
      Serial.println("Você perdeu! Pressione '3' para finalizar/resetar, ou '1' para iniciar uma nova partida.");
      jogoIniciado = false; // Desativa o jogo para esperar um novo comando
      delay(2000); 
      return; // Sai do loop atual para aguardar o menu
    }
    
    Serial.print("Rodada: ");
    Serial.println(rodada_atual + 1); 

    proximaRodada();
    reproduzirSequencia();
    // Verifica se o jogo ainda está ativo após reproduzir a sequência (pode ter sido pausado/finalizado)
    if (!jogoIniciado || jogoPausado) return; // Sai se o jogo foi interrompido

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
            processarComandoSerial(key); // Permite processar comandos durante o delay
            if (!jogoIniciado || jogoPausado) break; // Sai do loop de delay se o jogo foi interrompido
        }
        delay(10); // Pequeno delay para economizar CPU
    }

  } else if (jogoIniciado && jogoPausado) {
    // Se o jogo está pausado, podemos dar um feedback visual leve aqui
    digitalWrite(pinosLeds[0], !digitalRead(pinosLeds[0])); // Pisca o LED de status (LED 0)
    delay(500); // Ritmo da piscada
  } else {
    // Jogo não iniciado, apenas aguarda comandos.
    delay(50); 
  }
}

// Sorteia um novo item e adiciona na sequência.
void proximaRodada() {
  int numero_sorteado = random(0, 4);
  sequencia[rodada_atual++] = numero_sorteado;
}

// Reproduz a sequência para ser memorizada.
void reproduzirSequencia() {
  for (int i = 0; i < rodada_atual; i++) {
    // Adiciona verificação de pausa/finalização dentro do loop de reprodução
    if (!jogoIniciado || jogoPausado) {
      noTone(pinoAudio); // Garante que o som pare
      for(int j=0; j<4; j++) digitalWrite(pinosLeds[j], LOW); // Apaga LEDs
      return; // Sai da função de reprodução
    }

    tone(pinoAudio, tons[sequencia[i]]);
    digitalWrite(pinosLeds[sequencia[i]], HIGH);
    
    unsigned long tempoFinalNota = millis() + 500; // Duração da nota
    while (millis() < tempoFinalNota && !jogoPausado && jogoIniciado) {
        if (Serial.available()) { // Verifica comandos serial durante a nota
            char key = Serial.read();
            processarComandoSerial(key);
            if (!jogoIniciado || jogoPausado) break; // Sai se o jogo foi interrompido
        }
        delay(10); // Pequeno delay para economizar CPU
    }
    
    noTone(pinoAudio);
    digitalWrite(pinosLeds[sequencia[i]], LOW);

    unsigned long tempoFinalPausa = millis() + 100; // Pausa entre notas
    while (millis() < tempoFinalPausa && !jogoPausado && jogoIniciado) {
        if (Serial.available()) { // Verifica comandos serial durante a pausa
            char key = Serial.read();
            processarComandoSerial(key);
            if (!jogoIniciado || jogoPausado) break; // Sai se o jogo foi interrompido
        }
        delay(10); // Pequeno delay
    }
    if (!jogoIniciado || jogoPausado) return; // Sai se o jogo foi interrompido
  }
  noTone(pinoAudio); // Garante que o som pare após a sequência
}

// Aguarda o jogador iniciar sua jogada.
void aguardarJogador() {
  for (int i = 0; i < rodada_atual; i++) {
    // Verifica antes de cada jogada se o jogo está ativo
    if (!jogoIniciado || perdeu_o_jogo || jogoPausado) {
      break; 
    }
    
    aguardarJogada();
    
    // Verifica novamente o estado do jogo após aguardar a jogada
    if (!jogoIniciado || perdeu_o_jogo || jogoPausado) {
      break; 
    }
    
    // verifica a jogada   
    if (sequencia[passo_atual_na_sequencia] != botao_pressionado) {
      gameOver(); // perdeu
      break; // Sai do loop para a lógica de game over
    }
    
    passo_atual_na_sequencia++;
  }

  // Redefine a variável para 0 após a rodada (seja sucesso ou falha)
  passo_atual_na_sequencia = 0;
}

void aguardarJogada() {
  boolean jogada_efetuada = false;
  unsigned long tempoLimite = millis() + 5000; // 5 segundos para a jogada (ajustável)

  // O loop continua enquanto não houver jogada, não exceder o tempo E o jogo estiver iniciado e não pausado
  while (!jogada_efetuada && millis() < tempoLimite && jogoIniciado && !jogoPausado) { 
    // --- Verificação de comandos serial durante a espera da jogada ---
    if (Serial.available()) {
      char key = Serial.read();
      processarComandoSerial(key); // Chama a função para processar o comando
      // Se o jogo foi pausado ou finalizado pelo comando, saímos do loop de espera da jogada
      if (jogoPausado || !jogoIniciado) return; 
    }
    // --- Fim da verificação de comandos serial ---

    // Lógica para aguardar os botões físicos
    for (int i = 0; i <= 3; i++) {
      if (!digitalRead(pinosBotoes[i])) { // Se o botão for pressionado
        delay(50); // Debounce
        if (!digitalRead(pinosBotoes[i])) { // Confirma a leitura
          botao_pressionado = i;
          tone(pinoAudio, tons[i]);
          digitalWrite(pinosLeds[i], HIGH);
          delay(300); 
          digitalWrite(pinosLeds[i], LOW);
          noTone(pinoAudio);
          
          jogada_efetuada = true;
          while(!digitalRead(pinosBotoes[i])); // Aguarda o botão ser solto
          delay(50); // Debounce após soltar
          break; // Sai do for (um botão foi pressionado)
        }
      }
    }
    delay(10); // Pequeno atraso no loop while principal para não sobrecarregar a CPU
  }

  // Se o tempo limite foi atingido e nenhuma jogada foi efetuada, considera como erro
  // Só aplica se o jogo ainda estiver iniciado e não estiver pausado ao sair do loop while
  if (!jogada_efetuada && jogoIniciado && !jogoPausado) {
    Serial.println("Tempo esgotado! Voce nao fez a jogada a tempo.");
    gameOver();
  }
}

void gameOver() {
  // GAME OVER.
  Serial.println("GAME OVER!");
  noTone(pinoAudio); // Garante que qualquer som pare
  for(int i=0; i<4; i++) digitalWrite(pinosLeds[i], LOW); // Garante que todos os LEDs estejam apagados

  // Sequência rápida de sons e luzes para indicar a perda
  for (int i = 0; i <= 3; i++) {
    tone(pinoAudio, tons[i]);
    digitalWrite(pinosLeds[i], HIGH);
    delay(200);
    digitalWrite(pinosLeds[i], LOW);
    noTone(pinoAudio);
  }

  // Piscada de todos os LEDs e um som contínuo
  tone(pinoAudio, tons[3]); // Mantém um tom grave
  for (int i = 0; i < 5; i++) { // Pisca 5 vezes
    digitalWrite(pinosLeds[0], HIGH);
    digitalWrite(pinosLeds[1], HIGH);
    digitalWrite(pinosLeds[2], HIGH);
    digitalWrite(pinosLeds[3], HIGH);
    delay(100);
    digitalWrite(pinosLeds[0], LOW);
    digitalWrite(pinosLeds[1], LOW);
    digitalWrite(pinosLeds[2], LOW);
    digitalWrite(pinosLeds[3], LOW);
    delay(100);
  }
  noTone(pinoAudio); // Para o som após a sequência
  
  perdeu_o_jogo = true; // Define a flag de "perdeu"
  // O reset completo das variáveis é feito no loop principal ou via comando '3' do menu
} 

void tocarSomDeInicio() {
  tone(pinoAudio, tons[0]); // Um tom
  // Acende todos os LEDs
  digitalWrite(pinosLeds[0], HIGH);
  digitalWrite(pinosLeds[1], HIGH);
  digitalWrite(pinosLeds[2], HIGH);
  digitalWrite(pinosLeds[3], HIGH);
  delay(500); // Mantém ligado por meio segundo
  // Apaga todos os LEDs
  digitalWrite(pinosLeds[0], LOW);
  digitalWrite(pinosLeds[1], LOW);
  digitalWrite(pinosLeds[2], LOW);
  digitalWrite(pinosLeds[3], LOW);
  delay(500); // Meio segundo de silêncio
  noTone(pinoAudio); // Para o som
}