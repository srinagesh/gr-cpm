%function Sx=basisdecomp(q_n,Q,L,M,K,P);
L=2;
T=2;
M=2;
K=1;
P=2;
Ml=(M^L)*P;
Q=2;
R=Q;
h=K/P;
q_n=(1/(2*L*T))*ones(1,4);     %lrec
q_array=zeros(L,Q);
u=zeros(1,L);
unitrowvector=ones(1,L);
psi_array=zeros(Q,Ml);
for i=1:Q
    for j=1:L
    q_array(j,i)=q_n(i+(j-1)*Q);
    end
end
pre_w=zeros(1,L);
for i=1:Q
    pre_w(i)=(pi*h*(M-1)*i/T);
end

w=pre_w+2*pi*h*(M-1)*unitrowvector*q_array + pi*h*(L-1)*(M-1);

for i=1:Ml
        v_n=mod(i-1,P);
        sum=0;
        idash=floor((i-1)/P);
        for k=0:L-1
            pwr=M^(L-k);
            u(k+1)=floor((idash-sum)/pwr);
            sum=sum+pwr*u(k+1);
        end
        
        psi=2*pi*h*v_n+(4*pi*h)*u*q_array+w;
        psi_array(:,i)=psi';
      
end

Sx=exp(1i*psi_array);
[U,S,V]=svd(Sx);
