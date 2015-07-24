using UnityEngine;
using System.Collections;

public class v2 : MonoBehaviour {

	public string uri = "http://www.sudftw.com/imageppc.php";
	//public string uri = "https://wicron.com/en/webot";
	public string username;
	public string password;
	//public string username = "skoltech1.0052";
	//public string password = "CwrvHPTii";
	public Renderer rend;

	Texture2D cam; 



	// Use this for initialization
	public void Start () {
		rend = GetComponent<Renderer>();
		cam = new Texture2D (1, 1, TextureFormat.RGB24, false);
		StartCoroutine (Fetch ());
		rend.material.mainTexture = cam;

	}

	public IEnumerator Fetch() {
		while (true) {
			Debug.Log("fetching... "+Time.realtimeSinceStartup);
		
			WWWForm form = new WWWForm();
			form.AddField("dummy", "field");
			WWW www = new WWW(uri, form.data, new System.Collections.Generic.Dictionary<string,string>() {
				{"Authorization", "Basic" + System.Convert.ToBase64String(System.Text.Encoding.ASCII.GetBytes(username+":"+password))}
			});
			yield return www;

			www.LoadImageIntoTexture(cam);
		}
	}

	//public void OnGUI() {
		//GUI.DrawTexture (new Rect (5, 5, 5, 5), cam);
	//}
}
