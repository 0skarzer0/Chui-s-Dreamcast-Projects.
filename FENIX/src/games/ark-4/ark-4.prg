// Ark-4 - Juego de ejemplo para Fenix
// ----------------------------------
//
// (c) 2000 Carlos Guillermo Coll Silvera
// 
// Este juego se considera de dominio público. Puedes hacer lo que quieras
// con él y con su código, incluyendo modificarlo, redistribuirlo, o
// utilizarlo en tus propios programas de cualquier manera.
//
// ULTIMA REVISION: 28/07/2002, adaptación del programa para su correcto funcionamiento con Fenix 08 por [ManOwaR].
PROGRAM Ark4;

CONST
     Grilla_Ancho = 12;
     Grilla_Alto = 19;
     Ancho_cubo = 36;
     Alto_cubo  = 14;
     x_ini = 120;
     y_ini = 70;
     x_fin = Ancho_cubo * Grilla_Ancho + x_ini -1;
     y_fin = Alto_cubo * Grilla_Alto + y_ini -1;

GLOBAL
      INT Grilla[Grilla_Ancho*Grilla_Alto], regalon;
      INT temp, xtemp, ytemp;
      INT ID_Barra;
      INT x_temp, y_temp;
      INT Niveles;
      INT Lugar;
      INT graf_cubo;
      INT Puntos;
      INT ID_Pelota;
      INT Cant_Niveles = 10;
      INT Demo = 0; //0 = modo normal, 1 = modo DEMO
      INT Nivel;
      INT Quedan_Cubos;
      INT Cant_Vidas;
      INT ID_Vidas[3];
      INT ID_BarraH[1];
      INT ID_BarraV[1];
      INT Dir_Regalo;
      INT Cant_Disparo;
      INT acel_xact;
      INT acel_yact;
      INT acel_x;
      INT acel_y;
      INT mostrando_texto = 0;
      INT OpcionMenu; //Variable necesaria para la creacion de las opciones del menu principal

      INT M_Juego;
      INT M_Menus;
      INT M_Creditos;

      INT S_boing;
      INT S_boing2;
      INT S_fierro;
      INT S_regalo;
      INT S_sefue;
      INT S_disparo;
      INT S_cubito;
      INT S_tecla;
      INT VolumenSFX = 256;
      INT VolumenMODS = 64;

// Codigo de inicio del programa -------------------------------------------------------------------------------------
BEGIN
     // Inicialización
     graph_mode = MODE_16BITS;
     set_mode(m640x480);
     set_fps(40, 0);
     //sound_mode = MODE_STEREO + MODE_HQ + MODE_SURROUND + MODE_16BITS ;

     // Carga de gráficos
     load_fpg("juego.fpg");
     load_fnt("fuente2.fnt");

     // Carga de musicas
     M_Juego = Load_song("mods\juego.it");
     M_Menus = Load_song("mods\menus.mod");
     M_Creditos = Load_song("mods\creditos.mod");

     // Carga de Sonidos
     S_boing   = Load_WAV("sfxs\boing.wav");
     S_boing2  = Load_WAV("sfxs\boing2.wav");
     S_fierro  = Load_WAV("sfxs\fierro.wav");
     S_regalo  = Load_WAV("sfxs\regalo.wav");
     S_sefue   = Load_WAV("sfxs\sefue.wav");
     S_cubito  = Load_WAV("sfxs\cubito.wav");
     S_disparo = Load_WAV("sfxs\disparo.wav");
     S_tecla   = Load_WAV("sfxs\tecla.wav");
     // Presenta_Logo();
     Menu();
END
// Fin del codigo de inicio del programa -------------------------------------------------------------------------------------

// Codigo de presentacion  -------------------------------------------------------------------------------------
PROCESS Light(graph, x, y);
BEGIN
     FROM size = 0 TO 100
        FRAME(10);
     END
     LOOP
         FRAME;
     END
END

PROCESS SINC(graph, x, y, y_final);
BEGIN
        FROM y = y TO y_final
             FRAME(10);
        END
	LOOP
		FRAME;
	END
END

PROCESS ENTERTAINMENT(grafico, x, y);
PRIVATE
       INT R, G, B, y2, temp2, lado = 1, y_temp2, Division;
       maps[39];
