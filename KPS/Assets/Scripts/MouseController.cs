using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class MouseController : MonoBehaviour
{
	public Vector2 SpeedScale = new Vector2(10.0f, 10.0f);
	public float MaxPitch = 60.0f;


	private void Awake()
	{
		Cursor.visible = false;
		Cursor.lockState = CursorLockMode.Locked;
	}
	private void Update()
	{
		//Get the change in mouse position.
		Vector2 delta = new Vector2(Input.GetAxis("Mouse X"), Input.GetAxis("Mouse Y"));

		//Rotate the camera.
		transform.Rotate(Vector3.up, delta.x, Space.World);
		transform.Rotate(Vector3.right, -delta.y, Space.Self);

		//Keep the camera from pitching too far up or down.
		Vector3 eulerAngles = transform.eulerAngles;
		eulerAngles.z = 0.0f;
		if (eulerAngles.x <= 180.0f)
			eulerAngles.x = Math.Min(eulerAngles.x, MaxPitch);
		else
			eulerAngles.x = Math.Max(eulerAngles.x, 360.0f - MaxPitch);
		transform.eulerAngles = eulerAngles;
	}
}
