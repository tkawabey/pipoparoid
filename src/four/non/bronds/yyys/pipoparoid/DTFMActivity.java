package four.non.bronds.yyys.pipoparoid;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.HashMap;

import four.non.bronds.yyys.pipoparoid.R.id;
import four.non.bronds.yyys.pipoparoid.common.Constant;
import four.non.bronds.yyys.pipoparoid.util.DTFM;
import four.non.bronds.yyys.pipoparoid.util.PCMRecordBuffring;
import four.non.bronds.yyys.pipoparoid.util.PCMRecordFile;
import four.non.bronds.yyys.pipoparoid.util.StrageUtil;


import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.graphics.Typeface;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.MediaPlayer;
import android.media.MediaRecorder;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.SpannableString;
import android.text.style.ForegroundColorSpan;
import android.text.style.StyleSpan;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.Spinner;
import android.widget.TableLayout;
import android.widget.TextView;
import android.widget.LinearLayout.LayoutParams;

/**
 * 録音したPCMデータから、DTFM解析する。
 *
 */
class AsyncAnalyzeDTFM extends AsyncTask<String, Integer, Integer> {
	private DTFMActivity 	mActivity;
	private String				mTempDir;
	private ProgressDialog 		mProgressDialog;
	private Exception 			mError = null;
	
	public AsyncAnalyzeDTFM(DTFMActivity activity, String tmpDir)
	{
		mActivity = activity;
		mTempDir =tmpDir; 
	}
	
	@Override
	protected void onPreExecute() {
		try {
			// Show Progress Dialog
			mProgressDialog = new ProgressDialog( mActivity );
			mProgressDialog.setTitle(  Constant.TITLE  );
			mProgressDialog.setIcon(R.drawable.icon);
			mProgressDialog.setMessage( "Analising,,," );
			
			// 画面の回転を禁止
			if( mActivity instanceof Activity ) {
				Configuration config = mActivity.getResources().getConfiguration();
				((Activity)mActivity).setRequestedOrientation(config.orientation);
			}
			
			
			mProgressDialog.show();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	@Override
	protected Integer doInBackground(String... arg0) {
		try {
			DTFM dtfm = new DTFM();
			dtfm.init();
			
			
			System.out.println("File: " +  mTempDir + "/dtfm.wav" );
			
			String str = dtfm.analyzeDTFMFile( mTempDir + "/dtfm.wav" );
			if(mActivity.mMode != 0 ) {
				str = dtfm.decExtString(str);
			}
			mActivity.showAnalyzedDTFM( str );
			
			
			dtfm.destroy();
		} catch (Exception e) {
			mError = e;
		}
		return null;
	}

	@Override
	protected void onPostExecute(Integer retval) {
		try {
			mProgressDialog.dismiss();
			
			if( mError != null ) {
				throw mError;
			}
			
			
		} catch(Exception e) {
			
		}
		// 画面の回転を禁止を元に戻す
		if( mActivity instanceof Activity ) {
			((Activity)mActivity).setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_UNSPECIFIED);
		}
	}
}



public class DTFMActivity  extends MyBaseActivity implements View.OnClickListener{
	private static final String TAG = "PipoparoidActivity";
	
	private MediaPlayer 			mPlayer;
	private DTFM					mDTFM;
	private String					mTextDTFM = "";
	private TextView				txtDTFM;
//	private EditText				editTalkText;
	private EditText				editTalkTextEx;
	private PCMRecordFile			mPCMRecord;
	private Thread					mRecThread;
	private Button					mBtnRec;
	private TableLayout				layoutButtons;
	protected ScrollView 			mScrollView;
	protected LinearLayout			mLines;
	protected LogcatContext			mLogContext = new LogcatContext(5);
	public int						mMode = 0;

	private AudioManager 			mAudioManager;
	private int 					mCurVolume = 0;
	private int 					mMaxVolume = 0;
	
	
	
	public static final int MAX_LINES = 250;

	private static final int MSG_NEWLINE = 1;
	private static final int MSG_SHOW_DIALOG = 2;
	
	
	private void changeMode() {
		if( mMode == 0 ) {
			txtDTFM.setVisibility(View.VISIBLE);
			editTalkTextEx.setVisibility(View.GONE);
			layoutButtons.setVisibility(View.VISIBLE);
		} else {
			txtDTFM.setVisibility(View.GONE);
			editTalkTextEx.setVisibility(View.VISIBLE);
			layoutButtons.setVisibility(View.GONE);
		}
	}
	