BEGIN
     y_temp2 = y;
     Division = graphic_info(0, grafico, G_HEIGHT) / 30;
     IF Division < 1: Division = 1; END
     FROM temp2 = 0 to 39
          maps[temp2] = new_map(graphic_info(0, grafico, G_WIDE) ,2 ,16);
          set_center(0, maps[temp2], 0, 0);
          map_block_copy(0, maps[temp2], 0, 0, grafico, 0, y2, graphic_info(0, grafico, G_WIDE), 2, 0);
          IF Lado == 1:
             Mapa_Temporal(maps[temp2], (graphic_info(0, grafico, G_WIDE)*-1), y_temp2, x, Lado);
          ELSE
             Mapa_Temporal(maps[temp2], 641, y_temp2, x, Lado);
          END
          y2 += Division;
          IF y2 >= graphic_info(0, grafico, G_HEIGHT): BREAK; END
          y_temp2 += Division;
          Lado = Lado * -1;
     END
END

PROCESS Mapa_Temporal(graph, x1, y, x_final, sentido)
BEGIN
     IF sentido == 1:
          FROM x = x1 to x_final
               FRAME(15);
          END
     ELSE
          FROM x = x1 to x_final STEP -1
               FRAME(15);
          END
     END
     x = x_final;
     LOOP
         FRAME;
     END
END

PROCESS Presenta_Logo()
PRIVATE
	INT ID_Temp;
BEGIN
     FRAME(500);
     ID_Temp = Light(60, 213, 204);
     WHILE ID_Temp.size < 100: FRAME; END
     ID_Temp = SINC(61, 448, -50, 204);
     WHILE ID_Temp.y < 204: FRAME; END
     ID_Temp = ENTERTAINMENT(62, 77, 252);
     timer[0] = 0;
     WHILE timer[0] < 500: FRAME; END
     Menu();
END
// Fin del codigo de presentacion  --------------------------------------------------------------------------------

// Codigo del menu principal -----------------------------------------------------------------------------------------
PROCESS efecto_bueno(graph, x, y);
PRIVATE
	INT Ancho_Region = 1;
BEGIN
     define_region(1, x+5, y+5, (graphic_info(0, graph, G_WIDE) - 5),  (graphic_info(0, graph, G_HEIGHT) - 5));
     start_scroll(0,0,51,0,1,3);
     LOOP
	scroll.x0+=2;
 	scroll.y0+=2;
	FRAME ;
     END
END

PROCESS ARK4Logo(graph, x, y, flags, z)
BEGIN
     LOOP
         FRAME;
     END
END

PROCESS MouseFantasma(graph, x, y)
BEGIN
     flags = 4;
     FROM size = 100 TO 0 STEP -5
          FRAME;
     END
END

PROCESS MouseCursor(Mousegraph)
PRIVATE
       INT mouse_x_old, mouse_y_old;
BEGIN
     mouse.graph = Mousegraph;
     LOOP
         mouse_x_old = mouse.x;
         mouse_y_old = mouse.y;
         FRAME;
         IF mouse_y_old <> mouse.y or mouse_x_old <> mouse.x:
            MouseFantasma(999, mouse_x_old, mouse_y_old);
         END
     END
END

PROCESS OpcionFantasma(graph, x_act, y_act)
BEGIN
     flags=4;
     z = 10;
     resolution = 10;
     x = rand(x_act-100, x_act+100);
     y = rand(y_act-100, y_act+100);
     FRAME;
END

PROCESS OpMenu(x_final, y_final, string texto, INT dir, nro_menu)
// dir = 1 - Texto viene de arriba
// dir = 2 - Texto viene de abajo
// dir = 3 - Texto viene de izquierda
// dir = 4 - Texto viene de derecha

PRIVATE
       INT acele_x = 150;
       INT acele_y = 150;
