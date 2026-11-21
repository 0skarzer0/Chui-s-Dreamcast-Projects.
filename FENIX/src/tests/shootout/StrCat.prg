// N = 40000

include "Shootout.inc";

private int n;
	string str;
begin
	n = argv[1];
	IF (n == 0) n = 40000; END
	Shootout ("STRCAT - Prueba de concatenación de cadenas", "N = "+n);
	frame;

	BeginShootout();
	while (n-- > 0)
		str += "Hello";
	end
	say (len(str));

	EndShootout ("Longitud final = " + len(str));
end
