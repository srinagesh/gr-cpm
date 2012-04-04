#ifndef INCLUDED_HOWTO_PHASE_CORRECTION_VCVC_H
#define INCLUDED_HOWTO_PHASE_CORRECTION_VCVC_H

#include <howto_api.h>
#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gri_control_loop.h>
#include <digital_constellation.h>

class howto_phase_correction_vcvc;
typedef boost::shared_ptr<howto_phase_correction_vcvc> howto_phase_correction_vcvc_sptr;
HOWTO_API howto_phase_correction_vcvc_sptr howto_make_phase_correction_vcvc (unsigned int lvec,unsigned int constellation_size, float initial_phase, float loop_bw, float max_freq, float min_freq,digital_constellation_sptr cnst);


class HOWTO_API howto_phase_correction_vcvc: public gr_sync_block, public gri_control_loop
{
private:
friend HOWTO_API howto_phase_correction_vcvc_sptr howto_make_phase_correction_vcvc(unsigned int lvec, unsigned int constellation_size, float initial_phase, float loop_bw, float max_freq, float min_freq,digital_constellation_sptr cnst);
howto_phase_correction_vcvc(unsigned int lvec, unsigned int constellation_size, float initial_phase, float loop_bw, float max_freq, float min_freq, digital_constellation_sptr cnst);
digital_constellation_sptr d_cnst;
float d_initial_phase;
unsigned int d_lvec;
unsigned int d_constellation_size;
std::vector<gr_complex> d_s;
public:
virtual float phase_detector(gr_complex sample, float ref_phase);
virtual float mod_2pi(float in_ph);
int work(int noutput_items,
	gr_vector_const_void_star &input_items,
	gr_vector_void_star &output_items);
};

#endif
