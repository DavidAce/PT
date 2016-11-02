function [ Ah,lenh ] = transform( A )
    lenh = length(A)/2;
    Atemp = [];
    for i = 1:lenh;
        Atemp(i) = 0.5*(A(2*i-1) + A(2*i));
    end
    Ah = Atemp;
end

