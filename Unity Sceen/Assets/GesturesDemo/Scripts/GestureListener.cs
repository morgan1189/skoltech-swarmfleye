using UnityEngine;
using System.Collections;
using System;
using UnityEngine.UI;

public static class ShortGestureMessages
{
	public const string None = "NONE";
	public const string RaiseRightHand = "RRHD";
	public const string RaiseLeftHand = "RLHD";
	public const string Psi = "PSII";
	public const string Tpose = "TPSE";
	public const string Stop = "STOP";
	public const string Wave = "WAVE";
	public const string Click = "CLCK";
	public const string SwipeLeft = "SWLT";
	public const string SwipeRight = "SWRT";
	public const string SwipeUp = "SWUP";
	public const string SwipeDown = "SWDN";
	public const string RightHandCursor = "RHCR";
	public const string LeftHandCursor = "LHCR";
	public const string ZoomOut = "ZMOT";
	public const string ZoomIn = "ZMIN";
	public const string Wheel = "WHEL";
	public const string Jump = "JUMP";
	public const string Squat = "SQUT";
	public const string Push = "PUSH";
	public const string Pull = "PULL";
}

public class GestureListener : MonoBehaviour, KinectGestures.GestureListenerInterface
{
	// GUI Text to display the gesture messages.
	//public GUIText GestureInfo;
	public Text GestureInfo;
	private bool swipeLeft;
	private bool swipeRight;
	private bool swipeUp;
	private bool swipeDown;
	private bool zoomIn;
	private bool zoomOut;
	private bool noone;
	
	public bool IsSwipeLeft()
	{
		if(swipeLeft)
		{
			swipeLeft = false;
			return true;
		}
		
		return false;
	}
	
	public bool IsSwipeRight()
	{
		if(swipeRight)
		{
			swipeRight = false;
			return true;
		}
		
		return false;
	}
	
	public bool IsSwipeUp()
	{
		if(swipeUp)
		{
			swipeUp = false;
			return true;
		}
		
		return false;
	}

	public bool IsSwipeDown()
	{
		if(swipeDown)
		{
			swipeDown = false;
			return true;
		}
		
		return false;
	}

	public bool IsZoomIn()
	{
		if(zoomIn)
		{
			zoomIn = false;
			return true;
		}
		
		return false;
	}
	
	public bool IsZoomOut()
	{
		if(zoomOut)
		{
			zoomOut = false;
			return true;
		}
		
		return false;
	}

	public bool IsNoone()
	{
		if(noone)
		{
			noone = false;
			return true;
		}
		
		return false;
	}
	
	public void UserDetected(uint userId, int userIndex)
	{
		// detect these user specific gestures
		KinectManager manager = KinectManager.Instance;
		
		manager.DetectGesture(userId, KinectGestures.Gestures.SwipeLeft);
		manager.DetectGesture(userId, KinectGestures.Gestures.SwipeRight);

		if(GestureInfo != null)
		{
			GestureInfo.GetComponent<Text>().text = "Try some gestures!";
		}
	}
	
	public void UserLost(uint userId, int userIndex)
	{
		if(GestureInfo != null)
		{
			GestureInfo.GetComponent<Text>().text = string.Empty;
		}
	}

	public void GestureInProgress(uint userId, int userIndex, KinectGestures.Gestures gesture, 
	                              float progress, KinectWrapper.NuiSkeletonPositionIndex joint, Vector3 screenPos)
	{
		// don't do anything here
	}

