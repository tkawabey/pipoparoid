
/*

  http://ja.wikipedia.org/wiki/DTMF


          DTMF�}�g���b�N�X   	
                ���Q (Hz)
            |1209 1336 1477 1633
     ----------------------------
     | 697  |  1    2    3    A
��   | 770  |  4    5    6    B
�Q   | 852  |  7    8    9    C
(Hz) | 941  |  *    0    #    D
     ----------------------------
�M�����o����
    50ms�ȏ�
�~�j�}���|�[�Y
    �אڂ���M���Ԃ̋x�~���Ԃ̍ŏ��l�B30ms�ȏ�
����
    �M�����o���Ԃƃ~�j�}���|�[�Y�̘a�B120ms�ȏ�


22000hz�̏ꍇ�A
DTFM�̂P��50ms�i�K�i�j�Ȃ̂ŁA
22000/(1000/50)=1100�A�C�e���K�v
bit Per ���W�̏ꍇ�́A1100Byte


697Hz�̏ꍇ�A�P�b�Ԃ�697��g�ł�
50ms�̏ꍇ�́A697/�i1000/50�j��34.85
�����l�Ɍv�Z����ƁA

697 = 34.85
770 = 38.5
850 = 42.6
941 = 47.5
1209= 60.45
1336= 66.8
1477= 73.85
1633= 81.65

�M�����g���΍�
    �M�����g���́}1.5%�ȓ�
(100/0.15)=0.0015
1100�A�C�e���̌덷��1.65


697 = 34.85�@=�@32.85-36.5
770 = 38.5   =  36.5-40.15
850 = 42.6   =  
941 = 47.5
1209= 60.45
1336= 66.8
1477= 73.85
1633= 81.65

*/

#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "dtfm.h"

//#define MY_DBG	1


#ifndef _WINDOWS
	#ifndef BIG_ENDIAN
		#define BIG_ENDIAN	1
	#endif
#endif 

#define PI					3.141592653589
#define MARGIN_PROPORTION	0.0015	// �M�����g���΍�
#define LARGE_DATA_MAX 30			// �덷�A�G���p�ɂR�O�A�C�e���m��
#ifndef NULL
    #define NULL    (void*)0
#endif
#define DTFM_INCRE_SIZE					1
#define DTFM_JUDGE_START_POS			0
#define DTFM_DTFM_WAVE_HI_MARGINE		0.35



static void _dtfm_init_ji(dtfm_judge_item* ji, int judge_data_cnt, double sample, int bits);


static char _get_matrix_val(int row_hz, int hight_hz);

static short _dtfm_cnv_u2_native_2_intel_byte(short val);
static int _dtfm_cnv_u4_native_2_intel_byte(int val);
static short _dtfm_cnv_u2_intel_2_native_byte(short val);
static int _dtfm_cnv_u4_intel_2_native_byte(int val);
static int _dtfm_near2power(int val);
static int dtfm_ft(int judge_data_cnt, dtfm_complex* p_complex);
static int dtfm_fft(int judge_data_cnt, dtfm_complex* p_complex);


int dtfm_parse(int judge_data_cnt, 
			  char* pVal, 
			  dtfm_judge_item* ji_fix, 
			  dtfm_judge_item* ji_wk, 
			  double sample, 
			  dtfm_complex* p_complex,
			  int bits) ;



typedef struct tMyWAVEFORMATEX
{
    unsigned short        wFormatTag;         /* format type */
    unsigned short        nChannels;          /* number of channels (i.e. mono, stereo...) */
    unsigned int       nSamplesPerSec;     /* sample rate */
    unsigned int       nAvgBytesPerSec;    /* for buffer estimation */
    unsigned short        nBlockAlign;        /* block size of data */
    unsigned short        wBitsPerSample;     /* number of bits per sample of mono data */
    unsigned short        cbSize;             /* the count in bytes of the size of */
				    /* extra information (after cbSize) */
} MyWAVEFORMATEX;


typedef struct _large_data
{
	double fft_val;
	double val;
	dtfm_judge_item* p_ji;
}large_data;






#if _CONSOLE

//#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


void pearse_print(char c, void* p_user_data)
{
	printf("%c", c);
}
void pearse_buffring_print(const char* str, void* p_user_data)
{
	printf("Sentence : %s\n", str);
}

int main(int argc, char *argv[]) 
{
	int i = 0;
	int mode = -1; 
		// 0:input from wave file
		// 1:text to save file
	char* p_out = NULL;


	int data_index = -1;
	for(i = 1; i < argc; i++) {
		if( argv[i][0] == '-' ) {
			if( strncmp(&argv[i][1], "m:", 2) == 0 ) {
				// Mode
				mode = atoi(&argv[i][3]);
				if( !(mode == 0 || mode ==1) ) {
					fprintf(stderr, "invalid mode. %s\n", argv[i]);
					return 1;
				}
			} else
			if( strncmp(&argv[i][1], "o:", 2) == 0 ) {
				// Output
				p_out = &argv[i][3];
			} else {
				fprintf(stderr, "invalid oprion. %s\n", argv[i]);
				return 1;
			}
		} else {
			data_index = i;
		}
	}
	if( data_index == -1 ) {
		fprintf(stderr, "invalid command line\n");
		return 1;
	}



	if( mode == 0 ) {
		dtfm_chanel channel;
		dtfm_judge  judge;

		dtfm_init_default_chanel(&channel);

		int buff_size = dtfm_load_from_file(argv[data_index], &channel, NULL, 0);
		if( buff_size < 0 ) {

		} else {
			unsigned char* p_buff = (unsigned char*)malloc(buff_size);
			unsigned char* p_wk_buff = p_buff;


			dtfm_init_judge_data(&channel, &judge);



			dtfm_load_from_file(argv[data_index], &channel, p_buff, buff_size);

			dtfm_pcm_to_text(&channel, &judge, pearse_print, NULL, p_buff, buff_size, NULL);

			printf("\nBuffring test\n");

			for(int iii = 0; iii < buff_size;  ) {
				if( buff_size - iii > 4000 ) {
					dtfm_buffring_pcm_to_text(&channel, &judge, pearse_buffring_print, NULL, p_wk_buff, 4000);
					p_wk_buff += 4000;
					iii += 4000;
				} else {
					dtfm_buffring_pcm_to_text(&channel, &judge, pearse_buffring_print, NULL, p_wk_buff, buff_size - iii);
					break;
				}
			}
			


			free(p_buff);
			dtfm_destroy_judge_data(&judge);
		}
	} else
	if( mode == 1 ) {
		dtfm_chanel channel;
		dtfm_init_default_chanel(&channel);

		int buff_size = dtfm_text_to_pcm(&channel, argv[data_index], NULL, 0);
		if( buff_size < 0 ) {

		} else {
			unsigned char* p_buff = (unsigned char*)malloc(buff_size);

			dtfm_text_to_pcm(&channel, argv[data_index], p_buff, buff_size);



			free(p_buff);
		}
	}


	return 0;
}
#endif /*_CONSOLE*/

// �R�[���o�b�N�̒�`
void dtfm_init_default_chanel(dtfm_chanel* p_chanel)
{
	if( p_chanel == NULL ) {
		return ;
	}
	memset(p_chanel, 0, sizeof(dtfm_chanel));

	p_chanel->signal_msec = DTFM_MIN_SIGNAL_MSEC;
	p_chanel->judge_msec = DTFM_DEFALT_JUDGE_MSEC;
	p_chanel->blank_msec = DTFM_MIN_BLANK_MSEC;
	p_chanel->amplitude = DTFM_DEFALT_AMP;
	p_chanel->sample_bit_per_sec = 22050;
	p_chanel->sample_bits = 8;
	p_chanel->chanel = DTFM_CHANEL_MONO;	// 1:Mono :2:Stereo
}

// ����f�[�^��������
bool dtfm_init_judge_data(dtfm_chanel* p_chanel, dtfm_judge* judge)
{
	int freq[8]={697,770,852,941,1209,1336,1477,1633};
	int i;
	double thous = 1000;

	if( p_chanel == NULL || judge == NULL ) {
		return false;
	}
	memset(judge, 0, sizeof(dtfm_judge));

	for( i = 0; i < 8; i++ ) {
		judge->ji[i].hz = judge->ji_wk[i].hz = freq[i];
	}

	judge->sample_per = (double)( ((double)p_chanel->sample_bit_per_sec) / 1000 );

	judge->one_signal_per_mse = thous / p_chanel->signal_msec;
	judge->one_signal_cnt     = (int)(p_chanel->sample_bit_per_sec / judge->one_signal_per_mse);

	judge->pause_per_mse      = thous / p_chanel->blank_msec;
	judge->pause_cnt          = (int)(p_chanel->sample_bit_per_sec / judge->pause_per_mse);


	judge->judge_per_mse = thous / p_chanel->judge_msec;
	judge->judge_cnt     = (int)(p_chanel->sample_bit_per_sec / judge->judge_per_mse);
	// FFT�p�ɁA2�ׂ̂���ɂ��킹��
	judge->judge_cnt = _dtfm_near2power(judge->judge_cnt);

	// Talk �̋�؂�̒���
	judge->separate_cnt = (int)p_chanel->sample_bit_per_sec/2;

	_dtfm_init_ji(
		judge->ji, 
		judge->judge_cnt, 
		judge->sample_per, 
		p_chanel->sample_bits);


	judge->p_complex = (dtfm_complex*)malloc(sizeof(dtfm_complex)*judge->one_signal_cnt);
	if( judge->p_complex == NULL ) {
		return false;
	}
	return true;
}

