package four.non.bronds.yyys.pipoparoid.service;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Date;

import four.non.bronds.yyys.pipoparoid.R;
import four.non.bronds.yyys.pipoparoid.RecordActivity;
import four.non.bronds.yyys.pipoparoid.bean.RecFileInf;
import four.non.bronds.yyys.pipoparoid.db.RecDbAccessor;
import four.non.bronds.yyys.pipoparoid.util.DTFM;
import four.non.bronds.yyys.pipoparoid.util.PCMRecordFile;
import four.non.bronds.yyys.pipoparoid.util.StrageUtil;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.os.PowerManager;
import android.util.Log;

public class PipopaService  extends Service {
	final static String TAG = "MyService";
	public static final String REC_ACTION = "four.non.bronds.yyys.pipoparoid.recordaction";
	
	private static final Class<?>[] mSetForegroundSignature = new Class[] {
	    boolean.class};
	private static final Class<?>[] mStartForegroundSignature = new Class[] {
	    int.class, Notification.class};
	private static final Class<?>[] mStopForegroundSignature = new Class[] {
	    boolean.class};
	
	private Method mSetForeground;
	private Method mStartForeground;
	private Method mStopForeground;
	private Object[] mSetForegroundArgs = new Object[1];
	private Object[] mStartForegroundArgs = new Object[2];
	private Object[] mStopForegroundArgs = new Object[1];
	private PowerManager.WakeLock wakeLock = null;
	private DTFM			mDTFM;
	private PCMRecordFile	mPCMRecord;
	private Thread			mRecThread;
	private Object			mSync = new Object();
	private static final int TIMER_ID = 1;
	
	
	
	public class PipopaServiceBinder extends Binder {
		public PipopaService getService() {
			return PipopaService.this;
		}
	}

	@Override
	public void onCreate() {
		super.onCreate();
	    try {
	        mStartForeground = getClass().getMethod("startForeground",
	                mStartForegroundSignature);
	        mStopForeground = getClass().getMethod("stopForeground",
	                mStopForegroundSignature);
	        
	        mDTFM = new DTFM();
	        mDTFM.init();
	        mDTFM.setSampleBits((short)16);
	        
	        
	        
	        
	    } catch (NoSuchMethodException e) {
	        // Running on an older platform.
	        mStartForeground = mStopForeground = null;
	    }

	    try {
	        mSetForeground = getClass().getMethod("setForeground",
	                mSetForegroundSignature);
	    } catch (NoSuchMethodException e) {
	    	mSetForeground = null;
	    }		
	}

	@Override
	public void onStart(Intent intent, int startId) {
		super.onStart(intent, startId);
	}

	@Override
	public void onDestroy() {
		super.onDestroy();

		// if (timerItem != null) {
		// timerItem.release();
		// timerItem = null;
		// }
		recordStop();
		if( mDTFM != null ) {
			mDTFM.destroy();
		}
	}
	
	
	@Override
	public IBinder onBind(Intent arg0) {
		return new PipopaServiceBinder();
	}
	@Override
	public void onRebind(Intent intent) {
	}
	@Override
	public boolean onUnbind(Intent intent) {
		return true; // 再度クライアントから接続された際に onRebind を呼び出させる場合は true を返す
	}


	
	
