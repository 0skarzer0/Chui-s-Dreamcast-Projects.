
CONST

#if (LANG == EN)

	// Textos para cuadros de diálogo de abrir/guardar
	TXT_Directory   = "&Location:";
	TXT_AbrirDLG	= "Open File";
	TXT_GuardarDLG	= "Save as";
	TXT_Filetypes	= "Images:\.(fpg|png|map|gif|jpg|jpeg|bmp|tga)$;*.FPG;*.MAP;*.*";
	TXT_NomFichero	= "File Name";
	TXT_Bits  	= "Bits";
	TXT_Tamaño	= "Size";
	TXT_Creación	= "Creation Date";
	TXT_Directorio  = "Directory";
	TXT_Ver         = "Show";
	TXT_Salir0	= "Exit";
	TXT_Salir1	= "Warning! The following windows have been modified:";
	TXT_Salir2	= "All changes will be lost. Are you sure?";
	TXT_Salir3	= "Warning! Changes to ";
	TXT_Salir4	= " will be lost. Are you sure?";

	// Textos de uso general
	TXT_Aceptar     = "&OK";
	TXT_Cancelar    = "&Cancel";
	TXT_Untitled	= "(untitled)";
	TXT_Yes         = "&Yes";
	TXT_No          = "&No";

#endif

#if (LANG == ES)

	// Textos para cuadros de diálogo de abrir/guardar
	TXT_Directory   = "&Directorio:";
	TXT_AbrirDLG	= "Abrir";
	TXT_GuardarDLG	= "Guardar como";
	TXT_Filetypes	= "Imágenes:\.(fpg|png|map|gif|jpg|jpeg|bmp|tga)$;*.FPG;*.MAP;*.*";
	TXT_NomFichero	= "Nombre";
	TXT_Bits  	= "Bits";
	TXT_Tamaño	= "Tamaño";
	TXT_Creación	= "Creación";
	TXT_Directorio  = "Directorio";
	TXT_Ver         = "Ver";
	TXT_Salir0	= "Salir";
	TXT_Salir1	= "¡Atención! Los siguientes ficheros han sido modificados:";
	TXT_Salir2	= "Estos cambios se perderán. ¿Está seguro?";
	TXT_Salir3	= "¡Atención! Las modificaciones a ";
	TXT_Salir4	= " se perderán. ¿Está seguro?";

	// Textos de uso general
	TXT_Aceptar     = "&Aceptar";
	TXT_Cancelar    = "&Cancelar";
	TXT_Untitled	= "(sin título)";
	TXT_Yes         = "&Sí";
	TXT_No          = "&No";

#endif

END

