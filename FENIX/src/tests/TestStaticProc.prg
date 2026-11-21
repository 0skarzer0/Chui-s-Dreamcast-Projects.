
// Programa para probar procesos estáticos en pantalla. Este programa dibuja
// una cantidad elevada de procesos estáticos, representando los bloques
// de una pantalla isométrica. En el futuro el rendimiento debería
// incrementarse añadiendo soporte de PARTIAL_DUMP / PARTIAL_RESTORE

CONST
	Speed = 25;
	MaxPos = 9*Speed;
GLOBAL
	GrafBloque[5];
	ProcCount;
	FrameCount;
	Pantalla[15][15][15];

PROCESS Bloque (N, OX, OY, OZ)
BEGIN
	// Convierte coordenadas isométricas en corrientes
	X = 320 + 32 * OX - 32 * OY;
	Y = 180 + 16 * OX + 16 * OY - 24 * OZ;
	Z = -OZ-OX-OY;

	GRAPH = GrafBloque[N];
	LOOP FRAME; END
END

PROCESS BloqueMovil (GRAPH, OX, OY)
PRIVATE Dir = 1, OZ;
BEGIN
	OX *= Speed; 
	OY *= Speed;
	RESOLUTION = Speed;
	LOOP
		X = 320 * Speed + 32 * OX - 32 * OY;
		Y = 180 * Speed + 16 * OX + 16 * OY - 24 * OZ;
		Z = (-OZ-OX-OY)/Speed;
		IF (Dir == 0)
			IF ((TIMER % MaxPos) < OZ) 
				OZ = MaxPos; 
				Dir = 1; 
			ELSE
				OZ = (TIMER % MaxPos);
			END
		ELSE
			IF ((MaxPos - (TIMER % MaxPos)) > OZ) 
				OZ = 0; 
				Dir = 0; 
			ELSE
				OZ = MaxPos - (TIMER % MaxPos);
			END
		END
		FRAME;
	END
END

PROCESS PintaPantalla()
BEGIN
	ProcCount = 0;
	FROM X = 0 TO 9:
		FROM Y = 0 TO 9:
			FROM Z = 0 TO 9:
				IF (Pantalla[X][Y][Z] != 0)
					ProcCount++;
					Bloque(Pantalla[X][Y][Z]-1, X, Y, Z);
				END
			END
		END
	END
END

PROCESS Pinta (GRAPH, R, G, B)
BEGIN
	X = BLENDOP_NEW();
	BLENDOP_TINT(X, 0.25, R, G, B);
	BLENDOP_APPLY (0, GRAPH, X);
	BLENDOP_FREE(X);
END

BEGIN
	FULL_SCREEN = FALSE;
	SET_MODE (640, 480, 16);
	GrafBloque[0] = LOAD_PNG("Bloque.png");
	GrafBloque[1] = MAP_CLONE(0, GrafBloque[0]);
	GrafBloque[2] = MAP_CLONE(0, GrafBloque[0]);
	PINTA (GrafBloque[0], 255, 0, 0);
	PINTA (GrafBloque[1], 0, 128, 128);
	PINTA (GrafBloque[2], 255, 0, 128);

	// Rellena la pantalla con bastantes bloques
	FROM X = 0 TO 9:
		FROM Z = 0 TO 9:
			IF (Z < 10-X)
				Pantalla[X][0][Z] = 1;
				Pantalla[X][9][Z] = 1;
			END
			IF (Z > 0)
				Pantalla[0][X][Z] = 1;
			END
			Pantalla[X][Z][0] = 2;
		END
	END
	PANTALLA[4][4][0] = 0;
	PANTALLA[5][4][0] = 0;
	PANTALLA[4][5][0] = 0;
	PANTALLA[5][5][0] = 0;

	// Pinta la pantalla y crea algunos bloques móviles
	PintaPantalla();
	BloqueMovil(GrafBloque[2], 4,4);
	BloqueMovil(GrafBloque[2], 4,5);
	BloqueMovil(GrafBloque[2], 5,4);
	BloqueMovil(GrafBloque[2], 5,5);
	ProcCount += 4;

	// Espera hasta que el usuario pulse ESC
	SET_FPS(0,0);
	DELETE_TEXT(0);
	WRITE (0, 20, 16, 0, FORMAT(ProcCount) + " Procesos");
	WRITE (0, 580, 16, 2, "FPS:");
	WRITE_VAR (0, 620, 16, 2, FPS);
	WHILE (!KEY(_ESC)): 
		FRAME; 
		FrameCount++;
		TIMER = FrameCount;
		IF (KEY(_1)) 
			RESTORE_TYPE = COMPLETE_RESTORE; 
			DUMP_TYPE = COMPLETE_DUMP;
		ELSEIF (KEY(_2)) 
			RESTORE_TYPE = PARTIAL_RESTORE; 
			DUMP_TYPE = PARTIAL_DUMP;
		ELSEIF (KEY(_3)) 
			RESTORE_TYPE = PARTIAL_RESTORE; 
			DUMP_TYPE = COMPLETE_DUMP;
		END
	END
	LET_ME_ALONE();
END
