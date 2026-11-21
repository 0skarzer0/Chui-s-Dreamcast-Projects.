
GLOBAL
	ResultMap = -1;
	LAST_MAP_WINDOW;
	LAST_FPG_NUM;
	ASK_MULTIPLE = TRUE;
	EditorPalette;
	EDITORINFO DefaultOptions;

// -----------------------------------------------------------------------
//   MULTIPLE_MAP_ADD
// -----------------------------------------------------------------------
//
//   Muestra un cuadro de diálogo preguntando al usuario qué quiere
//   hacer, cuando intenta añadir múltiples gráficos a un FPG
//   (usando el icono "+" o pulsando INS)
//

PROCESS MULTIPLE_MAP_ADD ()
PRIVATE
	Ventana, B_OK, B_CANCEL, I;
	CHAR FirstID[3];
BEGIN
	IF (!ASK_MULTIPLE) RETURN; END

	IF (LAST_FPG_NUM >= 0 && LAST_FPG_NUM < 999)
		FirstID = LAST_FPG_NUM+1;
	END

	// Crea la ventana y su contenido

	SIGNAL (FATHER, S_SLEEP);
	Ventana = WINDOW(TXT_MultiAdd, STYLE_DIALOG);
	WINDOW_ADDCONTROL (Ventana,   0,  0, I = IMAGE_VIEW(GUI_FILE, 900));
	CONTROL_RESIZE (I, 80, 80);
	WINDOW_ADDCONTROL (Ventana,  90,  5, LABEL(TXT_MultiAdd1));
	WINDOW_ADDCONTROL (Ventana, 230,  5, INPUTLINE(4, FirstID));
	WINDOW_ADDCONTROL (Ventana,  90, 30, LABEL(TXT_MultiAdd2));
	WINDOW_ADDCONTROL (Ventana,  90, 42, LABEL(TXT_MultiAdd3));
	WINDOW_ADDCONTROL (Ventana, 120, 70, CHECKBOX(TXT_AskAgain, &ASK_MULTIPLE));
	WINDOW_ADDCONTROL (Ventana,  90, 100, B_OK = BUTTON(TXT_Aceptar));
	WINDOW_ADDCONTROL (Ventana, 175, 100, B_CANCEL = BUTTON(TXT_Cancelar));
	WINDOW_SET (Ventana, WINDOW_MODAL, TRUE);
	DESKTOP_ADDWINDOW (Ventana);

	WHILE (WINDOW_VISIBLE(Ventana) && !BUTTON_PRESSED(B_CANCEL))
		IF (BUTTON_PRESSED(B_OK) || SCAN_CODE == _ENTER)
			SCAN_CODE = 0;
			LAST_FPG_NUM = ATOI(FirstID)-1;
			IF (LAST_FPG_NUM < 0 OR LAST_FPG_NUM > 999)
				LAST_FPG_NUM = 0;
			END
			BREAK;
		END
		FRAME;
	END

	WINDOW_DESTROY(Ventana);
	SIGNAL (FATHER, S_WAKEUP);
END

// -----------------------------------------------------------------------
//   MULTIPLE_MAP_DROP
// -----------------------------------------------------------------------
//
//   Muestra un cuadro de diálogo preguntando al usuario qué quiere
//   hacer, cuando múltiples gráficos son arrastrados fuera de una
//   ventana FPG (crear un FPG nuevo o mostrarlos como ventana)
//

PROCESS MULTIPLE_MAP_DROP (Table, Library)
PRIVATE
	Ventana;
	Opcion;
	B_OK;
	B_CANCEL;
	I;
	Action;
BEGIN
	// Crea la ventana

	SIGNAL (FATHER, S_SLEEP);
	Ventana = WINDOW(TXT_MultiFPGDD, STYLE_DIALOG);
	WINDOW_ADDCONTROL (Ventana, 0,   0, I = IMAGE_VIEW(GUI_FILE, 900));
	WINDOW_ADDCONTROL (Ventana, 90,  5, LABEL(TXT_MultiFPGDD1));
	WINDOW_ADDCONTROL (Ventana, 90, 17, LABEL(TXT_MultiFPGDD2));
	WINDOW_ADDCONTROL (Ventana, 90, 40, RADIO(TXT_MultiFPGDDA, &Opcion, 0));
	WINDOW_ADDCONTROL (Ventana, 90, 55, RADIO(TXT_MultiFPGDDB, &Opcion, 1));
	WINDOW_ADDCONTROL (Ventana, 90, 70, RADIO(TXT_MultiFPGDDC, &Opcion, 2));
	WINDOW_ADDCONTROL (Ventana, 90, 100, B_OK = BUTTON(TXT_Aceptar));
	WINDOW_ADDCONTROL (Ventana, 175, 100, B_CANCEL = BUTTON(TXT_Cancelar));
	CONTROL_RESIZE (I, 80, 80);
	WINDOW_SET (Ventana, WINDOW_MODAL, TRUE);
	DESKTOP_ADDWINDOW (Ventana);

	// Bucle de ventana

	WHILE (WINDOW_VISIBLE(Ventana) && !BUTTON_PRESSED(B_CANCEL))
		IF (BUTTON_PRESSED(B_OK) || SCAN_CODE == _ENTER)
			SCAN_CODE = 0;
			ACTION = OPCION+1;
			BREAK;
		END
		FRAME;
	END
	WINDOW_DESTROY(Ventana);

	// Ejecuta la acción elegida por el usuario

	SWITCH(ACTION)
		CASE 1, 2:
			FILE = FPG_NEW();
			FROM X = 0 TO TABLE_LINES(TABLE)-1:
				IF (TABLE_SELECTED(TABLE, X))
					I = TABLE_GET(TABLE, X, "ID");
					FPG_ADD (FILE, I, LIBRARY, I);
					IF (ACTION == 2) 
						UNLOAD_MAP (LIBRARY, I); 
					END
				END
			END
			OPEN_FPG (FILE, TXT_Untitled);
			END
		CASE 3:
			FROM X = 0 TO TABLE_LINES(TABLE)-1:
				IF (TABLE_SELECTED(TABLE, X))
					I = TABLE_GET(TABLE, X, "ID");
					OPEN_MAP (LIBRARY, I, MAP_NAME(LIBRARY, I));
				END
			END
			END
	END
	SIGNAL (FATHER, S_WAKEUP);
END

// -----------------------------------------------------------------------
//   RESIZE_MAP
// -----------------------------------------------------------------------
//
//   Rutina de utilidad que cambia el tamaño de un gráfico,
//   empleando la función XPUT
//

PROCESS RESIZE_MAP (FILE, GRAPH, X, Y)
PRIVATE
	CX, CY, SX, SY, ZX, ZY, SDEPTH, I, NEW_MAP;