BEGIN
     resolution = 10;
     SWITCH (Dir)
          CASE 1: // De arriba
              graph = write_in_map(1, texto, 0);
              x = (x_final * 10);
              y = -100;
              LOOP
                 IF y < y_final*10:
                    y += acele_y;
                    acele_y-=1;
                    IF acele_y <= 0: acele_y = 1; END
                   ELSE
                    y = y_final*10;
                 END
                 IF mouse.x*10 > x AND mouse.y*10 > y AND mouse.x*10 < x + graphic_info(0, graph, g_WIDE)*10 AND mouse.y*10 < y + graphic_info(0, graph, g_HEIGHT)*10:
                    OpcionFantasma(graph, x, y);
                    IF mouse.left: REPEAT FRAME; UNTIL mouse.left == 0; OpcionMenu = nro_menu; END
                   ELSE
                    Size = 100;
                 END
                 FRAME;
              END
          END
          CASE 2: // De abajo
              graph = write_in_map(1, texto, 0);
              x = (x_final * 10);
              y = 4900;
              LOOP
                 IF y > y_final*10:
                    y -= acele_y;
                    acele_y-=1;
                    IF acele_y <= 0: acele_y = 1; END
                   ELSE
                    y = y_final*10;
                 END
                 IF mouse.x*10 > x AND mouse.y*10 > y AND mouse.x*10 < x + graphic_info(0, graph, g_WIDE)*10 AND mouse.y*10 < y + graphic_info(0, graph, g_HEIGHT)*10:
                    OpcionFantasma(graph, x, y);
                    IF mouse.left: REPEAT FRAME; UNTIL mouse.left == 0; OpcionMenu = nro_menu; END
                   ELSE
                    Size = 100;
                 END
                 FRAME;
              END
          END
          CASE 3: // De la izquierda
              graph = write_in_map(1, texto, 0);
              x = -100;
              y = (y_final * 10);;
              LOOP
                 IF x < x_final*10:
                    x += acele_x;
                    acele_x-=1;
                    IF acele_x <= 0: acele_x = 1; END
                   ELSE
                    x = x_final*10;
                 END
                 IF mouse.x*10 > x AND mouse.y*10 > y AND mouse.x*10 < x + graphic_info(0, graph, g_WIDE)*10 AND mouse.y*10 < y + graphic_info(0, graph, g_HEIGHT)*10:
                    OpcionFantasma(graph, x, y);
                    IF mouse.left: REPEAT FRAME; UNTIL mouse.left == 0; OpcionMenu = nro_menu; END
                   ELSE
                    Size = 100;
                 END
                 FRAME;
              END
          END
          CASE 4: // De la derecha
              graph = write_in_map(1, texto, 0);
              x = 6500;
              y = (y_final * 10);;
              LOOP
                 IF x > x_final*10:
                    x -= acele_x;
                    acele_x-=1;
                    IF acele_x <= 0: acele_x = 1; END
                   ELSE
                    x = x_final*10;
                 END
                 IF mouse.x*10 > x AND mouse.y*10 > y AND mouse.x*10 < x + graphic_info(0, graph, g_WIDE)*10 AND mouse.y*10 < y + graphic_info(0, graph, g_HEIGHT)*10:
                    OpcionFantasma(graph, x, y);
                    IF mouse.left: REPEAT FRAME; UNTIL mouse.left == 0; OpcionMenu = nro_menu; END
                   ELSE
                    Size = 100;
                 END
                 FRAME;
              END
          END
     END
END

PROCESS RegaloDESC(graph, x, y, string texto);
PRIVATE x_tempo = 1, x_tempo2;
BEGIN
   z = -10;
   x_tempo2 = x;
   RegaloDESC2(x + graphic_info(0, graph, g_wide)-10, y,  texto);
   LOOP
       x += x_tempo;
       IF x == x_tempo2+5 or x == x_tempo2-5: x_tempo = x_tempo * -1; END
       FRAME;
   END
END

PROCESS RegaloDESC2(x, y, string texto);
PRIVATE x_ori, y_ori;
BEGIN
   graph = write_in_map(0, texto, 3);
   flags = 4;
   LOOP
    FRAME;
   END
END

PROCESS FallingObjectGhost(graph, x, y)
BEGIN
     flags = 4;
     FROM size = 100 TO 0 STEP -10
          FRAME;
     END
END


// Proceso echo por José Luis Cebrian en su programa test.prg
// Con alguna modificacion mia
PROCESS FallingObject(graph, size, flags, angleinc)
PRIVATE xspeed, yspeed, inispeed, fx_old, fy_old;
BEGIN
	x      = rand (15, 305) ;
	y      = - rand (20, 100) ;
	xspeed = rand (-10, 10) ;
	yspeed = rand (-4, 0) ;
	inispeed = rand (10, 15) ;
	angle  = rand (0, 50 * angleinc);

	LOOP
                fx_old = x;
                fy_old = y;
		x += xspeed;
		IF (x > 630 || x < 15) xspeed = -xspeed; END

		y += yspeed++ ;
                IF (yspeed < -14) yspeed = -14; END
		IF (y > 470)
			yspeed = -inispeed ; 
			IF (inispeed > 1) 
				inispeed--; 
			ELSE              
				inispeed = 15; 
			END
		END

		IF (out_region(ID, 0)): y = -rand(20, 100); inispeed = 15; END
          	angle += angleinc;

		FRAME;

                IF fy_old <> y or fx_old <> x:
                   FallingObjectGhost(graph, fx_old, fy_old);
                END
	END
