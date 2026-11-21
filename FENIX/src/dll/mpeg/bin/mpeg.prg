PROGRAM MPEG;


// PROGRAMA DE DEMOSTRAIàN BµSICO DEL FUNCIONAMIENTO DE LA LIBRERIA MPEG
//
//
// A parte de las funciones usadas en este ejemplo existen las siguientes.
// Su uso es logico por lo cual no nos extenderemos.
//
// Pause_mpeg(mpeg id);
//
// -Pausa la reproducci¢n o la pone en marcha si ya estaba pausada
//
// Rewind_mpeg( mpeg id);
//
// -Rebobina y detiene la reproducci¢n de una pel¡cula en marcha
//
// Set_mpeg_volume( mpeg id, int volume);
//
// -Indica el volumen con el que se reproducir  el audio del video (0-100)


GLOBAL

video; //variable que usaremos para identificar el video
tiempo_total; //variable para controlar el tiempo total que dura un video
tiempo_actual; //variable para controlar el tiempo actual de  un video

texto; //variable para controlar el texto escrito

private

import "mpeg.dll"  // importamos la dll

BEGIN

//ESTA LIBRERIA UNICAMENTE FUNCIONA EN 16 BITS POR LO TANTO
//INICIALIZAMOS EL MODO GRAFICO


//FULL_SCREEN=false;
set_mode (640, 480, MODE_16BITS);

frame;

//Cargamos el video, el primer par metro corresponde al fichero
//El segundo par metro nos indica si queremos mostrar video o no
//El tercer par metro nos indica si queremos reproducir audio o no
//
// NOTA: La reproducci¢n de audio bloquea cualquier otra se¤al de audio 
// en proceso. Unicamente puede reproducirse un video con audio 
// simultaneamente. Podemos empezar a usar esta librer¡a despu‚s
// de haber inicializado el modo gr fico.



video=load_mpeg("goal.mpg",1,1);

//hacemos que el video tenga loop 


// calculamos el tiempo total que dura el video. Este resultado se devuelve
// en centisegundos para que use la misma escala que los timers

tiempo_total=mpeg_total_time(video);

//inicializamos tiempo actual;

tiempo_actual=0;




loop_mpeg(video,1);



// Para reproducir un fichero de video debemos indicar que vamos a usar
// en un proceso determinado el FILE 0 y el GRAPH devuelto por la funci¢n
// play_mpeg. A partir de entonces el podremos usar el mapa como si fuera
// uno m s. ATENCION: no debes hacer nunca unload_map de ese gr fico

file=0;
graph=play_mpeg (video);

x=320;
y=240;



// mostramos los textos por pantalla

write (0,0,0,0,"TIEMPO TOTAL: "+itoa(tiempo_total));
texto= write (0,0,20,0,"TIEMPO ACTUAL: "+itoa(tiempo_actual));


write (0,320,60,1,"1- ELIMINAR TODOS LOS FILTROS");
write (0,320,80,1,"2- ACTIVAR FILTRO BILINEAR");
write (0,320,100,1,"3- ACTIVAR FILTRO DEBLOCK");
write (0,320,140,1,"ESC- DESACTIVAR LOOP DEL VIDEO");

// pasaremos el video hasta que ‚ste se acabe una vez desactivemos el loop

repeat

   tiempo_actual=mpeg_current_time(video);
   delete_text(texto);
   texto= write (0,0,20,0,"TIEMPO ACTUAL: "+itoa(tiempo_actual));

   if (key(_esc))
           loop_mpeg(video,0);
   end;

   //activaci¢n de los diferentes tipos de filtros para reproducir video

   if (key(_1)) mpeg_filter(video,0); end;  //ning£n filtro
   if (key(_2)) mpeg_filter(video,1); end;  //filtro bilineal
   if (key(_3)) mpeg_filter(video,2); end;  //ning£n deblock


   frame;
until (is_playing_mpeg(video)==false);

// por £ltimo nos aseguraremos de parar la reproducci¢n y descargaremos
// el video de memoria

stop_mpeg(video);
unload_mpeg(video);


END;

