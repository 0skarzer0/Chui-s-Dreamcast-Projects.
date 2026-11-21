
/* 
 *	Programa para crear brushes.fpg
 */

TYPE COLOR BYTE r, g, b; END

/* Crea una paleta de escala de grises */

PROCESS MakePalette()
PRIVATE
	COLOR Color[255];
BEGIN
	FROM X = 0 TO 255:
		Color[X].r = X;
		Color[X].g = X;
		Color[X].b = X;
	END
	SET_COLORS (0, 256, &Color);
END

/* Crea un círculo sólido */

PROCESS MakeSolidCircle (Size)
BEGIN
	GRAPH = NEW_MAP (Size*2 - 1, Size*2 - 1, 8);
	DRAWING_MAP (0, GRAPH);
	DRAWING_COLOR (255);
	DRAW_FCIRCLE (Size-1, Size-1, Size-1);
	DRAW_FCIRCLE (Size, Size-1, Size-1);
	RETURN GRAPH;
END

/* Crea un círculo con degradado */

PROCESS MakeGradientCircle (Size)
BEGIN
	GRAPH = NEW_MAP (Size*2 - 1, Size*2 - 1, 8);
	DRAWING_MAP (0, GRAPH);
	IF (Size > 1)
		FROM X = SIZE-1 TO 0 STEP -1:
			DRAWING_COLOR (255-(X+1)*255/Size);
			DRAW_FCIRCLE (SIZE-1, SIZE-1, X);
		END
		DRAWING_COLOR (255);
		DRAW_LINE (SIZE-2, SIZE-1, SIZE, SIZE-1);
	END
	RETURN GRAPH;
END

/* Crea un rectángulo sólido */

PROCESS MakeSolidRect (Size)
BEGIN
	GRAPH = NEW_MAP (Size, Size, 8);
	DRAWING_MAP (0, GRAPH);
	DRAWING_COLOR (255);
	DRAW_BOX (0, 0, Size, Size);
	RETURN GRAPH;
END

/* Crea un rectángulo con degradado (de tamaño par) */

PROCESS MakeGradientRect (Size)
BEGIN
	GRAPH = NEW_MAP (Size, Size, 8);
	DRAWING_MAP (0, GRAPH);
	IF (Size > 1)
		FROM X = SIZE/2 TO 0 STEP -1:
			DRAWING_COLOR (255 - X*255/(SIZE/2));
			DRAW_RECT (SIZE/2-X, SIZE/2-X, SIZE/2+X, SIZE/2+X);
		END
	END
	RETURN GRAPH;
END

/* Crea el FPG */

BEGIN
	MakePalette();
	FILE = NEW_FPG();
	FPG_ADD (FILE, 1, 0, MakeSolidCircle(1));
	FPG_ADD (FILE, 2, 0, MakeSolidCircle(2));
	FPG_ADD (FILE, 3, 0, MakeSolidCircle(3));
	FPG_ADD (FILE, 4, 0, MakeSolidCircle(4));
	FPG_ADD (FILE, 5, 0, MakeSolidCircle(5));
	FPG_ADD (FILE, 6, 0, MakeSolidCircle(6));
	FPG_ADD (FILE, 7, 0, MakeSolidCircle(7));
	FPG_ADD (FILE, 8, 0, MakeSolidCircle(8));
	FPG_ADD (FILE, 9, 0, MakeSolidCircle(9));
	FPG_ADD (FILE, 10, 0, MakeSolidCircle(10));
	FPG_ADD (FILE, 11, 0, MakeGradientCircle(1));
	FPG_ADD (FILE, 12, 0, MakeGradientCircle(2));
	FPG_ADD (FILE, 13, 0, MakeGradientCircle(3));
	FPG_ADD (FILE, 14, 0, MakeGradientCircle(4));
	FPG_ADD (FILE, 15, 0, MakeGradientCircle(5));
	FPG_ADD (FILE, 16, 0, MakeGradientCircle(6));
	FPG_ADD (FILE, 17, 0, MakeGradientCircle(7));
	FPG_ADD (FILE, 18, 0, MakeGradientCircle(8));
	FPG_ADD (FILE, 19, 0, MakeGradientCircle(9));
	FPG_ADD (FILE, 20, 0, MakeGradientCircle(10));
	FPG_ADD (FILE, 21, 0, MakeSolidRect(1));
	FPG_ADD (FILE, 22, 0, MakeSolidRect(3));
	FPG_ADD (FILE, 23, 0, MakeSolidRect(5));
	FPG_ADD (FILE, 24, 0, MakeSolidRect(7));
	FPG_ADD (FILE, 25, 0, MakeSolidRect(9));
	FPG_ADD (FILE, 26, 0, MakeSolidRect(11));
	FPG_ADD (FILE, 27, 0, MakeSolidRect(13));
	FPG_ADD (FILE, 28, 0, MakeSolidRect(15));
	FPG_ADD (FILE, 29, 0, MakeSolidRect(17));
	FPG_ADD (FILE, 30, 0, MakeSolidRect(19));
	FPG_ADD (FILE, 31, 0, MakeGradientRect(2));
	FPG_ADD (FILE, 32, 0, MakeGradientRect(4));
	FPG_ADD (FILE, 33, 0, MakeGradientRect(6));
	FPG_ADD (FILE, 34, 0, MakeGradientRect(8));
	FPG_ADD (FILE, 35, 0, MakeGradientRect(10));
	FPG_ADD (FILE, 36, 0, MakeGradientRect(12));
	FPG_ADD (FILE, 37, 0, MakeGradientRect(14));
	FPG_ADD (FILE, 38, 0, MakeGradientRect(16));
	FPG_ADD (FILE, 39, 0, MakeGradientRect(18));
	FPG_ADD (FILE, 40, 0, MakeGradientRect(20));
	SAVE_FPG (FILE, "brushes.fpg");
END
