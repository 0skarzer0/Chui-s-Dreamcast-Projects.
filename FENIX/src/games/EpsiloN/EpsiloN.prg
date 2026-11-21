// *********************************************************************************************************
// *		NOMBRE: 	Epsilon.											*
// *		AUTOR:	Papada Soft.											*
// *		FECHA:	7 de Febrero de 2002.									*
// *		CONTACTO:	papadasoft@mixmail.com									*
// *		TIPO: 	Matamarcianos de corte clasico con algunos añadidos como editor de niveles		*
// *				y posibilidad de construir nuevos mundos, tres naves a elegir y una amplia		*
// * 				variedad de marcianos. Este juego es FREEWARE.						*
// *		MAS...:	Juego de ejemplo programado con FENIX - Http://fenix.divsite.net			*
// *				Ultima revision 27/06/2002 - Adaptacion a Fenix 08.					*
// *********************************************************************************************************
PROGRAM EpsiloN;
LOCAL num, disparando;
GLOBAL 

int idfpg, idenemigos, idnave, idfont, disparos, nEnemigos, muerto, idpeq, idseleccion,naveElejida=1,
puntos, fase=1, p, a, d, velocidad=1, auxiliar, fichero, idmenu, truko1, Opc, idraton,Reinicia,
veneno, tapon,idexplo2, codigo_id,energia=5, idenergia,aceleracion,pulsada, proceso,idblendop,
idmusica,modotruko,linea,textos,idnavesmal,id_textu,zalazar,power, m_volumen, e_volumen,cuanto;
int idsexplosion1, idsexplosion2, idslaser, idsitem, idsdisparo, idsbomba;
int Fases[113];

STRING TxT[45];

STRUCT Caracteristicas[2]
	STRING Texto;
END

STRUCT Naves[2]
	int resveneno,
	int restapon,
	int vida,
	int cadisparo,
	int acelera,
	int max_vida,
END=
5,5,4,5,2,5,
100,0,3,3,2,4,
0,100,3,3,3,3;

BEGIN
GRAPH_MODE=MODE_16BITS;	// Activamos el modo de 16Bits.
set_mode(m320x200);			// Resolucion que usa el juego, en principio la linea puede no incluirse pero mejor ponerla.
set_fps(60,0);				// 60 imagenes por segundo seran suficientes para darle velocidad al juego.
mouse.file=idmenu;
Caracteristicas[0].Texto="Muy resistente, gran cadencia de disparo.";
Caracteristicas[1].Texto="Inmune a paralisis, cadencia de disparo normal.";
Caracteristicas[2].Texto="Inmune a recalentamiento, muy rapida.";
idfont=Load_fnt("scores.fnt");
graph=Load_map("Logo.map"); 	// Asignamos el grafico de intro...
size=100;
x=160;
y=100;
WHILE (Scan_code==0)
    FRAME;					// ... lo visualizamos mientras no se pulse una tecla.
END
FROM size=100 to 0 Step -4
	FRAME;				// Y ahora hacemos que se pierda en el centro de la pantalla.
END
FRAME;

// Asignamos los fpgs a unas variables y los cargamos para luego poder manipularlos.

idnavesmal=Load_fpg("navesmal.fpg");
idexplo2=load_fpg("explo2.fpg");
idmenu=load_fpg("menu.fpg");
idpeq=Load_fnt("nuevofnt.fnt");
idfpg=Load_fpg("MataMata.fpg");
idSeleccion=Load_fpg("navessel.fpg");
idEnemigos=Load_fpg("enemigos.fpg");
idenergia=load_fpg("energia.fpg");

// Ahora hacemos lo mismo pero esta vez con los sonidos.

idsexplosion1=load_wav("efectos/explosion.wav");
idsdisparo=load_wav("efectos/disparonave.wav");
idslaser=load_wav("efectos/laser.wav");
idsitem=load_wav("efectos/item.wav");
idsbomba=load_wav("efectos/bomba.wav");
idsexplosion2=load_wav("efectos/explo2.wav");

// Configuracion.

fichero=fopen("config/config.dat",o_read);			// Intentamos abrir el fichero de consfiguracion.
IF(fichero<>0);
	cuanto=fgets(fichero);						// Si lo conseguimos pues cargamos los datos
	m_volumen=atoi(cuanto);					// en las variables correspondientes.
	cuanto=fgets(fichero);
	e_volumen=atoi(cuanto);
ELSE
	fichero=fopen("config/config.dat",o_write);	// En caso contrario lo creamos...
	fputs(fichero,"50");						// y grabamos en el los valores por defecto.
	fputs(fichero,"50");
	m_volumen=50;
	e_volumen=50;
END
fclose(fichero);								// Una vez realizado lo cerramos.


