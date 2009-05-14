P_H = [	0.002572,
		0.003272,
		0.004152,
		0.005264,
		0.006191,
		0.007132,
		0.007981,
		0.010185,
		0.011575,
		0.012721,
		0.013885,
		0.015747,
		0.017382,
		0.018536,
		0.020072];
P_H2 = [0.001842,
		0.002535,
		0.003183,
		0.004235,
		0.004802,
		0.005888,
		0.006839,
		0.007326,
		0.009616,
		0.011630,
		0.012882,
		0.014522,
		0.015200,
		0.016530,
		0.017562];
P_H3 = [0.001158,
		0.001753,
		0.002379,
		0.003115,
		0.004085,
		0.004890,
		0.005525,
		0.006702,
		0.007741,
		0.009058,
		0.009826,
		0.010707,
		0.012685,
		0.014173,
		0.015519];
P_H4 = [0.000651,
		0.001127,
		0.001679,
		0.002407,
		0.003134,
		0.003882,
		0.004604,
		0.005468,
		0.006760,
		0.007733,
		0.009537,
		0.011051,
		0.012078,
		0.013313,
		0.014259];	
P_H5 = [0.000296,
		0.000642,
		0.001016,
		0.001691,
		0.002227,
		0.003029,
		0.003904,
		0.004545,
		0.005271,
		0.006713,
		0.007706,
		0.009041,
		0.010359,
		0.011009,
		0.013033];

for i=0:size(P_H,1)-1
	%r (i+1) = (0.06+i*0.01);
	%r2(i+1) = (0.05+i*0.01);
	%r3(i+1) = (0.04+i*0.01);
	%r4(i+1) = (0.03+i*0.01);
	%r5(i+1) = (0.02+i*0.01);
	
	p (i+1) = pi*(0.06+i*0.01)^2;
	p2(i+1) = pi*(0.05+i*0.01)^2;
	p3(i+1) = pi*(0.04+i*0.01)^2;
	p4(i+1) = pi*(0.03+i*0.01)^2;
	p5(i+1) = pi*(0.02+i*0.01)^2;	

	%P_H (i+1) = P_H (i+1)/p(i+1);
	%P_H2(i+1) = P_H2(i+1)/p(i+1);
	%P_H3(i+1) = P_H3(i+1)/p(i+1);
	%P_H4(i+1) = P_H4(i+1)/p(i+1);
	%P_H5(i+1) = P_H5(i+1)/p(i+1);
end;

clf;
hold all;
plot(p ,P_H, '-s');
plot(p2,P_H2, '-+');
plot(p3,P_H3, '-o');
plot(p4,P_H4, '-*');
plot(p5,P_H5, '-x');
%ylim([0,0.3]);
xlabel('Link probability (p)');
ylabel('P_H(u,v)');
legend('n = 300', 'n = 400', 'n = 500', 'n = 600', 'n = 700', 'Location', 'NorthWest');
set(gcf, 'Position', [0, 0, 600, 250]);
