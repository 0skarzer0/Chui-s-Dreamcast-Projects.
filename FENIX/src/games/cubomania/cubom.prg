// CuboMania - Juego de ejemplo para Fenix
// ----------------------------------
//
// (c) 2000 Carlos Guillermo Coll Silvera
// 
// Este juego se considera de dominio público. Puedes hacer lo que quieras
// con él y con su código, incluyendo modificarlo, redistribuirlo, o
// utilizarlo en tus propios programas de cualquier manera.

// ULTIMA REVISION: 28/07/2002, adaptado para funcionar con Fenix v08 por [ManOWaR].
PROGRAM CuboMania;

// Inicializacion de variable globales ---------------------------------------------------------------------------------
GLOBAL
	INT grafico       =  1;
	INT FilMin        =  0;
	INT FilMax        =  2;
	INT ColMin        =  0;
	INT ColMax        =  1;
	INT Grafico_Mouse = 99;
	INT Mover;
	INT Finalizo;
	INT tCont[5][5];
	INT tX[5][5];
	INT tY[5][5];
	INT Numero, xTEMP, yTEMP;
	INT ID_Cubo[60];
	INT opcion;
	INT Movimientos;

	
	// Sonidos
	INT Click_Cubo;
	INT Mueve_Cubo;
	INT Tecla2;
	INT Pega_en_cubito;
	INT Textos;
	INT Cubito;

END

// Codigo verificacion de si se llego al final del juego ---------------------------------------------------------------
PROCESS Verifica();
PRIVATE tFIL, tCOL;
BEGIN
	Finalizo = 0;
	numero = 1;
	FROM tFIL = FilMin to FilMax
		FROM tCOL = ColMin to ColMax
			IF Numero < ((FilMax+1) * (ColMax+1)):
				IF ID_Cubo[Numero].x == tX[tFIL][tCOL] AND ID_Cubo[Numero].y == tY[tFIL][tCOL]:
					Finalizo++;
				END
			END
			Numero++;
		END
	END
	IF Finalizo == ((FilMax+1) * (ColMax+1)) -1:
		FROM Numero = 1 to ((FilMax+1) * (ColMax+1))-1
			REPEAT
				ID_Cubo[Numero].x-=Numero;
				ID_Cubo[Numero].y+=Numero;
				FRAME(3);
			UNTIL ID_Cubo[Numero].x < -20 and ID_Cubo[Numero].y > 500;
			play_wav (Click_Cubo, 0);
		END
		FilMax++;
		ColMax = FilMax - 1;
		IF FilMax == 6:
			FilMax = 3;
			ColMax = FilMax - 1;
		END
		Juego();
	END
	Finalizo = 1;
END

// Codigo creacion de cubos fantasmas (el efecto de movimiento con retardo) --------------------------------------------
PROCESS Cubo_Fant(graph, x, y, sentido)
BEGIN
	Z = 1;
	Flags = 4;
	FROM size = 150 to 0 step -10
		SWITCH (Sentido)
			CASE 1: x = x + 1; END
			CASE 2: x = x - 1; END
			CASE 3: y = y + 1; END
			CASE 4: y = y - 1; END
		END
		FRAME(120);
	END
END

// Codigo creacion de cubos con numero, movimientos --------------------------------------------------------------------
PROCESS Cubo(graph, x, y, Fil, Col)
PRIVATE
	INT s, vas = 60, tFIL, tCOL;
	INT mescla = 1;

