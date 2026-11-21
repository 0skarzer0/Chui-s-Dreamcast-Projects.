/*
 *	Prueba de precisión del blitter
 *	-------------------------------
 *	Todos los gráficos son dibujados con tamaños múltiplos de 100,
 *	por lo que deberían encajar perfectamente los unos con los otros,
 *	no presentar deformaciones, y encajar los bordes de los unos
 *	con los otros. Se prueban diversas combinaciones de flags,
 *	pero no ángulos.
 *
 *	Dado que este programa usa intensivamente el blitter, puede
 *	servir como benchmark. Se le ha añadido al caso un marcador de FPS.
 */

Program TestBlitter;

// Proceso que dibuja un gráfico en una posición y tamaño determinados
PROCESS Graphic (x, y, size, startangle, graph, flags);
BEGIN
	ANGLE = startangle;
	x = x / 2;
	y = y / 2;
	WHILE NOT KEY(_ESC):
		IF KEY(_R): ANGLE = startangle; END
		IF KEY(_RIGHT): ANGLE += 1000; END
		IF KEY(_LEFT): ANGLE -= 1000; END
		FRAME;
	END
END

// Dibuja el fondo de pantalla
PROCESS DibujarFondo();
BEGIN
	DRAWING_MAP (0, 0);
	DRAWING_COLOR (RGB(63, 31, 31));
	FOR (X = 0; X < 320; X += 4): DRAW_LINE (X, 0, X, 240); END
	FOR (Y = 0; Y < 240; Y += 4): DRAW_LINE (0, Y, 320, Y); END
	DRAWING_COLOR (RGB(127, 31, 31));
	FOR (X = -1; X < 320; X += 4): DRAW_LINE (X, 0, X, 240); END
	FOR (Y = -1; Y < 240; Y += 4): DRAW_LINE (0, Y, 320, Y); END
	DRAWING_COLOR (RGB(192, 31, 31));
	FOR (X = 0; X < 320; X += 16): DRAW_LINE (X, 0, X, 240); END
	FOR (Y = 0; Y < 240; Y += 16): DRAW_LINE (0, Y, 320, Y); END
END

PRIVATE 
	INT PNG16;
	INT PNG16C;
BEGIN
	// Inicializa un modo gráfico de 16 bits
	FULL_SCREEN = FALSE;
	GRAPH_MODE = MODE_16BITS;
	SET_MODE (M320x240);
	SET_FPS (0, 0);
	DibujarFondo();

	// Carga los gráficos de prueba
	PNG16 = LOAD_PNG ("check16.png");
	PNG16C = MAP_CLONE (0, PNG16);
	SET_CENTER (0, PNG16C, 0, 0);

	// Dibuja un texto explicativo
	WRITE (0, 16, 200, 0, "Prueba de precisión del blitter");
	WRITE_INT (0, 300, 220, 2, &FPS);

	// Dibuja los gráficos
	Graphic (16, 16, 100, 0, PNG16C, 0);
	Graphic (48, 16, 100, 0, PNG16C, 1);
	Graphic (16, 48, 100, 0, PNG16C, 2);
	Graphic (48, 48, 100, 0, PNG16C, 3);
	Graphic (16, 112, 100, 0, PNG16C, 4);
	Graphic (48, 112, 100, 0, PNG16C, 5);
	Graphic (16, 144, 100, 0, PNG16C, 6);
	Graphic (48, 144, 100, 0, PNG16C, 7);
	Graphic (112, 16, 100, 0, PNG16C, 0);
	Graphic (176, 16, 200, 0, PNG16C, 0);
	Graphic (240, 16, 200, 0, PNG16C, 1);
	Graphic (176, 80, 200, 0, PNG16C, 2);
	Graphic (240, 80, 200, 0, PNG16C, 3);
	Graphic (208, 176, 200, 0, PNG16C, 0);
	Graphic (336, 16, 300, 0, PNG16C, 0);
	Graphic (336, 144, 300, 0, PNG16C, 128);
	Graphic (336, 272, 300, 0, PNG16C, 4);
	Graphic (464, 16, 400, 0, PNG16C, 0);
	Graphic (528, 208, 400, 0, PNG16, 2);
	Graphic (528, 380, 400, -45000, PNG16, 2);
	Graphic (32, 310, 400, 45000, PNG16C, 3);
	Graphic (160, 390, 200, 45000, PNG16C, 0);

	WHILE NOT KEY(_ESC):
		IF KEY(_ALT) && KEY(_W) && FULL_SCREEN:
			FULL_SCREEN = FALSE;
			SET_MODE(m320x240);
			DibujarFondo();
		END
		IF KEY(_ALT) && KEY(_F) && !FULL_SCREEN:
			FULL_SCREEN = TRUE;
			SET_MODE(m320x240);
			DibujarFondo();
		END
		FRAME;
	END
END