WHILE(Reinicia==0)
	set_fps(60,0);							// Ajustamos los fps ya que en el Game Over se modificaron.
	FRAME;
	
	// Inicializamos los valores iniciales de las variables.
	
	energia=5;
	nEnemigos=puntos=0;
	fase=1;
	power=1;
	disparos=0;
	let_me_alone();		// Matamos todos los procesos activos excepto este.
	Stop_song();		// Paramos la musica...
	idmusica=Load_song("musica/creditos.xm");
	play_song(idmusica, -1);	// ... y hacemos que suene el modulo correspondiente.
	delete_text(all_text);
	mouse.file=idmenu;
	mouse.graph=0;
	p=d=a=muerto=0;
	delete_text(all_text);
	put_screen(idmenu,1);		// Colocamos la pantalla de menu
	Raton();				// y activamos el raton.
	FRAME;				// lo hacemos visible y
	Titulo();				// colocamos el titulo y
	Subtitulo();			// el subtitulo con su correspondiente scroll lateral.
	Opcion(130,215,1);		// Visualizamos las opciones.
	Opcion(172,239,2);
	Opcion(173,265,3);
	Opcion(146,285,4);
	scan_code=0;			// Liberamos el teclado indicando que ninguna tecla fue pulsada en el ultimo FRAME;
	Opc=0;				// Ninguna opcion se ha elegido aun...
	WHILE(Opc==0);			// Mientras que no se haya seleccionado alguno opcion nos mantendremos tal cual.
		IF(key(_esc))
			exit("",0);			// Si el jugador pulsa ESCAPE entonces salimos del juego.
		END
		
		// Estas lineas nos permitiran seleccionar las opciones por teclado.
		
		IF(key(_1)) Opc=1; END
		IF(key(_2)) Opc=2; END
		IF(key(_3)) Opc=3; END
		IF(key(_4)) Opc=4; END
		
		IF(rand(0,100)==1)			// Hacemos 1% de que un marcianito pase por el fondo de la pantalla... 
			bicho(rand(0,200));		// si se cumple lo mostramos, este es el modo truko ;).
		END
    	FRAME;
	END
	Let_me_alone();					// Como se elijio una opcion pues matamos a todos los procesos que
	Clear_screen();					// estubiesen activos y borramos el fondo de la pantalla.

	IF(Opc==1)					// El jugador elijio la opcion de JUGAR.
		graph=0;
		delete_text(all_text);
		put_screen(idSeleccion,13);		// Ponemos el grafico de seleccion de nave...
		Load_pal("scores.fnt");
		Nave1Sel();				// ... y las naves claro.
		Nave2Sel();
		Nave3Sel();
		Control_Sel();				// Activamos el proceso que controlara la seleccion de las naves.
		WHILE(!key(_enter))
			FRAME;
		END
		z=-4;
		let_me_alone();
		clear_screen();
		Stop_song();
		idmusica=Load_song("musica/mundo0.s3m");	// Una vez elejida la nave colocamos la nueva musica...
		play_song(idmusica, -1);
		energia=naves[naveelejida-1].vida;			// ... y asignamos valores de inicio.
		region=0;
		x=160;
		y=60;
		Delete_text(all_text);
		put_screen(0,Load_png("graficos/mundo0-1.png"));	// Colocamos la primera imagen del mundo 0
		fichero=fopen("textos/mundo0.txt", o_read);		// y mostramos la frase...
		TxT=fgets(fichero);
		TxT=Substr(TxT,0,-2);
		write(0,160,174,4,TxT);
		scan_code=0;
		WHILE(scan_code==0)
			FRAME;
		END
		delete_text(all_text);
		put_screen(0,Load_png("graficos/mundo0-2.png"));	// una vez pulsada una tecla hacemos lo propio
		TxT=fgets(fichero);							// con el segundo grafico...
		TxT=Substr(TxT,0,-2);
		write(0,160,174,4,TxT);						// y la segunda linea de texto.
		fclose(fichero);
		scan_code=0;
		WHILE(scan_code==0)
			FRAME;
		END
		delete_text(all_text);
		FRAME;
		put_screen(0, load_png("fondos/mundo0.png"));		// Comineza el juego, colocamos el grafico de
		IF(modotruko==1)							// fondo y si el modo truko esta activado
			Energia_infinita();						// creamos el proceso que nos dara vida infinita.
		END
		
		// Creamos los procesos necesarios para comenzar el juego...
		Marcador();
		Energia();
		nave();
		
		Texto(160,100,400, "Nivel "+itoa(fase));			// Mostramos la fase en la que nos encontramos.
		CargaFase();								// la cargamos
		WHILE(exists(TYPE CargaFase))
			FRAME;								// y hacemos una pausa hasta que los valores de
		END										// dicha fase esten confirmados.
		CreaFase(fase);								// entonces la mostramos en pantalla.
		fade_on();
		
		// Entramos en el bucle que controlara la partida...
		LOOP
    		IF(nEnemigos==0)							// Si hemos matado a todos los enemigos...
        		fase++;								// aumentamos la fase
			IF((fase-1) MOD 10==0 & fase <> 1)			// y comprobamos si llegamos al final de un mundo
				let_me_alone();						// en caso afirmativo destruimos todo
				CargaFase();						// cargamos la fase
				FRAME;
				Timer[5]=0;
				Stop_song();
				Delete_text(all_text);
				WHILE(Timer[5]<100)
					FRAME;						// hacemos una pequeña pausa
				END
				idmusica=Load_song("musica/mundo"+itoa(fase/10)+".s3m");			// y comenzamos con el 
				play_song(idmusica, -1);										// nuevo mundo
				put_screen(0,Load_png("graficos/mundo"+itoa((fase/10))+"-1.png"));
				fichero=fopen("textos/mundo"+itoa(fase/10)+".txt", o_read);
				TxT=fgets(fichero);
				TxT=Substr(TxT,0,-2);
				write(0,160,174,4,TxT);
				scan_code=0;
				WHILE(scan_code==0)
					FRAME;
				END
				delete_text(all_text);
				put_screen(0,Load_png("graficos/mundo"+itoa(fase/10)+"-2.png"));
				TxT=fgets(fichero);
				TxT=Substr(TxT,0,-2);
				write(0,160,174,4,TxT);
				fclose(fichero);
				scan_code=0;
				WHILE(scan_code==0)
					FRAME;
				END
				delete_text(all_text);
				p=d=a=muerto=0;							// Ponemos los bonus a 0.
				put_screen(0,Load_png("fondos/mundo"+itoa(fase/10)+".png"));
			END
			
			// Matamos los procesos que hubiesen podido quedar vivos.
			WHILE(auxiliar=GET_id(TYPE Disparo))
				signal(auxiliar,s_kill);							
			END
			auxiliar=Get_id(TYPE Marcador);
			signal(auxiliar,s_kill);
			auxiliar=Get_id(TYPE Nave);
			signal(auxiliar,s_kill);
			WHILE(auxiliar=get_id(TYPE DisparoTipo1))
				signal(auxiliar, s_kill);
			END
			
			// y nuevamente cremoas lo necesario para comenzar la fase.
			Disparos=0;	
			Marcador();
			Energia();
			nave();
			IF(modotruko==1)
				Energia_infinita();
			END
        		Texto(320/2,240/2,500,"Entrando en fase "+itoa(fase));	// Mostramos la fase...
			CargaFase();
        		CreaFase(Fase);
        		WHILE(Get_id(TYPE Texto))
            			FRAME;									// y a matar marcianos de nuevo.
        		END
    		END
    		IF(muerto==1)
			energia--;
			muerto=0;
			FRAME;
		END
    		IF(energia<1)							// Si nuestra energia esta a cero y a menos...
			Set_fps(15,0);						// Reducimos la velocidad del juego para hacer un
											// pequeño efecto...
        		Texto(320/2,240/2,500,"Game Over.");
        		signal(idnave, s_kill);
        		WHILE(Get_id(TYPE Texto))				// y ponemos el texto en pantalla...
            			FRAME;
        		END
        		let_me_Alone();						// Paramos todo y ponemos
			Stop_song();
			Delete_text(all_text);
			idmusica=Load_song("musica/game over.s3m");		// la musica y
			play_song(idmusica, -1);
			put_screen(0,Load_png("graficos/Game_Over1.png"));	// los graficos correspondientes al
			fichero=fopen("textos/Game_over.txt", o_read);		// Game Over.
			TxT=fgets(fichero);
			TxT=Substr(TxT,0,-2);
			write(0,160,174,4,TxT);
			scan_code=0;
			WHILE(scan_code==0)
				FRAME;
			END
			delete_text(all_text);
			put_screen(0,Load_png("graficos/Game_Over2.png"));
			TxT=fgets(fichero);
			TxT=Substr(TxT,0,-2);
			write(0,160,174,4,TxT);
			fclose(fichero);
			scan_code=0;
			WHILE(scan_code==0)
				FRAME;
			END
			delete_text(all_text);
        		BREAK;									// y salimos del bucle de control
													// para regresar al menu.
    		END
    		IF(p==2 & a ==3 & d==1)						// Si hemos conseguido forma PAPADA
    			Texto2(320/2,240/2,200,"Bonus!!!");			// informamos del bonus...
			p=0; a=0; d=0;							// lo ponemos a 0 para la proxima vez
			WHILE(auxiliar=Get_id(TYPE Item))
				signal(auxiliar,s_kill);					// borramos las letras de la pantalla
			END
			Zalazar=Rand(0,100);						// y aleatoriamente vemos que bonus hemos
												// obtenido.
			IF(Zalazar<50)							// 50% de que sea avanzar de fase
				WHILE(auxiliar=get_id(TYPE CreaEnemigo))	// destruyendo antes todos los marcianos
					Explosion(auxiliar.x,auxiliar.y);		// que aun sobreviviesen.
					signal(auxiliar,s_kill);
				END
				nEnemigos=0;						// ahora ya no hay enemigos en pantalla.
			ELSE
				IF(Zalazar<75)						// sino... 25% de que
					energia=Naves[NaveElejida-1].Vida;	// el bonus sea recarga de energia.
				ELSE
					power=2;						// y sino... pues solo nos queda el otro 25%
				END								// que es un aumento del poder de disparo...
			END									// ahora cada disparo eliminara 2 marcianos.
    		END
		IF(key(_esc))								// Si el jugador pulso ESCAPE salimos de la
			Let_me_alone();							// partida, eliminando los procesos.
			FRAME;
			BREAK;								// salimos...
		END
    		FRAME;
		END
	END

	// Parece que el jugador ha preferido toquetear o crear nuevos niveles.
	IF(Opc==2)		
		put_screen(idfpg, 10);  // Colocamos el grafico del fondo.
		Creatiles();				// Creamos los tiles uno por cada hueco para el marciano.
		
		// y mostramos la informacion sobre teclas, nivel que estamos editando y demas...
		mouse.file=idenemigos;
		mouse.graph=1;
		load_pal("scores.fnt");
		write(0,10,135,0,"Nivel");
		write_int(0,46,135,0,&fase);
		write(0,10,158,0,"Cursor Dcho e Izqdo para cambio de marciano.");
		write(0,10,166,0,"Cursor arriba y abajo + - nivel. Enter salva.");
		write(0,10,174,0,"C borra pantalla. L carga nivel. Espace sale.");
		write(0,10,182,0,"Boton drcho cambia marciano izqdo o mayuscula");
		write(0,10,191,0,"colocan marciano. F1 ayuda.");
		WHILE(Opc==2)	// Entramos en el bucle de la edicion del nivel.
			IF((mouse.right | key(_right)) & Timer[4]>15)		// Si se pulso el boton derecho del raton
				mouse.graph+=2;					// cambiamos el marciano seleccionado.
				FRAME;
				Timer[4]=0;						// e inicializamos el timer para que si
			END									// se deja pulsado no haga el cambio bruscamente.
			IF(key(_left) & Timer[4]>15)				// Lo mismo pero en la direccion contraria...
				mouse.graph-=2;
				FRAME;
				Timer[4]=0;
			END
			
			// Estos IFs comprueban los limites para retornar al marciano mas proximo por ambos lados, es decir si hemos acabado 
			// con el marciano mas a la derecha pues recomenzamos mostrando el primero de la izquierda de nuevo.
			IF(mouse.graph==47)
				mouse.graph=100;
			END
			IF(mouse.graph==102)
				mouse.graph=1;
			END
			IF(mouse.graph<=0)
				mouse.graph=100;
			END
			IF(mouse.graph==98)
				mouse.graph=45;
			END
			
			// El jugador pulso la tecla L eso significa que quiere cargar la fase actual para modificarla...
			IF(key(_l))
				CargaFase();		// Pues la cargamos...
				WHILE (proceso=get_id(TYPE CreaTile))	// y mostramos cada marciano en su tile
					num=Proceso.num;				// correspondiente
					Proceso.graph=fases[num];
					IF(Proceso.graph==0)
						Proceso.graph=100;			// los huecos vacios los reyenamos con tiles...
					END
				END
			END
			// El juador pulso C quiere borrar los tiles y dejarlos todos vacios...
			IF(key(_c))		
				WHILE (proceso=get_id(TYPE CreaTile))	// cogemos todas las ids de los tiles...
					num=Proceso.num;
					Proceso.graph=100;				// y les asignamos el grafico de tile vacio
					fases[num]=0;					// hacemos lo propio con su dato en la estructura de la fase.
				END
			END
			// El jugador pulso CURSOR ARRIBA quiere aumentar el numero de nivel a modificar.
			IF(key(_up) & timer[5]>15)
				fase++;			// pues hacemos lo propio...
				timer[5]=0;
			END
			// Ahora quiere disminuirlo con CURSOR ABAJO.
			IF(key(_down) & Timer[5]>15)
				fase--;			// pues lo disminuimos.
				timer[5]=0;
			END
			IF(fase==0)			// La fase 0 no existe siempre la primera ha de ser
				fase=1;			// la 1.
			END
			// El jugador ha pulsado ENTER haciendonos ver que quiere salvar "su" nivel.
			IF(key(_enter))
				SalvaFase();		// Pues los salvamos pasando los valores de cada tile a la estructura.
				FRAME;
			END
			// El jugador quiere mostrar la ayuda con F1.
			IF(key(_f1))
				IF(Get_id(TYPE Ayuda)==0)		// En la primera pasada no hay ayuda asi es que la creamos...
					delete_text(all_text);
					textos=0;				// y lo indicamos.
					Ayuda();
					FRAME;
				END				
			ELSE
				IF(Textos==0)				// La ayuda fue creada?
					auxiliar=Get_id(TYPE Ayuda);
					signal(auxiliar,s_kill);		// La matamos y creamos los textos...
										// esto nos sirve para que los textos solo se impriman una vez
										// por cada vez que se solicite la ayuda, en caso contrario
										// FENIX nos daria un error de Demasiados textos en pantalla.
					load_pal("scores.fnt");
					write(0,10,135,0,"Nivel");
					write_int(0,46,135,0,&fase);
					write(0,10,158,0,"Cursor Dcho e Izqdo para cambio de marciano.");
					write(0,10,166,0,"Cursor arriba y abajo + - nivel. Enter salva.");
					write(0,10,174,0,"C borra pantalla. L carga nivel. Escape sale.");
					write(0,10,182,0,"Boton drcho cambia marciano izqdo o mayuscula");
					write(0,10,191,0,"colocan marciano. F1 ayuda.");
					textos=1;				// Los textos ya no se escribiran mas hasta que se deje de 
										// pulsar la tecla F1.
					FRAME;
				END
			END
			IF(key(_esc));		// El jugador quiere salir...
				Opc=0;		// entonces salimos del bucle
				scan_code=0;	// liberamos el teclado
				let_me_alone();	// y destruimos todos los procesos
				delete_text(all_text);
				FROM auxiliar=1 TO 15	
					FRAME;		// Hacemos una pequeña pausa que nos evitara problemas.
				END
			END
			FRAME;
		END
	END
	
	// El jugador eligio la configuracion...
	IF(Opc==3)
		Stop_song();							// Paramos la musica y ponemos la 
		idmusica=Load_song("musica/creditos.xm");		// melodia de los creditos.
		play_song(idmusica, -1);
		fichero=fopen("config/config.dat",o_read);		// Miramos si el fichero de configuracion esta creado
		IF(fichero<>0);
			cuanto=fgets(fichero);					// si lo esta cargamos lo valores y los aplicamos.
			m_volumen=atoi(cuanto);
			cuanto=fgets(fichero);
			e_volumen=atoi(cuanto);
		ELSE
			Texto(160,100,100,"Generando configuracion...");	// Si no lo esta pues lo creamos e
			WHILE(get_id(TYPE Texto))
				FRAME;								// informamos al jugador de ello.
			END
			fichero=fopen("config/config.dat",o_write);		// Creamos el fichero con
			fputs(fichero,"50");							// los valores por defecto.
			fputs(fichero,"50");
			m_volumen=50;
			e_volumen=50;
		END
		fclose(fichero);
		scan_code=0;
		Let_Me_Alone();
		Clear_screen();
		put_screen(idfpg,25);			// Colocamos el fondo
		mouse.graph=9;
		Boton(110,86,1);				// y los botones que el jugador podra manipular.
		Boton2(160,179);
		WHILE(!key(_esc))
			IF(Key(_enter))
				Texto(160,100,200,"Salvando configuracion...");	// Salvamos la configuracion
				WHILE(Get_id(TYPE Texto))						// que el jugador a elejido.
					FRAME;
				END
				fichero=fopen("config/config.dat",o_write);		// lo salvamos.
				fputs(fichero,itoa(m_volumen));
				fputs(fichero,itoa(e_volumen));
				fclose(fichero);
			END
			FRAME;
		END
		mouse.graph=0;
	END
	scan_code=0;
	
	// El jugador elejio la opcion de ver los creditos.
	IF(Opc==4)
		Let_me_alone();							// Normalizamos la pantalla.
		Clear_screen();							// Borramos el fondo
		delete_text(all_text);						// y los textos.
		Stop_song();						
		idmusica=Load_song("musica/creditos.xm");		// Ponemos la musica de los creditos.
		play_song(idmusica,-1);
		FRAME;								// Lo hacemos visible.
		linea=210;
		load_pal("presenta.fnt");
		set_fps(20,0);							// Graduamos la velocidad para que las lineas se
											// puedan leer.
		idfont=load_fnt("presenta.fnt");				// cargamos la fuente.
		
		// Y mostramos los textos...
		Creditos("Un juego creado por: Papada Soft.");
		Creditos("Papada Soft son:");
		Creditos("");
		Creditos("ManOwaR, programacion y guion");
		Creditos("Dekar, programacion y guion");
		Creditos("Onslaugh, graficos");
		Creditos("Fernando Navarro, graficos");
		Creditos("Dadtrox, musica");
		Creditos("Rauko, musica");
		Creditos("");
		Creditos("Para Daniel Navarro y Jose Luis Cebrian, gracias por"); 
		Creditos("todo.");
		Creditos("");
		Creditos("Dedicado a toda esa gente que ha participado en el");
		Creditos("concurso de DIVnet, desde aqui le deseo suerte y que");
		Creditos("gane el mejor.");
		Creditos("");
		Creditos("Colaboraciones especiales:");
		Creditos("");
		Creditos("zwiTTeR, Titonus y Tristan, graficos");
		Creditos("CicTec, Titonus, Mix, Tropsy, Beorn y Benko,");
		Creditos("betatesters.");
		Creditos("");
		Creditos("Juego programado para el II Concurso de Matamarcianos");
		Creditos("DIVnet, Http://divnet.divsite.net, utilizando Fenix.");
		Creditos("");
		Creditos("Las melodias han sido tomadas prestadas de un cd de");
		Creditos("la revista PCMania, los derechos de las mismas");
		Creditos("pertenecen a sus respectivos autores.");
		Creditos("");
		Creditos("Agradecimientos especiales a todas las personas que");
		Creditos("dia a dia apoyan nuetros proyectos y nos hacen ver");
		Creditos("que podemos llegar un poquito mas alla, tambien a");
		Creditos("toda la gente que invierte su tiempo en crear juegos");
		Creditos("y en alimentar ste mundillo que nos rodea, el");
		Creditos("mundillo amateur.");
		scan_code=0;
		
		// Una vez mostrado por completo hacemos que aparezca el mensaje de "Pulsa Una tecla..."
		WHILE(scan_code==0)
			IF(get_id(TYPE Creditos)==0)
				auxiliar=write(idfont, 160,180,4,"Pulsa una tecla...");
				FRAME;
				delete_text(auxiliar);
			ELSE
				FRAME;
			END
		END
	END