BEGIN
	// Recupera información sobre el gráfico original

	SX     = GRAPHIC_INFO(FILE, GRAPH, G_WIDE);
	SY     = GRAPHIC_INFO(FILE, GRAPH, G_HEIGHT);
	SDEPTH = GRAPHIC_INFO(FILE, GRAPH, G_DEPTH);
	CX     = GRAPHIC_INFO(FILE, GRAPH, G_CENTER_X);
	CY     = GRAPHIC_INFO(FILE, GRAPH, G_CENTER_Y);

	// Calcula la nueva posición del centro

	CX = (CX * X) / SX;
	CY = (CY * Y) / SY;

	// Calcula el factor de zoom necesario

	ZX = X * 100 / SX;
	ZY = Y * 100 / SY;

	// Crea el nuevo gráfico y dibuja sobre él el original, ya escalado

	NEW_MAP = NEW_MAP(X, Y, SDEPTH);
	MAP_SET_NAME (0, NEW_MAP, MAP_NAME(FILE, GRAPH));
	MAP_XPUTNP (0, NEW_MAP, FILE, GRAPH, CX, CY, 0, ZX, ZY, 0);
	SET_CENTER (0, NEW_MAP, CX, CY);

	// Añade los puntos de control

	X  *= 2; Y *= 2;
	SX *= 2; SY *= 2;
	FROM I = 0 TO 999:
		GET_POINT (FILE, GRAPH, I, &CX, &CY);
		IF (CX != 0 && CY != 0)
			SET_POINT (0, NEW_MAP, I, (CX+1)*X/SX, (CY+1)*Y/SY);
		END
	END

	// Sustituye el gráfico antiguo por el nuevo

	FPG_ADD (FILE, GRAPH, 0, NEW_MAP);
	UNLOAD_MAP (0, NEW_MAP);
END

// -----------------------------------------------------------------------
//   DIALOG_NEW_MAP
// -----------------------------------------------------------------------
//
//   Muestra el cuadro de diálogo de crear un MAP nuevo
//

PROCESS DIALOG_NEW_MAP()
PRIVATE
	INT Ventana, BitDepth = 16, Lab, BTOk, BTCancel, OK;
	CHAR Name[32];
	CHAR SizeW[4] = "64" ; 
	CHAR SizeH[4] = "64" ;
BEGIN
	// Crea el cuadro de diálogo

	Ventana = WINDOW (TXT_NewMap, STYLE_DIALOG);
	WINDOW_SET (Ventana, WINDOW_MODAL, TRUE);
	WINDOW_ADDCONTROL (Ventana,   0, 0, Label(TXT_NombreDLG));
	WINDOW_ADDCONTROL (Ventana,  80, 0, InputLine(16, Name));
	WINDOW_ADDCONTROL (Ventana,  0, 20, Label(TXT_SizeDLG));
	WINDOW_ADDCONTROL (Ventana, 80, 20, InputLine(4, SizeW));
	WINDOW_ADDCONTROL (Ventana,120, 20, Label("x"));
	WINDOW_ADDCONTROL (Ventana,130, 20, InputLine(4, SizeH));
	WINDOW_ADDCONTROL (Ventana,171, 20, Label("pixels"));
	WINDOW_ADDCONTROL (Ventana, 80, 40, Lab = Label("", 0, 100));
	WINDOW_ADDCONTROL (Ventana,  0, 60, Label(TXT_ColorsDLG));
	WINDOW_ADDCONTROL (Ventana, 80, 60, RADIO("8 bits  ", &BitDepth, 8));
	WINDOW_ADDCONTROL (Ventana, 80, 75, RADIO("16 bits ", &BitDepth, 16));
	WINDOW_ADDCONTROL (Ventana, 30, 100, BTOk = BUTTON(TXT_Aceptar));
	WINDOW_ADDCONTROL (Ventana,140, 100, BTCancel = BUTTON(TXT_Cancelar));

	// Bucle de ventana

	DESKTOP_ADDWINDOW(Ventana);
	SIGNAL (FATHER, S_SLEEP);

	SCAN_CODE = 0;

	WHILE (WINDOW_VISIBLE(Ventana))

		// Actualiza la línea de texto a medida que el usuario hace cambios

		LABEL_TEXT (Lab, FORMAT((ATOI(SizeW) * ATOI(SizeH) * BitDepth / 8 + 1023) / 1024) + " KB");

		// Simplemente salimos cuando el usuario pulse OK o CANCEL
		// o bien pulse ENTER o ESCAPE

		IF (BUTTON_PRESSED(BTOk) OR SCAN_CODE == _ENTER)
			SCAN_CODE = 0;
			OK = TRUE;
			BREAK;
		ELSEIF (BUTTON_PRESSED(BTCancel) OR SCAN_CODE == _ESC)
			SCAN_CODE = 0;
			BREAK;
		END
		FRAME;
	END

	WINDOW_DESTROY(Ventana);

	// Crea el mapa y sale

	IF (OK) 
		GRAPH = NEW_MAP (SizeW, SizeH, BitDepth);
		IF (Name != "") MAP_SET_NAME (0, GRAPH, Name); END
		OPEN_MAP (0, GRAPH, Name == "" ? TXT_Untitled : Name); 
	END

	SIGNAL (FATHER, S_WAKEUP);
END

// -----------------------------------------------------------------------
//   DIALOG_MAP
// -----------------------------------------------------------------------
//
//   Muestra el cuadro de diálogo de propiedades de un MAP
//

PROCESS DIALOG_MAP (INT Library, INT Map, STRING Title)
PRIVATE
	INT Ventana, BitDepth, BTOk, BTCancel, IV, Copy, Lab;
	INT TablaPuntos, I;
	INT B_Nuevo, B_Quitar, B_Editar;
	CHAR Name[32];
	CHAR SizeW[4]; 
	CHAR SizeH[4];
	CHAR Identificador[3];
	INT OK, Refill = TRUE;
	EditorInfo info;
