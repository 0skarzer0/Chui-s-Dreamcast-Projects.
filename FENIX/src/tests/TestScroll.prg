/*	Prueba de scroll
 *	----------------
 *	Muestra un par de regiones de scroll para probar colisiones, etc.
 */

PROGRAM PruebaScroll;

BEGIN
	RESTORE_TYPE = COMPLETE_RESTORE;
	DUMP_TYPE = COMPLETE_DUMP;
	SET_MODE (640, 480, 16);
	LOAD_FPG ("test.fpg");
	SET_FPS (60, 0);
	DEFINE_REGION (1, 20, 30, 600, 200);
	DEFINE_REGION (2, 20, 260, 600, 200);
	WRITE (0, 20, 20, 0, "Punto de vista de la pelota (usar cursores)");
	WRITE (0, 20, 250, 0, "Punto de vista del triángulo (movimiento aleatorio)");
	Fondo();
	SCROLL[0].CAMERA = Pelota();
	SCROLL[1].CAMERA = Triangulo();
	WHILE (!KEY(_ESC)) FRAME; END
	EXIT();
END

PROCESS Fondo()
BEGIN
	GRAPH = NEW_MAP (1024, 1024, 16);
	DRAWING_MAP (0, GRAPH);
	DRAWING_COLOR (RGB(128,0,0));
	DRAW_BOX (0, 0, 1023, 1023);
	DRAWING_COLOR (RGB(128,128,128));
	DRAW_RECT (0, 0, 1023, 1023);
	FROM X = 0 TO 1024 STEP 16:
		DRAW_LINE (X, 0, X, 32);
		DRAW_LINE (X, 1023, X, 1023-32);
		DRAW_LINE (0, X, 32, X);
		DRAW_LINE (1023, X, 1023-32, X);
	END
	DRAWING_COLOR (RGB(255,255,255));
	FROM X = 0 TO 1024 STEP 64:
		DRAW_LINE (X, 0, X, 60);
		DRAW_LINE (X, 1023, X, 1023-60);
		DRAW_LINE (0, X, 60, X);
		DRAW_LINE (1023, X, 1023-60, X);
		DRAW_CIRCLE (512, 512, X);
	END

	MOUSE.GRAPH = LOAD_PNG("Cursor.png");
	SET_CENTER (0, MOUSE.GRAPH, 0, 0);
	START_SCROLL (0, 0, GRAPH, 0, 1, 0);
	START_SCROLL (1, 0, GRAPH, 0, 2, 0);
END

PROCESS Pelota()
PRIVATE STRING mensaje;
BEGIN
	GRAPH = LOAD_PNG("Pelota.png");
	X = 64;
	Y = 64;
	CTYPE = C_SCROLL;
	CNUMBER = C_0 + C_1;
	WRITE_VAR (0, 620, 20, 2, MENSAJE);
	LOOP
		IF (KEY(_RIGHT)) X++; END
		IF (KEY(_LEFT))  X--; END
		IF (KEY(_UP))    Y--; END
		IF (KEY(_DOWN))  Y++; END
		MENSAJE = "Sin colisión";
		IF (COLLISION(TYPE Triangulo))
			MENSAJE = "Colisión con triángulo";
		ELSEIF (COLLISION(TYPE MOUSE))
			MENSAJE = "Colisión con ratón";
		END
		FRAME 25;
	END
END

PROCESS Triangulo()
PRIVATE inc, steps;
BEGIN
	GRAPH = LOAD_PNG("Triangulo.png");
	RESOLUTION = 1000;
	X = 512000;
	Y = 512000;
	CTYPE = C_SCROLL;
	CNUMBER = C_0 + C_1;
	INC = RAND(-10, 10);
	STEPS = RAND(10, 100);
	LOOP
		IF (STEPS > 0)
			STEPS--;
			ANGLE += INC*200;
			ADVANCE (3000);
		ELSE
			INC = RAND(-10, 10);
			STEPS = RAND(10, 100);
		END
		IF (X < 0 || Y > 1024000)
			ANGLE -= 90000;
		ELSEIF (X > 1024000 || Y < 0)
			ANGLE += 90000;
		END
		FRAME;
	END
END
