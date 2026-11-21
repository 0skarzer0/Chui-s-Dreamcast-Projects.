/*
 *	FPG.PRG
 *	-----------------------------------------------------------------------
 *	Utilidad para crear y convertir ficheros FPG y MAP. Utiliza las
 *	DLL oficiales GUI.DLL y IMAGE.DLL.
 *
 */

#define LANG EN

IMPORT  "gui";
IMPORT  "image";
INCLUDE "gui.inc";

CONST

#if (LANG == ES)
	// Textos del menú
	TXT_NuevoMAP	= "&Nuevo gráfico...";
	TXT_NuevoFPG	= "&Nueva librería...";
	TXT_Fichero	= "&Fichero";
	TXT_Abrir	= "&Abrir...";
	TXT_Guardar	= "&Guardar";
	TXT_GuardarComo	= "Guardar &como...";
	TXT_Salir	= "&Salir";
	TXT_Editar	= "&Editar";
	TXT_Copiar	= "&Copiar";
	TXT_Cortar	= "Cor&tar";
	TXT_Slice	= "Optimizar tamaños";
	TXT_Convert8	= "Convertir a 8 bits";
	TXT_Convert16	= "Convertir a 16 bits";
	TXT_Pegar	= "&Pegar";
	TXT_PegarDentro	= "&Pegar dentro";
	TXT_ERROP1	= "Error al abrir ";
	TXT_ERROP2	= "Imposible abrir el fichero ";
	TXT_ERROP3	= "El formato es desconocido o el contenido está corrupto.";
	TXT_CopyOf	= "Copia de ";
	TXT_Properties	= "Propiedades de ";
	TXT_Propiedades	= "&Properties...";
	TXT_Ver		= "&Ver";
	TXT_VerEdPal	= "&Herramientas";
	TXT_VerEdPre	= "&Vista previa";
	TXT_VerPal	= "&Paleta";
	TXT_VerInfo	= "&Información";
	TXT_Ayuda	= "A&yuda";
	TXT_AcercaDe	= "&Acerca de...";
	TXT_Paleta	= "Paleta de Colores";
	TXT_Preview	= "&Vista Previa";
	TXT_PEditor	= "Editor de &Paleta...";
	TXT_POptions	= "Opciones del editor...";

	// Textos para ventana de información
	TXT_InfoTit	= "Información";
	TXT_InfoMem1	= "Memoria disponible:";
	TXT_InfoMem2	= "Memoria usada:";
	TXT_InfoMem3	= "Memoria de deshacer:";
	TXT_InfoMem4	= "Número de gráficos:";
	TXT_InfoMem5	= "Número de librerías:";

	// Textos para cuadro de diálogo de FPG/MAP
	TXT_MAPDup	= "Código duplicado";
	TXT_MAPDup1	= "Ya existe un gráfico de código ";
	TXT_MAPDup2	= " con nombre ";
	TXT_MAPDup3	= "¿Desea reemplazarlo?";
	TXT_NombreDLG	= "&Nombre:";
	TXT_CodeDLG	= "&Código:";
	TXT_Codenone	= "(No es parte de ninguna librería)";
	TXT_SizeDLG	= "&Tamaño:";
	TXT_ColorsDLG	= "&Profundidad:";
	TXT_Nombre	= "Nombre";
	TXT_FPGTooBig	= "FPG lleno";
	TXT_FPGTooBig1	= "Hay un máximo de 999 gráficos por librería";
	TXT_AsList	= "Ver como lista";
	TXT_AsIcons	= "Ver como iconos";
	TXT_AddMAP	= "Nuevo gráfico (INS)";
	TXT_DupMAP	= "Duplicar gráfico (CTRL+D)";
	TXT_DelMAP	= "Borrar gráfico (DEL)";
	TXT_ShowMAP	= "Ver gráfico (ENTER)";
	TXT_EditMAP	= "Propiedades (F2)";
	TXT_Deleting	= "Borrar gráfico";
	TXT_Deleting1	= "Esta operación no se puede deshacer. El gráfico ";
	TXT_Deleting2	= " será destruído. ¿Está seguro?";
	TXT_Deleting3	= "Esta operación no se puede deshacer.";
	TXT_Deleting4	= " gráficos serán destruídos. ¿Está seguro?";
	TXT_Zoom	= "&Zoom";
	TXT_ZoomMax	= "&Automático";
	TXT_MultiFPGDD  = "Múltiples gráficos";
	TXT_MultiFPGDD1 = "Está extrayendo múltiples gráficos de una librería.";
	TXT_MultiFPGDD2 = "Escoja una opción:";
	TXT_MultiFPGDDA = "Copiar los gráficos a una librería nueva";
	TXT_MultiFPGDDB = "Mover los gráficos a una librería nueva";
	TXT_MultiFPGDDC = "Mostrar una ventana por cada gráfico";
	TXT_MultiOpen   = "Abriendo múltiples gráficos";
	TXT_MultiOpen1  = "Va a abrir múltiples gráficos simultáneamente.";
	TXT_MultiOpen2  = "Escoja una opción:";
	TXT_MultiOpenA  = "Crear una librería con los gráficos";
	TXT_MultiOpenB  = "Mostrar una ventana por cada gráfico";
	TXT_MultiAdd    = "Añadiendo gráficos";
	TXT_MultiAdd1   = "Siguiente código:";
	TXT_MultiAdd2   = "A cada gráfico añadido se le asignará el siguiente";
	TXT_MultiAdd3   = "código disponible, empezando por el indicado.";
	TXT_AskAgain    = "Preguntar de nuevo";
	TXT_CPoints	= "&Puntos C.:";
	TXT_Points	= "Puntos";
	TXT_Center	= "Centro";
	TXT_NewMap	= "Creando un nuevo gráfico";

	// Textos para cuadro de diálogo de opciones
	TXT_ODLGTitle1	= "Opciones por defecto";
	TXT_ODLGTitle2	= "Opciones de la ventana";
	TXT_ODLGShowG   = "Mostrar rejilla";
	TXT_ODLGGridSp  = "Espaciado de rejilla:";
	TXT_ODLGShowPG  = "Mostrar rejilla de pixels";
	TXT_ODLGPaper   = "Mostrar papel sólido";
	TXT_ODLGColor	= "Color de rejilla:";
	TXT_ODLGPColor	= "Color de papel:";

	// Textos para cuadro de diálogo "acerca de..."
	TXT_AboutTit	= "Acerca de Fenix";
	TXT_Version	= "Fenix 0.84 Alpha";
	TXT_Copyright1	= "(c) 1999-2003 José Luis Cebrián";
	TXT_Copyright2	= "(c) 2002-2003 Fenix Team";
	TXT_License     = "Licencia";
	TXT_GNU		= "This program is free software; you can redistribute it and/or modify it^"
	 		  "under the terms of the GNU General Public License as published by^"
			  "the Free Software Foundation; either version 2 of the license or^"
			  "(at your option) any later version.^^"
 			  "This program is distributed in the hope that it will be useful,^"
 			  "but WITHOUT ANY WARRANTY; without even the implied warranty of^"
 			  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the^"
 			  "GNU General Public License for more details.^^"
 			  "You should have received a copy of the GNU General Public License^"
 			  "along with this program; if not, write to the Free Software ^"
 			  "Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA";
