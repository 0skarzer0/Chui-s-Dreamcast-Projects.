/*
 *	Prueba de clipping de líneas
 *	----------------------------
 *	Dibujar líneas con el ratón para observar el funcionamiento del clip
 *
 *	04/07/03: Modificado para que el mapa de fondo tenga un ancho impar
 *		y probar así bugs en el dibujo de líneas y rectángulos
 */

PROGRAM PruebaLineas;

PRIVATE
	x1, y1 ;
	x2, y2 ;
	drawing = FALSE ; 
	map, map2 ;
	STRING prim = "Dibujando líneas";
	INT mode;
BEGIN
	// Inicializa el modo gráfico
	SET_MODE (640, 480, 0);
	SET_FPS (0, 0);

	// Establece el cursor del ratón
	MOUSE.graph = LOAD_MAP("Cursor8.map");
	SET_CENTER (0, MOUSE.graph, 0, 0) ;

	WRITE_VAR (0, 16, 448, 0, prim);
	WRITE (0, 16, 460, 0, "Usa el ratón para dibujar, 1-5 para escoger, 0 para borrar y ESC para salir");

	// Crea el mapa que se dibujará en el centro
	MAP = NEW_MAP(313, 240, 8);
	MAP_CLEAR (0, MAP, RGB(127, 0, 0));
	PUT_SCREEN (0, MAP);

	WHILE NOT KEY(_ESC):

		// Tecla de 1 a 4 para escoger la primitiva
		IF (KEY(_0))
			MAP_CLEAR (0, MAP, RGB(127, 0, 0));
			PUT_SCREEN (0, MAP);
		ELSEIF (KEY(_1)):
			prim = "Dibujando líneas";
			mode = 0;
		ELSEIF (KEY(_2)):
			prim = "Dibujando rectángulos";
			mode = 1;
		ELSEIF (KEY(_3)):
			prim = "Dibujando círculos";
			mode = 2;
		ELSEIF (KEY(_4)):
			prim = "Dibujando cajas";
			mode = 3;
		ELSEIF (KEY(_5)):
			prim = "Dibujando círculos rellenos";
			mode = 4;
		END

		// Botón izquierdo para dibujar 
		IF MOUSE.LEFT:
			IF NOT drawing:
				// Empezar a dibujar una línea
				drawing = TRUE;
				x = mouse.x ;
				y = mouse.y ;
			ELSE
				// Cambiar la posición de la línea
				CLEAR_SCREEN () ;
				PUT_SCREEN (0, map) ;
				DRAWING_MAP (0, 0) ;
				DRAWING_COLOR(RGB(255,255,255));
				Dibuja (x, y, mouse.x, mouse.y, mode);
			END
		ELSE
			IF drawing:
				// Acabar de dibujar la línea
				DRAWING_MAP (0, map) ;
				Dibuja (x-164, y-120, mouse.x-164, mouse.y-120, mode);
				drawing = FALSE;
				PUT_SCREEN (0, map) ;
			END
		END
		
		FRAME;
	END
END

PROCESS Dibuja (x, y, mx, my, mode)
BEGIN
	SWITCH (mode)
	    CASE 0:
		DRAW_LINE (x, y, mx, my) ;
		END
	    CASE 1:
		DRAW_RECT (x, y, mx, my) ;
		END
	    CASE 2:
		DRAW_CIRCLE (x, y, SQRT((mx-x)*(mx-x) + (my-y)*(my-y)));
		END
	    CASE 3:
		DRAW_BOX (x, y, mx, my) ;
		END
	    CASE 4:
		DRAW_FCIRCLE (x, y, SQRT((mx-x)*(mx-x) + (my-y)*(my-y)));
		END
	END
END