END
// Aqui finaliza el programa principal.
END



// Ahora pasamos a describir las funciones utilizadas por el programa principal y por las mismas funciones internamente.


// *************************************************************************************
// * Este proceso creara un boton que sera usado para la configuracion, el boton funciona       *
// * funciona dejando pulsado el boton cuando el raton esta sobre el y una vez en ese estado    *
// * podremos moverlo, variando asi el valor del volumen con el cual se escucharan las		*
// * melodias y los efectos de sonido. La funcion recibe como parametros las coordenadas	*
// * de pantalla y el numero de boton, que sera usado para manipularlo internamente		*
// *************************************************************************************
PROCESS Boton(x,y,num)
BEGIN
file=idfpg;			// Asignamos el fpg del cual tomara el grafico.
graph=27;
IF(num==1)
	x=x+m_volumen;	// adecuamos su posicion en pantalla dependiendo del valor de la configuracion.
ELSE
	x=x+e_volumen;	// igual para el otro boton.
END
LOOP
	IF(Collision (TYPE mouse) & mouse.left)
		WHILE(mouse.left)			// Mientras el boton del raton esta pulsado
			x=mouse.x;				// hacemos que el boton varie claro esta...
			IF(x>210)
				x=210;			// sin salirse
			END
			IF(x<110)
				x=110;			// de sus limites minimo y maximo.
			END
			IF(num==1)
				volume=x-110;		// y variamos el valor del volumen segun el valor del boton.
			ELSE
				e_volumen=x-110;
			END
			FRAME;
		END
	END
	FRAME;
