function [ A_boot,A_avg, A_std ] = bootstrap2( A, block_length, num_bootstraps )
    num_blocks = floor(length(A) / block_length);
    block = zeros(num_blocks,1);
    for j = 1:num_bootstraps
%         block = [];
        start = randi(length(A), num_blocks,1);
        for i = 1:num_blocks
            block(i) = mean(A(mod(start(i):start(i)+block_length - 1, length(A)) + 1));
        end
        A_boot(j) = mean(block)/16^2;

%         while  (length(block) < length(A) )
%             range = mod( randi(length(A)) + block_length - 1 , length(A)) + 1;
% %           start = randi(length(A));
%             B = A(mod((start : start + block_length) - 1 , length(A)) + 1);
%             block = cat(1, block,B);
%         end
    end
    
    A_avg = mean(A_boot);
    A_std = sqrt(mean(A_boot.^2) - mean(A_boot)^2);

end
