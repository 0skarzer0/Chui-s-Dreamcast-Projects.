include "Shootout.inc";

int Ackermann (int m, int n)
BEGIN
	IF (m == 0) return n+1; END
	IF (n == 0) return Ackermann(m-1, 1); END
	return Ackermann(m-1, Ackermann(m, n-1));
END

PRIVATE int n;
BEGIN
	n = argv[1];
	IF (n < 1) n = 1; END
	Shootout ("Ackermann's Function", "N = "+n);
	frame;

	BeginShootout();
	EndShootout ("Ackermann(3," + n + ") = " + Ackermann(3, n));
END

