// N = 7000

include "Shootout.inc";

private 
	int i, k, n;
	int pointer data;
	int pointer data2;
begin
	n = argv[1];
	IF (n == 0) n = 7000; END
	Shootout ("Array Access", "N = "+n);
	frame;

	BeginShootout();
	data = alloc(4*n);
	data2 = alloc(4*n);

	from i = 0 to n-1:
		data[i] = i+1;
		data2[i] = 0;
	end

	from k = 0 to 1000:
		from i = n-1 to 0 step -1:
			data2[i] += data[i];
		end
	end

	EndShootout ("data[0] = " + data[0]);
end

