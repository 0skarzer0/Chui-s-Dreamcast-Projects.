// N = 900000

include "Shootout.inc";

const
	im = 139968;
	ia = 3877;
	ic = 29573;
end

global
	last;
end

float gen_random(float max)
begin
	return ( max * (last = ((last * ia + ic) % im)) )/im;
end

private int n, m;
begin
	n = argv[1];
	if (!n) n = 900000; end
	Shootout ("Random Number Generator", "N = "+n);
	frame;

	BeginShootout();
	while (n-- > 0)
		gen_random(100.0);
	end
	EndShootout ("gen_random(100) = " + gen_random(100));
end