// ����f�[�^��j��
void dtfm_destroy_judge_data(dtfm_judge* judge)
{
	if( judge->p_complex != NULL ) {
		free( judge->p_complex );
	}
	if( judge->m_p_buff != NULL ) {
		free( judge->m_p_buff );
	}
	if( judge->m_p_str_buff != NULL ) {
		free( judge->m_p_str_buff );
	}
	memset(judge, 0, sizeof(dtfm_judge));
}




/*	@brief	�����񂩂�ADTFM�f�[�^(PCM)���쐬
 *
 *	@param	p_chanel	�`�����l�����
 *	@param	p_txt	�ϊ���������
 *	@param	p_buff	�ϊ���o�b�t�@�BNULL���w�肷��ƁA�߂�l�ŕK�v���o�b�t�@�����擾�ł���
 *	@param	buff_size	�ϊ���̃o�b�t�@��
 *
 *	@retval	0	�ϊ�����
 *	@retval	> 0	�K�v�ȃo�b�t�@��
 *	@retval	-1	�������[�G���[��
 *	@retval	-2	�o�b�t�@�T�C�Y(buff_size)���s��
 *	@retval	-3	p_txt���󕶎�
 *	@verbatim
 *	@endverbatim
 *	
 */
int dtfm_text_to_pcm(
		dtfm_chanel* p_chanel, 
		const char* p_txt, 
		void* p_buff, 
		int buff_size)
{
	unsigned long data_cnt = 0;
	const char* p_save_text = p_txt;
	int freq[8]={697,770,852,941,1209,1336,1477,1633};
	const char* key_array = "112233Aa445566Bb778899Cc**00##Dd";
	short	*p_sig_array[16];
	short	*p_sig_buff = NULL;
	short	*p_sig_work = NULL;
	short	*p_data_buff = NULL;
	unsigned int i,j;
	int ret = -1;

	double per_one_msec = 1000 / p_chanel->signal_msec;
	unsigned int one_signal_cnt = (unsigned int)( p_chanel->sample_bit_per_sec / per_one_msec );
	unsigned int pause_cnt = (unsigned int)(p_chanel->sample_bit_per_sec/(1000/p_chanel->blank_msec) );
	unsigned int volume_changing_cnt = p_chanel->sample_bit_per_sec/1000;
	

	while( *p_txt != '\0' ) {
		data_cnt += one_signal_cnt;
		if( *(p_txt+1) != '\0' ) {
			data_cnt += pause_cnt;
		}
		p_txt++;
	}
	if( data_cnt == 0 ) {
		ret = -3;
		goto END_;
	}
	if( p_buff == NULL ) {
		ret = data_cnt * (p_chanel->sample_bits/8);
		goto END_;
	}
	if( buff_size < (data_cnt * (p_chanel->sample_bits/8)) ) {
		ret = -2;
		goto END_;
	}

	// �������[�̊m��
	p_sig_buff = (short*)malloc(one_signal_cnt*sizeof(short)*16);
	if( p_sig_buff == NULL) {
		goto END_;
	}
	memset(p_sig_buff, 0, one_signal_cnt*sizeof(short)*16);
	p_data_buff = (short*)malloc(data_cnt*sizeof(short));
	if( p_data_buff == NULL) {
		goto END_;
	}
	memset(p_data_buff, 0, data_cnt*sizeof(short));


	// �g�[���f�[�^���쐬
	for(i = 1; i < 17 ;i++){
		int hight_freq=freq[((i-1) % 4)+4];
		int low_freq =freq[(i-1) / 4];
		double amp = (double)p_chanel->amplitude;
		if( p_chanel->sample_bits == 16 ) {
			amp = 10000;
		}
		double hight_angle_per_sample = 2*PI*hight_freq/p_chanel->sample_bit_per_sec;	//�p���x�i�P�ʁF[rad/sample]�j
		double low_angle_per_sample   = 2*PI*low_freq /p_chanel->sample_bit_per_sec;	//�p���x�i�P�ʁF[rad/sample]�j

		int index = one_signal_cnt * ( i - 1 );
		p_sig_array[i-1] = p_sig_work = p_sig_buff + ( one_signal_cnt * ( i - 1 ) );

		for( j = 0; j < one_signal_cnt; j++ ) {
			p_sig_work[j] = (short)(amp*(sin(hight_angle_per_sample*j)+sin(low_angle_per_sample*j)));
		}


		//�Ō�ɐU�����O�ɂȂ����n�_��T��
		unsigned int last_point;
		for(last_point=one_signal_cnt-1;p_sig_work[last_point]*p_sig_work[last_point-1]>0;last_point--) {
			p_sig_work[last_point]=0;
		}
		last_point++;

		double my_delta=double(1)/volume_changing_cnt;
		//���ʂ����X�ɑ傫��
		for(j=0;j<volume_changing_cnt;j++)
			p_sig_work[j]=(short)my_delta*j*p_sig_work[j];
		//���ʂ����X�ɏ�����
		for(j=last_point-volume_changing_cnt;j<last_point;j++){
			int d=j-last_point;
			p_sig_work[j]=-(short)my_delta*d*p_sig_work[j];
		}

	}

	// �w�肳�ꂽTEXT�̃g�[����
	p_sig_work = p_data_buff;
	p_txt = p_save_text;
	while( *p_txt != '\0' ) {
		char* p_find = strchr(key_array, *p_txt);
		if( p_find != NULL ) {
			int index = (p_find - key_array)/2;

			memcpy(p_sig_work, p_sig_array[index], one_signal_cnt*sizeof(short));
		}

		p_sig_work += one_signal_cnt;
		if( *(p_txt+1) != '\0' ) {
			p_sig_work += pause_cnt;
		}
		p_txt++;
	}

	// ���[�U�[�w��̃o�b�t�@�ɃR�s�[
	if( p_chanel->sample_bits == 8 ) {
		unsigned char* p_u = (unsigned char*)malloc(data_cnt*sizeof(unsigned char));
		if( p_u == NULL ) {
			goto END_;
		}
		for( j = 0; j < data_cnt; j++ ) {
			p_u[j] = (unsigned char)(p_data_buff[j]+128);
		}
		memcpy(p_buff, p_u, data_cnt*sizeof(unsigned char));
	} else {
		memcpy(p_buff, p_data_buff, data_cnt*sizeof(short));
	}



	ret = 0;
	goto END_;
END_:
	if( p_sig_buff != NULL ) {
		free(p_sig_buff);
	}
	if( p_data_buff != NULL ) {
		free(p_data_buff);
	}


	return ret;
}

static void _dtfm_buffring_judge_cb(char c, void* p_user_data)
{
	dtfm_judge* judge = (dtfm_judge*)p_user_data;

	if( judge->m_p_str_buff == NULL ) {
		judge->m_p_str_buff = (char*)malloc(256);
		if( judge->m_p_str_buff == NULL ) {
			return ;
		}
		judge->m_n_str_buff_size = 256;
		memset(judge->m_p_str_buff, 0, judge->m_n_str_buff_size);
	}  else
	if( (judge->m_n_str_buff_size - judge->m_n_str_cur_size) <= 10 ) {
		// �ǉ�����o�b�t�@���s�����Ă���B
		int new_size = judge->m_n_str_buff_size + 256;
		char* p_new_buf = (char*)malloc( new_size );
		if( p_new_buf == NULL ) {
			return ;
		}
		memcpy(p_new_buf, judge->m_p_str_buff, judge->m_n_cur_size);
		free( judge->m_p_str_buff );
		judge->m_p_str_buff      = p_new_buf;
		judge->m_n_str_buff_size = new_size;
	}
	judge->m_p_str_buff[judge->m_n_str_cur_size] = c;

	judge->m_n_str_cur_size++;
}
/*	
 *
 *	@param	p_chanel	a parameter of type dtfm_chanel*
 *	@param	judge	a parameter of type dtfm_judge*
 *	@param	p_data	a parameter of type void*
 *	@param	data_size	a parameter of type int
 *
 *	@retval	value	DESC
 *	@retval	value	DESC
 */
