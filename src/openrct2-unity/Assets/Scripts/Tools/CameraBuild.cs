using UnityEngine;


// Old look around script
public class CameraBuild : MonoBehaviour 
{
	public GameObject terrainMap;
	
	public float cameraRotateSpeed = 100.0f;
	public float cameraMoveSpeed = 100.0f;
	public float cameraZoomSpeed = 1500.0f;
	
	public float minCameraHeight = 5.0f;
	public float maxCameraHeight = 150f;
	
	public float outsideMapDistance = 250.0f;

	
	Vector3 rotatepos = Vector3.zero;
	float distancepos = 0.0f;

	float x = 0.0f;
	float y = 0.0f;
	
	// ---------------------------------------------------------------------
	
	void LateUpdate () 
	{		
		// Cheaty code for changing game speed
		if (Input.GetKeyDown(KeyCode.KeypadPlus))
		{
			Time.timeScale *= 2.0f;
			//Time.fixedDeltaTime = 0.02f * Time.timeScale;
			Debug.Log("Set timescale to: " + Time.timeScale);
		}
		else if (Input.GetKeyDown(KeyCode.KeypadMinus))
		{
			Time.timeScale *= 0.5f;
			//Time.fixedDeltaTime = 0.02f * Time.timeScale;
			Debug.Log("Set timescale to: " + Time.timeScale);
		}
		
		UpdateCameraMovement ();
	}

	// ---------------------------------------------------------------------

	void UpdateCameraMovement()
	{
		// If right-mouse-button is held
		if (Input.GetMouseButton(1)) 
		{
			// The first click
			if (Input.GetMouseButtonDown(1))
			{
				RaycastHit hit;
				Ray ray = new Ray (transform.position, transform.forward);
				if (terrainMap.GetComponent<Collider>().Raycast (ray, out hit, Mathf.Infinity)) 
				{
					rotatepos = hit.point;
					distancepos = hit.distance;
					
					x = transform.eulerAngles.y;
					y = transform.eulerAngles.x;
				}
			}

            // Rotate camera with mouse axis
            if (rotatepos != Vector3.zero && distancepos != 0.0f)
            {
                float xAxis = Input.GetAxis("Mouse X");
                float yAxis = Input.GetAxis("Mouse Y");

                if (xAxis != 0f || yAxis != 0f)
                { 
                    x += xAxis * cameraRotateSpeed * Time.deltaTime;
                    y -= yAxis * cameraRotateSpeed * Time.deltaTime;

                    if (y < -360) y += 360;
                    if (y > 360) y -= 360;
                    y = Mathf.Clamp(y, 5, 89);

                    Quaternion rotation = Quaternion.Euler(y, x, 0.0f);
                    Vector3 position = rotation * new Vector3(0.0f, 0.0f, -distancepos) + rotatepos;

                    transform.rotation = rotation;
                    transform.position = position;
                }
			}
			return;
		}
		
		// Middle-mouse button
		if (Input.GetMouseButton (2))
        {
            float xAxis = Input.GetAxis("Mouse X");
            float yAxis = Input.GetAxis("Mouse Y");

            if (xAxis != 0f || yAxis != 0f)
            {
                Vector3 movement = Vector3.zero;
                movement.x = -xAxis * Time.deltaTime * cameraMoveSpeed;
                movement.z = -yAxis * Time.deltaTime * cameraMoveSpeed;

                Vector3 rotation = transform.localRotation.eulerAngles;
                rotation.x = 0.0f;
                transform.localPosition += Quaternion.Euler(rotation) * movement;
            }
			return;
		}
		
		// WASD/arrow keys pressed
		float horizontal = Input.GetAxis ("Horizontal");
		float vertical = Input.GetAxis ("Vertical");
		if (horizontal != 0.0f || vertical != 0.0f) 
		{
			Vector3 movement = Vector3.zero;
			movement.x = horizontal * Time.deltaTime * cameraMoveSpeed;
			movement.z = vertical * Time.deltaTime * cameraMoveSpeed;
			
			Vector3 rotation = transform.localRotation.eulerAngles;
			rotation.x = 0.0f;
			transform.localPosition += Quaternion.Euler (rotation) * movement;
		}
		
		// Zooming with scroll wheel
		float scrollaxis = Input.GetAxis("Mouse ScrollWheel");
		if (scrollaxis != 0.0f)
		{
			Vector3 movement = Vector3.zero;
			movement.z = scrollaxis * Time.deltaTime * cameraZoomSpeed;
			
			// Check if not colliding with limits
			if ((movement.z > 0.0f && transform.position.y == minCameraHeight) || (movement.z < 0.0f && transform.position.y == maxCameraHeight))
			{
				return;
			}
			transform.Translate (movement);
			
			// Check if between height limits
			Vector3 curpos = transform.position;
			if (curpos.y < minCameraHeight)
			{
				curpos.y = minCameraHeight;
				transform.position = curpos;
			}
			else if (curpos.y > maxCameraHeight)
			{
				curpos.y = maxCameraHeight;
				transform.position = curpos;
			}
			return;
		}
	}

	// ---------------------------------------------------------------------
}