CONST
	STYLE_NORMAL		= 0;
	STYLE_FIXED		= 1;
	STYLE_DIALOG		= 2;
	STYLE_RESIZABLE		= 3;
	STYLE_STATIC		= 4;
	STYLE_DOCK_NORTH	= 5;
	STYLE_DOCK_SOUTH	= 6;
	STYLE_DOCK_EAST		= 7;
	STYLE_DOCK_WEST		= 8;
	FRAME_RAISE		= 0;
	FRAME_LOWER		= 1;
	FRAME_LINE		= 2;
	FRAME_INSET		= 3;
	SCROLLBAR_BUTTONS 	= 1;
	SCROLLBAR_FRAME		= 2;
	SCROLLBAR_FOCUSABLE	= 4;
	SCROLLBAR_FIXED		= 8;
	MENUITEM_SEPARATOR	= 0;
	MENUITEM_ACTION		= 1;
	MENUITEM_TOGGLE		= 2;
	MENUITEM_RADIO		= 3;
	MENUITEM_SUBMENU	= 4;
	COLUMN_STRINGS		= 1;
	IMAGEVIEW_TRANSPARENT	= 0;
	IMAGEVIEW_BLACK		= 1;
	IMAGEVIEW_CHESSBOARD	= 2;
	IMAGEVIEW_GRAY		= 3;
	IMAGEVIEW_WINDOW	= 4;
	WINDOW_ALPHA		= 1;
	WINDOW_AUTODESTROY	= 2;
	WINDOW_STYLE		= 3;
	WINDOW_MOVEABLE		= 4;
	WINDOW_ONTOP		= 5;
	WINDOW_MODAL		= 6;
	WINDOW_WIDTH		= 7;
	WINDOW_HEIGHT		= 8;
	WINDOW_CAPTION		= 9;
	WINDOW_DONTFOCUS	= 10;
	WINDOW_FULLSCREEN	= 11;
	WINDOW_MAXIMIZED 	= 12;
	CONTROL_WIDTH		= 1;
	CONTROL_HEIGHT		= 2;
	CONTROL_X		= 3;
	CONTROL_Y		= 4;
	CONTROL_HIDDEN		= 5;
	CONTROL_MIN_WIDTH	= 6;
	CONTROL_MIN_HEIGHT	= 7;
	CONTROL_MAX_WIDTH	= 8;
	CONTROL_MAX_HEIGHT	= 9;
	CONTROL_HRESIZABLE	= 10;
	CONTROL_VRESIZABLE	= 11;
	TABLE_CAPTION		= 1;
	TABLE_RESIZABLE_COLUMNS = 2;
	TABLE_MOVEABLE_COLUMNS  = 3;
	TABLE_SORTABLE_COLUMNS  = 4;
	TABLE_MULTISELECT       = 5;
	POINTER_ARROW		= 0;
	POINTER_RESIZEH		= 1;
	POINTER_RESIZEV		= 2;
	POINTER_RESIZENW	= 3;
	POINTER_RESIZENE	= 4;
	POINTER_CROSS		= 5;
	POINTER_IBEAM		= 6;
	POINTER_RESIZEH2	= 7;
	POINTER_HAND		= 8;
	POINTER_ARROWCOPY	= 10;
	POINTER_DRAWLINE	= 16;
	POINTER_DRAWRECT	= 17;
	POINTER_DRAWCIRCLE	= 18;
	POINTER_DRAWSELECT	= 19;
	POINTER_DRAWFILL	= 20;
	POINTER_PICK		= 21;
	DRAG_INACTIVE		= 0;
	DRAG_ACTIVE		= 1;
	DRAG_DROPPED		= 2;
	DRAG_ATDESKTOP		= -1;
	TOOL_PENCIL		= 0;
	TOOL_LINE  		= 1;
	TOOL_RECT  		= 2;
	TOOL_CIRCLE		= 3;
	TOOL_CONTROLPOINT	= 4;
	CCM_ENABLETRANSPARENT	= 01h;
	CCM_ENABLENONE		= 02h;
	CCM_ENABLEPALETTE	= 04h;
	CCM_ENABLEOK		= 08h;
END

TYPE Column
	INT		ItemType;
	CHAR		Name[63];
	INT	POINTER LineCount;
	STRING	POINTER Data;
	INT		LineHeight;
	INT		Width;
END;

TYPE MenuItem
	INT	ItemType;
	CHAR	Text[63];
	INT	Data;
	INT	Value;
	INT	Width;
	INT	Height;
	INT	Enabled;
	INT	Internal;	// Must be 0	
END;

TYPE ScrollBarInfo
	INT	Min;
	INT	Max;
	INT	SingleStep;
	INT	PageStep;
	INT	Pos;
END;

TYPE EditorInfo
	INT	Tool;
	INT	Zoom = 100;
	INT	Color = 0FFFFh;
	INT	Color8 = 15;
	INT	Background = -1;
	INT	Background8 = -1;
	INT	CPoint;
	INT	Alpha = 255;
	INT	File;
	INT	Graph;
	INT	BrushFile;
	INT	BrushGraph;
	INT	TextureFile;
	INT	TextureGraph;
	INT	CursorX;
	INT	CursorY;
	INT	GridVisible;
	INT	GridStep = 8;
	INT	GridColor = 0FFFFh;
	INT	PixelGridColor;
	INT	PaperMode;
	INT	PaperColor = 0000h;
END;

/* ---------------------------------------------------------------------- */
/* Message Box                                                            */
/* ---------------------------------------------------------------------- */

CONST
	MB_INFO		= 1;
	MB_ALERT	= 2;
	MB_DANGER	= 4;
	MB_OK		= 0;
	MB_YESNO	= 8;
END

GLOBAL
	MB_win;
	MB_result;
	GUI_FILE;
END

PROCESS MessageBox_Close() BEGIN
	DESKTOP_REMOVEWINDOW(mb_win);
END
PROCESS MessageBox_Yes() BEGIN
	mb_result = 1;
	DESKTOP_REMOVEWINDOW(mb_win);
END

PROCESS MessageBox (STRING Title, STRING Message, INT Flags)
PRIVATE
	image;
	ic;
	lc;
	buttons_left;
