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
	[DllImport("main")]
	private static extern IntPtr GetRenderEventFunc();

	[DllImport("main")]
	private static extern void SetDebugCallback (IntPtr ptrFunc);

	[DllImport("main")]
	private static extern void SetAudioCallback (IntPtr ptrFunc);

	[DllImport("main")]
	private static extern void SetUnityTexture (IntPtr tex);

	[DllImport("main")]
	private static extern void VRVideoPlayerSetup (string file);

	[DllImport("main")]
	private static extern void VRVideoPlayerDestroy ();

	[DllImport("main")]
	private static extern void VRVideoPlayerPlay();

	[DllImport("main")]
	private static extern void VRVideoPlayerPause();

	[DllImport("main")]
	private static extern void VRVideoPlayerStop();

	[DllImport("main")]
	private static extern int VRVideoPlayerGetState();

	[DllImport("main")]
	private static extern int VRVideoPlayerGetAudioChannels();

	[DllImport("main")]
	private static extern int VRVideoPlayerGetAudioSampleRate();

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

		VRVideoPlayerSetup (mediaFile);

		// Setup video texture
		videoTexture = new Texture2D (width, height, TextureFormat.RGB24, false);
		videoTexture.Apply();
		GetComponent<Renderer> ().material.mainTexture = videoTexture;

		audioSource = gameObject.AddComponent<AudioSource> ();

		audioClip = AudioClip.Create(
			"audioClip", 
			(int)((float)VRVideoPlayerGetAudioSampleRate() * 600.0f),
			VRVideoPlayerGetAudioChannels(),
			VRVideoPlayerGetAudioSampleRate(),
			false
		);

		audioSource.clip = audioClip;

		// Attach unity texture to plugin
		SetUnityTexture (videoTexture.GetNativeTexturePtr ());
		yield return StartCoroutine("CallPluginAtEndOfFrames");
	}

	void OnDestroy () {
		VRVideoPlayerStop ();
	}

	static void DebugCallback (string str) {
		Debug.Log ("Plugin: " + str);
	}

	void AudioCallback (IntPtr audioPointer, int bufSize) {
		float[] buffer = new float[bufSize];
		Marshal.Copy (audioPointer, buffer, 0, bufSize);

		float[] data = new float[buffer.Length / 4];
		Buffer.BlockCopy (buffer, 0, data, 0, buffer.Length);

		float[] audioData = new float[data.Length];

		for (int i = 0; i < data.Length; i++) {
			if (data[i] > 1.0f) {
				audioData[i] = 1.0f;
			} else if (data[i] < -1.0f) {
				audioData [i] = -1.0f;
			} else {
				audioData [i] = (float)data [i];
			}
		}

		audioDataQueue.Add (audioData);

		//audioSource.Play ();
	}

	private int offset = 0;
	void Update () {
		if (audioDataQueue.Count > 0) {
			try {
				audioClip.SetData (audioDataQueue [0], offset);
				audioDataQueue.RemoveAt (0);
				offset++;

				audioSource.Play();

			} catch (Exception e) {
				Debug.Log (e.Message);
				Debug.Log (e.Source);
				Debug.Log (e.StackTrace);
			}

		}
	}

	private IEnumerator CallPluginAtEndOfFrames()
	{
		while (true) {
			// Wait until all frame rendering is done
			yield return new WaitForEndOfFrame();

			GL.IssuePluginEvent(GetRenderEventFunc(), 1);
		}
	}
	#endregion

	#region Media player public API

	/// <summary>
	/// Starts media player playback
	/// </summary>
	public void Play () {
		VRVideoPlayerPlay ();
	}

	/// <summary>
	/// Pauses the media player
	/// </summary>
	public void Pause () {
		VRVideoPlayerPause ();
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
		return (MediaPlayerState)(VRVideoPlayerGetState() - 1);
	}

	public void SetPosition () {
		Debug.Log ("Method not yet implemented");
	}

	public void Scrub () {
		Debug.Log ("Method not yet implemented");
	}

	#endregion

}
