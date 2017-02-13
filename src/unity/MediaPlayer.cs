using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;
using System;
using System.IO;
using System.Collections.Generic;

public enum MediaPlayerState {
	STOPPED,
	PAUSED,
	PLAYING,
	BUFFERING
}

public class MediaPlayer : MonoBehaviour {

	#region Plugin imports
	[DllImport("VRPlayer")]
	private static extern IntPtr GetRenderEventFunc();

	[DllImport("VRPlayer")]
	private static extern void SetDebugCallback (IntPtr ptrFunc);

	[DllImport("VRPlayer")]
	private static extern void SetAudioCallback (IntPtr ptrFunc);

	[DllImport("VRPlayer")]
	private static extern void SetUnityTexture (IntPtr tex);

	[DllImport("VRPlayer")]
	private static extern void MediaPlayerSetup (string file);

	[DllImport("VRPlayer")]
	private static extern void MediaPlayerDestroy ();

	[DllImport("VRPlayer")]
	private static extern void MediaPlayerPlay();

	[DllImport("VRPlayer")]
	private static extern void MediaPlayerPause();

	[DllImport("VRPlayer")]
	private static extern void MediaPlayerStop();

	[DllImport("VRPlayer")]
	private static extern int MediaPlayerGetState();

	[DllImport("VRPlayer")]
	private static extern float MediaPlayerGetDuration();

	[DllImport("VRPlayer")]
	private static extern float MediaPlayerGetPosition();

	[DllImport("VRPlayer")]
	private static extern int MediaPlayerGetAudioChannels();

	[DllImport("VRPlayer")]
	private static extern int MediaPlayerGetAudioSampleRate();

	[DllImport("VRPlayer")]
	private static extern void OnReadAudio(float[] data);

	[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
	public delegate void DebugDelegate (string str);

	[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
	public delegate void AudioDelegate (IntPtr audioPointer, int bufSize);
	#endregion

	#region Media player properties
	public string mediaFile;
	public int width;
	public int height;

	private Texture2D videoTexture;
	private AudioSource audioSource;
	private AudioClip audioClip;

	private float[] audioData;
	private List<float[]> audioDataQueue = new List<float[]> ();
	#endregion

	#region Behaviour callbacks
	IEnumerator Start () {

		// Setup debug
		DebugDelegate debugDelegate = new DebugDelegate(DebugCallback);
		IntPtr ptrIntDelegate = Marshal.GetFunctionPointerForDelegate (debugDelegate);
		SetDebugCallback (ptrIntDelegate);

		// Setup debug
		AudioDelegate audioDelegate = new AudioDelegate(AudioCallback);
		IntPtr ptrIntAudioDelegate = Marshal.GetFunctionPointerForDelegate (audioDelegate);
		SetAudioCallback (ptrIntAudioDelegate);

		MediaPlayerSetup (mediaFile);

		// Setup video texture
		videoTexture = new Texture2D (width, height, TextureFormat.RGB24, false);
		videoTexture.Apply();
		GetComponent<Renderer> ().material.mainTexture = videoTexture;

		audioSource = gameObject.AddComponent<AudioSource> ();

		audioClip = AudioClip.Create(
			"audioClip",
			(int)((float)MediaPlayerGetAudioSampleRate() * 600.0f),
			MediaPlayerGetAudioChannels(),
			MediaPlayerGetAudioSampleRate(),
			true,
			OnReadAudioUnity
		);
		audioSource.clip = audioClip;

		// Attach unity texture to plugin
		SetUnityTexture (videoTexture.GetNativeTexturePtr ());

		yield return StartCoroutine("CallPluginAtEndOfFrames");
	}

	void OnReadAudioUnity (float[] data) {
		OnReadAudio(data);
	}

	void OnDestroy () {
		MediaPlayerStop ();
	}

	static void DebugCallback (string str) {
		Debug.Log ("Plugin: " + str);
	}

	void AudioCallback (IntPtr audioPointer, int bufSize) {
		// float[] buffer = new float[bufSize];
		// Marshal.Copy (audioPointer, buffer, 0, bufSize);

		// float[] data = new float[buffer.Length / 4];
		// Buffer.BlockCopy (buffer, 0, data, 0, buffer.Length);

		// float[] audioData = new float[data.Length];

		// for (int i = 0; i < data.Length; i++) {
		// 	if (data[i] > 1.0f) {
		// 		audioData[i] = 1.0f;
		// 	} else if (data[i] < -1.0f) {
		// 		audioData [i] = -1.0f;
		// 	} else {
		// 		audioData [i] = (float)data [i];
		// 	}
		// }

		// audioDataQueue.Add (audioData);

		//audioSource.Play ();
	}

	void Update () {

	}

	private IEnumerator CallPluginAtEndOfFrames()
	{
		while (true) {

			// Wait until all frame rendering is done
			yield return new WaitForSeconds(1.0f / 29.95f);

			GL.IssuePluginEvent(GetRenderEventFunc(), 1);

			if (!audioSource.isPlaying) {
				audioSource.Play();
			}
		}
	}
	#endregion

	#region Media player public API

	/// <summary>
	/// Starts media player playback
	/// </summary>
	public void Play () {
		Debug.Log ("MEDIA PLAY HOORAY");
		MediaPlayerPlay ();
	}

	/// <summary>
	/// Pauses the media player
	/// </summary>
	public void Pause () {
		MediaPlayerPause ();
	}

	/// <summary>
	/// Either plays or pauses the media player based on the
	/// current state.
	/// </summary>
	/// <returns>The state after trying to either resume or pause</returns>
	public void PlayPause() {
		MediaPlayerState state = GetState ();

		if (state == MediaPlayerState.PLAYING) {
			Pause ();
		} else if (state == MediaPlayerState.PAUSED || state == MediaPlayerState.STOPPED){
			Play ();
		}
	}

	/// <summary>
	/// Get the media player state
	/// </summary>
	/// <returns>The media player state.</returns>
	public MediaPlayerState GetState () {

		/*
		 * Plugin enums are based on natural indexes, convert to
		 * 0 based index to use with behavior enum
		 */
		return (MediaPlayerState)(MediaPlayerGetState() - 1);
	}

	public void SetPosition () {
		Debug.Log ("Method not yet implemented");
	}

	public void Scrub () {
		Debug.Log ("Method not yet implemented");
	}

	#endregion

}