#endif

#if (LANG == EN)
	// Textos del menú
	TXT_NuevoMAP	= "&New graphic...";
	TXT_NuevoFPG	= "&New library...";
	TXT_Fichero	= "&File";
	TXT_Abrir	= "&Open...";
	TXT_Guardar	= "&Save";
	TXT_GuardarComo	= "Save &as...";
	TXT_Salir	= "E&xit";
	TXT_Editar	= "&Edit";
	TXT_Copiar	= "&Copy";
	TXT_Cortar	= "Cu&t";
	TXT_Slice	= "Optimize sizes";
	TXT_Convert8	= "Convert to 8 bits";
	TXT_Convert16	= "Convert to 16 bits";
	TXT_Pegar	= "&Paste";
	TXT_PegarDentro	= "&Paste as selection";
	TXT_ERROP1	= "Error opening ";
	TXT_ERROP2	= "Could not open the file ";
	TXT_ERROP3	= "The file format is unknown or the file is corrupt.";
	TXT_CopyOf	= "Copy of ";
	TXT_Properties	= "Properties of ";
	TXT_Propiedades	= "&Properties...";
	TXT_Ver		= "&Show";
	TXT_VerEdPal	= "&Toolbox";
	TXT_VerEdPre	= "&Preview";
	TXT_VerPal	= "&Palette";
	TXT_VerInfo	= "&Information";
	TXT_Ayuda	= "&Help";
	TXT_AcercaDe	= "&About...";
	TXT_Paleta	= "Color Palette";
	TXT_Preview	= "&Preview";
	TXT_PEditor	= "Palette &Editor...";
	TXT_POptions	= "Editor Options...";

	// Textos para ventana de información
	TXT_InfoTit	= "Information";
	TXT_InfoMem1	= "Available Memory:";
	TXT_InfoMem2	= "Used Memory:";
	TXT_InfoMem3	= "UNDO Memory:";
	TXT_InfoMem4	= "Map count:";
	TXT_InfoMem5	= "Library count:";

	// Textos para cuadro de diálogo de FPG/MAP
	TXT_MAPDup	= "Duplicated code";
	TXT_MAPDup1	= "There is a graphic with code ";
	TXT_MAPDup2	= "already, with name ";
	TXT_MAPDup3	= "Do you want to replace the existing one?";
	TXT_NombreDLG	= "&Name:";
	TXT_CodeDLG	= "&Code:";
	TXT_Codenone	= "(Not part of a library)";
	TXT_SizeDLG	= "&Size:";
	TXT_ColorsDLG	= "&Depth:";
	TXT_Nombre	= "Name";
	TXT_FPGTooBig	= "FPG is full";
	TXT_FPGTooBig1	= "There is a maximum of 999 graphics allowed per FPG file";
	TXT_AsList	= "Show as list";
	TXT_AsIcons	= "Show as icons";
	TXT_AddMAP	= "Add new map (INS)";
	TXT_DupMAP	= "Duplicate map (CTRL+D)";
	TXT_DelMAP	= "Remove map (DEL)";
	TXT_ShowMAP	= "Show map (ENTER)";
	TXT_EditMAP	= "Map properties (F2)";
	TXT_Deleting	= "Remove map";
	TXT_Deleting1	= "This operation cannot be undone. The graphic";
	TXT_Deleting2	= " will be destroyed. Are you sure?";
	TXT_Deleting3	= "This operation cannot be undone.";
	TXT_Deleting4	= " graphics will be destroyed. Are you sure?";
	TXT_Zoom	= "&Zoom";
	TXT_ZoomMax	= "&Automatic";
	TXT_MultiFPGDD  = "Multiple drop operation";
	TXT_MultiFPGDD1 = "You are dragging multiple graphics from a library.";
	TXT_MultiFPGDD2 = "Please choose one option:";
	TXT_MultiFPGDDA = "Create a new library with all the graphics";
	TXT_MultiFPGDDB = "Create a new library, move the graphics there";
	TXT_MultiFPGDDC = "Show a window for each graphic";
	TXT_MultiOpen   = "Opening multiple map files";
	TXT_MultiOpen1  = "You are trying to open multiple graphic files simultaneously.";
	TXT_MultiOpen2  = "Please choose one option:";
	TXT_MultiOpenA  = "Create a new library with all the graphics";
	TXT_MultiOpenB  = "Show a window for each graphic";
	TXT_MultiAdd    = "Adding maps";
	TXT_MultiAdd1   = "Next code to assign:";
	TXT_MultiAdd2   = "Every map added to your library will be assigned";
	TXT_MultiAdd3   = "a new free code, starting with the one given here.";
	TXT_AskAgain    = "Ask again next time";
	TXT_CPoints	= "C. &Points:";
	TXT_Points	= "Points";
	TXT_Center	= "Center";
	TXT_NewMap	= "Creating new map";

	// Textos para cuadro de diálogo de opciones
	TXT_ODLGTitle1	= "Default options";
	TXT_ODLGTitle2	= "Graphic window options";
	TXT_ODLGShowG   = "Show grid";
	TXT_ODLGGridSp  = "Grid spacing:";
	TXT_ODLGShowPG  = "Show pixel grid";
	TXT_ODLGPaper   = "Show paper";
	TXT_ODLGColor	= "Grid color:";
	TXT_ODLGPColor	= "Paper color:";

	// Textos para cuadro de diálogo "acerca de..."
	TXT_AboutTit	= "About Fenix";
	TXT_Version	= "Fenix 0.84 Alpha";
	TXT_Copyright1	= "(c) 1999-2003 José Luis Cebrián";
	TXT_Copyright2	= "(c) 2002-2003 Fenix Team";
	TXT_License     = "License";
	TXT_GNU		= "This program is free software; you can redistribute it and/or modify it^"
	 		  "under the terms of the GNU General Public License as published by^"
			  "the Free Software Foundation; either version 2 of the license or^"
			  "(at your option) any later version.^^"
 			  "This program is distributed in the hope that it will be useful,^"
 			  "but WITHOUT ANY WARRANTY; without even the implied warranty of^"
 			  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the^"
 			  "GNU General Public License for more details.^^"
 			  "You should have received a copy of the GNU General Public License^"
 			  "along with this program; if not, write to the Free Software ^"
 			  "Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA";