END
END


// *************************************************************************************
// * Este otro boton, tambien usado en la configuracion, funciona cuando se hace click sobre    *
// * el, reproduciendo un efecto de sonido para comprobar su volumen. Unicamente recibe como *
// * parametro sus coordenadas en pantalla.							*
// *************************************************************************************
PROCESS Boton2(x,y)
BEGIN
file=idfpg;
graph=26;
LOOP
	IF(Collision (TYPE mouse) & mouse.left & Timer[8]>35) 	// Se ha pulsado y puede ser activado?
			play_wav(idsexplosion1,0);				// pues reproducimos el sonido
			Timer[8]=0;						// y hacemos una pausa antes de la proxima 
	END										// reproduccion.
	FRAME;
END
END

// *************************************************************************************
// * Este proceso es usado por los creditos y recibe como parametro una frase o texto el	*
// * realizara un scroll desde la parte inferior de la pantalla hasta la superior destruyendose *
// * una vez sobrepasada la parte superior. La variable linea indica a que altura ha de		*
// * situarse la frase, cada vez que se imprime una frase esta variable aumenta en 15pixels.	*
// *************************************************************************************
PROCESS Creditos(STRING Frase)
PRIVATE idtextu;
BEGIN
x=10;
y=linea;			// Aignamos la altura dependiendo del valor contenido en linea.
linea+=15;			// hacemos que la proxima linea se imprima 15 pixels por debajo de esta.
WHILE(y>-20)					// Cuando rebasemos la parte superior de la pantalla...
	idtextu=write(idfont,x,y,0,frase);		// Escribimos el texto en su posicion actual
	FRAME(200);					// lo mostramos
	delete_text(idtextu);				// y lo borramos para poder visualizarlo en su nueva posicion.
	y-=1;						// asignamos su nueva posicion.
END							// ... destruimos el proceso.
END
	
// *************************************************************************************
// * El proceso que controlara nuestra nave, se encarga de su total control, de mostrar las     *
// * situaciones anormales, daños, y demas cosas relacionadas con la misma.			*
// *************************************************************************************
PROCESS Nave()
PRIVATE conta;
BEGIN
idnave=id;
file=idseleccion;
Graph=NaveElejida*2;		// Los graficos fueron colocados en el fpg de manera que se pudiese utilizar esta formula.
x=320/2;
y=185;
LOOP
file=idseleccion;
	IF(Veneno<1)		// Si nuestra no padece paralisis...
		pulsada=0;
    		IF(key(_right))
			aceleracion++;		// Pues dejamos que se mueva, a la derecha y
			pulsada=1;
    		END
    		IF(key(_left))
			aceleracion--;		// a la izquierda.
			pulsada=1;
	    	END
		IF(aceleracion>Naves[NaveElejida-1].acelera)		// graducamos la aceleracion dependiendo de la nave
			aceleracion=Naves[NaveElejida-1].acelera;	// que hayamos seleccionado al comienzo de la 
		END									// partida.
		IF(aceleracion<-Naves[NaveElejida-1].acelera)
			aceleracion=-Naves[NaveElejida-1].acelera;
		END
		x+=aceleracion;							// lo sumamos a las coordenadas de pantalla.
		IF(pulsada==0)
			IF(aceleracion<>0)
				IF(aceleracion>0)
					aceleracion--;
				ELSE
					aceleracion++;
				END
			END
		END
	ELSE				// Si nuestra nave esta paralizada...
		file=idnavesmal;	// cambiamos su color
		graph=NaveElejida;
		veneno--;		// y bajamos el contador del veneno.
	END
    	IF(x<10)
	    	x=10;		// limitamos la coordenada minima de nuestra nave hacia la izquierda
    	END
    	IF(x>310)
	    	x=310;		// y hacia la derecha.
    	END
	IF(Tapon<1)		// Nuestra nave no esta taponada luego puede disparar perfectamente...
	file=idSeleccion;
    		IF(key(_space))
			IF(disparando==0 & disparos<Naves[NaveElejida-1].cadisparo)	// si no hemos sobrepasado el
				IF(Get_id(TYPE Texto)==0)						// limite simultaneo de disparos
	        			disparo(x,y);							// de nuestra nave, entonces 
					Timer[2]=0;							// creamos un disparo nuevo.
					disparando=1;							// y lo indicamos.
				END
			END
		ELSE
			disparando=0;
    		END
	ELSE					// si por el contrario estaba taponada....
		file=idnavesmal;		// pues cambiamos su color para indicarlo
		graph=NaveElejida*4;
		Tapon--;			// y bajamos el contador de taponamiento del cañon.
	END
	IF(auxiliar=Collision(TYPE CreaEnemigo))		// Si nuestra nave colisiona con algun marciano...
		Explosion(auxiliar.x, auxiliar.y);		// mostramos una explosion en las coordenadas del marciano
		signal(auxiliar,s_kill);				// lo matamos
		energia-=2;					// reducimos nuestra energia en 2 unidades
		nenemigos--;					// e indicamos que ahora queda un enemigo menos.
	END
    	Conta++;							// Esta variable controla el dibujo de nuestra nave que hemos de
    	IF(conta==40)						// mostrar, es para simular el cambio del motor, a unas 20fps.
	    	Conta=0;
    	END
	// Mostramos el grafico correspondiente dependiendo de si la nave esta normal, taponada o paralizada.
    	IF(conta>20)
	    	Graph=NaveElejida*2;
		IF(Veneno>0)
			file=idnavesmal;
			graph=NaveElejida;
		END
		IF(Tapon>0)
			file=idnavesmal;
			graph=NaveElejida*4;
		END
    	ELSE
		file=idSeleccion;
	    	Graph=NaveElejida*2+1;
    	END
    	FRAME;
	END
