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


// コールバックの定義
typedef void (*dtfm_parse_cb)(char c, void* p_user_data);
typedef void (*dtfm_parse_buffring_cb)(const char* str, void* p_user_data);

// Chanelデータをデフォルト値で初期化
void dtfm_init_default_chanel(
		dtfm_chanel* p_chanel);

// 判定データを初期化
bool dtfm_init_judge_data(
		dtfm_chanel* p_chanel, 
		dtfm_judge* judge);

// 判定データを破棄
void dtfm_destroy_judge_data(
		dtfm_judge* judge);




/**	@brief	文字列から、DTFMデータ(PCM)を作成
 *
 *	@param	p_chanel	チャンネル情報
 *	@param	p_txt	変換元文字列
 *	@param	p_buff	変換先バッファ。NULLを指定すると、戻り値で必要をバッファ数を取得できる
 *	@param	buff_size	変換先のバッファ数
 *
 *	@retval	0	変換成功
  *	@retval	> 0	必要なバッファ数
 *	@retval	-1	メモリーエラー等
 *	@retval	-2	バッファサイズ(buff_size)が不足
 *	@retval	-3	p_txtが空文字
 *	@verbatim
 *	@endverbatim
 */
int  dtfm_text_to_pcm(
		dtfm_chanel* p_chanel, 
		const char* p_txt, 
		void* p_buff, 
		int buff_size);



/**	@brief	DTFMデータ(PCM)を、文字データに変換する
 *			バッファリングしながら、解析する。
 *			DTFM判定できないデータが0.5秒続くと、コールバックで解析データを通知する。
 *
 *	@param	p_chanelチャンネル情報
 *	@param	judge	判定データ
 *	@param	p_callback	判定結果コールバック
 *	@param	p_user_data	コールバックに渡すユーザーデータ
 *	@param	p_data	変換元DTFMデータ
 *	@param	data_size	変換元DTFMデータのデータサイズ
 *
  *	@retval	-1	メモリーエラー等
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

/**	@brief	DTFMデータ(PCM)を、文字データに変換する
 *
 *	@param	p_chanel	チャンネル情報
 *	@param	judge	判定データ
 *	@param	p_callback	判定結果コールバック
 *	@param	p_user_data	コールバックに渡すユーザーデータ
 *	@param	p_data	変換元DTFMデータ
 *	@param	data_size	変換元DTFMデータのデータサイズ
 *	@param	p_analyzed_size	解析したデータサイズを取得。NULLを指定した場合は、シグナルの不足分データも解析する。COMIT処理などにしよう
 *
 *	@retval	0	常に０
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




/**	@brief	指定されたPCM音源のWAVEをファイルを、解析してTEXTに変換する。
 *
 *	@param	p_file_path	ファイルのパス
 *	@param	p_callback	判定結果コールバック
 *	@param	p_user_data	コールバックに渡すユーザーデータ
 *
 *	@retval	＞=0	TEXTのカウント
 *	@retval	-2	指定された音源ファイルをオープンできません。
 *	@retval	-3	指定された音源ファイルが不正
 *	
 */
int dtfm_pcm_to_text_from_file(
		const char* p_file_path, 
		dtfm_parse_cb p_callback,
		void* p_user_data);


/**	@brief	WAVEファイルのヘッダー情報をバッファに書き込む（Java-JNI用）
 *
 *	@param	p_chanel	チャンネル情報
 *	@param	data_size	データのサイズ
 *	@param	p_buff	ヘッダーを書き込むバッファ
 *	
 */
void dtfm_wave_file_header(dtfm_chanel* p_chanel, 
		int data_size, 
		unsigned char* p_buff);

/**	@brief	指定したチャンネル情報と、データサイズからプレイ時間（ミリ秒）を取得する。
 *
 *	@param	p_chanel	チャンネル情報
 *	@param	data_size	データのサイズ
 *
 *	@return	プレイ時間（ミリ秒）
 *	
 */
int dtfm_get_play_time(dtfm_chanel* p_chanel, 
		int data_size);



/**	@brief	ファイルから、PCM音源のデータをオープンする。
 *
 *	@param	p_file_path	ファイルのパス
 *	@param	p_chanel	チャンネル情報
 *	@param	p_buff	データを取得するバッファ。NULLを指定すると、関数の戻り値で必要なバッファサイズを取得できる
 *	@param	buff_size	バッファサイズ
 *
 *	@retval	0	成功
 *	@retval	＞0	バッファサイズ
 *	@retval	-2	指定された音源ファイルをオープンできません。
  *	@retval	-3	指定された音源ファイルが不正
 *	@verbatim
 *	@endverbatim
 *	
 */
int dtfm_load_from_file(
		const char* p_file_path, 
		dtfm_chanel* p_chanel, 
		void* p_buff, 
		int buff_size);



/**	@brief	PCM音源のデータをファイルに保存
 *
 *	@param	p_file_path	ファイルのパス
 *	@param	p_chanel	チャンネル情報
 *	@param	p_buff	データ
 *	@param	buff_size	データサイズ
 *
 *	@retval	0	成功
 *	@retval	-1	ファイルオープンエラー
 *	@verbatim
 *	@endverbatim
 *	
 */
int dtfm_save_to_file(
		const char* p_file_path, 
		dtfm_chanel* p_chanel, 
		void* p_buff, 
		int buff_size);

/**	@brief	Byteデータから、DTFM文字列に変換
 *
 *	@param	p_src	Byteデータ
 *	@param	p_buf	DTFM文字列出力用バッファ。NULLを指定すると、関数の戻り値で必要なバッファサイズを取得できる
 *	@param	buf_size	p_bufのサイズ
 *
 *	@retval	0	成功
 *	@retval	>0	必要なバッファ数
 *	@retval	-1	バッファが足らない
 */
int enc_ext_dtfm_charactor(const char* p_src, char* p_buf, int buf_size);


/**	@brief	DTFM文字列からByeデータに変換
 *
 *	@param	p_src	DTFM文字列
 *	@param	p_buf	a parameter of type char*
 *	@param	buf_size	a parameter of type int
 *
 *	@retval	0>=	変換したByeデータのデーター長
 */
int dec_ext_dtfm_charactor(const char* p_src, char* p_buf, int buf_size);


#endif /*__DTFM_H__*/