BEGIN
	// Especifica el modo de edición de puntos de control

	info.Zoom = 100;
	WHILE (GRAPHIC_INFO(Library,Map,G_WIDTH)*info.Zoom/50 < 350 &&
	       GRAPHIC_INFO(Library,Map,G_HEIGHT)*info.Zoom/50 < 350)
	       info.Zoom *= 2;
	END
	info.Tool = TOOL_CONTROLPOINT;
	copy = MAP_CLONE(Library, Map);

	// Guarda las propiedades del mapa en las variables privadas
	// que posteriormente utilizarán los controles

	Name = MAP_NAME(Library, Map);
	SizeW = GRAPHIC_INFO(Library, Map, G_WIDE);
	SizeH = GRAPHIC_INFO(Library, Map, G_HEIGHT);
	BitDepth = GRAPHIC_INFO(Library, Map, G_DEPTH);
	Identificador = Map;

	// Crea la ventana y añade los controles

	IF (Title == "") Title = Name; END
	IF (Title == "") Title = TXT_Properties+"..."; END
	Ventana = WINDOW(Title, STYLE_DIALOG);
	IV = GREDITOR(0, Copy, &Info, 350, 350);
	GREDITOR_ACTIVE (IV, TRUE);
	WINDOW_ADDCONTROL (Ventana,   0, 0, Label(TXT_NombreDLG));
	WINDOW_ADDCONTROL (Ventana,  80, 0, InputLine(20, Name));
	WINDOW_ADDCONTROL (Ventana,  0, 20, Label(TXT_CodeDLG));

	// El cuadro de código sólo está disponible en mapas
	// que forman parte de una librería FPG

	IF (LIBRARY != 0)
		WINDOW_ADDCONTROL (Ventana, 80, 20, InputLine(20, Identificador));
	ELSE
		WINDOW_ADDCONTROL (Ventana, 80, 20, Label(TXT_CodeNone));
	END

	WINDOW_ADDCONTROL (Ventana,  0, 40, Label(TXT_SizeDLG));
	WINDOW_ADDCONTROL (Ventana, 80, 40, InputLine(4, SizeW));
	WINDOW_ADDCONTROL (Ventana,120, 40, Label("x"));
	WINDOW_ADDCONTROL (Ventana,130, 40, InputLine(4, SizeH));
	WINDOW_ADDCONTROL (Ventana,171, 40, Label("pixels"));
	WINDOW_ADDCONTROL (Ventana, 80, 60, Lab = Label("", 0, 100));
	WINDOW_ADDCONTROL (Ventana,  0, 80, Label(TXT_ColorsDLG));
	WINDOW_ADDCONTROL (Ventana, 80, 80, RADIO("8 bits  ", &BitDepth, 8));
	WINDOW_ADDCONTROL (Ventana, 80, 95, RADIO("16 bits ", &BitDepth, 16));
	WINDOW_ADDCONTROL (Ventana, 30, 320, BTOk = BUTTON(TXT_Aceptar));
	WINDOW_ADDCONTROL (Ventana,140, 320, BTCancel = BUTTON(TXT_Cancelar));
	WINDOW_ADDCONTROL (Ventana,  0, 110, Label(TXT_CPoints));
	WINDOW_ADDCONTROL (Ventana, 80, 110, TablaPuntos = TABLE(160, 190));
	WINDOW_ADDCONTROL (Ventana, 30, 260, B_Quitar = BUTTON("-"));
	WINDOW_ADDCONTROL (Ventana, 30, 280, B_Nuevo = BUTTON("+"));
	WINDOW_ADDCONTROL (Ventana, 260, 0, IV);
	CONTROL_RESIZE (B_Nuevo, 40, 20);
	CONTROL_RESIZE (B_Quitar, 40, 20);

	// Crea la tabla de puntos de control

	TABLE_ADDCOLUMN (TablaPuntos, TXT_Points);
	TABLE_ADDCOLUMN (TablaPuntos, "X");
	TABLE_ADDCOLUMN (TablaPuntos, "Y");
	TABLE_WIDTH (TablaPuntos, 1, 40);
	TABLE_WIDTH (TablaPuntos, 2, 40);
	TABLE_ALIGNMENT (TablaPuntos, 1, 1);
	TABLE_ALIGNMENT (TablaPuntos, 2, 1);
	TABLE_AUTOEXPAND (TablaPuntos, 0);
	TABLE_SET (TablaPuntos, TABLE_RESIZABLE_COLUMNS, 0);
	TABLE_SET (TablaPuntos, TABLE_SORTABLE_COLUMNS, 0);
	TABLE_SET (TablaPuntos, TABLE_MOVEABLE_COLUMNS, 0);

	WINDOW_SET (Ventana, WINDOW_MODAL, TRUE);

	// Proceso principal de ventana (observación: dormimos al proceso padre)

	DESKTOP_ADDWINDOW(Ventana);
	SIGNAL (FATHER, S_SLEEP);

	SCAN_CODE = 0;

	WHILE (WINDOW_VISIBLE(Ventana))

		// Actualiza la tabla a medida que el usuario hace cambios

		IF (Refill || CONTROL_CHANGED(IV));
			TABLE_REMOVELINES (TablaPuntos, 0, 0);
			GET_POINT (0, Copy, 0, &X, &Y);
			TABLE_ADDLINE (TablaPuntos, TXT_Center, X, Y);
			FROM I = 1 TO 999:
				IF (GET_POINT (0, Copy, I, &X, &Y))
					TABLE_ADDLINE (TablaPuntos, I, X, Y);
				END
			END
			TABLE_SELECT (TablaPuntos, INFO.CPOINT);
			Refill = FALSE;
		ELSEIF (CONTROL_CHANGED(TablaPuntos))
			INFO.CPOINT = TABLE_SELECTED(TablaPuntos);
		ELSE
			TABLE_SELECT (TablaPuntos, INFO.CPOINT);
		END

		LABEL_TEXT (Lab, FORMAT((ATOI(SizeW) * ATOI(SizeH) * BitDepth / 8 + 1023) / 1024) + " KB");

		// Responde a los botones de añadir o quitar puntos de control

		IF (BUTTON_PRESSED(B_Nuevo))
			FROM I = 1 TO 999:
				IF (!GET_POINT(0, Copy, I, &X, &Y))
					GET_POINT (0, Copy, INFO.CPOINT, &X, &Y);
					SET_POINT (0, Copy, I, X, Y);
					INFO.CPOINT = I;
					Refill = TRUE;
					BREAK;
				END
			END
		ELSEIF (BUTTON_PRESSED(B_Quitar) && INFO.CPOINT > 0)
			SET_POINT (0, Copy, INFO.CPOINT, -1, -1);
			WHILE (!GET_POINT(0, Copy, INFO.CPOINT, &X, &Y))
				INFO.CPOINT--;
			END
			Refill = TRUE;
		END

		// Simplemente salimos cuando el usuario pulse OK o CANCEL
		// o bien pulse ENTER o ESCAPE

		IF (BUTTON_PRESSED(BTOk) OR SCAN_CODE == _ENTER)
			OK = TRUE;
			BREAK;
		ELSEIF (BUTTON_PRESSED(BTCancel) OR SCAN_CODE == _ESC)
			BREAK;
		END
		FRAME;
	END
	SCAN_CODE = 0;

	// Si el usuario salió pulsando Aceptar, hay que cambiar el gráfico acordemente

	IF (OK)
		// Copia el gráfico sobre el original, por si fue editado

		FPG_ADD (Library, Map, 0, Copy);

		// Cambio de nombre

		MAP_SET_NAME (Library, Map, Name);

		// Cambio de tamaño

		IF (ATOI(SizeW) != GRAPHIC_INFO(Library, Map, G_WIDE) ||
		    ATOI(SizeH) != GRAPHIC_INFO(Library, Map, G_HEIGHT))
		    	CLOSE_MAP (Library, Map);
		    	RESIZE_MAP (Library, Map, SizeW, SizeH);
		END

		// Cambio de profundidad de color

		IF (ATOI(BitDepth) != GRAPHIC_INFO(Library, Map, G_DEPTH))
			MAP_SETDEPTH (Library, Map, BitDepth);
		END

		// Cambio de identificador

		IF (Library != 0 && ATOI(Identificador) != Map)

			// Ya existe un gráfico con ese código

			IF (MAP_EXISTS(Library, Identificador))
				MessageBox (TXT_MAPDup,
					TXT_MAPDup1+Identificador+CHR(10)+
					TXT_MAPDup2+MAP_NAME(Library, Identificador)+"."+CHR(10)+
					TXT_MAPDup3, MB_ALERT | MB_YESNO);
				IF (!MB_RESULT)
					Identificador = Map;
				ELSE

					// Se eligió sobreescribirlo

					CLOSE_MAP (Library, ATOI(Identificador));
					UNLOAD_MAP (Library, Identificador);
				END
			END

			// Asignación del nuevo identificador

			IF (ATOI(Identificador) != Map)
				CLOSE_MAP (Library, Map);
				FPG_ADD (Library, Identificador, Library, Map);
				UNLOAD_MAP (Library, Map);
				IF (LAST_FPG_NUM == MAP)
					LAST_FPG_NUM = IDENTIFICADOR;
				END
			END
		END
		ResultMap = ATOI(Identificador);
	ELSE
		ResultMap = -1;
	END

	// Fin de proceso

	UNLOAD_MAP (0, Copy);
	WINDOW_DESTROY(Ventana);
	SIGNAL (FATHER, S_WAKEUP);