END

// *************************************************************************************
// * Nuestro proceso disparo, es el que genera la nave cuando en condiciones normales 		*
// * pulsamos ESPACIO, toma como parametros las coordenadas iniciales en pantalla.		*
// *************************************************************************************
PROCESS Disparo(x,y)
PRIVATE idenemigo, fuerza;
BEGIN
file=idfpg;
z=10;
IF(power==1)			// Dependiendo del poder de disparo que nuestra nave tenga
	graph=2;			// lo mostraremos azul
	fuerza=1;
ELSE
	graph=24;			// o verde.
	fuerza=2;			// y por consiguiente podremos matar dos marcianos con un solo disparo.
END
disparos++;			// el disparo ha sido creado, informamos de ello para el limite de cada nave.
play_wav(idsdisparo,0);	// y reproducimos el sonido asociado al disparo.
WHILE(y>0)			// Si el disparo no ha salido por la parte superior de la pantalla...
    IF(idEnemigo=Collision (TYPE CreaEnemigo))	// ... miramos si ha impactado contra algun marciano.
        Explosion(idEnemigo.x, idEnemigo.y);		// si impacto pues mostramos la explosion
        IF(Rand(0,100)<40)					// y damos un 40% de que el marciano tubiese un item.
            Item(idEnemigo.x,idEnemigo.y);
        END
        Signal(idEnemigo, s_kill);				// matamos el marciano
        nEnemigos--;						// informamos de la baja del enemigo
        Puntos+=100;						// sumamos los 100 puntos que vale el marciano
	fuerza--;							// y reducimos la fuerza del disparo en 1
        FRAME;
	IF(Fuerza==0)						// si el disparo ya no tiene fuerza
        	BREAK;						// lo matamos
	END
    END
    IF(Get_id(TYPE Texto))					// si hay algun texto en pantalla pues tambien matamos el disparo
    	break;							// esto nos sirve para evitar que el jugador dispare mientras se
    END								// muestra el mensage de "Fase 1" o "Fase X".
    y-=4;								// le hacemos subir un poquillo...
    FRAME;
END
Disparos--;								// Si el disparo murio pues informamos de que ahora hay un disparo
END									// menos en la pantalla.

// *************************************************************************************
// * El proceso ITEM generara aleatoriamente una letra que caera desde la posicion del	*
// * marciano que la contenia hasta la parte inferior de la pantalla, en caso de que colisione	*
// * con la nave pues se pondra en el marcador o se sumara a sus puntos si ya la tenia.		*
// *************************************************************************************
PROCESS Item(x,y)
BEGIN
file=idfpg;
z=-32;
Graph=12+(Rand(0,2));		// Generamos aleatoriamente el ITEM
WHILE (y<200)				// o lo hacemos bajar hasta la parte inferior de la pantalla...
    IF(Collision (TYPE Nave))		// Si la nave lo recoge...
    	play_wav(idsitem,0);		// ponemos su sonido
        IF(Graph==12)			// y dependiendo de su grafico sumamos un item mas del tipo en cuestion
	    IF(p<2)
                p++;
                IF(p==1)			// y si no esta completo pues lo colocamos en el marcador.
                    y=16;
                    x=500/2;
		    FRAME;
                ELSE
                    y=16;
                    x=540/2;
		    FRAME;
                END
                LOOP
                    FRAME;
                END
            ELSE
                Puntos+=50;
                BREAK;
            END
        END
        IF(Graph==13)
            IF(a<3)
                a++;
                y=16;
                SWITCH (a):
                    Case 1:
                        x=520/2;
                    END
                    CASE 2:
                        x=560/2;
                    END
                    CASE 3:
                        x=600/2;
                    END
                END
                LOOP
                    FRAME;
                END
            ELSE
                Puntos+=50;
                BREAK;
            END
        END
        IF(graph==14)
            IF(d<1)
                d++;
                y=16;
                x=580/2;
                LOOP
                    FRAME;
                END
            ELSE
                Puntos+=50;
                BREAK;
            END
        END
    END
    y+=2;
    FRAME;
END
END

// *************************************************************************************
// * El proceso explosion simplemente mostrara una explosion en las corrdenadas que se le	*
// * pasen y reproducira su sonido.									*
// *************************************************************************************
PROCESS Explosion(x,y)
BEGIN
file=idfpg;
size=50;
play_wav(idsexplosion1,0);
FROM graph=3 TO 8;
    FRAME;
END
END

// *************************************************************************************
// * El proceso Texto es el encargado de mostrar un texto parpadeante en pantalla durante	*
// * un tiempo limitado que viene especificado por el parametro Tiempo, Frase es el texto a	*
// * mostrar y x e y las coordenadas en pantalla.							*
// *************************************************************************************
PROCESS Texto(x,y,Tiempo, STRING Frase)
PRIVATE idTitulos, idTexto, nframes;
BEGIN
codigo_id=id;
load_pal("Titulos.fnt");
idTitulos=Load_fnt("scores.fnt");		// Cargamos la fuente
Timer[3]=0;						// Y hacemos que el reloj comience a contar.
WHILE(timer[3]<Tiempo)				// mientras que no se haya terminado el tiempo de visionado
    idTexto=Write(idTitulos, x, y, 4, Frase);	// mostramos el texto a
    FROM nframes=1 TO 20;				// unas 20fps parpadeando en pantalla, para ello lo mostramos
        FRAME;
    END
    Delete_text(idTexto);				// y borramos continuamente refrescando la pantalla para mostrar
    FROM nframes=1 TO 20;				// el cambio
        FRAME;
    END
END
Unload_fnt(idTitulos);					// antes de abandonar el proceso descargamos la fuente usada.
END

// *************************************************************************************
// * El proceso Texto2 es  exactamente igual que el proceso anterior, su mision es la misma	*
// *************************************************************************************
PROCESS Texto2(x,y,Tiempo, STRING Frase)
PRIVATE idTitulos, idTexto, nframes;
BEGIN
codigo_id=id;
load_pal("Titulos.fnt");
idTitulos=Load_fnt("scores.fnt");
Timer[7]=0;
WHILE(timer[7]<Tiempo)
    idTexto=Write(idTitulos, x, y, 4, Frase);
    FROM nframes=1 TO 20;
        FRAME;
    END
    Delete_text(idTexto);
    FROM nframes=1 TO 20;
        FRAME;
    END
    FRAME;
END
Unload_fnt(idTitulos);
END

// *************************************************************************************
// * Este proceso es el encargado de generar la matriz de enemigos y creara en la posicion	*
// * correspondiente dependiendo de la celda que ocupe un marciano en pantalla			*
// *************************************************************************************
PROCESS CreaFase(numfase)
PRIVATE Contador, Contador2;
BEGIN
FOR(Contador=1; Contador<97; Contador+=16)
    FOR(Contador2=1; Contador2<=16; Contador2++)
        CreaEnemigo(27+(Contador2*34), (64+(Contador/16)*34), Fases[Contador+Contador2]);	// Ajustamos en pantalla la posicion del marciano.
    END
END
END

// *************************************************************************************
// * Rellena la pantalla con tiles vacias, para que el jugador pueda diseñar su nivel sobre las	*
// * casillas vacias.											*
// *************************************************************************************
PROCESS CreaTiles()
PRIVATE Contador, Contador2;
BEGIN
FOR(Contador=1; Contador<97; Contador+=16)
    FOR(Contador2=1; Contador2<=16; Contador2++)
	Fases[Contador+Contador2]=0;		// Reseteamos para que todas las casillas contengan tiles vacios.
	CreaTile(27+(Contador2*34), (64+(Contador/16)*34),contador+contador2);	// Y entonces lo creamos.
    END
END
FRAME;
END

