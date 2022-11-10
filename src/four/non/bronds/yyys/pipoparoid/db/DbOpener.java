package four.non.bronds.yyys.pipoparoid.db;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

public class DbOpener extends SQLiteOpenHelper {

	public DbOpener(Context context) {
		super(context, "pipopa.db", null, 1);
	}

	@Override
	public void onCreate(SQLiteDatabase db) {
		StringBuilder sb;
		
		//REC_FILE
		sb = new StringBuilder();
		sb.append("CREATE TABLE REC_FILE (");
		sb.append("     fname varchar(255) NOT NULL ");
		sb.append("   , disp varchar(256) NOT NULL ");
		sb.append("   , rec_date Integer");
		sb.append("   , time Integer");
		sb.append("   , bits_per Integer");
		sb.append("   , bit Integer");
		sb.append("   , chanel Integer");
		sb.append("   , type Integer");
		sb.append("   ,  CONSTRAINT PK_REC_FILE  PRIMARY KEY  (fname)");
		sb.append(")");
		db.execSQL(sb.toString());
	}
	

	@Override
	public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
		
	}
}
