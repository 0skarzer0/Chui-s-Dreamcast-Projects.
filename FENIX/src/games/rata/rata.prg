// Versi¢n libre del juego de la rata y las serpientes
// Ejemplo 1 de inteligencia artifical
// Para que la animaci¢n tuviese una velocidad adecuada, se triplican los frames


PROGRAM rata ;

CONST

  MAX_VIDAS = 3 ;             // Vidas m ximas
  MAX_SNAKE = 5 ;             // M ximo numero de serpientes
  SCORE_PASO = 50 ;           // Puntos por cada movimiento
  BONUS_TIME = 300 ;          // Tiempo que tarda en aparecer un bonus
  SNAKE_TIME = 1000 ;         // Tiempo que tarda en crearse una serpiente
  WARP_TIME = 200 ;           // Tiempo de carga del warp
  MINX = 20 ;                 // L¡mites del area de juego
  MAXX = 229 ;
  MINY = 64 ;
  MAXY = 375 ;
  SNAKE_VEL = 15 ;
  MENU = 0 ;                  // Estados del programa
  JUEGO = 1 ;
  OVER = 2 ;
  SAL = 3 ;

GLOBAL

  int xcoor[7] = 20,55,90,125,160,195,230 ;                 // centros x de casilla
  int ycoor[10] = 62,97,132,167,202,237,272,307,342,377 ;   // centros y de casilla
  int fpgfile ;                                             // fichero gr fico
  int salida ;                                              // flan final de partida
  int vidas ;                                               // vidas actuales
  int score ;                                               // puntuaci¢n actual
  int warp = 1 ;                                            // warp activo o no
  int vidid[3] ;                                            // id de los procesos que marcan vidas
  byte keyp ;                                               // flag tecla pulsada
  int warpt = 0 ;                                           // conteo del warp
  int snaket = 0 ;                                          // conteo de la serpiente
  int bonust = 0 ;                                          // conteo del bonus
  int font ;                                                // fuente
  int scoreid ;                                             // id del texto puntos
  int move ;                                                // movimiento actual
  int ratx ;                                                // X de la rata... se puede usar el id.X en su lugar
  int raty ;                                                // Y de la rata... igual que la X
  int ratID ;                                               // proceso de la rata
  int serp[5] ;                                             // procesos de las serpientes
  byte serpientes = 0 ;                                     // serpientes activas
  byte bonustr = 1 ;                                        // bonus timer ready
  int warpid ;                                              // texto del warp
  byte estado = 0 ;                                         // Estado del programa
  byte salir = 0 ;                                          // Salida del juego
  int scoretid ;                                            // id del texto del score
  int warps ;                                               // sonido del warp
  int sonido;
BEGIN

  SOUND_MODE=1;
  SOUND_FREQ=44100;
  FULL_SCREEN=false;
  salida = 0 ;
  graph_mode = mode_16bits ;
  fpgfile = load_fpg("rata\rata.fpg") ;
  set_title("El ratón y las serpientes") ;
  set_icon(fpgfile,100) ; 
  set_mode(m320x400) ;
  set_fps(24,3) ;
  frame;

  warps=load_wav("warp.wav") ;
  font = load_fnt("rata\rata.fnt") ;

  frame;


  // Bucle principal del juego

  REPEAT

  switch (estado)

    CASE MENU:

         if (scoretid) delete_text(scoreid) ; END
         clear_screen() ;
         put_screen(0,2) ;

         REPEAT

           FRAME ;

           if (key(_enter)) estado = JUEGO ; END
           if (key(_esc)) estado = SAL ; END

         UNTIL (estado <> MENU)
         END


    CASE JUEGO:

         // inicializar todas las variables

         serpientes = 0;
         warp = 1 ;
         snaket = 0 ;
         bonust = 0 ;
         bonustr = 1 ;
         warpt = 0 ;
         salir = 0 ;
         score = 0 ;
         vidas = MAX_VIDAS ;
         warpid = 0 ;

         clear_screen() ;
         put_screen(0,1) ;

         // Creaci¢n de procesos

         vidid[0] = vida (267,112) ;
         vidid[1] = vida (283,112) ;
         vidid[2] = vida (299,112) ;
         ratID = rat (125,237) ;
         serp[serpientes] = serpiente(xcoor[6],ycoor[0],serpientes) ;
         serpientes ++ ;

         // Bucle principal de juego

         while (salir <> 1)

         // Generaci¢n de puntuaci¢n

	 IF (scoretid != 0) delete_text(scoretid); END
         scoretid = write (font,305,182,5,substr("00000"+score,-5,-1)) ;
         IF (warp && warpid == 0) warpid = write (font,307,252,5,"WARP!") ; END

         // Simulaci¢n de timers

         IF (snaket < SNAKE_TIME) snaket++ ;   // Controla la aparici¢n de serpientes
         ELSE
           snaket=0 ;
           if (serpientes < MAX_SNAKE)
             serp[serpientes] = serpiente(xcoor[6],ycoor[0],serpientes) ;
             serpientes++ ;
           END
         END

         IF (bonust == BONUS_TIME)             // Controla la aparici¢n de bonus
           IF (bonustr==1)
             bonust=0 ;
             bonustr = 0 ;
             bonus(xcoor[rand(0,6)],ycoor[rand(0,9)],40+rand(0,7)) ;
           END
         ELSE
           if (bonustr==1)
             bonust++ ;
           END
         END

         IF (warp==0)                          // Controla si tenemos Warps
           IF (warpt == WARP_TIME)
             warp = 1 ;
           ELSE
             warpt++ ;
           END
         END

         // gest¢n de teclado

         keyp = 0;         // no hay teclas pulsadas

         if (key(_left))   // cursor izqda
           keyp = 1 ;
         END

         if (key(_right))  // cursor derecha
           keyp = 2 ;
         END

         if (key(_up))     // cursor arriba
           keyp = 3 ;
         END

         if (key(_down))     // cursor abajo
           keyp = 4 ;
         END

         if (key(_space) AND warp)    // warp
           if (move==0)
             play_wav(warps,0) ;
             signal(ratid,s_kill) ;
             ratid = rat(xcoor[rand(0,6)],ycoor[rand(0,9)]) ;  // nueva rata aleatoria
             delete_text(warpid) ;
	     warpid = 0 ;
             warp = 0 ;
             warpt = 0 ;
           END
         END

         if (key(_esc))     // abandonar juego
           salir=1 ;
           delete_text(scoreid) ;   // borrado de textos
           delete_text(warpid) ;  // borrado de textos
           let_me_alone() ;  // desetruye los procesos
           clear_screen() ;  // borra pantalla
           estado = MENU ;
         END

         if (vidas == 0 )
           salir = 1 ;
           estado = OVER ;
         END

         // actualizaci¢n de pantalla

         FRAME ;
         END

         if (estado == OVER)
           delete_text(scoreid) ;
           if (warpid) delete_text(warpid) ; END
           let_me_alone() ;
           clear_screen() ;
           put_screen(0,3) ;
           FROM snaket=0 TO 50 ;
             FRAME ;
           END
           salida = 0 ;
           estado = MENU ;
         end
         END

    CASE SAL:

         salida=1 ;
         END

  END
  UNTIL (salida == 1) ;

  let_me_alone() ;    // por si nos ha quedado de alg£n sitio un proceso activo
  clear_screen() ;