int  dtfm_buffring_pcm_to_text(
		dtfm_chanel* p_chanel, 
		dtfm_judge* judge, 
		dtfm_parse_buffring_cb p_callback,
		void* p_user_data,
		void* p_data, 
		int data_size)
{
	int ret = 0;
	int able_size = (judge->one_signal_cnt + judge->pause_cnt)*( p_chanel->sample_bits / 8 );
	int analyzed_size = 0;
	int now_txt_cnt = judge->m_n_str_cur_size;

	// �o�b�t�@�����O
	if( judge->m_p_buff == NULL ) {
		// �o�b�t�@���܂����蓖�ĂĂ��Ȃ�
		judge->m_p_buff = (unsigned char*)malloc( data_size * 2 );
		judge->m_n_buff_size = data_size * 2;
	} else
	if( (judge->m_n_buff_size - judge->m_n_cur_size -10) <= data_size ) {
		// �ǉ�����o�b�t�@���s�����Ă���B
		int new_size = judge->m_n_cur_size + (data_size * 2);
		unsigned char* p_new_buf = (unsigned char*)malloc( new_size );
		if( p_new_buf == NULL ) {
			return -1;
		}
		memcpy(p_new_buf, judge->m_p_buff, judge->m_n_cur_size);
		free( judge->m_p_buff );

		judge->m_p_buff = p_new_buf;
		judge->m_n_buff_size = new_size;
	}
	memcpy(&judge->m_p_buff[judge->m_n_cur_size], p_data, data_size);
	judge->m_n_cur_size += data_size;

	if( able_size > judge->m_n_cur_size ) {
		return 0;
	}

	// DTFM���
	ret = dtfm_pcm_to_text(
				p_chanel, 
				judge, 
				_dtfm_buffring_judge_cb,
				judge,
				judge->m_p_buff, 
				judge->m_n_cur_size,
				&analyzed_size);
	if( analyzed_size != 0 ) {
		// ��͂ł����o�b�t�@��O�ɋl�߂�
		memmove(judge->m_p_buff, &judge->m_p_buff[analyzed_size], 
			judge->m_n_cur_size - analyzed_size);
		judge->m_n_cur_size -= analyzed_size;

		if( now_txt_cnt == judge->m_n_str_cur_size && judge->m_n_str_cur_size != 0 ) {
			
			judge->m_n_wk_sep_size += analyzed_size; 
			

			if( judge->m_n_wk_sep_size > (judge->separate_cnt*( p_chanel->sample_bits / 8 )) ) {
				// DTFM�ȊO�̔g�`��0.5�b��������ATEXT���v�����g����
				if( p_callback != NULL  && judge->m_n_str_cur_size != 0) {
					p_callback(judge->m_p_str_buff, p_user_data);
				}

				memset(judge->m_p_str_buff, 0, judge->m_n_str_buff_size);
				judge->m_n_str_cur_size = 0; 
				judge->m_n_wk_sep_size = 0; 
			}
		}



	}

	return ret;
}

/*	@brief	DTFM�f�[�^(PCM)���A�����f�[�^�ɕϊ�����
 *
 *	@param	p_chanel	�`�����l�����
 *	@param	judge	����f�[�^
 *	@param	p_callback	���茋�ʃR�[���o�b�N
 *	@param	p_user_data	�R�[���o�b�N�ɓn�����[�U�[�f�[�^
 *	@param	p_data	�ϊ���DTFM�f�[�^
 *	@param	data_size	�ϊ���DTFM�f�[�^�̃f�[�^�T�C�Y
 *	@param	p_analyzed_size	��͂����f�[�^�T�C�Y���擾�BNULL���w�肵���ꍇ�́A�V�O�i���̕s�����f�[�^����͂���BCOMIT�����Ȃǂɂ��悤
 *
 *	@retval	0	��ɂO
 *	@verbatim
 *	@endverbatim
 *	
 */
int  dtfm_pcm_to_text(
		dtfm_chanel* p_chanel, 
		dtfm_judge* judge, 
		dtfm_parse_cb p_callback,
		void* p_user_data,
		void* p_data, 
		int data_size,
		int *p_analyzed_size)
{
	int data_cnt = data_size / ( p_chanel->sample_bits / 8 );
	int i, j;
	int ret = 0;


	//---------------------------------
	// �V�����v�Z���Ă��Ȃ��ꍇ�͌v�Z����B
	//
	if( p_chanel->wave_hi.u2 == 0 ) {

		if( p_chanel->sample_bits == 8 ) {
			unsigned char max_wave = 0;
			unsigned char *p_wk_data = (unsigned char*)p_data;

			for( i = 0; i < data_cnt; i++) {
				if( max_wave < p_wk_data[ i ] ) {
					max_wave = p_wk_data[ i ];
				}
			}

			if( max_wave < 50 ) {
				p_chanel->wave_hi.u1 = 128 + 50;
				p_chanel->wave_row.u1 = 128 - 50;
			} else {
				double d = (double)max_wave;
				d -= 128;

				p_chanel->wave_hi.u1 = 128 + (unsigned char)(d * DTFM_DTFM_WAVE_HI_MARGINE);
				p_chanel->wave_row.u1 = 128 - (unsigned char)(d * DTFM_DTFM_WAVE_HI_MARGINE);
			}

		} else 
		if( p_chanel->sample_bits == 16 ) {
			short max_wave = 0;
			short *p_wk_data = (short*)p_data;
			
			for( i = 0; i < data_cnt; i++) {
				if( max_wave < p_wk_data[ i ] ) {
					max_wave = p_wk_data[ i ];
				}
			}

			if( max_wave < 1000 ) {
				p_chanel->wave_hi.u2 = (short)((double)1000 * DTFM_DTFM_WAVE_HI_MARGINE);
				p_chanel->wave_row.u2 = p_chanel->wave_hi.u1 * -1;
			} else {
				p_chanel->wave_hi.u2 = (short)((double)max_wave* DTFM_DTFM_WAVE_HI_MARGINE);
				p_chanel->wave_row.u2 = p_chanel->wave_hi.u2 * -1;
			}
		}
	}






	//------------------------------------------
	//	�f�[�^�̉�͏���
	//
	if( p_chanel->sample_bits == 8 ) {
		unsigned char *p_wk_data = (unsigned char*)p_data;

		for( i = 0; i < data_cnt; i++, p_wk_data++) {
			if( !(p_chanel->wave_row.u1  <= *p_wk_data && *p_wk_data <= p_chanel->wave_hi.u1  )  ) {
				unsigned char max_wave = 0;
				unsigned char max_wave_hi = 0;
				if( judge->judge_cnt > (data_cnt-i) ) {
					break;
				}



				for(j = 0; j < judge->judge_cnt; j++ ) {
					judge->p_complex[j].r = (double)p_wk_data[j+DTFM_JUDGE_START_POS];
					judge->p_complex[j].i = 0.0;
					judge->p_complex[j].R = 0.0;

					if( max_wave < p_wk_data[ j+DTFM_JUDGE_START_POS ] ) {
						max_wave = p_wk_data[ j+DTFM_JUDGE_START_POS ];
					}
				}

				{
					double d = (double)max_wave;
					max_wave_hi = 128 + (unsigned char)(d * DTFM_DTFM_WAVE_HI_MARGINE);
					if( max_wave_hi > p_chanel->wave_hi.u1  ) {
						p_chanel->wave_hi.u1 = 128 + (unsigned char)(d * DTFM_DTFM_WAVE_HI_MARGINE);
						p_chanel->wave_row.u1 = 128 - (unsigned char)(d * DTFM_DTFM_WAVE_HI_MARGINE);
					}
				}


				// dtfm_parse
				if( (data_cnt-i) > ( judge->one_signal_cnt + judge->pause_cnt ) ) {
					char val;
					if( dtfm_parse(
							judge->judge_cnt, 
							&val, 
							judge->ji, 
							judge->ji_wk, 
							judge->sample_per, 
							judge->p_complex, 
							p_chanel->sample_bits) != 1 ) {
						i += DTFM_INCRE_SIZE;
						p_wk_data += DTFM_INCRE_SIZE;
					} else {
						ret++;

						p_callback(val, p_user_data);

						// ��͏o�����A�M���𔻒肷��f�[�^�T�C�Y���X�L�b�v
						i += judge->one_signal_cnt;
						p_wk_data += judge->one_signal_cnt;
						// �~�j�}���|�[�Y���X�L�b�v
						i += judge->pause_cnt;
						p_wk_data += judge->pause_cnt;
					}				
				} else {
					if( p_analyzed_size == NULL ) {

						char val;
						if( dtfm_parse(
								judge->judge_cnt, 
								&val, 
								NULL, 
								judge->ji_wk, 
								judge->sample_per, 
								judge->p_complex, 
								p_chanel->sample_bits) != 1 ) {
							i += DTFM_INCRE_SIZE;
							p_wk_data += DTFM_INCRE_SIZE;
						} else {
							ret++;

							p_callback(val, p_user_data);

							break;
						}
					} else {
						break;
					}
				}
			}
		}
	} else 
	if( p_chanel->sample_bits == 16 ) {
		short *p_wk_data = (short*)p_data;

		for( i = 0; i < data_cnt; i++, p_wk_data++) {
			if( !(p_chanel->wave_row.u2  <= *p_wk_data && *p_wk_data <= p_chanel->wave_hi.u2  )  ) {

				short max_wave = 0;
				short max_wave_hi = 0;
				int   hirow_cnt = 0;
				short hi  = (short)(p_chanel->wave_hi.u2 * 1.25);
				short row = (short)(p_chanel->wave_row.u2 * 1.25);
				

				if( judge->judge_cnt > (data_cnt-i) ) {
					break;
				}
				for(j = 0; j < judge->judge_cnt; j++ ) {
					if( !(row  <= p_wk_data[j+DTFM_JUDGE_START_POS] && 
						  p_wk_data[j+DTFM_JUDGE_START_POS] <= hi )  ) {
						hirow_cnt++;
					}
					judge->p_complex[j].r = (double)p_wk_data[j+DTFM_JUDGE_START_POS];
					judge->p_complex[j].i = 0.0;
					judge->p_complex[j].R = 0.0;
					if( max_wave < p_wk_data[ j+DTFM_JUDGE_START_POS ] ) {
						max_wave = p_wk_data[ j+DTFM_JUDGE_START_POS ];
					}
				}
				if( hirow_cnt < 6  ) {
					i += judge->judge_cnt;
					p_wk_data += judge->judge_cnt;					
					continue;
				}


				{
					max_wave_hi = (short)((double)max_wave* DTFM_DTFM_WAVE_HI_MARGINE);
					if( max_wave_hi > p_chanel->wave_hi.u2  ) {
						p_chanel->wave_hi.u2 = max_wave_hi;
						p_chanel->wave_row.u2 = p_chanel->wave_hi.u2 * -1;
					}
				}



				// dtfm_parse
				if( (data_cnt-i) > ( judge->one_signal_cnt + judge->pause_cnt ) ) {
					char val;
					if( dtfm_parse(
							judge->judge_cnt, 
							&val, 
							judge->ji, 
							judge->ji_wk, 
							judge->sample_per, 
							judge->p_complex, 
							p_chanel->sample_bits) != 1 ) {

						i += DTFM_INCRE_SIZE;
						p_wk_data += DTFM_INCRE_SIZE;
					} else {
						ret++;

						p_callback(val, p_user_data);

						// ��͏o�����A�M���𔻒肷��f�[�^�T�C�Y���X�L�b�v
						i += judge->one_signal_cnt;
						p_wk_data += judge->one_signal_cnt;
						// �~�j�}���|�[�Y���X�L�b�v
						i += judge->pause_cnt;
						p_wk_data += judge->pause_cnt;
					}				
				} else {
					if( p_analyzed_size == NULL ) {
						char val;
						if( dtfm_parse(
								judge->judge_cnt, 
								&val, 
								NULL, 
								judge->ji_wk, 
								judge->sample_per, 
								judge->p_complex, 
								p_chanel->sample_bits) != 1 ) {
							i += DTFM_INCRE_SIZE;
							p_wk_data += DTFM_INCRE_SIZE;
						} else {
							ret++;

							p_callback(val, p_user_data);

							break;
						}
					} else {
						break;
					}
				}
			}
		}
	}

	if( p_analyzed_size != NULL ) {
		if( p_chanel->sample_bits == 8 ) {
			*p_analyzed_size = i * sizeof(unsigned char);
		} else {
			*p_analyzed_size = i * sizeof(short);
		}
	}
	return ret;
}

