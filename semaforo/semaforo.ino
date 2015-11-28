/*
  semaforo
  
 Foi usado uma máquina de estados para implementar o semaforo e usado o timer
 do arduino. 


 
  repo: https://github.com/RE-INTEGRA/Projetos/semaforo/semaforo.ino
  autor: Joao Campos
 */

 // definindo variáveis do semaforo dos carros
 const int semVerde = 13;
 const int semAmarelo = 12;
 const int semVermelho = 11;

// definindo botões

// botão pedestre 
 const int botaoPedestre = 7;
 //botão alerta
 const int botaoAlerta = 6;

// variáveis auxiliares
 int estado = 0;
 int temporizador = 0;
 int pedestreEstado = 0; 
 int *ordem = 0;

 // Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated

// the setup function runs once when you press reset or power the board
void setup() {
  // inicializando leds semaforo como saida
  pinMode(semVerde, OUTPUT);
  pinMode(semAmarelo, OUTPUT);
  pinMode(semVermelho, OUTPUT);
  
    // inicializando botões semaforo como entrada
  pinMode(botaoPedestre, INPUT);
  pinMode(botaoAlerta, INPUT);
    
}
// the loop function runs over and over again forever
void loop() {

  tempo = millis();
  
  // semaforo funcionando normal
  while(estado == 0)
  {
   
   tarefaMudaPorta(semVerde,HIGH,tempo,1000,estado, 0, &ordem,0)
   tarefaMudaPorta(semAmarelo,LOW,tempo,1000, estado,0,&ordem,0)

   tarefaMudaPorta(semAmarello,HIGH,tempo,1000,estado, 0, &ordem,1)
   tarefaMudaPorta(semVerde,LOW,tempo,1000, estado,0,&ordem,1)

   tarefaMudaPorta(semVermelho,HIGH,tempo,1000,estado, 0, &ordem,2)
   tarefaMudaPorta(semAmarelo,LOW,tempo,1000, estado,0,&ordem,2)

  

  if(*ordem > 2)
  {
    *ordem = 0;
  }
   
   pedestreEstado = digitalRead(botaoPedestre);  
   if ( pedestreEstado == HIGH) {     
    // turn LED on:    
    estado = 1; 
  } 
   
   
  }
  
 
}

// Tarefa que muda o estado da porta baseado no tempo, e no estado atual
void tarefaMudaPorta(int porta, bool estadoPorta, unsigned long tempo, int intervalo, int estadoAtual, int estado, int* ordemAtual, int ordem){
 // pega os segundos atuais do temporizdor
   unsigned long miliCorrente = millis();
   int intervaloAtual =  abs(tempo - miliCorrente);

  // verifica se está no estado, ordem atual e já passou o tempo  de execução para executar tarefa
   if(intervaloAtual > tempo && estado == estadoAtual && ordem == ordemAtual)
   {
      digitalWrite(porta, estadoPorta);
      *ordemAtual ++;
   }
  
}