END


PROCESS vida(x,y)

// Proceso que pinta los indicadores de las vidas

PRIVATE

  BYTE cont ;
  int graphs[12] = 10,10,10,11,11,11,12,12,12,11,11,11 ;
  byte cgraph = 0 ;

BEGIN
  graph = graphs[cgraph] ;
  loop
      if (cgraph < 11) cgraph++ ;
      else cgraph = 0 ;
      END
      graph = graphs[cgraph] ;
    FRAME;
  END

END

PROCESS rat(x,y)

// Proceso de la rata. Personaje controlado por el jugador

PRIVATE

  BYTE cont ;
  int qgraphs[12] = 15,15,15,16,16,16,17,17,17,16,16,16 ;
  int igraphs[12] = 20,20,20,21,21,21,20,20,20,22,22,22 ;
  int dgraphs[12] = 25,25,25,26,26,26,25,25,25,27,27,27 ;
  byte cgraph = 0 ;
  byte oldk = 0 ;
  byte endmove = 1 ;
  byte inc = 0;
  int serpid ;

BEGIN
  graph = qgraphs[cgraph] ;
  cont = 0 ;
  z=0 ;

  REPEAT

    if (keyp <> 0 AND endmove <> 0)
      oldk = keyp ;
      cgraph=0 ;
      inc = 0 ;
      endmove = 0 ;
      switch (keyp)
        case 0:  graph = qgraphs[cgraph] ;
                 move = 0 ;
                 END
        case 1:  graph = igraphs[cgraph] ;
                 move = 1 ;
                 END
        case 2:  graph = dgraphs[cgraph] ;
                 move = 2 ;
                 END
        case 3:  graph = qgraphs[cgraph] ;
                 move = 3 ;
                 END
        case 4:  graph = qgraphs[cgraph] ;
                 move = 4 ;
                 END
      END
    END
    if (cgraph < 11) cgraph++ ;
    else cgraph = 0 ;
    END
    SWITCH (move)
      case 0: graph = qgraphs[cgraph] ;
              endmove = 1 ;
              END ;
      case 1: graph = igraphs[cgraph] ;
                if (x >= MINX)
                  x-=2 ;
                  inc+=2 ;
                  if (inc>32)
                    move = 0 ;
                    endmove = 1 ;
                    score+=SCORE_PASO ;
                    x-=1 ;
                  END
                ELSE
                  endmove = 1 ;
                END
              END
      case 2: graph = dgraphs[cgraph] ;
                if (x =< MAXX)
                  x+=2 ;
                  inc+=2 ;
                  if (inc>32)
                    move = 0 ;
                    endmove = 1 ;
                    score+=SCORE_PASO ;
                    x+=1 ;
                  END
                ELSE
                  endmove = 1 ;
                END
              END
      case 3: graph = qgraphs[cgraph] ;
                if (y >= MINY)
                  y-=2 ;
                  inc+=2 ;
                  if (inc>32)
                    move = 0 ;
                    endmove = 1 ;
                    score+=SCORE_PASO ;
                    y-=1 ;
                  END
                ELSE
                  endmove = 1 ;
                END
              END
      case 4: graph = qgraphs[cgraph] ;
                if (y =< MAXY)
                  y+=2 ;
                  inc+=2 ;
                  if (inc>32)
                    move = 0 ;
                    endmove = 1 ;
                    score+=SCORE_PASO ;
                    y+=1 ;
                  END
                ELSE
                  endmove = 1 ;
                END
              END
    END
    ratx = x ;
    raty = y ;
    FRAME;

  UNTIL (collision(TYPE serpiente)) ;

  FROM graph=50 TO 55 ;
    FRAME :
  END

  warp = 1 ;
  move = 0 ;
  ratid = rat(xcoor[rand(0,5)],ycoor[rand(1,9)]) ;

