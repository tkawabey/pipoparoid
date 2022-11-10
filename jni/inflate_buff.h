#ifndef __INFLATE_BUFF_H__
#define __INFLATE_BUFF_H__

class inflate_buff
{
public:
	inflate_buff(int init_size, int inflate_size);
	~inflate_buff();
	void add(char c);
	const char* str();
protected:
	char*	m_buff;
	int		m_buff_size;
	int		m_inflate_size;
	int		m_cur_pos;
};


#endif /*__INFLATE_BUFF_H__*/
