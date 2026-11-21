/*
 *  Prueba de teclado
 *  -----------------
 *  Este programa muestra simplemente todos los códigos de KEY a la
 *  vez en pantalla, para poder comprobar que cambian correctamente
 *  al pulsar teclas. Usa ALT+X para salir.
 */

GLOBAL
	STRING KeyName[100] =
		"",
		"ESC",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"0",
		"MINUS",
		"PLUS",
		"BACKSPACE",
		"TAB",
		"Q",
		"W",
		"E",
		"R",
		"T",
		"Y",
		"U",
		"I",
		"O",
		"P",
		"L_BRACHET",
		"R_BRACHET",
		"ENTER",
		"CONTROL",
		"A",
		"S",
		"D",
		"F",
		"G",
		"H",
		"J",
		"K",
		"L",
		"SEMICOLON",
		"APOSTROPHE",
		"WAVE",
		"L_SHIFT",
		"BACKSLASH",
		"Z",
		"X",
		"C",
		"V",
		"B",
		"N",
		"M",
		"COMMA",
		"POINT",
		"SLASH",
		"R_SHIFT",
		"PRN_SCR",
		"ALT",
		"SPACE",
		"CAPS_LOCK",
		"F1",
		"F2",
		"F3",
		"F4",
		"F5",
		"F6",
		"F7",
		"F8",
		"F9",
		"F10",
		"NUM_LOCK",
		"SCROLL_LOCK",
		"HOME",
		"UP",
		"PGUP",
		"C_MINUS",
		"LEFT",
		"C_CENTER",
		"RIGHT",
		"C_PLUS",
		"END",
		"DOWN",
		"PGDN",
		"INS",
		"DEL",
		"",
		"",
		"",
		"F11",
		"F12",
		"LESS",
		"EQUALS",
		"GREATER",
		"ASTERISK",
		"R_ALT",
		"R_CONTROL",
		"L_ALT",
		"L_CONTROL",
		"MENU",
		"L_WINDOWS",
		"R_WINDOWS",
		"*"
	    ; 
	    
PRIVATE
	i;
BEGIN
	SET_TITLE ("Test de Teclado");
	SET_MODE (640, 480, MODE_16BITS);
	SET_FPS (0, 0);

	WHILE (!KEY(_ALT) || !KEY(_X))
		DELETE_TEXT(0);
		WRITE (0, 384, 460, 0, "SHIFT_STATUS = " + SHIFT_STATUS);
		x = 40;
		y = 10;
		FOR (i = 1 ; KeyName[i] != "*" ; i++)
			SET_TEXT_COLOR (RGB(128,128,128));
			IF (KEY(i))
				SET_TEXT_COLOR (RGB(255,255,255));
				WRITE (0, x+200, y, 0, KEY(i));
			END
			WRITE (0, x, y, 2, FORMAT(i));
			WRITE (0, x+25, y, 0, KeyName[i]);
			y += 9;
			IF (y > 460)
				y = 10;
				x += 364;
			END
	    END
		FRAME;
	END
END
