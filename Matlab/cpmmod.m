function [wave,Sx]=cpmmod(x,q_n,Q,L,M,K,P)
T=1;
Q=10;
[Sx,f0]=statewave(q_n,Q,L,M,K,P);
[F,E]=cpmdecomp(Sx,0.5);
L_x=length(x);
v_n=0;
un_nl=zeros(1,L);
wave=zeros(1,L_x*Q);

for i=1:L_x
    if(i>L)
        v_n=mod(v_n+x(i-L),P);
    end
    
    un_nl(1:min(i,L))=x(i:-1:max((i-L+1),1))';
    st=[un_nl,v_n];
    val=0;
    for j=0:L
        if(j<=1)
            val=val+st(L+1-j)*(P^j);
        else
            val=val+st(L+1-j)*(M^(j-1));
        end
    end
    
    wave((i-1)*Q+1:i*Q)=Sx(:,val+1)';
end