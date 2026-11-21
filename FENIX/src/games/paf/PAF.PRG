
//************************** PAF! *****************************
//********** creado por:.. Jose Manuel Habas Millan ***********
//*********************************** 7/08/2001 ***************
//******************************** AMAKA SOFT 2001/02 *********
//********** Version para el proyecto Fenix 1.0. **************

//************* TECLAS DE JUEGO: cursor, b, n, m **************

program PAF;                       //se inicia el programa
    global                         //declaramos las variables globales que se van a usar durante todo el juego
        vidas;
        puntos;
        porcentaje;

    begin
        set_mode(m320x240);        //ponemos resolucion de pantalla
        graph_mode=mode_16bits;    //ponemos el juego a 16 bits de color, si no se pone sera automaticamente 8 bits
        define_region(0,0,0,320,240);       //con esto definimos una region de pantalla, que en este caso es para la pantalla completa

        load_fpg("paf\graficos.fpg");      //carga graficos del juego
        load_fnt("paf\fuentes.fnt");       //carga la fuente del juego

        inicio();  //llamada al proceso inicial donde pondremos el valor inicial a todas las variables globales
end

//proceso que inicializa el valor de las variables globales del juego, siempre que acabes la partida o te maten, volvera a entrar en este proceso

process inicio();
    private;
        tiempo=0;   //variable privada, es exclusiva para este proceso
    begin
        fade_off();       //quita la imagen con un fundido
        let_me_alone();   //borramos todo para cuando vuelvas aki otra vez, cuando te hayan matado por ejemplo
        delete_text(all_text);      //borra todos los textos de pantalla
        fade_on();        //devuelve la imagen con otro fundido

        vidas=3;        //le damos valor inicial a vidas
        puntos=0;       //le damos valor inicial a puntos
        porcentaje=0;   //le damos valor inicial a porcentaje

        foto(30,160,120,100,100,0);     //llamada para crear imagen del fondo

        letra_titulo(10,107,-60);       //llamada al proceso letra_titulo para la letra P
        letra_titulo(12,146,-80);       //para la letra A
        letra_titulo(14,187,-100);       //para la letra F
        letra_titulo(16,220,-120);      //para la letra !

        foto(5,160,220,100,100,0);      //llamada para crear imagen del nombre del creador

        prota_demo();       //llamada para mostrar al prota andando en la pantalla de presentacion

        pulsa_intro();

    loop     //esto crea los bucles infinitos en los juegos, si lo quitas no podras pulsar las teclas y dejara de contar la variable tiempo
        tiempo=tiempo+1;    //temporizador para esperar un rato antes de poder pulsar las teclas
            if (tiempo>40)      //comprueba si es mayor de 30, si es mayor ya puedes pulsar las teclas
                if (key(_enter)) juego();end    //pulsando la tecla intro hace llamada al proceso juego()
                if (key(_esc)) exit("www.amakasoft.net",0);end      //pulsando la tecla esc finalizara el juego
            end

    frame;
    end
end

//proceso que deja imagen fija en pantalla

process foto(graph,x,y,size,z,flags);
    begin
    loop
    frame;
    end
end

//proceso para las letras de titulo de presentacion

process letra_titulo(graph,x,y);
    private;
        anima=0;    //variable para hacer saltos de animacion
    begin
        size=100;       //lo dejamos al tama¤o normal, si fuera 200 seria el doble de grande
        z=10;       //esto hace la profundidad para los 2D si un grafico debe de estar delante de otro, si es menor estara delante si es mayor detras

    loop

        if (anima==0)       //comprueba si anima es 0
            y=y+6;if (y>90) y=90;anima=1;end    //si anima es 0 incremeta la y+6 y comprueba si y es mayor de 90 si es mayor, la variable anima se le da el valor 1 para seguir con otra animacion
        end
        if (anima==1)       //comprueba si anima es 1
            graph=graph+1;size=200;anima=2;     //si anima es 1 hara otra animacion a la anterior y pasara a anima=2 para no volver a repetirlo
        end
        if (anima==2)       //comprueba si anima es 2
            size=size-20;if (size<50) size=100;anima=3;end
        end
        if (anima==3)
            if (graph==11) graph=1;end      //comprueba si graph es 11 para cambiarlo a 1
            if (graph==13) graph=2;end      //comprueba si graph es 13 para cambiarlo a 2
            if (graph==15) graph=3;end      //comprueba si graph es 15 para cambiarlo a 3
            if (graph==17) graph=4;end      //comprueba si graph es 17 para cambiarlo a 4
        end

    frame;
    end
