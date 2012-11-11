M=2;
L=2;
P=2;
Ml=(M^L)*P;
for i=1:Ml
        v_n=mod(i-1,P);
        d=floor((i-1)/P);
        sum=0;
        for k=0:L-1
            u(L-k)=mod(d,M);
            d=floor(d/M);
        end
        vec=[u,v_n]
end