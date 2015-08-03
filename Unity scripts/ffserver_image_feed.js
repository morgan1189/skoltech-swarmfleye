#pragma strict
	// Continuously get the latest webcam shot from the url provided (ffserver on the local network)
	// and DXT compress them at runtime
	var url = "http://192.168.1.34:8090/test.jpg";

	function Start () {
		// Create a texture in DXT1 format
		GetComponent.<Renderer>().material.mainTexture = new Texture2D(4, 4, TextureFormat.DXT1, false);
		while(true) {
			// Start a download of the given URL
			var www = new WWW(url);

			// wait until the download is done
			yield www;

			// assign the downloaded image to the main texture of the object
			www.LoadImageIntoTexture(GetComponent.<Renderer>().material.mainTexture);
		}
	}
	