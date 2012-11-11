L=2;
T=1;
M=2;
K=1;
P=2;
Ml=(M^L)*P;
Q=10;
R=Q/L;
h=K/P;
q_n=(1/(2*L*T))*ones(1,10);     %lrec
q_padded=[q_n,zeros(1,Q)];
q_array=zeros(L,Q);
vec=zeros(Ml,L+1);
u=zeros(1,L);
w=ones(1,Q);
psi_array=zeros(Q,Ml);
for i=1:Q
    q_array(:,i)=[q_padded(i); q_padded(i+R)];
end

for i=1:Ml
        v_n=mod(i-1,P);
        sum=0;
        for k=0:L-1
            pwr=M^(L-k);
            u(k+1)=floor((i-1-sum)/pwr);
            sum=sum+pwr*u(k+1);
        end
        
        psi=2*pi*h*v_n+(4*pi*h)*u*q_array+w;
        psi_array(:,i)=psi';
      
end

Sx=exp(1i*psi_array);
[U,S,V]=svd(Sx);