BEGIN
	mb_win = WINDOW (Title, STYLE_DIALOG);
	WINDOW_SET (mb_win, WINDOW_MODAL, 1);

	/* Crea el icono decorativo */
	IF ((Flags & MB_INFO) != 0)
		image = 900;
	END
	IF ((Flags & MB_DANGER) != 0)
		image = 901;
	END
	IF ((Flags & MB_ALERT) != 0)
		image = 902;
	END
	WINDOW_ADDCONTROL (mb_win, 0, 0, 
		ic = IMAGE_VIEW(GUI_FILE, image, 100, IMAGEVIEW_WINDOW));
	WINDOW_RESIZE     (mb_win, 80, 80);

	/* Crea la etiqueta */
	WINDOW_ADDCONTROL (mb_win, 90, 0, lc = LABEL(Message));

	/* Calcula la posición de los botones */
	y = 10 + CONTROL_INFO (lc, CONTROL_HEIGHT);
	IF y < 45: 
		y = 45; 
		CONTROL_RESIZE (lc, CONTROL_INFO(lc, CONTROL_WIDTH), 45);
	END

	/* Crea los botones */
	buttons_left = 90 + CONTROL_INFO(lc, CONTROL_WIDTH)/2;
	IF ((Flags & MB_YESNO) != 0)
		buttons_left -= 100;
		WINDOW_ADDCONTROL (mb_win, buttons_left, y, 
			BUTTON(TXT_Yes, TYPE MessageBox_Yes));
		WINDOW_ADDCONTROL (mb_win, buttons_left+85, y, 
			BUTTON(TXT_No, TYPE MessageBox_Close));
	ELSE:
		buttons_left -= 60;
		WINDOW_ADDCONTROL (mb_win, buttons_left, y, 
			BUTTON(TXT_Aceptar, TYPE MessageBox_Close));
	END

	/* Ajusta el tamaño de la imagen, si es preciso */
	IF (y > 45)
		CONTROL_RESIZE (ic, 90, y + 20);
	END

	/* Muestra la ventana */
	DESKTOP_ADDWINDOW (mb_win);

	/* Duerme al proceso padre hasta que la ventana vuelva */
	SIGNAL (father, S_SLEEP);
	mb_result = 0;
	WHILE WINDOW_VISIBLE(mb_win):
		IF (SCAN_CODE == _ESC || SCAN_CODE == _N):
			SCAN_CODE = 0;
			BREAK; 
		END
		IF (SCAN_CODE == _Y || SCAN_CODE == _S):
			SCAN_CODE = 0;
			mb_result = 1;
			BREAK;
		END
		FRAME;
	END

	/* Recupera al padre y descarga la imagen */
	WINDOW_DESTROY(mb_win);
	SIGNAL (father, S_WAKEUP);
END

/* ---------------------------------------------------------------------- */
/* Open File Dialog                                                       */
/* ---------------------------------------------------------------------- */

CONST
	OF_ASKOVERWRITE			= 1;
	OF_USEDEFAULT			= 2;
	OF_MULTISELECT			= 4;
	OF_MUSTEXIST			= 8;
END

GLOBAL
	of_win;
	of_result;
	STRING of_path;
	CHAR of_file[65535];
END

PROCESS OpenFileDialog_OK()
BEGIN
	of_result = TRUE;
	DESKTOP_REMOVEWINDOW(DESKTOP_TOPWINDOW());
END

PROCESS OpenFileDialog_CANCEL()
BEGIN
	of_result = FALSE;
	DESKTOP_REMOVEWINDOW(DESKTOP_TOPWINDOW());
END

STRING GetFileList (INT Table)
PRIVATE
	STRING Result;
	INT I;
BEGIN
	FROM I = 0 TO TABLE_LINES(Table)-1:
		IF (TABLE_SELECTED(Table, I))
			IF (Result == "")
				Result = TABLE_GET(Table, I, TXT_NomFichero);
			ELSE
				Result += ";"+TABLE_GET(Table, I, TXT_NomFichero);
			END
		END
	END
	RETURN Result;
END

PROCESS OpenFileDialog (STRING Title, STRING Masks, INT Options)
PRIVATE
	INT i, pos, num;
	INT c_directory;
	INT c_filetype;
	INT c_path;
	INT c_files;
	INT c_input;
	INT win;
	INT selectedcount;
	STRING CurrentDrive = "C:";
	STRING CurrentMask;
	CHAR   CurrentDirectory[2048];
	STRING GlobMask;
	STRING RegMask;
	STRING s;
	INT currentfile;
	INT currentpath;
	INT refill = TRUE;
	STRING lastdrive;
	STRING lastmask;
	STRING patterns[31];