BEGIN
	LOOP
		IF finalizo == 1;
			IF mescla <> 0: mescla++; END
			IF mescla > ((rand((FilMax+1) * (ColMax+1),(FilMax+1) * (ColMax+1)+20))*2): mescla = 0; END
			IF ((mouse.left AND collision(TYPE mouse)) OR (mescla > 0)):
				Mover = 0;
				IF tCont[Fil][Col+1] == 0 AND Col >= ColMin AND Col < ColMax AND Mover == 0:
					IF mescla == 0: Movimientos+=1; END
					tCont[Fil][Col] =       0;
					Col             = Col + 1;
					tCont[Fil][Col] =       1;
					Mover           =       1;
					Cubo_Fant(graph, x, y, 1);
					play_wav(Mueve_Cubo, 0);
					FROM x = tX[Fil][Col-1] to tX[Fil][Col]-1
						IF mescla == 0: s = s + 2; ELSE s = 1; END
						FRAME(s);
					END
				END
				IF tCont[Fil][Col-1] == 0 AND Col > ColMin AND Col <= ColMax AND Mover == 0:
					IF mescla == 0: Movimientos+=1; END
					tCont[Fil][Col]   =       0;
					Col               = Col - 1;
					tCont[Fil][Col]   =       1;
					Mover             =       1;
					Cubo_Fant(graph, x, y, 2);
					play_wav (Mueve_Cubo, 0);
					FROM x = tX[Fil][Col+1] to tX[Fil][Col]+1 STEP -1 
						IF mescla == 0: s = s + 2; ELSE s = 1; END
						FRAME(s);
					END
				END
				IF tCont[Fil+1][Col] == 0 AND Fil >= FilMin AND Fil < FilMax AND Mover == 0:
					IF mescla == 0: Movimientos+=1; END
					tCont[Fil][Col] =       0;
					Fil             = Fil + 1;
					tCont[Fil][Col] =       1;
					Mover           =       1;
					Cubo_Fant(graph, x, y, 3);
					play_wav (Mueve_Cubo, 0);
					FROM y = tY[Fil-1][Col] to tY[Fil][Col]-1
						IF mescla == 0: s = s + 2; ELSE s = 1; END
						FRAME(s);
					END
				END
				IF tCont[Fil-1][Col] == 0 AND Fil > FilMin AND Fil <= FilMax AND Mover == 0:
					IF mescla == 0: Movimientos+=1; END
					tCont[Fil][Col] =       0;
					Fil             = Fil - 1;
					tCont[Fil][Col] =       1;
					Mover           =       1;
					Cubo_Fant(graph, x, y, 4);
					play_wav (Mueve_Cubo, 0);
					FROM y = tY[Fil+1][Col] to tY[Fil][Col]+1 STEP -1
						IF mescla == 0: s = s + 2; ELSE s = 1; END
						FRAME(s);
					END
				END
				s = 0;
				IF Mescla == 0:	Verifica(); END
			END
		END
		FRAME;
	END
END

// Codigo de comienzo del juego, creacion de arrays, inicializacion de variables ---------------------------------------
PROCESS Juego()
PRIVATE scroll_X, scroll_Y, vas = 60, tFIL, tCOL;
BEGIN
	fade_off();
	fade_on();

	let_me_alone();
	delete_text(0);
	clear_screen();
	mouse.graph = Grafico_Mouse;
	write(1,10,10,0,"MOVIMIENTOS:");
	write_int(1,text_width(1,"MOVIMIENTOS:")+10,10,0, &Movimientos);
	write(0,320,477,7,"Para salir presione ESC");

	xTEMP = 280 - (ColMax * graphic_info(0, grafico, G_WIDE))/2;
	yTEMP = 220 - (FilMax * graphic_info(0, grafico, G_HEIGHT))/2;
	// Rellena la Tabla tCont para saber que lugares estan ocupados = 1 y vacios = 2
	FROM tFIL = FilMin to FilMax
		FROM tCOL = ColMin to ColMax
			tCont[tFIL][tCOL] = 1;
			xTEMP = xTEMP + graphic_info(0, grafico, G_WIDE);
			tX[tFIL][tCOL] = xTEMP;
			tY[tFIL][tCOL] = yTEMP;
		END
		yTEMP = yTEMP + graphic_info(0, grafico, G_HEIGHT);
		xTEMP = 280 - (ColMax * graphic_info(0, grafico, G_WIDE))/2;
	END
	tCont[tFIL-1][tCOL-1] = 0;	

	// Ponemos el grafico correspondiente tCont y si contiene un lugar lleno pones el grafico en esas coordenadas
	Numero = 0;
	FROM tFIL = FilMin to FilMax
		FROM tCOL = ColMin to ColMax
			Numero++;
			IF tCont[tFIL][tCOL] == 1;
				ID_Cubo[Numero] = Cubo(Numero, tX[tFIL][tCOL], tY[tFIL][tCOL], tFIL, tCOL);				
			END
		END
	END
	finalizo = 1;
	start_scroll(0,0,54,0,0,1);
	scroll_X = rand(-1,1);
	scroll_Y = rand(-1,1);
	LOOP
		IF key(_ESC): fade_off(); exit("",0); END
		scroll.x0+=scroll_X;
		scroll.y0+=scroll_Y;
		FRAME ;
	end
END

// Codigo para la creacion y movimiento de los cubitos en la presentacion ----------------------------------------------
PROCESS cubitos(graph, x, y)
PRIVATE
	INT Dire = 15;

BEGIN
	LOOP
		IF y > 140: Dire = -2; END
		IF y < 128 and y > 120:	Dire = 2; END
		y += Dire;
		FRAME;
	END
END

// Codigo para la creacion y movimiento de las letras en la presentacion -----------------------------------------------
PROCESS letraslogo(graph,x,y)
PRIVATE
	INT valFRAME = 5;
	INT Dire = 2;

BEGIN
	z = -1;
	size = 400;
	LOOP
		IF size >= 102: 
			size-=2; 
		END
		IF size == 102: graph +=9; valFRAME = 100; play_wav (Pega_en_cubito, 0); END
		IF size == 100:
			IF y > 136: Dire = -2; END
			IF y < 128 and y > 120:	Dire = 2; END
			y += Dire;
		END
		FRAME(valFRAME);
	END
