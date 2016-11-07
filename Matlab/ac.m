
E = load('../output/default/timeseries/E4.dat');
[acf,lags,bounds] = autocorr(E,1000);

plot(lags,acf)
idx = 1:50;
P = polyfit(lags(idx),-log(acf(idx)),1)

1/P(1)