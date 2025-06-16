🧠 Genius Game para Arduino
Bem-vindo ao Genius Game, uma recriação moderna do clássico brinquedo dos anos 80! Prepare-se para testar sua memória e reflexos com este projeto interativo desenvolvido para Arduino.

🎮 Sobre o Jogo
O Genius é um desafio de memória e atenção. O objetivo é simples: o jogo apresenta uma sequência de luzes e sons, e você deve repeti-la na ordem exata. A cada acerto, a sequência aumenta, tornando o desafio progressivamente mais difícil e a diversão ainda maior!

Como se Joga:
Luzes e Sons: O Genius exibirá uma sequência de luzes nos LEDs e tocará seus respectivos sons.
Repita: Memorize a sequência e pressione os botões correspondentes na ordem correta.
Progresso: Se você acertar, a sequência aumentará em um passo na próxima rodada. Se errar... Game Over!
✨ Funcionalidades Incríveis
Este projeto vai além do jogo clássico, oferecendo funcionalidades adicionais para uma experiência mais completa:

📊 Contador de Pontos
Mantenha-se desafiado!

Pontuação Progressiva: A cada rodada concluída com sucesso, 1 ponto é adicionado à sua pontuação total.
Desafio Crescente: Cada nova rodada adiciona um passo à sequência, aumentando a dificuldade e a recompensa.
Acertos Contam: Sua pontuação continua subindo enquanto você acertar a ordem exata das cores.
🕹️ Menu de Opções Interativo
Controle o jogo diretamente pelo Monitor Serial da IDE do Arduino, usando as teclas do seu teclado!

1 - Iniciar Jogo: Comece uma nova partida emocionante a qualquer momento.
2 - Pausar/Despausar Jogo: Precisa de uma pausa estratégica? Pressione 2 para parar o jogo e retome exatamente de onde parou. Quando despausado, o jogo replicará a sequência da rodada atual para você continuar.
3 - Finalizar Jogo: Quer encerrar a partida? Use esta opção para finalizar o jogo e resetar tudo.
4 - Mostrar Pontuação: Curioso para saber quantos pontos você já acumulou? Use esta opção para visualizar sua pontuação atual no Monitor Serial.
Como Perder o Jogo:
O jogo termina quando você comete um erro. Se você pressionar um botão fora da sequência correta, o jogo detecta o erro, aciona o "Game Over" e encerra a partida.

🛠️ Montagem do Hardware
Para montar seu Genius Game, você precisará dos seguintes componentes:

Placa Arduino (Uno, Nano, etc.)
4 LEDs (cores diferentes são recomendadas para melhor experiência)
4 Botões (push buttons)
1 Buzzer (para os sons)
Resistores (para os LEDs e, opcionalmente, para os botões se não usar INPUT_PULLUP)
Jumpers e Protoboard
Conecte os componentes aos pinos do Arduino conforme as definições no código:

Pino de Áudio (Buzzer): Pino 12
Pinos dos LEDs: Pinos 2, 4, 6, 8 (na ordem que preferir para suas cores)
Pinos dos Botões: Pinos 3, 5, 7, 9 (o botão 0 deve corresponder ao LED 0, e assim por diante)
Certifique-se de usar resistores apropriados para seus LEDs (geralmente 220 ohms) para protegê-los. Os botões estão configurados com INPUT_PULLUP no código, o que significa que não precisam de resistores externos de pull-up, apenas conectados diretamente ao GND.

🚀 Como Usar o Código
Baixe o Projeto: Clone este repositório ou baixe o arquivo .ino.
Abra na IDE do Arduino: Abra o arquivo na sua IDE do Arduino.
Configure o Monitor Serial:
No menu Ferramentas (Tools) > Monitor Serial (Serial Monitor).
Defina a Taxa de Comunicação (Baud Rate) para 9600.
MUITO IMPORTANTE: No Monitor Serial, selecione a opção "Sem terminação de linha" (No line ending) no menu suspenso. Isso é crucial para que os comandos do teclado funcionem corretamente.
Faça o Upload: Carregue o código para sua placa Arduino.
Interaja: Use o Monitor Serial e as teclas 1, 2, 3, 4 para controlar o jogo e os botões físicos para jogar!
Divirta-se jogando e desafiando sua memória com este projeto Genius para Arduino! 
