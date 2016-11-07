function [ error, block_error , block_error_error, chosen, tau, block_size] = dataerror( A )
len = length(A);
maxBlockingSteps = floor(log(length(A))/log(2)-1);
for i = 1:maxBlockingSteps;
    block_error(i)       = sqrt(var(A)/(len-1));
%     block_error_error(i) = block_error(i) / sqrt(2*(len-1));
    block_error_error(i) = sqrt(block_error(i)^2 * sqrt(2/(len-1)));
    [A,len] = transform(A);
    block_size(i) = 2^i; 
end
block_error = block_error';
block_error_error = block_error_error';
[val,chosen] = max([block_error-block_error_error]);
error = block_error(chosen);
tau   = max(1,0.5 * error^2 / block_error(1)^2);
end

%     return 0.5*pow(flyvbjerg(A),2) / (variance(A)/(A.size()-1));