END
// Fin del proceso echo por José Luis Cebrian en su programa test.prg

PROCESS Menu()
PRIVATE
       INT tempi;
BEGIN
         let_me_alone();
         Solo_YO();
         play_song(M_Menus, -1);
         ARK4Logo(50, 150, 50, 0, 1);
         efecto_bueno(50, 150, 50);
         RegaloDESC(20, 150, 430, "- Disparo");
         RegaloDESC(21, 500, 450, "- Expansion");
         RegaloDESC(22,  30, 450, "- MiniBarra");
         RegaloDESC(23, 400, 430, "- Rebote Hiper-Rapido");
         RegaloDESC(24, 250, 450, "- Pelotita Rapida");
         FROM tempi = 1 to 10
              FallingObject(18, 100, 0, rand(1000, 90000));
         END
         MouseCursor(999);
         OpcionMenu = 0;
         OpMenu(250, 200, "Empezar a jugar", 1, 1);
         OpMenu(250, 230, "Editor de niveles", 2, 2);
         OpMenu(250, 260, "Creditos", 3, 3);
         OpMenu(250, 290, "Salir", 4, 4);
         LOOP
           //mod_set(mod_volume, VolumenMODS);
           SWITCH( OpcionMenu );
              CASE 1: define_region(1,0,0,0,0); STOP_song(); Juego(); END
              CASE 2: define_region(1,0,0,0,0); STOP_song(); Editor(); END
              CASE 3: define_region(1,0,0,0,0); STOP_song(); Creditos(); END
              CASE 4: Exit("", 0); END
           END
           OpcionMenu = 0;
           FRAME;
         END
END
// Fin del codigo del menu principal -----------------------------------------------------------------------------------------

PROCESS Pelota(graph, x, y);
PRIVATE
       tamaño = 2;
       ID_Cubo;
       y_dir = 2;
       x_dir = 2;
       Angulo_1, Angulo_2, Angulo_3, Angulo_4, Angulo_P, Cant_Cubos_temp, x_old, y_old;
       Se_FUE = 0;