	public boolean recordStart(String strSaveDir, String strFileName)
	{
		boolean ret = false;
		
		recordStop();

		synchronized( mSync ) {
			try {
				mPCMRecord = new PCMRecordFile(mDTFM, StrageUtil.getTempDir(this), strFileName);
				mPCMRecord.setOnRecodListener(
						new PCMRecordFile.OnRecodListener() {
							@Override
							public void onRecodeEnd(RecFileInf rcf) {
								// DBに登録
								RecDbAccessor acc = new RecDbAccessor(PipopaService.this);
								acc.add(rcf);
							}
						}
				);
				
				mRecThread = new Thread( mPCMRecord );
				mRecThread.start();
				
				registNotify();
				
				PowerManager powerManager = (PowerManager) this.getSystemService(this.POWER_SERVICE);
				wakeLock = powerManager.newWakeLock(
						PowerManager.PARTIAL_WAKE_LOCK, "wakelock");
				wakeLock.acquire();
				
				
				ret = true;
			} catch( Exception e ) {
				mRecThread = null;
				mPCMRecord = null;
			}
		}
		return ret;
	}
	public void recordStop()
	{
		if( mPCMRecord != null ) {
			synchronized( mSync ) {
	    		mPCMRecord.stop();
	    		
	    		if( mRecThread != null ) {
	    			try {
	    				mRecThread.join();
	        		} catch (InterruptedException e) {
	    				e.printStackTrace();
	    			}
					mRecThread = null;
	    		}
	    		
	    		mPCMRecord = null;
	        	if( wakeLock != null ) {
	        		wakeLock.release();
					wakeLock = null;
	        	}
	    		unregistNotify();
			}
		}
	}
	public boolean isRecording() {
		if( mPCMRecord == null ) {
			return false;
		}
		return mPCMRecord.isRecording();
	}
	public long getStartTime() {
		if( mPCMRecord == null ) {
			return 0;
		}
		return mPCMRecord.getStartTime();
	}
	
	
	public void registNotify() {
		String strSummary = "Summary";
		
		NotificationManager mNM = (NotificationManager) this.getSystemService(Context.NOTIFICATION_SERVICE);
		Notification notification = new Notification(
				R.drawable.rec_24
				, "Start Recording...."
				, (long)0);
		Intent intent = new Intent(this, RecordActivity.class);
		intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
		PendingIntent contentIntent = PendingIntent.getActivity(
				this
				, 0
				, intent
				, PendingIntent.FLAG_UPDATE_CURRENT);
		notification.setLatestEventInfo(
				this
				, "Recording...."
				, strSummary
				, contentIntent);
		//notification.flags |= Notification.FLAG_AUTO_CANCEL;
		notification.flags =  Notification.FLAG_NO_CLEAR         // クリアボタンを表示しない ※ユーザがクリアできない
		          | Notification.FLAG_ONGOING_EVENT;   // 継続的イベント領域に表示 ※「実行中」領域
		
		if( mStartForeground != null ) {
			
			mStartForegroundArgs[0] = Integer.valueOf( TIMER_ID );
	        mStartForegroundArgs[1] = notification;
	        invokeMethod(mStartForeground, mStartForegroundArgs);
		} else
		if( mSetForeground != null ) {
		    // Fall back on the old API.
		    mSetForegroundArgs[0] = Boolean.TRUE;
		    invokeMethod(mSetForeground, mSetForegroundArgs);
		    mNM.notify(TIMER_ID, notification);
		}
	}
	public void unregistNotify() {
	    if (mStopForeground != null) {
	        mStopForegroundArgs[0] = Boolean.TRUE;
	        try {
	            mStopForeground.invoke(this, mStopForegroundArgs);
	        } catch (InvocationTargetException e) {
	            // Should not happen.
	            Log.w("ApiDemos", "Unable to invoke stopForeground", e);
	        } catch (IllegalAccessException e) {
	            // Should not happen.
	            Log.w("ApiDemos", "Unable to invoke stopForeground", e);
	        }
	    } else
	    if( mSetForegroundArgs != null ) {
	        mSetForegroundArgs[0] = Boolean.FALSE;
	        invokeMethod(mSetForeground, mSetForegroundArgs);
	    }
		// 通知を消去
		NotificationManager mNM = (NotificationManager)this.getSystemService(Context.NOTIFICATION_SERVICE);
		mNM.cancel(TIMER_ID);
	}
	
	
	void invokeMethod(Method method, Object[] args) {
	    try {
	        mStartForeground.invoke(this, mStartForegroundArgs);
	    } catch (InvocationTargetException e) {
	        // Should not happen.
	        Log.w("ApiDemos", "Unable to invoke method", e);
	    } catch (IllegalAccessException e) {
	        // Should not happen.
	        Log.w("ApiDemos", "Unable to invoke method", e);
	    }
	}
}
