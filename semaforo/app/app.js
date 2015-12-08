/*
app de semáforo
As funcionalidades e materiais do projeto estão descritas em README.md
github: https://github.com/RE-INTEGRA/Projetos/semaforo/
autor: João Campos
 */

var express = require('express');
var path = require('path');
var app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var bodyParser = require('body-parser');
var multer = require('multer'); // v1.0.5
var upload = multer(); // for parsing multipart/form-data
// requerendo semaforo da biblioteca local
var Semaforo = require('./lib/semaforo.js');

// inicia classe semaforo com os eventos da porta serial
var semaforo = new Semaforo({port:'COM8',baudrate:57600});
// requerendo semaforo da biblioteca local
semaforo.iniciaEscutaDePortaSerial();

http.listen(3000);


app.use(bodyParser.json()); // for parsing application/json
app.use(bodyParser.urlencoded({ extended: true })); // for parsing application/x-www-form-urlencoded

// assets in public directory
app.use('/public', express.static(__dirname + '/public'));
 // servir index html
 app.get('/', function (req, res) {
   res.sendFile(__dirname + '/index.html');
 });

// Na ligação do socket, recebe os eventos
io.on('connection', function(socket){
  console.log('socket ligado');
  semaforo.on('carroVerde',function (argument) {
    socket.emit('carroVerde');
  });
  semaforo.on('carroAmarelo',function (argument) {
    socket.emit('carroAmarelo');
  });
  semaforo.on('carroVermelho',function (argument) {
    socket.emit('carroVermelho');
  });
  semaforo.on('alertaLigado',function (argument) {
    socket.emit('alertaLigado');
  });
  semaforo.on('alertaDesligado',function (argument) {
    socket.emit('alertaDesligado');
  });


// botão alerta foi pressionado
  socket.on('alertaAcionado', function(){
    console.log('alerta acionado');
    semaforo.mandaComando('a');
  });
// botão alerta foi pressionado
  socket.on('pedestreAcionado', function(){
      console.log('pedestreAcionado');
      semaforo.mandaComando('p');
    });


});