BEGIN
     x = 4000;
     y = 500;
     acel_x = 30;
     acel_y = 30;
     acel_xact = 30;
     acel_yact = 30;
     resolution = 10;
     z = -1;
     REPEAT
           size+=tamaño;
           IF size > 120 or size < 80: tamaño = tamaño * -1; END
           FRAME;
     UNTIL mouse.left;
     size = 100;
     REPEAT
         angle = fget_angle(x, y, x+x_dir, y+y_dir);
         x_old = x;
         y_old = y;
         x += get_distx(angle, acel_x);
         y += get_disty(angle, acel_y);
         IF ID_Cubo = collision(TYPE cubo):
            x = x_old;
            y = y_old;
            IF ID_Cubo.flags <> 4:
               ID_CUBO.size = 99;
               Angulo_1 = fget_angle(graphic_info(0, ID_Cubo.graph, G_WIDE)/2, graphic_info(0, ID_Cubo.graph, G_HEIGHT)/2, graphic_info(0, ID_Cubo.graph, G_WIDE), 0 );
               Angulo_2 = fget_angle(graphic_info(0, ID_Cubo.graph, G_WIDE)/2, graphic_info(0, ID_Cubo.graph, G_HEIGHT)/2, 0, 0);
               Angulo_3 = fget_angle(graphic_info(0, ID_Cubo.graph, G_WIDE)/2, graphic_info(0, ID_Cubo.graph, G_HEIGHT)/2, 0, graphic_info(0, ID_Cubo.graph, G_HEIGHT));
               Angulo_4 = fget_angle(graphic_info(0, ID_Cubo.graph, G_WIDE)/2, graphic_info(0, ID_Cubo.graph, G_HEIGHT)/2, graphic_info(0, ID_Cubo.graph, G_WIDE), graphic_info(0, ID_Cubo.graph, G_HEIGHT)) + 360000;
               Angulo_P = fget_angle((ID_Cubo.x*resolution), (ID_Cubo.y*resolution), x, y);
               IF angulo_P < 0: angulo_P += 360000; END

               IF Angulo_P > Angulo_1 AND Angulo_P < Angulo_2: y_dir = y_dir * -1; Dir_Regalo = 1; END //Pega arriba
               IF Angulo_P > Angulo_2 AND Angulo_P < Angulo_3: x_dir = x_dir * -1; Dir_Regalo = 2; END //Pega izquierda
               IF Angulo_P > Angulo_3 AND Angulo_P < Angulo_4: y_dir = y_dir * -1; Dir_Regalo = 3; END //Pega abajo
               IF ((Angulo_P > Angulo_4 AND Angulo_P <= 359999) OR (Angulo_P >= 0 AND Angulo_P < Angulo_1)): x_dir = x_dir * -1; Dir_Regalo = 4; END //Pega derecha
               IF Angulo_P == Angulo_1 OR Angulo_P == Angulo_2 OR Angulo_P == Angulo_3 OR Angulo_P == Angulo_4:
                  x_dir = x_dir * -1;
                  y_dir = y_dir * -1;
               END
            END
         END
         IF ID_Barra = collision(TYPE BarraH):
            IF ID_Barra.graph == 17: acel_x = 60; acel_y = 60; play_wav(s_boing2, 0); ELSE acel_x = acel_xact; acel_y = acel_yact; play_wav(s_boing, 0); END
            x = x_old;
            y = y_old;
            IF y < 2400: y = (ID_Barra.y + (graphic_info(0, ID_Barra.graph, G_HEIGHT)/2) + (graphic_info(0, graph, G_HEIGHT)/2))*10; END
            IF y > 2400: y = (ID_Barra.y - (graphic_info(0, ID_Barra.graph, G_HEIGHT)/2) - (graphic_info(0, graph, G_HEIGHT)/2))*10; END
            y_dir = y_dir * -1;
            IF x > ID_Barra.x*10: x_dir += 2; IF x_dir == 0: x_dir = x_dir -= 2; END END
            IF x < ID_Barra.x*10: x_dir -= 2; IF x_dir == 0: x_dir = x_dir += 2; END END
         END
         IF ID_Barra = collision(TYPE BarraV):
            IF ID_Barra.graph == 17: acel_x = 60; acel_y = 60; play_wav(s_boing2, 0); ELSE acel_x = acel_xact; acel_y = acel_yact; play_wav(s_boing, 0); END
            x = x_old;
            y = y_old;
            IF x < 3200: x = (ID_Barra.x + (graphic_info(0, ID_Barra.graph, G_HEIGHT)/2) + (graphic_info(0, graph, G_HEIGHT)/2))*10; END
            IF x > 3200: x = (ID_Barra.x - (graphic_info(0, ID_Barra.graph, G_HEIGHT)/2) - (graphic_info(0, graph, G_HEIGHT)/2))*10; END
            x_dir = x_dir * -1;
            IF y > ID_Barra.y*10: y_dir += 2; IF y_dir == 0: y_dir = y_dir -= 2; END END
            IF y < ID_Barra.y*10: y_dir -= 2; IF y_dir == 0: y_dir = y_dir += 2; END END
         END

         // Condiciones para saber si la pelota se salio del rango permitido y por cual lado se salio
         IF y >= 3700 AND x > 0 AND x < 6400: //Se fue por abajo
            ID_BarraH[0].flags = 4;
            Se_FUE = 1;
         END
         IF x < 0 AND y > 0 AND y < 3700: //Se fue por la izquierda
            ID_BarraV[0].flags = 4;
            Se_FUE = 1;
         END
         IF y < 0 AND x > 0 AND x < 6400: //Se fue por arriba
            ID_BarraH[1].flags = 4;
            Se_FUE = 1;
         END
         IF x > 6400 AND y > 0 AND y < 3700: //Se fue por la derecha
            ID_BarraV[1].flags = 4;
            Se_FUE = 1;
         END
         // FIN de condiciones -------------------------------------------------------------------------

         FRAME(50);
     UNTIL Se_FUE OR out_region(ID, 0);
     play_wav(s_sefue, 0);
     ID_Pelota = Pelota(18, 3900, 100);
END


PROCESS BarraH(graph, x_iniH, y_iniH, PosH);
PRIVATE
       INT old_Graph;
BEGIN
     old_graph = graph;
     IF posH == 3: angle = 180000; END
     x = x_iniH;
     y = y_iniH;
     REPEAT
         IF flags == 4:
            Cant_Vidas--;
            signal(ID_Vidas[Cant_vidas], s_kill);
            REPEAT
               size -= 10;
               FRAME;
            UNTIL size < 0;
         ELSE
            IF demo:
               x = ID_Pelota.x/10;
            ELSE
               x = mouse.x;
            END
            IF mouse.left and Cant_Disparo < 5 AND graph == 14: Disparo(19, x, y, PosH); END
            IF x-(graphic_info(0, graph, G_WIDE)/2) < 20: x = 20+(graphic_info(0, graph, G_WIDE)/2); END
            IF x+(graphic_info(0, graph, G_WIDE)/2) > 620: x = 620-(graphic_info(0, graph, G_WIDE)/2); END

         END
         FRAME;
     UNTIL size < 0;
     IF PosH == 1:
        ID_BarraH[0] = BarraH(old_graph, x_iniH, y_iniH, 1);
     ELSE
        ID_BarraH[1] = BarraH(old_graph, x_iniH, y_iniH, 3);
     END