	public bool GestureCompleted (uint userId, int userIndex, KinectGestures.Gestures gesture, 
	                              KinectWrapper.NuiSkeletonPositionIndex joint, Vector3 screenPos)
	{
		string sGestureText = gesture + " detected";
		if(GestureInfo != null)
		{
			GestureInfo.GetComponent<Text>().text = sGestureText;
		}
		
		if (gesture == KinectGestures.Gestures.SwipeLeft)
			swipeLeft = true;
		else if (gesture == KinectGestures.Gestures.SwipeRight)
			swipeRight = true;
		else if (gesture == KinectGestures.Gestures.SwipeUp)
			swipeUp = true;
		else if (gesture == KinectGestures.Gestures.SwipeDown)
			swipeDown = true;
		else if (gesture == KinectGestures.Gestures.ZoomIn)
			zoomIn = true;
		else if (gesture == KinectGestures.Gestures.ZoomOut)
			zoomOut = true;
		else if (gesture == KinectGestures.Gestures.None)
			noone = true;

		if (gesture != KinectGestures.Gestures.None)
		{
			switch (gesture) {
				case (KinectGestures.Gestures.SwipeLeft):
					gameObject.SendMessage("OnGestureRecognized", ShortGestureMessages.SwipeLeft);
					break;
				case (KinectGestures.Gestures.SwipeRight):
					gameObject.SendMessage("OnGestureRecognized", ShortGestureMessages.SwipeRight);
					break;
				case (KinectGestures.Gestures.RaiseRightHand):
					gameObject.SendMessage("OnGestureRecognized", ShortGestureMessages.RaiseRightHand);
					break;
				case (KinectGestures.Gestures.RaiseLeftHand):
					gameObject.SendMessage("OnGestureRecognized", ShortGestureMessages.RaiseLeftHand);
					break;
				case (KinectGestures.Gestures.Psi):
					gameObject.SendMessage("OnGestureRecognized", ShortGestureMessages.Psi);
					break;
				case (KinectGestures.Gestures.Tpose):
					gameObject.SendMessage("OnGestureRecognized", ShortGestureMessages.Tpose);
					break;
				case (KinectGestures.Gestures.Stop):
					gameObject.SendMessage("OnGestureRecognized", ShortGestureMessages.Stop);
					break;
				case (KinectGestures.Gestures.Wave):
					gameObject.SendMessage("OnGestureRecognized", ShortGestureMessages.Wave);
					break;
				case (KinectGestures.Gestures.Click):
					gameObject.SendMessage("OnGestureRecognized", ShortGestureMessages.Click);
					break;
				case (KinectGestures.Gestures.SwipeUp):
					gameObject.SendMessage("OnGestureRecognized", ShortGestureMessages.SwipeUp);
					break;
				case (KinectGestures.Gestures.SwipeDown):
					gameObject.SendMessage("OnGestureRecognized", ShortGestureMessages.SwipeDown);
					break;
				case (KinectGestures.Gestures.RightHandCursor):
					gameObject.SendMessage("OnGestureRecognized", ShortGestureMessages.RightHandCursor);
					break;
				case (KinectGestures.Gestures.LeftHandCursor):
					gameObject.SendMessage("OnGestureRecognized", ShortGestureMessages.LeftHandCursor);
					break;
				case (KinectGestures.Gestures.ZoomOut):
					gameObject.SendMessage("OnGestureRecognized", ShortGestureMessages.ZoomOut);
					break;
				case (KinectGestures.Gestures.ZoomIn):
					gameObject.SendMessage("OnGestureRecognized", ShortGestureMessages.ZoomIn);
					break;
				case (KinectGestures.Gestures.Wheel):
					gameObject.SendMessage("OnGestureRecognized", ShortGestureMessages.Wheel);
					break;
				case (KinectGestures.Gestures.Jump):
					gameObject.SendMessage("OnGestureRecognized", ShortGestureMessages.Jump);
					break;
				case (KinectGestures.Gestures.Squat):
					gameObject.SendMessage("OnGestureRecognized", ShortGestureMessages.Squat);
					break;
				case (KinectGestures.Gestures.Push):
					gameObject.SendMessage("OnGestureRecognized", ShortGestureMessages.Push);
					break;
				case (KinectGestures.Gestures.Pull):
					gameObject.SendMessage("OnGestureRecognized", ShortGestureMessages.Pull);
					break;
			}
		}

		return true;
	}



	public bool GestureCancelled (uint userId, int userIndex, KinectGestures.Gestures gesture, 
	                              KinectWrapper.NuiSkeletonPositionIndex joint)
	{
		// don't do anything here, just reset the gesture state
		return true;
	}
	
}
