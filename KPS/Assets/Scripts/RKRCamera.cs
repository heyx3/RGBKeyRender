using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;


/// <summary>
/// Handles rendering the attached camera to the RGB keyboard.
/// </summary>
public class RKRCamera : MonoBehaviour
{
	private Camera cam;

	private RenderTexture rendTarget;
	private Texture2D rendTargetBuffer;
	private RKRPlugin.KeyRGB[] keyColorBuffer;


	private void Awake()
	{
		cam = GetComponent<Camera>();

		rendTarget = new RenderTexture(RKRPlugin.rkrGetWidth(), RKRPlugin.rkrGetHeight(), 24);
		rendTarget.Create();
		cam.targetTexture = rendTarget;

		rendTargetBuffer = new Texture2D(rendTarget.width, rendTarget.height,
										 TextureFormat.RGB24, false);
		keyColorBuffer = new RKRPlugin.KeyRGB[rendTarget.width * rendTarget.height];
	}
	private void OnDestroy()
	{
		RKRPlugin.rkrCleanUp();
	}
	private void OnPostRender()
	{
		//Get the texture data.
		RenderTexture.active = rendTarget;
		rendTargetBuffer.ReadPixels(new Rect(0, 0, rendTarget.width, rendTarget.height), 0, 0);
		var colors = rendTargetBuffer.GetPixels32();

		//Convert to the right format for RKR.
		for (int y = 0; y < rendTarget.height; ++y)
		{
			int _y = rendTarget.height - y - 1;
			for (int x = 0; x < rendTarget.width; ++x)
			{
				int index = x + (y * rendTarget.width),
					_index = x + (_y * rendTarget.width);
				keyColorBuffer[index] = new RKRPlugin.KeyRGB(colors[_index]);
			}
		}

		RKRPlugin.rkrRenderToKeyboardSimple(keyColorBuffer);
	}
}