// N = 9000

include "Shootout.inc";

private 
	char marks[8192];
	int i, k, n, count;
begin
	n = argv[1];
	if(!n) n = 900; end
	Shootout ("Sieve of Erastotenes", "N = "+n);
	frame;

	BeginShootout();
	while (n-- > 0)
		from i = 2 to 8192:
			marks[i] = 1;
		end
		from i = 2 to 8192:
			if (marks[i] > 0)
				k = i+i;
				while (k <= 8192)
					marks[k] = 0;
					k += i;
				end
				count++;
			end
		end
	end
	EndShootout ("Count = " + count);
end


