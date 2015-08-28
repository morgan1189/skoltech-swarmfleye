#pragma strict
	// Continuously get the latest webcam shot from the url provided (ffserver on the local network)
	// and DXT compress them at runtime
	var url = "http://192.168.1.50:8090/test.jpg";
	var i : int = 0;
	
	function Start () {
		// Create a texture in DXT1 format
		GetComponent.<Renderer>().material.mainTexture = new Texture2D(4, 4, TextureFormat.DXT1, false);
		//Debug.Log(i);	
	}	
	
	function Update(){
		GetImage();
		
	
	}
	function GetImage(){
			// Start a download of the given URL
			//Debug.Log('First one in while loop');
			var www = new WWW(url);
			
			//Debug.Log('Between www and yield');
			// wait until the download is done
			//while (www.isDone == false){
			//	Debug.Log(www.progress);
			//	Debug.Log(www.error);
			//}
			yield www;

			//Debug.Log(www.isDone);
			// assign the downloaded image to the main texture of the object
			www.LoadImageIntoTexture(GetComponent.<Renderer>().material.mainTexture);
			//Debug.Log(i);
			//i++;
	} 
