using System;
using System.Runtime.InteropServices;
using UnityEngine;


public static class RKRPlugin
{
	public enum ErrorCodes : long
	{
		Invalid = -1,
		GeneralFailure = 2147500037,

		Success = 0,

		AccessDenied = 5,
		InvalidHandle = 6,
		NotSupported = 50,
		InvalidParam = 87,
		ServiceNotActive = 1062,
		SingleInstanceApp = 1152,
		DeviceNotConnected = 1167,
		NotFound = 1167,
		RequestAborted = 1235,
		AlreadyInitialized = 1247,
		ResourceDisabled = 4309,
		DeviceNotAvailable = 4319,
		NotValidState = 5023,
		NoMoreItems = 259,
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct KeyRGB
	{
		public byte R, G, B;
		public KeyRGB(byte r, byte g, byte b) { R = r; G = g; B = b; }
		public KeyRGB(Color32 col)
		{
			R = col.r;
			G = col.g;
			B = col.b;
		}
	}


	[DllImport("RGBKeyRender")]
	public static extern byte rkrGetWidth();
	[DllImport("RGBKeyRender")]
	public static extern byte rkrGetHeight();
	
	[DllImport("RGBKeyRender")]
	public static extern ErrorCodes rkrRenderToKeyboard(uint imgWidth, uint imgHeight,
														KeyRGB[] texData);
	[DllImport("RGBKeyRender")]
	public static extern ErrorCodes rkrRenderToKeyboardSimple(KeyRGB[] texData);
	
	[DllImport("RGBKeyRender")]
	public static extern ErrorCodes rkrCleanUp();
}