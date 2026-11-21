// Rocks - Juego de ejemplo para Fenix
// ----------------------------------
//
// (c) 2000 José Luis Cebrián 
//
// Este juego se considera de dominio público. Puedes hacer lo que quieras
// con él y con su código, incluyendo modificarlo, redistribuirlo, o
// utilizarlo en tus propios programas de cualquier manera.

PROGRAM Rocks;

GLOBAL
	// Estas variables contienen los códigos de los gráficos
	// empleados en el juego

	INT grafico_bloque;
	INT grafico_bob1;
	INT grafico_bob2;

	// A continuación, los sonidos

	INT wav_slam;
	INT wav_bloink;
	INT module;

	// Esta variable estará a 1 cuando los bloques deban detenerse
	// (tras la muerte o tras pasar de nivel)

	parar_bloques;

	// Distintas opciones de dificultad

	INT frecuencia = 200;
	INT frames_pausa = 10;
	INT velocidad_caida = 4;
	INT velocidad_salida = 1;

	// La siguiente tabla contiene la coordenada Y del último bloque
	// depositado sobre cada columna. Por defecto se inicializa al
	// máximo (188) y va decreciendo a medida que se apilan bloques

	INT altura[12];

	INT score;
	INT nivel;
END

// Proceso de inicialización

PROCESS Inicio()
BEGIN
	FROM x = 0 TO 12: altura[x] = 188; END
	clear_screen();
	FROM x = 0 TO 12: bloque(x); END
	score = 0;
	parar_bloques = 40;
	write_int (0, 314, 8, 5, &score);
	write     (0, 264, 8, 5, "Puntuación:");
	write_int (0,  64, 8, 5, &nivel);
	write     (0,   8, 8, 3, "Nivel:");
	Bob(12,12,100);
	Vida(120,8);
	Vida(100,8);
	Vida( 80,8);
	play_song(module, -1);
END

// Proceso de muerte

PROCESS Muerte()
PRIVATE
	proc;
BEGIN
	play_wav (wav_bloink, 0);

	// Obtiene el proceso protagonista (DEBE existir)
	proc = get_id(TYPE Bob);

	// Muestra diferentes miniaturas
	Bobby (proc.x, proc.y, -300, -100);
	Bobby (proc.x, proc.y,  300, -100);
	Bobby (proc.x, proc.y, -200, -200);
	Bobby (proc.x, proc.y,  200, -200);
	Bobby (proc.x, proc.y, -100, -400);
	Bobby (proc.x, proc.y,  100, -400);
	Bobby (proc.x, proc.y, -100,    0);
	Bobby (proc.x, proc.y,  100,    0);

	// Elimina al protagonista
	signal (proc, S_KILL);

	// Espera a que no queden miniaturas en pantalla
	WHILE get_id(TYPE Bobby): FRAME; END

	proc = get_id(TYPE Vida);
	IF proc:
		parar_bloques = 40;
		FROM x = 0 TO 20: FRAME; END
		Bob(proc.x, proc.y, 50);
		signal (proc, S_KILL);
	ELSE
		// Hace una pausa
		FROM x = 0 TO 40: FRAME; END
		// Reinicia el juego
		Menu();
	END
END

// Rutina que pone el nivel de dificultad

PROCESS PonerNivel(int n)
BEGIN
	nivel = n;

	IF n == 1:
		frecuencia = 250;
		frames_pausa = 10;
		velocidad_caida = 2;
		velocidad_salida = 1;
	END
	IF n == 2:
		frecuencia = 225;
		frames_pausa = 10;
		velocidad_caida = 2;
		velocidad_salida = 2;
	END
	IF n == 3:
		frecuencia = 200;
		frames_pausa = 5;
		velocidad_caida = 3;
		velocidad_salida = 1;
	END
	IF n == 4:
		frecuencia = 175;
		frames_pausa = 0;
		velocidad_caida = 4;
		velocidad_salida = 2;
	END
	IF n == 5:
		frecuencia = 150;
		frames_pausa = 20;
		velocidad_caida = 6;
		velocidad_salida = 6;
	END
	IF n == 6:
		frecuencia = 125;
		frames_pausa = 10;
		velocidad_caida = 6;
		velocidad_salida = 8;
	END
	IF n == 7:
		frecuencia = 90;
		frames_pausa = 0;
		velocidad_caida = 4;
		velocidad_salida = 4;
	END
	IF n == 8:
		frecuencia = 90;
		frames_pausa = 0;
		velocidad_caida = 8;
		velocidad_salida = 1;
	END
	IF n == 9:
		frecuencia = 90;
		frames_pausa = 10;
		velocidad_caida = 8;
		velocidad_salida = 8;
	END
