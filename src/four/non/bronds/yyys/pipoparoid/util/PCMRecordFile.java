package four.non.bronds.yyys.pipoparoid.util;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Date;

import four.non.bronds.yyys.pipoparoid.bean.RecFileInf;

public class PCMRecordFile extends PCMRecord {
    private String	mStrageDir;
    private String	mFileName;
    private int		mType = 0;
    private FileOutputStream	mFos = null;
    private int					mLotalSize = 0;
	protected Date		mStateDate = null;
	protected OnRecodListener	mOnRecodListener = null;
    

	public interface OnRecodListener {
    	public void onRecodeEnd(RecFileInf rcf);
    }
	
	
    public PCMRecordFile(DTFM dtfm, String strageDir, String strFileName)
			throws Exception {
		super(dtfm);

		mStrageDir = strageDir;
		mFileName = strFileName;
		

	}



 // プロパティ
 	public String getStrageDir() {
 		return mStrageDir;
 	}
 	public String getFileName() {
 		return mFileName;
 	}
    public OnRecodListener getOnRecodListener() {
		return mOnRecodListener;
	}
	public void setOnRecodListener(OnRecodListener mOnRecodListener) {
		this.mOnRecodListener = mOnRecodListener;
	}	
	
	public void recodStart()
	{
		mLotalSize = 0;
		mStateDate = new Date();
		File fnTmp = new File(mStrageDir + "/temp.wav");
		
		try {
			mFos = new FileOutputStream(fnTmp);
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
	}
	
	public void receiveBuff(int size, byte []buf)
	{
		try {
			mFos.write(buf, 0, size);
			mLotalSize += size;
			
			
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public void recodEnd(long elapseTime)
	{
		try {
			int buff_size = 8000;
			byte[] buffer = new byte[buff_size];
			mFos.close();

			System.out.println("total_len : ----- " + mLotalSize);
			
			
			File fnTmp = new File(mStrageDir + "/temp.wav");
	        int size;
	        File fn = new File(mStrageDir + "/" + mFileName);
	        byte [] headerByte = mDTFM.getWaveFileHeader(mLotalSize);
	        mFos = new FileOutputStream(fn);
	        mFos.write(headerByte);
	        
	        FileInputStream fi = new FileInputStream(fnTmp);
	        
	        while ((size = fi.read(buffer, 0, buff_size)) != -1 ) {
	        	mFos.write(buffer, 0, size);
	        }
	        mFos.close();
	        fi.close();
	        fnTmp.delete();
	        if( mOnRecodListener != null ) {
	        	RecFileInf rcf = new RecFileInf();
	        	rcf.setFName(  mStrageDir + "/" + mFileName );
	        	rcf.setDisp( mFileName );
	        	rcf.setRecDate(mStateDate);
	        	rcf.setTime( elapseTime );
	        	rcf.setBitsPer( mDTFM.getSampleBitPerSec() );
	        	rcf.setBit( mDTFM.getSampleBits() );
	        	rcf.setChanel( mDTFM.getChanel() );
	        	rcf.setType( mType );
	        	mOnRecodListener.onRecodeEnd(rcf);
	        }
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
    
    
}
