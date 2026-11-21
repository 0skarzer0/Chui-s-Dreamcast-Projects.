PROGRAM test;


IMPORT "mixer.dll"

GLOBAL

fichero;
fichero2;
canal;
playing;

float volumen=1.0;
float frecuencia=44100;


private


begin
     SOUND_MODE=1;
     SOUND_FREQ=44100;
     frame;
     frame;
     frame;

     mixer_init();
     
     fichero=mixer_load_wav("game.wav");
     //fichero=mixer_load_wav("arabe.ogg");
     //fichero=mixer_load_wav("ff5title.it");

     canal=mixer_play_wav(fichero);

     write_int (0,0,0,0,&playing);

     repeat
        playing=mixer_is_playing_wav(canal);
        if (key(_1)) 
                mixer_play_wav(fichero2);
                repeat frame; until (key(_1)==false);
        end;
        if (key(_q))
                volumen=volumen-0.1;
                mixer_set_wav_volume(canal,volumen);
                repeat frame; until (key(_q)==false);
        end;
        if (key(_w))
                volumen=volumen+0.1;
                mixer_set_wav_volume(canal,volumen);
                repeat frame; until (key(_w)==false);
        end;
        if (key(_z))
                frecuencia=frecuencia-100;
                mixer_set_wav_freq(canal,frecuencia);
                
        end;
        if (key(_x))
                frecuencia=frecuencia+100;
                mixer_set_wav_freq(canal,frecuencia);
                
        end;

        if (key(_u))
                mixer_unload_wav(fichero2);
                repeat frame; until (key(_u)==false);
        end;
        if (key(_p))
                mixer_pause_wav(canal);
                repeat frame; until (key(_p)==false);
        end;
        if (key(_r))
                mixer_resume_wav(canal);
                repeat frame; until (key(_r)==false);
        end;
        if (key(_s))
                mixer_stop_wav(canal);
                repeat frame; until (key(_s)==false);
        end;
        if (key(_m))
                mixer_rewind_wav(canal);
                repeat frame; until (key(_m)==false);
        end;


        frame;
     until (key(_esc));

end;
