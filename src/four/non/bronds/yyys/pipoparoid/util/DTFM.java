
/*
■Jvah のコマンドライン履歴
set PATH=%PATH%;C:\Program Files\Java\jdk1.6.0_13\bin
cd C:\work\Develop\AndroidNDK\android-ndk-r6b\app\pipoparoid\bin
javah -classpath .\ -d ..\jni  four.non.bronds.yyys.pipoparoid.util.DTFM




■Build 
cd /cygdrive/c/work/Develop/AndroidNDK/android-ndk-r6b/app/pipoparoid
ndk-build



■jiniにいて
http://www.ne.jp/asahi/hishidama/home/tech/java/jni_code.html

 */



package four.non.bronds.yyys.pipoparoid.util;



public class DTFM {
	
//	public DTFMChanel	mChanel;
	public long		chanelHandle;
	
	public DTFM()
	{
		chanelHandle = 0;
//		mChanel = new DTFMChanel();
		
	}
	public void init() {
		initDefaultChanel(this);
	}
	public void destroy() {
		destroyDefaultChanel(this);
	}

	
    
	static {
        System.loadLibrary("pipoparoid");
    }
    
	public byte[] getWaveFileHeader(int datalen)
	{
		return getWaveFileHeader(chanelHandle, datalen);
	}
	
	public byte[] string2PCM(String txt)
	{
		return string2PCM(chanelHandle, txt);
	}
	public String analyzeDTFMFile(String strFilePath)
	{
		return analyzeDTFMFile(chanelHandle, strFilePath);
	}
	
	


	
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
	

	public void analyzeDTFMBuffring(byte[] datas, int arry_size, PCMRecordBuffring cb)
	{
		analyzeDTFMBuffring(chanelHandle, datas, arry_size, cb);
	}
	public String encExtString(String str)
	{
		return nEncExtString(str);
	}
	public String decExtString(String str)
	{
		return nDecExtString(str);
	}
    
	native static void initDefaultChanel(DTFM chanel);
	native static void destroyDefaultChanel(DTFM chanel);
	
	public native static int getSampleBitPerSec(long handle);
	public native static void setSampleBitPerSec(long handle, int val);
	
	public native static short getSampleBits(long handle);
	public native static void setSampleBits(long handle, short val);
	
	public native static short getChanel(long handle);
	public native static void setChanel(long handle, short val);
	
	public native static int getSingnalMsec(long handle);
	public native static void setSingnalMsec(long handle, int val);
	
	public native static int getJudgeMsec(long handle);
	public native static void setJudgeMsec(long handle, int val);
	
	public native static int getBlankMsec(long handle);
	public native static void setBlankMsec(long handle, int val);
	
	
	public native static byte[] getWaveFileHeader(long handle, int datalen);
	public native static byte[] string2PCM(long handle, String txt);
	public native static String analyzeDTFMFile(long handle, String strFilePath);
	public native static void analyzeDTFMBuffring(long handle, byte[] datas, int arry_size, PCMRecordBuffring cb);
	
	public native static String nEncExtString(String str);
	public native static String nDecExtString(String str);
}


