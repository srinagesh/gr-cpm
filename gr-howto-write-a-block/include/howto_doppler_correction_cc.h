/* This block implements a doppler correction for gnu radio. It reads frequency from a local port and interfaces with gpredict. It also sets a source with a complex sinewave with this frequency and mixes it with the input. effectively working as a doppler shift system */

#ifndef INCLUDED_HOWTO_DOPPLER_CORRECTION_CC_H
#define INCLUDED_HOWTO_DOPPLER_CORRECTION_CC_H

#include <gr_core_api.h>
#include <howto_api.h>
#include <gr_sync_block.h>
#include <gr_fxpt_nco.h>
#include <fstream>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

class howto_doppler_correction_cc;
typedef boost::shared_ptr<howto_doppler_correction_cc> howto_doppler_correction_cc_sptr;
HOWTO_API howto_doppler_correction_cc_sptr howto_make_doppler_correction_cc (double sampling_freq, unsigned int port, double int_freq, long int rxfreq);

class HOWTO_API howto_doppler_correction_cc: public gr_sync_block
{
private:
friend HOWTO_API howto_doppler_correction_cc_sptr howto_make_doppler_correction_cc (double sampling_freq, unsigned int port, double int_freq, long int rxfreq);
howto_doppler_correction_cc(double sampling_freq, unsigned int port, double int_freq, long int rxfreq);

double d_sampling_freq;
double d_int_freq;
unsigned int d_port;
double d_local_freq;
double d_doppler;
long int d_rxfreq;
gr_fxpt_nco d_nco;
pthread_t socket_thread;

static void *start_thread(void *obj){
	reinterpret_cast<howto_doppler_correction_cc *>(obj)->socket_thread_work();
}

public:
~howto_doppler_correction_cc();

void socket_thread_work();

int process_command(char*, char*);

int work (int noutput_items,
	  gr_vector_const_void_star &input_items,
          gr_vector_void_star &output_items);
};

#endif
