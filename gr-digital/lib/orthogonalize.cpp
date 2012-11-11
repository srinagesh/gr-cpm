#include <iostream>
#include <string.h>
#include <math.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_matrix_double.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_eigen.h>
using namespace std;

/*

For a signal matrix Sx (QxM) you want to decompose it into
Sx = F S (with F QxN  and S NxM), where F^H F = I.
For this, we form the product
Sx Sx^H 
and do its eigen decomposition into
Sx Sx^H = U L U^H
and identify U (or part of it by dropping some columns) with F
*/

inline int orthogonalize(gsl_matrix_complex* Sx, int Q, int M, double p,gsl_matrix_complex* &F, double &E, int &nvec,gsl_matrix_complex* &S_f)
{
	gsl_matrix_complex* Sxh, *C, *Fh, *C1;
	gsl_complex one,zero,z;
	gsl_vector* S;
	gsl_matrix_complex* U;
	gsl_eigen_hermv_workspace* w;
	double e,er,frac;
	int vcount;

	Sxh=gsl_matrix_complex_alloc(Q,M);
	C=gsl_matrix_complex_alloc(Q,Q);
	U=gsl_matrix_complex_alloc(Q,Q);
	S=gsl_vector_alloc(Q);
	w=gsl_eigen_hermv_alloc(Q); 

	gsl_matrix_complex_memcpy(Sxh,Sx);
	one=gsl_complex_rect(1.0,0.0);
	zero=gsl_complex_rect(0.0,0.0);
	gsl_matrix_complex_set_zero(C);
    // evaluate C := 1 Sxh Sxh^* + 0 C
	gsl_blas_zgemm(CblasNoTrans,CblasConjTrans,one,Sx,Sxh,zero,C); 

	gsl_eigen_hermv(C,S,U,w);
	gsl_eigen_hermv_free(w);
	gsl_eigen_hermv_sort(S,U,GSL_EIGEN_SORT_VAL_DESC);
	//Transpose U. gsl works with column vectors.
	//gsl_matrix_complex_transpose(U);
	
	e=0;
	for(int i=0;i<Q;i++)
	{
		e=e+gsl_vector_get(S,i);
	}

	//compute the number of vectors for suboptimal decoding
	er=0.0;
	vcount=0;
	for(int i=0;i<Q;i++)
	{
		frac = er/e;
		if(frac<p)
		{
			er+=gsl_vector_get(S,i);
			vcount++;
		}
	}


	//imp:allocate space for F

	F=gsl_matrix_complex_alloc(Q,vcount);

	for(int i=0;i<Q;i++)
	{
		for(int j=0;j<vcount;j++)
		{
			z=gsl_matrix_complex_get(U,i,j);
			gsl_matrix_complex_set(F,i,j,z);
		}
	}

	E=er/e;
	nvec=vcount;
	
	S_f = gsl_matrix_complex_alloc(vcount,M);
	Fh = gsl_matrix_complex_alloc(Q,vcount);
	C1=gsl_matrix_complex_alloc(vcount,M);
	gsl_matrix_complex_memcpy(Fh,F);

	gsl_matrix_complex_set_zero(C1);
    // evaluate C := 1 Sxh Sxh^* + 0 C
	gsl_blas_zgemm(CblasConjTrans,CblasNoTrans,one,Fh,Sx,zero,C1);
	gsl_matrix_complex_memcpy(S_f,C1);

/*
	cout<<"U="<<endl;
	for(int i=0;i<Q;i++)
	{
		for(int j=0;j<Q;j++)
		{
			cout<<"\t"<<GSL_REAL(gsl_matrix_complex_get(U,i,j))<<" +i"<<GSL_IMAG(gsl_matrix_complex_get(U,i,j));
		}
		cout<<endl;
	}
*/
	gsl_matrix_complex_free(Sxh);
	gsl_matrix_complex_free(C);
	gsl_matrix_complex_free(C1);
	gsl_matrix_complex_free(Fh);
	gsl_matrix_complex_free(U);
	gsl_vector_free(S);

return 0;
}
