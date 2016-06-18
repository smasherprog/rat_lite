package com.Android_Server;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.PixelFormat;
import android.graphics.Point;
import android.hardware.display.DisplayManager;
import android.hardware.display.VirtualDisplay;
import android.media.Image;
import android.media.ImageReader;
import android.media.projection.MediaProjection;
import android.media.projection.MediaProjectionManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.OrientationEventListener;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;
import java.nio.ByteBuffer;

public class Android_Server extends Activity {

	public native void StartService();

	public native void StopService();

	public native void OnImage(ByteBuffer b, int w, int h);

	private static final String TAG = Android_Server.class.getName();
	private static final int REQUEST_CODE = 100;

	private static final int VIRTUAL_DISPLAY_FLAGS = DisplayManager.VIRTUAL_DISPLAY_FLAG_AUTO_MIRROR;
	private MediaProjection sMediaProjection;
	private static final String SCREENCAP_NAME = "screencap";
	private MediaProjectionManager mProjectionManager;
	private ImageReader mImageReader;
	private Handler mHandler;
	private Display mDisplay;
	private VirtualDisplay mVirtualDisplay;
	private int mDensity;
	private int mWidth;
	private int mHeight;
	private int mRotation;
	private OrientationChangeCallback mOrientationChangeCallback;

	static {
		System.loadLibrary("Android_RDPServerBindings");
	}

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		// call for the projection manager
		mProjectionManager = (MediaProjectionManager) getSystemService(Context.MEDIA_PROJECTION_SERVICE);

		TextView txt = (TextView) findViewById(R.id.ipareatext);
		txt.setText(Utils.getIPAddress(true));
	
		// start projection
		Button startButton = (Button) findViewById(R.id.startButton);
		startButton.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				startProjection();
			}
		});

		// stop projection
		Button stopButton = (Button) findViewById(R.id.stopButton);
		stopButton.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				stopProjection();
			}
		});
		// start capture handling thread
		new Thread() {
			@Override
			public void run() {
				Looper.prepare();
				mHandler = new Handler();
				Looper.loop();
			}
		}.start();
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (requestCode == REQUEST_CODE) {
			sMediaProjection = mProjectionManager.getMediaProjection(resultCode, data);

			if (sMediaProjection != null) {
				StartService();
				// display metrics
				DisplayMetrics metrics = getResources().getDisplayMetrics();
				mDensity = metrics.densityDpi;
				mDisplay = getWindowManager().getDefaultDisplay();

				// create virtual display depending on device width / height
				createVirtualDisplay();

				// register orientation change callback
				mOrientationChangeCallback = new OrientationChangeCallback(this);
				if (mOrientationChangeCallback.canDetectOrientation()) {
					mOrientationChangeCallback.enable();
				}

				// register media projection stop callback
				sMediaProjection.registerCallback(new MediaProjectionStopCallback(), mHandler);
			}
		}
	}

	/****************************************** UI Widget Callbacks *******************************/
	private void startProjection() {
		startActivityForResult(mProjectionManager.createScreenCaptureIntent(), REQUEST_CODE);
	}

	private void stopProjection() {
		mHandler.post(new Runnable() {
			@Override
			public void run() {
				if (sMediaProjection != null) {
					sMediaProjection.stop();
				}
			}
		});
	}

	private class MediaProjectionStopCallback extends MediaProjection.Callback {
		@Override
		public void onStop() {
			mHandler.post(new Runnable() {
				@Override
				public void run() {

					Log.e("ScreenCapture", "stopping projection.");

					if (mVirtualDisplay != null)
						mVirtualDisplay.release();
					if (mImageReader != null)
						mImageReader.setOnImageAvailableListener(null, null);
					if (mOrientationChangeCallback != null)
						mOrientationChangeCallback.disable();
					sMediaProjection.unregisterCallback(MediaProjectionStopCallback.this);
					StopService();

				}
			});

		}
	}

	private void createVirtualDisplay() {
		// get width and height
		Point size = new Point();
		mDisplay.getSize(size);
		mWidth = size.x;
		mHeight = size.y;

		// start capture reader
		mImageReader = ImageReader.newInstance(mWidth, mHeight, PixelFormat.RGBA_8888, 2);
		mVirtualDisplay = sMediaProjection.createVirtualDisplay(SCREENCAP_NAME, mWidth, mHeight, mDensity,
				VIRTUAL_DISPLAY_FLAGS, mImageReader.getSurface(), null, mHandler);
		mImageReader.setOnImageAvailableListener(new ImageAvailableListener(), mHandler);
	}

	private class OrientationChangeCallback extends OrientationEventListener {
		public OrientationChangeCallback(Context context) {
			super(context);
		}

		@Override
		public void onOrientationChanged(int orientation) {
			synchronized (this) {
				final int rotation = mDisplay.getRotation();
				if (rotation != mRotation) {
					mRotation = rotation;
					try {
						// clean up
						if (mVirtualDisplay != null)
							mVirtualDisplay.release();
						if (mImageReader != null)
							mImageReader.setOnImageAvailableListener(null, null);

						// re-create virtual display depending on device width / height
						createVirtualDisplay();
					} catch (Exception e) {
						e.printStackTrace();
					}
				}
			}
		}
	}

	private class ImageAvailableListener implements ImageReader.OnImageAvailableListener {
		@Override
		public void onImageAvailable(ImageReader reader) {
			Image image = null;
			try {
				image = reader.acquireLatestImage();
				if (image != null) {
					Image.Plane[] planes = image.getPlanes();
					ByteBuffer buffer = planes[0].getBuffer();
					int pixelStride = planes[0].getPixelStride();
					int rowStride = planes[0].getRowStride();
					int rowPadding = rowStride - pixelStride * mWidth;

					OnImage(buffer, mWidth + rowPadding / pixelStride, mHeight);

					//bitmap = Bitmap.createBitmap(mWidth + rowPadding / pixelStride, mHeight, Bitmap.Config.ARGB_8888);
					//bitmap.copyPixelsFromBuffer(planes[0].getBuffer());

					Log.e(TAG, "captured image: ");
				}

			} catch (Exception e) {
				e.printStackTrace();
			} finally {
				if (image != null) {
					image.close();
				}
			}
		}
	}
}