END

PROCESS serpiente(x,y,ident)

// Proceso que controla las serpientes

PRIVATE

  BYTE cont ;
  int  qgraphs[12] = 30,30,30,31,31,31,30,30,30,31,31,31 ;
  int  igraphs[12] = 32,32,32,33,33,33,32,32,32,33,33,33 ;
  int  dgraphs[12] = 34,34,34,35,35,35,34,34,34,35,35,35 ;
  byte cgraph = 0 ;
  byte dir = 0 ;    // 0 = x  1 = y
  byte mov ;
  byte endmov = 1 ;
  byte inc = 0 ;
  byte idenum ;


BEGIN

  graph = qgraphs[cgraph] ;
  cont = 0;
  z=0 ;
  idenum = ident ;

  REPEAT
      if (cont <= SNAKE_VEL ) cont++ ;
      else
        if (endmov<>0)
          cont = 0 ;
          inc=0 ;
          IF (rand(0,1) < 1) dir = 0 ;
          ELSE dir = 1 ;
          END
        SWITCH (dir)
        CASE 0: IF (ratx < x) mov = 1 ;
                ELSE mov = 2 ;
                END
                END
        CASE 1: IF (raty < y) mov = 3 ;
                ELSE mov = 4 ;
                END
                END
        default: mov = 0 ;
                END
        END
        END
      END

      if (cgraph < 11) cgraph++ ;
      else cgraph = 0 ;
      END
      SWITCH (mov)
        case 0: graph = qgraphs[cgraph] ;
                endmov = 1 ;
                END ;
        case 1: graph = igraphs[cgraph] ;
                if (x >= MINX)
                  x-=2 ;
                  inc+=2 ;
                  if (inc>32)
                    mov = 0 ;
                    endmov = 1 ;
                    x-=1 ;
                  END
                ELSE
                  endmov = 1 ;
                END
              END
        case 2: graph = dgraphs[cgraph] ;
                if (x =< MAXX)
                  x+=2 ;
                  inc+=2 ;
                  if (inc>32)
                    mov = 0 ;
                    endmov = 1 ;
                    x+=1 ;
                  END
                ELSE
                  endmov = 1 ;
                END
              END
        case 3: graph = qgraphs[cgraph] ;
                if (y >= MINY)
                  y-=2 ;
                  inc+=2 ;
                  if (inc>32)
                    mov = 0 ;
                    endmov = 1 ;
                    y-=1 ;
                  END
                ELSE
                  endmov = 1 ;
                END
              END
        case 4: graph = qgraphs[cgraph] ;
                if (y =< MAXY)
                  y+=2 ;
                  inc+=2 ;
                  if (inc>32)
                    mov = 0 ;
                    endmov = 1 ;
                    y+=1 ;
                  END
                ELSE
                  endmov = 1 ;
                END
              END
    END
    FRAME;

    UNTIL (collision(TYPE rat))

     vidas-- ;
     signal(vidid[vidas],s_kill) ;

     FROM graph=60 TO 65 ;
       FRAME :
     END

     signal(serp[idenum],s_kill) ;
     serp[idenum] = serpiente(xcoor[6],ycoor[0],idenum) ;
     snaket = 0 ;

END

PROCESS BONUS(x,y,graph)

PRIVATE

  int timeup = 150 ;
  int colid ;
  int graphs[12] = 5,5,5,6,6,6,5,5,5,7,7,7 ;
  byte cont ;
  byte cgraph ;

BEGIN

  z=0 ;

  REPEAT

    timeup-- ;
    colid = collision (TYPE rat) ;
    FRAME ;

  UNTIL (timeup == 0 OR colid<> 0) ;

  if (colid <> 0)
    z=-10 ;
    timeup = 50 ;
    score += 500 ;
    graph = graphs[cgraph] ;
    REPEAT
      timeup-- ;
      if (cgraph < 11) cgraph++ ;
      else cgraph = 0 ;
      END
      graph = graphs[cgraph] ;
      FRAME;
    UNTIL (timeup == 0)


  END

  bonustr = 1 ;

END

