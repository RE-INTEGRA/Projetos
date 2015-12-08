// biblioteca semaforo para uso com arduino

var events = require('events');

util = require('util');
EventEmitter = require('events');
// requerendo a biblioteca serial
var serialport = require("serialport");
var SerialPort = require("serialport").SerialPort;

var i = 1;
var buffer = "";
/**
 * construtor da classe semaforo
 * @construtor Semaforo
 * @param  {[arrt]} serialPortOptions [recebe as opções da porta serial]
 */
function Semaforo(serialPortOptions) {

  this.serialPort = new SerialPort(serialPortOptions.port, {
    baudrate: serialPortOptions.baudrate,
    parser: serialport.parsers.readline("\n")
  }, false); // false significa que abre imediatamente
  // inicializa a classe semaforo com as propriedades da classe events
  EventEmitter.call(this);

}
// recebe as funções da classe events
util.inherits(Semaforo, EventEmitter);

/**
 * manda comandos para o arduino via serial
 * @method mandaComando
 * @param  {[string]} comando [comando a mandar para o arduino]
 *
 */
Semaforo.prototype.mandaComando = function(comando) {
  var self = this;

  self.serialPort.write(comando);
  console.log(comando + 'lancado');

};

// inicia escuta da porta serial. Todos os dados da porta serial
// são enviados para processamento
/**
 * inicia a escuta da porta serial e processa dados recebidos
 * @method iniciaEscutaDePortaSerial
 *
 */
Semaforo.prototype.iniciaEscutaDePortaSerial = function() {
  var self = this;
  var estado;
  self.serialPort.open(function() {
    self.serialPort.on('data', function(data) {
      buffer = data.toString('ascii');


        estado = self.processaSemaforo(buffer);

    });

  });

};


/**
 * [processa dados vindos do serial e emite eventos de acordo]
 * evento semaforoCarroVerde - estado 0 e ordem 0
 * evento semaforoCarroAmarelo - estado 0 e ordem 1
 * evento semaforoCarroVermelho - estado 0 e ordem 2
 * @method processaSemaforo
 * @param  {[buffer]} buffer [buffer recebido da porta serial]
 *
 */
Semaforo.prototype.processaSemaforo = function(buffer) {
  var self = this;
  var estadoSemaforo = JSON.parse(buffer);


   estadoAtual = estadoSemaforo.estado;
   ordemAtual = estadoSemaforo.ordem;
  // emite eventos para o estado 0 semaforo carro verde
  if (parseInt(estadoSemaforo.estado) === 0 && parseInt(estadoSemaforo.ordem) === 0)
    self.emit("carroVerde");

  // emite evento semaforo carro Amarelo
  if (parseInt(estadoSemaforo.estado) === 0 && parseInt(estadoSemaforo.ordem) === 1)
    self.emit("carroAmarelo");
  // emite evento semaforo carro Verde
  if (parseInt(estadoSemaforo.estado) === 0 && parseInt(estadoSemaforo.ordem) === 2)
    self.emit("carroVermelho");

    // emite eventos para o estado 1, botão alerta pressionado
    if (parseInt(estadoSemaforo.estado) === 1 && parseInt(estadoSemaforo.ordem) === 0)
      self.emit("alertaDesligado");

    // emite evento semaforo carro Amarelo
    if (parseInt(estadoSemaforo.estado) === 1 && parseInt(estadoSemaforo.ordem) === 1)
      self.emit("alertaLigado");


  // emite eventos para o estado 2


};







// Exporta classe Semaforo
module.exports = Semaforo;
