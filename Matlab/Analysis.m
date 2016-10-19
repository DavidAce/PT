close all
clear all;
for i = 0:7
    E{i+1}     = load(['../output/default/timeseries/E' num2str(i) '.dat']);
    M{i+1}    = load(['../output/default/timeseries/M' num2str(i) '.dat']);
end
figure()
bar(cat(2,E{:})')