package four.non.bronds.yyys.pipoparoid;

import java.io.File;
import java.util.Date;
import java.util.List;

import four.non.bronds.yyys.pipoparoid.bean.RecFileInf;
import four.non.bronds.yyys.pipoparoid.db.RecDbAccessor;
import four.non.bronds.yyys.pipoparoid.service.PipopaService;
import four.non.bronds.yyys.pipoparoid.util.StrageUtil;
import android.app.AlertDialog;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.graphics.Color;
import android.net.Uri;
import android.os.Bundle;
import android.os.IBinder;
import android.os.PowerManager;
import android.os.SystemClock;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Chronometer;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.TabHost;
import android.widget.TextView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemLongClickListener;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.TabHost.TabSpec;
import android.widget.ToggleButton;

public class RecordActivity extends MyBaseActivity {
	private static final String TAG = "RecordActivity";
	private ToggleButton 	mBtnRec;
	private ToggleButton 	mBtnPause;
	private Chronometer		mChronometer;
	private ListView		mListRecFiles;
	private boolean			mReviewing = false;
	
	private PipopaService	mService;
	private PipopaServiceReceiver	mServiceReciver = new PipopaServiceReceiver();
	
	private class PipopaServiceReceiver extends BroadcastReceiver {
		@Override
		public void onReceive(Context arg0, Intent arg1) {
			
		}
	}
	
	
	// サービスコネクション
	private ServiceConnection serviceConnection = new ServiceConnection() {
		@Override
		public void onServiceConnected(ComponentName className, IBinder service) {
			mService = ((PipopaService.PipopaServiceBinder)service).getService();
			
			RecordActivity.this.reView();
		}
		@Override
		public void onServiceDisconnected(ComponentName className) {
			mService = null;
		}
	};
    public void onStart() {
     	super.onStart();
    }
	@Override
	protected void onStop() {
		super.onStop();
	}
	@Override
    public void onResume() {
		mReviewing = true;
     	super.onResume();
     	
     	this.reView();
     	mReviewing = false;
    }
	@Override
	public void onDestroy() {
		super.onDestroy();
		
		unbindService(serviceConnection); // バインド解除
		unregisterReceiver(mServiceReciver); // 登録解除
	}
	
	/**
	 * 画面の再描画
	 */
	public void reView() {
		mReviewing = true;
		if( mService  != null ) {
			mBtnRec.setEnabled(true);
			if( mService.isRecording() ) {
				mBtnRec.setBackgroundResource(R.drawable.rec_stop);
				mBtnRec.setChecked(true);
				mChronometer.setBase( mService.getStartTime()  );
				mChronometer.start();
			} else {
				mChronometer.setBase( SystemClock.elapsedRealtime() );
				mBtnRec.setBackgroundResource(R.drawable.rec);
				mBtnRec.setChecked(false);
			}
		}
		
		// リストを再表示
        RecFileItemAdapter adapter = new RecFileItemAdapter(this, new RecDbAccessor(this).load());
        mListRecFiles.setAdapter(adapter);
		
		mReviewing = false;
	}
	
