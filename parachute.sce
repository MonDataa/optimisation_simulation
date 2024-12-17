// Script Scilab ajusté pour NRPB 0
deff('yp = f(t,y)', 'yp = - (k/m)*y^2 + g');
m = 100; // Masse ajustée
k = 10;
g = 9.81;
v0 = 200*1D3/3600;
t0 = 0;
t = 2;
v = ode(v0, t0, t, f);
v = v*3600/1D3; // Conversion en km/h
printf("%lf", v);
quit;