// *************************************************************************************
// * Coloca un tile vacio en la posicion que recibe como parametros, ademas de la posicion	*
// * tambien recibe el numero de tile para posteriormente poder referirnos a ella cuando	*
// * colisione con el puntero del raton.								*
// *************************************************************************************
PROCESS CreaTile(x,y,num)
BEGIN
mouse.left=0;
mouse.graph=1;
file=idenemigos;
graph=100;		// Asignamos el grafico del tile vacio.
x=x/2;		// Esto es necesario porque en un primer momento las coordenadas estaban pensadas para una resolucion de
y=y/2;		// 640x480 por lo que el juego al funcionar en 320x200 hay que reducirlas a la mitad,
flags=4;		// la hacemos trasparente.
z=-10;
Timer[4]=0;
WHILE(Timer[4]<10)	// Esta pausa es necesaria para solventar algun oscuro problema interno de FENIX, quizas se pudiese
	FRAME;		// prescindir de ella en estos momentos pero por lo pronto y al no afectar sobremanera al desarrollo
END				// del juego la dejare puesta.
LOOP
	IF(Collision(TYPE mouse) & (mouse.left==1 | key(_r_shift)))	// Si el tile colisiona con el raton y el jugador
		graph=mouse.graph;							// pulsa el boton pues asignamos el marciano
		Fases[Num]=graph;							// que este seleccionado y hacemos el cambio
		IF(graph==100)								// en la estructura, si es un tile vacio
			fases[Num]=0;							// tambien lo indicamos.
		END
	END
	FRAME;
END
END

// *************************************************************************************
// * Este proceso es el encargado de mostrar un enemigo en pantalla, es algo largo porque 	*
// * preferi crearlos todos y controlarlos desde el mismo proceso pero igualmente se podria	*
// * haber creado varias procesos, recibe las coordenadas en 640x480 por eso re reducen a 	*
// * la mitad al comienzo del proceso y el tipo de enemigo, que nos servira despues para	*
// * reproducir el comportamiento del enemigo en cuestion.					*
// *************************************************************************************
PROCESS CreaEnemigo(x,y, Tipo)
PRIVATE Estado,x_Inicial,y_Inicial,x_final, fBicho, izquierda=0, Bajando;
BEGIN
IF(Tipo<>0);				// Entraremos al control del proceso siempre que el hueco no este vacio.
	x=x/2;				// actualizamos coordenadas a 320x200.
	y=y/2;
	z=-20;
	nEnemigos++;			// e informamos que ahora hay un enemigo mas en la pantalla.
	file=idEnemigos;
	graph=Tipo;			// ponemos el grafico que corresponde al tipo
	X_inicial=10;
	Y_Inicial=y;
	X_final=310;
	Estado=0;
	WHILE(get_id(TYPE Texto))		// Hacemos que no actue mientras haya algun texto informativo en pantalla.
	    	FRAME;
	END
	LOOP
	// *********************** Enemigo Tipo 1-3-7-9-33-45 *************************
	IF(Tipo==1 | Tipo==3 | Tipo==7 | Tipo==9 | Tipo==33 | Tipo==45)
	// Movemos el marciano de un lado a otro de la pantalla haciendo que baje 5 pixels cada vez que llegue a uno de los limites.
    	IF(Izquierda==0)
	        IF(x<=x_Final)
        	   	x+=Velocidad;
        	ELSE
            		Izquierda=1;
	    		y+=5;
        	END
    	ELSE
	        IF(x>x_inicial)
            		x-=Velocidad;
        	ELSE
            		Izquierda=0;
	    		y+=5;
        	END
    	END
	// y representamos su animacion 
    	IF(fBicho<20)
	        graph=Tipo+1;
    	ELSE
	        graph=Tipo;
    	END
    	fBicho++;
    	IF(fBicho>40)
	        fBicho=0;
    	END
    	IF(Rand(0,500)<1*Tipo)	// Le damos una posibilidad entre 500 de que dispare si lo consigue...
	        DisparoTipo1(x,y);	// ... pues creamos el disparo.
    	END
	END
	// **************** Aqui termina Enemigo Tipo 1-3-7-9-33-45 **********************
	
	// *********************** Enemigo Tipo 13-21-39 **************************
	IF(Tipo==13 | Tipo==21 | Tipo==39)
    	IF(Izquierda==0)
	        IF(x<=x_Final)
        	   	x+=Velocidad;
        	ELSE
            		Izquierda=1;
	    		y+=5;
        	END
    	ELSE
	        IF(x>x_inicial)
            		x-=Velocidad;
        	ELSE
            		Izquierda=0;
	    		y+=5;
        	END
    	END
    	IF(fBicho<20)
	        graph=Tipo+1;
    	ELSE
	        graph=Tipo;
    	END
    	fBicho++;
    	IF(fBicho>40)
	        fBicho=0;
    	END
    	IF(Rand(0,300)<1 & disparando==0)		// Si nos esta disparando y surge la probabilidad
	        Laser1(x,y);					// creamos el laser
		disparando=1;					// e informamos que ahora si esta disparando.
    	END
	END
	// ****************** Aqui termina Enemigo Tipo 13-21-39 ********************
	
	// ************************** Enemigo Tipo 23-31 ***************************
	IF(Tipo==23 | Tipo==31)
    	IF(Izquierda==0)
	        IF(x<=x_Final)
        	   	x+=Velocidad;
        	ELSE
            		Izquierda=1;
	    		y+=5;
        	END
    	ELSE
	        IF(x>x_inicial)
            		x-=Velocidad;
        	ELSE
            		Izquierda=0;
	    		y+=5;
        	END
    	END
    	IF(fBicho<20)
	        graph=Tipo+1;
    	ELSE
	        graph=Tipo;
    	END
    	fBicho++;
    	IF(fBicho>40)
	        fBicho=0;
    	END
    	IF(Rand(0,300)<1 & disparando==0)
	        Laser2(x,y);
		disparando=1;
    	END
	END
	// ******************** Aqui termina Enemigo Tipo 23-31 *********************
	
	
	// *********************** Enemigo Tipo 15-19-25-35*************************
	IF(Tipo==15 | Tipo==19 | Tipo==25 | Tipo==35)
		IF(Izquierda==0)
        		IF(x<=x_Final)
            			x+=Velocidad;
        		ELSE
            			Izquierda=1;
	    			y+=5;
        		END
    		ELSE
        		IF(x>x_inicial)
            			x-=velocidad;
        		ELSE
            			Izquierda=0;
	    			y+=5;
        		END
    		END
        	IF(fBicho<20)
            		Graph=tipo+1;
        	ELSE
            		Graph=Tipo;
        	END
        	fBicho++;
        	IF(fBicho>40)
            		fBicho=0;
        	END
		IF(Rand(0,300)<1)
	        	DisparoTipo3(x,y);
    		END
	END
	// ********************* Aqui acaba Tipo 15-19-25-35 ************************
	
	// ************************ Enemigo Tipo 17-27-41 ***************************
	IF(Tipo==17 | Tipo==27 | Tipo==41)
		IF(Izquierda==0)
        		IF(x<=x_Final)
            			x+=Velocidad;
        		ELSE
            			Izquierda=1;
	    			y+=5;
        		END
    		ELSE
        		IF(x>x_inicial)
            			x-=velocidad;
        		ELSE
            			Izquierda=0;
	    			y+=5;
        		END
    		END
        	IF(fBicho<20)
            		Graph=tipo+1;
        	ELSE
            		Graph=Tipo;
        	END
        	fBicho++;
        	IF(fBicho>40)
            		fBicho=0;
        	END
		IF(Rand(0,300)<1)
	        	DisparoTipo4(x,y);
    		END
	END
	// ************************ Aqui acaba Tipo 17-27-41 *************************
	
	
	// ************************** Enemigo Tipo 11-43 ****************************
	IF(Tipo==11 | Tipo==43)
		IF(Izquierda==0)
        		IF(x<=x_Final)
            			x+=Velocidad;
        		ELSE
            			Izquierda=1;
	    			y+=5;
        		END
    		ELSE
        		IF(x>x_inicial)
            			x-=velocidad;
        		ELSE
            			Izquierda=0;
	    			y+=5;
        		END
    		END
        	IF(fBicho<20)
            		Graph=tipo+1;
        	ELSE
            		Graph=Tipo;
        	END
        	fBicho++;
        	IF(fBicho>40)
            		fBicho=0;
        	END
		IF(Rand(0,300)<1)
	        	DisparoTipo2(x,y);
    		END
	END
	// ************************ Aqui acaba Tipo 11-43 **************************
		
	// ************************ Enemigo Tipo 5-29-37 **************************
	IF(Tipo==5 | Tipo==29 | Tipo==37)
    	IF(Estado==0)
    		IF(Izquierda==0)
        		IF(x<=x_Final)
            			x+=Velocidad;
        		ELSE
            			Izquierda=1;
	    			y+=5;
        		END
    		ELSE
        		IF(x>x_inicial)
            			x-=velocidad;
        		ELSE
            			Izquierda=0;
	    			y+=5;
        		END
    		END
        	IF(fBicho<20)
            		Graph=tipo+1;
        	ELSE
            		Graph=Tipo;
        	END
        	fBicho++;
        	IF(fBicho>40)
            		fBicho=0;
        	END
    	END
    	IF(Rand(0,800)<((Tipo+5/2)-4) & Bajando==0)	// si la probabilidad dio positiva pues hacemos que el marciano
	        angle=get_angle(idnave);				// se lance en pikado, primero tomamos el angula hacia la nave
	        Bajando=1;							// indicamos que ahora esta bajando
	        WHILE(y<490/2)					// y le hacemos bajar hasta la parte inferior de la pantalla
            		advance(2);
            	FRAME;
        	END
        	y=-10;							// una vez abajo le hacemos aparecer en la parte superior
        	angle=0;
        	WHILE (y<y_inicial);					// y que baje hasta la coordenada Y que tenia antes del descenso.
            		y+=2;
               		FRAME;
        	END
        	angle=fget_angle(x,y,x_inicial,y_inicial);
		angle=0;							// Colocamos el angulo inicial
        	Advance(2);
        	Bajando=0;
    	END
    	IF(Bajando==1)
	        angle=fget_angle(x,y,x_inicial,y_inicial);
	        advance(2);
	        FRAME;
    	END
	END
	// *************** Aqui finaliza el enemigo de Tipo 5-29-37 ****************
	
    	FRAME;
	END
