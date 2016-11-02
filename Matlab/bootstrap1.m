function [ A_boot,A_avg, A_std ] = bootstrap1( A, tau, num_bootstraps )
        
    num_blocks = floor(length(A) / tau);
    for j = 1:num_bootstraps
        block = [];
        for i = 1:num_blocks
            block_number = randi(num_blocks) - 1;
            B = A(block_number*tau + 1: (block_number + 1) * tau);
            block = cat(1, block,B);
        end
     	A_boot(j) = mean(block)/16^2;
    end
    
    A_avg = mean(A_boot);
    A_std = sqrt(mean(A_boot.^2) - mean(A_boot)^2);

end
