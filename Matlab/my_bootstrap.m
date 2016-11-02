function [ A_boot,A_avg, A_std ] = my_bootstrap( A, block_length, num_bootstraps )
        
    num_items = floor(length(A) / block_length);
    for j = 1:num_bootstraps
        boot = A(randi(length(A),num_items,1));
     	A_boot(j) = mean(boot)/16^2;
    end
    A_avg = mean(A_boot);
    A_std = sqrt(mean(A_boot.^2) - mean(A_boot)^2);

end