nEnemigos--;	// si el enemigo murio... entonces informamos de ello.
END
END

// *************************************************************************************
// * El simple proceso de disparo hara caer un disparo desde la coordenadas del marciano que 	*
// * le haya llamado hasta la parte inferior de la pantalla, si colisiona con la nave mostrara 	*
// * una explosion y restara uno a la energia.						 	*
// *************************************************************************************
PROCESS DisparoTipo1(x,y);
PRIVATE idNaveJ;
BEGIN
file=idfpg;
size=50;
graph=9;
WHILE(y<480)
    y+=3;
    FRAME;
    IF(idNaveJ=Collision(TYPE Nave))
        energia--;
        Explosion(idnaveJ.x,idNaveJ.y);
        BREAK;
    END
END
END

// Mismo proceso que el anterior pero aplicara el efecto de paralisis dependiendo del nivel del marciano sera mayor o menor.
PROCESS DisparoTipo2(x,y);
PRIVATE idNaveJ;
BEGIN
file=idfpg;
size=50;
graph=17;
WHILE(y<480)
    y+=3;
    FRAME;
    IF(idNaveJ=Collision(TYPE Nave))
        Explosion(idnaveJ.x,idNaveJ.y);
	IF(veneno==0)		
		veneno=80+(father.graph*2)-Naves[NaveElejida-1].resveneno;
	END
        BREAK;
    END
END
END

// Igual que el anterior pero para el recalentamiento del cañon.
PROCESS DisparoTipo3(x,y);
PRIVATE idNaveJ;
BEGIN
file=idfpg;
size=50;
graph=18;
WHILE(y<480)
    y+=3;
    FRAME;
    IF(idNaveJ=Collision(TYPE Nave))
        Explosion(idnaveJ.x,idNaveJ.y);
	IF(tapon==0)		
		tapon=80+(father.graph*2)-Naves[NaveElejida-1].restapon;
	END
        BREAK;
    END
END
END

// Este proceso coge el angulo hasta la nave, lanza un proyectil que no puede impactar contra la nave, solo contra el suelo, pero una
// vez explot contra el suelo creara una explosion que si dañara al jugador si entra en contacto con ella, ademas quita 2 de energia.
PROCESS DisparoTipo4(x,y);
PRIVATE idNaveJ, x_destino;
BEGIN
file=idfpg;
size=50;
graph=20;
x_destino=Rand(0,320);
angle=fget_angle(x,y,x_destino,200);
play_wav(idsbomba,0);
WHILE(y<193)
	advance(3);
	FRAME;
END
size=100;
file=idexplo2;
play_wav(idsexplosion2,0);
FROM graph=1 to 16
	FRAME;
	IF(idNaveJ=Collision(TYPE Nave))
	    energia-=2;
    	    Explosion(idnaveJ.x,idNaveJ.y);
    	    BREAK;
    	END
END
END

// *************************************************************************************
// * El proceso laser mostrara un rayo laser lanzado por el marciano, siempre que este este  	*
// * vivo, si logra impactar con el jugador le producira un efecto de recalentamiento del	*
// * cañon.											 	*
// *************************************************************************************
PROCESS Laser1(x,y);
PRIVATE idNaveJ,conta=0;
BEGIN
file=idfpg;
graph=19;
play_wav(idslaser,0);			// Reproducimos el sonido de la bomba al ser lanzada
WHILE(conta<father.graph*2)	// Dependiendo del nivel del marciano el laser durara mas o menos.
    IF(exists(father))			// ahora si el marciano que lo lanzo aun vive...
    	y=father.y;				// pues lo mostramos en sus correspondientes coordendas que las tomamos del marciano
    	x=father.x;
    	IF(idNaveJ=Collision(TYPE Nave))		// Si colisiona con el jugador aplicamos el efecto
	        Explosion(idnaveJ.x,idNaveJ.y);
		Tapon=150-Naves[NaveElejida-1].restapon;	// del recalentamiento y mostramos la explosion.
        	BREAK;							// y matamos el laser.
    	END
   ELSE 
   	FRAME;						
   	BREAK;								// si el marciano que lo llamo ha sido destruido entonces
   END									// destruimos el laser tambien.
   conta++;								// aumentamos el tiempo que el lase lleba funcionando.
   FRAME;
END
father.disparando=0;							// una vez hemos terminado informamos que el marciano
END										// ya no dispara.

// Este proceso es exactamente igual al anterior salvo que el efecto que aplica al colisionar con la nave es el de paralisis.
PROCESS Laser2(x,y);
PRIVATE idNaveJ,conta=0;
BEGIN
file=idfpg;
graph=21;
play_wav(idslaser,0);
WHILE(conta<father.graph*2)
    IF(exists(father))
    	y=father.y;
    	x=father.x;
    	IF(idNaveJ=Collision(TYPE Nave) & veneno==0)
	        Explosion(idNave.x,idNave.y);
		veneno=150-Naves[NaveElejida-1].resveneno;
        	BREAK;
    	END
   ELSE 
   	FRAME;
   	BREAK;
   END
   conta++;
   FRAME;
END
father.disparando=0;
END


// este proceso unicamente mostrara los datos del marcador y pondra su grafico en la parte superior de la pantalla.
PROCESS Marcador()
BEGIN
file=idfpg;
x=160;
y=12;
z=-31;
graph=11;
Write(idpeq, 10, 1, 0, "Puntos:");
Write(idpeq, 10, 10, 0, "Fase:");
Write_int(idpeq, 50, 1, 0, &puntos);
Write_int(idpeq, 42, 10, 0, &fase);
LOOP
    FRAME;
END
END

// Basandose en el fpg de energia aplicara un grafico u otro dependiendo del valor de la variable energia que corresponde a los
// graficos que tiene el fpg.
PROCESS Energia()
BEGIN
x=115;
y=9;
z=-33;
file=idenergia;
LOOP
	graph=energia;
	FRAME;
END
END