/*	@brief	�w�肳�ꂽPCM������WAVE���t�@�C�����A��͂���TEXT�ɕϊ�����B
 *
 *	@param	p_file_path	�t�@�C���̃p�X
 *	@param	p_callback	���茋�ʃR�[���o�b�N
 *	@param	p_user_data	�R�[���o�b�N�ɓn�����[�U�[�f�[�^
 *
 *	@retval	��=0	TEXT�̃J�E���g
 *	@retval	-2	�w�肳�ꂽ�����t�@�C�����I�[�v���ł��܂���B
 *	@retval	-3	�w�肳�ꂽ�����t�@�C�����s��
 *	
 */
int dtfm_pcm_to_text_from_file(
		const char* p_file_path, 
		dtfm_parse_cb p_callback,
		void* p_user_data)
{
	int ret = -1, sts;
	FILE*				fp = NULL;
	dtfm_chanel			chanel;
	dtfm_judge			judge;
	long				size_format = 0;
	long				size = 0;
	MyWAVEFORMATEX		tagFormat;
	unsigned char		uc4[4];
	unsigned char*		p_buff = NULL;
	int i, j,  header_size = 0;
	dtfm_wave_hi		hi,row;
	int					cur_size = 0;
	int					reading_size;
	
	dtfm_init_default_chanel( &chanel );



	fp = fopen(p_file_path, "rb");
	if( fp == NULL ) {
		ret = -2;	// �t�@�C�� NOT�@FOUND
		goto END_;
	}

	//------------------------------------------
	//	�w�b�_�[���
	//
	ret = -3;	// �t�H�[�}�b�g�G���[
	sts = fread(uc4, 4, 1, fp);
	if( sts != 1 ) {
		goto END_;
	}
	header_size += 4;
	if( memcmp(uc4, "RIFF", 4 ) != 0 ) {
		goto END_;
	}


	sts = fread(uc4, 4, 1, fp);
	if( sts != 1 ) {
		goto END_;
	}
	header_size += 4;
	memcpy(&size, uc4, 4);



	sts = fread(uc4, 4, 1, fp);
	if( sts != 1 ) {
		goto END_;
	}
	header_size += 4;
	if( memcmp(uc4, "WAVE", 4 ) != 0 ) {
		goto END_;
	}



	sts = fread(uc4, 4, 1, fp);
	if( sts != 1 ) {
		goto END_;
	}
	header_size += 4;
	if( memcmp(uc4, "fmt ", 4 ) != 0 ) {
		goto END_;
	}



	sts = fread(uc4, 4, 1, fp);
	if( sts != 1 ) {
		goto END_;
	}
	header_size += 4;
	memcpy(&size_format, uc4, 4);
	size_format = _dtfm_cnv_u4_intel_2_native_byte(size_format);



	sts = fread(&tagFormat, 16, 1, fp);
	if( sts != 1 ) {
		goto END_;
	}
	header_size += size_format;
	tagFormat.wFormatTag = _dtfm_cnv_u2_intel_2_native_byte( tagFormat.wFormatTag );
	tagFormat.nChannels = _dtfm_cnv_u2_intel_2_native_byte( tagFormat.nChannels );
	tagFormat.nSamplesPerSec = _dtfm_cnv_u4_intel_2_native_byte( tagFormat.nSamplesPerSec );
	tagFormat.nAvgBytesPerSec = _dtfm_cnv_u4_intel_2_native_byte( tagFormat.nAvgBytesPerSec );
	tagFormat.nBlockAlign = _dtfm_cnv_u2_intel_2_native_byte( tagFormat.nBlockAlign );
	tagFormat.wBitsPerSample = _dtfm_cnv_u2_intel_2_native_byte( tagFormat.wBitsPerSample );
	if( tagFormat.wFormatTag != 1 ) {
		// PCM�łȂ��B
		goto END_;
	}
	chanel.chanel = tagFormat.nChannels == 1 ? DTFM_CHANEL_MONO : DTFM_CHANEL_STEREO;
	chanel.sample_bits = (unsigned short)tagFormat.wBitsPerSample;
	chanel.sample_bit_per_sec = tagFormat.nSamplesPerSec;

	size_format -= 16;
	if( size_format != 0 ) {
		sts = fread(uc4, 1, 1, fp);
		if( sts != 1 ) {
			goto END_;
		}
		size_format--;
	}



	sts = fread(uc4, 4, 1, fp);
	if( sts != 1 ) {
		goto END_;
	}
	header_size += 4;
	if( memcmp(uc4, "fact", 4 ) == 0 ) {
		int chunkSize = 0;
		
		sts = fread(uc4, 4, 1, fp);
		if( sts != 1 ) {
			goto END_;
		}
		header_size += 4;
		memcpy(&chunkSize, uc4, 4);
		chunkSize = _dtfm_cnv_u4_intel_2_native_byte(chunkSize);
		if( chunkSize != 0 ) {
			sts = fread(uc4, 1, 1, fp);
			if( sts != 1 ) {
				goto END_;
			}
			header_size += 1;
			chunkSize--;
		}
		sts = fread(uc4, 4, 1, fp);
		if( sts != 1 ) {
			goto END_;
		}
		header_size += 4;
	}


	if( memcmp(uc4, "data", 4 ) != 0 ) {
		goto END_;
	}



	sts = fread(uc4, 4, 1, fp);
	if( sts != 1 ) {
		goto END_;
	}
	memcpy(&size, uc4, 4);
	size = _dtfm_cnv_u4_intel_2_native_byte(size);

	
	// ����f�[�^��������
	dtfm_init_judge_data( &chanel, &judge );


	p_buff = (unsigned char*)malloc( (judge.one_signal_cnt + judge.judge_cnt + judge.pause_cnt) * (chanel.sample_bits/8) * 2 );
	if( p_buff == NULL ) {
		goto END_;
	}
	//------------------------------------------
	//	�U�����擾
	//
	while( 1 ) {

		int data_size = fread(p_buff, 1, judge.one_signal_cnt*(chanel.sample_bits/8), fp);
		if( data_size <= 0 ) {
			break;
		}
		int data_cnt = data_size / (chanel.sample_bits/8);

		if( chanel.sample_bits == 8 ) {
			unsigned char *p_wk_data = (unsigned char*)p_buff;
			for(i = 0; i < data_cnt; i++ ) {
				if( chanel.wave_hi.u1 < p_wk_data[i] ) {
					chanel.wave_hi.u1 = p_wk_data[i];
				}
			}
		} else
		if( chanel.sample_bits == 16 ) {
			short *p_wk_data = (short*)p_buff;

			for(i = 0; i < data_cnt; i++ ) {
				if( chanel.wave_hi.u2 < p_wk_data[i] ) {
					chanel.wave_hi.u2 = p_wk_data[i];
				}
			}
		}
	}

	// ���肷��V����臒l�����߂�
	if( chanel.sample_bits == 8 ) {
		double d = (double)chanel.wave_hi.u1;
		d -= 128;

		hi.u1 = 128 + (unsigned char)(d * DTFM_DTFM_WAVE_HI_MARGINE);
		row.u1 = 128 - (unsigned char)(d * DTFM_DTFM_WAVE_HI_MARGINE);
	} else
	if( chanel.sample_bits == 16 ) {
		hi.u2 = (short)((double)chanel.wave_hi.u2 * DTFM_DTFM_WAVE_HI_MARGINE);
		row.u2 = hi.u2*-1;
	}
	if( fp != NULL ) {
		fclose( fp );
		fp = NULL;
	}


	// ���I�[�v��
	fp = fopen(p_file_path, "rb");
	if( fp == NULL ) {
		ret = -2;	// �t�@�C�� NOT�@FOUND
		goto END_;
	}
	// �w�b�_�[���X�L�b�v
	sts = fread(p_buff, header_size, 1, fp);
	if( sts != 1 ) {
		goto END_;
	}

	//------------------------------------------
	//	�f�[�^�̉�͏���
	//
	ret = 0;
	cur_size = 0;
	reading_size = (judge.one_signal_cnt + judge.pause_cnt )*(chanel.sample_bits/8);
	while( 1 ) {

		int readed_size = fread(&p_buff[cur_size], 1, reading_size, fp);
		if( readed_size <= 0 ) {
			break;
		}
		int data_size = cur_size + readed_size;
		int data_cnt = data_size / (chanel.sample_bits/8);

		if( chanel.sample_bits == 8 ) {
			unsigned char *p_wk_data = (unsigned char*)p_buff;

			for( i = 0; i < data_cnt; i++, p_wk_data++) {
				if( (row.u1 <= *p_wk_data && *p_wk_data <= hi.u1  )  ) {
					continue;
				}
				if( judge.judge_cnt > (data_cnt-i) ) {
					break;
				}
				for(j = 0; j < judge.judge_cnt; j++ ) {
					judge.p_complex[j].r = (double)p_wk_data[j+DTFM_JUDGE_START_POS];
					judge.p_complex[j].i = 0.0;
					judge.p_complex[j].R = 0.0;
				}

				// dtfm_parse
				if( (data_cnt-i) > judge.one_signal_cnt ) {
					char val;

					if( dtfm_parse(
							judge.judge_cnt, 
							&val, 
							judge.ji, 
							judge.ji_wk, 
							judge.sample_per, 
							judge.p_complex, 
							chanel.sample_bits) != 1 ) {

						i += DTFM_INCRE_SIZE;
						p_wk_data += DTFM_INCRE_SIZE;
					} else {
						ret++;
						if( p_callback != NULL ) {
							p_callback(val, p_user_data);
						}
						// ��͏o�����A�M���𔻒肷��f�[�^�T�C�Y���X�L�b�v
						i += judge.one_signal_cnt;
						p_wk_data += judge.one_signal_cnt;


						// �~�j�}���|�[�Y���X�L�b�v
						int rest_buff_cnt = data_cnt - i;
						if( judge.pause_cnt < rest_buff_cnt ) {
							i += judge.pause_cnt;
							p_wk_data += judge.pause_cnt;
						} else {
							rest_buff_cnt = judge.pause_cnt - rest_buff_cnt;
							cur_size = 0;
							sts = fread(p_buff, 1, rest_buff_cnt*(chanel.sample_bits/8), fp);
							i += judge.pause_cnt;
						}
					}
				} else {
					if( reading_size != readed_size ) {
						// �t�@�C���̏I�[
						
						char val;

						if( dtfm_parse(
								judge.judge_cnt, 
								&val, 
								judge.ji, 
								judge.ji_wk, 
								judge.sample_per, 
								judge.p_complex, 
								chanel.sample_bits) != 1 ) {

							i += DTFM_INCRE_SIZE;
							p_wk_data += DTFM_INCRE_SIZE;
							continue;
						} else {
							ret++;
							if( p_callback != NULL ) {
								p_callback(val, p_user_data);
							}
							break;
						}

						i += judge.one_signal_cnt;
					}
					break;
				}
			}


			if( i < data_cnt ) {
				int distance_size = i * (chanel.sample_bits/8);
				cur_size = data_size - distance_size;

				memmove(p_buff, &p_buff[distance_size], cur_size);
			} else {
				cur_size = 0;
			}
		} else
		if( chanel.sample_bits == 16 ) {
			short *p_wk_data = (short*)p_buff;

			for( i = 0; i < data_cnt; i++, p_wk_data++) {
				if( (row.u2 <= *p_wk_data && *p_wk_data <= hi.u2  )  ) {
					continue;
				}
				if( judge.judge_cnt > (data_cnt-i) ) {
					break;
				}
				int   hirow_cnt = 0;
				short _hi  = (short)(hi.u2 * 1.25);
				short _row = (short)(row.u2 * 1.25);


				for(j = 0; j < judge.judge_cnt; j++ ) {
					if( !(_row  <= p_wk_data[j+DTFM_JUDGE_START_POS] && 
						  p_wk_data[j+DTFM_JUDGE_START_POS] <= _hi )  ) {
						hirow_cnt++;
					}
					judge.p_complex[j].r = (double)p_wk_data[j+DTFM_JUDGE_START_POS];
					judge.p_complex[j].i = 0.0;
					judge.p_complex[j].R = 0.0;
				}


				if( hirow_cnt < 3  ) {
					if( (data_cnt-i) > judge.one_signal_cnt ) {
						i += judge.judge_cnt;
						p_wk_data += judge.judge_cnt;
					}
				} else {

					// dtfm_parse
					if( (data_cnt-i) > judge.one_signal_cnt ) {
						char val;

						if( dtfm_parse(
								judge.judge_cnt, 
								&val, 
								judge.ji, 
								judge.ji_wk, 
								judge.sample_per, 
								judge.p_complex, 
								chanel.sample_bits) != 1 ) {

							i += DTFM_INCRE_SIZE;
							p_wk_data += DTFM_INCRE_SIZE;
						} else {
							ret++;
							if( p_callback != NULL ) {
								p_callback(val, p_user_data);
							}
							// ��͏o�����A�M���𔻒肷��f�[�^�T�C�Y���X�L�b�v
							i += judge.one_signal_cnt;
							p_wk_data += judge.one_signal_cnt;


							// �~�j�}���|�[�Y���X�L�b�v
							int rest_buff_cnt = data_cnt - i;
							if( judge.pause_cnt < rest_buff_cnt ) {
								i += judge.pause_cnt;
								p_wk_data += judge.pause_cnt;
							} else {
								rest_buff_cnt = judge.pause_cnt - rest_buff_cnt;
								cur_size = 0;
								sts = fread(p_buff, 1, rest_buff_cnt*(chanel.sample_bits/8), fp);
								i += judge.pause_cnt;
							}
						}
					} else {
						if( reading_size != readed_size ) {
							// �t�@�C���̏I�[
							
							char val;

							if( dtfm_parse(
									judge.judge_cnt, 
									&val, 
									judge.ji, 
									judge.ji_wk, 
									judge.sample_per, 
									judge.p_complex, 
									chanel.sample_bits) != 1 ) {

								i += DTFM_INCRE_SIZE;
								p_wk_data += DTFM_INCRE_SIZE;
								continue;
							} else {
								ret++;
								if( p_callback != NULL ) {
									p_callback(val, p_user_data);
								}
								break;
							}

							i += judge.one_signal_cnt;
						}
						break;
					}
				}
			}

			// ����͂ł��܂����o�b�t�@�́A�A�h���X�̐擪�ɋl�߂�
			if( i < data_cnt ) {
				int distance_size = i * (chanel.sample_bits/8);
				cur_size = data_size - distance_size;

				memmove(p_buff, &p_buff[distance_size], cur_size);
			} else {
				cur_size = 0;
			}
		}
	}
END_:
	if( fp != NULL ) {
		fclose( fp );
	}
	if( p_buff != NULL ) {
		free( p_buff );
	}
	return ret;
}

