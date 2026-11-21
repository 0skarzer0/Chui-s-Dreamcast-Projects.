program Benchmark;

global;
	testmap;
	testmap16;
	testmap8;
	scrwidth;
	scrheight;
	bpp;
	minx;
	miny;
	maxx;
	maxy;

process TestBlit(angle, size, flags);
private oldtimer, count, oldcount;
begin
	oldtimer = timer;
	scan_code = 0;
	while scan_code != _ESC:
		oldcount = count;
		from count = oldcount to oldcount+25000:
			map_xput (0, 0, testmap, rand(minx,maxx), rand(miny,maxy), angle, size, flags);
		end
		frame;
		if timer > oldtimer+100:
			delete_text(0);
			write (0, scrwidth/2, 16, 1, "Blits por segundo: " 
				+ count*100/(timer-oldtimer));
			count = 0;
			oldtimer = timer;
		end
	end
end

process TestLines();
private oldtimer, count, oldcount;
begin
	oldtimer = timer;
	scan_code = 0;
	drawing_map (0, 0);
	while scan_code != _ESC:
		oldcount = count;
		while count < oldcount+20000:
			drawing_color (rand(0,65535));
			draw_line (rand(minx,maxx), rand(miny,maxy),
				rand(minx,maxx), rand(miny,maxy));
			count++;
		end
		frame;
		if timer > oldtimer+100:
			delete_text(0);
			write (0, scrwidth/2, 16, 1, "Líneas por segundo: " 
				+ count*100/(timer-oldtimer));
			count = 0;
			oldtimer = timer;
		end
	end
end

process TestMaximumFPS();
begin
	while not key(_ESC): 
		delete_text(0);
		write (0, scrwidth/2, 16, 1, "FPS: " + fps);
		frame; 
	end
end

PROCESS FallingObject(graph, size, flags, angleinc)
PRIVATE xspeed, yspeed, inispeed, maxspeed ;
BEGIN
        z      = rand (-5, 125) ;
        x      = rand (15, scrwidth-15) ;
        y      = rand (20, scrheight-80) ;
        xspeed = rand (-10, 10) ;
        yspeed = rand (-4, 0) ;
        inispeed = 0;
        angle  = rand (0, 50 * angleinc) ;
        LOOP
                x += xspeed ;
                IF (x > scrwidth-15 || x < 15) xspeed = -xspeed; END
                y += yspeed++ ;
                IF (y > scrheight-60 AND yspeed > 0) 
			IF (yspeed > maxspeed) maxspeed = yspeed; END
			IF (yspeed < 4) yspeed = RAND(4, maxspeed); END
                        yspeed = -yspeed+2 ; 
                END
                IF (out_region(ID, 0)) 
			y = -rand(20, 100); 
			yspeed = 0;
		END
                angle += angleinc ;
                FRAME ;
        END
END
process TestFallingObjects()
private map, i;
BEGIN
	map = new_map (96, 96, bpp);
	map_xput (0, map, testmap, 48, 48, 0, 300, 0);
        FROM x = 0 TO 100: FallingObject(map, 100, 0, 0); END
	LOOP
		scan_code = 0;
		WHILE scan_code == 0: 
			delete_text(0);
			write (0, scrwidth/2, 8, 1, "Cien objetos botantes");
			write (0, scrwidth/2, 16, 1, "[T]ransparencia [C]olor-key");
			write (0, scrwidth/2, 32, 1, "FPS: " + fps);
			frame; 
		end
		if scan_code == _ESC: break; end
		if scan_code == _T:
		    while I = get_id(type FallingObject): i.flags ^= 4; end
		end
		if scan_code == _C:
		    while I = get_id(type FallingObject): i.flags ^= 128; end
		end
		if scan_code == _E:
		    while I = get_id(type FallingObject): i.flags ^= 1; end
		end
		if scan_code == _V:
		    while I = get_id(type FallingObject): i.flags ^= 2; end
		end
	end
        signal (TYPE FallingObject, S_KILL) ;
	unload_map (0, map);
END

process testblitter();
begin
	graph = testmap;
	x = scrwidth/2;
	y = scrheight/2;
	size = 100;
	angle = 0;
	loop

		scan_code = 0;
		while scan_code == 0: 
			delete_text(0);
			write (0, scrwidth/2, 8, 1, "Prueba de dibujo de un proceso");
			write (0, scrwidth/2, 16, 1, "Flags: "+flags+"   Size: "+size+"   Angle: "+angle);
			write (0, scrwidth/2, 32, 1, "FPS: " + fps + "   Speed: " + speed_gauge + "%");
			FRAME; 
		end
		switch (scan_code)
			case _ESC: return; end
			case _E: flags ^= 1; end
			case _V: flags ^= 2; end
			case _T: flags ^= 4; end
			case _C: flags ^= 128; end
			case _LEFT: angle -= 11250; end
			case _RIGHT: angle += 11250; end
		end
		switch (ascii)
			case ASC('+'): size *= 2; end
			case ASC('-'): size /= 2; end
		end
	end
