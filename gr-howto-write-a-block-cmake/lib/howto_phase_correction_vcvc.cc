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

HOWTO_API howto_phase_correction_vcvc_sptr howto_make_phase_correction_vcvc(unsigned int lvec, unsigned int constellation_size, float initial_phase, float loop_bw, float max_freq, float min_freq, digital_constellation_sptr cnst)
{
	return howto_phase_correction_vcvc_sptr(new howto_phase_correction_vcvc(lvec,constellation_size, initial_phase, loop_bw, max_freq, min_freq, cnst));
}

howto_phase_correction_vcvc::howto_phase_correction_vcvc(unsigned int lvec,  unsigned int constellation_size, float initial_phase, float loop_bw, float max_freq, float min_freq, digital_constellation_sptr cnst):gr_sync_block("phase_correction_vcvc", gr_make_io_signature(1,1,lvec*sizeof(gr_complex)), gr_make_io_signature(1,1,constellation_size*sizeof(gr_complex))),gri_control_loop(loop_bw, max_freq, min_freq)
{
	d_phase = initial_phase;
	d_lvec = lvec;
	d_constellation_size=constellation_size;
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
	gr_complex *in = (gr_complex*)input_items[0];
	gr_complex *out = (gr_complex*)output_items[0];
	float t_real,t_imag, sig_distance, error;
	unsigned int min_index;
	gr_complex t_complex, pll_ip, rec_sig_ptr;
	std::vector<gr_complex> rec_sig, constellation_pt, constellation;

	//rec_sig.resize(d_lvec);
	pll_ip=gr_complex(0.0,0.0);

	for(unsigned int n=0;n<noutput_items;n++){
		gr_sincosf(d_phase,&t_imag,&t_real);
		t_complex = gr_complex(t_real,-t_imag);

		for(unsigned int i=0;i<d_lvec;i++){
			rec_sig[i]=(in[n*d_lvec+i]*t_complex);
		}
		rec_sig_ptr=rec_sig[0];
		d_cnst->calc_euclidean_metric(&rec_sig_ptr,&sig_distance);
		
		for(unsigned int i=0;i<d_constellation_size;i++){
			*out++=sig_distance++;
		}

		min_index=d_cnst->decision_maker(&rec_sig_ptr);
		constellation = d_cnst->points();
		for(unsigned int i=0;i<d_lvec;i++){
			constellation_pt[i] = constellation[min_index*d_lvec+i];
		}
		
		pll_ip=gr_complex(0.0,0.0);
		for(unsigned int i=0;i<d_lvec;i++){
			pll_ip=pll_ip+(in[n*d_lvec+i]*conj(constellation_pt[i]));
		}
		
		gr_sincosf(d_phase,&t_imag,&t_real);
		t_complex = gr_complex(t_real,-t_imag);

		
		error = phase_detector(pll_ip,d_phase);
		advance_loop(error);
		phase_wrap();
		frequency_limit();
	}
		
return noutput_items;
}

