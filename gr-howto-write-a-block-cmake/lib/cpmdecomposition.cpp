#include<iostream>
#include <string.h>
#include <math.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_matrix_double.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_blas.h>
using namespace std;

inline int cpmdecomposition(gsl_vector* q, int Q, int L, int M, int K, int P,gsl_matrix_complex* &Sx, double &f_o)
{
	double Ml,h,t,t1,t2;
	gsl_matrix* q_array,*u,*unitrowvector,*psi_array,*w, *C, *Ct;
	int i,j,v_n;
	gsl_complex z,z1;
//	gsl_matrix_complex* Sx;

	Ml=pow(M,L)*P;
	h=((double)K)/((double)P);
	q_array = gsl_matrix_alloc(L,Q);
	u = gsl_matrix_alloc(1,L);
	psi_array = gsl_matrix_alloc(Q,int(Ml));
	w = gsl_matrix_alloc(1,Q);
	unitrowvector=gsl_matrix_alloc(1,L);
	C=gsl_matrix_alloc(1,Q);
	Ct=gsl_matrix_alloc(Q,1);
	Sx=gsl_matrix_complex_alloc(Q,(int)Ml);
	gsl_matrix_set_all(unitrowvector,1);

	//obtain q_array
	for(i=0;i<Q;i++)
	{
		for(j=0;j<=L-1;j++)
		{
			t=gsl_vector_get(q,(i+j*Q));
			gsl_matrix_set(q_array,j,i,t);
		}
	}

	//obtaining w(n) samples
	for(i=1;i<=Q;i++)
	{
		t=M_PI*h*(M-1)*(i-1)/Q;
		gsl_matrix_set(w,0,i-1,t);
	}

	gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,unitrowvector,q_array,0.0,C);
	gsl_matrix_scale(C,(2*M_PI*h*(M-1)));
	gsl_matrix_add_constant(C,-(M_PI*h*(L-1)*(M-1)));
	gsl_matrix_sub(w,C);

	cout<<Ml<<endl;

	//computing psi_array, which can be used to find Sx
	for(i=0;i<Ml;i++)
	{
		v_n=i%P;
		int d=(int)(i/P);
		for(int k=1;k<=L;k++)
		{
			t=d%M;
			gsl_matrix_set(u,0,L-k,(double)t);
		}

		gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,u,q_array,0.0,C);
		gsl_matrix_scale(C,4*M_PI*h);
		t1=4;

		gsl_matrix_add(C,w);
		gsl_matrix_add_constant(C,2*M_PI*h*v_n);
		gsl_matrix_transpose_memcpy(Ct,C);
		gsl_matrix_view a = gsl_matrix_submatrix(psi_array,0,i,Q,1);
		gsl_matrix_memcpy(&a.matrix,Ct);
	}

	for(i=0;i<Q;i++)
	{
		for(j=0;j<Ml;j++)
		{
			t=gsl_matrix_get(psi_array,i,j);
			z=gsl_complex_rect(0,t);
			z1=gsl_complex_exp(z);
			gsl_matrix_complex_set(Sx,i,j,z1);
		}
	}
	
	
	f_o = -h*(M-1)/2;



	return 0;

}