END

// -----------------------------------------------------------------------
//   ACTION_PROPERTIES
// -----------------------------------------------------------------------
//
//   Muestra un cuadro de diálogo con propiedades de la ventana activa
//   (si se trata de una ventana de MAP)
//

PROCESS ACTION_PROPERTIES()
PRIVATE
	Ventana;
BEGIN
	Ventana = DESKTOP_TOPWINDOW();
	IF (WINDOW_TYPE(Ventana) == "MAP")
		IF (REGEX ("^([0-9]*),([0-9]*) ", WINDOW_DATA(Ventana)) != -1)
			DIALOG_MAP (REGEX_REG[1], REGEX_REG[2], "");
		END
	END
END

// -----------------------------------------------------------------------
//   OPEN_MAP
// -----------------------------------------------------------------------
//
//   Abre la ventana de un gráfico MAP. Las ventanas MAP tienen por
//   estilo de ventana "MAP" y por datos internos "Library,Map Name" 
//   donde Library y Map son los números de librería y mapa, y
//   Name el nombre de fichero.
//

PROCESS OPEN_MAP (INT Library, Int Map, STRING Name)
PRIVATE
	Ventana, I, W, H, Editor;
	MAP_MENU, ZOOM_MENU;
	EditorInfo info;
	STRING Title;
BEGIN

	// Crea el menú contextual

	ZOOM_MENU = MENU();
	MAP_MENU = MENU();
	MENU_RADIO (ZOOM_MENU,  "25%", &INFO.ZOOM, 25);
	MENU_RADIO (ZOOM_MENU,  "50%", &INFO.ZOOM, 50);
	MENU_RADIO (ZOOM_MENU, "&100%", &INFO.ZOOM, 100);
	MENU_RADIO (ZOOM_MENU, "&200%", &INFO.ZOOM, 200);
	MENU_RADIO (ZOOM_MENU, "&300%", &INFO.ZOOM, 300);
	MENU_RADIO (ZOOM_MENU, "&500%", &INFO.ZOOM, 500);
	MENU_SEPARATOR (ZOOM_MENU);
	MENU_RADIO (ZOOM_MENU, TXT_ZoomMax, &INFO.ZOOM, 0);
	MENU_ACTION (MAP_MENU, TXT_Guardar, TYPE ACTION_SAVE);
	MENU_ACTION (MAP_MENU, TXT_GuardarComo, TYPE ACTION_SAVEAS);
	MENU_SEPARATOR (MAP_MENU);
	MENU_SUBMENU (MAP_MENU, TXT_Zoom, ZOOM_MENU);
	MENU_SEPARATOR (MAP_MENU);
	MENU_ACTION (MAP_MENU, TXT_Propiedades, TYPE ACTION_PROPERTIES);
	
	// Calcula un factor de zoom adecuado, a ojo, para que la imagen
	// no aparezca dentro de una ventana muy pequeña

	W = GRAPHIC_INFO(Library, Map, G_WIDTH);
	H = GRAPHIC_INFO(Library, Map, G_HEIGHT);

	INFO.ZOOM = 100;
	WHILE (INFO.ZOOM * W / 100 < 150 AND INFO.ZOOM * H / 100 < 100)
		INFO.ZOOM += 100;
	END
	WHILE (INFO.ZOOM * W / 100 > 512 OR INFO.ZOOM * H / 100 > 400)
		INFO.ZOOM /= 2;
	END

	W = W * INFO.ZOOM / 100;
	H = H * INFO.ZOOM / 100;

	INFO.BRUSHFILE = BrushFile;
	INFO.TEXTUREFILE = TextureFile;

	// Copia los parámetros por defecto

	INFO.GridStep = DefaultOptions.GridStep;
	INFO.GridVisible = DefaultOptions.GridVisible;
	INFO.GridColor = DefaultOptions.GridColor;
	INFO.PaperMode = DefaultOptions.PaperMode;
	INFO.PaperColor = DefaultOptions.PaperColor;
	INFO.PixelGridColor = DefaultOptions.PixelGridColor;

	// Crea la ventana y el control de imagen

	Title = MAP_NAME(Library, Map);
	IF (Title == "") Title = TXT_Untitled; END
	Ventana = LAST_MAP_WINDOW = WINDOW (Title, STYLE_NORMAL);
	WINDOW_ADDCONTROL (Ventana, 0, 0, EDITOR = GREDITOR(Library, Map, &INFO, W, H));
	CONTROL_DRAGGABLE (EDITOR, "FPG:"+Library+","+Map, Library, Map);
	DESKTOP_ADDWINDOW(Ventana);
	WINDOW_TYPE (Ventana, "MAP");
	WINDOW_DATA (Ventana, ITOA(Library)+","+Map+" "+Name);
	WINDOW_SET (Ventana, WINDOW_FULLSCREEN, TRUE);

	// Proceso principal de ventana

	IF (GLOB(Name) != "")
		WINDOW_FILE (Ventana, FILEINFO.PATH + FILEINFO.NAME);
	END

	WHILE (MAP_EXISTS(Library,Map))

		// Si se cierra la ventana sin guardar cambios, pedir confirmación
		// (sólo para mapas que no pertenecen a ningún FPG abierto)

		IF (!WINDOW_VISIBLE(Ventana))
			IF (Library == 0 AND WINDOW_CHANGED(Ventana))
				I = ShowEdPal;
				ShowEdPal = FALSE;
				DESKTOP_ADDWINDOW(Ventana);
				MessageBox (TXT_Salir0, TXT_Salir3 + WINDOW_TITLE(Ventana) + TXT_Salir4, MB_DANGER | MB_YESNO);
				FRAME;
				ShowEdPal = I;
				IF (MB_RESULT == FALSE) CONTINUE; END
			END
			BREAK;
		END

		IF (DESKTOP_TOPWINDOW() == Ventana)

			IF (WINDOW_INFO(Ventana, WINDOW_MAXIMIZED))
				GREDITOR_SHOWBARS (Editor, FALSE);
			ELSE
				GREDITOR_SHOWBARS (Editor, TRUE);
			END

			IF (!GREDITOR_ACTIVE (Editor))
				IF (CONTROL_DOUBLECLICK(Editor))
					GREDITOR_ACTIVE (Editor, TRUE);
				END
			ELSE
				IF (CONTROL_CHANGED(Editor))
					WINDOW_CHANGED (Ventana, TRUE);
				END
				CONTROL_DOUBLECLICK(Editor);
			END

			// Actualiza las ventanas accesorias (paleta, toolbox, etc)
			// con la información referente al actual editor

			GRPALETTE_SET (EditorPalette, &info);
			PALETTE_PANEL_SET (PalettePanel, &info.color8);

			// Interpreta teclas de acceso rápido

			SWITCH (SCAN_CODE)
				CASE _MENU:
					SCAN_CODE = 0;
					DESKTOP_ADDPOPUP (0, POPUP_NEW(MAP_MENU), MOUSE.X, MOUSE.Y);
					END
				CASE _F2:
					IF (KEY(_ALT))
						SCAN_CODE = 0;
						DIALOG_MAP (Library, Map, MAP_NAME(Library,Map));
						FRAME;
					END
					END
				CASE _ESC:
					SCAN_CODE = 0;
					IF (!GREDITOR_ACTIVE(Editor))
						DESKTOP_REMOVEWINDOW(Ventana);
					ELSE
						GREDITOR_ACTIVE(Editor, FALSE);
					END
					END
			END

			// Si el usuario mantiene TAB pulsado tratamos de hacer una
			// animación buscando otros gráficos en el mismo FPG

			IF (KEY(_TAB) && TIMER[0] > FPS/10 && LIBRARY != 0)
				TIMER[0] = 0;
				IF (MAP_EXISTS(Library, Map+1))
					Map++;
				ELSE
					WHILE (MAP_EXISTS(Library, Map-1)) Map--; END
				END
				INFO.GRAPH = MAP;
				WINDOW_TITLE (Ventana, MAP_NAME(Library, Map));
			END
		END
		FRAME;
	END

	DESKTOP_REMOVEWINDOW(Ventana);
	WINDOW_DESTROY(Ventana);
	MENU_DESTROY (ZOOM_MENU);
	MENU_DESTROY (MAP_MENU);
