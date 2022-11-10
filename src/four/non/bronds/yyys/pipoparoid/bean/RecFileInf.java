package four.non.bronds.yyys.pipoparoid.bean;

import java.io.Serializable;
import java.util.Date;

public class RecFileInf implements Serializable {


	/** Serial ID */
	private static final long serialVersionUID = 515559359187693860L;

	private String		mFName = "";
	private String		mDisp = "";
	private Date		mRecDate = null;
	private long		mTime	= 0;
	private int			mBitsPer = 0;
	private int			mBit = 0;
	private int			mChanel = 0;
	private int			mType = 0;
	
	
	public String getFName() {
		return mFName;
	}
	public void setFName(String mFName) {
		this.mFName = mFName;
	}
	
	
	public String getDisp() {
		return mDisp;
	}
	public void setDisp(String mDisp) {
		this.mDisp = mDisp;
	}
	
	
	public Date getRecDate() {
		return mRecDate;
	}
	public void setRecDate(Date mRecDate) {
		this.mRecDate = mRecDate;
	}
	
	
	public long getTime() {
		return mTime;
	}
	public void setTime(long mTime) {
		this.mTime = mTime;
	}
	
	
	public int getBitsPer() {
		return mBitsPer;
	}
	public void setBitsPer(int mBitsPer) {
		this.mBitsPer = mBitsPer;
	}
	
	
	public int getBit() {
		return mBit;
	}
	public void setBit(int mBit) {
		this.mBit = mBit;
	}
	
	
	public int getChanel() {
		return mChanel;
	}
	public void setChanel(int mChanel) {
		this.mChanel = mChanel;
	}
	
	
	public int getType() {
		return mType;
	}
	public void setType(int mType) {
		this.mType = mType;
	}
	
	
	
	public String getPlayTime() {
		long lVal = mTime / 1000;	// 秒にする
		long lHour   = lVal / (60*60);
		long lMinite = (lVal - ( lHour * (60*60) ))/60;
		long lSec    = lVal % 60;
		return String.format("%02d:%02d:%02d", lHour, lMinite, lSec);
	}
	
	public String getDateStr() {
		java.text.SimpleDateFormat formatter = new java.text.SimpleDateFormat(
				"MM/dd HH:mm:ss");
		return formatter.format(mRecDate);
	}


}