/*	@brief	�t�@�C������APCM�����̃f�[�^���I�[�v������B
 *
 *	@param	p_chanel	�`�����l�����
 *	@param	data_size	�f�[�^�̃T�C�Y
 *	@param	p_buff	�w�b�_�[���������ރo�b�t�@
 *	
 */
void dtfm_wave_file_header(dtfm_chanel* p_chanel, 
		int data_size, 
		unsigned char* p_buff)
{
	unsigned int					sizesize = data_size + 16 + 20;
	unsigned int					size_fmt = 16;
	unsigned int					u4_work;
	MyWAVEFORMATEX		tagFormat;



	memcpy(p_buff, "RIFF", 4);
	p_buff += 4;
	u4_work = _dtfm_cnv_u4_native_2_intel_byte(sizesize);
	memcpy(p_buff, &u4_work, sizeof(u4_work));
	p_buff += 4;
	memcpy(p_buff, "WAVE", 4);
	p_buff += 4;
	memcpy(p_buff, "fmt ", 4);
	p_buff += 4;

    tagFormat.wFormatTag      = _dtfm_cnv_u2_native_2_intel_byte( 1 );
    tagFormat.nChannels       = _dtfm_cnv_u2_native_2_intel_byte( p_chanel->chanel );
    tagFormat.nSamplesPerSec  = _dtfm_cnv_u4_native_2_intel_byte( p_chanel->sample_bit_per_sec );
    tagFormat.wBitsPerSample  = _dtfm_cnv_u2_native_2_intel_byte( p_chanel->sample_bits );
    tagFormat.nBlockAlign     = _dtfm_cnv_u2_native_2_intel_byte( tagFormat.nChannels * tagFormat.wBitsPerSample/8 );
    tagFormat.nAvgBytesPerSec = _dtfm_cnv_u4_native_2_intel_byte( tagFormat.nSamplesPerSec * tagFormat.nBlockAlign );
    tagFormat.cbSize = sizeof(MyWAVEFORMATEX);
	u4_work = _dtfm_cnv_u4_native_2_intel_byte(size_fmt);
	memcpy(p_buff, &u4_work, sizeof(u4_work));
	p_buff += 4;
	memcpy(p_buff, &tagFormat, 16);
	p_buff += 16;


	memcpy(p_buff, "data", 4);
	p_buff += 4;
	u4_work = _dtfm_cnv_u4_native_2_intel_byte(data_size);
	memcpy(p_buff, &u4_work, 4);

}

