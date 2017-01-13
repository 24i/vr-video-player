using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;
using System;
using System.IO;

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

	[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
	public delegate void DebugDelegate (string str);
	#endregion

	#region Media player properties
	public string mediaFile;
	public int width;
	public int height;

	private Texture2D videoTexture;
	#endregion

	#region Behaviour callbacks
	IEnumerator Start () {

		// Setup debug
		DebugDelegate debugDelegate = new DebugDelegate(DebugCallback);
		IntPtr ptrIntDelegate = Marshal.GetFunctionPointerForDelegate (debugDelegate);
		SetDebugCallback (ptrIntDelegate);

		VRVideoPlayerSetup (mediaFile);

		// Setup video texture
		videoTexture = new Texture2D (width, height, TextureFormat.RGB24, false);
		videoTexture.Apply();
		GetComponent<Renderer> ().material.mainTexture = videoTexture;

		// Attach unity texture to plugin
		SetUnityTexture (videoTexture.GetNativeTexturePtr ());
		Debug.Log (GetState ());

		yield return StartCoroutine("CallPluginAtEndOfFrames");
	}

	void OnDestroy () {
		VRVideoPlayerStop ();
	}

	static void DebugCallback (string str) {
		Debug.Log ("Plugin: " + str);
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
