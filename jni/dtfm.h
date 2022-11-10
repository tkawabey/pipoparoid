#ifndef __DTFM_H__
#define __DTFM_H__

#define DTFM_MIN_SIGNAL_MSEC	50	// Minimul signal time(msec)
#define DTFM_MIN_BLANK_MSEC		30	// Minimul blank time(msec)
#define DTFM_DEFALT_AMP			60	// Default amplitude
#define DTFM_DEFALT_JUDGE_MSEC	17	// Default Judge
#define DTFM_CHANEL_MONO		(unsigned char)1
#define DTFM_CHANEL_STEREO		(unsigned char)2
#define WAVE_FILE_HEADER_SIZE	(4+4+4+4+4+16+4+4)

typedef union _dtfm_wave_hi
{
	unsigned char u1;
	short         u2;
}dtfm_wave_hi;

typedef struct _dtfm_chanel
{
	int		signal_msec;
	int		judge_msec;
	int		blank_msec;
	int		amplitude;
	unsigned int	sample_bit_per_sec;
	unsigned short	sample_bits;
	unsigned short	chanel;	// 1:Mono :2:Stereo
	dtfm_wave_hi	wave_hi;
	dtfm_wave_hi	wave_row;
}dtfm_chanel;



typedef struct _dtfm_complex
{
	double r;
	double i;
	double R;
}dtfm_complex;


typedef struct _dtfm_judge_item
{
	int			hz;
	double		min_low_per;
	double		std_low_per;
	double		max_low_per;
	
	double		min_hi_per;
	double		std_hi_per;
	double		max_hi_per;
	int			cnt;
}dtfm_judge_item;



typedef struct _dtfm_judge
{
	dtfm_complex*	p_complex;

	dtfm_judge_item	ji[8];
	dtfm_judge_item	ji_wk[8];

	double			sample_per;

	double			one_signal_per_mse;
	int				one_signal_cnt;
	
	double			judge_per_mse;
	int				judge_cnt;

	double			pause_per_mse;
	int				pause_cnt;


	int				separate_cnt;

	unsigned char*	m_p_buff;
	int				m_n_buff_size;
	int				m_n_cur_size;

	char*			m_p_str_buff;
	int				m_n_str_buff_size;
	int				m_n_str_cur_size;
	int				m_n_wk_sep_size;
}dtfm_judge;


// �R�[���o�b�N�̒�`
typedef void (*dtfm_parse_cb)(char c, void* p_user_data);
typedef void (*dtfm_parse_buffring_cb)(const char* str, void* p_user_data);

// Chanel�f�[�^���f�t�H���g�l�ŏ�����
void dtfm_init_default_chanel(
		dtfm_chanel* p_chanel);

// ����f�[�^��������
bool dtfm_init_judge_data(
		dtfm_chanel* p_chanel, 
		dtfm_judge* judge);

// ����f�[�^��j��
void dtfm_destroy_judge_data(
		dtfm_judge* judge);




/**	@brief	�����񂩂�ADTFM�f�[�^(PCM)���쐬
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
 */
int  dtfm_text_to_pcm(
		dtfm_chanel* p_chanel, 
		const char* p_txt, 
		void* p_buff, 
		int buff_size);



/**	@brief	DTFM�f�[�^(PCM)���A�����f�[�^�ɕϊ�����
 *			�o�b�t�@�����O���Ȃ���A��͂���B
 *			DTFM����ł��Ȃ��f�[�^��0.5�b�����ƁA�R�[���o�b�N�ŉ�̓f�[�^��ʒm����B
 *
 *	@param	p_chanel�`�����l�����
 *	@param	judge	����f�[�^
 *	@param	p_callback	���茋�ʃR�[���o�b�N
 *	@param	p_user_data	�R�[���o�b�N�ɓn�����[�U�[�f�[�^
 *	@param	p_data	�ϊ���DTFM�f�[�^
 *	@param	data_size	�ϊ���DTFM�f�[�^�̃f�[�^�T�C�Y
 *
  *	@retval	-1	�������[�G���[��
 *	@verbatim
 *	@endverbatim
 *	
 */
int  dtfm_buffring_pcm_to_text(
		dtfm_chanel* p_chanel, 
		dtfm_judge* judge, 
		dtfm_parse_buffring_cb p_callback,
		void* p_user_data,
		void* p_data, 
		int data_size);

/**	@brief	DTFM�f�[�^(PCM)���A�����f�[�^�ɕϊ�����
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
		int *p_analyzed_size);




/**	@brief	�w�肳�ꂽPCM������WAVE���t�@�C�����A��͂���TEXT�ɕϊ�����B
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
		void* p_user_data);


/**	@brief	WAVE�t�@�C���̃w�b�_�[�����o�b�t�@�ɏ������ށiJava-JNI�p�j
 *
 *	@param	p_chanel	�`�����l�����
 *	@param	data_size	�f�[�^�̃T�C�Y
 *	@param	p_buff	�w�b�_�[���������ރo�b�t�@
 *	
 */
void dtfm_wave_file_header(dtfm_chanel* p_chanel, 
		int data_size, 
		unsigned char* p_buff);

/**	@brief	�w�肵���`�����l�����ƁA�f�[�^�T�C�Y����v���C���ԁi�~���b�j���擾����B
 *
 *	@param	p_chanel	�`�����l�����
 *	@param	data_size	�f�[�^�̃T�C�Y
 *
 *	@return	�v���C���ԁi�~���b�j
 *	
 */
int dtfm_get_play_time(dtfm_chanel* p_chanel, 
		int data_size);



/**	@brief	�t�@�C������APCM�����̃f�[�^���I�[�v������B
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
		int buff_size);



/**	@brief	PCM�����̃f�[�^���t�@�C���ɕۑ�
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
		int buff_size);

/**	@brief	Byte�f�[�^����ADTFM������ɕϊ�
 *
 *	@param	p_src	Byte�f�[�^
 *	@param	p_buf	DTFM������o�͗p�o�b�t�@�BNULL���w�肷��ƁA�֐��̖߂�l�ŕK�v�ȃo�b�t�@�T�C�Y���擾�ł���
 *	@param	buf_size	p_buf�̃T�C�Y
 *
 *	@retval	0	����
 *	@retval	>0	�K�v�ȃo�b�t�@��
 *	@retval	-1	�o�b�t�@������Ȃ�
 */
int enc_ext_dtfm_charactor(const char* p_src, char* p_buf, int buf_size);


/**	@brief	DTFM�����񂩂�Bye�f�[�^�ɕϊ�
 *
 *	@param	p_src	DTFM������
 *	@param	p_buf	a parameter of type char*
 *	@param	buf_size	a parameter of type int
 *
 *	@retval	0>=	�ϊ�����Bye�f�[�^�̃f�[�^�[��
 */
int dec_ext_dtfm_charactor(const char* p_src, char* p_buf, int buf_size);


#endif /*__DTFM_H__*/

