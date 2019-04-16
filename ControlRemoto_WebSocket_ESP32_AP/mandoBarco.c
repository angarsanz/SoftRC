#include <pgmspace.h>
char mando_html_Barco[] PROGMEM = R"=====(

<!--DOCTYPE html-->
<html>

<!--MANDO A DISTANCIAN COCHE TELEDIRIGIDO BASADO EN ESP32, CANVAS Y WEBSOCKET/JSON-->
  <head>
    <title>Mando coche RC/WiFi</title>
    <meta name="viewport" content="width=device-width, minimum-scale=1.0, maximum-scale=1.0, user-scalable=no">
	<style type="text/css">
		body { margin: 0px; overflow: hidden; }
		canvas { border: 0px solid black; }
		
		.button {
		  display: inline-block;
		  padding: 15px 25px;
		  font-size: 24px;
		  cursor: pointer;
		  text-align: center;
		  text-decoration: none;
		  outline: none;
		  color: #fff;
		  background-color: #4CAF50;
		  border: none;
		  border-radius: 15px;
		  box-shadow: 0 9px #999;
		}
		
		.button:hover {background-color: #3e8e41}

		.button:active {
		  background-color: #3e8e41;
		  box-shadow: 0 5px #666;
		  transform: translateY(4px);
		}
		
	</style>

	<script type="text/javascript">

		var canvas;
		var ctx;
		var w = 0;
		var h = 0;
		


		var timer;
		var timer2;
		var updateStarted = false;
		var touches = [];
		
		var valorDireccion=0;
		var valorVelocidad=0;

		var numActiveTouch = 0;
		
		
		
		

		function update() {
		
			if (updateStarted) return;
			updateStarted = true;

			var nw = window.innerWidth;
			var nh = window.innerHeight;

			if ((w != nw) || (h != nh)) {
				w = nw;
				h = nh;
				canvas.style.width = w+'px';
				canvas.style.height = h+'px';
				canvas.width = w;
				canvas.height = h;
				resetCanvas(); //HiDPI

			}

			ctx.clearRect(0, 0, w, h);
			
			valorDireccion=0;
			valorVelocidad=0;
			

			var i, len = touches.length;
			
			ctx.textAlign = 'left'; //determina la alineacion del texto
			
			if(len > 0){ //Si no hay pulascones activas no se muestra nada
			
			//console.log("sin pulsacion");
			}
				for (i=0; i<len; i++) {
					//	console.log(touches.length);
				
				var touch = touches[i];
				var px = touch.pageX;
				var py = touch.pageY;

				
					ctx.beginPath();
					ctx.arc(px, py, 20, 0, 2*Math.PI, true);
					
					ctx.fillStyle = "rgba(0, 0, 200, 100)";
					ctx.fill();



					
					//annade el texto HUD
					
					if(py>nh/2){
						//zona inferior de la pantalla
						
						//calculo valocidad
						var maxVelocidad = (nh/4);
						valorVelocidad = py-((nh/4)*3);
						valorVelocidad = (valorVelocidad*100)/maxVelocidad;
						valorVelocidad *=-1; //hace que para alente el valor sea positivo
						
						
						if(py<nh-(nh/4)){
							//acia adelanta
							//ctx.fillText("F: " +parseInt(py)+ " %", 0, 0+60); //annade texto
							ctx.fillText("F: " +parseInt(valorVelocidad)+ " %", 0, 0+60); //annade texto
						}else{
							//hacia detras
							//ctx.fillText("R: " +parseInt(py)+ " %", 0, 0+60); //annade texto
							ctx.fillText("R: " +parseInt(valorVelocidad)+ " %", 0, 0+60); //annade texto
						}
						
						
						//calculo direccion
						var maxGiro= w/2;
						valorDireccion= px-(w-maxGiro); //reduccion
						valorDireccion= (valorDireccion*100)/maxGiro;
						
						if(px<w-maxGiro){ //hacia Izquierda

							ctx.fillText("L: "+parseInt(valorDireccion)+ " %", w/2, 0+60); //annade texto
						}else{ //hacia Derecha

							ctx.fillText("R: "+parseInt(valorDireccion) +" %", w/2, 0+60); //annade texto
						}
						
						
					}

				}

				//Objetos permanentes
				
			//titulos
			ctx.textAlign = 'center';
			ctx.fillStyle = "rgba(115, 115, 115, 100)";
			ctx.font ="30px Arial"; //annade texto
			ctx.fillText("Velocidad", (nw/4), 30); //annade texto
			ctx.fillText("Direccion", (nw/2)+(nw/4), 30); //annade texto				
				
				
			//pinta una linea para separar las 2 zonas
				
			ctx.lineWidth = 3;
			ctx.strokeStyle = "#f00";
			//ctx.strokeStyle = "rgba(0, 0, 200, 1)";
			ctx.beginPath();
			ctx.moveTo(0,nh/2);
			ctx.lineTo(nw,nh/2);
			
			ctx.stroke();
			
			//enviar(); //Envia los valores de velocidad y direcion (definidos mediante variables globales)

			updateStarted = false;

			
		};


		
		
		function ol() {
			
			initComunicacion();
			
			canvas = document.getElementById('canvas');
			ctx = canvas.getContext('2d');
			timer = setInterval(update, 15); 
			timer2 = setInterval(enviar,50);
			resetCanvas();	//HiDPI

			
			canvas.addEventListener('touchend', function() {
			ctx.clearRect(0, 0, w, h);
			
			numActiveTouch--;
			console.log("Strop: "+ event.targetTouches.length);
			toucles=[];
			touches = event.targetTouches;

			
		});

		canvas.addEventListener('touchmove', function(event) {
			console.log("change: "+event.targetTouches.length);

			touches = event.targetTouches;
			event.preventDefault();


		});

		canvas.addEventListener('touchstart', function(event) {
			numActiveTouch++;
		  	console.log("start: "+ numActiveTouch);

		});
		

		
		canvas.addEventListener('touchleave', function(event) {
			numActiveTouch++;
		  	console.log("touchleave: "+ numActiveTouch);

		});
		

		
		};

		function resetCanvas() {
			// HiDPI canvas adapted from http://www.html5rocks.com/en/tutorials/canvas/hidpi/
			devicePixelRatio = window.devicePixelRatio || 5;
			canvas.width = window.innerWidth * devicePixelRatio;
			canvas.height = window.innerHeight * devicePixelRatio;
			canvas.style.width = window.innerWidth + 'px';
			canvas.style.height = window.innerHeight + 'px';
			ctx.scale(devicePixelRatio, devicePixelRatio);
		}

		
		
		/*********************Funciones para realizar la comunicion*********************/
		
		var ws;
		
		function initComunicacion(){
            if ("WebSocket" in window) {
               
               // Abrimos el socket
                ws = new WebSocket("ws://192.168.1.44:8080/");
                  
                  // Web Socket is connected, send data using send()
               ws.onopen = function () {
					ws.send("Mando conectado"); 
				};

            } else {
              
               // El explorador no soporta webSocket
               alert("El explorador no soporta WebSocket !");
            }
      
	  }
	  
		function enviar(){
		
			var orden ={
						velocidad: parseInt(valorVelocidad),
						direccion: parseInt(valorDireccion),
						luces: 0
			
			};
			

			if(ws.readyState ===1){
				ws.send(JSON.stringify(orden));
				modificarMarcador(true);
			}else{
				console.log("todavia no");
				modificarMarcador(false);
			}
			
			if (ws.readyState === ws.OPEN) {
				// open
				modificarMarcador(true);
			}else{
				modificarMarcador(false);
			}
			
				
		}
	  
		function modificarMarcador(estado){
		
			if(estado==true){
				document.getElementById('IndicadorEstado').innerHTML = '<h4 style="color:green">CONECTADO</h4>';
			}else{
				document.getElementById('IndicadorEstado').innerHTML = '<h4 style="color:red">DESCONECTADO</h4>';
			}
		
		}
		
	</script>
</head>
<body onload="ol()">
	<div id="estadoConexion" style="position:absolute; transform: translate(-50%, -50%); top:47%; left:50%; "> <label id="IndicadorEstado"> </label></div>
	<!--<div id="areaEfectiva" style="position:absolute;;width: 100; height: 100; background-color:gray;"> </div>-->
	<!--<div id="opciones" style="position:absolute"> <button class="button">Luces Posicion</button><button class="button">Bomba agua</button></div>
	-->
	<canvas id="canvas" width="100" height="100" style="top:0px; left:0px; width: 100; height: 100;"></canvas>
	<object id="tuio" type="application/x-tuio">Plugin FAILED to load</object>

</body>
</html>
)=====";