	private final Handler mHandler = new Handler()
	{
		@Override
		public void handleMessage(Message msg)
		{
			switch (msg.what)
			{
//			case MSG_ERROR:
//				handleMessageError(msg);
//				break;
			case MSG_NEWLINE:
				handleMessageNewline(msg);
				break;
//			case MSG_SHOW_DIALOG:
//				showDialog(DIALOG_SEND_LOGCAT);
//				break;
			default:
				super.handleMessage(msg);
			}
		}
	};
	
	/**
	 * @param str
	 */
	public void showAnalyzedDTFM(String str)
	{
		System.out.println("showAnalyzedDTFM : " + str);
		Message msg = mHandler.obtainMessage(MSG_NEWLINE);
		msg.obj = "Listen : " + str;
		mHandler.sendMessage(msg);
	}
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        Spinner	 spinner;
    	try {
	        super.onCreate(savedInstanceState);
	        setContentView(R.layout.dtfm);
	        
	        mAudioManager = (AudioManager) this.getSystemService(Context.AUDIO_SERVICE);
	        mCurVolume = mAudioManager.getStreamVolume(AudioManager.STREAM_MUSIC);
	        mMaxVolume = mAudioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
	        
	        
	        mScrollView = (ScrollView)this.findViewById(R.id.scrollVLog);
	        mLines = (LinearLayout)this.findViewById(R.id.lines);
			
			// 音声のコントロールをMusicに変更
			setVolumeControlStream(AudioManager.STREAM_MUSIC);
			
	        mDTFM = new DTFM();
	        mDTFM.init();
	        
	        
	        mDTFM.setSampleBits((short)16);
	        
	        Log.i(TAG, "SampleBitPerSec:" + mDTFM.getSampleBitPerSec());
	        Log.i(TAG, "SampleBits:" + mDTFM.getSampleBits());
	        Log.i(TAG, "Chanel:" + mDTFM.getChanel());
	        
	        mPCMRecord = new PCMRecordFile( mDTFM, StrageUtil.getTempDir(this), "dtfm.wav" );
	        
	        
	        mPlayer = new MediaPlayer();
	        mPlayer.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
	        	public void onCompletion(MediaPlayer mp) {
	        		mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC, mCurVolume, AudioManager.FLAG_SHOW_UI);
	        	}
	        });
	        
	        layoutButtons = (TableLayout)this.findViewById(R.id.tableLayout1);
	        txtDTFM = (TextView)this.findViewById(R.id.textViewDTFM);
