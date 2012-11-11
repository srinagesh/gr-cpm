%-------------------------------------------------------------------------
%This function returns the basis decomposition of cpm signals and the eigen
%values.
%usage:[U,S,Sx]=basisdecomp(q_n,Q,L,M,K,P)
%q_n=row vector of Q*L samples of q(t)(integral of the pulse g(t))
%Q=number of samples of q_n per time period
%L=size of memory
%M=input symbol stream size when input x belongs to(1,2...M)
%K=numerator of modulation index
%P=denominator of modulation index
%I have tried to use almost the same notation as in the CPM write up
%-------------------------------------------------------------------------

function [U,S,Sx]=basisdecomp(q_n,Q,L,M,K,P)
%compute required parameters
Ml=(M^L)*P;                 %the entire size of the input and state
h=K/P;                      %modulation index
q_array=zeros(L,Q);         %an array of all shifts of q_n
u=zeros(1,L);               %array to have u_n to u_(n-L) of the inputs
unitrowvector=ones(1,L);    %unit vector for computation of w(n)
psi_array=zeros(Q,Ml);      %Sx=exp(j*psi_array), Sx has to be decomposed

%obtain q_array
for i=1:Q
    for j=1:L
    q_array(j,i)=q_n(i+(j-1)*Q);
    end
end

%obtaining w(n) samples
pre_w=zeros(1,L);
for i=1:Q
    pre_w(i)=(pi*h*(M-1)*(i-1)/Q);
end

w=pre_w - 2*pi*h*(M-1)*unitrowvector*q_array + pi*h*(L-1)*(M-1);

%compute psi_array, which can be used to find Sx
for i=1:Ml
        v_n=mod(i-1,P);
        d=floor((i-1)/P);
        for k=0:L-1
            u(L-k)=mod(d,M);
            d=floor(d/M);
        end
        
        psi=2*pi*h*v_n+(4*pi*h)*u*q_array+w;
        psi_array(:,i)=psi';
      
end

Sx=exp(1i*psi_array);
%singular value decomposition
[U,S,V]=svd(Sx);
end
