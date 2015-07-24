#pragma strict
// Starts the default camera and assigns the texture to the current renderer
function Start() {
	var webcamTexture: WebCamTexture = new WebCamTexture();
	var renderer: Renderer = GetComponent.<Renderer>();
	renderer.material.mainTexture = webcamTexture;
	webcamTexture.Play();
}