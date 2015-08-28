using UnityEngine;
using System.Collections;
using System.Net;
using System.Net.Sockets;
using System.Text;


public class ClientControl : MonoBehaviour
{
	const int kInputWidth = 100;


	string input = "";
	bool send = false;
	Socket socket;


	void OnClientStarted (Socket socket)
	{
		Debug.Log ("Client started");

		this.socket = socket;
		SocketRead.Begin (socket, OnReceive, OnError);
	}


	void OnReceive (SocketRead read, byte[] data)
	{
		Debug.Log (Encoding.ASCII.GetString (data, 0, data.Length));
	}


	void OnError (SocketRead read, System.Exception exception)
	{
		Debug.LogError ("Receive error: " + exception);
	}
}
