function [ A_boot,A_avg, A_std ] = bootstrap2( A, block_length, num_bootstraps )
    for j = 1:num_bootstraps
        block = [];
        while  (length(block) < length(A) )
            start = randi(length(A));
            B = A(mod((start : start + block_length) - 1 , length(A)) + 1);
            block = cat(1, block,B);
        end
     	A_boot(j) = mean(block)/16^2;
    end
    
    A_avg = mean(A_boot);
    A_std = sqrt(mean(A_boot.^2) - mean(A_boot)^2);

end