	/** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.pcmrec);
        
        
        mListRecFiles = (ListView)findViewById(R.id.listRecFiles);
        mChronometer = (Chronometer)findViewById(R.id.recChronometer);
      
        
        mBtnRec = (ToggleButton) findViewById(R.id.btnRec);
        mBtnRec.setOnCheckedChangeListener(new OnCheckedChangeListener() {
        	@Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        		if( isChecked ) {
        			mBtnRec.setBackgroundResource(R.drawable.rec_stop);
        			onRecordStart();
        		} else {
        			mBtnRec.setBackgroundResource(R.drawable.rec);
        			onRecordStop();
        		}
            }
        });
        mBtnRec.setEnabled(false);
        
        
        mBtnPause = (ToggleButton) findViewById(R.id.btnPause);
        
        // タブをセットアップ
        TabHost tabs = (TabHost) findViewById(android.R.id.tabhost);
        tabs.setup();
        TabSpec tab1 = tabs.newTabSpec("tab1");
        tab1.setIndicator(
        		"Record",
        		this.getResources().getDrawable(R.drawable.rec_48)
    	); 
        tab1.setContent(R.id.tab1);
        tabs.addTab(tab1);

        TabSpec tab2 = tabs.newTabSpec("tab2");
        tab2.setIndicator(
        		"List",
        		this.getResources().getDrawable(R.drawable.list)
    	); 
        tab2.setContent(R.id.tab2);
        tabs.addTab(tab2);
        

        // リスト
        mListRecFiles.setCacheColorHint(Color.TRANSPARENT); 
        mListRecFiles.setOnItemClickListener(new OnItemClickListener() {
			@Override
			public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
				// タップで再生
				RecordActivity.this.playRecFileInf( (RecFileInf)parent.getItemAtPosition(position));
			}
		});
        mListRecFiles.setOnItemLongClickListener(new OnItemLongClickListener() {
			@Override
			public boolean onItemLongClick(AdapterView<?> parent, View view, int position, long id) {
				// ロングタップで編集
				return RecordActivity.this.onListLongClick(parent, view, position, id);
			}
		} );
        
        // サービスを開始
        Intent intent = new Intent(this, PipopaService.class);
        startService(intent);
		IntentFilter filter = new IntentFilter(PipopaService.REC_ACTION);
		registerReceiver(mServiceReciver, filter);
		
		// サービスにバインド
		bindService(intent, serviceConnection, Context.BIND_AUTO_CREATE);
    }
    
    
    /**
     * リストメニューのロングタッチの動作
     * @param parent
     * @param view
     * @param position
     * @param id
     * @return
     */
    private boolean onListLongClick(AdapterView<?> parent, View view, int position, long id) {
		final RecFileInf rcf = (RecFileInf)parent.getItemAtPosition(position);
		
		String titles[] = this.getResources().getStringArray(R.array.rec_file_long_click_menu);
		new AlertDialog.Builder(this)
		.setIcon(R.drawable.icon)
		.setTitle("Menu")
		.setItems(titles,  new DialogInterface.OnClickListener(){
			public void onClick(DialogInterface dialog, int which) {
				
				try {
					switch( which ) {
					case 0:
						playRecFileInf(rcf);
						break;
					case 1:
						editRecFileTitle(rcf);
						break;
					case 2:
						deleteRecFileTitle(rcf);
						break;
					case 3:
						mailRecFileTitle(rcf);
						break;
					case 4:
						shareRecFileTitle(rcf);
						break;
					}
					dialog.dismiss();
				} catch(Exception e) {
					
				}
			}
		})
		.show();
		
    	return true;
    }

    
    /**
     * WAVファイルを再生する
     * @param rcf
     */
    private void playRecFileInf(RecFileInf rcf) {
		Uri uri = Uri.fromFile(new File(rcf.getFName() ));
		Intent intent = new Intent(android.content.Intent.ACTION_VIEW, uri);
		intent.setAction(android.content.Intent.ACTION_VIEW);
		intent.setDataAndType(uri, "audio/x-wav");
		 startActivity(Intent.createChooser(
		            intent, "Please Select"));
    }    
    /**
     * WAVファイルのタイトルを変更する。
     * @param rcf
     */
    private void editRecFileTitle(RecFileInf rcf) {
    	final RecFileInf frcf = rcf;
    	final EditText edtInput;
    	edtInput = new EditText(this);
    	
    	new AlertDialog.Builder(this)
        .setIcon(R.drawable.icon)
        .setTitle("Please Input Title.")
        .setView(edtInput)
        .setPositiveButton("OK", new DialogInterface.OnClickListener() {
        	public void onClick(DialogInterface dialog, int whichButton) {
        		
        		String strDisp = edtInput.getEditableText().toString();
        		if( strDisp.length() == 0 ) {
        			RecordActivity.this.showAlert( "No specify Title." );
        			return ;
        		}
        		
        		
        		frcf.setDisp(
        				edtInput.getEditableText().toString()
        		);
        		
        		new RecDbAccessor(RecordActivity.this).update(frcf);
        		RecordActivity.this.reView();
        	}
        })
        .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
        	public void onClick(DialogInterface dialog, int whichButton) {

        	}
        })
        .show();
    	
    }
    /**
     * WAVファイルを削除。
     * @param rcf
     */
    private void deleteRecFileTitle(RecFileInf rcf) {
    	final RecFileInf frcf = rcf;
    	new AlertDialog.Builder(this)
    	.setIcon(R.drawable.icon)
    	.setTitle("Are you delete this file?")
    	.setMessage("Are you delete this file?")
    	.setPositiveButton("Yes", new DialogInterface.OnClickListener() {
    	    public void onClick(DialogInterface dialog, int whichButton) {
        		new RecDbAccessor(RecordActivity.this).del( frcf.getFName() );
        		RecordActivity.this.reView();
    	    }
    	})
    	.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
    	    public void onClick(DialogInterface dialog, int whichButton) {
    	    }
    	})
    	.show();
    }
    /**
     * WAVファイルのタイトルを変更する。
     * @param rcf
     */
    private void mailRecFileTitle(RecFileInf rcf) {
    	Uri uri = Uri.fromFile(new File(rcf.getFName() ));

    	Intent intent = new Intent(Intent.ACTION_SEND);
    	intent.putExtra(Intent.EXTRA_SUBJECT,"Pipopadroid record file. " + rcf.getDateStr());
    	intent.putExtra(Intent.EXTRA_STREAM, uri );
    	intent.putExtra(Intent.EXTRA_TEXT, "Pipopadroid file.\n"
    			+ "Bitrate : " + rcf.getBitsPer() + "Hz\n"
    			+ "Bits: " + rcf.getBit() + "\n"
    			+ "Chanel : Monoral" + "\n"
    			+ "Date :" + rcf.getDateStr() + "\n"
    			+ "Play time : " + rcf.getPlayTime()
    			
    			);
    	intent.setType("audio/wav");
    	startActivity(Intent.createChooser(intent, "Choose Email Client")); 
    }
    /**
     * WAVファイルを共有
     * @param rcf
     */
    private void shareRecFileTitle(RecFileInf rcf) {
    	Uri uri = Uri.fromFile(new File(rcf.getFName() ));

    	Intent intent = new Intent(Intent.ACTION_SEND);
    	intent.putExtra(Intent.EXTRA_SUBJECT,"Pipopadroid record file. " + rcf.getDateStr());
    	intent.putExtra(Intent.EXTRA_STREAM, uri );
    	intent.putExtra(Intent.EXTRA_TEXT, "Pipopadroid file.\n"
    			+ "Bitrate : " + rcf.getBitsPer() + "Hz\n"
    			+ "Bits: " + rcf.getBit() + "\n"
    			+ "Chanel : Monoral" + "\n"
    			+ "Date :" + rcf.getDateStr() + "\n"
    			+ "Play time : " + rcf.getPlayTime()
    			
    			);
    	intent.setType("audio/wav");
    	startActivity(Intent.createChooser(intent, "Choose Email Client")); 
    }
    
    
    /**
     * 録音開始
     */
    private void onRecordStart() {
    	if( mReviewing == true ) {
    		return ;
    	}
    	if( mService == null ) {
    		showAlert("Service is not start.");
    		return ;
    	}
    	java.text.SimpleDateFormat formatter = new java.text.SimpleDateFormat("yyyyMMdd_HHmmss");
    	mService.recordStart(StrageUtil.getTempDir(this), formatter.format(new Date() ) + ".wav");
    	mChronometer.setBase( SystemClock.elapsedRealtime() );
    	mChronometer.start();
    }
    
    /**
     * 録音停止
     */
    private void onRecordStop() {
    	if( mReviewing == true ) {
    		return ;
    	}
    	if( mService == null ) {
    		showAlert("Service is not start.");
    		return ;
    	}
    	mService.recordStop();
    	
    	mChronometer.stop();
    	
    	this.reView();
    }
    
    
    
    /**
     * リストビューのアダプター
     * @author user
     *
     */
    private class RecFileItemAdapter extends ArrayAdapter<RecFileInf> {
    	private LayoutInflater 	mInflater;
    	private java.text.SimpleDateFormat formatter = new java.text.SimpleDateFormat(
				"MM/dd HH:mm:ss");
    	public RecFileItemAdapter(Context context, List<RecFileInf> objects) {
    		super(context, 0, objects);
    		mInflater = (LayoutInflater) context
				.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
    		
    		
    	}
		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			View view = convertView;
			if (convertView == null) {
				view =  mInflater.inflate(R.layout.rec_play_list_row, null);
			}
			RecFileInf rcf = this.getItem(position);

			
			TextView  txtView = null;
			
			
			// textItemName
			txtView = (TextView)view.findViewById(R.id.textItemName);
			txtView.setText( rcf.getDisp() );
			
			// textOther
			txtView = (TextView)view.findViewById(R.id.textOther);
			long lVal = rcf.getTime() / 1000;	// 秒にする
			long lHour   = lVal / (60*60);
			long lMinite = (lVal - ( lHour * (60*60) ))/60;
			long lSec    = lVal % 60;
			String.format("%02d:%02d:%02d", lHour, lMinite, lSec);
			
			txtView.setText(String.format("%02d:%02d:%02d", lHour, lMinite, lSec)
					+ "       " 
					+ formatter.format(rcf.getRecDate()));
			return view;
		}
    }
}
