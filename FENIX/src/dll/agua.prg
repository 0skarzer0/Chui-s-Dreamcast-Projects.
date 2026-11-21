program hello;
private
    import "agua"
begin
    full_screen=false;
    load_fpg("test.fpg"); // Load screen
    graph_mode = mode_16bits;
    set_mode (m640x480);
    mouse.graph = 100;

    map_xput (0, 0, 1, 320, 240, 0, 250, 0);

    rgb(255,255,255);
    write (0, 320, 350, 1, "Prueba del agua");
    rgb(192,192,192);
    write (0, 320, 360, 1, "El texto se refleja si el agua");
    write (0, 320, 370, 1, "se crea con una Z menor a -256");
    write (0, 320, 390, 1, "Pulsa Q/A para cambiar el nivel del agua");

    /* Usamos una coordenada Z como parámetro para el efecto de agua 
     * de manera que sólo se reflejará aquello que haya sido dibujado
     * antes que el efecto (es decir, procesos y otros objetos que
     * tengan una coordenada Z más grande). El texto por defecto se
     * dibuja con una coordenada Z de -256. 
     *
     * El segundo parámetro es el número de líneas
     */

    y = 60;
    fxagua (-512, 60);

    while (not key(_esc)) 
    	IF KEY(_q):
	    fxagua (-512, y++);
	END
    	IF KEY(_a):
	    fxagua (-512, y--);
	END
    	frame; 
    	end

end
