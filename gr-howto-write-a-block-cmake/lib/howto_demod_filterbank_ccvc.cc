/*new version, changes in parameters "vtaps", d_taps_per_filter is removed, output is an std::vector<gr_complex> */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <howto_demod_filterbank_ccvc.h>
#include <gr_io_signature.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <stdlib.h>
#include <assert.h>

howto_demod_filterbank_ccvc_sptr howto_make_demod_filterbank_ccvc (unsigned int numvec, unsigned int taps_per_filter,const std::vector<gr_complex> vtaps)
{
	return howto_demod_filterbank_ccvc_sptr(new howto_demod_filterbank_ccvc(numvec, taps_per_filter, vtaps));
}

/*----------------------------------------------------------------------------------
shared pointer for constructor with filename
----------------------------------------------------------------------------------*/
howto_demod_filterbank_ccvc_sptr howto_make_demod_filterbank_ccvc (unsigned int numvec, const char *name)
{
	return howto_demod_filterbank_ccvc_sptr(new howto_demod_filterbank_ccvc(numvec, name));
}
//----------------------------------------------------------------------------------

howto_demod_filterbank_ccvc::howto_demod_filterbank_ccvc(unsigned int numvec, unsigned int taps_per_filter, const std::vector<gr_complex> vtaps):gr_sync_block("demod_filterbank_cvcc",gr_make_io_signature(1,1,sizeof(gr_complex)),gr_make_io_signature(1,1,numvec*sizeof(gr_complex)))
{
	d_numvec=numvec;
	d_taps_per_filter=taps_per_filter;
	d_vfilters = std::vector<gri_fir_filter_with_buffer_ccc*>(d_numvec);
	std::vector<gr_complex> v_taps;
	for(unsigned int i=0;i<d_numvec;i++){
		d_vfilters[i]=new gri_fir_filter_with_buffer_ccc(v_taps);
	}

	set_taps(vtaps);
}

/*----------------------------------------------------------------------------------
constructor with filename
----------------------------------------------------------------------------------*/
howto_demod_filterbank_ccvc::howto_demod_filterbank_ccvc(unsigned int numvec, const char *name):gr_sync_block("demod_filterbank_cvcc",gr_make_io_signature(1,1,sizeof(gr_complex)),gr_make_io_signature(1,1,numvec*sizeof(gr_complex)))
{
	FILE *demod_file;
	if((demod_file=fopen(name,"r"))==NULL) throw std::runtime_error("file open error");
	std::vector<gr_complex> ab_vtaps;
	gr_complex z;
	std::vector<gr_complex> v_taps;
	float x,y;
	
	if((fscanf(demod_file,"%u %u\n", &d_numvec, &d_taps_per_filter))==EOF)
		throw std::runtime_error("reading file open error");
	printf("numvec=%u taps per filt=%u", d_numvec, d_taps_per_filter);
	assert(numvec==d_numvec);

	d_vfilters = std::vector<gri_fir_filter_with_buffer_ccc*>(d_numvec);
	ab_vtaps.resize(d_numvec*d_taps_per_filter);

	for(unsigned int i=0;i<d_numvec;i++){
		for(unsigned int j=0;j<d_taps_per_filter;j++){
			printf("loop number %u\n",j);
			fscanf(demod_file,"%f %f ", &x, &y);
			ab_vtaps[i*d_taps_per_filter + d_taps_per_filter-j-1]=gr_complex(x,y);
			printf("real=%f, complex=%f", ab_vtaps[i*d_taps_per_filter + d_taps_per_filter-j-1].real(),ab_vtaps[i*d_taps_per_filter + d_taps_per_filter-j-1].imag());
		}
		fscanf(demod_file,"\n");
	}

	for(unsigned int i=0;i<d_numvec;i++){
		d_vfilters[i]=new gri_fir_filter_with_buffer_ccc(v_taps);
	}
	printf("the program has passed this constructor\n");
	set_taps(ab_vtaps);
	fclose(demod_file);
}	

//---------------------------------------------------------------------------------

howto_demod_filterbank_ccvc::~howto_demod_filterbank_ccvc()
{
	for(unsigned int i=0;i<d_numvec;i++){
		delete d_vfilters[i];
	}
}


void howto_demod_filterbank_ccvc::set_taps(const std::vector<gr_complex> vtaps)
{
	unsigned int i,j;
	unsigned int ntaps=vtaps.size();

	d_vtaps.resize(d_numvec);

	for(i=0;i<d_numvec;i++){
		d_vtaps[i]=std::vector<gr_complex>(d_taps_per_filter);
		for(j=0;j<d_taps_per_filter;j++){
			d_vtaps[i][j]=vtaps[i*d_taps_per_filter+j];
		}

		d_vfilters[i]->set_taps(d_vtaps[i]);
	}

	set_history(d_taps_per_filter);

}

void howto_demod_filterbank_ccvc::print_taps()
{
	unsigned int i,j;
	for(i=0;i<d_numvec;i++){
		printf("filter[%d]:[",i);
		for(j=0;j<d_taps_per_filter;j++){
			printf(" %.4e+%.4ej",d_vtaps[i][j].real(),d_vtaps[i][j].imag());
		}
		printf("]\n\n");
	}
}


int howto_demod_filterbank_ccvc::work(int noutput_items,
				    gr_vector_const_void_star &input_items,
				    gr_vector_void_star &output_items)
{
	gr_complex *in=(gr_complex*)input_items[0];
	gr_complex *out=(gr_complex*)output_items[0];

	unsigned int n,i;
	for(n=0;n<noutput_items;n++){
		for(i=0;i<d_numvec;i++){
			*out++=d_vfilters[i]->filter(in[n]);
		}
	}

return noutput_items;
}