END

PROCESS BarraV(graph, x_iniV, y_iniV, PosV);
PRIVATE
       INT old_Graph;
BEGIN
     old_graph = graph;
     IF posV == 4: angle = 90000; ELSE angle = 270000; END
     x = x_iniV;
     y = y_iniV;
     REPEAT
         IF flags == 4:
            Cant_Vidas--;
            signal(ID_Vidas[Cant_vidas], s_kill);
            REPEAT
               size -= 10;
               FRAME;
            UNTIL size < 0;
         ELSE
            IF demo:
               y = ID_Pelota.y/10;
            ELSE
               y = mouse.y;
            END
            IF mouse.left and Cant_Disparo < 5 AND graph == 14: Disparo(19, x, y, PosV); END
            IF get_id(TYPE disparo) == 0: Cant_Disparo = 0; END
            IF y-(graphic_info(0, graph, G_WIDE)/2) < 20: y = 20+(graphic_info(0, graph, G_WIDE)/2); END
            IF y+(graphic_info(0, graph, G_WIDE)/2) > 370: y = 370-(graphic_info(0, graph, G_WIDE)/2); END
         END
         FRAME;
     UNTIL size < 0;
     IF PosV == 2:
        ID_BarraV[0] = BarraV(old_graph, x_iniV, y_iniV, 2);
     ELSE
        ID_BarraV[1] = BarraV(old_graph, x_iniV, y_iniV, 4);
     END
END

PROCESS disparo(graph, x, y, dir);
PRIVATE
       INT ID_Cubo;
BEGIN
     angle = father.angle;
     z = 2;
     play_wav(s_disparo, 0);
     Cant_Disparo++;
     REPEAT
           SWITCH(dir)
             CASE 1: y-=5; END
             CASE 2: x+=5; END
             CASE 3: y+=5; END
             CASE 4: x-=5; END
           END
           IF ID_Cubo = collision(TYPE cubo):
              IF ID_Cubo.flags <> 4:
                 Cant_Disparo--;
                 ID_Cubo.size = 99;
                 break;
              END
           END
           FRAME;
     UNTIL out_region(ID, 0) or y > 380;
END

PROCESS cubo(graph, x, y, regalo);
PRIVATE
       INT Salir;
BEGIN
     REPEAT
         IF size < 100:
            IF graph == 10:
               play_wav(s_cubito, 0);
               graph = 11;
               size = 100;
            ELSE
               IF graph <> 12:
                  play_wav(s_cubito, 0);
                  SWITCH(graph)
                    CASE  1: Puntos +=  10; END
                    CASE  2: Puntos +=  20; END
                    CASE  3: Puntos +=  30; END
                    CASE  4: Puntos +=  40; END
                    CASE  5: Puntos +=  50; END
                    CASE  6: Puntos +=  60; END
                    CASE  7: Puntos +=  70; END
                    CASE  8: Puntos +=  80; END
                    CASE  9: Puntos +=  90; END
                    CASE 11: Puntos += 100; END
                  END
                  SWITCH(Regalo)
                    CASE 20: Regalo(20); END
                    CASE 21: Regalo(21); END
                    CASE 22: Regalo(22); END
                    CASE 23: Regalo(23); END
                    CASE 24: Regalo(24); END
                  END
                  Quedan_Cubos--;
                  flags = 4;
                  Salir = 1;
                  REPEAT
                     size -= 10;
                     FRAME;
                  UNTIL size <= 0;
                 ELSE
                  play_wav(s_fierro, 0);
                  size = 100;
               END
            END
         END
         FRAME;
     UNTIL Salir;
END