// Los tres procesos siguientes sirven para la eleccion de nave de tal forma que cuando uno de ellos es seleccionado mostrara la nave
// con su motor parpadeando y si no esta elegida la dibujara en gris. Cuando se pulsa Enter los tres procesos se destruyen.
PROCESS Nave1Sel();
PRIVATE Elejida, cont;
BEGIN
x=160;
y=120;
file=idSeleccion;
graph=1;
LOOP
	IF(NaveElejida==1)
		Elejida=1;
	ELSE
		Elejida=0;
		cont=0;
	END
	IF(Elejida==0) graph=1; END
	IF(Elejida==1 & Cont<20)
		graph=2;
	END
	IF(Elejida==1 & Cont>20)
		graph=3;
	END
	cont++;
	IF(cont==40)
		cont=0;
	END
	IF(Key(_enter))
		BREAK;
	END
	FRAME;
END
END


PROCESS Nave2Sel();
PRIVATE Elejida, cont;
BEGIN
x=80;
y=120;
file=idSeleccion;
graph=8;
LOOP
	IF(NaveElejida==2)
		Elejida=1;
	ELSE
		Elejida=0;
		cont=0;
	END
	IF(Elejida==0) graph=8; END
	IF(Elejida==1 & Cont<20)
		graph=4;
	END
	IF(Elejida==1 & Cont>20)
		graph=5;
	END
	cont++;
	IF(cont==40)
		cont=0;
	END
	IF(Key(_enter))
		BREAK;
	END
	FRAME;
END
END


PROCESS Nave3Sel();
PRIVATE Elejida, cont;
BEGIN
x=240;
y=120;
file=idSeleccion;
graph=9;
LOOP
	IF(NaveElejida==3)
		Elejida=1;
	ELSE
		Elejida=0;
		cont=0;
	END
	IF(Elejida==0) graph=9; END
	IF(Elejida==1 & Cont<20)
		graph=6;
	END
	IF(Elejida==1 & Cont>20)
		graph=7;
	END
	cont++;
	IF(cont==40)
		cont=0;
	END
	IF(Key(_enter))
		BREAK;
	END
	FRAME;
END
END

PROCESS Control_sel()
PRIVATE x_max=1, x_min=160;
BEGIN
file=idSeleccion;
x=160;
y=70;
graph=9+NaveElejida;
Define_region(1,x_min,50,x_max,90);
region=1;
LOOP
	write(idpeq,160,175,4,Caracteristicas[NaveElejida-1].Texto);
	IF(x_min>100)
		x_min-=4;
	END
	IF(x_max<120)
		x_max+=4;
	END
	IF(key(_left) & Timer[0]>25)
		NaveElejida++;
		Timer[0]=0;
		graph++;
		x_max=0;
		x_min=160;
	END
	IF(Key(_right) & Timer[0]>25)
		graph--;
		NaveElejida--;
		Timer[0]=0;
		x_max=0;
		x_min=160;
	END
	IF(key(_enter))
		BREAK;
	END
	IF(graph==13)
		graph=10;
	END
	IF(graph==9)
		graph=12;
	END
	IF(NaveElejida>3)
		NaveElejida=1;
	END
	IF(NaveElejida<1)
		NaveElejida=3;
	END
	Define_region(1,x_min,50,x_max,90);
	FRAME;
	delete_text(id_textu);
END
END

// Este proceso salva una fase grabando en el fichero nivel+fase.dat el valor de la estructura de los marcianos.
PROCESS SalvaFase()
BEGIN
fichero=fopen("niveles/nivel"+itoa(fase)+".dat",o_write);
fwrite(fichero,fases);
fclose(fichero);
FRAME;
END

// El proceso inverso, carga la estructura para ser modificada o usada o en su defecto informa de que el nivel no existe.
PROCESS CargaFase()
BEGIN
IF((fichero=fopen("niveles/nivel"+itoa(fase)+".dat",0))<>0);
	fread(fichero,fases);
	fclose(fichero);
	FRAME;
ELSE
	IF(Opc==1)
		Let_me_Alone();
		Clear_screen();
		delete_text(all_text);
		Fin();
	ELSE
		Texto2(160,100,400,"Fichero no encontrado...");
	END
END
END

// realiza el scroll del titulo principal hasta su posicion en la pantalla principal.
PROCESS Titulo()
BEGIN
file=idmenu;
graph=2;
y=24;
FROM x=450 TO 130 STEP -9
	FRAME;
END
LOOP
	FRAME;
END
END

// Hace lo propio con el subtitulo y cuando llega a su destino hace un efecto de "vibrador".
PROCESS SubTitulo()
PRIVATE x_ant, valor;
BEGIN
file=idmenu;
graph=3;
y=54;
FROM x=-50 TO 235 STEP 10
	FRAME;
END
LOOP
	IF(rand(0,50)<5)
		IF(rand(0,50)<25)
			x-=1;
		ELSE
			x+=1;
		END
	ELSE
		x=235;
	END
	FRAME;
END
END

PROCESS Opcion(x,y_inicial,num);
BEGIN
file=idmenu;
y=y_inicial;
graph=4+num;
FOR(auxiliar=1; auxiliar<165;auxiliar++)
	y-=3;
	FRAME;
END
LOOP
	IF(collision(TYPE raton) & mouse.left==1)
		Opc=num;
		FRAME;
	END
	FRAME;
END
END

// Este proceso pone un marciano en pantalla y realiza un scroll de derecha a izquierda. Si el marciano es destruido creara el 
// el modo truko.
PROCESS Bicho(y)
BEGIN
file=idenemigos;
graph=18;
z=10;
FOR(x=330; x>-5; x-=4);
	FRAME;
	IF(Collision(TYPE raton) & mouse.left==1 & idraton.graph==4)
		Explosion(x,y);
		ModoTruko=1;
		BREAK;
	END
END
END

// Proceso que ejecutandose 5 veces mas rapido que los demas consigue que nuestra energia nunca decaiga por debajo de 0 obteniendo
// asi la energia infinita que nos proporciona el modo_truko.
PROCESS Energia_Infinita()
BEGIN
LOOP
	energia=Naves[NaveElejida-1].vida;
	frame(20);
END
END
	
// Este proceso controla el raton en la pantalla del menu haciendo que podamos activar el punto de mira para poder destruir los
// marcianos y acceder al modo_truko.
PROCESS Raton()
BEGIN
idraton=id;
z=-10;
graph=mouse.graph;
file=mouse.file;
LOOP
	IF(Opc==0)
		IF(key(_l_shift))			// Con el mayusculas izquierdo activaremos el punto de mira.
			Truko1=1;
			graph=4;
		ELSE
			graph=9;
		END
	END
	x=mouse.x;
	y=mouse.y;
	FRAME;
END
END

// Proceso que muestra los graficos de final, es llamado cuando estamos jugando una partida, si al tratar de cargar un nivel este
// no se encuentra entonces se llamara a este proceso ya que no hay mas niveles y por lo tanto se ha llegado al final del juego.
// La forma de funcionar es exactamente la misma que cuando se cambia de fase solo que aqui se cargaran siempre los graficos de
// final de partida.
PROCESS Fin()
BEGIN
x=160;
y=60;
Delete_text(all_text);
put_screen(0,Load_png("graficos/fin1.png"));
fichero=fopen("textos/fin.txt", o_read);
TxT=fgets(fichero);
TxT=Substr(TxT,0,-2);
write(0,160,174,4,TxT);
scan_code=0;
WHILE(scan_code==0)
	FRAME;
END
delete_text(all_text);
put_screen(0,Load_png("graficos/fin2.png"));
TxT=fgets(fichero);
TxT=Substr(TxT,0,-2);
write(0,160,174,4,TxT);
fclose(fichero);
scan_code=0;
WHILE(scan_code==0)
	FRAME;
END
delete_text(all_text);
FRAME;
Exit("Fin",0);			// Al haber terminado con todos los niveles saldremos del programa.
END


// El proceso ayuda estara activo para controlar que los textos de la misma no se impriman muchas veces seguidas porque de suceder
// asi el programa nos daria un mensaje de "demasiados textos en pantalla".
PROCESS Ayuda()
BEGIN
z=-10;
x=160;
y=100;
file=idfpg;
graph=23;
LOOP
	FRAME;
END
END