end


//este process crea el parpadeo del Pulsa Intro

process pulsa_intro();
    begin
        x=160;      //posicion horizontal de pantalla
        y=200;      //posicion vertical de pantalla
        size=100;       //lo dejamos al tama¤o normal
        z=50;       //esto hace la profundidad para los 2D si un grafico debe de estar delante de otro, si es menor estara delante si es mayor detras

    loop        //el bucle hace que el parpadeo sea infinito
        graph=6;

    frame(300);     //este frame, hace que se mantenga un rato en color amarillo, si lo incrementas tardara mas en pasar a rojo
        graph=7;

    frame(300);     //este frame, hace que se mantenga un rato en color rojo, si lo incrementas tardara mas en pasar a amarillo
    end
end

//a este proceso no le paso ningun dato porque solo se va a mostrar una vez en todo el juego

process prota_demo();
    private;
        anima=0;    //variable para hacer saltos de animacion
    begin
        graph=22;   //esto es el para poner el grafico en pantalla
        x=340;      //posicion horizontal de pantalla
        y=160;      //posicion vertical de pantalla
        size=100;   //lo dejamos al tama¤o normal
        z=10;       //esto hace la profundidad para los 2D si un grafico debe de estar delante de otro, si es menor estara delante si es mayor detras

    loop
        if (anima==0)       //comprueba si anima es 0
            graph=graph+1;if (graph>25) graph=22;end        //incrementa graph+1 y comprueba si es mayor de 25 si es mayor inicia al grafico 22
            x=x-3;if (x<-20) x=-20;anima=1;graph=26;end     //resta x-3 y comprueba si es menor que -20 y cambia de graph y de anima
        end
        if (anima==1)       //comprueba si anima es 1
            graph=graph+1;if (graph>29) graph=26;end        //incrementa graph+1 y comprueba si es mayor de 29 si es mayor inicia al grafico 29
            x=x+3;if (x>340) x=340;anima=0;graph=22;end     //incrementa x+3 y comprueba si es mayor que 340 y cambia de graph y de anima
        end

    frame;
    end
end

//proceso para iniciar el juego, aqui se llamaran los procesos para las bolas y el protagonista

process juego();
    begin
        fade_off();         //quita la imagen con un fundido
        let_me_alone();     //borramos todo para cuando vuelvas aki otra vez, cuando te hayan matado por ejemplo
        fade_on();          //devuelve la imagen con otro fundido

        write_int(1,284,120,4,offset puntos);

        foto(8,282,120,100,0,0);        //llamada para crear imagen del marcador
        foto(9,120,224,100,80,0);       //llamada para crear imagen del suelo
        foto(31,160,120,100,100,0);     //llamada para crear imagen del fondo

        foto(19,284,84,100,10,0);      //llamada para crear imagen de las vidas
        foto(18,284,158,100,10,0);     //llamada para crear imagen de la barra de %juego

        controlador(0);     //llamamos a un proceso para controlar la barra de vidas
        controlador(1);     //llamamos a un proceso para controlar la barra de porcentaje

        prota();

        controlador_de_bolas();     //llamada para controlar la salida de las bolas

    loop
        if (key(_esc)) inicio();end     //si pulsas la tecla esc, saldremos del juego hacia la presentacion con la llamada a inicio()

    frame;
    end
end

//controlar la barra de porcentaje de juego y la barra de las vidas

process controlador(tipo);
    begin
        graph=32;       //este graph sera el que tapa la barra de porcentaje y de vidas

        if (tipo==0) y=84;end       //compruba si tipo es 0 para poner el graph 32 a la altura de la barra de vidas
        if (tipo==1) y=158;end      //compruba si tipo es 1 para poner el graph 32 a la altura de la barra de porcentaje

        size=100;       //le ponemos tama¤o normal
        z=5;        //la z de las vidas y de porcentaje es de 10 y la del grafico de marcador 0 este se pone entre ambos para que tape el de las vidas y el del porcentaje

    loop
        if (tipo==0)        //comprueba si tipo es 0 para poder tapar las vidas
            if (vidas==3) x=340;end     //si vidas es 3 o el protagonista tiene 3 vidas de juego, no tapara nada
            if (vidas==2) x=316;end     //si vidas es 2 posaremos el graph 32 encima de la ultima vida
            if (vidas==1) x=303;end     //si vidas es 1 posaremos el graph 32 encima de dos vidas mostrando solo 1 en pantalla
            if (vidas==0) x=284;end     //si vidas es 0 se taparan todas las vidas
        end
        if (tipo==1)        //comprueba si tipo es 1 para destapar la barra de porcentaje
            x=284+porcentaje;       //ponemos el graph 32 delante de la barra de %juego e ira destapandose a medida que incremente porcentaje
        end

    frame;
    end
