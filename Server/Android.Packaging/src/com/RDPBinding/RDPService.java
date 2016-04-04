package com.RDPBinding;

import android.app.Service;
import android.widget.Toast;
import android.content.Intent;
import android.os.IBinder;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.HandlerThread;
import android.os.Process;


public class RDPService extends Service {
	private static final String TAG = RDPService.class.getSimpleName();
	private Looper mServiceLooper;
	private ServiceHandler mServiceHandler;

	public native long StartService();
	public native boolean StopService(long p);

	private final class ServiceHandler extends Handler {
		public ServiceHandler(Looper looper) {
			super(looper);
		}

		@Override
		public void handleMessage(Message msg) {

			long UnMangedPtr = StartService();

			try {
				Thread.sleep(5000);
			} catch (InterruptedException e) {
				// Restore interrupt status.
				Thread.currentThread().interrupt();
			}
			StopService(UnMangedPtr);
			stopSelf(msg.arg1);
		}
	}

	static {
		System.loadLibrary("Android_Server");
	}

	@Override
	public void onCreate() {
		HandlerThread thread = new HandlerThread("ServiceStartArguments", Process.THREAD_PRIORITY_BACKGROUND);
		thread.start();

		// Get the HandlerThread's Looper and use it for our Handler
		mServiceLooper = thread.getLooper();
		mServiceHandler = new ServiceHandler(mServiceLooper);
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		Message msg = mServiceHandler.obtainMessage();
		msg.arg1 = startId;
		mServiceHandler.sendMessage(msg);

		Toast.makeText(this, " onStartCommand ", Toast.LENGTH_SHORT).show();
		return START_NOT_STICKY;
	}

	@Override
	public IBinder onBind(Intent intent) {
		// A client is binding to the service with bindService()
		Toast.makeText(this, " onBind", Toast.LENGTH_SHORT).show();
		return null;
	}

	@Override
	public boolean onUnbind(Intent intent) {
		// All clients have unbound with unbindService()
		Toast.makeText(this, " onUnbind", Toast.LENGTH_SHORT).show();
		return false;
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

	}

}