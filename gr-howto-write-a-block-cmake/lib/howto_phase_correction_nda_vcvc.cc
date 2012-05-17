//This program outputs the new constellation after rotation. last changed on 4-8-2012.
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <howto_api.h>
#include <gri_control_loop.h>
#include <howto_phase_correction_nda_vcvc.h>
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
using namespace std;

HOWTO_API howto_phase_correction_nda_vcvc_sptr howto_make_phase_correction_nda_vcvc(unsigned int lvec, unsigned int power, float loop_bw, float max_freq, float min_freq, digital_constellation_sptr cnst)
{
	return gnuradio::get_initial_sptr(new howto_phase_correction_nda_vcvc(lvec, power, loop_bw, max_freq, min_freq,cnst));
}

howto_phase_correction_nda_vcvc::howto_phase_correction_nda_vcvc(unsigned int lvec, unsigned int power, float loop_bw, float max_freq, float min_freq,digital_constellation_sptr cnst):gr_sync_block("phase_correction_nda_vcvc", gr_make_io_signature(1,1,lvec*sizeof(gr_complex)), gr_make_io_signature(1,1,lvec*sizeof(gr_complex))),gri_control_loop(loop_bw, max_freq, min_freq),d_cnst(cnst)
{	cout<<"passing through constructor"<<endl;
	float temp_real,temp_imag;
	std::vector<gr_complex> y;
	d_power = power;
	d_lvec = lvec;
	z.resize(d_lvec);
	y.resize(d_lvec);

	constellation = d_cnst->points();
	for(unsigned int j=0;j<d_lvec;j++){
		y[j]=gr_complex(1,0);
		for(unsigned int i=0;i<d_power;i++){
			temp_real=constellation[j].real() * y[j].real() - constellation[j].imag() * y[j].imag();
			temp_imag=constellation[j].real() * y[j].imag() + constellation[j].imag() * y[j].real();
			y[j]=gr_complex(temp_real,temp_imag);
		}
		z[j]=gr_complex(y[j].real(),0);
		cout<<"Z is "<<z[j]<<endl;
	}
	cout<<"this program has passed through the pc constructor"<<endl;
}


float howto_phase_correction_nda_vcvc::phase_detector(gr_complex sample, gr_complex ref)
{
	float phase_diff;
	phase_diff = sample.real()*ref.imag() + sample.imag()*ref.real();
	return phase_diff;
}

int howto_phase_correction_nda_vcvc::work(int noutput_items,
					gr_vector_const_void_star &input_items,
					gr_vector_void_star &output_items)
{
	const gr_complex *in = (const gr_complex*)input_items[0];
	gr_complex *out = (gr_complex*)output_items[0];
	float t_real,t_imag, error, temp_real, temp_imag, d_phase_parameter;
	gr_complex t_complex, pll_ip, rec_sig_sum, refph;
	std::vector<gr_complex> rec_sig;

	rec_sig.resize(d_lvec);

	for(unsigned int n=0;n<noutput_items;n++){

//raise input signal to the nth power------------------------------------
		for(unsigned int i=0;i<d_power;i++){
			for(unsigned int j=0;j<d_lvec;j++){
				if(i==0)
				rec_sig[j]=gr_complex(1,0);

				temp_real=in[n*d_lvec+j].real() * rec_sig[j].real() - in[n*d_lvec+j].imag() * rec_sig[j].imag();
				temp_imag=in[n*d_lvec+j].real() * rec_sig[j].imag() + in[n*d_lvec+j].imag() * rec_sig[j].real();
				rec_sig[j]=gr_complex(temp_real,temp_imag);
			}
		}

//-----------------------------------------------------------------------
	
		rec_sig_sum=gr_complex(0,0);
		for(unsigned int i=0;i<d_lvec;i++){	
			temp_real=(rec_sig[i].real() * z[i].real() + rec_sig[i].imag() * z[i].imag())/pow(z[i].real(),2);
			temp_imag=(-rec_sig[i].real() * z[i].imag() + rec_sig[i].imag() * z[i].real())/pow(z[i].real(),2);
			temp_real=rec_sig_sum.real()+temp_real;
			temp_imag=rec_sig_sum.imag()+temp_imag;
			rec_sig_sum=gr_complex(temp_real,temp_imag);
		}	

		d_phase_parameter=(float)d_phase/d_power;
		gr_sincosf(d_phase_parameter,&t_imag,&t_real);
		t_complex = gr_complex(t_real,-t_imag);

		for(unsigned int i=0;i<d_lvec;i++){
			temp_real=in[n*d_lvec+i].real() * t_complex.real() - in[n*d_lvec+i].imag() * t_complex.imag();
			temp_imag=in[n*d_lvec+i].real() * t_complex.imag() + in[n*d_lvec+i].imag() * t_complex.real();
			out[n*d_lvec+i]=gr_complex(temp_real,temp_imag);
		}
		
		refph = gr_expj(-d_phase);
		error = phase_detector(rec_sig_sum, refph);
		advance_loop(error);
		phase_wrap();
		frequency_limit();
	}
		
return noutput_items;
}

