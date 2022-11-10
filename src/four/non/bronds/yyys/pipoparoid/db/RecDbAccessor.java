package four.non.bronds.yyys.pipoparoid.db;

import java.io.File;
import java.util.Date;
import java.util.List;
import java.util.ArrayList;

import four.non.bronds.yyys.pipoparoid.bean.RecFileInf;


import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteStatement;

public class RecDbAccessor {
	private Context		mContext = null;

	public RecDbAccessor(Context ctx) {
		mContext = ctx;
	}
	
	public List<RecFileInf> load() {
		List<RecFileInf> container = new ArrayList<RecFileInf>();
		DbOpener dbOpnner = new DbOpener(mContext);
		SQLiteDatabase db = null;
		Cursor c = null;
		int length = 0, i;
		
		
		try {
			db = dbOpnner.getReadableDatabase();
			StringBuilder sb;
			
			sb = new StringBuilder();
			sb.append(" select   ");
			sb.append("    fname  ");
			sb.append("  , disp  ");
			sb.append("  , rec_date  ");
			sb.append("  , time  ");
			sb.append("  , bits_per  ");
			sb.append("  , bit  ");
			sb.append("  , chanel  ");
			sb.append("  , type  ");
			sb.append(" from REC_FILE  ");
			c = db.rawQuery(sb.toString(), null);

			
			
			
			c.moveToFirst();
			length = c.getCount();
			for (i = 0; i < length; i++) {
				RecFileInf rcf = new RecFileInf();
				rcf.setFName( c.getString(0) );
				rcf.setDisp(c.getString(1) );
				rcf.setRecDate( new Date(c.getLong(2)) );
				rcf.setTime(c.getLong(3));
				rcf.setBitsPer(c.getInt(4));
				rcf.setBit(c.getInt(5));
				rcf.setChanel(c.getInt(6));
				rcf.setType(c.getInt(7));
				
				container.add(rcf);
				
				c.moveToNext();
			}

		} finally {
			if(c != null ) {
				try { c.close(); } catch(Exception e) {}
			}
			if( db != null ) {
				try { db.close(); } catch(Exception e) {}
			}
		}
		return container;
	}
	
	public void add(RecFileInf rcf) {
		DbOpener dbOpnner = new DbOpener(mContext);
		
		SQLiteDatabase db = null;
		SQLiteStatement stmt = null;
		int index = 0;
		
		try {
			db = dbOpnner.getReadableDatabase();
			db.beginTransaction();
			
			index = 1;
			stmt = db.compileStatement("insert into REC_FILE values(?, ?, ?, ?, ?, ?, ?, ?)");
			stmt.bindString(index++, rcf.getFName());
			stmt.bindString(index++, rcf.getDisp());
			stmt.bindLong(index++, rcf.getRecDate().getTime());
			stmt.bindLong(index++, rcf.getTime());
			stmt.bindLong(index++, rcf.getBitsPer());
			stmt.bindLong(index++, rcf.getBit());
			stmt.bindLong(index++, rcf.getChanel());
			stmt.bindLong(index++, rcf.getType());
			stmt.executeInsert();
			
			
			
			db.setTransactionSuccessful();
		} finally {
			if(stmt != null ) {
				try { stmt.close(); } catch(Exception e) {}
			}
			if( db != null ) {
				try { 
					db.endTransaction();
					db.close(); 
				} catch(Exception e) {}
			}
		}
	}
	
	public void del(String strFName) {
		DbOpener dbOpnner = new DbOpener(mContext);
		
		SQLiteDatabase db = null;
		SQLiteStatement stmt = null;
		int index = 0;
		
		try {
			db = dbOpnner.getReadableDatabase();
			db.beginTransaction();
			
			try { 
				File fName = new File(strFName);
				if( fName.exists() ) {
					fName.delete();
				}
			} catch(Exception e ) {
				// noting
			}
			
			
			index = 1;
			stmt = db.compileStatement("delete from REC_FILE where fname=?");
			stmt.bindString(index++, strFName);
			stmt.executeInsert();
			
			
			db.setTransactionSuccessful();
		} finally {
			if(stmt != null ) {
				try { stmt.close(); } catch(Exception e) {}
			}
			if( db != null ) {
				try { 
					db.endTransaction();
					db.close(); 
				} catch(Exception e) {}
			}
		}		
	}

	public void update(RecFileInf rcf) {
		DbOpener dbOpnner = new DbOpener(mContext);
		
		SQLiteDatabase db = null;
		SQLiteStatement stmt = null;
		int index = 0;
		
		try {
			db = dbOpnner.getReadableDatabase();
			db.beginTransaction();
			
			index = 1;
			stmt = db.compileStatement("update REC_FILE set disp=?  where fname=?");
			
			stmt.bindString(index++, rcf.getDisp());
			stmt.bindString(index++, rcf.getFName());
			stmt.executeInsert();
			
			
			db.setTransactionSuccessful();
		} finally {
			if(stmt != null ) {
				try { stmt.close(); } catch(Exception e) {}
			}
			if( db != null ) {
				try { 
					db.endTransaction();
					db.close(); 
				} catch(Exception e) {}
			}
		}
	}
}
