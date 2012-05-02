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
	d_power = power;
	d_lvec = lvec;
	z.resize(d_lvec);

	constellation = d_cnst->points();
	for(unsigned int i=0;i<d_power;i++){
		for(unsigned int j=0;j<d_lvec;j++){
			if(i==0)
			z[j]=gr_complex(1,0);

			temp_real=constellation[j].real() * z[j].real() - constellation[j].imag() * z[j].imag();
			temp_imag=constellation[j].real() * z[j].imag() + constellation[j].imag() * z[j].real();
			z[j]=gr_complex(temp_real,temp_imag);
		}
	}
//	cout<<"c_real "<<z[0].real()<<" c_imag "<<z[0].imag()<<endl;
	cout<<"this program has passed through the pc constructor"<<endl;
}

//howto_phase_correction_vcvc::~howto_phase_correction_vcvc()
//{
//	delete digital_constellation_sptr d_cnst;
//}

float howto_phase_correction_nda_vcvc::mod_2pi(float in_ph)
{
	if (in_ph>M_PI)
		return in_ph-M_PI;
	else if (in_ph<-M_PI)
		return in_ph+M_PI;
	else
		return in_ph;
}

float howto_phase_correction_nda_vcvc::phase_detector(gr_complex sample, float ref_phase)
{
	float sample_phase;
	sample_phase = gr_fast_atan2f(sample.imag(),sample.real());
//	cout<<sample_phase<<endl;
	return mod_2pi(sample_phase-ref_phase);
}

int howto_phase_correction_nda_vcvc::work(int noutput_items,
					gr_vector_const_void_star &input_items,
					gr_vector_void_star &output_items)
{
	const gr_complex *in = (const gr_complex*)input_items[0];
	gr_complex *out = (gr_complex*)output_items[0];
	float t_real,t_imag, error, temp_real, temp_imag, d_phase_parameter;
	gr_complex t_complex, pll_ip, rec_sig_sum;
	std::vector<gr_complex> rec_sig;

	rec_sig.resize(d_lvec);

	for(unsigned int n=0;n<noutput_items;n++){
	//	cout<<"first line"<<endl;

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
//		cout<<"c_real "<<rec_sig[0].real()<<" c_imag "<<rec_sig[0].imag()<<endl;
//-----------------------------------------------------------------------	
		rec_sig_sum=gr_complex(0,0);
		for(unsigned int i=0;i<d_lvec;i++){				//!!NOTE:change this back to d_lvec
			temp_real=(rec_sig[i].real() * z[i].real() + rec_sig[i].imag() * z[i].imag());
			temp_imag=(-rec_sig[i].real() * z[i].imag() + rec_sig[i].imag() * z[i].real());
			temp_real=rec_sig_sum.real()+temp_real;
			temp_imag=rec_sig_sum.imag()+temp_imag;
			rec_sig_sum=gr_complex(temp_real,temp_imag);
		}
	//	cout<<"c_real "<<rec_sig_sum.real()<<" c_imag "<<rec_sig_sum.imag()<<endl;	
	
		//rec_sig_sum is the input to the phase locked loop.

		
		d_phase_parameter=(float)d_phase/d_power;
		gr_sincosf(d_phase_parameter,&t_imag,&t_real);
		t_complex = gr_complex(t_real,-t_imag);
	//	cout<<"second line"<<endl;

		for(unsigned int i=0;i<d_lvec;i++){
	//		cout<<"loop"<< i <<endl;
			temp_real=in[n*d_lvec+i].real() * t_complex.real() - in[n*d_lvec+i].imag() * t_complex.imag();
			temp_imag=in[n*d_lvec+i].real() * t_complex.imag() + in[n*d_lvec+i].imag() * t_complex.real();
			out[n*d_lvec+i]=gr_complex(temp_real,temp_imag);
		}
		
	//	cout<<"d_lvec="<<d_lvec<<endl;
		
		error = phase_detector(rec_sig_sum,d_phase);
		advance_loop(error);
	//	cout<<"d_phase "<<d_phase<<endl;
	//	cout<<"d_freq "<<d_freq<<endl;
		phase_wrap();
		frequency_limit();
	//	cout<<"end of loop"<<endl;
	}
	
	//cout<<error<<endl;
		
return noutput_items;
}

