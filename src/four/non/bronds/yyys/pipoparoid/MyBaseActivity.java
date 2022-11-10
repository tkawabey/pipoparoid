package four.non.bronds.yyys.pipoparoid;


import four.non.bronds.yyys.pipoparoid.common.Constant;
import android.app.Activity;
import android.app.AlertDialog;

public class MyBaseActivity extends Activity {
    /**
     * @param strMsg
     */
    protected void showAlert(String strMsg)
    {
		new AlertDialog.Builder(this)
		.setIcon(R.drawable.icon)
		.setTitle(Constant.TITLE)
		.setMessage(strMsg)
		.setPositiveButton("OK", null)
		.show();
    }
}
