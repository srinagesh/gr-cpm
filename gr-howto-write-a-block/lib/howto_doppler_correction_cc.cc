/* C++ file for implementation of doppler correction with gpredict */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <howto_doppler_correction_cc.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <cstdio>
#include <iostream>
#include <time.h>
using namespace std;

howto_doppler_correction_cc_sptr howto_make_doppler_correction_cc (double sampling_freq, unsigned int port, double int_freq, long int rxfreq)
{
	return howto_doppler_correction_cc_sptr(new howto_doppler_correction_cc(sampling_freq, port, int_freq, rxfreq));
}

/*----------------------------------------------------
*		CONSTRUCTOR
*---------------------------------------------------*/
howto_doppler_correction_cc::howto_doppler_correction_cc(double sampling_freq, unsigned int port, double int_freq, long int rxfreq):gr_sync_block("doppler_correction_cc",gr_make_io_signature(1,1,sizeof(gr_complex)), gr_make_io_signature(1,1,sizeof(gr_complex))), d_sampling_freq(sampling_freq), d_port(port)
{
	d_int_freq = int_freq;
	d_local_freq = int_freq;
	d_doppler=0;
	d_rxfreq=rxfreq;
	d_nco.set_freq(2 * M_PI * d_local_freq/d_sampling_freq);

	pthread_create(&socket_thread, 0, &(howto_doppler_correction_cc::start_thread), this);
	cout<<"creating socket thread"<<endl;
}


/*----------------------------------------------------
*		DESTRUCTOR
*---------------------------------------------------*/
howto_doppler_correction_cc::~howto_doppler_correction_cc()
{

}

/*----------------------------------------------------
*		WORK FUNCTION
*---------------------------------------------------*/
int howto_doppler_correction_cc::work(int noutput_items,
					gr_vector_const_void_star &input_items,
					gr_vector_void_star &output_items)
{
	gr_complex *in = (gr_complex*)input_items[0];
	gr_complex *out = (gr_complex*)output_items[0];

	gr_complex *local_out;
	unsigned int n,i;
	float t_real, t_imag;
	double d_ampl;
	d_ampl=1;
	d_nco.sincos (out,noutput_items,d_ampl);
	for(n=0; n<noutput_items;n++){
		t_real = in[n].real()*out[n].real() - in[n].imag()*out[n].imag();
		t_imag = in[n].real()*out[n].imag() + in[n].imag()*out[n].real();
		out[n] = gr_complex(t_real,t_imag);
	}

return noutput_items;
}

/*---------------------------------------------------
*		SOCKET THREAD
*--------------------------------------------------*/

void howto_doppler_correction_cc::socket_thread_work()
{
	cout<<"in the socket thread.."<<endl;
	int sockfd, newsockfd, portno, indata, response_length;
	socklen_t clilen;
	char buffer[128];
	char command_buffer[128];
	char response[128];
	int command_buffer_idx = 0;
	struct sockaddr_in serv_addr, cli_addr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
		cout<<"ERROR opening socket..."<<endl;
	bzero((char *)&serv_addr, sizeof(serv_addr));

	portno = (int)d_port;
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		cout<<"ERROR on binding socket..."<<endl;
	
	listen(sockfd,5);

	clilen = sizeof(cli_addr);

	while(1){
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
		if(newsockfd < 0)
			cout<<"ERROR on socket accept... "<<endl;
		bzero(buffer, 128);

		while(1){
			indata = read(newsockfd, buffer, 127);
			if(indata <= 0){
				cout<<"ERROR reading from socket..."<<endl;
				break;
			}

			//Processing gpredict response
			for(unsigned int i=0; i< indata; i++){
				command_buffer[command_buffer_idx++] = buffer[i];

				//End of a command
				if(buffer[i] == 10 || command_buffer_idx == 128){
					response_length = process_command(command_buffer, response);

					command_buffer_idx = 0;
					write(newsockfd, response, response_length);
				}

			}
		//	cout<<buffer<<endl;
			if(response_length == -1000)
				break;
		}
		if(response_length == -1000)
			break;

	}
}

int howto_doppler_correction_cc::process_command(char *command_in, char *response){
	int response_length = 0;
	int length;
	long int radio_freq;

	//Figure out which command has been issued
	switch(command_in[0]){
		case 'F':
			sscanf(command_in,"F %ld\n", &radio_freq);
		//	cout<<"Got command to change frequency to: "<<radio_freq<<endl;
			d_doppler = (float) radio_freq - d_rxfreq - d_int_freq;
			d_local_freq = -d_int_freq - d_doppler;
			d_nco.set_freq(2 * M_PI * d_local_freq/d_sampling_freq);
			memcpy(response, "RPRT 0", 6);
			return 6;
			break;

		case 'f':
			radio_freq = (long int) d_nco.get_freq();
			length = sprintf(response, "%ld\n", radio_freq);
		//	cout<<"frequency responded to by program"<<endl;
			return length;
			break;

		case 'q':
			return -1000;
			break;
	}
}



	