#endif

END

GLOBAL
	STRING Messages[9];
	STRING MenuAction;
	INT    MessageCount = 0;
	INT    MultiMapAction;
	INT    ShowEdPal = 1;
	INT    ShowPreview;
	INT    ShowPal;
	INT    ShowInfo;
	INT    PalettePanel;
	INT    PreviewImageView;
	INT    BrushFILE;
	INT    TextureFile;
END


INCLUDE "fpg.inc";
INCLUDE "pal.inc";

// -----------------------------------------------------------------------------
//   MESSAGE_BOX
// -----------------------------------------------------------------------------
//   Muestra una caja de mensajes en pantalla, y va borrando los mensajes
//   más antiguos a medida que pasan los segundos

PROCESS MESSAGE_BOX()
BEGIN
	LOOP
		DELETE_TEXT(0);
		FROM X = 0 TO MessageCount-1:
			SET_TEXT_COLOR(RGB(255,255,255));
			WRITE (0, 8, 450-11*X, 0, Messages[X]);
		END
		Y = MessageCount;
		WHILE (MessageCount == Y && TIMER[1] < 250) FRAME; END
		TIMER[1] = 0;
		IF (MessageCount > 0 && MessageCount == Y)
			FROM X = 0 TO MessageCount-2:
				Messages[X] = Messages[X+1];
			END
			MessageCount--;
		END
	END
END

// -----------------------------------------------------------------------------
//   SHOW_MESSAGE
// -----------------------------------------------------------------------------
//   Muestra un mensaje en pantalla... Estos mensajes desaparecen cada
//   poco tiempo.

PROCESS SHOW_MESSAGE (STRING Text)
BEGIN
	WHILE (MessageCount >= 9) FRAME; END
	Messages[MessageCount++] = Text;
END

// -----------------------------------------------------------------------
//   MULTIPLE_MAP_OPEN
// -----------------------------------------------------------------------
//
//   Muestra un cuadro de diálogo preguntando al usuario qué quiere
//   hacer, cuando intenta cargar múltiples gráficos PNG, MAP, etc.
//

PROCESS MULTIPLE_MAP_OPEN ()
PRIVATE
	Ventana;
	Opcion;
	B_OK;
	B_CANCEL;
	I;
	Action;
BEGIN
	// Crea la ventana

	MultiMapAction = 0;
	SIGNAL (FATHER, S_SLEEP);
	Ventana = WINDOW(TXT_MultiOpen, STYLE_DIALOG);
	WINDOW_ADDCONTROL (Ventana, 0,   0, I = IMAGE_VIEW(GUI_FILE, 900));
	WINDOW_ADDCONTROL (Ventana, 90,  5, LABEL(TXT_MultiOpen1));
	WINDOW_ADDCONTROL (Ventana, 90, 17, LABEL(TXT_MultiOpen2));
	WINDOW_ADDCONTROL (Ventana, 90, 40, RADIO(TXT_MultiOpenA, &Opcion, 0));
	WINDOW_ADDCONTROL (Ventana, 90, 55, RADIO(TXT_MultiOpenB, &Opcion, 1));
	WINDOW_ADDCONTROL (Ventana, 90, 80, B_OK = BUTTON(TXT_Aceptar));
	WINDOW_ADDCONTROL (Ventana, 175, 80, B_CANCEL = BUTTON(TXT_Cancelar));
	CONTROL_RESIZE (I, 80, 80);
	WINDOW_SET (Ventana, WINDOW_MODAL, TRUE);
	DESKTOP_ADDWINDOW (Ventana);

	// Bucle de ventana

	WHILE (WINDOW_VISIBLE(Ventana) && !BUTTON_PRESSED(B_CANCEL))
		IF (BUTTON_PRESSED(B_OK) || SCAN_CODE == _ENTER)
			MultiMapAction = OPCION+1;
			SCAN_CODE = 0;
			BREAK;
		END
		FRAME;
	END
	WINDOW_DESTROY(Ventana);
	SIGNAL (FATHER, S_WAKEUP);
END

// -----------------------------------------------------------------------------
//   ACTION_PASTE
// -----------------------------------------------------------------------------
//
//   Pega el mapa actual en el portapapeles en una nueva ventana
//

PROCESS ACTION_PASTE()
BEGIN
	GRAPH = PASTE_GRAPH();
	IF (GRAPH != 0)
		MAP_SET_NAME (0, GRAPH, TXT_Untitled);
		OPEN_MAP (0, GRAPH, TXT_Untitled);
	END
END

// -----------------------------------------------------------------------------
//   ACTION_CUT
// -----------------------------------------------------------------------------
//
//   Copia el mapa actual en el portapapeles y luego la cierra
//

PROCESS ACTION_CUT()
PRIVATE
	Ventana;
BEGIN
	Ventana = DESKTOP_TOPWINDOW();
	IF (WINDOW_TYPE(Ventana) == "MAP")
		ACTION_COPY();
		DESKTOP_REMOVEWINDOW(Ventana);
	END
END

// -----------------------------------------------------------------------------
//   ACTION_COPY
// -----------------------------------------------------------------------------
//
//   Copia el mapa actual en el portapapeles
//

PROCESS ACTION_COPY()
PRIVATE
	Ventana;
BEGIN
	Ventana = DESKTOP_TOPWINDOW();
	IF (WINDOW_TYPE(Ventana) == "MAP")
	        REGEX("^([0-9]*),([0-9]*) ", WINDOW_DATA(Ventana));
		COPY_GRAPH (REGEX_REG[1], REGEX_REG[2]);
	END
END