end

//proceso para controlar protagonista

process prota();
    private;
        anima=1;        //variable para controlar los saltos de animacion
        pulsado=0;      //variable para controlar el disparo del protagonista y si no se pulsa ninguna tecla el protagonista se quedara con el grafico de parado
        inmune=0;       //variable para controlar que pases un rato de inmunidad

    begin
        graph=27;       //ponemos ese grafico de inicio
        x=120;          //centramos al protagonista en pantalla en la posicion horizontal
        y=212;          //ponemos al protagonista encima del suelo
        size=100;       //ponemos tama¤o normal
        z=20;           //ponemos profundidad de 20, delante del fondo y del suelo

    loop
        if (pulsado<3)      //comprueba si pulsado es menor de 3
            if (key(_left) and x>5)     //si pulsado es menor de 3 y pulsas el cursor a la izquierda siempre que la posicion x del personaje sea mayor de 5
                x=x-3;pulsado=2;        //se disminuira la x-3 y pondremos pulsado a 2
                if (anima==1) graph=22;anima=0;end      //comprueba si anima es 1 para darle un grafico inicial y cambiarlo a anima 0
            end
            if (key(_right) and x<240)      //si pulsado es menor de 3 y pulsas el cursor a la izquierda siempre que la posicion x del personaje sea mayor de 5
                x=x+3;pulsado=2;            //se incrementara la x+3 y pondremos pulsado a 2
                if (anima==0) graph=26;anima=1;end      //comprueba si anima es 1 para darle un grafico inicial y cambiarlo a anima 1
            end
        end

        if (key(_b) or key(_n) or key(_m)) pulsado=5;graph=21;end       //si pulsamos las teclas b, n, m pondremos pulsado a 5 y el grafico del prota mirando arriba

        pulsado=pulsado-1;      //restamos a pulsando-1
        if (pulsado==0 and anima==0) graph=23;end       //si ha llegado aki el programa quiere decir que no se ha pulsado ninguna tecla de control y pondra el grafico del protagonista quieto
        if (pulsado==0 and anima==1) graph=27;end       //aki lo mismo que el de arriba pero mirando el protagonista a la derecha
        if (pulsado>0 and pulsado<3)        //controla si pulsado es mayor de 0 y menor de 3
            if (graph<>21) graph=graph+1;end        //si es mayor de 0 y menor de 3 y el graph es distinto a 21 incrementara graph+1
            if (graph==21 and anima==0) graph=22;end    //esto es para que una vez haya disparado el prota mire a la izquierda, aunque es mas comodo con los flags
            if (graph==21 and anima==1) graph=26;end    //esto es para que una vez haya disparado el prota mire a la derecha, aunque es mas comodo con los flags
            if (anima==0 and graph>25) graph=22;end     //comprueba que el prota esta andando para la izquierda y si es mayor el grafico a 25 volverlo a poner desde el grafico 22
            if (anima==1 and graph>29) graph=26;end     //comprueba que el prota esta andando para la derecha y si es mayor el grafico a 29 volverlo a poner desde el grafico 26
        end

        //las funciones que he hecho aki con anima, se podria hacer mas comodo con flags=1 o flags=0, pero el protagonista lleva el arma apoyada al brazo derecho y son distintos graficos

        if (pulsado==3) disparo(x+6,y-5,size);end       //esto comprueba si pulsado es 3 y hace una llamada al disparo, usease es para cuando una vez hayas pulsado b, n, m y lo sueltes el prota dispare

        inmune=inmune+1;        //incrementa inmune+1
        if (inmune<60) flags=4;end      //comprueba si inmune es menor de 50, si es menor pone al protagonista trasparente para que se vea que es inmune
        if (inmune=>60)     //comprueba si inmune es mayor o igual de 50, si es mayor pondremos la colision para que el protagonista sea vulnerable
            flags=0;        //si es mayor o igual le quita la trasparencia al protagonista y lo pone normal
            if (collision(type bola))       //comprueba si te toca una bola
                vidas=vidas-1;inmune=0;     //si te ha tocado resta vidas-1 y pone inmune a 0, para volver al protagonista inmunerable por unos instantes
                if (vidas<=-1) inicio();end     //comprueba si vidas es menor o igual a -1, si es igual o menor a -1 quiere decir que te han quitado todas las vidas y volveras a inicio del programa con la llamada inicio()
            end
        end

    frame;
    end
