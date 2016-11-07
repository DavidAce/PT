function [ Atemp,len ] = transform( A )
    len = floor(length(A)/2);
    Atemp = zeros(len,1);
    for i = 1:len;
        Atemp(i) = 0.5*(A(2*i-1) + A(2*i));
    end
end

