program effects;
private
    import "efects"
begin
    set_mode ( m640x480 ) ;

    load_fpg("test.fpg"); // Loads intro screen
    put_screen(0,1);      // Show bg

    fxripplereset();
    while ( 1 )
        fxripple();
        frame;
    end

end
