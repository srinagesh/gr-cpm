#include <iostream>
#include <math.h>
#include <gsl/gsl_matrix.h>
#include "orthogonalize.cpp"
#include "cpmdecomposition.cpp"
using namespace std;

int main()
{
	gsl_vector* q;
	gsl_matrix_complex *Sx,*F,*S_f;
	//gsl_matrix_complex* S1=&Sx;
	int Q,L,M,K,P,i,j,t,Ml,nvec;
	double freq, E;
	FILE *file;

	file=fopen("cpmreadtest2.txt","a+");

	Q=2;
	L=2;
	M=2;
	K=1;
	P=2;

	Ml=(int)floor(pow(M,L)*P+0.5);
	q=gsl_vector_alloc(Q*L);
	Sx=gsl_matrix_complex_alloc(Q,Ml);

	for(i=0;i<Q*L;i++)
	{
		gsl_vector_set(q,i,(double(i)/(2.0*Q*L)));
	}
	cout<<"Calling CPM decomposition function"<<endl;

	t=cpmdecomposition(q,Q,L,M,K,P,Sx,freq);

	cout<<"Sx="<<endl;
	for(i=0;i<Q;i++)
	{
		for(j=0;j<Ml;j++)
		{
			cout<<"\t"<<GSL_REAL(gsl_matrix_complex_get(Sx,i,j))<<" +i"<<GSL_IMAG(gsl_matrix_complex_get(Sx,i,j));
		}
		cout<<endl;
	}

	cout<<"the operating frequency fo="<<freq<<endl;

    // this function is not CPM related...
    // it takes a signal set description and performs (approximate) orthogonalization
    	t=orthogonalize(Sx,Q,Ml,0.99,F,E,nvec,S_f);

	cout<<"F="<<endl;
	for(i=0;i<Q;i++)
	{
		for(j=0;j<nvec;j++)
		{
			cout<<"\t"<<GSL_REAL(gsl_matrix_complex_get(F,i,j))<<" +i"<<GSL_IMAG(gsl_matrix_complex_get(F,i,j));
		}
		cout<<endl;
	}

	cout<<"the actual energy percentage E= "<<E<<endl;
	cout<<"the coefficients S_f="<<endl;
	for(i=0;i<nvec;i++)
	{
		for(j=0;j<Ml;j++)
		{
			cout<<"\t"<<GSL_REAL(gsl_matrix_complex_get(S_f,i,j))<<" +i"<<GSL_IMAG(gsl_matrix_complex_get(S_f,i,j));
		}
		cout<<endl;
	}

/*------------------------------------------------------------------------------
write to file routine here
------------------------------------------------------------------------------*/

	fprintf(file,"%d %d\n",nvec,Q);
	for(j=0;j<nvec;j++){
		for(i=0;i<Q;i++){
			fprintf(file,"%f %f ",GSL_REAL(gsl_matrix_complex_get(F,i,j)),-GSL_IMAG(gsl_matrix_complex_get(F,i,j)));
		}
		fprintf(file,"\n");
	}
	fclose(file);

return 0;
}
