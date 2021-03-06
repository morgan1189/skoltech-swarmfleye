﻿#pragma strict

var url = "http://www.unity3d.com/webplayers/Movie/sample.ogg";
function OnMouseDown () {
 // Start download
 var www = new WWW(url);
// Make sure the movie is ready to start before we start playing
 var movieTexture = www.movie;
 while (!movieTexture.isReadyToPlay)
 	yield;
// Initialize texture to be 1:1 resolution
 GetComponent.<Renderer>().material.mainTexture = movieTexture;
// Assign clip to audio source
 // Sync playback with audio

// Play both movie & sound
 movieTexture.Play();

}
function Update () {
}