u    = load(['../output/default/u.dat']);
m    = load(['../output/default/m.dat']);
c    = load(['../output/default/c.dat']);
c_WL    = load(['../../WL/outdata/final/c.dat']);

x    = load(['../output/default/x.dat']);
u_std= load(['../output/default/u_std.dat']);
m_std= load(['../output/default/m_std.dat']);
c_std= load(['../output/default/c_std.dat']);
x_std= load(['../output/default/x_std.dat']);
T    = load(['../output/default/T.dat']);
T_WL    = load(['../../WL/outdata/final/T.dat']);

addpath('Ising');
L = 12;
T_i = (min(T):(max(T)-min(T))/200:max(T))';
[u_i,c_i] = thermo_ising(T_i,L);


figure(1)
subplot(2,2,1)
errorbar(T,u,u_std, '.','DisplayName','PT'),hold all;
plot(T_i, u_i,'r','DisplayName','Exact')
title('Internal energy')

subplot(2,2,2)
errorbar(T,m,m_std, '.b');

subplot(2,2,3)
errorbar(T,c,c_std, '.b'),hold all
plot(T_i, c_i,'r')
plot(T_WL, c_WL,'g')
title('Specific heat')

subplot(2,2,4)
errorbar(T,x,x_std, '.b');