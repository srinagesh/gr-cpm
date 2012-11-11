#ifndef INCLUDED_DIGITAL_PHASE_CORRECTION__NDA_VCVC_H
#define INCLUDED_DIGITAL_PHASE_CORRECTION_NDA_VCVC_H

#include <digital_api.h>
#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gri_control_loop.h>
#include <digital_constellation.h>
#include <gr_cpm.h>

/*NON-DECISION AIDED PHASE CORRECTION BLOCK
input parameters:
lvec: length of the input vector
power: power to raise the input signal to kill the modulation
loop_bw, max_freq, min_freq: PLL parameters
constellation object: either takes in a pointer to the constellation object, or takes in CPM RIMOLDI DECOMPOSITION parameters and calculates the resulting constellation
*/

class digital_phase_correction_nda_vcvc;
typedef boost::shared_ptr<digital_phase_correction_nda_vcvc> digital_phase_correction_nda_vcvc_sptr;

DIGITAL_API digital_phase_correction_nda_vcvc_sptr digital_make_phase_correction_nda_vcvc (unsigned int lvec, unsigned int power, float loop_bw, float max_freq, float min_freq, digital_constellation_sptr cnst);

DIGITAL_API digital_phase_correction_nda_vcvc_sptr digital_make_phase_correction_nda_vcvc (unsigned int lvec, unsigned int power, float loop_bw, float max_freq, float min_freq, unsigned int M, unsigned int L, unsigned int K, unsigned int P, unsigned int Q, int pulsetype, double beta);


class DIGITAL_API digital_phase_correction_nda_vcvc: public gr_sync_block, public gri_control_loop
{
private:
friend DIGITAL_API digital_phase_correction_nda_vcvc_sptr digital_make_phase_correction_nda_vcvc(unsigned int lvec, unsigned int power, float loop_bw, float max_freq, float min_freq,digital_constellation_sptr cnst);

friend DIGITAL_API digital_phase_correction_nda_vcvc_sptr digital_make_phase_correction_nda_vcvc (unsigned int lvec, unsigned int power, float loop_bw, float max_freq, float min_freq, unsigned int M, unsigned int L, unsigned int K, unsigned int P, unsigned int Q, int pulsetype, double beta);

digital_phase_correction_nda_vcvc(unsigned int lvec, unsigned int power, float loop_bw, float max_freq, float min_freq,digital_constellation_sptr cnst);

digital_phase_correction_nda_vcvc(unsigned int lvec, unsigned int power, float loop_bw, float max_freq, float min_freq, unsigned int M, unsigned int L, unsigned int K, unsigned int P, unsigned int Q, gr_cpm::cpm_type pulsetype, double beta);

unsigned int d_power;
unsigned int d_lvec;
digital_constellation_sptr d_cnst;
std::vector<gr_complex> z;
std::vector<gr_complex> constellation;
public:
int work(int noutput_items,
	gr_vector_const_void_star &input_items,
	gr_vector_void_star &output_items);
};

#endif