end

//proceso de disparo del prota le pasamos los datos x, y, size del process prota

process disparo(x,y,size);
    begin
        graph=20;   //le ponemos el grafico
        z=25;       //le ponemos la profundidad, que este detras del prota
        flags=0;    //le ponemos flags normal sin espejado

    loop
        y=y-6;if (y<-20) break;end      //va incrementado y-6 si no ha tocado ninguna bola saldra de la pantalla y borramos el proceso con break
        flags=flags+1;if (flags>1) flags=0;end      //esto vale para hacer la animacion del disparo cambiando constantemente el flags de 0 a 1

    frame;
    end
end

//proceso que va llamando a los procesos bola aleatoriamente

process controlador_de_bolas();
    private;
        grafico_bola=0;     //variable para hacer el rand del grafico de la bola
        posicion_x=0;       //variable para la posicion x de la bola
        tamano_bola=0;      //variable para hacer el rand del tama¤o de la bola
    begin

    frame(4500);    //cuando inicia el juego esperamos un ratito antes de que salga las bolas

    loop
        grafico_bola=rand(0,3);     //hacemos un rando para que saque uno de los cuantro graficos de la bola
        if (grafico_bola==0) grafico_bola=10;end
        if (grafico_bola==1) grafico_bola=12;end
        if (grafico_bola==2) grafico_bola=14;end
        if (grafico_bola==3) grafico_bola=16;end

        posicion_x=rand(20,220);      //hacemos un rando para que la bola salga en la posicion x, cuando esta la bola arriba, aleatoria

        tamano_bola=rand(0,2);      //otro random para los 3 tama¤os de la bola size=100, size=50 y size=25
        if (tamano_bola==0) tamano_bola=100;end
        if (tamano_bola==1) tamano_bola=50;end
        if (tamano_bola==2) tamano_bola=25;end

        bola(grafico_bola,posicion_x,-20,tamano_bola,4,0,0);

        //los datos que mandamos al proceso bola son bola(graph,x,y,size,flags,anima_x,anima_y);

    frame(7000);
    end
end


//proceso para las bolas

