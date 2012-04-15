//This program outputs the new constellation after rotation. last changed on 4-8-2012.
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <howto_api.h>
#include <gri_control_loop.h>
#include <howto_phase_correction_vcvc.h>
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

HOWTO_API howto_phase_correction_vcvc_sptr howto_make_phase_correction_vcvc(unsigned int lvec, unsigned int constellation_size, float initial_phase, float loop_bw, float max_freq, float min_freq, digital_constellation_sptr cnst)
{
	return gnuradio::get_initial_sptr(new howto_phase_correction_vcvc(lvec,constellation_size, initial_phase, loop_bw, max_freq, min_freq, cnst));
}

howto_phase_correction_vcvc::howto_phase_correction_vcvc(unsigned int lvec,  unsigned int constellation_size, float initial_phase, float loop_bw, float max_freq, float min_freq, digital_constellation_sptr cnst):gr_sync_block("phase_correction_vcvc", gr_make_io_signature(1,1,lvec*sizeof(gr_complex)), gr_make_io_signature(1,1,lvec*sizeof(gr_complex))),gri_control_loop(loop_bw, max_freq, min_freq),d_cnst(cnst)
{	cout<<"passing through constructor"<<endl;
	d_initial_phase = initial_phase;
	d_lvec = lvec;
	d_constellation_size=constellation_size;
	cout<<"this program has passed through the pc constructor"<<endl;
}

//howto_phase_correction_vcvc::~howto_phase_correction_vcvc()
//{
//	delete digital_constellation_sptr d_cnst;
//}

float howto_phase_correction_vcvc::mod_2pi(float in_ph)
{
	if (in_ph>M_PI)
		return in_ph-M_PI;
	else if (in_ph<-M_PI)
		return in_ph+M_PI;
	else
		return in_ph;
}

float howto_phase_correction_vcvc::phase_detector(gr_complex sample, float ref_phase)
{
	float sample_phase;
	sample_phase = gr_fast_atan2f(sample.imag(),sample.real());
	return mod_2pi(sample_phase-ref_phase);
}

int howto_phase_correction_vcvc::work(int noutput_items,
					gr_vector_const_void_star &input_items,
					gr_vector_void_star &output_items)
{
	const gr_complex *in = (const gr_complex*)input_items[0];
	gr_complex *out = (gr_complex*)output_items[0];
	float t_real,t_imag, error, temp_real, temp_imag, d_phase_parameter;
	float sig_distance[d_constellation_size];
	unsigned int min_index;
	gr_complex t_complex, pll_ip, *rec_sig_ptr;
	std::vector<gr_complex> rec_sig, constellation_pt, constellation;

	rec_sig.resize(d_lvec);
	constellation_pt.resize(d_lvec);
//	cout<<"passing through initialization"<<endl;

	for(unsigned int n=0;n<noutput_items;n++){
	//	cout<<"first line"<<endl;
		d_phase_parameter=d_initial_phase+d_phase;
		gr_sincosf(d_phase_parameter,&t_imag,&t_real);
		t_complex = gr_complex(t_real,-t_imag);
	//	cout<<"second line"<<endl;

		for(unsigned int i=0;i<d_lvec;i++){
	//		cout<<"loop"<< i <<endl;
			temp_real=in[n*d_lvec+i].real() * t_complex.real() - in[n*d_lvec+i].imag() * t_complex.imag();
			temp_imag=in[n*d_lvec+i].real() * t_complex.imag() + in[n*d_lvec+i].imag() * t_complex.real();
			rec_sig[i]=gr_complex(temp_real,temp_imag);
		}
		
	//	cout<<"d_lvec="<<d_lvec<<endl;

		rec_sig_ptr=&rec_sig[0];
	//	d_cnst->calc_euclidean_metric(rec_sig_ptr,sig_distance);
		
	//	cout<<"passed euclidean metric"<< sizeof(sig_distance)<<endl;
		for(unsigned int i=0;i<d_lvec;i++){				//change to d_constellation_size when using sig_distance
	//		out[n*d_constellation_size+i]=sig_distance[i];
			temp_real=rec_sig[i].real();
			temp_imag=rec_sig[i].imag();
			out[n*d_lvec+i]=gr_complex(temp_real,temp_imag);
		}
		
	//	cout<<"passing through decision maker"<<endl;
		min_index=d_cnst->decision_maker(rec_sig_ptr);
		constellation = d_cnst->points();
	//	cout<<"passed decision maker and points"<<endl;
		for(unsigned int i=0;i<d_lvec;i++){
			temp_real=constellation[min_index*d_lvec+i].real();
			temp_imag=constellation[min_index*d_lvec+i].imag();
	//		cout<<"real "<<temp_real<<" imag "<<temp_imag<<endl;
			constellation_pt[i]=gr_complex(temp_real,temp_imag);
		}
	//	cout<<"passed loop"<<endl;
		
		pll_ip=gr_complex(0.0,0.0);
		for(unsigned int i=0;i<d_lvec;i++){
			temp_real= in[n*d_lvec+i].real() *constellation_pt[i].real() + in[n*d_lvec+i].imag() * constellation_pt[i].imag();
			temp_imag= -in[n*d_lvec+i].real() * constellation_pt[i].imag() + in[n*d_lvec+i].imag() * constellation_pt[i].real();
			temp_real=temp_real+pll_ip.real();
			temp_imag=temp_imag+pll_ip.imag();
			pll_ip=gr_complex(temp_real,temp_imag);

		}

	//	cout<<"passed phase correction"<<endl;
		
		error = phase_detector(pll_ip,d_phase);
		advance_loop(error);
	//	cout<<"d_phase "<<d_phase<<endl;
	//	cout<<"d_freq "<<d_freq<<endl;
		phase_wrap();
		frequency_limit();
		d_initial_phase=0;
	//	cout<<"end of loop"<<endl;
	}
	
	//cout<<d_phase_parameter<<endl;
		
return noutput_items;
}