/*	@brief	�w�肵���`�����l�����ƁA�f�[�^�T�C�Y����v���C���ԁi�~���b�j���擾����B
 *
 *	@param	p_chanel	�`�����l�����
 *	@param	data_size	�f�[�^�̃T�C�Y
 *
 *	@return	�v���C���ԁi�~���b�j
 *	
 */
int dtfm_get_play_time(dtfm_chanel* p_chanel, 
		int data_size)
{
	int ret = 0;
	int block = p_chanel->chanel * (p_chanel->sample_bits/8);
	int avg   = p_chanel->sample_bit_per_sec * block;

	double dd = ((double)data_size / (double)avg);
	dd *= 1000;

	return (int)dd;
}

/*	@brief	�t�@�C������APCM�����̃f�[�^���I�[�v������B
 *
 *	@param	p_file_path	�t�@�C���̃p�X
 *	@param	p_chanel	�`�����l�����
 *	@param	p_buff	�f�[�^���擾����o�b�t�@�BNULL���w�肷��ƁA�֐��̖߂�l�ŕK�v�ȃo�b�t�@�T�C�Y���擾�ł���
 *	@param	buff_size	�o�b�t�@�T�C�Y
 *
 *	@retval	0	����
 *	@retval	��0	�o�b�t�@�T�C�Y
 *	@retval	-2	�w�肳�ꂽ�����t�@�C�����I�[�v���ł��܂���B
  *	@retval	-3	�w�肳�ꂽ�����t�@�C�����s��
 *	@verbatim
 *	@endverbatim
 *	
 */
int dtfm_load_from_file(
		const char* p_file_path, 
		dtfm_chanel* p_chanel, 
		void* p_buff, 
		int buff_size)
{
	int ret = -1, sts;
	struct stat ffstat;
	FILE *fp = NULL;
	unsigned char* p_temp_buf = NULL;
	unsigned char* p_work = NULL;
	long	size_format = 0;
	long	size = 0;
	MyWAVEFORMATEX tagFormat;


	// �t�@�C���̃T�C�Y���擾
	if( stat(p_file_path, &ffstat) < 0 ) {
		ret = -2;
		goto END_;
	}
	p_work = p_temp_buf = (unsigned char*)malloc(ffstat.st_size);
	if( p_temp_buf == NULL ) {
		ret = -2;	// �t�@�C�� NOT�@FOUND
		goto END_;
	}
	fp = fopen(p_file_path, "rb");
	if( fp == NULL ) {
		ret = -2;	// �t�@�C�� NOT�@FOUND
		goto END_;
	}
	sts = fread(p_temp_buf, ffstat.st_size, 1, fp);
	fclose( fp );
	fp = NULL;

	ret = -3;	// �t�H�[�}�b�g�G���[
	// �t�H�[�}�b�g�`�F�b�N
	if( memcmp(p_work, "RIFF", 4 ) != 0 ) {
		goto END_;
	}
	p_work += 4;


	memcpy(&size, p_work, 4);
	p_work += 4;
	size = _dtfm_cnv_u4_intel_2_native_byte(size);

	
	if( memcmp(p_work, "WAVE", 4 ) != 0 ) {
		goto END_;
	}
	p_work += 4;
	
	
	if( memcmp(p_work, "fmt ", 4 ) != 0 ) {
		goto END_;
	}
	p_work += 4;

	// �t�H�[�}�b�g���̃T�C�Y
	memcpy(&size_format, p_work, 4);
	p_work += 4;
	size_format = _dtfm_cnv_u4_intel_2_native_byte(size_format);

	// �t�H�[�}�b�g���
	memcpy(&tagFormat, p_work, sizeof(MyWAVEFORMATEX) );
	tagFormat.wFormatTag = _dtfm_cnv_u2_intel_2_native_byte( tagFormat.wFormatTag );
	tagFormat.nChannels = _dtfm_cnv_u2_intel_2_native_byte( tagFormat.nChannels );
	tagFormat.nSamplesPerSec = _dtfm_cnv_u4_intel_2_native_byte( tagFormat.nSamplesPerSec );
	tagFormat.nAvgBytesPerSec = _dtfm_cnv_u4_intel_2_native_byte( tagFormat.nAvgBytesPerSec );
	tagFormat.nBlockAlign = _dtfm_cnv_u2_intel_2_native_byte( tagFormat.nBlockAlign );
	tagFormat.wBitsPerSample = _dtfm_cnv_u2_intel_2_native_byte( tagFormat.wBitsPerSample );
	if( tagFormat.wFormatTag != 1 ) {
		// PCM�łȂ��B
		goto END_;
	}
	p_chanel->chanel = tagFormat.nChannels == 1 ? DTFM_CHANEL_MONO : DTFM_CHANEL_STEREO;
	p_chanel->sample_bits = (unsigned short)tagFormat.wBitsPerSample;
	p_chanel->sample_bit_per_sec = tagFormat.nSamplesPerSec;

	p_work += size_format;
	if( memcmp(p_work, "fact", 4 ) == 0 ) {
		p_work += 4;
		int chunkSize = 0;
		memcpy(&chunkSize, p_work, 4);
		chunkSize = _dtfm_cnv_u4_intel_2_native_byte(chunkSize);
		p_work += 4;
		p_work += chunkSize;
	}
	if( memcmp(p_work, "data", 4 ) != 0 ) {
		goto END_;
	}
	p_work += 4;
	memcpy(&size, p_work, 4);
	p_work += 4;
	size = _dtfm_cnv_u4_intel_2_native_byte( size );

	if( p_buff == NULL ) {
		ret = size;
		goto END_;
	}

	if( size > buff_size ) {
		ret = -9;
		goto END_;
	}

	memcpy(p_buff, p_work, size);

	ret = 0;
END_:
	if( fp != NULL ) {
		fclose(fp);
	}
	if( p_temp_buf != NULL ) {
		free( p_temp_buf );
	}

	return ret;
}



