#pragma strict

var feedAddress : String;
var inputField : UnityEngine.UI.InputField;
var urlDisplay : UnityEngine.UI.Text;
var finalAddress : String;
var Canvas : GameObject;
var complete : boolean = false;
 
function Start () {
	finalAddress = 'maki';
}

function Update () {
	inputField.ActivateInputField();
	feedAddress = inputField.text.ToString();
	
}
function PassUrl () {
	finalAddress = feedAddress;
	Debug.Log(finalAddress.ToString());
}

function Close () {
	Canvas.SetActive(false);
	complete = true;
}