END

// Menú de opciones

PROCESS Menu()
BEGIN
	// Muestra los textos del menú. Normalmente es preferible utilizar
	// un gráfico con put_screen o al menos un tipo de letra propio
	delete_text(0);
	let_me_alone();
	frame;
	put (0, -1, 0, 0);
	play_song(module, -1);
	write (0, 160, 60, 4, "Nivel de dificultad");
	write (0, 100, 80, 3, "1. Muy Fácil");
	write (0, 100, 90, 3, "2. Fácil");
	write (0, 100,100, 3, "3. Normal");
	write (0, 100,110, 3, "4. Difícil");
	write (0, 100,120, 3, "5. Muy difícil");
	write (0, 100,140, 3, "0. Salir");
	drawing_map (0, 0);
	drawing_color (rgb(255,255,128));
	draw_box (0, 50, 320, 51);
	draw_box (0, 151, 320, 152);
	drawing_color (rgb(128,128,64));
	draw_box (0, 52, 320, 150);

	// Pone un titulillo
	graph = write_in_map (0, "Rocks", 4);
	size = 200;
	x = 160;
	y = 20;

	// Recoge la entrada por teclado
	LOOP
		scan_code = 0;
		FRAME;

		IF key(_0) or scan_code == _ESC: exit(0,0); END

		// Establece las diferentes opciones según el nivel
		// de difucultad

		IF key(_1):
			PonerNivel(1);
			BREAK;
		END
		IF key(_2):
			PonerNivel(2);
			BREAK;
		END
		IF key(_3):
			PonerNivel(3);
			BREAK;
		END
		IF key(_4):
			PonerNivel(4);
			BREAK;
		END
		IF key(_5):
			PonerNivel(5);
			BREAK;
		END
		IF key(_6):
			PonerNivel(6);
			BREAK;
		END
		IF key(_7):
			PonerNivel(7);
			BREAK;
		END
		IF key(_8):
			PonerNivel(8);
			BREAK;
		END
		IF key(_9):
			PonerNivel(9);
			BREAK;
		END
	END

	delete_text(0);
	Inicio();
END

// Rutina que incremente el score

PROCESS MasScore(int n)
BEGIN
	score += n;
	IF ((score % 1000) < n):
		parar_bloques = 80;
		PonerNivel(++nivel);
	END
END

// ----------------------------------------------------------------------
// Proceso de bloque que cae por una columna determinada

PROCESS Bloque(columna)
PRIVATE
	INT n, mapa;
BEGIN
	graph = grafico_bloque;
	x = columna*24+12;

	LOOP
		y = -12;

		// Los bloques pueden estar detenidos
		IF parar_bloques > 0: FRAME; END

		// Hace una pausa aleatoria
		FROM n = rand(0,frecuencia) TO 0 STEP -1: 
			FRAME; 
			IF parar_bloques > 0: BREAK; END
		END

		// Los bloques pueden estar detenidos
		IF parar_bloques == 0: 

			play_wav (wav_slam, 0);

			// Aparece en pantalla poco a poco
			WHILE y <= 12: 
				y += velocidad_salida;
				FRAME; 
			END

			// Hace una segunda pausa
			FROM n = 0 TO frames_pausa: FRAME; END

			// Cae hasta la posición final
			WHILE y < altura[columna]:
				y += velocidad_caida;
				IF collision(TYPE Bob) AND !get_id(TYPE Muerte): 
					Muerte(); 
				END
				FRAME;
			END

			// Dibuja el bloque en la posición adecuada
			altura[columna] -= 24;
			PUT (0, graph, x, y);

			// 10 puntos por bloque caído
			IF get_id(TYPE Bob): MasScore(10); END
		END

		// Hace que descienda la columna
		WHILE altura[columna] < 92 OR 
			(altura[columna] < 188 AND parar_bloques > 0) OR
			(altura[columna] < 164 AND rand(0,3) > 1):

			// Para hacer descender la columna utilizamos un 
			// truquillo: Creamos un nuevo mapa en memoria donde 
			// capturamos la parte del fondo de pantalla que 
			// corresponde a la columna y lo dibujamos en modo 
			// de ignorar el color 0 de forma que sobreescriba 
			// el fondo cada vez

			mapa = new_map(24,200,16);
			set_center (0, mapa, 0, 0);
			map_block_copy(0,mapa,0,0,0,columna*24,0,24,200, 0);
			y = -12;
			FROM n = 0 TO 24 STEP 2:
				XPUT (0, mapa, columna*24, n, 0, 100, 128, 0);
				FRAME; 
				altura[columna] += 2;
			END
			altura[columna] -= 2;
			unload_map(0,mapa);
		END
	END