/*	@brief	PCM�����̃f�[�^���t�@�C���ɕۑ�
 *
 *	@param	p_file_path	�t�@�C���̃p�X
 *	@param	p_chanel	�`�����l�����
 *	@param	p_buff	�f�[�^
 *	@param	buff_size	�f�[�^�T�C�Y
 *
 *	@retval	0	����
 *	@retval	-1	�t�@�C���I�[�v���G���[
 *	@verbatim
 *	@endverbatim
 *	
 */
int dtfm_save_to_file(
		const char* p_file_path, 
		dtfm_chanel* p_chanel, 
		void* p_buff, 
		int buff_size)
{
	int ret = -1;
	FILE *fp;
	unsigned int					sizesize = buff_size + 16 + 20;
	unsigned int					size_fmt = 16;
	unsigned int					u4_work;
	MyWAVEFORMATEX		tagFormat;

	fp = fopen(p_file_path, "wb");
	if( fp == NULL ) {
		goto END_;
	}
	fwrite("RIFF", 4, 1, fp);
	u4_work = _dtfm_cnv_u4_native_2_intel_byte(sizesize);
	fwrite(&u4_work, sizeof(u4_work), 1, fp);
	fwrite("WAVE", 4, 1, fp);
	fwrite("fmt ", 4, 1, fp);

    tagFormat.wFormatTag      = _dtfm_cnv_u2_native_2_intel_byte( 1 );
    tagFormat.nChannels       = _dtfm_cnv_u2_native_2_intel_byte( p_chanel->chanel );
    tagFormat.nSamplesPerSec  = _dtfm_cnv_u4_native_2_intel_byte( p_chanel->sample_bit_per_sec );
    tagFormat.wBitsPerSample  = _dtfm_cnv_u2_native_2_intel_byte( p_chanel->sample_bits );
    tagFormat.nBlockAlign     = _dtfm_cnv_u2_native_2_intel_byte( tagFormat.nChannels * tagFormat.wBitsPerSample/8 );
    tagFormat.nAvgBytesPerSec = _dtfm_cnv_u4_native_2_intel_byte( tagFormat.nSamplesPerSec * tagFormat.nBlockAlign );
    tagFormat.cbSize = sizeof(MyWAVEFORMATEX);
	u4_work = _dtfm_cnv_u4_native_2_intel_byte(size_fmt);
	fwrite(&u4_work, sizeof(u4_work), 1, fp);
	fwrite(&tagFormat, 16, 1, fp);


	fwrite("data", 4, 1, fp);
	u4_work = _dtfm_cnv_u4_native_2_intel_byte(buff_size);
	fwrite(&u4_work, 4, 1, fp);
	fwrite(p_buff, buff_size, 1, fp);

	ret = 0;
	goto END_;
END_:
	if( fp != NULL ) {
		fclose(fp);
	}
	return 0;
}


int enc_ext_dtfm_charactor(const char* p_src, char* p_buf, int buf_size)
{
	const char* key_array = "123A456B789C*0#D";
	char* p_wk = p_buf;
	int len = strlen(p_src);

	if( p_buf == NULL ) {
		// �o�b�t�@��NULL�̏ꍇ�́A�K�v�ȃf�[�^�T�C�Y��Ԃ�
		return len*2+1;
	}

	if( len*2 > buf_size ) {
		return -1;
	}
	

	while( *p_src != 0 ) {
		unsigned char c1, c2;

		c1 = *p_src & 0xF0;
		c2 = *p_src & 0x0F;
		c1 >>= 4;

		*p_wk = key_array[ c1 ];
		p_wk++;
		*p_wk = key_array[ c2 ];
		p_wk++;
		p_src++;
	}

	return 0;
}

int dec_ext_dtfm_charactor(const char* p_src, char* p_buf, int buf_size)
{
	const char* key_array = "123A456B789C*0#D";
	char* p_dst = p_buf;
	int index = 0;
	char *p_find = 0;
	char *p_wk = p_buf;
	int ret = 0;



	while( *p_src != 0 ) {
		unsigned char c1, c2;

		c1 = *p_src;
		p_src++;
		if( *p_src == 0 ) {
			break;
		}
		c2 = *p_src;

		p_find = strchr(key_array, (char)c1);
		if( p_find == NULL ) {
			return -2;	
		}
		index = p_find - key_array;
		c1 = (char)index;
		c1 <<= 4;

		p_find = strchr(key_array, (char)c2);
		if( p_find == NULL ) {
			return -2;	
		}
		index = p_find - key_array;
		c2 = (char)index;

		*p_wk = (char)(c1 | c2);
		ret++;
		p_wk++;
		p_src++;
	}

	return ret;
}



void _dtfm_init_ji(dtfm_judge_item* ji, int judge_data_cnt, double sample, int bits)
{
	int i;
	double play_time = (((double)judge_data_cnt)/sample);
	double per_one_msec = 1000 / play_time;
	double margin = judge_data_cnt * MARGIN_PROPORTION;

	if( bits == 16 ) {
		margin*=10;
	}

#if MY_DBG==1
	fprintf(stderr, "  _dtfm_init_ji judge_data_cnt  =%d\n", judge_data_cnt);
	fprintf(stderr, "  _dtfm_init_ji sample          =%f\n", sample);
	fprintf(stderr, "  _dtfm_init_ji play_time       =%f\n", play_time);
	fprintf(stderr, "  _dtfm_init_ji per_one_msec    =%f\n", per_one_msec);
	fprintf(stderr, "  _dtfm_init_ji margin          =%f\n", margin);
#endif /*MY_DBG*/
	for( i = 0; i < 8; i++ ) {
		ji[i].std_low_per = ji[i].hz / per_one_msec;
		ji[i].min_low_per = ji[i].std_low_per - margin;
		ji[i].max_low_per = ji[i].std_low_per + margin;

		ji[i].std_hi_per = judge_data_cnt - ji[i].std_low_per;
		ji[i].min_hi_per = ji[i].std_hi_per - margin;
		ji[i].max_hi_per = ji[i].std_hi_per + margin;
#if MY_DBG==1
		fprintf(stderr, "  _dtfm_init_ji [%uHz] %f:{%f--%f} %f:{%f--%f}\n", 
			ji[i].hz,
			ji[i].std_low_per, ji[i].min_low_per, ji[i].max_low_per,
			ji[i].std_hi_per,  ji[i].min_hi_per,  ji[i].max_hi_per
			);
#endif /*MY_DBG*/
	}
}




