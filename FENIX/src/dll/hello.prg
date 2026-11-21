program hello;
private
    import "hello"
    import "hello2"
begin

    set_mode(m320x200);

    hello();
    hello2();

    print( "Testing!" );
    while ( 1 )
        putstar();
        frame;
    end

end
