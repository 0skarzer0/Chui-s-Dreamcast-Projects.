
include "Shootout.inc";

int fibo(int n)
begin
	if (n < 2)
		return 1;
	else
		return fibo(n-2) + fibo(n-1);
	end
end

private int n;
begin
	n = argv[1];
	if (n == 0) n = 32; end
	Shootout ("Fibonacci - Serie de Fibonacci", "N = " + n);
	frame;

	BeginShootout();
	EndShootout ( "Fibonacci " + (n-1) + " = " + fibo(n) );
end