// -----------------------------------------------------------------------------
//   ACTION_OPEN
// -----------------------------------------------------------------------------
//
//   Muestra el cuadro de diálogo de abrir fichero y crea las ventanas
//   adecuadas al tipo de fichero que se desea abrir, a partir de su 
//   extensión. Este proceso duerme a su padre mientras funciona.
//

PROCESS ACTION_OPEN()
PRIVATE
	INT    Library;
	INT    Map;
	INT    Is_Import;
	STRING Extension;
	STRING Remaining;
	INT    C, NUM = 1;
BEGIN
	SIGNAL (FATHER, S_SLEEP);
	OpenFileDialog (TXT_AbrirDLG, TXT_Filetypes, OF_MULTISELECT);
	FRAME;

	// Pide un curso de acción si se intentan abrir múltiples ficheros

	IF (REGEX("(fpg|map|png|jpeg);.*", OF_FILE) > -1 && OF_Result)
		MULTIPLE_MAP_OPEN();
		FRAME;
		IF (MultiMapAction == 1)
			FILE = FPG_NEW();
		END
	END

	// ¿Pulsó el usuario Aceptar?

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

		IF (FILE == 0)
			SHOW_MESSAGE("Opening " + OF_FILE);
		END

		Library = 0;
		Map = 0;

		// Busca la extensión del fichero elegido

		IF (REGEX("\.(.{2,4})$", OF_File) != -1)

			// Abre una librería o imagen, en función de esta

			Extension = UCASE(REGEX_REG[1]);

			SWITCH (Extension)
				CASE "FPG":
					Library = LOAD_FPG (OF_File);
					LOAD_PAL (OF_File);
				    END
				CASE "MAP":
					Map = LOAD_MAP (OF_File);
				    END
			        CASE "PNG":
					Map = LOAD_PNG (OF_File);
					IF (Map != 0)
						MAP_SET_NAME (0, Map, OF_File);
					END
				    END
				DEFAULT:
					Map = LOAD_IMAGE(OF_File);
					IF (Map != 0)
						LOAD_IMAGE_PAL(OF_File);
						MAP_SET_NAME (0, Map, OF_File);
						Is_Import = TRUE;
					END
				    END
			END
		ELSE
			// Extensión desconocida. Trata de abrirlo como imagen.

			Map = LOAD_IMAGE(OF_File);
		END

		// Abre la ventana correspondiente al tipo y la marca
		// como importación si es necesario

		IF (Library > 0)
			OPEN_FPG (Library, OF_File);
		ELSEIF (Map > 0)
			IF (FILE == 0)
				OPEN_MAP (0, Map, OF_File);
				IF (LAST_MAP_WINDOW != 0 && Is_Import)
					WINDOW_DATA (LAST_MAP_WINDOW, WINDOW_DATA(LAST_MAP_WINDOW) + ":IMPORT");
				END
			ELSE
				FPG_ADD (FILE, NUM++, 0, MAP);
				UNLOAD_MAP(0, MAP);
			END
		ELSE
			// No se pudo abrir ningún fichero

			MessageBox (TXT_ERROP1 + OF_File,
				TXT_ERROP2 + OF_File + "." + CHR(10) + TXT_ERROP3,
				MB_DANGER);
			BREAK;
		END

		// Pasa al siguiente fichero

		IF (C >= 0)
			OF_File = Remaining;
		END
	END
	IF (FILE != 0)
		OPEN_FPG (FILE, TXT_Untitled);
	END
	SIGNAL (FATHER, S_WAKEUP);
END

// -----------------------------------------------------------------------------
//   CHECK_FPG_DEPTH
// -----------------------------------------------------------------------------
//
//   Comprueba que todos los mapas de un FPG tengan la misma profundidad de
//   color y, si no fuera el caso, los convierte a 16 bits
//

PROCESS CHECK_FPG_DEPTH (Library)
PRIVATE I, Last, Differents;
BEGIN
	FROM I = 0 TO 999:
		IF (MAP_EXISTS(Library, I))
			IF (Last == 0)
				Last = GRAPHIC_INFO(Library, I, G_DEPTH);
			ELSE
				IF (Last != GRAPHIC_INFO(Library, I, G_DEPTH))
					Differents = TRUE;
					BREAK;
				END
			END
		END
	END

	IF (Differents)
		FROM I = 0 TO 999:
			IF (MAP_EXISTS(Library, I))
				MAP_SETDEPTH(Library, I, 16);
			END
		END
		MenuAction = "CONVERT16";
	END
END

// -----------------------------------------------------------------------------
//   ACTION_SAVE
// -----------------------------------------------------------------------------
//
//   Examina la ventana que actualmente tiene el foco, y almacena en disco
//   su contenido (fichero FPG o MAP) siempre que la ventana tenga nombre
//   asignado. Si la ventana corresponde a un fichero importado, invoca
//   a ACTION_SAVEAS en lugar de sobreescribirlo.
//

PROCESS ACTION_SAVE()
PRIVATE
	INT    Ventana;
	STRING Library;
	STRING Map;
	STRING Data;
	STRING Filename;
BEGIN
	Ventana = DESKTOP_TOPWINDOW();
	
	// Si es un gráfico importado, sigue en SAVEAS

	IF (REGEX(":IMPORT$", WINDOW_DATA(Ventana)) != -1)
		RETURN ACTION_SAVEAS();
	ELSEIF (WINDOW_TITLE(Ventana) == TXT_Untitled)
		RETURN ACTION_SAVEAS();
	ELSEIF (REGEX(":EMBEDDED$", WINDOW_DATA(Ventana)) != -1)
		RETURN;
	ELSEIF (WINDOW_FILE(Ventana) == "")
		RETURN ACTION_SAVEAS();
	END

	// Obtiene el nombre de fichero (el formato de los datos de ventana
	// es siempre "LIBRERÍA,MAPA FICHERO" o "LIBRERÍA FICHERO")

	Filename = WINDOW_DATA(Ventana);
	Filename = REGEX_REPLACE("^([0-9]+)(,([0-9]+))? ", "", Filename);
	Library  = REGEX_REG[1];
	Map      = REGEX_REG[3];

	IF (WINDOW_FILE(Ventana) != "")
		Filename = WINDOW_FILE(Ventana);
	END

	IF (Filename == "")
		RETURN ACTION_SAVEAS();
	END

	SHOW_MESSAGE("Saving " + Filename);

	// Graba el contenido a disco

	SWITCH (WINDOW_TYPE(Ventana))
		CASE "FPG":
			CHECK_FPG_DEPTH(Library);
			IF (SAVE_FPG(Library, Filename))
				WINDOW_CHANGED (Ventana, FALSE);

				// Busca otras ventanas de esta librería y quita
				// la marca de modificado también a ellas

				Ventana = DESKTOP_BOTTOMWINDOW();
				WHILE (Ventana != 0)
					IF (REGEX("^"+Library+",", WINDOW_DATA(Ventana)) != -1)
					    	WINDOW_CHANGED(Ventana, FALSE);
					END
					Ventana = DESKTOP_NEXTWINDOW(Ventana);
				END
			END
		    END

		CASE "MAP":

			// Determina si el gráfico es un PNG o MAP

			IF (REGEX("\.png$", Filename) != -1)
				IF (SAVE_PNG(Library, Map, Filename))
					WINDOW_CHANGED (Ventana, FALSE); 
				END
			ELSEIF (SAVE_MAP(Library, Map, Filename)) 
				WINDOW_CHANGED (Ventana, FALSE); 
			END
		    END
	END
