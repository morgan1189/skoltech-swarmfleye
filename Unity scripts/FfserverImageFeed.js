#pragma strict
	// Continuously get the latest webcam shot from the url provided (ffserver on the local network)
	// and DXT compress them at runtime
	var feedUrl : GameObject;
	var urlObject : VideoFeedUrl;
	var url : String;
	// When defining server URL directly in the script, just put GetFeed function in the Start function,
	// and commnet out unnecessary parts  
	// var url = "http://192.168.1.39:8090/test.jpg";
	
	function Start () {
		urlObject = feedUrl.GetComponent(VideoFeedUrl); 
	}	

	function Update () {
		if (urlObject.complete == true) {
			url = urlObject.finalAddress.ToString();
			GetFeed();
			Debug.Log('flag');
		}
	}
	
	function GetFeed () {
		urlObject.complete = false;
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
	
