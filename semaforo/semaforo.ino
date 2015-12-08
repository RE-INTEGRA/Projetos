/*
  semaforo

 Foi usado uma máquina de estados e o timer do arduino para implementar o semaforo

 Máquina de estados:
 estado 1 - semáforo de carro e pedestre funcionando normal;
 estado 2 - semáforo de carro entra em alerta fica neste estado por x segundos;
 estado 3 - semáforo de pedestre foi pressionado e fica neste estado por x segundos.

  repo: https://github.com/RE-INTEGRA/Projetos/semaforo/semaforo.ino
  autor: Joao Campos
 */

 // definindo variáveis do semaforo dos carros
 const int semVerde = 13;
 const int semAmarelo = 12;
 const int semVermelho = 11;
 // definindo variáveis do semaforo do pedestre
 const int semPedestreVerde = 10;
 const int semPedestreVermelho = 9;

// definindo botões

// botão pedestre
 const int botaoPedestre = 7;
 //botão alerta
 const int botaoAlerta = 6;

// variáveis auxiliares
 int estado = 0;
 int temporizador = 0;
 int pedestreEstado = 0;
 int ordem = 0;

 // declarando variável tempo como long porque a variável irá ter um valor muito grande
 unsigned long tempoSemaforoCarro;        // temporizador usado pela tarefa semaforo carro
 unsigned long tempoSemaforoPedestre;        // temporizador usado pela tarefa semaforo pedestre
 unsigned long tempoMudaEstado;       // temporizador usado pelo muda estado

// variáveis usadas para calcular subida do estado do botão 1
int estadoBotao1 = 0;         // estado atual do botao1
int ultimoEstadoBotao1 = 0;     // estado passado do botao1


// variáveis usadas para calcular subida do estado do botão 2
int estadoBotao2 = 0;         // estado atual do botao2
int ultimoEstadoBotao2 = 0;     // estado passado do botao2


// configura botões e semaforos, bem como a porta Serial
void setup() {
  Serial.begin(57600);
  // inicializando leds semaforo como saida
  pinMode(semVerde, OUTPUT);
  pinMode(semAmarelo, OUTPUT);
  pinMode(semVermelho, OUTPUT);

   // inicializando leds semaforo pedestre como saida
  pinMode(semPedestreVerde, OUTPUT);
  pinMode(semPedestreVermelho, OUTPUT);

   // inicializando botões semaforo como entrada
  pinMode(botaoPedestre, INPUT);
  pinMode(botaoAlerta, INPUT);

  tempoSemaforoCarro = millis();
  tempoSemaforoPedestre = millis();
  tempoMudaEstado = millis();



}
// the loop function runs over and over again forever
void loop() {

   int tempoSemaforoCarro = 2000;
   int tempoIntermitenteSemaforoAlerta = 200;
   int tempoSemaforoAlerta = 2000;
   int tempoPedestre = 2000;
   int tempoIntermitentePedestre = 200;
   bool intermitentePedestre = HIGH;

  ordem = 0;
  // semaforo de carro funcionando normal
  while(estado == 0)
    {
     // liga semaforo carro verde, desliga amarelo e vermelho
     // liga semaforo vermelho de pedestre e desliga o verde
     tarefaSemaforoCarro(HIGH, LOW, LOW, tempoSemaforoCarro, 0);
     tarefaSemaforoPedestre(LOW, HIGH, tempoSemaforoCarro, 0, LOW);


    // liga semaforo amarelo e desliga verde
    // liga intermitente verde do pedestre
    tarefaSemaforoCarro(LOW, HIGH, LOW, tempoSemaforoCarro, 1);
    tarefaSemaforoPedestre(LOW, HIGH, tempoSemaforoCarro, 1, LOW);


    // liga semaforo vermelho e desliga amarelo
    tarefaSemaforoCarro(LOW, LOW, HIGH, tempoSemaforoCarro, 2);
    tarefaSemaforoPedestre(HIGH, LOW, tempoSemaforoCarro, 2, LOW);


    if(ordem > 2)
      {
        ordem = 0;
      }
 // envia estado e ordem atual via serial
   tarefaEnviaEstado();

    // botão alerta sobe botão alto, vai para estado 1
   tarefaEstadoBotao1(botaoAlerta, 1);
   // botão alerta sobe botão alto, vai para estado 2
   tarefaEstadoBotao2(botaoPedestre, 2);
  // recebe comandos via serial
   tarefaRecebeComando();
  }

  ordem = 0;
  tempoMudaEstado = millis();
   // botão alerta foi pressionado
 while(estado == 1)
  {
    // coloca em alerta o semafóro por 5s
     tarefaMudaEstado(0,tempoSemaforoAlerta);
     tarefaSemaforoPedestre(LOW, HIGH, tempoSemaforoAlerta, 0, LOW);
     tarefaSemaforoCarro(LOW, LOW, LOW, tempoIntermitenteSemaforoAlerta, 0);
     tarefaSemaforoCarro(LOW, HIGH, LOW, tempoIntermitenteSemaforoAlerta, 1);

     tarefaEnviaEstado();

     if(ordem > 1)
      {
        ordem = 0;
      }
   // botão alerta sobe botão alto, vai para estado 1
   tarefaEstadoBotao1(botaoAlerta, 1);
   // botão alerta sobe botão alto, vai para estado 2
   tarefaEstadoBotao2(botaoPedestre, 2);

  }

  ordem = 0;
  tempoMudaEstado = millis();
  // botão pedestre foi pressionado
  // coloca em vermelho o semafóro por 10s
  while(estado == 2)
  {

     // Muda o semáforo para vermelho
     tarefaMudaEstado(0,tempoPedestre);
     // coloca intermitente as luzes amarelas
    tarefaSemaforoCarro(LOW, LOW, HIGH, tempoPedestre, 0);
    // inicia pedestre verde
    tarefaSemaforoPedestre(HIGH, LOW, 0.75*tempoPedestre, 0, HIGH);
    tarefaSemaforoPedestre(HIGH, LOW, tempoIntermitentePedestre, 1, HIGH);
    tarefaSemaforoPedestre(LOW, LOW, tempoIntermitentePedestre, 2, HIGH);
      if(ordem > 2)
      {
        ordem = 1;
      }

    tarefaEnviaEstado();

        // botão alerta sobe botão alto, vai para estado 1
   tarefaEstadoBotao1(botaoAlerta, 1);
   // botão alerta sobe botão alto, vai para estado 2
   tarefaEstadoBotao2(botaoPedestre, 2);

  }
  ordem = 0;

}

