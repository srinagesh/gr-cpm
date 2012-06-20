/*new version, changes in parameters d_updated removed, no gr_fft class */
#ifndef INCLUDED_HOWTO_DEMOD_FILTERBANK_CCVC_H
#define INCLUDED_HOWTO_DEMOD_FILTERBANK_CCVC_H

#include <gr_core_api.h>
#include <howto_api.h>
#include <gr_cpm.h>
#include <gr_sync_block.h>
#include <gri_fir_filter_with_buffer_ccc.h>
#include <gsl/gsl_matrix.h>

/*
DEMODULATION FILTERBANK
takes in a scalar complex input and produces a complex vector of size numvec
usage:
param: number of vectors: numvec
param: filterbank args: dependant on the constructor
note: numvec has to be again input in the filterbank arguments
*/

class howto_demod_filterbank_ccvc;
typedef boost::shared_ptr<howto_demod_filterbank_ccvc> howto_demod_filterbank_ccvc_sptr;

//enum howto_demod_filterbank_ccvc::cpm_type;

HOWTO_API howto_demod_filterbank_ccvc_sptr howto_make_demod_filterbank_ccvc (unsigned int numvec, unsigned int taps_per_filter, const std::vector<gr_complex> vtaps);
HOWTO_API howto_demod_filterbank_ccvc_sptr howto_make_demod_filterbank_ccvc (unsigned int numvec, const char *name);
HOWTO_API howto_demod_filterbank_ccvc_sptr howto_make_demod_filterbank_ccvc (unsigned int numvec, unsigned int M, unsigned int L, unsigned int K, unsigned int P, unsigned int Q, int pulsetype, double beta);

class gr_fir_ccc;

class HOWTO_API howto_demod_filterbank_ccvc: public gr_sync_block
{
private:
friend HOWTO_API howto_demod_filterbank_ccvc_sptr howto_make_demod_filterbank_ccvc (unsigned int numvec, unsigned int taps_per_filter,const std::vector<gr_complex> vtaps);
friend HOWTO_API howto_demod_filterbank_ccvc_sptr howto_make_demod_filterbank_ccvc (unsigned int numvec, const char *name);
friend HOWTO_API howto_demod_filterbank_ccvc_sptr howto_make_demod_filterbank_ccvc (unsigned int numvec, unsigned int M, unsigned int L, unsigned int K, unsigned int P, unsigned int Q, int pulsetype, double beta);

/*constructor which accepts taps
param: numvec: size of output vectors and the number of filters in the filterbank
param: taps_per_filter: taps per filter
param: vtaps: vector of taps: number of taps must be numvec*taps_per_filter
*/
howto_demod_filterbank_ccvc(unsigned int numvec, unsigned int taps_per_filter,const std::vector<gr_complex> vtaps);

/*constructor which reads taps from a textfile
param: numvec: size of output vectors and the number of filters in the filterbank
param: name: textfile name
NOTE: text file should be of the following format:
NUMVEC TAPS_PER_FILTER

TAPS OF FILTER 1

TAPS OF FILTER 2
.
.

TAPS OF FILTER NUMVEC

*/
howto_demod_filterbank_ccvc(unsigned int numvec, const char *name);

/*constructor which performs the RIMOLDI decomposition of CPM and sets the taps resulting from the decomposition
param: numvec: size of output vectors and the number of filters in the filterbank
param: M: arity of the alphabet of CPM
param: L: length of the pulse in symbols
param: K, P: modulation index of the form h=K/P, K and P are relatively prime
param: Q: samples per symbol
param: pulsetype: CPM pulse type: input of the form gr.cpm.LREC
param: beta: the beta parameter in gr_cpm.h
*/
howto_demod_filterbank_ccvc(unsigned int numvec, unsigned int M, unsigned int L, unsigned int K, unsigned int P, unsigned int Q, gr_cpm::cpm_type pulsetype, double beta);

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
