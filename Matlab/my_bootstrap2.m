function [ A_boot,A_avg, A_std ] = my_bootstrap2( A, block_length, num_bootstraps )
    for j = 1:num_bootstraps
        start = 1 +  mod(j-1, block_length);
     	A_boot(j) = mean(A(start : block_length :length(A)))/16^2;
    end
    A_avg = mean(A_boot);
    A_std = sqrt(mean(A_boot.^2) - mean(A_boot)^2);

end