END

// Codigo para poner las letras con mejor resolucion en la presentacion ------------------------------------------------
PROCESS letras2(graph,x,y,sent)
BEGIN
	LOOP
		IF x <> 320: 
			x+=Sent; 
		END
		FRAME(5);
	END
END

// Codigo de inicio de la presentacion, creacion del menu inicial, etc -------------------------------------------------
PROCESS Inicio()
PRIVATE
	INT cont, cont1, xt, yt, Graf_Cubito = 30, ID_Letras[9], ID_Cubs[9];
BEGIN	
	xt = 78;
	yt = -50;
	write(0,639,475,8,"Ver 1.0 - Agosto del 2000");
	FROM cont = 1 to 9
		ID_Cubs[cont] = cubitos(Graf_Cubito,  xt, yt);
		yt-=50;
		xt+=graphic_info(0, Graf_Cubito, G_WIDE)+6;
		REPEAT
		FRAME;
		UNTIL son.y > 128;
		play_wav (Cubito, 0); 
	END
	REPEAT
		FRAME;
	UNTIL ID_Cubs[9].y > 128;

	xt = 78;
	yt = -50;
	FROM cont = 1 to 9
		FROM cont1 = 1 to 10 FRAME; END
		ID_Letras[cont] = letraslogo(cont + 32,xt,132);
		yt-=50;
		xt+=graphic_info(0, Graf_Cubito, G_WIDE)+6;
	END

	FROM cont1 = 1 to 10 FRAME; END
	letras2(51, 740, 400, -1);
	play_wav (Textos, 0);
	FRAME(3000);
	letras2(52, -100, 420, 1);
	play_wav (Textos, 0);
	FRAME(3000);
	letras2(53, 740, 440, -1);
	play_wav (Textos, 0);
	FRAME(3000);
	Menu();
END

// Codigo de creacion del menu de la presentacion con efecto maquina de escribir(LA VERDAD QUE ME QUEDO LINDO XD) ------
PROCESS Menu();
BEGIN
	mouse.graph = Grafico_Mouse;
	CreaOPCIONES (0, 280, 240, 1, "Empezar el juego");
	FRAME(5000);
	CreaOPCIONES (0, 280, 260, 2, "Salir del juego");

	LOOP
		SWITCH (opcion)
			CASE 1: Juego(); END
			CASE 2: exit("Gracias por jugar - Guillermo Coll",0); END
		END
		FRAME;
	END
END

PROCESS CreaOPCIONES(fuente, x, y, region, string opTexto);
PRIVATE
	INT Ancho_Texto;
BEGIN
	Ancho_Texto = text_width(0, opTexto) + 10;
	OpcionMENU(30, x, y, (x + (Ancho_Texto))+2, opTexto, 1, region);
	graph = write_in_map(fuente, opTexto, 3);
	z = -2;
	LOOP
		IF mouse.x > x AND mouse.x < ((x + (Ancho_Texto))+2) AND mouse.y > (y - text_height(0, opTexto)/2) AND mouse.y < (y + text_height(0, opTexto)/2) AND mouse.left: Opcion = region; END
		IF son.x <> (x + (Ancho_Texto))+2: 
			play_wav (Tecla2, 0);
			FRAME(400);
		ELSE
			FRAME;
		END
	END
END

PROCESS OpcionMENU(graph, x, y, xFinal, STRING opTexto, INT Sent, reg)
	PRIVATE
		INT xt1;
BEGIN
	size = 20;
	xt1 = x;
	z = -3;
	LOOP
		IF x <> xFinal: 
			x+=Sent; 
			define_region(reg, xt1, (y - text_height(0, opTexto)/2), x-xt1, (text_height(0, opTexto)));
		END
		IF collision(TYPE mouse) and mouse.left: Opcion = reg; END
		FRAME(30);
	END
END

// Codigo de inicio del programa ---------------------------------------------------------------------------------------
BEGIN
	// Inicialización
	graph_mode = MODE_16BITS;
	set_mode(m640x480);
	set_fps(40, 0);

	// Carga de gráficos
 	load_fpg("grx.fpg");
 	load_fnt("fuente1.fnt");
	//sound_mode = MODE_STEREO + MODE_HQ + MODE_16BITS ;

	Click_Cubo     = load_wav("cliccubo.wav");
	Mueve_Cubo     = load_wav("muev_cub.wav");
	Tecla2         = load_wav("tecla2.wav");
	Pega_en_cubito = load_wav("pegacubi.wav");
	Textos         = load_wav("textos.wav");
	Cubito         = load_wav("cubito.wav");

	Inicio();
END
