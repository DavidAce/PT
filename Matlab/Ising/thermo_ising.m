function [ u,c ] = thermo_ising( T, L ) %
%Take a real number T, integer L and give U.
h = 1e-4;
fph  = logZ_ising(T+h,L);
fp2h = logZ_ising(T+2*h,L);
f    = logZ_ising(T,L);
u   = -2*L^2*sign(T) + T.^2.*(fph - f )./h;
u = u/L^2;
c = 2*T.*(fph - f)/h/L^2 + T.^2.*(fp2h - 2*fph + f)/h^2/L^2;

end

