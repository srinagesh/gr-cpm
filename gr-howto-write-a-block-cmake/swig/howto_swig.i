/* -*- c++ -*- */

#define HOWTO_API

%include "gnuradio.i"			// the common stuff
%include "gri_control_loop.i"
%import "digital_constellation.i"

//load generated python docstrings
%include "howto_swig_doc.i"


%{
#include "howto_square_ff.h"
#include "howto_square2_ff.h"
#include "howto_demod_filterbank_ccvc.h"
#include "howto_phase_correction_vcvc.h"
%}

GR_SWIG_BLOCK_MAGIC(howto,square_ff);
%include "howto_square_ff.h"

GR_SWIG_BLOCK_MAGIC(howto,square2_ff);
%include "howto_square2_ff.h"

GR_SWIG_BLOCK_MAGIC(howto,demod_filterbank_ccvc);
%include "howto_demod_filterbank_ccvc.h"

GR_SWIG_BLOCK_MAGIC(howto,phase_correction_vcvc);
%include "howto_phase_correction_vcvc.h"


#if SWIGGUILE
%scheme %{
(load-extension-global "libguile-gnuradio-howto_swig" "scm_init_gnuradio_howto_swig_module")
%}

%goops %{
(use-modules (gnuradio gnuradio_core_runtime))
%}
#endif