BEGIN
	win = WINDOW (Title, STYLE_NORMAL);
	WINDOW_SET (win, WINDOW_MODAL, TRUE);

	/* Create the current-path input */
	WINDOW_ADDCONTROL (win, 0, 0, c_directory = INPUTLINE(28, CurrentDirectory));
	CurrentDirectory = CD();

	/* Create and fill the file type drop-down control */
	WINDOW_ADDCONTROL (win, 250, 0, LABEL(TXT_Ver+":"));
	WINDOW_ADDCONTROL (win, 300, 0, c_filetype = DROPDOWN(100, &CurrentMask));
	num = SPLIT (";", Masks, &patterns, 32);
	FROM i = 0 TO num-1:
		IF (REGEX("^(.*):(.*)$", patterns[i]) != -1)
			DROPDOWN_ADDOPTION (c_filetype, REGEX_REG[1]);
			IF (i == 0) CurrentMask = REGEX_REG[1]; END
		ELSE
			DROPDOWN_ADDOPTION (c_filetype, patterns[i]);
			IF (i == 0) CurrentMask = patterns[0]; END
		END
	END
	CONTROL_LOCK (c_filetype, TRUE, FALSE);

	/* Create and fill the path table */
	WINDOW_ADDCONTROL (win, 0, 20, c_path = TABLE(120, 200));
	TABLE_ADDCOLUMN_BITMAPS (c_path, " ");
	TABLE_LIBRARY   (c_path, "  ", GUI_FILE);
	TABLE_ADDCOLUMN (c_path, TXT_Directorio);
	CONTROL_LOCK (c_path, TRUE, FALSE);
	TABLE_SET (c_path, TABLE_CAPTION, 0);
	TABLE_SET (c_path, TABLE_RESIZABLE_COLUMNS, 0);
	TABLE_SET (c_path, TABLE_SORTABLE_COLUMNS, 0);
	TABLE_SET (c_path, TABLE_MOVEABLE_COLUMNS, 0);
	TABLE_AUTOEXPAND (c_path, 1);

	/* Create the file table */
	WINDOW_ADDCONTROL (win, 125, 20, c_files = TABLE(275, 200));
	TABLE_ADDCOLUMN_BITMAPS (c_files, " ");
	TABLE_ADDCOLUMN (c_files, TXT_NomFichero);
	TABLE_ADDCOLUMN (c_files, TXT_Tamaño);
	TABLE_ADDCOLUMN (c_files, TXT_Creación);
	TABLE_LIBRARY   (c_files, "  ", GUI_FILE);
	TABLE_ALIGNMENT (c_files, TXT_Tamaño, 2);
	TABLE_SELECT (c_path, -1);
	TABLE_AUTOEXPAND (c_files, TXT_NomFichero);
	IF ((OPTIONS & OF_MULTISELECT) != 0)
		TABLE_SET (c_files, TABLE_MULTISELECT, 1);
	END

	/* Create the input line */
	WINDOW_ADDCONTROL (win, 5, 230, LABEL (Title+":"));
	WINDOW_ADDCONTROL (win, 75, 228, c_input = INPUTLINE (18, of_file));
	WINDOW_ADDCONTROL (win, 235, 225, BUTTON (TXT_Aceptar, TYPE OpenFileDialog_OK));
	WINDOW_ADDCONTROL (win, 320, 225, BUTTON (TXT_Cancelar, TYPE OpenFileDialog_CANCEL));
	WINDOW_RESIZE (win, 480, 400);

	/* Main loop */
	DESKTOP_ADDWINDOW (win);
	IF ((OPTIONS & OF_USEDEFAULT) == 0)
		CONTROL_FOCUS (c_files);
	ELSE
		CONTROL_FOCUS (c_input);
	END
	SIGNAL (father, S_SLEEP);
	currentfile = TABLE_SELECTED(c_files);
	currentpath = TABLE_SELECTED(c_path);
	lastdrive = currentdrive;
	lastmask = currentmask;

	of_result = FALSE;
	WHILE ( WINDOW_VISIBLE(win) )
		IF (CONTROL_LEAVED(c_directory) && CurrentDirectory != CD())
			CHDIR(CurrentDirectory);
			CurrentDirectory = CD();
			refill = TRUE;
		END
		IF (SCAN_CODE == _ESC):
			SCAN_CODE = 0;
			BREAK; 
		END
		IF (SCAN_CODE == _ENTER)
			IF (CurrentDirectory != CD())
				CHDIR(CurrentDirectory);
				CurrentDirectory = CD();
				refill = TRUE;
				SCAN_CODE = 0;
			ELSEIF (OF_FILE != "")
				SCAN_CODE = 0;
				of_result = 1;
				BREAK;
			END
		END
		IF (CONTROL_DOUBLECLICK(c_files))
			of_result = TRUE;
			BREAK;
		END
		/* Check for file selection */
		IF ((OPTIONS & OF_MULTISELECT) == 0)
			IF (!MOUSE.LEFT && TABLE_SELECTED(c_files) != currentfile)
				currentfile = TABLE_SELECTED(c_files);
				of_file = TABLE_GET(c_files, currentfile, TXT_NomFichero);
			END
		ELSEIF (CONTROL_CHANGED(c_files))
			selectedcount = TABLE_SELECTEDCOUNT(c_files);
			of_file = GetFileList(c_files);
		END
		/* Check for directory selection */
		IF (!MOUSE.LEFT && TABLE_SELECTED(c_path) != currentpath)
			currentpath = TABLE_SELECTED(c_path);
			CHDIR (TABLE_GET(c_path, currentpath, 1));
			CurrentDirectory = CD();
			refill = TRUE;
		END
		/* Check for mask selection */
		IF (lastmask != currentmask)
			lastmask = currentmask;
			refill = TRUE;
		END
		/* Update the tables */
		IF (refill)
			refill = FALSE;

			/* Fill the directory table */	
			TABLE_REMOVELINES (c_path, 0, 0);
			GlobMask = CD() + "\*.*";
			WHILE ( GLOB(GlobMask) != "" )
				IF ( fileinfo.directory && fileinfo.name != "." )
					TABLE_ADDLINE (c_path, fileinfo.name == ".." ? 102:100, fileinfo.name);
				END
			END
			TABLE_SORT (c_path, 1, 1);

			/* Fill the file table */
			TABLE_REMOVELINES (c_files, 0, 0);
			GlobMask = CD() + "\" + CurrentMask;
			RegMask = "";
			FROM i = 0 TO Num:
				IF (FIND(patterns[i], CurrentMask+":") == 0)
					GlobMask = CD() + "\*.*";
					RegMask = SUBSTR(patterns[i], FIND(patterns[i],":")+1);
				END
			END
			WHILE ( GLOB(GlobMask) != "" )
				IF ( !fileinfo.directory )
					IF ( RegMask != "" && REGEX(RegMask, fileinfo.name) == -1 )
						CONTINUE;
					END
					s = format(fileinfo.size/1024.0, 0)+" KB";
					IF ( fileinfo.size < 1024 )
					    s = "1 KB";
					END
					TABLE_ADDLINE (c_files, 
						REGEX("\.(map|fpg)$", fileinfo.name) == -1 ? 101:103,
						fileinfo.name, s,
						fileinfo.created);
				END
			END
			TABLE_SORT (c_files, TXT_NomFichero, 1);
			IF ((OPTIONS & OF_USEDEFAULT) == 0)
				TABLE_SELECT (c_files, 0);
			ELSE
				TABLE_SELECT (c_files, -1);
				OPTIONS &= !OF_USEDEFAULT;
			END
			TABLE_SELECT (c_path, -1);
			TABLE_AUTOEXPAND (c_files, TXT_NomFichero);
			currentfile = -1;
			currentpath = -1;
		END
		CONTROL_CHANGED(c_files);
		FRAME;
	END

	IF (OF_RESULT)
		IF ((OPTIONS & OF_ASKOVERWRITE) != 0)
	       		IF (FILE_EXISTS(OF_FILE))
				MessageBox ("Guardar fichero",
					"¡Atención! Va a sobreescribir '"+OF_FILE+"'"+CHR(10)+
					"¿Está seguro?", MB_YESNO | MB_ALERT);
				OF_RESULT = MB_RESULT;
			END
		END
	END

	DESKTOP_REMOVEWINDOW (win);
	WINDOW_DESTROY (win);
	SIGNAL (father, S_WAKEUP);

	of_path = FILEINFO.PATH;
END

