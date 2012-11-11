L=1;
T=1;
q_n=(0:0.25:0.75)/2;
[Sx,f0]=statewave(q_n,4,L,2,1,2);
[F,E]=cpmdecomp(Sx,0.5);
figure(1);
plot(real(Sx(:,3)))