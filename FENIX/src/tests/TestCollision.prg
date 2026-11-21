/*
 *	Prueba de chequeo de colisión
 *	-----------------------------
 *	Las posiciones, tamaño y flags de los gráficos son aleatorias.
 *	Pueden arrastrarse gráficos con el ratón para hacer pruebas
 *	exhaustivas.
 */

PROGRAM TestCollision;

GLOBAL
	INT cogido = 0;		// ID del proceso "cogido" por el ratón

// Proceso a ejecutar por cada gráfico

PROCESS TestGraphic(GRAPH)
PRIVATE
	INT I;
	INT BX = -1, BY;
BEGIN
	// El padre debe ejecutarse primero, para borrar los textos que
	// los hijos hayan escrito el frame anterior
	PRIORITY--;

	LOOP
		// Pone el gráfico en una posición aleatoria
		X = RAND(0,640);
		Y = RAND(0,380);
		FLAGS = RAND(0,7);
		SIZE = RAND(100,200);
		ANGLE = 0;

		// Pone una cruz sobre su centro inicial en el fondo
		DRAWING_MAP (0, 0);
		DRAWING_COLOR (RGB(182,64,64));
		DRAW_LINE (X-60,Y,X+60,Y);
		DRAW_LINE (X,Y-60,X,Y+60);

		// Cuando se pulse 0, se volverá a colocar el gráfico
		// en una nueva posición aleatoria
		WHILE ASCII != 48:

			// Los cursores giran el gráfico
			IF KEY(_RIGHT): ANGLE += RAND(1000,2000); END
			IF KEY(_LEFT): ANGLE -= RAND(1000,2000); END
			IF KEY(_S): SIZE = 100; END

			// Si el gráfico ha sido cogido por el ratón...
			IF BX != -1:
				IF MOUSE.LEFT:
					// ... actualiza su posición ...
					X += MOUSE.X - BX;
					Y += MOUSE.Y - BY;
					BX = MOUSE.X;
					BY = MOUSE.Y;
				ELSE
					// ... o es soltado
					cogido = 0;
					BX = BY = -1;
				END
			END

			// Comprueba colisión con el ratón
			I = COLLISION(TYPE MOUSE);
			IF I:
				WRITE (0, X, Y, 4, "Mouse");
				IF MOUSE.LEFT AND cogido == 0:
					// El usuario "coge" el gráfico, pero
					// sólo se permite uno a la vez
					BX = MOUSE.X;
					BY = MOUSE.Y;
					cogido = ID;
				END
			END

			// Comprueba otras colisiones
			I = COLLISION(TYPE TestGraphic);
			IF I:  WRITE (0, X, Y+8, 4, I); END
			FRAME;
		END
		FRAME;
		ASCII = 0;
	END
END

BEGIN
	// Inicializa el modo gráfico
	FULL_SCREEN = FALSE;
	GRAPH_MODE = MODE_16BITS;
	SET_MODE (M640x480);
	SET_FPS (0, 0);

	// Pone un cursor al ratón
	MOUSE.GRAPH = LOAD_PNG ("Cursor.png");
	SET_CENTER (0, MOUSE.GRAPH, 1, 1);

	// Crea diversos gráficos de prueba
	GRAPH = LOAD_PNG("Check16.png");
	SET_CENTER (0, GRAPH, 24, 24);
	FROM X = 1 TO 20: TestGraphic(GRAPH); END
	GRAPH = 0;

	// Borra el fondo y los textos de los procesos hijos a cada frame
	LOOP 
		DELETE_TEXT(0); 
		WRITE (0, 16, 448, 0, "Prueba del chequeo de colisiones");
		WRITE (0, 16, 460, 0, "Ratón: arrastrar | Cursores: Rotar | S: Reinicar tamaños | 0: Posiciones aleatorias | ESC: Salir");
		IF ASCII == 48: MAP_CLEAR (0, 0, 0); END
		IF KEY(_ESC): EXIT(); END
		FRAME; 
	END
END