END

// ----------------------------------------------------------------------
// Proceso de las vidas

PROCESS Vida(x, y)
BEGIN
	size = 50;
	// Usamos un temporizador para controlar el movimiento
	// y la animación de las vidas en miniatura
	timer[1] = 0;
	LOOP
		IF ((timer[1] % 50) > 25)
			graph = grafico_bob2;
		ELSE
			graph = grafico_bob1;
		END
		IF (timer[1] > 120)
			flags = 1; 	// a la izquierda
			x--;
		ELSE
			flags = 0;	// a la derecha
			x++;
		END
		FRAME ;
		IF key(_ESC): Menu(); END
		IF timer[1] > 240: timer[1] = 0; END
	END
END

// ----------------------------------------------------------------------
// Proceso del protagonista

PROCESS Bob(x, y, size)
PRIVATE
	INT columna1, columna2, prex, salto, nframe;
BEGIN
	x -= (x%4) ;
	LOOP
		// Crece automáticamente (si viene de una vida)
		IF size < 100: size += 2; END

		// Sólo por estar vivo, la puntuación se incrementa
		IF timer[0] >= 40:
			timer[0] = 0;
			MasScore(1);
		END

		// La parada de bloques es temporal
		IF parar_bloques > 0: parar_bloques--; END

		// Anima el gráfico
		graph = grafico_bob1;
		IF nframe > 10: graph = grafico_bob2; END
		IF nframe > 20: nframe = 0; END

		// Averigua las 2 columnas sobre las que se encuentra
		// Este cálculo se emplea varias veces
		columna1 = (x-12)/24;
		columna2 = (x+11)/24;

		// Actualiza la altura
		IF salto > 0:
			// Hay un salto en curso
			y -= salto;
			salto = salto-1;
			IF !key(_SPACE) && !key(_UP): salto = 0; END
			IF salto == 0: y-=(y%4); END
		ELSE 
			IF y < altura[columna1] AND y < altura[columna2]:
				// No hay suelo bajo el protagonista
				y += 4;
				IF collision(TYPE Bloque): 
					y -= 4; 
					IF key(_SPACE) or key(_UP): salto = 12; END
				END
			ELSE
				// Es posible saltar sólo si hay suelo
				IF key(_SPACE) or key(_UP): salto = 12; END
			END
		END

		// Actualiza la coordenada X en función del teclado
		prex = x;
		IF key(_left):  x-=4; flags = 1; nframe++; END
		IF key(_right): x+=4; flags = 0; nframe++; END
		IF x < 12: x = 12; END
		IF x > 12+24*12: x = 12+24*12; END

		// Averigua si la nueva coordenada X entra en colisión
		// con alguna columna o bloque; si así fuera, no
		// permite realizar el movimiento y deja X como estaba
		IF x <> prex:
			columna1 = (x-11)/24;
			columna2 = (x+11)/24;
			IF y > altura[columna1] OR y > altura[columna2]:
				x = prex; 
			ELSE
				IF collision(TYPE Bloque):
					x = prex;
				END
			END
		END

		FRAME;
	END
END

// ----------------------------------------------------------------------
// "Bobby": versiones en miniatura del protagonista que salen despedidas
// según la velocidad recibida como parámetro. Se usan al morir.

PROCESS Bobby (x, y, velx, vely)
BEGIN
	graph = grafico_bob1;
	size = 25;
	resolution = 100;
	x *= 100; y *= 100;

	WHILE y < 20000:
		y += vely;
		x += velx;
		vely += 25;
		angle += 2000;
		FRAME;
	END
END

// ----------------------------------------------------------------------

BEGIN
	// Inicialización
	graph_mode = MODE_16BITS;
	full_screen = FALSE;
	sound_freq = 44100;
	set_fps(40, 0);

	// Carga de gráficos
	load_fpg("rocks.fpg");
	grafico_bob1   = 1;
	grafico_bob2   = 2;
	grafico_bloque = 3;

	// Carga de sonidos
	wav_slam   = load_wav("slam.wav");
	wav_bloink = load_wav("bloink.wav");

	module = load_song("badmoon.mod");

	Menu();
END