//*********************************
//*********************************
//        Funções
//**********************************
//*********************************



// Tarefa que muda o estado semaforo do carro
// usa variáveis globais ordem, estado e tempo
//
void tarefaSemaforoCarro(bool semaforoVerde, bool semaforoAmarelo, bool semaforoVermelho, int intervalo, int ordemTarefa){
 // pega os segundos atuais do temporizador
   unsigned long miliCorrente = millis();
   // calcula o intervalo
   int intervaloAtual =  miliCorrente - tempoSemaforoCarro;
   if (ordem == ordemTarefa)
   {
     digitalWrite(semVerde, semaforoVerde);
     digitalWrite(semAmarelo, semaforoAmarelo);
     digitalWrite(semVermelho, semaforoVermelho);
   }
 //  delay(1000);
  // verifica se está no estado, ordem atual e já passou o tempo  de execução para executar tarefa
   if(intervaloAtual > intervalo && ordem == ordemTarefa)
   {
        ordem ++;
        tempoSemaforoCarro = millis();
   }

}


// Tarefa que muda o estado semaforo do pedestre
// usa variáveis globais ordem, estado e tempo
//
void tarefaSemaforoPedestre(bool semaforoPedestreVerde, bool semaforoPedestreVermelho, int intervalo, int ordemTarefa, bool incrementaOrdem){
 // pega os segundos atuais do temporizador
   unsigned long miliCorrente = millis();
   // calcula o intervalo
   int intervaloAtual =  miliCorrente - tempoSemaforoPedestre;
   if (ordem == ordemTarefa)
   {
     digitalWrite(semPedestreVerde, semaforoPedestreVerde);
     digitalWrite(semPedestreVermelho, semaforoPedestreVermelho);
   }
  // verifica se está no estado, ordem atual e já passou o tempo  de execução para executar tarefa
   if(intervaloAtual > intervalo && ordem == ordemTarefa)
   {
       if (incrementaOrdem){
        ordem ++;
       }
        tempoSemaforoPedestre = millis();
   }

}


// tarefa temporizada que muda estado com intervalo em ms
void tarefaMudaEstado(int estadoSeguinte, int intervalo){
 // pega os segundos atuais do temporizador
   unsigned long miliCorrente = millis();
   // calcula o intervalo
   int intervaloAtual =  miliCorrente - tempoMudaEstado;

  // verifica se está no estado, ordem atual e já passou o tempo  de execução para executar tarefa
   if(intervaloAtual > intervalo)
   {
      estado = estadoSeguinte;
      ordem = 0;
   }

}


// tarefa que envia estado atual do semaforo e ordem de tarefa atual
// usa variáveis globais estado e ordem
void tarefaEnviaEstado()
{
  // declarar variável string
  String jsonString;
 // construir json {"estado":"value","ordem":"value"};
  jsonString = "{\"estado\":\"";
  jsonString+= estado;
  jsonString+= "\",\"ordem\":\"";
  jsonString+= ordem;
  jsonString+= "\"}";
  // enviar para porta serial
   Serial.println(jsonString);
}

// tarefa que recebe comandos da porta serial
void tarefaRecebeComando()
{
  int serialValue;
 if(Serial.available() > 0)
 {
  serialValue = Serial.read();
  // recebe comando de alerta
  if(serialValue=='a')
    estado = 1;
   // recebe comando de pedestre
  if(serialValue=='p')
    estado = 2;
 }
}


// Deteta se o botão 2 mudou de estado, e muda o estado atual do semaforo
// Funão funciona somente para súbida do estado do botão
void tarefaEstadoBotao1(int porta, int estadoSeguinte){
   estadoBotao1 = digitalRead(porta);

  // comparando o estado atual com o passado
  if (estadoBotao1 != ultimoEstadoBotao1) {

    // botão subiu o estado
    if (estadoBotao1 == HIGH) {
      // inserir função
      estado = estadoSeguinte;
      ordem = 0;
      delay(10);

    }

  }
  // salva o estado do botão
  ultimoEstadoBotao1 = estadoBotao1;
}

void tarefaEstadoBotao2(int porta, int estadoSeguinte){
   estadoBotao2 = digitalRead(porta);

  // comparando o estado atual com o passado
  if (estadoBotao2 != ultimoEstadoBotao2) {

    // botão subiu o estado
    if (estadoBotao2 == HIGH) {
      // inserir função
      estado = estadoSeguinte;
      ordem = 0;
      delay(10);

    }

  }
  // salva o estado do botão
  ultimoEstadoBotao2 = estadoBotao2;
}
