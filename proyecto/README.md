**INFORMACION DEL PROYECTO** 

El proyecto se encuentra estructurado en una capa de dominios en la cual se encuentran el tablero proncipal y los tipos de datos, esta constituido a su vez por una logica
en donde se encuentra centralizada toda la logica del juego y tambien hay una parte de interfas grafica en donde se encuentran todos los parametros visuales del proyecto de damas chinas.

El proyecto se encuentra dividido en un archivo el cual contiene unas interfases en donde se definen cuales son las acciones o las clases que se tendran en el apartado de
la implementación que en este caso seran los archivos ".CPP".

El proyecto esta hecho en Visual studio comunity 2022, se enviara un archivo *.ZIP* para el funcionamiento del proyecto y su correcto funcionamiento 

**DESARROLLOS INOVADORES**

En el proyecto encontramos integraciones inovadoras como lo son:

1. temporizador por jugador:

 ¿Para qué se implementó?

 Este se implemento con el fin de simular una partida más realista y competitiva, tambien se hace con el fin de hacer que los jugadores hagan movimientos rapidos y de esta manera 
 se logre una partida mas efectiva

 ¿Por qué se consideró necesario?

 Esta implementación se concidero necesaria ya que se le queria dar una experiencia competitiva al usuario en la cual pueda practicar sus habilidades mas eficazmente

 ¿Cómo se llevó a cabo su implementación?

 Esta implementación se llevo a cabo gracias a que se utilizo el sistema de mensajeria de Windows haciendo que exista un pulso constante en el cual es de 1s lo que genera 
 que el tiempo vaya cambiando a medida que avance la partida 

 2. Historial de movimientos y desacer jugada:

 ¿Para qué se implementó?

 Este se incluyo en el proyecto con el fin de darles un registro a los jugadores para que estos puedan analizar su proximo movimiento o analisen un posible moviento de 
 su contricante, el desacer la jugada se implento para ayudar a los jugadores en dado caso se equivoquen.

 ¿Por qué se consideró necesario?

 Se concidero necesaria esta implementación ya que se queria hacer un juego en donde sus partidas sean más intensas y analiticas, de esta manera logrando un avance mas 
 significativo en las avilidades del jugador 

 ¿Cómo se llevó a cabo su implementación?

 Esto se logro gracias a que el programa toma las jugas realizadas para luego añadirlas a una lista visible y para lograr desacer un movimiento el programa toma una 
 captura del tablero y agregandola a una pila, en dado caso en el que el jugador se equivoque este sacara la captura de la pila.

 3. Muestra de coordenadas:

¿Para qué se implementó?

Esto se implemento con el fin de poder dar una guia visual a los jugadores más nuevo en la cual ellos puedan familiarizarce con las diferentes casillas que se 
encuentran en el tablero 

¿Por qué se consideró necesario?

Se concidero necesario ya que se podria generar confuciones al monto en que los jugadores quisieran identificar una posición 

¿Cómo se llevó a cabo su implementación?

La implemntacion de esto se logro gracias a la realizacin de un vector el cual contiene las didtntas posiciones del tablero de juegos y su medida de 10 x 10

4. muestra de posibles movimientos:

¿Para qué se implementó?

Esta se implemnto con el fin de hacer que la partida sea un poco mas rapida y para que los jugadores no se pierdan al momento de mover su pieza 

¿Por qué se consideró necesario?

Se concidero necesario hacer esta implementación ya que puede haber varios jugadores los cuales no conozcan bien los movimiento de las piezas, asi que esta implementacion los ayudara
aprenderse a mover en el tablero

¿Cómo se llevó a cabo su implementación?

Este se implento gracias a que se filtran los movimiento dependiendo si es peon o dama, luego se validan los movimientos posibles, teniendo en cuenta si hay captura obligatoria 
en ese momento o no y luego mostrandolo en la pantalla para finalizar con el movimiento del jugador.


