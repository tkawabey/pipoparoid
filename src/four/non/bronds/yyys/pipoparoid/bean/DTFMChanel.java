package four.non.bronds.yyys.pipoparoid.bean;

import four.non.bronds.yyys.pipoparoid.util.DTFM;

public class DTFMChanel {
	public long		chanelHandle;

	
	public int getSampleBitPerSec()
	{
		return DTFM.getSampleBitPerSec(chanelHandle);
	}
	void setSampleBitPerSec(int val)
	{
		DTFM.setSampleBitPerSec(chanelHandle, val);
	}
			
	public short getSampleBits()
	{
		return DTFM.getSampleBits(chanelHandle);
	}
	public void setSampleBits(short val)
	{
		DTFM.setSampleBits(chanelHandle, val);
	}
	
	
	public short getChanel()
	{
		return DTFM.getChanel(chanelHandle);
	}
	public void setChanel(short val)
	{
		DTFM.setChanel(chanelHandle, val);
	}
	
	
	public int getSingnalMsec()
	{
		return DTFM.getSingnalMsec(chanelHandle);
	}
	public void setSingnalMsec(int val)
	{
		DTFM.setSingnalMsec(chanelHandle, val);
	}
	
	
	public int getJudgeMsec()
	{
		return DTFM.getJudgeMsec(chanelHandle);
	}
	public void setJudgeMsec(int val)
	{
		DTFM.setJudgeMsec(chanelHandle, val);
	}
	
	public int getBlankMsec()
	{
		return DTFM.getBlankMsec(chanelHandle);
	}
	public void setBlankMsec(int val)
	{
		DTFM.setBlankMsec(chanelHandle, val);
	}
}