END

// -----------------------------------------------------------------------------
//   CLOSE_MAP
// -----------------------------------------------------------------------------
//
//   Cierra cualquier ventana de MAP abierta con el gráfico especificado.
//   Se utiliza al borrar el mapa del FPG correspondiente.
//

PROCESS CLOSE_MAP (INT Library, INT Map)
PRIVATE 
	Ventana;
	STRING Data;
BEGIN
	Data = "MAP"+Library+","+Map;
	Ventana = DESKTOP_BOTTOMWINDOW();
	WHILE (Ventana != 0)
		IF (WINDOW_TYPE(Ventana) == "MAP" && WINDOW_DATA(Ventana) == Data)
			DESKTOP_REMOVEWINDOW(Ventana);
		END
		Ventana = DESKTOP_NEXTWINDOW(Ventana);
	END
END

// -----------------------------------------------------------------------------
//   FILL_TABLE
// -----------------------------------------------------------------------------
//
//   Rutina de utilidad que actualiza el contenido de la tabla de una
//   ventana FPG con la lista de gráficos en la librería.
//
//   Si el parámetro SELECTED es -1, se intenta preservar la selección.
//   En caso contrario, se intenta seleccionar el mapa cuyo ID sea ese.
//
//   Si el parámetro RESIZE es FALSE, se intenta preservar el ancho
//   de la columna de Nombre. ADDLINE lo suele ajustar automáticamente.
//   Normalmente se le llama con RESIZE = TRUE sólo durante la primera
//   vez que se añade contenido a la tabla.
//

PROCESS STRING FILL_FPGTABLE (TABLE, LIBRARY, SELECTED, RESIZE)
PRIVATE
	I, C, CWIDTH;
	BYTES, COUNT;
BEGIN
	// Guarda el ancho de la columna Nombre

	CWIDTH = TABLE_WIDTH (TABLE, TXT_Nombre);

	// Guarda la opción seleccionada, si desea preservarse

	IF (SELECTED < 0)
		SELECTED = TABLE_SELECTED(TABLE);
		IF (SELECTED >= 0)
			SELECTED = TABLE_GET (TABLE, SELECTED, "ID");
		ELSE
			SELECTED = -1;
		END
	END

	// Borra todo el contenido de la tabla

	TABLE_REMOVELINES(TABLE, 0, 0);

	// Añade el nuevo contenido

	FROM I = 0 TO 999:
		IF (MAP_EXISTS(Library, I))
			COUNT++;
			BYTES += GRAPHIC_INFO(Library, I, G_WIDTH) * 
			         GRAPHIC_INFO(Library, I, G_HEIGHT) * 
			         GRAPHIC_INFO(Library, I, G_DEPTH) / 8;

			TABLE_ADDLINE (TABLE, I, I, MAP_NAME(Library, I),
			    GRAPHIC_INFO(Library, I, G_DEPTH),
			    "" + GRAPHIC_INFO(Library, I, G_WIDE) +
			    "x" + GRAPHIC_INFO(Library, I, G_HEIGHT));

			// Preserva la selección
			IF (SELECTED == I) TABLE_SELECT(TABLE, C); END

			C++;
		END
	END

	// Preserva el ancho de la columna Nombre

	IF (!RESIZE)
		TABLE_WIDTH (TABLE, TXT_Nombre, CWIDTH);
	END

	RETURN COUNT + " maps: " + FORMAT((BYTES + 1023)/1024) + " KB";
END

// -----------------------------------------------------------------------------
//   ADD_TOFPG
// -----------------------------------------------------------------------------
//
//   Rutina de utilidad que añade el destino de una operación Drag & Drop
//   a una librería FPG
//

PROCESS ADD_TOFPG (LIBRARY, STRING Data, INT DEST_NUM)
PRIVATE
	STRING Remaining;
	INT    MAP;
	INT    NEXT_NUM;
BEGIN
	REGEX("^FPG:(.*),(.*)$", Data);
	FILE = REGEX_REG[1];
	DATA = REGEX_REG[2];

	// Escoge un número adecuado para el siguiente gráfico

	IF (DEST_NUM == -1)
		DEST_NUM = ATOI(Data);
	END

	IF (DEST_NUM < 1 OR DEST_NUM > 999)
		DEST_NUM = LAST_FPG_NUM+1;
		IF (DEST_NUM == 1000)
			DEST_NUM = 1;
		END
	END

	NEXT_NUM = DEST_NUM;

	LOOP
		// Busca el siguiente número de mapa libre

		WHILE (MAP_EXISTS(LIBRARY, DEST_NUM))
			IF (++DEST_NUM == 1000) 
				DEST_NUM = 1;
			END
			IF (DEST_NUM == NEXT_NUM)
				MessageBox (TXT_FPGTooBig, TXT_FPGTooBig1, MB_DANGER);
				RETURN;
			END
		END

		// Añade el mapa y pasa al siguiente, si quedan más

		LAST_FPG_NUM = DEST_NUM;
		FPG_ADD (LIBRARY, DEST_NUM, FILE, ATOI(Data));
		MAP_SET_NAME (LIBRARY, DEST_NUM, REGEX_REPLACE (
			"\.[a-z]{2,4}$", "", MAP_NAME(LIBRARY, DEST_NUM)));
		IF (FIND(Data, "+") > 0)
			Data = SUBSTR(Data, FIND(Data,"+")+1);
		ELSE
			BREAK;
		END
	END
