/*new version, changes in parameters d_updated removed, no gr_fft class */
#ifndef INCLUDED_HOWTO_DEMOD_FILTERBANK_CCVC_H
#define INCLUDED_HOWTO_DEMOD_FILTERBANK_CCVC_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gri_fir_filter_with_buffer_ccc.h>

class howto_demod_filterbank_ccvc;
typedef boost::shared_ptr<howto_demod_filterbank_ccvc> howto_demod_filterbank_ccvc_sptr;
howto_demod_filterbank_ccvc_sptr howto_make_demod_filterbank_ccvc (unsigned int numvec, unsigned int taps_per_filter, const std::vector<gr_complex> vtaps);
howto_demod_filterbank_ccvc_sptr howto_make_demod_filterbank_ccvc (unsigned int numvec, const char *name);

class gr_fir_ccc;

class howto_demod_filterbank_ccvc: public gr_sync_block
{
private:
friend howto_demod_filterbank_ccvc_sptr howto_make_demod_filterbank_ccvc (unsigned int numvec, unsigned int taps_per_filter,const std::vector<gr_complex> vtaps);
friend howto_demod_filterbank_ccvc_sptr howto_make_demod_filterbank_ccvc (unsigned int numvec, const char *name);
howto_demod_filterbank_ccvc(unsigned int numvec, unsigned int taps_per_filter,const std::vector<gr_complex> vtaps);
howto_demod_filterbank_ccvc(unsigned int numvec, const char *name);

std::vector<gri_fir_filter_with_buffer_ccc*> d_vfilters;
std::vector<std::vector <gr_complex> > d_vtaps;
	unsigned int d_numvec;
	unsigned int d_taps_per_filter;

public:
~howto_demod_filterbank_ccvc();

void set_taps(const std::vector<gr_complex> vtaps);

void print_taps();

int work (int noutput_items,
          gr_vector_const_void_star &input_items,
          gr_vector_void_star &output_items);
};

#endif
