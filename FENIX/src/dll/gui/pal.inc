
CONST
	// Textos para el editor de paleta
	TXT_PETitle	= "Palette Editor";
	TXT_PSection1	= "Palette";
	TXT_PButton1	= "Default";
	TXT_PButton2	= "&Load...";
	TXT_PButton3	= "&Save...";
	TXT_PSection2	= "Selection";
	TXT_PButton4	= "&Copy";
	TXT_PButton5	= "&Paste";
	TXT_PButton6	= "&Restore";
	TXT_PButton7	= "&Invert";
	TXT_PButton8	= "&Sort";
	TXT_PButton9	= "&Gradient";
	TXT_PButton10	= "&Left";
	TXT_PButton11	= "&Right";
	TXT_PButton12	= "Reverse";
	TXT_PButton13	= "Select...";
	TXT_PWarning1A	= "Loading default palette";
	TXT_PWarning1B	= "Current palette will be lost. Are you sure?";
	TXT_POpen	= "Loading palette";
	TXT_PSave	= "Saving palette";
	TXT_PFiletypes	= "Native:\.(pal|map|fpg);*.PAL;*.MAP;*.*";
	TXT_PNotValid   = "This is not a valid palette file or native 8-bits map";
	TXT_PMenuAll	= "Select &All";
	TXT_PMenuNone	= "Select &None";
	TXT_PMenuInv	= "&Invert Selection";
END

GLOBAL
	BYTE PalSelection[255];
END

TYPE ColorB
	BYTE R, G, B;
END

// Procesos usados por el menú de selección

PROCESS PALMENU_SELECT_ALL()
BEGIN
	MEMSET (&PalSelection, 1, 256);
END

PROCESS PALMENU_SELECT_NONE()
BEGIN
	MEMSET (&PalSelection, 0, 256);
END

PROCESS PALMENU_SELECT_INVERT()
BEGIN
	FROM X = 0 TO 255:
		PalSelection[X] ^= 1;
	END
END
// -----------------------------------------------------------------------------
//   DIALOG_PALETTE_EDITOR
// -----------------------------------------------------------------------------
//
//   Muestra el cuadro de diálogo del editor de paleta
//

PROCESS DIALOG_PALETTE_EDITOR()
PRIVATE
	INT	Ventana, Panel;
	INT	Color;
	INT	ColorValue;
	INT	PreviousColor;
	INT	PreviousValue;
	INT	Boton[15];
	INT	I;
	INT	MenuSel;
	INT  	R, G, B;
	INT	R2, G2, B2;
	INT	MAX, MIN, DIF, MINH;
	INT	Count, Pass;

	COLORB Original[255];
	COLORB Palette[255];
	COLORB Sorted[255];
	COLORB PaletteCopy[255];
	BYTE   PalSelectionCopy[255];

	// Estructura usada temporalmente para ordenar colores

	STRUCT ToSort[255]
		INDEX;
		H, L;
	END