END

// -----------------------------------------------------------------------------
//   OPEN_FPG
// -----------------------------------------------------------------------------
//
//   Muestra la ventana de un FPG. Esta ventana contiene una lista con
//   todos los gráficos del FPG y permite al usuario interactuar con
//   ellos (arrastrarlos a otra ventana del FPG o fuera del mismo, 
//   editar sus propiedades, etc).
//
//   Una ventana FPG tiene por tipo "FPG" y por datos su código de
//   librería seguido del nombre de fichero, separado por un espacio.
//

PROCESS OPEN_FPG (INT Library, STRING Name)
PRIVATE 
	Ventana, TABLE, I, TB, NUM, NEW, MAP, C;
	SELECTED = -1, SELECTED_COUNT, OPTION;
	VIEW_MODE;
	INFO_LABEL;

	STRING Str, Remaining, Extension, Filename;
BEGIN
	// Tal vez ya exista la ventana, en cuyo caso no la crea de nuevo

	IF (GLOB(Name) != "")
		Filename = FILEINFO.PATH + FILEINFO.NAME;
		Ventana = DESKTOP_BOTTOMWINDOW();
		WHILE (Ventana != 0)
			IF (WINDOW_FILE(Ventana) == Filename)
				DESKTOP_BRINGTOFRONT(Ventana);
				RETURN;
			END
			Ventana = DESKTOP_NEXTWINDOW(Ventana);
		END
	END

	// Crea la ventana 

	Ventana = WINDOW (Name, STYLE_NORMAL);
	WINDOW_FILE (Ventana, Filename);
	WINDOW_TYPE (Ventana, "FPG");
	WINDOW_DATA (Ventana, ITOA(Library)+" "+Name);

	// Crea la tabla que pasará a contener los gráficos

	TABLE = TABLE (320, 250);
	TABLE_ADDCOLUMN_BITMAPS (TABLE, "  ");
	TABLE_LIBRARY   (TABLE, "  ", Library);
	TABLE_ADDCOLUMN (TABLE, "ID");
	TABLE_ALIGNMENT (TABLE, "ID", 2);
	TABLE_ADDCOLUMN (TABLE, TXT_Nombre);
	TABLE_AUTOEXPAND(TABLE, TXT_Nombre);
	TABLE_ADDCOLUMN (TABLE, TXT_Bits);
	TABLE_ALIGNMENT (TABLE, TXT_Bits, 1);
	TABLE_ADDCOLUMN (TABLE, TXT_Tamaño);
	TABLE_ALIGNMENT (TABLE, TXT_Tamaño, 2);
	TABLE_SET (TABLE, TABLE_MULTISELECT, 1);
	WINDOW_ADDCONTROL (Ventana, 0, 15, TABLE);

	// Crea la barra de herramientas

	WINDOW_ADDCONTROL (Ventana, 0, 0, TB = TOOLBAR());
	TOOLBAR_BUTTON (TB, GUI_FILE, 1, TXT_AsList, &VIEW_MODE, 0);
	TOOLBAR_BUTTON (TB, GUI_FILE, 2, TXT_AsIcons, &VIEW_MODE, 1);
	TOOLBAR_SEPARATOR(TB);
	TOOLBAR_BUTTON (TB, GUI_FILE, 3, TXT_AddMAP);
	TOOLBAR_BUTTON (TB, GUI_FILE, 4, TXT_DupMAP);
	TOOLBAR_BUTTON (TB, GUI_FILE, 5, TXT_DelMAP);
	TOOLBAR_SEPARATOR(TB);
	TOOLBAR_BUTTON (TB, GUI_FILE, 7, TXT_ShowMAP);
	TOOLBAR_BUTTON (TB, GUI_FILE, 6, TXT_EditMAP);
	TOOLBAR_SEPARATOR(TB);

	WINDOW_ADDCONTROL (Ventana, CONTROL_INFO(TB, CONTROL_WIDTH) + 8, 0,
		INFO_LABEL = LABEL("", 2, 312 - CONTROL_INFO(TB, CONTROL_WIDTH)));

	// Rellena la tabla con el contenido inicial

	LABEL_TEXT (INFO_LABEL, FILL_FPGTABLE (TABLE, Library, 0, TRUE));

	// Bucle principal de ventana

	DESKTOP_ADDWINDOW (Ventana);
	WHILE (TRUE)

		// Pide confirmación si el usuario intenta cerrar la ventana
		// pero no ha grabado los cambios

		IF (NOT WINDOW_VISIBLE(Ventana))
			IF (WINDOW_CHANGED(Ventana))
				DESKTOP_ADDWINDOW(Ventana);
				MessageBox (TXT_Salir0, TXT_Salir3 + WINDOW_TITLE(Ventana) + TXT_Salir4, MB_DANGER | MB_YESNO);
				FRAME;
				IF (MB_RESULT == FALSE) CONTINUE; END
			END
			BREAK;
		END

		// Si el usuario selecciona una o más líneas, activa
		// la facilidad Drag & Drop en la tabla

		IF (CONTROL_CHANGED(TABLE) || TABLE_SELECTEDCOUNT(TABLE) != SELECTED_COUNT)

			IF (TABLE_SELECTEDCOUNT(TABLE) > 1)

				// Múltiples líneas seleccionadas: crear una descripción
				// que contenga todos los números seleccionados, separados por "+"

				SELECTED_COUNT = TABLE_SELECTEDCOUNT(TABLE);
				Str = "";
				FROM I = 0 TO TABLE_LINES(TABLE)-1:
					IF (TABLE_SELECTED(TABLE, I))
						NUM = TABLE_GET(TABLE, I, "ID");
						IF (Str == "")
							Str = "FPG:" + Library + "," + NUM;
						ELSE
							Str += "+" + NUM;
						END
					END
				END
				CONTROL_DRAGGABLE (TABLE, Str);

			ELSEIF (TABLE_SELECTEDCOUNT(TABLE) == 0)

				// No hay selección

				CONTROL_DRAGGABLE (TABLE, "");

			ELSE

				// Sólo una línea seleccionada

				SELECTED = TABLE_SELECTED(TABLE);
				NUM = TABLE_GET(TABLE, SELECTED, "ID");
				CONTROL_DRAGGABLE (TABLE, "FPG:" + Library + "," + NUM, Library, NUM);
			END

		END

		// Si el usuario está haciendo Drag & Drop, controla el destino
		// de la operación y la ejecuta si es preciso

		CONTROL_POINTER (TABLE, POINTER_ARROW);
		IF (GUI_DRAGGING() != 0)

			// Todos los objetos Drag & Drop tienen esta sintaxis

			REGEX("^FPG:(.*),(.*)$", GUI_DRAGTYPE());

			IF (ATOI(REGEX_REG[1]) == Library)

				// El usuario está arrastrando un gráfico nuestro...
			
				IF (GUI_DRAGDESTINATION() == TABLE)

					// ...Sobre nuestra propia tabla...

					GUI_ACCEPTDRAG();
					CONTROL_POINTER (TABLE, POINTER_ARROWCOPY);
					IF (GUI_DRAGGING() == DRAG_DROPPED)
						
						// El usuario ha dejado caer el control
					
						FROM I = 1 TO 1000:
							IF (!MAP_EXISTS(LIBRARY, I)) BREAK; END
						END
						IF (I > 999)
							MessageBox (TXT_FPGTooBig, TXT_FPGTooBig1, MB_ALERT);
						ELSE
							ADD_TOFPG (LIBRARY, GUI_DRAGTYPE(), 0);
							LABEL_TEXT (INFO_LABEL, FILL_FPGTABLE (TABLE, Library, I, 0));
							WINDOW_CHANGED (Ventana, TRUE);
							SELECTED = -1;
						END
					END

				ELSEIF (GUI_DRAGDESTINATION() == DRAG_ATDESKTOP)

					// ...Sobre el escritorio...

					GUI_ACCEPTDRAG();
					IF (GUI_DRAGGING() == DRAG_DROPPED)

						// Lo ha dejado caer, así que creamos la ventana del mapa
						// y además la movemos para que quede centrada sobre la
						// posición donde lo ha soltado en el escritorio
					
						IF (FIND(GUI_DRAGTYPE(), "+") >= 0)
							MULTIPLE_MAP_DROP (TABLE, LIBRARY);
							FRAME;
							LABEL_TEXT (INFO_LABEL, FILL_FPGTABLE (TABLE, Library, I, 0));
						ELSE
							OPEN_MAP (LIBRARY, REGEX_REG[2], ":EMBEDDED");
							WINDOW_MOVE (LAST_MAP_WINDOW, 
								MOUSE.X-WINDOW_INFO(LAST_MAP_WINDOW,WINDOW_WIDTH)/2, 
								MOUSE.Y-WINDOW_INFO(LAST_MAP_WINDOW,WINDOW_HEIGHT)/2-
									WINDOW_INFO(LAST_MAP_WINDOW,WINDOW_CAPTION));
						END
					END
				END

				// Si el usuario desplaza un gráfico nuestro sobre otra librería,
				// no hacemos nada, ya que cada proceso de ventana FPG es el
				// responsable de los objetos que se dejan caer sobre ella

			ELSEIF (GUI_DRAGDESTINATION() == TABLE && REGEX("^FPG:", GUI_DRAGTYPE()) != -1)

				// El usuario está desplazando aquí un gráfico, pero de otra librería

				GUI_ACCEPTDRAG();
				CONTROL_POINTER (TABLE, POINTER_ARROWCOPY);
				IF (GUI_DRAGGING() == DRAG_DROPPED)

					// ...y lo acaba de dejar caer

					FROM I = 1 TO 1000:
						IF (!MAP_EXISTS(LIBRARY, I)) BREAK; END
					END
					IF (I > 999)
						MessageBox (TXT_FPGTooBig, TXT_FPGTooBig1, MB_ALERT);
					ELSE
						ADD_TOFPG (LIBRARY, GUI_DRAGTYPE(), 0);
						LABEL_TEXT (INFO_LABEL, FILL_FPGTABLE (TABLE, Library, I, 0));
						WINDOW_CHANGED (Ventana, TRUE);
						SELECTED = -1;
						DESKTOP_BRINGTOFRONT(Ventana);
					END
				END
			END
		END

		// Fin de la gestión de Drag & Drop sobre ventanas FPG
		// Ahora, comprobar si el usuario hace clic en un botón de la barra de herramientas

		OPTION = TOOLBAR_PRESSED(TB);
		IF (OPTION == -1 AND DESKTOP_TOPWINDOW() == Ventana)

			// No hay clic, pero somos la ventana en primer plano,
			// así que verificar si pulsa alguna tecla de acceso rápido

			IF (SCAN_CODE == _INS)   OPTION = 3; END
			IF (SCAN_CODE == _DEL)   OPTION = 5; END
			IF (SCAN_CODE == _ENTER) OPTION = 7; END
			IF (SCAN_CODE == _F2)    OPTION = 8; END
			IF (SCAN_CODE == _D && KEY(_CONTROL)) OPTION = 4; END
			IF (OPTION != -1) SCAN_CODE = 0; END

			// Doble clic sobre la tabla equivale a pulsar ENTER

			IF (CONTROL_DOUBLECLICK(TABLE)) OPTION = 7; END
		END

		// Actúa frente a acciones elegidas en el menú principal

		IF (MenuAction != "" && DESKTOP_TOPWINDOW() == Ventana)

			SWITCH (MenuAction)

				// Convertir los mapas seleccionados a 8 o 16 bits

				CASE "CONVERT8", "CONVERT16":

					MenuAction = SUBSTR(MenuAction, 7);
					IF (TABLE_SELECTEDCOUNT(TABLE) > 0)
						// Sólo los seleccionados
						FROM I = 0 TO TABLE_LINES(TABLE)-1:
							IF (TABLE_SELECTED(TABLE, I))
								NUM = TABLE_GET(TABLE, I, "ID");
								MAP_SETDEPTH (Library, NUM, MenuAction);
							END
						END
					ELSE
						// Todos los mapas de la librería
						FROM I = 0 TO 999:
							IF (MAP_EXISTS(Library, I))
								MAP_SETDEPTH (Library, I, MenuAction);
							END
						END

					END

					LABEL_TEXT (INFO_LABEL, FILL_FPGTABLE (TABLE, Library, -1, 0));
					WINDOW_CHANGED (Ventana, TRUE);
					SELECTED = -1;
					END

				// Recortar los mapas seleccionados hasta un tamaño óptimo

				CASE "SLICE":

					IF (TABLE_SELECTEDCOUNT(TABLE) > 0)
						// Sólo los seleccionados
						FROM I = 0 TO TABLE_LINES(TABLE)-1:
							IF (TABLE_SELECTED(TABLE, I))
								NUM = TABLE_GET(TABLE, I, "ID");
								MAP_SLICE (Library, NUM);
							END
						END
					ELSE
						// Todos los mapas de la librería
						FROM I = 0 TO 999:
							IF (MAP_EXISTS(Library, I))
								MAP_SLICE (Library, I);
							END
						END
					END

					LABEL_TEXT (INFO_LABEL, FILL_FPGTABLE (TABLE, Library, -1, 0));
					WINDOW_CHANGED (Ventana, TRUE);
					SELECTED = -1;
					END
			END
			MenuAction = "";
		END

		// Se detectó un clic o pulsación de acceso rápido:

		SWITCH (OPTION)
		    CASE 0:

		    	// Modo de vista normal

			TABLE_NORMALMODE(TABLE);
			END

		    CASE 1:

		    	// Modo de vista por iconos

			TABLE_PREVIEWMODE (TABLE, 100, 80, TXT_Nombre, "  ");
			END

		    CASE 3:

		    	// Opción de añadir gráficos a un mapa

			OpenFileDialog (TXT_AbrirDLG, TXT_Filetypes, OF_MULTISELECT);
			FRAME;

			// Pregunta el número de ID si se intenta añadir más de un mapa

			IF (OF_Result)
				MULTIPLE_MAP_ADD(); 
				FRAME;
			END

			WHILE (OF_Result)

				// Tal vez OF_FILE contenga varios ficheros separados por ;
				// en cuyo caso procesa sólo el primero cada vez

				C = FIND(OF_FILE, ";");
				IF (C >= 0)
					Remaining = SUBSTR(OF_FILE, C+1);
					OF_FILE = SUBSTR(OF_FILE, 0, C);
				ELSE
					OF_Result = FALSE;
				END

				// Busca la extensión del fichero elegido

				IF (REGEX("\.(.{2,4})$", OF_File) != -1)

					// Abre una librería o imagen, en función de esta

					Extension = UCASE(REGEX_REG[1]);

					SWITCH (Extension)
						CASE "MAP":
							Map = LOAD_MAP (OF_File);
						    END
						CASE "PNG":
							Map = LOAD_PNG (OF_File);
						    END
						DEFAULT:
							Map = LOAD_IMAGE(OF_File);
							IF (Map != 0)
								LOAD_IMAGE_PAL(OF_File);
							END
						    END
					END
					IF (Map != 0)
						MAP_SET_NAME (0, Map, 
							REGEX_REPLACE("^.*\([^\]*)(\.[A-Za-z])?$", "\1", OF_File));
					END
				ELSE
					// Extensión desconocida. Trata de abrirlo como imagen.

					Map = LOAD_IMAGE(OF_File);
				END

				IF (Map != 0)
					ADD_TOFPG (LIBRARY, "FPG:0,"+MAP, -1);
				END

				IF (C >= 0)
					OF_File = Remaining;
				END
			END

			LABEL_TEXT (INFO_LABEL, FILL_FPGTABLE (TABLE, Library, I, 0));
			WINDOW_CHANGED (Ventana, TRUE);
			SELECTED = -1;
			END

		    CASE 4:

			// Opción de duplicar mapa

		    	IF (SELECTED >= 0)
				FROM I = 1 TO 1000:
					IF (!MAP_EXISTS(LIBRARY, I)) BREAK; END
				END
				IF (I > 999)
					MessageBox (TXT_FPGTooBig, TXT_FPGTooBig1, MB_ALERT);
				ELSE
					NUM = TABLE_GET(TABLE, SELECTED, "ID");
					FPG_ADD (LIBRARY, I, LIBRARY, NUM);
					DIALOG_MAP (Library, I, TXT_CopyOf+MAP_NAME(Library,I));
					FRAME;
					IF (ResultMap != -1)
						LABEL_TEXT (INFO_LABEL, FILL_FPGTABLE (TABLE, Library, I, 0));
						WINDOW_CHANGED (Ventana, TRUE);
						SELECTED = -1;
					ELSE
						CLOSE_MAP (Library, I);
						UNLOAD_MAP (Library, I);
					END
				END
			END
			END

		    CASE 5:

		    	// Opción de borrar mapa

		    	IF (TABLE_SELECTEDCOUNT(TABLE) > 0)

				// Múltiples mapas seleccionados

				MessageBox (TXT_Deleting, TXT_Deleting3 + CHR(10)+
					TABLE_SELECTEDCOUNT(TABLE) + TXT_Deleting4,
					MB_ALERT | MB_YESNO);
				IF (MB_RESULT)
					FROM I = 0 TO TABLE_LINES(TABLE)-1:
						IF (TABLE_SELECTED(TABLE, I))
							NUM = TABLE_GET(TABLE, I, "ID");
							CLOSE_MAP (Library, NUM);
							UNLOAD_MAP (LIBRARY, NUM);
						END
					END
					LABEL_TEXT (INFO_LABEL, FILL_FPGTABLE (TABLE, Library, -1, 0));
					WINDOW_CHANGED (Ventana, TRUE);
					SELECTED = -1;
				END

		    	ELSEIF (SELECTED >= 0)

				// Sólo un mapa seleccionado

				NUM = TABLE_GET(TABLE, SELECTED, "ID");
				MessageBox (TXT_Deleting, TXT_Deleting1 + CHR(10)+
					MAP_NAME(LIBRARY,NUM) + TXT_Deleting2,
					MB_ALERT | MB_YESNO);
				IF (MB_RESULT)
					CLOSE_MAP (Library, NUM);
					UNLOAD_MAP (LIBRARY, NUM);
					LABEL_TEXT (INFO_LABEL, FILL_FPGTABLE (TABLE, Library, -1, 0));
					WINDOW_CHANGED (Ventana, TRUE);
					SELECTED = -1;
				END
			END
			END

		    CASE 7:

		    	// Opción de ver mapa (abre la ventana del MAP)

		    	IF (TABLE_SELECTEDCOUNT(TABLE) > 0)

				// Múltiples mapas seleccionados

				FROM I = 0 TO TABLE_LINES(TABLE)-1:
					IF (TABLE_SELECTED(TABLE, I))
						NUM = TABLE_GET(TABLE, I, "ID");
						OPEN_MAP (Library, NUM, ":EMBEDDED");
					END
				END

		    	ELSEIF (SELECTED >= 0)

				// Sólo un mapa seleccionado

				NUM = TABLE_GET(TABLE, SELECTED, "ID");
				OPEN_MAP (Library, NUM, ":EMBEDDED");
			END
			END

		    CASE 8:

		    	// Opción de propiedades de mapa

		    	IF (SELECTED >= 0)
				NUM = TABLE_GET(TABLE, SELECTED, "ID");
				DIALOG_MAP (Library, NUM, TXT_Properties+MAP_NAME(Library,NUM));
				FRAME;
				IF (ResultMap != -1)
					LABEL_TEXT (INFO_LABEL, FILL_FPGTABLE (TABLE, Library, ResultMap, 0));
					WINDOW_CHANGED (Ventana, TRUE);
					SELECTED = -1;
				END
			END
			END
		END
		FRAME;
	END

	// La ventana ha sido cerrada. Destruye el FPG en memoria,
	// así como cualquier ventana MAP abierta con gráficos de él.

	DESKTOP_REMOVEWINDOW(Ventana);
	WINDOW_DESTROY(Ventana);
	Ventana = DESKTOP_BOTTOMWINDOW();
	WHILE (Ventana != 0)
		IF (WINDOW_TYPE(Ventana) == "MAP" && REGEX("^"+Library+",", WINDOW_DATA(Ventana)) != -1)
			DESKTOP_REMOVEWINDOW(Ventana);
		END
		Ventana = DESKTOP_NEXTWINDOW(Ventana);
	END
	UNLOAD_FPG (Library);
END
