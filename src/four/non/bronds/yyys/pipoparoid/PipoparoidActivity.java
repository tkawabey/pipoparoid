

package four.non.bronds.yyys.pipoparoid;




import four.non.bronds.yyys.pipoparoid.common.Constant;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.AdapterView.OnItemClickListener;





public class PipoparoidActivity extends Activity {
	private static final String TAG = "PipoparoidActivity";
	private GridView 					mGrid = null;
	
    public class BindData {
        int iconId;
        String title;
  
        BindData(int id, String s) {
            this.iconId = id;
            this.title = s;
        }
    }
    static class ViewHolder {
        TextView textView;
        ImageView imageView;
    } 

    private BindData[] mDatas = {
        new BindData(R.drawable.rec_72, "PCM Record"),
        new BindData(R.drawable.dtfm_72, "DTFM Talk"),
    };
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        try {
            GridView gridview = (GridView) findViewById(R.id.gridview);
            gridview.setAdapter( new MyAdapter(this, R.layout.main_item, mDatas) );

            gridview.setOnItemClickListener(new OnItemClickListener() {
            	@Override
    			public void onItemClick(AdapterView<?> arg0, View arg1, int position, long id) {
            		if( position == 0 ) {
            			Intent intent = new Intent(PipoparoidActivity.this, RecordActivity.class);
            		
            			startActivityForResult(intent, 0);
            		}
            		if( position == 1 ) {
            			Intent intent = new Intent(PipoparoidActivity.this, DTFMActivity.class);
            		
            			startActivityForResult(intent, 0);
            		}
            	}      
            });
        } catch(Exception e) {
        	showAlert( e.getMessage() );
        }
    }
    public class MyAdapter extends ArrayAdapter<BindData> {
        private LayoutInflater inflater;
        private int layoutId;

        public MyAdapter(Context context, int layoutId, BindData[] objects) {
            super(context, 0, objects);
            this.inflater = (LayoutInflater) context
                .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            this.layoutId = layoutId;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            ViewHolder holder;

            if (convertView == null) {
                convertView = inflater.inflate(layoutId, parent, false);
                holder = new ViewHolder();
                holder.textView = (TextView) convertView.findViewById(R.id.textview);
                holder.imageView = (ImageView) convertView.findViewById(R.id.imageview);
                convertView.setTag(holder);
            } else {
                holder = (ViewHolder) convertView.getTag();
            }
            BindData data= getItem(position);
            holder.textView.setText(data.title);
            holder.imageView.setImageResource(data.iconId);
            return convertView;
        }
    }
    
    @Override
    public void onResume() {
    	super.onResume();
    	


    }
    @Override
    public void onPause() {
    	super.onPause();
    }
    @Override
    public void onDestroy() {
    	super.onDestroy();
    }
    
    /**
     * @param strMsg
     */
    private void showAlert(String strMsg)
    {
		new AlertDialog.Builder(this)
		.setIcon(R.drawable.icon)
		.setTitle(Constant.TITLE)
		.setMessage(strMsg)
		.setPositiveButton("OK", null)
		.show();
    }

}