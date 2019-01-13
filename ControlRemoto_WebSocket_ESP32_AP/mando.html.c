#include <pgmspace.h>
char mando_html[] PROGMEM = R"=====(
<!--DOCTYPE html-->
<html>

<!--MANDO A DISTANCIAN COCHE TELEDIRIGIDO BASADO EN ESP32, CANVAS Y WEBSOCKET/JSON-->
  <head>
    <title>Mando coche RC/WiFi</title>
    <meta name="viewport" content="width=device-width, minimum-scale=1.0, maximum-scale=1.0, user-scalable=no">
	<style type="text/css">
		body { margin: 0px; overflow: hidden; }
		canvas { border: 0px solid black; }
	</style>
	<!--<script type="text/javascript" src="magictouch.js"></script>-->
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
					
					if(px < nw/2){
						//Velocidad
						ctx.font ="30px Arial"; //annade texto
						//ctx.fillText(String(px)+","+py, 0, nh-30); //annade texto coordenadas
						
						//determinar velocidad
						var maxVelocidad = h/2;
						//var valorVelocidad;
						
						valorVelocidad = py-(h/2);
						valorVelocidad = (valorVelocidad*100)/maxVelocidad;
						valorVelocidad *=-1; //hace que para alente el valor sea positivo
						
						if(py<(h/2)){ //hacia delante

							ctx.fillText("F: " +parseInt(valorVelocidad)+ " %", 0, 0+60); //annade texto
						}else{ //hacia atras

							ctx.fillText("R: " +parseInt(valorVelocidad)+ " %", 0, 0+60); //annade texto
						}
						
						
					}else{
						//Direccion
						ctx.font ="30px Arial"; //annade texto
						//ctx.fillText(String(px)+","+py, nh/2, nh-30); //annade texto coordenadas

						//Determinar giro	
						var maxGiro= w/4;	
						//var valorDireccion;
						
						if(px<w-(w/4)){ //hacia Izquierda
							valorDireccion= px-(w-(w/4)); //reduccion
							valorDireccion= (valorDireccion*100)/maxGiro;
							ctx.fillText("L: "+parseInt(valorDireccion)+ " %", w/2, 0+60); //annade texto
						}else{ //hacia Derecha
							valorDireccion= px-(w-(w/4));
							valorDireccion= (valorDireccion*100)/maxGiro;
							ctx.fillText("R: "+parseInt(valorDireccion) +" %", w/2, 0+60); //annade texto
						}
						
					}
					//Numero de pulsaciones activas
					//ctx.font ="30px Verdana"; //annade texto
					//ctx.fillText(numActiveTouch, nw/2, 200); //annade texto
					
			
					
					//console.log('drawn circle at ' + px +',' + py);
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
			ctx.moveTo(nw/2, 0);
			ctx.lineTo(nw/2, nh);
			
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
		/*	
			var evento = ['touchstart', 'touchmove', 'touchend', 'touchcancel'];
			
			for(var i=0;i<evento.length;i++){
			
				canvas.addEventListener(evento[i], posicionPulso, false);
			}
			*/
			
		canvas.addEventListener('touchend', function() {
			ctx.clearRect(0, 0, w, h);
			
			numActiveTouch--;
			console.log("Strop: "+ event.targetTouches.length);
			toucles=[];
			touches = event.targetTouches;
			//toucles=[];
			
		});

		canvas.addEventListener('touchmove', function(event) {
			console.log("change: "+event.targetTouches.length);
		//	console.log(event.changedTouches[0].pageX);
			//touches = event.touches;
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
		/*
		function posicionPulso(e){
			console.log("touchleave: "+ e.targetTouches.length);
			for(var i=0;i<e.targetTouches.length;i++){
				//touches = e.targetTouches[i];
				touches.push(e.targetTouches[i]);
			}
		//event.preventDefault();
			//window.requestAnimationFrame(update);
		}
*/
		
		
		/*********************Funciones para realizar la comunicion*********************/
		
		var ws;
		
		function initComunicacion(){
            if ("WebSocket" in window) {
               
               // Abrimos el socket
                ws = new WebSocket("ws://192.168.4.1:8080/");
                  
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
			
			//var num=9;
			//var orden ={velocidad: num ,direccion:2};
			if(ws.readyState ===1){
				ws.send(JSON.stringify(orden));
			}else{
				console.log("todavia no");
			}
				
		}
	  
		
		
	</script>
</head>
<body onload="ol()">

	<canvas id="canvas" width="100" height="100" style="top:0px; left:0px; width: 100; height: 100;"></canvas>
	<object id="tuio" type="application/x-tuio">Plugin FAILED to load</object>

</body>
</html>
)=====";