close all
clear all;
for i = 3:3
    E{i+1}    = load(['../output/default/timeseries/E' num2str(i) '.dat']);
    M{i+1}    = load(['../output/default/timeseries/M' num2str(i) '.dat']);
    figure(1)
    plot(autocorr(E{i+1}),'DisplayName', [num2str(i+1)]),hold all
    figure(2)
    [sigma, sigma_vec, sigma_sigma_vec, num, tau, block_size] = dataerror(E{i+1}/16^2);
    errorbar(1:length(sigma_vec),sigma_vec,sigma_sigma_vec, 'DisplayName', ['Flyvbjerg']),hold all;
    block_length = 20*ceil(tau);
    [~, ~, boot_std] = bootstrap1(E{i+1}, block_length, 50);
    h1 = refline(0,boot_std);
    h1.DisplayName = ['block bootstrap1 | ' num2str(20) '\tau'];
    h1.LineStyle =  ':';
    
    block_length = 20*ceil(tau);
    [~, ~, boot_std] = bootstrap2(E{i+1}, block_length, 50);
    h2 = refline(0,boot_std);
    h2.DisplayName = ['block bootstrap2 | ' num2str(20) '\tau'];
    h2.LineStyle =  '-';


    block_length = 2*ceil(tau);
    [boot_data, boot_mean, boot_std] = my_bootstrap(E{i+1}, block_length, 50);
    h3 = refline(0,boot_std);
    h3.DisplayName = ['my bootstrap | ' num2str(2) '\tau'];
    h3.LineStyle =  '--';
    
end
figure(2)
legend('show')