PROCESS Regalo(graph)
PRIVATE Dir_X, Dir_Y;
BEGIN
     x = father.x;
     y = father.y;
     SWITCH(Dir_Regalo)
        CASE 1: Dir_X =  0; Dir_Y = -5; END // Dirección del regalo: Arriba
        CASE 2: Dir_X = -5; Dir_Y =  0; END // Dirección del regalo: Izquierda
        CASE 3: Dir_X =  0; Dir_Y =  5; END // Dirección del regalo: Abajo
        CASE 4: Dir_X =  5; Dir_Y =  0; END // Dirección del regalo: Derecha
     END
     REPEAT
        x+=Dir_X;
        y+=Dir_Y;
        IF ID_Barra = collision(TYPE BarraH):
           play_wav(s_regalo, 0);
           SWITCH(graph)
              CASE 20: ID_Barra.graph = 14; END
              CASE 21: ID_Barra.graph = 15; END
              CASE 22: ID_Barra.graph = 16; END
              CASE 23: ID_Barra.graph = 17; END
              CASE 24: acel_xact = 45; acel_yact = 45; acel_x = 45; acel_y = 45; END
           END
           BREAK;
        END
        IF ID_Barra = collision(TYPE BarraV):
           play_wav(s_regalo, 0);
           SWITCH(graph)
              CASE 20: ID_Barra.graph = 14; END
              CASE 21: ID_Barra.graph = 15; END
              CASE 22: ID_Barra.graph = 16; END
              CASE 23: ID_Barra.graph = 17; END
              CASE 24: acel_xact = 45; acel_yact = 45; acel_x = 45; acel_y = 45; END
           END
           BREAK;
        END
        FRAME;
     UNTIL (out_region(ID, 0) OR y > 370);
END

PROCESS Vidas(graph, x, y_temp);
PRIVATE var_y = 1;
BEGIN
     size = 40;
     angle = 270000;
     y = y_temp;
     LOOP
         y += var_y;
         If y > y_temp+10 or y < y_temp: var_y = var_y * -1; END
         FRAME;
     END
END

PROCESS Juego();
PRIVATE
       INT Regalo;
