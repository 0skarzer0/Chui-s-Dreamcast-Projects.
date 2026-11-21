
/** SHOOTOUT.INC
 *
 *	Contiene rutinas utilizadas por las rutinas del directorio shootout
 *	para medir el tiempo que tarda un proceso determinado
 */

GLOBAL
	Timer1;
	Timer2;
	Initial;
	Remaining;
END

PROCESS Shootout (STRING Title, STRING Subtitle)
BEGIN
	FULL_SCREEN = FALSE;
	SET_MODE (320, 240, 16);
	WRITE (0, 160, 40, 1, Title);
	WRITE (0, 160, 60, 1, Subtitle);
END

PROCESS BeginShootout()
BEGIN
	Initial = MEMORY_FREE();
	Timer1 = TIMER[0];
END

PROCESS EndShootout (STRING Result)
BEGIN
	FRAME;
	Remaining = MEMORY_FREE();
	Timer2 = TIMER[0];
	WRITE (0, 160, 120, 1, "Tiempo empleado: " 
	    + FORMAT((Timer2-Timer1)/100.0, 2) + " s");
	WRITE (0, 160, 140, 1, "Uso de memoria: "
	    + FORMAT((Remaining - Initial)/1024) + " K");
	WRITE (0, 160, 160, 1, Result);
	DRAWING_Z (0);
	DRAWING_COLOR (RGB (255, 255, 255));
	DRAW_LINE (0, 110, 319, 110);
	DRAW_LINE (0, 178, 319, 178);
	WRITE (0, 160, 230, 1, "Pulse ENTER para salir");

	WHILE (NOT KEY(_ESC) AND NOT KEY(_ENTER))
		FRAME;
	END
END