int dtfm_parse(int judge_data_cnt, 
			  char* pVal, 
			  dtfm_judge_item* ji_fix, 
			  dtfm_judge_item* ji_wk, 
			  double sample, 
			  dtfm_complex* p_complex,
			  int bits) 
{
//	register dtfm_complex wkComplex;
	register int		i, j;
	register large_data ld[LARGE_DATA_MAX];
	int ret = 0;
	int dbgint = 1;
	int my_pair[3] = {0,0,0};
	double max_r = 0;
	double min_r = 0;
	double max_R = 0;
	double min_R = 0;
	double max_R_margin = 0;
	double min_R_margin = 0;
	int    max_R_cnt = 0;
	int    min_R_cnt = 0;

#if MY_DBG==1
	fprintf(stderr, "\n");
#endif
	memset(ld, 0, sizeof(ld));

	if( ji_fix != NULL ) {
		ji_wk = ji_fix;
	} else {
		_dtfm_init_ji(ji_wk, judge_data_cnt, sample, bits);
	}
	for(i = 0; i < 8; i++ ) {
		ji_wk[i].cnt = 0;
	}

	// FFT
	dtfm_fft(judge_data_cnt, p_complex);
	for(i = 0; i < judge_data_cnt; i++ ) {
		if( max_R <  p_complex[i].R ) {
			max_R = p_complex[i].R;
		}
		if( min_R > p_complex[i].R ) {
			min_R = p_complex[i].R;
		}
	}


	max_r = max_R / 2;
	min_r = min_R / 2;

	for(i = 0; i < judge_data_cnt; i++ ) {
		int index = -1;

		if( p_complex[i].R > 0 ) {
			if( p_complex[i].R  < max_r ) {
				continue;
			}
		} else {
			if( p_complex[i].R  > min_r ) {
				continue;
			}
		}

		double dAbs = fabs( p_complex[i].R );
		for(j = 0; j < LARGE_DATA_MAX; j++ ) {
			if( ld[j].val < dAbs) {
				index = j;
				break;
			}
		}
		if( index != -1 ) {
			int min_index = 0;
			double min_val = ld[ 0 ].val;
			for(j = 1; j < LARGE_DATA_MAX; j++ ) {
				if( min_val >  ld[ j ].val ) {
					min_index = j;
					min_val = ld[ j ].val;
				}
			}
			ld[ min_index ].fft_val = (double)i;
			ld[ min_index ].val = (double)dAbs;
		}
	}

	for(j = 0; j < LARGE_DATA_MAX; j++ ) {
		if( ld[j].fft_val != 0 ) {
#if MY_DBG==1
			fprintf(stderr, "LD[%d] %f %f", j, ld[j].fft_val, ld[j].val);
#endif
			dtfm_judge_item* temp_ji = NULL;

			for(i = 0; i < 8; i++ ) {
				if( ( ji_wk[i].min_low_per < ld[j].fft_val && ld[j].fft_val < ji_wk[i].max_low_per ) ||
					( ji_wk[i].min_hi_per < ld[j].fft_val && ld[j].fft_val < ji_wk[i].max_hi_per )) {

					if( temp_ji == NULL ) {
						temp_ji = &ji_wk[i];
						temp_ji->cnt++;
					} else {
						// �덷�͈̔͂ł��Ԃ��Ă���ꍇ�́A�W���ɋ߂����̐�Βl�Ō��肷��
						double d1 = 0;
						double d2 = 0;
						if( ( temp_ji->min_low_per < ld[j].fft_val && ld[j].fft_val < temp_ji->max_low_per ) ) {
							d1 = fabs( ld[j].fft_val - temp_ji->std_low_per );
						} else {
							d1 = fabs( ld[j].fft_val - temp_ji->std_hi_per );
						}

						if( ( ji_wk[i].min_low_per < ld[j].fft_val && ld[j].fft_val < ji_wk[i].max_low_per ) ) {
							d2 = fabs( ld[j].fft_val - ji_wk[i].std_low_per );
						} else {
							d2 = fabs( ld[j].fft_val - ji_wk[i].std_hi_per );
						}

						if( d1 > d2 ) {
							temp_ji->cnt--;
							temp_ji = &ji_wk[i];
							temp_ji->cnt++;
						}

					}

				}
			}
				
			if( temp_ji != NULL ) {
#if MY_DBG==1			
				fprintf(stderr, "  Have Hz:[%d]", temp_ji->hz);
#endif
				ld[j].p_ji = temp_ji;
				if( !( my_pair[0] == temp_ji->hz || my_pair[1] == temp_ji->hz ) ) {
					if( my_pair[0] == 0 ) {
						my_pair[0] = temp_ji->hz;
					} else 
					if( my_pair[1] == 0 ) {
						my_pair[1] = temp_ji->hz;
					} else 
					if( my_pair[2] == 0 ) {
						my_pair[2] = temp_ji->hz;
					} else {

					}
				}
			}


#if MY_DBG==1
			fprintf(stderr, "\n");
#endif
		}
	}

	
	
	large_data* row_lg = NULL;
	large_data* hight_lg = NULL;
	for(j = 0; j < LARGE_DATA_MAX; j++ ) {
		if( ld[j].p_ji != NULL ) {
			if( ld[j].p_ji->hz <= 941 ) {
				if( row_lg == NULL ) {
					row_lg = &ld[j];
				} else {
					// ���[�g�̑傫�������̗p
					if( row_lg->val < ld[j].val ) {
						row_lg = &ld[j];
					}
				}
			} else {
				if( hight_lg == NULL ) {
					hight_lg = &ld[j];
				} else {
					// ���[�g�̑傫�������̗p
					if( hight_lg->val < ld[j].val ) {
						hight_lg = &ld[j];
					}
				}

			}
		}
	}







	if( row_lg != NULL && hight_lg != NULL 
		&& my_pair[2] == 0 ) {


		// �V���̍������덷�O�D6�ȓ��Ɏ��܂��Ă��邩�H
		double row_R   = fabs(min_r*2*0.6);
		double hight_R = fabs(max_r*2*0.6);
		
		if( hight_lg->val > hight_R && 
			row_lg->val   > row_R   ) {

			*pVal = _get_matrix_val(row_lg->p_ji->hz, hight_lg->p_ji->hz);
			if( *pVal != 0 ) {
				ret = 1;
			}
		}
	}

	return ret;
}




static char _get_matrix_val(int row_hz, int hight_hz)
{
#if MY_DBG==1
	fprintf(stderr, "  init_ji row_hz=%d hight_hz=%d\n", row_hz, hight_hz);
#endif /*MY_DBG*/
	char c = 0;
	if( (row_hz == 697  && hight_hz == 1209) ) {
		c = '1';
	} else
	if( (row_hz == 697  && hight_hz == 1336) ) {
		c = '2';
	} else
	if( (row_hz == 697  && hight_hz == 1477) ) {
		c = '3';
	} else
	if( (row_hz == 697  && hight_hz == 1633) ) {
		c = 'A';
	} else
	if( (row_hz == 770  && hight_hz == 1209) ) {
		c = '4';
	} else
	if( (row_hz == 770  && hight_hz == 1336) ) {
		c = '5';
	} else
	if( (row_hz == 770  && hight_hz == 1477) ) {
		c = '6';
	} else
	if( (row_hz == 770  && hight_hz == 1633) ) {
		c = 'B';
	} else
	if( (row_hz == 852  && hight_hz == 1209) ) {
		c = '7';
	} else
	if( (row_hz == 852  && hight_hz == 1336) ) {
		c = '8';
	} else
	if( (row_hz == 852  && hight_hz == 1477) ) {
		c = '9';
	} else
	if( (row_hz == 852  && hight_hz == 1633) ) {
		c = 'C';
	} else
	if( (row_hz == 941  && hight_hz == 1209) ) {
		c = '*';
	} else
	if( (row_hz == 941  && hight_hz == 1336) ) {
		c = '0';
	} else
	if( (row_hz == 941  && hight_hz == 1477) ) {
		c = '#';
	} else
	if( (row_hz == 941  && hight_hz == 1633) ) {
		c = 'D';
	}
	return c;
}



static short _dtfm_cnv_u2_native_2_intel_byte(short val)
{
#ifdef BIG_ENDIAN
	return val;
//	return htons(val);
#else 
	return val;
#endif
}

static int _dtfm_cnv_u4_native_2_intel_byte(int val)
{
#ifdef BIG_ENDIAN
	return val;
//	return htonl(val);
#else 
	return val;
#endif
}
static short _dtfm_cnv_u2_intel_2_native_byte(short val)
{
#ifdef BIG_ENDIAN
	return val;
//	return ntohs(val);
#else 
	return val;
#endif
}
static int _dtfm_cnv_u4_intel_2_native_byte(int val)
{
#ifdef BIG_ENDIAN
	return val;
//	return ntohl(val);
#else 
	return val;
#endif
}


static int _dtfm_near2power(int val)
{
	int ret = 0;
	int y = 0;

	do {
		val /= 2;
		y++;
		if( val <= 1 ) {

			break;
		}
	} while (1);


	return (int)pow(2, (double)y);
}



int dtfm_ft(int judge_data_cnt, 
			  dtfm_complex* p_complex)
{
	dtfm_complex wkComplex;
	int i,j;
	int	ret = 0;

	for(i = 0; i < judge_data_cnt; i++ ) {
		wkComplex.r=0;
		wkComplex.i=0;
		for(j = 0; j < judge_data_cnt; j++ ) {
			wkComplex.r += p_complex[j].r*cos(2.0*PI*i*j/ judge_data_cnt );
			wkComplex.i += p_complex[j].i*sin(2.0*PI*i*j/ judge_data_cnt );
		}
		p_complex[i].R = sqrt( wkComplex.r * wkComplex.r+ wkComplex.i * wkComplex.i) / 1000;
	}

	return 0;
}


int dtfm_fft(int judge_data_cnt, 
			  dtfm_complex* p_complex) 
{
	register int m, mh, i, j, k, irev;
	register double wr, wi, xr, xi;
	register int n = judge_data_cnt;

	double theta = 2*PI/judge_data_cnt;

	/* ---- scrambler ---- */
	i = 0;
	for (j = 1; j < n - 1; j++) {
		for (k = n >> 1; k > (i ^= k); k >>= 1);
		if (j < i) {
			xr = p_complex[j].r;
			xi = p_complex[j].R;
			p_complex[j].r = p_complex[i].r;
			p_complex[j].R = p_complex[i].R;
			p_complex[i].r = xr;
			p_complex[i].R = xi;
		}
	}
	for (mh = 1; (m = mh << 1) <= n; mh = m) {
		irev = 0;
		for (i = 0; i < n; i += m) {
			wr = cos(theta * irev);
			wi = sin(theta * irev);
			for (k = n >> 2; k > (irev ^= k); k >>= 1);
			for (j = i; j < mh + i; j++) {
				k = j + mh;
				xr = p_complex[j].r - p_complex[k].r;
				xi = p_complex[j].R - p_complex[k].R;
				p_complex[j].r += p_complex[k].r;
				p_complex[j].R += p_complex[k].R;
				p_complex[k].r = wr * xr - wi * xi;
				p_complex[k].R = wr * xi + wi * xr;
			}
		}
	}
	return 0;
}