//	        editTalkText = (EditText)this.findViewById(R.id.editTalkText);
	        editTalkTextEx = (EditText)this.findViewById(R.id.editTalkTextEx);
	        spinner = (Spinner)findViewById(R.id.spinnerDTFMMode);
	        spinner.setSelection( mMode );
    		spinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
    			@Override
    			public void onItemSelected(AdapterView<?> parent,
    					View view, int position, long id) {
    				mMode = position;
    				DTFMActivity.this.changeMode();
    			}

    			@Override
    			public void onNothingSelected(AdapterView<?> parent) {
    			}
    		});
	        // Id	@+id/btnTalk
	        Button btn;
	        btn = (Button)this.findViewById(R.id.btnTalk);
	        btn.setOnClickListener( 
	        	new View.OnClickListener() {
					@Override
					public void onClick(View v) {
						// 
						DTFMActivity.this.onTalkDTFM();
					}
		        	
		        }
	        	
	        );
	
	        mBtnRec = (Button)this.findViewById(R.id.btnRec);
	        mBtnRec.setOnClickListener( 
	        	new View.OnClickListener() {
					@Override
					public void onClick(View v) {
						// 
						DTFMActivity.this.onRecord();
					}
		        	
		        }
	        	
	        );
	        
	        
	        ((Button)this.findViewById(R.id.button1)).setOnClickListener( this );
	        ((Button)this.findViewById(R.id.button2)).setOnClickListener( this );
	        ((Button)this.findViewById(R.id.button3)).setOnClickListener( this );
	        ((Button)this.findViewById(R.id.button4)).setOnClickListener( this );
	        ((Button)this.findViewById(R.id.button5)).setOnClickListener( this );
	        ((Button)this.findViewById(R.id.button6)).setOnClickListener( this );
	        ((Button)this.findViewById(R.id.button7)).setOnClickListener( this );
	        ((Button)this.findViewById(R.id.button8)).setOnClickListener( this );
	        ((Button)this.findViewById(R.id.button9)).setOnClickListener( this );
	        ((Button)this.findViewById(R.id.button0)).setOnClickListener( this );
	        ((Button)this.findViewById(R.id.buttonA)).setOnClickListener( this );
	        ((Button)this.findViewById(R.id.buttonB)).setOnClickListener( this );
	        ((Button)this.findViewById(R.id.buttonC)).setOnClickListener( this );
	        ((Button)this.findViewById(R.id.buttonD)).setOnClickListener( this );
	        ((Button)this.findViewById(R.id.buttonAS)).setOnClickListener( this );
	        ((Button)this.findViewById(R.id.buttonSH)).setOnClickListener( this );
	        ((Button)this.findViewById(R.id.btnBack)).setOnClickListener( this );
		} catch (Exception e) {
			showAlert(e.getMessage());
		}
    }
    
    @Override
    public void onResume() {
    	super.onResume();
    	
    	changeMode();
    }
    @Override
    public void onPause() {
    	super.onPause();
    	if( mPCMRecord.isRecording() ) {
    		
    		mPCMRecord.stop();
    	}
    }
    @Override
    public void onDestroy() {
    	super.onDestroy();
    	
    	mDTFM.destroy();
    }


	@Override
	public void onClick(View v) {

		switch( v.getId() ) {
		case R.id.button1:
			mTextDTFM += "1";
			break;
		case R.id.button2:
			mTextDTFM += "2";
			break;
		case R.id.button3:
			mTextDTFM += "3";
			break;
		case R.id.button4:
			mTextDTFM += "4";
			break;
		case R.id.button5:
			mTextDTFM += "5";
			break;
		case R.id.button6:
			mTextDTFM += "6";
			break;
		case R.id.button7:
			mTextDTFM += "7";
			break;
		case R.id.button8:
			mTextDTFM += "8";
			break;
		case R.id.button9:
			mTextDTFM += "9";
			break;
		case R.id.button0:
			mTextDTFM += "0";
			break;
		case R.id.buttonA:
			mTextDTFM += "A";
			break;
		case R.id.buttonB:
			mTextDTFM += "B";
			break;
		case R.id.buttonC:
			mTextDTFM += "C";
			break;
		case R.id.buttonD:
			mTextDTFM += "D";
			break;
		case R.id.buttonAS:
			mTextDTFM += "*";
			break;
		case R.id.buttonSH:
			mTextDTFM += "#";
			break;
		case R.id.btnBack:
			if( mTextDTFM.length() != 0 ) {
				mTextDTFM = mTextDTFM.substring(0, mTextDTFM.length()-1);
			}
			break;
		}
		txtDTFM.setText(mTextDTFM);
	}
    
    
    private void onTalkDTFM() {
    	String strDisp;
    	String strText;
    	if( mMode == 0 ) {
    		strDisp = strText = mTextDTFM;// txtDTFM.getText();
    	} else {
    		strDisp = editTalkTextEx.getEditableText().toString();
    		
    		strText = mDTFM.encExtString( strDisp );
    		
    	}
    	if( strText.length() == 0 ) {
    		return ;
    	}

		Message msg = mHandler.obtainMessage(MSG_NEWLINE);
		msg.obj = "Talk : " + strDisp;
		mHandler.sendMessage(msg);

    	
    	byte[] pcm = mDTFM.string2PCM("      " + strText);
    	if( pcm == null ) {
    		return ;
    	}
    	Log.i(TAG, "pcm len : " + pcm.length);


    	try {
    		mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC, mMaxVolume, AudioManager.FLAG_SHOW_UI);
			File tempWav = File.createTempFile("pipoparoid", "wav", getCacheDir());
			tempWav.deleteOnExit();
			
			if( mPlayer.isPlaying() ) {
				mPlayer.stop();
			}
			
			
			FileOutputStream fos = new FileOutputStream(tempWav);
	        fos.write(pcm);
	        fos.close();
	        
	        mPlayer.reset();
	        
	        FileInputStream fis = new FileInputStream(tempWav);
	        try {
	        	mPlayer.setDataSource(fis.getFD());
	        } catch (IllegalStateException e) {
	        	mPlayer.reset();
	        	mPlayer.setDataSource(fis.getFD());
	        }
	        
	        mPlayer.prepare();
	        mPlayer.start();
	        
		} catch (IOException e) {
			e.printStackTrace();
		}
    }
    
    
    private void onRecord() {
    	
    	if( mPCMRecord.isRecording() ) {
    		try {
	    		mPCMRecord.stop();
	    		
	    		if( mRecThread != null ) {
	    			
					mRecThread.join();
					
					mRecThread = null;
	    		}
    		} catch (InterruptedException e) {
				e.printStackTrace();
			}
    		
    		mBtnRec.setText("Listen");
    		
    		// 解析処理を実行
    		new AsyncAnalyzeDTFM(this, StrageUtil.getTempDir(this) ).execute("");
    	} else {
    		mRecThread = new Thread( mPCMRecord );
    		mRecThread.start();
    		
    		mBtnRec.setText("Stop");
    	}
    }
    
	/* TODO: Scrolling needs a lot of work.  Might not be worth it, though. */
	private void handleMessageNewline(Message msg)
	{
		String line = (String)msg.obj;
		
//		checkForError(line);
		
		final boolean autoscroll = 
		  (mScrollView.getScrollY() + mScrollView.getHeight() >= mLines.getBottom()) ? true : false;
			
		TextView lineView = new TextView(DTFMActivity.this);
		lineView.setTypeface(Typeface.MONOSPACE);
//		lineView.setText(new LoglineFormattedString(line));
		lineView.setText(line);

		mLogContext.addLine(line);
		mLines.addView(lineView, new LayoutParams(LayoutParams.FILL_PARENT,
		  LayoutParams.WRAP_CONTENT));

		if (mLines.getChildCount() > MAX_LINES)
			mLines.removeViewAt(0);

		mScrollView.post(new Runnable() {
			public void run()
			{
				if (autoscroll == true)
				{
					mScrollView.scrollTo(0,
					  mLines.getBottom() - mScrollView.getHeight());
				}
			}
		});    				
	}
	
	
	
	
	
	
	
	
	/**
	 * Simple class to help keep context across multiple logcat invocations 
	 * (between onStop and onStart).  Similar to the way that the unidiff
	 * patch format works to keep context.
	 */
	private static class LogcatContext
	{
		int mPrec;
		int mPos;
		String[] mLastLines;
		int mLastLineCount;
	
		public LogcatContext(int precision)
		{
			mPrec = precision;
			mPos = mLastLineCount = 0;
			mLastLines = new String[precision * 2];
		}
	
		public void addLine(String line)
		{
			/* TODO */
		}
	}
	
	/* 
	 * Format a logcat line of the form:
	 * 
	 *   L/tag(????): Message
	 */
	private static class LoglineFormattedString extends SpannableString
	{
		public static final HashMap<Character, Integer> LABEL_COLOR_MAP; 
		
		public LoglineFormattedString(String line)
		{
			super(line);
			
			try
			{
				/* We expect at least "L/f:" */
				if (line.length() <= 4)
					throw new RuntimeException();
				
				/* And the log-level label must be exactly 1 character. */
				if (line.charAt(1) != '/')
					throw new RuntimeException();
				
				Integer labelColor = LABEL_COLOR_MAP.get(line.charAt(0));
				
				if (labelColor == null)
					labelColor = LABEL_COLOR_MAP.get('E');

				setSpan(new ForegroundColorSpan(labelColor), 0, 1, 0);
				setSpan(new StyleSpan(Typeface.BOLD), 0, 1, 0);

				int leftIdx;

				if ((leftIdx = line.indexOf(':', 2)) >= 0)
				{
					setSpan(new ForegroundColorSpan(labelColor), 2, leftIdx, 0);
					setSpan(new StyleSpan(Typeface.ITALIC), 2, leftIdx, 0);
				}
			}
			catch (RuntimeException e) /* Lazy FormatException */
			{
				setSpan(new ForegroundColorSpan(0xffddaacc), 0, length(), 0);
			}
		}
		
		static
		{
			LABEL_COLOR_MAP = new HashMap<Character, Integer>();
			LABEL_COLOR_MAP.put('D', 0xff9999ff);
			LABEL_COLOR_MAP.put('V', 0xffcccccc);
			LABEL_COLOR_MAP.put('I', 0xffeeeeee);
			LABEL_COLOR_MAP.put('E', 0xffff9999);
			LABEL_COLOR_MAP.put('W', 0xffffff99);
		}
	}
}