process bola(graph,x,y,size,flags,anima_x,anima_y);
    private;
        velocidad=0;    //variable para controlar la velocidad de subida y bajada de la bola
        id_disp;        //variable para identificar si colisiona con el disparo del prota

    begin
        z=30;

    loop
        //comprueba si la bola tiene distintos graph para que hagan distintos movimientos en pantalla

        if (graph==10)      //comprueba si el graph del proceso es 10
            if (anima_x==0 and anima_y==0)      //si anima_x & anima_y es 0
                y=y+1;      //saldra la bola en la parte superior de la pantalla bajando a 1
                if (y=>30)      //comprueba si y es mayor o igual de 30
                    anima_x=rand(1,2);anima_y=1;flags=0;    //y hace un rand para que vaya saltado hacia la izquierda o hacia la derecha
                end
            end
            if (anima_x==1)
                x=x-3;
                if (x<10) x=10;anima_x=2;end
            end
            if (anima_x==2)
                x=x+3;
                if (x>230) x=230;anima_x=1;end
            end
            if (anima_y==1)
                velocidad=velocidad+1;y=y+velocidad;
                if (velocidad>12) velocidad=12;end
                if (y>222) y=222;anima_y=2;end
            end
            if (anima_y==2)
                y=y-velocidad;velocidad=velocidad-1;
                if (velocidad<0) velocidad=0;anima_y=1;end
            end
        end

        if (graph==12)      //comprueba si el graph del proceso es 12
            if (anima_x==0 and anima_y==0)      //si anima_x & anima_y es 0
                y=y+1;      //saldra la bola en la parte superior de la pantalla bajando a 1
                if (y=>30)      //comprueba si y es mayor o igual de 30
                    anima_x=rand(1,2);anima_y=1;flags=0;    //y hace un rand para que vaya saltado hacia la izquierda o hacia la derecha
                end
            end
            if (anima_x==1)
                x=x-1;
                if (x<10) x=10;anima_x=2;end
            end
            if (anima_x==2)
                x=x+1;
                if (x>230) x=230;anima_x=1;end
            end
            if (anima_y==1)
                velocidad=velocidad+1;y=y+velocidad;
                if (velocidad>16) velocidad=16;end
                if (y>222) y=222;anima_y=2;end
            end
            if (anima_y==2)
                y=y-velocidad;velocidad=velocidad-1;
                if (velocidad<0) velocidad=0;anima_y=1;end
            end
        end

        if (graph==14)      //comprueba si el graph del proceso es 14
            if (anima_x==0 and anima_y==0)      //si anima_x & anima_y es 0
                y=y+1;      //saldra la bola en la parte superior de la pantalla bajando a 1
                if (y=>30)      //comprueba si y es mayor o igual de 30
                    anima_x=rand(1,2);anima_y=1;flags=0;    //y hace un rand para que vaya saltado hacia la izquierda o hacia la derecha
                end
            end
            if (anima_x==1)
                x=x-2;
                if (x<10) x=10;anima_x=2;end
            end
            if (anima_x==2)
                x=x+2;
                if (x>230) x=230;anima_x=1;end
            end
            if (anima_y==1)
                y=y+2;
                if (y>222) y=222;anima_y=2;end
            end
            if (anima_y==2)
                y=y-2;
                if (y<20) y=20;anima_y=1;end
            end
        end

        if (graph==16)      //comprueba si el graph del proceso es 16
            if (anima_x==0 and anima_y==0)      //si anima_x & anima_y es 0
                y=y+1;      //saldra la bola en la parte superior de la pantalla bajando a 1
                if (y=>30)      //comprueba si y es mayor o igual de 30
                    anima_x=rand(1,2);anima_y=1;flags=0;    //y hace un rand para que vaya saltado hacia la izquierda o hacia la derecha
                end
            end
            if (anima_x==1)
                x=x-4;
                if (x<10) x=10;anima_x=2;end
            end
            if (anima_x==2)
                x=x+4;
                if (x>230) x=230;anima_x=1;end
            end
            if (anima_y==1)
                y=y+4;
                if (y>222) y=222;anima_y=2;end
            end
            if (anima_y==2)
                y=y-4;
                if (y<20) y=20;anima_y=1;end
            end
        end

        //ahora toca lo de la colision a las bolas con el proceso disparo

        if (anima_x<>0 and anima_y<>0)      //comprueba que anima_x y anima_y sean distintos a 0
            if ((id_disp=collision(type disparo)))      //comprueba si la bola a colisionado con el proceso disparo
                signal(id_disp,s_kill);     //si ha colisionado con el disparo, borra el proceso disparo con un signal
                explota(graph+1,x,y,size+100,25);       //y hacemos que la bola llame al proceso explosion para que haga la explosion de la bola
                porcentaje=porcentaje+1;    //incrementamos el porcentaje +1
                if (size==100)      //comprueba si la bola era de tama¤o normal
                    puntos=puntos+25;       //si era del tama¤o normal te daran 25 puntos
                    bola(graph,x-10,y-10,50,0,1,2);     //y hacemos dos llamadas para hacer que salten dos bolas a mitad de tama¤o
                    bola(graph,x+10,y-10,50,0,2,2);
                    break;          //eliminamos el proceso de la bola que ha explotao
                end
                if (size==50)       //comprueba si la bola era la mitad del tama¤o normal
                    puntos=puntos+50;       //si era la mitad del tama¤o te daran 50 puntos
                    bola(graph,x-10,y-10,25,0,1,2);     //y hacemos dos llamadas para hacer que salten dos bolas a mitad de tama¤o
                    bola(graph,x+10,y-10,25,0,2,2);
                    break;          //eliminamos el proceso de la bola que ha explotao
                end
                if (size==25)       //comprueba si el tama¤o de la bola era la mas peque¤a
                    puntos=puntos+100;      //te incrementara 100 puntos
                    break;          //eliminamos el proceso de la bola que ha explotao, sin hacer mas llamadas
                end
            end
        end

    frame;
    end
end

//proceso para la explosion de las bolas, le pasa los datos del proceso bola

process explota(graph,x,y,size,z);
    begin
        flags=0;    //pone la bola normal sin trasparencias y sin espejados

    loop
        size=size-20;       //disminuye el tama¤o con size a -20
        if (size<100) flags=4;end   //comprueba si size es menor de 100 para poner la explosion trasparente
        if (size<10) break;end      //comprueba si size es menor de 10 para quitar el proceso

    frame;
    end
end

//gracias por habernos visitado.  http://www.amakasoft.net - http://www.amaksoft.com
//proyecto Fenix 1.0. http://fenix.divsite.net

