clear all
close all
N = 16^2;

for i = 1:8
E     = load(['../output/default/timeseries/E' num2str(i-1) '.dat']);
c (i) = var(E)/2.26/2.26 / N;
end
T = linspace(0.1, 6, 8);
simps = 0;
s(1) = 0;
si(1) = 0;
for i = 2:length(c)
    k = 0;
    s(i) = s(i-1) + (T(i) - T(i-1))/6 * (c(i-1)/T(i-1) + 4 * (c(i-1) + c(i))/(T(i-1) + T(i)) + c(i)/T(i));
end
%Interpolation
j = 1;
for i = 2:length(s)
    k = 1;
    while k < 100
        ti(j) = k/100 * (T(i)-T(i-1)) + T(i-1);
        si(j) = k/100 * (s(i)-s(i-1)) + s(i-1);
        j = j+1;
        k = k+1;
    end
end
figure()
scatter(T,c),hold all
plot(T,s,'o'),hold all;
plot(ti,si);

%Now make a linearly spaced vector in s

s_linear = linspace(min(s), max(s), 8);

%Find the intersections of s_linear with s
t_new = T;
for i = 2:length(s_linear)-1
    [~, idx] = min(abs(si-s_linear(i)));
    t_new(i) = ti(idx);
end
for i = 1:length(t_new)
    refline(0, s_linear(i))
    line([t_new(i),t_new(i)],ylim), hold all
end

