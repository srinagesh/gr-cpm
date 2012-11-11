//This program outputs the new constellation after rotation. last changed on 5-18-2012.
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <digital_api.h>
#include <gri_control_loop.h>
#include <digital_phase_correction_nda_vcvc.h>
#include <gr_io_signature.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <gr_sincos.h>
#include <gr_expj.h>
#include <math.h>
#include <gr_math.h>
#include <digital_constellation.h>
#include <gr_cpm.h>
#include <gsl/gsl_matrix.h>
#include "cpmdecomposition.cpp"
#include "orthogonalize.cpp"
using namespace std;

DIGITAL_API digital_phase_correction_nda_vcvc_sptr digital_make_phase_correction_nda_vcvc(unsigned int lvec, unsigned int power, float loop_bw, float max_freq, float min_freq, digital_constellation_sptr cnst)
{
	return gnuradio::get_initial_sptr(new digital_phase_correction_nda_vcvc(lvec, power, loop_bw, max_freq, min_freq,cnst));
}

//---------------------------------------------------------------------------
//		SHARED POINTER USING GR_CPM
//---------------------------------------------------------------------------
DIGITAL_API digital_phase_correction_nda_vcvc_sptr digital_make_phase_correction_nda_vcvc(unsigned int lvec, unsigned int power, float loop_bw, float max_freq, float min_freq, unsigned int M, unsigned int L, unsigned int K, unsigned int P, unsigned int Q, int pulsetype, double beta)
{
	return gnuradio::get_initial_sptr(new digital_phase_correction_nda_vcvc(lvec, power, loop_bw, max_freq, min_freq, M, L, K, P, Q, (gr_cpm::cpm_type)pulsetype, beta));
}


/*-------------------------------------------------------------------------------
*			CONSTRUCTOR
*------------------------------------------------------------------------------*/

digital_phase_correction_nda_vcvc::digital_phase_correction_nda_vcvc(unsigned int lvec, unsigned int power, float loop_bw, float max_freq, float min_freq,digital_constellation_sptr cnst):gr_sync_block("phase_correction_nda_vcvc", gr_make_io_signature(1,1,lvec*sizeof(gr_complex)), gr_make_io_signature(1,1,lvec*sizeof(gr_complex))),gri_control_loop(loop_bw, max_freq, min_freq),d_cnst(cnst)
{	cout<<"passing through constructor"<<endl;
	float tmag, treal, timag;
	int num_points;
	std::vector<gr_complex> y;
	d_power = power;
	d_lvec = lvec;
	z.resize(d_lvec);

	constellation = d_cnst->points();
	num_points = d_cnst->arity();
	y.resize(num_points);
	for(unsigned int j=0;j<d_lvec;j++){
		z[j] = gr_complex(0,0);
		for(unsigned int i = 0; i<num_points;i++){
			tmag = abs(constellation[i*d_lvec+j]);
			y[i] = polar(float(pow(tmag,d_power)), float(arg(constellation[i*d_lvec+j])*d_power));
			z[j] +=y[i];
		}
		z[j]=gr_complex(z[j].real()/num_points, z[j].imag()/num_points);
		cout<<"Z is "<<z[j]<<endl;
	}
	cout<<"this program has passed through the pc constructor"<<endl;
}

