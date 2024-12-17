deff('yp = f(t,y)', 'yp = - (k/m)*y^2 + g') ;
m = 100 ;
k = 10 ;
g = 9.81 ;
v0 = 200*1D3/3600 ;
t0 = 0 ;
t = 4.00000000 ;
v = ode(v0, t0, t, f) ;
v = v*3600/1D3 ;
printf("%lf",v)
quit
