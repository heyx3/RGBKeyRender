using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class WASDController : MonoBehaviour
{
	public Vector2 Speed = new Vector2(10.0f, 10.0f);

	private void Update()
	{
		//Get movement input.
		Vector2 localMovement = Vector2.zero;
		if (Input.GetKey(KeyCode.W))
			localMovement.y += 1.0f;
		if (Input.GetKey(KeyCode.S))
			localMovement.y -= 1.0f;
		if (Input.GetKey(KeyCode.D))
			localMovement.x += 1.0f;
		if (Input.GetKey(KeyCode.A))
			localMovement.x -= 1.0f;

		//Apply the movement.
		Vector3 pos = transform.position;
		float currentY = pos.y;
		pos += transform.forward * localMovement.y * Time.deltaTime * Speed.y;
		pos += transform.right * localMovement.x * Time.deltaTime * Speed.x;
		pos.y = currentY;
		transform.position = pos;
	}
}