#include "inflate_buff.h"
#include <memory.h>
#include <stdlib.h>
#include <string.h>


inflate_buff::inflate_buff(int init_size, int inflate_size)
	:	m_buff_size( init_size )
	,	m_inflate_size( inflate_size )
	,	m_cur_pos( 0 )
{
	if( m_inflate_size < 100 ) {
		m_inflate_size = 100;
	}
	m_buff = (char*)malloc(m_buff_size);
	memset(m_buff, 0, m_buff_size);
}
inflate_buff::~inflate_buff()
{
	if( m_buff != NULL ) {
		free( m_buff );
	}
}
void inflate_buff::add(char c)
{
	if( m_buff_size < m_cur_pos + 10 ) {
		int new_size = m_buff_size + m_inflate_size;

		char* p = (char*)malloc( new_size );

		memset(p, 0, new_size);
		memcpy(p, m_buff, m_cur_pos);

		m_buff_size = new_size;

		free(m_buff);

		m_buff = p;
	}

	m_buff[m_cur_pos++] = c;
}
const char* inflate_buff::str()
{
	return m_buff;
}
