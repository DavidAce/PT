close all
clear all;
iterations = 25;
bootstraps = 50;
for i = 4:4
    E{i+1}    = load(['../output/default/timeseries/E' num2str(i) '.dat']);
    M{i+1}    = load(['../output/default/timeseries/M' num2str(i) '.dat']);
%     E{i+1}    = E{i+1}(1:100000);
    figure(1)
    plot(autocorr(E{i+1}),'DisplayName', [num2str(i+1)]),hold all
    figure(2)
    [sigma, sigma_vec, sigma_sigma_vec, num, tau, block_size] = dataerror(E{i+1}/16^2);
    errorbar(1:length(sigma_vec),sigma_vec,sigma_sigma_vec, 'r'),hold all;
    h = refline(0,sigma);
    h.Color = [1 0 0];
    h.DisplayName = ['Flyvbjerg'];

    time = zeros(8,1);
    for j = 1:iterations
        block_length = 2*ceil(tau);
        tstart = tic;
        [~, ~, boot1_std(j)] = bootstrap1(E{i+1}, block_length, bootstraps);
        time(1) = time(1) + toc(tstart);
                
        block_length = 20*ceil(tau);
        tstart = tic;
        [~, ~, boot12_std(j)] = bootstrap1(E{i+1}, block_length, bootstraps);
        time(2) = time(2) + toc(tstart);

        block_length = 2*ceil(tau);
        tstart = tic;
        [~, ~, boot2_std(j)] = bootstrap2(E{i+1}, block_length, bootstraps);
        time(3) = time(3) + toc(tstart);
        
        block_length = 20*ceil(tau);
        tstart = tic;
        [~, ~, boot22_std(j)] = bootstrap2(E{i+1}, block_length, bootstraps);
        time(4) = time(4) + toc(tstart);
        
        block_length = 2*ceil(tau);
        tstart = tic;
        [~, ~, boot_my1_std(j)] = my_bootstrap(E{i+1}, block_length, bootstraps);
        time(5) = time(5) + toc(tstart);
        
        block_length = 20*ceil(tau);
        tstart = tic;
        [~, ~, boot_my12_std(j)] = my_bootstrap(E{i+1}, block_length, bootstraps);
        time(6) = time(6) + toc(tstart);
                
        block_length = 2*ceil(tau);
        tstart = tic;
        [~,  ~, boot_my2_std(j)] = my_bootstrap2(E{i+1}, block_length, bootstraps);
        time(7) = time(7) + toc(tstart);
                
        block_length = 20*ceil(tau);
        tstart = tic;
        [~,  ~, boot_my22_std(j)] = my_bootstrap2(E{i+1}, block_length, bootstraps);
        time(8) = time(8) + toc(tstart);
    end
    time = time./max(time);
    
    x_data = (1:3)';
    y_data = ones(length(x_data),1) * mean(boot1_std);
    e_data = ones(length(x_data),1) * sqrt(mean(boot1_std.^2) - mean(boot1_std)^2);
    h1 = shadedErrorBar(x_data,y_data,e_data,'-',1);hold all;
    h1.mainLine.DisplayName = ['Non-overlapping block bootstrap | ' num2str(2) '\tau | Rel time:  ' num2str(time(1))];
    h1.mainLine.LineStyle =  ':';
    h1.mainLine.LineWidth =  1.5;
    
    x_data = (1:3)';
    y_data = ones(length(x_data),1) * mean(boot12_std);
    e_data = ones(length(x_data),1) * sqrt(mean(boot12_std.^2) - mean(boot12_std)^2);
    h12 = shadedErrorBar(x_data,y_data,e_data,'-',1);hold all;
    h12.mainLine.DisplayName = ['Non-overlapping block bootstrap | ' num2str(20) '\tau | Rel time:  ' num2str(time(2))];
    h12.mainLine.LineStyle =  ':';
    h12.mainLine.LineWidth =  1.5;
    

    
    x_data = (4:6)';
    y_data = ones(length(x_data),1) * mean(boot2_std);
    e_data = ones(length(x_data),1) * sqrt(mean(boot2_std.^2) - mean(boot2_std)^2);
    h2 = shadedErrorBar(x_data,y_data,e_data,'-',1);hold all;
    h2.mainLine.DisplayName = ['Overlapping block bootstrap | ' num2str(2) '\tau| Rel time:  ' num2str(time(3))];
    h2.mainLine.LineStyle =  '-';
    h2.mainLine.LineWidth =  1.5;
    
    x_data = (4:6)';
    y_data = ones(length(x_data),1) * mean(boot22_std);
    e_data = ones(length(x_data),1) * sqrt(mean(boot22_std.^2) - mean(boot22_std)^2);
    h22 = shadedErrorBar(x_data,y_data,e_data,'-',1);hold all;
    h22.mainLine.DisplayName = ['Overlapping block bootstrap | ' num2str(20) '\tau | Rel time:  ' num2str(time(4))];
    h22.mainLine.LineStyle =  '-';
    h22.mainLine.LineWidth =  1.5;
    

    x_data = (7:9)';
    y_data = ones(length(x_data),1) * mean(boot_my1_std);
    e_data = ones(length(x_data),1) * sqrt(mean(boot_my1_std.^2) - mean(boot_my1_std)^2);
    h3 = shadedErrorBar(x_data,y_data,e_data,'-',1);hold all;
    h3.mainLine.DisplayName = ['Random lag bootstrap | ' num2str(2) '\tau | Rel time:  ' num2str(time(5))];
    h3.mainLine.LineStyle =  '--';
    h3.mainLine.LineWidth =  1.5;
    
    x_data = (7:9)';
    y_data = ones(length(x_data),1) * mean(boot_my12_std);
    e_data = ones(length(x_data),1) * sqrt(mean(boot_my12_std.^2) - mean(boot_my12_std)^2);
    h32 = shadedErrorBar(x_data,y_data,e_data,'-',1);hold all;
    h32.mainLine.DisplayName = ['Random lag bootstrap | ' num2str(20) '\tau | Rel time:  ' num2str(time(6))];
    h32.mainLine.LineStyle =  '--';
    h32.mainLine.LineWidth =  1.5;
    
    
    x_data = (10:12)';
    y_data = ones(length(x_data),1) * mean(boot_my2_std);
    e_data = ones(length(x_data),1) * sqrt(mean(boot_my2_std.^2) - mean(boot_my2_std)^2);
    h4 = shadedErrorBar(x_data,y_data,e_data,'-',1);hold all;
    h4.mainLine.DisplayName = ['Const. lag bootstrap | ' num2str(2) '\tau | Rel time:  ' num2str(time(7))];
    h4.mainLine.LineStyle =  '-.';    
    h4.mainLine.LineWidth =  1.5;
    
    x_data = (10:12)';
    y_data = ones(length(x_data),1) * mean(boot_my22_std);
    e_data = ones(length(x_data),1) * sqrt(mean(boot_my22_std.^2) - mean(boot_my22_std)^2);
    h42 = shadedErrorBar(x_data,y_data,e_data,'-',1);hold all;
    h42.mainLine.DisplayName = ['Const. lag bootstrap | ' num2str(20) '\tau | Rel time:  ' num2str(time(8))];
    h42.mainLine.LineStyle =  '-.';    
    h42.mainLine.LineWidth =  1.5;
end
figure(2)
%%
title(['Data size = ' num2str(length(E{i+1})) ' | \tau = ' num2str(tau) ' | Boots. = ' num2str(bootstraps) ' | Avgs: ' num2str(iterations)]);
legend([h, h1.mainLine, h12.mainLine, h2.mainLine , h22.mainLine  , h3.mainLine,h32.mainLine, h4.mainLine, h42.mainLine])