//----------------------------------------------------------------------------
//		CONSTRUCTOR WITH GR_CPM
//----------------------------------------------------------------------------
digital_phase_correction_nda_vcvc::digital_phase_correction_nda_vcvc(unsigned int lvec, unsigned int power, float loop_bw, float max_freq, float min_freq, unsigned int M, unsigned int L, unsigned int K, unsigned int P, unsigned int Q, gr_cpm::cpm_type pulsetype, double beta):gr_sync_block("phase_correction_nda_vcvc", gr_make_io_signature(1,1,lvec*sizeof(gr_complex)), gr_make_io_signature(1,1,lvec*sizeof(gr_complex))),gri_control_loop(loop_bw, max_freq, min_freq)
{
	float tmag, treal, timag;
	int num_points;
	std::vector<gr_complex> y;
	std::vector<float> d_g, d_q;
	gsl_vector* q;
	gsl_matrix_complex *Sx, *F, *S_f;
	int Ml, t, nvec;
	double freq, E;


	d_power = power;
	d_lvec = lvec;
	z.resize(d_lvec);
	
	Ml = (int)floor(pow(M,L)*P+0.5);
	d_q.resize(Q*L);
	q = gsl_vector_alloc(Q*L);
	Sx = gsl_matrix_complex_alloc(Q,Ml);
	constellation.resize(Ml*lvec);
	y.resize(Ml);
	
	d_g = gr_cpm::phase_response(pulsetype, Q, L, beta);
	d_q[0] = 0;
	gsl_vector_set(q,0,d_q[0]);
	for(unsigned int i=1;i<Q*L;i++){
		d_q[i] = (d_q[i-1]+d_g[i]);
		gsl_vector_set(q,i,d_q[i]/2);
		cout << "q of "<<i<<" is "<<d_q[i]/2<<endl;
	}
	
	t = cpmdecomposition(q,Q,L,M,K,P,Sx,freq);

	cout<<"the operating frequency is fo= "<<freq<<endl;

	t = orthogonalize(Sx,Q,Ml,1,F, E, nvec, S_f);

	num_points = Ml;
	
	for(unsigned int j=0;j<Ml;j++){
		for(unsigned int i=0;i<d_lvec;i++){
			treal = GSL_REAL(gsl_matrix_complex_get(S_f,i,j));
			timag = GSL_IMAG(gsl_matrix_complex_get(S_f,i,j));
			constellation[j*d_lvec+i] = gr_complex(treal, timag);
		}
	} 
	
	for(unsigned int j=0;j<d_lvec;j++){
		z[j] = gr_complex(0,0);
		for(unsigned int i = 0; i<num_points;i++){
			tmag = abs(constellation[i*d_lvec+j]);
			y[i] = polar(float(pow(tmag,d_power)), float(arg(constellation[i*d_lvec+j])*d_power));
			z[j] +=y[i];
		}
		z[j]=gr_complex(z[j].real()/num_points, z[j].imag()/num_points);
		cout<<"Z is "<<z[j]<<endl;
	}
	cout<<"this program has passed through the gr_cpm constructor"<<endl;
}


//----------------------------------------------------------------------------
//			WORK FUNCTION
//----------------------------------------------------------------------------
int digital_phase_correction_nda_vcvc::work(int noutput_items,
					gr_vector_const_void_star &input_items,
					gr_vector_void_star &output_items)
{
	const gr_complex *in = (const gr_complex*)input_items[0];
	gr_complex *out = (gr_complex*)output_items[0];
	float t_real,t_imag, error, temp_real, temp_imag,temp_mag, d_phase_parameter;
	gr_complex t_complex, pll_ip, rec_sig_sum, refph;
	std::vector<gr_complex> rec_sig;

	rec_sig.resize(d_lvec);

	for(unsigned int n=0;n<noutput_items;n++){

//raise input signal to the nth power------------------------------------
		for(unsigned int j=0;j<d_lvec;j++){
			temp_mag = abs(in[n*d_lvec+j]);
			rec_sig[j] = polar(float(pow(temp_mag,d_power)), float(arg(in[n*d_lvec+j])*d_power));
		}

//-----------------------------------------------------------------------

		rec_sig_sum=gr_complex(0,0);
		for(unsigned int i=0;i<d_lvec;i++){	
			temp_real=(rec_sig[i].real() * z[i].real() - rec_sig[i].imag() * z[i].imag());
			temp_imag=(rec_sig[i].real() * z[i].imag() + rec_sig[i].imag() * z[i].real());
			rec_sig_sum=gr_complex(rec_sig_sum.real()+temp_real,rec_sig_sum.imag()+temp_imag);
		}	

		temp_mag = sqrt(pow(rec_sig_sum.real(),2)+pow(rec_sig_sum.imag(),2));
		if (temp_mag !=0)
		rec_sig_sum = gr_complex(rec_sig_sum.real()/temp_mag, rec_sig_sum.imag()/temp_mag);
		

		d_phase_parameter=(float)d_phase/d_power;
		gr_sincosf(d_phase_parameter,&t_imag,&t_real);
		t_complex = gr_complex(t_real,-t_imag);

		for(unsigned int i=0;i<d_lvec;i++){
			out[n*d_lvec+i] = in[n*d_lvec+i]*t_complex;
		}

		refph = gr_expj(-d_phase);
		error = rec_sig_sum.real()*refph.imag()+rec_sig_sum.imag()*refph.real();
		advance_loop(error);
		phase_wrap();
		frequency_limit();
	}
		
return noutput_items;
}

