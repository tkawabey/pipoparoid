package four.non.bronds.yyys.pipoparoid.util;



import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.SystemClock;

public abstract class PCMRecord implements Runnable{
	static final String TAG = "PCMRecord";
	// DTFMオブジェクト
	protected DTFM	mDTFM;
    // 録音中フラグ
	protected boolean isRecoding = false;
	protected int		mBuffSize = 0;
	protected long		mStartTime = 0;


	public abstract void recodStart();
	public abstract void receiveBuff(int size, byte []buf);
	public abstract void recodEnd(long elapseTime);
	

	public PCMRecord(DTFM dtfm) throws Exception
	{
		mDTFM = dtfm;


        // 必要なバッファ数
		mBuffSize = AudioRecord.getMinBufferSize(
        		mDTFM.getSampleBitPerSec(),
        		mDTFM.getChanel() == 1 ?  AudioFormat.CHANNEL_CONFIGURATION_MONO  : AudioFormat.CHANNEL_CONFIGURATION_STEREO,
        		mDTFM.getSampleBits() == 8 ?  AudioFormat.ENCODING_PCM_8BIT : AudioFormat.ENCODING_PCM_16BIT);
		if( mBuffSize < 0 ) {
			throw new Exception("");
		}
	}
// プロパティ
	public int getBuffSize() {
		return mBuffSize;
	}	
	public long getStartTime() {
		return mStartTime;
	}
// メソッド
    // 録音を停止
    public void stop() {
    	isRecoding = false;
    }
    public boolean isRecording() {
    	return isRecoding;
    }
	@Override
	public void run() {
		try{
	    	android.os.Process.setThreadPriority(
	                android.os.Process.THREAD_PRIORITY_URGENT_AUDIO);

	        
	        AudioRecord audioRecord = new AudioRecord(
	                MediaRecorder.AudioSource.MIC,
	                mDTFM.getSampleBitPerSec(),
	                mDTFM.getChanel() == 1 ?  AudioFormat.CHANNEL_CONFIGURATION_MONO  : AudioFormat.CHANNEL_CONFIGURATION_STEREO,
	                mDTFM.getSampleBits() == 8 ?  AudioFormat.ENCODING_PCM_8BIT : AudioFormat.ENCODING_PCM_16BIT,
	                		mBuffSize);
	        
	        
	        byte[] buffer = new byte[mBuffSize];
	        audioRecord.startRecording();
	        mStartTime = SystemClock.elapsedRealtime();
	        recodStart();
	        isRecoding = true;
				
	        while(isRecoding) {
	            int readed = audioRecord.read(buffer, 0, mBuffSize);
	            System.out.println("readed : ----- " + readed + "  mBuffSize : " + mBuffSize);
	            if( readed > 0 ) {
	            	receiveBuff(readed, buffer);
	            }
	        }
	        recodEnd( SystemClock.elapsedRealtime() - mStartTime );
	        

	        audioRecord.stop();
	        audioRecord.release();
		} catch(IllegalArgumentException ex) {
			ex.printStackTrace();
		} catch(SecurityException ex) {
			ex.printStackTrace();
		}
	}
	
}
