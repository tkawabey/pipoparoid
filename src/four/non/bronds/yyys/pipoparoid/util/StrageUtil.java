package four.non.bronds.yyys.pipoparoid.util;

import java.io.File;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;


import four.non.bronds.yyys.pipoparoid.common.Constant;

import android.content.Context;
import android.os.Environment;



public class StrageUtil {
	
	public static Method getExternalFilesDir;
	
	static public boolean makeDir(String strDir) 
	{
		File f = new File( strDir );
    	if( f.isDirectory() == false ) {
    		f.mkdir();
    	}		
		return true;
	}
	/**
	 * ストレージの存在を確かめて、アプリケーションディレクトリを作成する
	 * @return
	 */
	public static boolean isSDcardExist(Context ctx)
	{
		String strState = Environment.getExternalStorageState() ;
		if( !strState.equals(Environment.MEDIA_MOUNTED)  ) { 
			return false;		
		}
		
		// ディレクトリの作成
    	File f = new File( StrageUtil.getBaseDir(ctx) );
    	if( f.isDirectory() == false ) {
    		f.mkdir();
    	}
    	
    	
		return true;
	}
	
	/**
	 * @param ctx
	 * @return
	 */
	public static String getBaseDir(Context ctx) {
		String str = "";
        try {
            Class<?> partypes[] = new Class[1];
            partypes[0] = String.class;
            getExternalFilesDir = Context.class.getMethod("getExternalFilesDir", partypes);
            
            Object arglist[] = new Object[1];
            arglist[0] = null;  

			File path = (File)getExternalFilesDir.invoke(ctx, arglist);

			str = path.getPath() + "/" + Constant.FS_APP_DIR_NAME;
		} catch (NoSuchMethodException e) {
			java.io.File file = Environment.getExternalStorageDirectory();
			str = file.getPath() + "/" + Constant.FS_APP_DIR_NAME;
		}  catch (IllegalArgumentException e) {
			java.io.File file = Environment.getExternalStorageDirectory();
			str = file.getPath() + "/" + Constant.FS_APP_DIR_NAME;
		} catch (IllegalAccessException e) {
			java.io.File file = Environment.getExternalStorageDirectory();
			str = file.getPath() + "/" + Constant.FS_APP_DIR_NAME;
		} catch (InvocationTargetException e) {
			java.io.File file = Environment.getExternalStorageDirectory();
			str = file.getPath() + "/" + Constant.FS_APP_DIR_NAME;
		}
		
		
		
		makeDir(str);
		
		return str;
	}
	
	
	public static String getTempDir(Context ctx) {
		String str = "";
		
		str = getBaseDir(ctx);
		str += "/tmp";
		
		makeDir(str);
		return str;
	}
	
	public static String getRecDir(Context ctx) {
		String str = "";
		
		str = getBaseDir(ctx);
		str += "/rec";
		
		makeDir(str);
		return str;
	}
}
