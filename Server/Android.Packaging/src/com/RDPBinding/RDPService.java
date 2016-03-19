package com.RDPBinding;

import android.app.Service;
import android.widget.Toast;
import android.content.Intent;
import android.os.IBinder;

public class RDPService extends Service {
	private static final String TAG = RDPService.class.getSimpleName();

	private long UnMangedPtr;

	public native long StartService();

	public native boolean StopService(long p);

	int mStartMode; // indicates how to behave if the service is killed
	IBinder mBinder; // interface for clients that bind
	boolean mAllowRebind; // indicates whether onRebind should be used


	public RDPService() {
		System.loadLibrary("Android_Server");
		UnMangedPtr = 0;
		mStartMode = START_STICKY;
		mBinder = null;
		mAllowRebind = true;
	}

	@Override
	public void onCreate() {
		// The service is being created
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		// The service is starting, due to a call to startService()
		UnMangedPtr = StartService();
		Toast.makeText(this, " onStartCommand " + UnMangedPtr, Toast.LENGTH_SHORT).show();
		return mStartMode;
	}

	@Override
	public IBinder onBind(Intent intent) {
		// A client is binding to the service with bindService()
		Toast.makeText(this, " onBind", Toast.LENGTH_SHORT).show();
		return mBinder;
	}

	@Override
	public boolean onUnbind(Intent intent) {
		// All clients have unbound with unbindService()
		Toast.makeText(this, " onUnbind", Toast.LENGTH_SHORT).show();
		return mAllowRebind;
	}

	@Override
	public void onRebind(Intent intent) {
		// A client is binding to the service with bindService(),
		Toast.makeText(this, " onRebind", Toast.LENGTH_SHORT).show();
		// after onUnbind() has already been called
	}

	@Override
	public void onDestroy() {
		// The service is no longer used and is being destroyed
		Toast.makeText(this, " onDestroy", Toast.LENGTH_SHORT).show();
		StopService(UnMangedPtr);
		UnMangedPtr = 0;
	}

}