BEGIN
	// Crea la ventana y sus controles

	Ventana = WINDOW (TXT_PETitle, STYLE_DIALOG);
	WINDOW_ADDCONTROL (Ventana, 0, 0, Panel = PALETTE_PANEL(288, 288, &Color));
	WINDOW_ADDCONTROL (Ventana, 1, 292, COLOR_CHOOSER(&ColorValue, CCM_ENABLEOK));
	WINDOW_ADDCONTROL (Ventana, 292,   0, FRAME(90, 382, FRAME_RAISE));
	WINDOW_ADDCONTROL (Ventana, 292,   4, LABEL(TXT_PSection1, 1, 90));
	WINDOW_ADDCONTROL (Ventana, 297,  20, Boton[0] = BUTTON(TXT_PButton1));
	WINDOW_ADDCONTROL (Ventana, 297,  45, Boton[1] = BUTTON(TXT_PButton2));
	WINDOW_ADDCONTROL (Ventana, 297,  70, Boton[2] = BUTTON(TXT_PButton3));
	WINDOW_ADDCONTROL (Ventana, 292, 100, LABEL(TXT_PSection2, 1, 90));
	WINDOW_ADDCONTROL (Ventana, 297, 116, Boton[3] = BUTTON(TXT_PButton4));
	WINDOW_ADDCONTROL (Ventana, 297, 141, Boton[4] = BUTTON(TXT_PButton5));
	WINDOW_ADDCONTROL (Ventana, 297, 166, Boton[5] = BUTTON(TXT_PButton6));
	WINDOW_ADDCONTROL (Ventana, 297, 191, Boton[6] = BUTTON(TXT_PButton7));
	WINDOW_ADDCONTROL (Ventana, 297, 216, Boton[7] = BUTTON(TXT_PButton8));
	WINDOW_ADDCONTROL (Ventana, 297, 241, Boton[8] = BUTTON(TXT_PButton9));
	WINDOW_ADDCONTROL (Ventana, 297, 266, Boton[9] = BUTTON(TXT_PButton10));
	WINDOW_ADDCONTROL (Ventana, 297, 291, Boton[10] = BUTTON(TXT_PButton11));
	WINDOW_ADDCONTROL (Ventana, 297, 316, Boton[11] = BUTTON(TXT_PButton12));
	WINDOW_ADDCONTROL (Ventana, 297, 356, Boton[12] = BUTTON(TXT_PButton13));
	WINDOW_SET (Ventana, WINDOW_MODAL, TRUE);
	PALETTE_PANEL_SELECTION (Panel, &PalSelection);

	// Crea el menú de opciones para cambiar la selección

	MenuSel = MENU();
	MENU_ACTION (MenuSel, TXT_PMenuAll, TYPE PALMENU_SELECT_ALL);
	MENU_ACTION (MenuSel, TXT_PMenuNone, TYPE PALMENU_SELECT_NONE);
	MENU_ACTION (MenuSel, TXT_PMenuInv, TYPE PALMENU_SELECT_INVERT);

	// Establece los valores iniciales

	GET_COLORS (0, 256, &Original);
	ColorValue = RGB(Original.R, Original.G, Original.B);
	MEMSET (&PalSelection, 0, 256);
	PalSelection[0] = 1;
	PreviousValue = ColorValue;
	PreviousColor = Color;

	DESKTOP_ADDWINDOW (Ventana);
	WHILE (WINDOW_VISIBLE(Ventana))

		// Respuesta a los botones de acción

		IF (BUTTON_PRESSED(Boton[0])) 		

			// Cargar la paleta por defecto
			// ----------------------------
			// Esta acción se limita a recuperar el fichero "default.pal"
			// grabado durante el comienzo del programa

			MessageBox (TXT_PWarning1A, TXT_PWarning1B, MB_ALERT | MB_YESNO);
			FRAME;
			IF (MB_RESULT) LOAD_PAL("default.pal"); END

		ELSEIF (BUTTON_PRESSED(Boton[1]))	
		
			// Cargar una nueva paleta
			// -----------------------
			// Usa el cuadro de diálogo estandar para abrir un fichero PAL

			OpenFileDialog (TXT_POpen, TXT_PFiletypes, 0);
			FRAME;
			IF (OF_RESULT) 
				IF (LOAD_PAL(OF_File) == 0)
					MessageBox (TXT_POpen, TXT_PNotValid, MB_DANGER); 
				END
			END

		ELSEIF (BUTTON_PRESSED(Boton[2]))	
		
			// Grabar la paleta en disco
			// -------------------------
			// Igual al anterior. Añade la extensión .pal automáticamente.

			OpenFileDialog (TXT_PSave, "*.PAL", 0);
			FRAME;
			IF (OF_RESULT) 
				IF (FIND(OF_FILE, ".") == -1)
					OF_FILE += ".pal";
				END
				SAVE_PAL(OF_File); 
			END

		ELSEIF (BUTTON_PRESSED(Boton[3]))	
		
			// Copiar colores
			// --------------
			// Guarda la selección actual en un buffer en memoria
			// (tanto los colores, como la matriz de seleccionados)

			GET_COLORS (0, 256, &Palette);
			MEMCOPY (&PaletteCopy, &Palette, SIZEOF(Palette));
			MEMCOPY (&PalSelectionCopy, &PalSelection, SIZEOF(PalSelection));

		ELSEIF (BUTTON_PRESSED(Boton[4]))	
		
			// Pegar colores
			// -------------
			// Recupera la matriz copiada sobre la actual (si eran
			// menos colores, los va repitiendo sucesivamente)

			X = 0;
			WHILE (!PalSelectionCopy[X] && X < 256) X++; END
			IF (X < 256)
				GET_COLORS (0, 256, &Palette);
				FROM I = 0 TO 255:
					IF (PalSelection[I])
						Palette[I].R = PaletteCopy[X].R;
						Palette[I].G = PaletteCopy[X].G;
						Palette[I].B = PaletteCopy[X].B;
						X++;
						WHILE (!PalSelectionCopy[X])
							X++;
							IF (X == 256) X = 0; END
						END
					END
				END
				SET_COLORS (0, 256, &Palette);
				ColorValue = RGB(Palette[Color].R, Palette[Color].G, Palette[Color].B);
			END

		ELSEIF (BUTTON_PRESSED(Boton[5]))	
		
			// Restaurar colores
			// -----------------
			// Utiliza una copia realizada antes de sacar el cuadro de
			// diálogo en memoria, para deshacer todas las operaciones

			GET_COLORS (0, 256, &Palette);
			FROM I = 0 TO 255:
				IF (PalSelection[I])
					Palette[I].R = Original[I].R;
					Palette[I].G = Original[I].G;
					Palette[I].B = Original[I].B;
				END
			END
			SET_COLORS (0, 256, &Palette);
			ColorValue = RGB(Palette[Color].R, Palette[Color].G, Palette[Color].B);
		ELSEIF (BUTTON_PRESSED(Boton[6]))	
		
			// Invertir colores
			// ----------------
			// Simplemente invierte las componentes RGB de todos los
			// colores seleccionados

			GET_COLORS (0, 256, &Palette);
			FROM I = 0 TO 255:
				IF (PalSelection[I])
					Palette[I].R = 255-Palette[I].R;
					Palette[I].G = 255-Palette[I].G;
					Palette[I].B = 255-Palette[I].B;
				END
			END
			SET_COLORS (0, 256, &Palette);
			ColorValue = RGB(Palette[Color].R, Palette[Color].G, Palette[Color].B);

		ELSEIF (BUTTON_PRESSED(Boton[7]))	
		
			// Ordenar colores
			// ---------------
			// Ordena los colores seleccionados utilizando un criterio
			// complejo basado en una conversión RGB a HSL... Da mejores
			// resultados si los colores son parecidos o están divididos
			// en bloques claramente separados.

			GET_COLORS (0, 256, &Palette);
			X = 0;
			MINH = 999999;
			FROM I = 0 TO 255:
				IF (PalSelection[I])
					ToSort[X].INDEX = I;
					R = Palette[I].R;
					G = Palette[I].G;
					B = Palette[I].B;
					MAX = (R   > G ? R : G);
					MAX = (MAX > B ? MAX : B);
					MIN = (R   < G ? R : G);
					MIN = (MIN < B ? MIN : B);
					DIF = MAX-MIN;
					IF (DIF == 0) DIF = 1; END
					R2 = (MAX-R)*256/DIF;
					G2 = (MAX-G)*256/DIF;
					B2 = (MAX-B)*256/DIF;
					IF (R == MAX)
						ToSort[X].H = (G == MIN ? 1280+B2 : 256-G2);
					ELSEIF (G == MAX)
						ToSort[X].H = (B == MIN ? 256+R2  : 768-B2);
					ELSE
						ToSort[X].H = (R == MIN ? 768+G2  : 1280-R2);
					END
					IF (MINH > ToSort[X].H)
						MINH = ToSort[X].H;
					END
					ToSort[X].L = ((ToSort[X].H-MINH)/32)*256 + (R+G+B)/3;
					X++;
				END
			END
			IF (X > 0)
				QUICKSORT (&ToSort, SIZEOF(ToSort[0]), X, 8, 4, 0);
				MEMCOPY (&Sorted, &Palette, SIZEOF(Palette));
				I = 0;
				WHILE (!PalSelection[I]) I++; END
				FROM Y = 0 TO X-1:
					Sorted[I].R = Palette[ToSort[Y].INDEX].R;
					Sorted[I].G = Palette[ToSort[Y].INDEX].G;
					Sorted[I].B = Palette[ToSort[Y].INDEX].B;
					I++;
					WHILE (!PalSelection[I]) I++; END
				END
				SET_COLORS (0, 256, &Sorted);
				ColorValue = RGB(Sorted[Color].R, Sorted[Color].G, Sorted[Color].B);
			END

		ELSEIF (BUTTON_PRESSED(Boton[8]))	
		
			// Degradado de colores
			// --------------------
			// Crea un degradado entre el primer y el último color
			// entre los seleccionados, y lo distribuye sobre el
			// resto de la selección

			Count = Pass = 0;
			FROM X = 0 TO 255: IF (PalSelection[X]) BREAK; END END
			FROM Y = 255 TO 0 STEP -1: IF (PalSelection[Y]) BREAK; END END
			FROM I = 0 TO 255: IF (PalSelection[I]) Count++; END END
			IF (X < 255 && Y > X && Count > 1)
				GET_COLORS (0, 256, &Palette);
				FROM I = X TO Y:
					IF (PalSelection[I])
						Palette[I].R = (Palette[X].R*(Count-Pass-1) + Palette[Y].R*Pass) / (Count-1);
						Palette[I].G = (Palette[X].G*(Count-Pass-1) + Palette[Y].G*Pass) / (Count-1);
						Palette[I].B = (Palette[X].B*(Count-Pass-1) + Palette[Y].B*Pass) / (Count-1);
						Pass++;
					END
				END
				SET_COLORS (0, 256, &Palette);
				ColorValue = RGB(Palette[Color].R, Palette[Color].G, Palette[Color].B);
			END

		ELSEIF (BUTTON_PRESSED(Boton[9]))	
		
			// Rotación a la izquierda
			// -----------------------
			// Desplaza a la izquierda los colores seleccionados

			GET_COLORS (0, 256, &Palette);
			GET_COLORS (0, 256, &Sorted);
			FROM X = 0 TO 255:
				IF (PalSelection[X])
					Y = X+1;
					WHILE (!PalSelection[Y])
						Y++;
						IF (Y == 256) Y = 0; END
					END
					Palette[X].R = Sorted[Y].R;
					Palette[X].G = Sorted[Y].G;
					Palette[X].B = Sorted[Y].B;
				END
			END
			SET_COLORS (0, 256, &Palette);
			ColorValue = RGB(Palette[Color].R, Palette[Color].G, Palette[Color].B);

		ELSEIF (BUTTON_PRESSED(Boton[10]))	
		
			// Rotación a la derecha
			// ---------------------
			// Desplaza a la derecha los colores seleccionados

			GET_COLORS (0, 256, &Palette);
			GET_COLORS (0, 256, &Sorted);
			FROM X = 0 TO 255:
				IF (PalSelection[X])
					Y = X-1;
					WHILE (!PalSelection[Y])
						Y--;
						IF (Y == -1) Y = 255; END
					END
					Palette[X].R = Sorted[Y].R;
					Palette[X].G = Sorted[Y].G;
					Palette[X].B = Sorted[Y].B;
				END
			END
			SET_COLORS (0, 256, &Palette);
			ColorValue = RGB(Palette[Color].R, Palette[Color].G, Palette[Color].B);

		ELSEIF (BUTTON_PRESSED(Boton[11]))	
		
			// Orden inverso
			// -------------
			// Invierte el orden de los colores seleccionados

			GET_COLORS (0, 256, &Palette);
			GET_COLORS (0, 256, &Sorted);
			Y = 256;
			FROM X = 0 TO 255:
				IF (PalSelection[X])
					Y--;
					WHILE(!PalSelection[Y])
						Y--;
						IF (Y == -1) BREAK; END
					END
					IF (Y == -1) BREAK; END
					Palette[X].R = Sorted[Y].R;
					Palette[X].G = Sorted[Y].G;
					Palette[X].B = Sorted[Y].B;
				END
			END
			SET_COLORS (0, 256, &Palette);
			ColorValue = RGB(Palette[Color].R, Palette[Color].G, Palette[Color].B);

		ELSEIF (BUTTON_PRESSED(Boton[12]))	

			DESKTOP_ADDPOPUP (0, POPUP_NEW(MenuSel), MOUSE.X, MOUSE.Y);
		
		END

		IF (Color != PreviousColor) 		
		
			// Ha sido seleccionado otro color

			GET_COLORS (Color, 1, &Palette);
			ColorValue = RGB(Palette.R, Palette.G, Palette.B);
			PreviousValue = ColorValue;
			PreviousColor = Color;

		ELSEIF (ColorValue != PreviousValue) 	
		
			// Ha sido modificado el color actual

			GET_RGB (ColorValue, &R, &G, &B);
			Palette.R = R;
			Palette.G = G;
			Palette.B = B;
			SET_COLORS (Color, 1, &Palette);
			PreviousValue = ColorValue;
			PreviousColor = Color;
		END

		IF (SCAN_CODE == _ESC || SCAN_CODE == _ENTER)
			SCAN_CODE = 0;
			BREAK;
		END
		FRAME;
	END

	WINDOW_DESTROY(Ventana);
END

