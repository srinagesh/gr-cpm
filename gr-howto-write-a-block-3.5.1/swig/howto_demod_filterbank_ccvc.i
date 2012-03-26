GR_SWIG_BLOCK_MAGIC(howto,demod_filterbank_ccvc);

howto_demod_filterbank_ccvc_sptr howto_make_demod_filterbank_ccvc(unsigned int numvec, unsigned int taps_per_filter, const std::vector<gr_complex> vtaps);
howto_demod_filterbank_ccvc_sptr howto_make_demod_filterbank_ccvc (unsigned int numvec, const char *name);

class howto_demod_filterbank_ccvc:public gr_sync_block
{
private:
howto_demod_filterbank_ccvc(unsigned int numvec,unsigned int taps_per_filter, const std::vector<gr_complex> vtaps);
howto_demod_filterbank_ccvc(unsigned int numvec, const char *name);

public:
~howto_demod_filterbank_ccvc();

void set_taps(const std::vector<gr_complex> vtaps);
};
