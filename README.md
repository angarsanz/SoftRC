# SoftRC


//ES//

En este repositorio se presenta un mando a distiacnia basado en ESP32 y WebSocket, para utilizarse con cochers RC y un dispositivo con pantalla tactil como mando.


1)Se presenta un codigo para el ide de arduino ".ino", el cual debera ser grabado dentro del esp32. Se pueden configurar varios parametros:

  #configuracion de la red
  #configuracion de la direccion (se configurar haciendo pruebas con el mando y el RC a controlar)
    ---int centroDireccion = xx; //Grados
    ---int direccionMax = xx; // Grados
    ---int direccionMin = xx; //Grados
  

2) En el fuichero "mando.html", se encutra la pagina web que se enviara al dispositivo que se conecte, este codigo se entrega en el ".ino"




/////////En el futuro se irá haciendo el codigo mas generico y modular