END

// -----------------------------------------------------------------------------
//   ACTION_SAVEAS
// -----------------------------------------------------------------------------
//
//   Examina la ventana que actualmente tiene el foco, y muestra un cuadro
//   de diálogo "Guardar como" para que el usuario escoja un nombre. Si
//   el usuario acepta el cuadro, elimina cualquier marca de importación
//   de la ventana y continúa en SAVE.
//

PROCESS ACTION_SAVEAS ()
PRIVATE
	Ventana;
	STRING Tipos;
BEGIN
	// Averigua el tipo de la ventana actual

	Ventana = DESKTOP_TOPWINDOW();
	IF (WINDOW_TYPE(Ventana) == "FPG")
		Tipos = "*.FPG;*.*";
	ELSEIF (WINDOW_TYPE(Ventana) == "MAP")
		Tipos = "Bitmaps:\.(png|map)$;*.*";
	ELSE
		RETURN;
	END

	// Muestra el cuadro de diálogo

	SIGNAL (FATHER, S_SLEEP);
	OF_File = WINDOW_DATA(Ventana);
	OF_File = REGEX_REPLACE("^"+TXT_Untitled+"$", "", OF_File);
	OF_File = REGEX_REPLACE("^[^ ]* ", "", OF_File);
	OF_File = REGEX_REPLACE("\.[a-z]{2,4}:IMPORT$", ".map", OF_File);
	OF_File = REGEX_REPLACE(":IMPORT$", "", OF_File);
	OpenFileDialog (TXT_GuardarDLG, Tipos, OF_ASKOVERWRITE | OF_USEDEFAULT);
	FRAME;

	// Continúa en SAVE si el usuario acepta

	IF (OF_Result)

		// Asigna el nuevo nombre de fichero

		WINDOW_FILE (Ventana, CD() + "\" + OF_File);
		WINDOW_DATA (Ventana, 
			REGEX_REPLACE("^([0-9,]+) .*$", "\1 "+OF_File, 
				WINDOW_DATA(Ventana)));
		WINDOW_TITLE (Ventana, OF_File);
		ACTION_SAVE();
	END
	SIGNAL (FATHER, S_WAKEUP);
END

// -----------------------------------------------------------------------------
//   ACTION_ABOUT
// -----------------------------------------------------------------------------
//
//   Muestra el cuadro de diálogo "acerca de"
//

PROCESS ACTION_ABOUT()
PRIVATE Ventana, BOK, BC;
BEGIN
	SIGNAL (FATHER, S_SLEEP);
	Ventana = WINDOW(TXT_AboutTit, STYLE_DIALOG);
	WINDOW_ADDCONTROL (Ventana, 0, 0, IMAGE_VIEW (GUI_FILE, 950));
	WINDOW_ADDCONTROL (Ventana, 110, 0, LABEL(TXT_Version));
	WINDOW_ADDCONTROL (Ventana, 110, 15, LABEL(TXT_Copyright1));
	WINDOW_ADDCONTROL (Ventana, 110, 25, LABEL(TXT_Copyright2));
	WINDOW_ADDCONTROL (Ventana, 110, 130, BOK = BUTTON(TXT_Aceptar));
	WINDOW_ADDCONTROL (Ventana, 240, 130, BC = BUTTON(TXT_License));
	WINDOW_SET (Ventana, WINDOW_MODAL, TRUE);
	DESKTOP_ADDWINDOW (Ventana);
	FRAME;
	BUTTON_PRESSED(BOK);
	WHILE (WINDOW_VISIBLE (Ventana)) 
		IF (BUTTON_PRESSED(BC)) MESSAGEBOX (TXT_License, REGEX_REPLACE("\^", CHR(10), TXT_GNU), MB_INFO); END
		IF (BUTTON_PRESSED(BOK)) BREAK; END
		FRAME; 
	END
	WINDOW_DESTROY (Ventana);
	SIGNAL (FATHER, S_WAKEUP);
END

// -----------------------------------------------------------------------------
//   ACTION_EXIT
// -----------------------------------------------------------------------------
//
//   Comprueba si hay ventanas abiertas tipo FPG o MAP por guardar. 
//   Abandona luego el programa, con confirmación si es preciso.
//

PROCESS ACTION_EXIT()
PRIVATE
	INT    Ventana, Total;
	STRING Lista;
	STRING Ultimo;
BEGIN
	// Hace una lista de todas las ventanas modificadas pero no grabadas

	Ventana = DESKTOP_BOTTOMWINDOW();
	WHILE (Ventana != 0)
		IF (WINDOW_TYPE(Ventana) == "FPG" || WINDOW_TYPE(Ventana) == "MAP")
			IF (WINDOW_CHANGED(Ventana))
				Lista += "     - "+WINDOW_TITLE(Ventana)+CHR(10);
				Ultimo = WINDOW_TITLE(Ventana);
				Total++;
			END
		END
		Ventana = DESKTOP_NEXTWINDOW(Ventana);
	END

	// Pide confirmación para salir, si se encontraron ventanas

	IF (Lista != "")
		IF (Total == 1)
			MessageBox (TXT_Salir0, TXT_Salir3+Ultimo+TXT_Salir4,
				MB_DANGER | MB_YESNO);
		ELSE
			MessageBox (TXT_Salir0, TXT_Salir1+ CHR(10)+Lista+TXT_Salir2,
				MB_DANGER | MB_YESNO);
		END
		IF (MB_RESULT)
			EXIT (0, 0);
		END
	ELSE
		EXIT (0, 0);
	END
END

// -----------------------------------------------------------------------------
//   Ventana informativa
// -----------------------------------------------------------------------------
//
//   Muestra diversas estadísticas sobre el programa
//

PROCESS OPEN_INFO()
PRIVATE Ventana, Labels[9], Libs, Maps, K, I, C, UNDO_MEMORY;
BEGIN
	Ventana = WINDOW(TXT_InfoTit, STYLE_FIXED);
	WINDOW_ADDCONTROL (Ventana,   0,   0, LABEL(TXT_InfoMem1));
	WINDOW_ADDCONTROL (Ventana,   0,  10, LABEL(TXT_InfoMem2));
	WINDOW_ADDCONTROL (Ventana,   0,  20, LABEL(TXT_InfoMem3));
	WINDOW_ADDCONTROL (Ventana,   0,  30, LABEL(TXT_InfoMem4));
	WINDOW_ADDCONTROL (Ventana,   0,  40, LABEL(TXT_InfoMem5));
	FROM X = 0 TO 40 STEP 10:
		WINDOW_ADDCONTROL (Ventana, 120, X, Labels[X/10] = LABEL("", 2, 100));
	END

	LOOP
		WHILE (!ShowInfo) FRAME; END

		DESKTOP_ADDWINDOW(Ventana);

		WHILE (ShowInfo)
			
			// Averigua cuantos mapas del sistema hay en memoria 
			// y cuenta la memoria que ocupa cada uno (aproximadamente)

			Libs = Maps = K = 0;
			FROM X = 1000 TO 5000:
				IF (MAP_EXISTS(0, X))
					Maps++;
					K += GRAPHIC_INFO(0,X,G_WIDTH)
					   * GRAPHIC_INFO(0,X,G_PITCH);
				END
			END

			// Hace lo propio con librerías del usuario

			FROM X = 0 TO 999:
				IF (FPG_EXISTS(X))
					Libs++;
					FROM Y = 0 TO 999:
						IF (MAP_EXISTS(X, Y))
							Maps++;
							K += GRAPHIC_INFO(X,Y,G_WIDTH)
							   * GRAPHIC_INFO(X,Y,G_PITCH);
						END
					END
				END
			END

			// Cuenta el total de memoria de deshacer acumulada
			// entre todas las ventanas tipo MAP abiertas

			I = DESKTOP_BOTTOMWINDOW();
			UNDO_MEMORY = 0;
			WHILE (I != 0)
				IF (WINDOW_TYPE(I) == "MAP")
					C = WINDOW_CONTROL(I, 0);
					UNDO_MEMORY += GREDITOR_UNDOMEM(C);
				END
				I = DESKTOP_NEXTWINDOW(I);
			END

			// Muestra la información en las etiquetas

//			LABEL_TEXT (Labels[0], FORMAT(MEMORY_TOTAL()/1024) + " KB");
//			LABEL_TEXT (Labels[1], FORMAT(MEMORY_FREE()/1024) + " KB");
			LABEL_TEXT (Labels[2], FORMAT(UNDO_MEMORY/1024) + " KB");
			LABEL_TEXT (Labels[3], FORMAT(Maps) + ": " + FORMAT(K/1024) + " KB");
			LABEL_TEXT (Labels[4], FORMAT(Libs));

			// Espera unos 30 frames hasta volver a recopilar la información

			X = 0;
			WHILE (ShowInfo && X++ < 30 && WINDOW_VISIBLE(Ventana)) FRAME; END
			IF (!WINDOW_VISIBLE(Ventana))
				ShowInfo = FALSE;
			END
		END
	END
END

// -----------------------------------------------------------------------------
//   DIALOG_OPTIONS
// -----------------------------------------------------------------------------
//
//   Muestra el cuadro de diálogo de opciones generales
//

PROCESS DIALOG_OPTIONS()
PRIVATE
	INT	Ventana;
	INT	ShowGrid;
	INT	ShowPixelGrid;
	INT	GridColor;
	INT	PixelGridColor;
	INT	PaperMode;
	INT	PaperColor;
	INT	BOK, BCANCEL;
	INT	Result;
	INT	T;
	CHAR    GridSpacing[3];
	STRING	Title;

	EDITORINFO POINTER EdInfo;
BEGIN
	// Obtiene las características del editor actual

	Ventana = DESKTOP_TOPWINDOW();
	IF (WINDOW_TYPE(Ventana) == "MAP")
		EdInfo = GREDITOR_INFO(WINDOW_CONTROL(Ventana, 0));
		Title = TXT_ODLGTitle2;
	ELSE
		EdInfo = &DefaultOptions;
		Title = TXT_ODLGTitle1;
	END

	GridSpacing = EdInfo.GridStep;
	ShowPixelGrid = (EdInfo.GridVisible & 1);
	ShowGrid = (EdInfo.GridVisible & 2) >> 1;
	GridColor = EdInfo.GridColor;
	PixelGridColor = EdInfo.PixelGridColor;
	PaperMode = EdInfo.PaperMode;
	PaperColor = EdInfo.PaperColor;

	// Crea la ventana y sus controles

	T = TABS(220, 195);
	TABS_ADDPAGE (T, "Pencil");
	TABS_ADDPAGE (T, "Brush");
	TABS_ADDPAGE (T, "Display");
	TABS_ADDCONTROL (T,  10, 10, CHECKBOX(TXT_ODLGShowG, &ShowGrid));
	TABS_ADDCONTROL (T,  40, 30, LABEL(TXT_ODLGGridSp));
	TABS_ADDCONTROL (T, 138, 30, INPUTLINE(3, GridSpacing));
	TABS_ADDCONTROL (T,  40, 50, LABEL(TXT_ODLGColor));
	TABS_ADDCONTROL (T, 150, 50, CLICKABLE_COLOR(&GridColor, CCM_ENABLEPALETTE));
	TABS_ADDCONTROL (T,  10, 80, CHECKBOX(TXT_ODLGShowPG, &ShowPixelGrid));
	TABS_ADDCONTROL (T,  40,100, LABEL(TXT_ODLGColor));
	TABS_ADDCONTROL (T, 150,100, CLICKABLE_COLOR(&PixelGridColor, CCM_ENABLEPALETTE));
	TABS_ADDCONTROL (T,  10,130, CHECKBOX(TXT_ODLGPaper, &PaperMode));
	TABS_ADDCONTROL (T,  40,150, LABEL(TXT_ODLGPColor));
	TABS_ADDCONTROL (T, 150,150, CLICKABLE_COLOR(&PaperColor, CCM_ENABLEPALETTE));

	Ventana = WINDOW(Title, STYLE_DIALOG);
	WINDOW_SET (Ventana, WINDOW_MODAL, TRUE);
	WINDOW_ADDCONTROL (Ventana,   0,  0, T);
	WINDOW_ADDCONTROL (Ventana,  10,205, BOK = BUTTON(TXT_Aceptar));
	WINDOW_ADDCONTROL (Ventana, 120,205, BCANCEL = BUTTON(TXT_Cancelar));
	DESKTOP_ADDWINDOW(Ventana);

	// Bucle principal de ventana

	SCAN_CODE = 0;

	WHILE (WINDOW_VISIBLE(Ventana))
		IF (SCAN_CODE == _ENTER || BUTTON_PRESSED(BOK))
			SCAN_CODE = 0;
			Result = TRUE;
			BREAK;
		END
		IF (SCAN_CODE == _ESC || BUTTON_PRESSED(BCANCEL))
			SCAN_CODE = 0;
			BREAK;
		END
		FRAME;
	END

	// Actualiza los datos

	IF (Result == TRUE)
		EdInfo.GridVisible = (ShowGrid << 1) | ShowPixelGrid;
		EdInfo.GridColor = GridColor;
		EdInfo.PaperMode = PaperMode;
		EdInfo.PaperColor = PaperColor;
		EdInfo.GridStep = GridSpacing;
		EdInfo.PixelGridColor = PixelGridColor;
	END

	DESKTOP_REMOVEWINDOW(Ventana);
	WINDOW_DESTROY(Ventana);
END

// -----------------------------------------------------------------------------
//   OPEN_PREVIEW
// -----------------------------------------------------------------------------
//
//   Muestra la ventana de "preview"
//

PROCESS OPEN_PREVIEW()
PRIVATE
	INT			Ventana, Editor, Preview, Top;
	EDITORINFO		Info;
	EDITORINFO POINTER    	CurrentInfo;
BEGIN
	Ventana = WINDOW("Preview", STYLE_NORMAL);
	WINDOW_ADDCONTROL (Ventana, 0, 0, Preview = GREDITOR (0, 0, &INFO, 64, 64));
	GREDITOR_SHOWBARS (Preview, FALSE);
	WINDOW_SET (Ventana, WINDOW_CAPTION, 0);
	WINDOW_SET (Ventana, WINDOW_ONTOP, 1);
	WINDOW_SET (Ventana, WINDOW_DONTFOCUS, 1);
	WINDOW_MOVE (Ventana, 550, 24);

	LOOP
		DESKTOP_REMOVEWINDOW(Ventana);

		IF (ShowPreview)
			Top = DESKTOP_TOPWINDOW();
			IF (WINDOW_TYPE(Top) == "MAP")
				Editor = WINDOW_CONTROL(DESKTOP_TOPWINDOW(), 0);
				IF (GREDITOR_ACTIVE(Editor))
					CurrentInfo = GREDITOR_INFO(Editor);
					Info.FILE = CurrentInfo.FILE;
					Info.GRAPH = CurrentInfo.GRAPH;
					Info.PaperMode = CurrentInfo.PaperMode;
					Info.PaperColor = CurrentInfo.paperColor;
					GREDITOR_ZOOMTO (Preview, CurrentInfo.CursorX, CurrentInfo.CursorY, 100);
					DESKTOP_ADDWINDOW(Ventana);
				END
			END
		END
		FRAME;
	END
END

// -----------------------------------------------------------------------------
//   Proceso principal
// -----------------------------------------------------------------------------
//
//   Crea el menú principal, así como la barra de tareas y la de menús.
//   Contiene el bucle principal, que interpreta además atajos de teclado.
//

PRIVATE
	MenuFichero;
	MenuEditar;
	MenuEditarFPG;
	MenuEditarMAP;
	MenuVer;
	MenuAyuda;
	TB;
	Ventana;
	VentanaPaleta;
	VentanaInfo;
	PaletaEditor;
	EditorInfo info;
BEGIN
	ALPHA_STEPS = 8;
	SET_MODE (640, 480, MODE_16BITS);
	DUMP_TYPE = 1;
	RESTORE_TYPE = 1;
	SAVE_PAL("default.pal");
	PUT_SCREEN (0, LOAD_MAP("bg640x480.map"));
	GUI_FILE = LOAD_FPG("gui.fpg");
	BrushFile = LOAD_FPG("brushes.fpg");
	TextureFile = LOAD_FPG("textures.fpg");
	LOAD_PAL("default.pal");
	SET_FPS (75, 0);

	MESSAGE_BOX();

	// Crea la paleta de edición de gráficos

	PaletaEditor = WINDOW("Editor Palette", STYLE_DIALOG);
	WINDOW_ADDCONTROL (PaletaEditor, 0, 0, 
		EditorPalette = GRPALETTE (&info, LOAD_FPG("tools.fpg")));
	WINDOW_SET (PaletaEditor, WINDOW_CAPTION, 0);
	WINDOW_SET (PaletaEditor, WINDOW_ONTOP, 1);
	WINDOW_SET (PaletaEditor, WINDOW_DONTFOCUS, 1);
	WINDOW_MOVE (PaletaEditor, 16, 400);

	// Crea la ventana de paleta

	VentanaPaleta = WINDOW(TXT_Paleta, STYLE_NORMAL);
	WINDOW_ADDCONTROL (VentanaPaleta, 0, 0, 
		PalettePanel = PALETTE_PANEL(128, 128));
	WINDOW_SET (VentanaPaleta, WINDOW_CAPTION, 0);
	WINDOW_SET (VentanaPaleta, WINDOW_ONTOP, 1);
	WINDOW_SET (VentanaPaleta, WINDOW_DONTFOCUS, 1);
	WINDOW_MOVE (VentanaPaleta, 16, 24);

	// Crea el menú principal

	MenuFichero = MENU();
	MENU_ACTION (MenuFichero, TXT_NuevoFPG, TYPE ACTION_NEWFPG);
	MENU_ACTION (MenuFichero, TXT_NuevoMAP, TYPE DIALOG_NEW_MAP);
	MENU_SEPARATOR (MenuFichero);
	MENU_ACTION (MenuFichero, TXT_Abrir, TYPE ACTION_OPEN);
	MENU_ACTION (MenuFichero, TXT_Guardar, TYPE ACTION_SAVE);
	MENU_ACTION (MenuFichero, TXT_GuardarComo, TYPE ACTION_SAVEAS);
	MENU_SEPARATOR (MenuFichero);
	MENU_ACTION (MenuFichero, TXT_Salir, TYPE ACTION_EXIT);
	MenuEditar = MENU();
	MENU_ACTION (MenuEditar, TXT_Pegar, TYPE ACTION_PASTE);
	MENU_SEPARATOR (MenuEditar);
	MENU_ACTION (MenuEditar, TXT_PEditor, TYPE DIALOG_PALETTE_EDITOR);
	MENU_ACTION (MenuEditar, TXT_POptions, TYPE DIALOG_OPTIONS);
	MenuEditarFPG = MENU();
	MENU_ACTION (MenuEditarFPG, TXT_Copiar, TYPE ACTION_COPY);
	MENU_ACTION (MenuEditarFPG, TXT_Cortar, TYPE ACTION_CUT);
	MENU_ACTION (MenuEditarFPG, TXT_Pegar, TYPE ACTION_PASTE);
	MENU_SEPARATOR (MenuEditarFPG);
	MENU_ACTION (MenuEditarFPG, TXT_Slice, TYPE ACTION_SLICE);
	MENU_ACTION (MenuEditarFPG, TXT_Convert8, TYPE ACTION_CONVERT8);
	MENU_ACTION (MenuEditarFPG, TXT_Convert16, TYPE ACTION_CONVERT16);
	MENU_SEPARATOR (MenuEditarFPG);
	MENU_ACTION (MenuEditarFPG, TXT_PEditor, TYPE DIALOG_PALETTE_EDITOR);
	MENU_ACTION (MenuEditarFPG, TXT_POptions, TYPE DIALOG_OPTIONS);
	MenuEditarMAP = MENU();
	MENU_ACTION (MenuEditarMAP, TXT_Copiar, TYPE ACTION_COPY);
	MENU_ACTION (MenuEditarMAP, TXT_Cortar, TYPE ACTION_CUT);
	MENU_ACTION (MenuEditarMAP, TXT_PegarDentro, TYPE ACTION_PASTE);
	MENU_ACTION (MenuEditarMAP, TXT_Pegar, TYPE ACTION_PASTE);
	MENU_SEPARATOR (MenuEditarMAP);
	MENU_ACTION (MenuEditarMAP, TXT_PEditor, TYPE DIALOG_PALETTE_EDITOR);
	MENU_ACTION (MenuEditarMAP, TXT_POptions, TYPE DIALOG_OPTIONS);
	MenuVer = MENU();
	MENU_TOGGLE (MenuVer, TXT_VerEdPal, &ShowEdPal);
	MENU_TOGGLE (MenuVer, TXT_VerEdPre, &ShowPreview);
	MENU_TOGGLE (MenuVer, TXT_VerPal, &ShowPal);
	MENU_SEPARATOR (MenuVer);
	MENU_TOGGLE (MenuVer, TXT_VerInfo, &ShowInfo);
	MenuAyuda = MENU();
	MENU_ACTION (MenuAyuda, TXT_AcercaDe, TYPE ACTION_ABOUT);

	// Crea la barra de menús

	Ventana = WINDOW (STYLE_DOCK_NORTH);
	WINDOW_ADDCONTROL (Ventana, 0, 0, TB = TOOLBAR());
	TOOLBAR_MENU (TB, TXT_Fichero, MenuFichero);
	TOOLBAR_MENU (TB, "MAP", TXT_Editar, MenuEditarMAP);
	TOOLBAR_MENU (TB, "FPG", TXT_Editar, MenuEditarFPG);
	TOOLBAR_MENU (TB, "NONE", TXT_Editar, MenuEditar);
	TOOLBAR_MENU (TB, TXT_Ver, MenuVer);
	TOOLBAR_MENU (TB, TXT_Ayuda, MenuAyuda);
	DESKTOP_ADDWINDOW (Ventana);

	// Crea la barra de tareas

	Ventana = WINDOW (STYLE_DOCK_SOUTH);
	WINDOW_ADDCONTROL (Ventana, 0, 0, TASKBAR());
	DESKTOP_ADDWINDOW (Ventana);

	// Bucle principal

	GUI_SHOW(0);

	OPEN_INFO();
	OPEN_PREVIEW();

	LOOP
		FRAME;

		IF (FIND(WINDOW_TYPE(DESKTOP_TOPWINDOW()), "MAP") == 0 && ShowEdPal)
			IF (GREDITOR_ACTIVE(WINDOW_CONTROL (DESKTOP_TOPWINDOW(), 0)))
				DESKTOP_ADDWINDOW(PaletaEditor);
			ELSE
				DESKTOP_REMOVEWINDOW(PaletaEditor);
			END
		ELSE
			DESKTOP_REMOVEWINDOW(PaletaEditor);
		END

		IF (ShowPal)
			DESKTOP_ADDWINDOW(VentanaPaleta);
		ELSE
			DESKTOP_REMOVEWINDOW(VentanaPaleta);
		END

		// Interpreta atajos globales de teclado

		IF (KEY(_CONTROL))
			SWITCH (SCAN_CODE)
				CASE _O:			// CTRL+O: Abrir
					SCAN_CODE = 0;
					ACTION_OPEN();
				END
				CASE _S:			// CTRL+S: Guardar
					SCAN_CODE = 0;
					ACTION_SAVE();
				END
				CASE _N:			// CTRL+N: Nuevo mapa
					SCAN_CODE = 0;
					DIALOG_NEW_MAP();
				END
			END
		END
	END
END

// Procesos por hacer

PROCESS ACTION_NEWMAP(); BEGIN END
PROCESS ACTION_NEWFPG() 
BEGIN 
	FILE = FPG_NEW();
	OPEN_FPG (FILE, TXT_Untitled);
END

PROCESS ACTION_SLICE() 		BEGIN MenuAction = "SLICE"; END
PROCESS ACTION_CONVERT16() 	BEGIN MenuAction = "CONVERT16"; END
PROCESS ACTION_CONVERT8() 	BEGIN MenuAction = "CONVERT8"; END

