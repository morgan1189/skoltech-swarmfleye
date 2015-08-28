// Get the latest webcam shot from outside "Friday's" in Times Square
using UnityEngine;
using System.Collections;

public class image : MonoBehaviour {
	public string url = "file:///Users/Student/Desktop/maki.png";
	IEnumerator Start() {
		WWW www = new WWW(url);
		yield return www;
		Renderer renderer = GetComponent<Renderer>();
		renderer.material.mainTexture = www.texture;
	}
}