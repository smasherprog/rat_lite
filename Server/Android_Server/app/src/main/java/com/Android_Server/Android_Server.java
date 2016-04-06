
package com.Android_Server;

import android.app.Activity;
import android.os.Bundle;
import android.widget.Button;
import android.widget.LinearLayout;
import android.view.View;
import android.content.Intent;

public class Android_Server extends Activity {

	private static final String TAG = Android_Server.class.getSimpleName();
	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL); 

		Button myButton = new Button(this);
	
		LinearLayout row = new LinearLayout(this);
		row.setLayoutParams(new LinearLayout.LayoutParams(LinearLayout.LayoutParams.FILL_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT));

		myButton.setLayoutParams(new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT));
		myButton.setText("Start");
		myButton.setId(78);
		row.addView(myButton);
		myButton.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				startService(new Intent(getBaseContext(), RDPService.class));
			}
		});
		 layout.addView(row);
		LinearLayout.LayoutParams params1 = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
		LinearLayout row1 = new LinearLayout(this);
		row1.setLayoutParams(new LinearLayout.LayoutParams(LinearLayout.LayoutParams.FILL_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT));
		Button myButton1 = new Button(this);

		myButton1.setLayoutParams(new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT));
		myButton1.setText("Stop");
		myButton1.setId(79);
		row1.addView(myButton1);
		 layout.addView(row1);

		myButton1.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				stopService(new Intent(getBaseContext(), RDPService.class));
			}
		});

		setContentView(layout);
	}

}