end

global menuy;
process menuline(string line);
begin
	write (0, 16, menuy, 0, line);
	menuy += 10;
end

private changemode = true;
begin
	full_screen = false;
	set_fps(0, 0);
	scrwidth = 320;
	scrheight = 240;
	bpp = 8;
	graph_mode = mode_16bits;
	testmap16 = load_png("Check16.png");
	testmap8 = load_map("Check8.map");
	testmap = testmap8;

	loop
		if bpp == 8: 
			graph_mode = 0;
			testmap = testmap8;
		else
			graph_mode = mode_16bits;
			testmap = testmap16;
		end

		if changemode:
			switch (scrwidth)
				case 320:  set_mode (m320x240); end
				case 640:  set_mode (m640x480); end
				case 800:  set_mode (m800x600); end
				case 1024: set_mode (m1024x768); end
				default: say ("Scrwidth "+scrwidth); end
			end
			changemode = false;
			minx = 32;
			miny = 64;
			maxx = scrwidth - 32;
			maxy = scrheight - 32;
		end

		map_clear (0, 0, rgb(0,0,0));
		delete_text (0);
		menuy = 8;
		menuline ("Escoge una opción:");
		menuline ("");
		menuline ("1. Cambiar de resolución ("+scrwidth+"x"+scrheight+")");
		menuline ("2. Cambiar profundidad de color ("+bpp+" bpp)");
		menuline ("3. Cambiar entre ventana y pantalla completa");
		menuline ("");
		menuline ("A. Blitter sin color key");
		menuline ("B. Blitter normal");
		menuline ("C. Blitter con translucencia");
		menuline ("D. Blitter sin color key, con rotado/escalado");
		menuline ("E. Blitter con rotado/escalado");
		menuline ("F. Blitter con translucencia y rotado/escalado");
		menuline ("G. Dibujo de líneas");
		menuline ("W. Prueba de dibujo sencillo");
		menuline ("X. Test de los objetos botantes");
		menuline ("Y. Tasa máxima de FPS (con fondo)");
		menuline ("Z. Tasa máxima de FPS (sin fondo)");

		scan_code = 0;
		while scan_code == 0: frame; end; 
		delete_text (0);

		IF scan_code == _W or scan_code == _X or scan_code == _Y:
			map_clear (0, 0, rgb(64,64,64));
			drawing_map (0, 0);
			drawing_color (rgb(127,127,127));
			draw_rect(minx, miny, maxx, maxy);
			drawing_color (rgb(255,255,255));
			draw_rect(minx-2, miny-2, maxx+2, maxy+2);
		ELSE
			map_clear (0, 0, 0);
		END

		switch (scan_code):
			case _1:
				switch (scrwidth):
				case 320: scrwidth = 640; scrheight = 480; end
				case 640: scrwidth = 800; scrheight = 600; end
				case 800: scrwidth = 1024; scrheight = 768; end
				case 1024: scrwidth = 320; scrheight = 240; end
				end
				changemode = true;
			     end
			case _2:
				if bpp == 8: bpp = 16; else bpp = 8; end
				changemode = true;
			     end
			case _3:
				full_screen = !full_screen;
				changemode = true;
			     end
			case _A:
				TestBlit(0, 100, 128); 
				while exists(son): frame; end
			     end
			case _B:
				TestBlit(0, 100, 0); 
				while exists(son): frame; end
			     end
			case _C:
				TestBlit(0, 100, 4); 
				while exists(son): frame; end
			     end
			case _D:
				TestBlit(45000, 100, 128); 
				while exists(son): frame; end
			     end
			case _E:
				TestBlit(45000, 100, 0); 
				while exists(son): frame; end
			     end
			case _F:
				TestBlit(45000, 100, 4); 
				while exists(son): frame; end
			     end
			case _G:
				TestLines(); 
				while exists(son): frame; end
			     end
			case _W:
				TestBlitter();
				while exists(son): frame; end
			     end
			case _X:
				TestFallingObjects(); 
				while exists(son): frame; end
			     end
			case _Y:
				TestMaximumFPS(); 
				while exists(son): frame; end
			     end
			case _Z:
				TestMaximumFPS(); 
				while exists(son): frame; end
			     end
			case _ESC:
				exit(0,0);
			     end
		end
	end
end