BEGIN
     Let_me_Alone();
     Solo_YO();
     play_song(M_Juego, -1);
     //mod_set(mod_volume, VolumenMODS);
     put_screen(0, 30);
     write(1, 10, 420, 0, "PUNTOS:");
     write_int(1, 10+text_width(1,"PUNTOS:"), 420, 0, &Puntos);
     write(1, 10, 440, 0, "NIVEL:");
     write_int(1, 10+text_width(1,"NIVEL:"), 440, 0, &Nivel);
     Puntos = 0;
     Nivel = 0;
     Cant_Vidas = 4;
     Quedan_Cubos = 0;
     LOOP
         IF Cant_Vidas < 0:
            stop_song();
            Menu();
         END
         IF Quedan_Cubos == 0:
            Let_me_Alone();
            Nivel++;
            IF Nivel > 10: Creditos(); END
            Niveles=fopen("niveles\" + itoa(Nivel) + ".dat", o_read);
            fread(Niveles, Grilla);
            Lugar=0;

            FROM y_temp = y_ini to y_fin step Alto_Cubo
                 FROM x_temp = x_ini to x_fin step Ancho_Cubo
                      IF Grilla[Lugar] <> 0:
                         regalo = rand(10, 30);
                         cubo(Grilla[Lugar], x_temp, y_temp, regalo);
                         Quedan_Cubos++;
                      END
                      Lugar++;
                 END
            END
            fclose(Niveles);
            ID_Pelota = Pelota(18, 3200, 3000);
            ID_BarraH[0] = BarraH(13,  10, 378, 1); // Barra de abajo
            ID_BarraV[0] = BarraV(13,  12,  10, 2); // Barra de la izquierda
            ID_BarraH[1] = BarraH(13,  10,  12, 3); // Barra de arriba
            ID_BarraV[1] = BarraV(13, 626,  10, 4); // Barra de la derecha
            IF Cant_Vidas >= 4: ID_Vidas[3] = Vidas(13, 574, 435); END
            FRAME(200);
            IF Cant_Vidas >= 3: ID_Vidas[2] = Vidas(13, 586, 435); END
            FRAME(200);
            IF Cant_Vidas >= 2: ID_Vidas[1] = Vidas(13, 598, 435); END
            FRAME(200);
            IF Cant_Vidas >= 1: ID_Vidas[0] = Vidas(13, 610, 435); END
         END
         FRAME;
     END
END

PROCESS Creditos();
PRIVATE i;
BEGIN
     Let_Me_Alone();
     Solo_YO();
     play_song(M_Creditos, 1);
     //mod_set(mod_volume, VolumenMODS);
     Salir();
     FRAME(500);
     LOOP
         letras_caen(1, "LIGHT SINC ENTERTAINMENT");
         WHILE mostrando_texto: FRAME; END
         letras_caen(1, "PROGRAMACION: GUILLERMO COLL");
         WHILE mostrando_texto: FRAME; END
         letras_caen(1, "GRAFICOS: GUILLERMO COLL");
         WHILE mostrando_texto: FRAME; END
         letras_caen(1, "EFECTOS DE SONIDO: DX-BALL 2");
         WHILE mostrando_texto: FRAME; END
         letras_caen(1, "MUSICA: DIVERSOS");
         WHILE mostrando_texto: FRAME; END
         letras_caen(1, "IDEA: GUILLERMO COLL");
         WHILE mostrando_texto: FRAME; END
         FRAME;
     END
END

PROCESS Salir() BEGIN LOOP IF key(_ESC): STOP_song(); menu(); END FRAME; END END

// Codigo del editor del juego ---------------------------------------------------------------------------------------
PROCESS Cubo_Editor(graph, x, y)
BEGIN

     LOOP
         IF collision(TYPE mouse) AND mouse.left:
            mouse.graph = graph;
         END
         FRAME;
     END
END

PROCESS Cubo_grilla(graph, x, y, N_Cubo)
BEGIN
     LOOP
         IF collision(TYPE mouse) AND mouse.left:
            Grilla[N_Cubo] = mouse.graph;
            graph = mouse.graph;
         END
         IF collision(TYPE mouse) AND mouse.right:
            Grilla[N_Cubo] = 0;
            graph = 40;
         END
         FRAME;
     END
END

PROCESS Editor();
PRIVATE
       INT N_Cubo;
BEGIN
     Let_me_Alone();
     Solo_YO();
     play_song(M_Juego, -1);
     //mod_set(mod_volume, VolumenMODS);
     Put_Screen(0, 30);
     mouse.graph = 1;
     FROM y_temp = y_ini to y_fin step Alto_Cubo
          FROM x_temp = x_ini to x_fin step Ancho_Cubo
              Cubo_Grilla(40,x_temp,y_temp,N_Cubo);
              N_Cubo++;
          END
     END
     x_temp = 30;
     FROM temp = 1 TO 12
          Cubo_Editor(temp, x_temp, 440);
          x_temp += (Ancho_cubo+2);
     END
     write(0, 20, 20, 0, "EDITOR v1.0");
     write(0, 20, 40, 0, "Para grabar presione F2, el mismo se grabara en el directorio \niveles con el nombre temp.dat");
     LOOP
         IF key(_ESC): Stop_song(); Menu(); END
         IF key(_F2):
            Niveles=fopen("niveles\temp.dat",o_write); // Abre archivo para lectura
            fwrite(Niveles, Grilla);
            fclose(Niveles);
         END
        FRAME(200);
     END
END
// FIN del codigo del editor del juego -------------------------------------------------------------------------------


// PROCEDIMIENTOS UTILES ---------------------------------------------------------------------------------------------
Process Solo_YO();
BEGIN
     clear_screen();
     put_screen(0, 0);
     delete_text(0);
     mouse.graph = 0;
END

PROCESS letras_caen(int fuente, string texto)
PRIVATE string letras[50] = "";
        INT i, y_old, fy_old;
        STRUCT ID_letras[50]
               INT ID_Letra;
               INT x_Letra;
        END

BEGIN
     mostrando_texto = 1;
     x = rand (100, 305);
     y = rand (200, 300);
     y_old = y;
    FROM i = 0 to len(texto)
      letras[i] = substr(texto, i, i);
      x = x + text_width(fuente, letras[i]);
      ID_letras[i].ID_Letra = write(fuente, x, y+500, 4, letras[i]);
      ID_letras[i].x_Letra = x;
    END
    FROM i = 0 to len(texto)
       move_text(ID_letras[i].ID_Letra, ID_letras[i].x_Letra, y);
       play_wav(s_tecla, 0);
       FRAME(500);
    END
    FRAME(3000);
    FROM i = len(texto) to 0 step -1
       y = y_old;
       REPEAT
         fy_old = y;
         y+=50;
         move_text(ID_letras[i].ID_Letra, ID_letras[i].x_Letra, y);
         FRAME;
         IF fy_old <> y: letraGhost(fuente, letras[i], ID_letras[i].x_Letra, fy_old); END
       UNTIL y >= 490;
    END
    FRAME(2000);
    mostrando_texto = 0;
END

PROCESS letraGhost(int fuente, string texto, int x, y)
BEGIN
     graph = write_in_map(fuente, texto, 4);
     flags = 4;
     FROM size = 100 TO 0 STEP -10
          FRAME;
     END
     unload_map(0, graph);
END


