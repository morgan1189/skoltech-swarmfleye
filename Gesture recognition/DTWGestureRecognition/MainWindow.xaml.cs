namespace DTWGestureRecognition
{
    using System;
    using System.Collections;
    using System.Collections.Generic;
    using System.Diagnostics;
    using System.Windows;
    using System.Windows.Forms;
    using System.Windows.Media;
    using System.Windows.Media.Imaging;
    using System.Windows.Shapes;
    using System.Linq;
    using Microsoft.Kinect;
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow
    {
        #region Private State
        private const int Ignore = 2;
        private const int BufferSize = 32;
        private const int MinimumFrames = 6;
        private const int CaptureCountdownSeconds = 3;
        private string GestureSaveFileLocation = Environment.CurrentDirectory;
        private const string GestureSaveFileNamePrefix = @"RecordedGestures";
        private bool _capturing;
        private DtwGestureRecognizer _dtw;
        private int _flipFlop;
        private ArrayList _video;
        private DateTime _captureCountdown = DateTime.Now;
        private Timer _captureCountdownTimer;
        private KinectSensor _Kinect;
        private static Skeleton[] _FrameSkeletons;
        private WriteableBitmap _ColorImageBitmap;
        private Int32Rect _ColorImageBitmapRect;
        private int _ColorImageStride;
        #endregion Private State
        #region ctor + Window Events
        public MainWindow()
        {
            InitializeComponent();
        }
        private void WindowLoaded(object sender, RoutedEventArgs e)
        {
            //kinect discovery and initialization
            DiscoverKinectSensor();
            _dtw = new DtwGestureRecognizer(12, 0.6, 2, 2, 10);
            _video = new ArrayList();
        }
        private void WindowClosed(object sender, EventArgs e)
        {
            this.Kinect = null;
            Environment.Exit(0);
        }
        #endregion ctor + Window Events
        #region Kinect discovery + set up
        private void InitializeKinectSensor(KinectSensor sensor)
        {
            if (sensor != null)
            {
                //enable skeleton stream
                sensor.SkeletonStream.Enable();
                _FrameSkeletons = new Skeleton[sensor.SkeletonStream.FrameSkeletonArrayLength];
                sensor.SkeletonFrameReady += new EventHandler<SkeletonFrameReadyEventArgs>(NuiSkeletonFrameReady);
                //enable color stream
                ColorImageStream colorStream = sensor.ColorStream;
                colorStream.Enable(ColorImageFormat.RgbResolution640x480Fps30);
                this._ColorImageBitmap = new WriteableBitmap(colorStream.FrameWidth,
                                                colorStream.FrameHeight, 96, 96,
                                                PixelFormats.Bgr32, null);
                this._ColorImageBitmapRect = new Int32Rect(0, 0, colorStream.FrameWidth,
                colorStream.FrameHeight);
                this._ColorImageStride = colorStream.FrameWidth * colorStream.FrameBytesPerPixel;
                videoImage.Source = this._ColorImageBitmap;
                sensor.ColorFrameReady += new EventHandler<ColorImageFrameReadyEventArgs>(NuiColorFrameReady);
                //Dtw events
                sensor.SkeletonFrameReady += SkeletonExtractSkeletonFrameReady;
                Skeleton2DDataExtract.Skeleton2DdataCoordReady += NuiSkeleton2DdataCoordReady;
                sensor.Start();
            }
        }
        private void UninitializeKinectSensor(KinectSensor sensor)
        {
            if (sensor != null)
            {
                sensor.Stop();
                sensor.SkeletonFrameReady -= NuiSkeletonFrameReady;
                sensor.ColorFrameReady -= NuiColorFrameReady;
            }
        }
        public KinectSensor Kinect
        {
            get { return this._Kinect; }
            set
            {
                if (this._Kinect != null)
                {
                    UpdateDisplayStatus("No connected device.");
                    UninitializeKinectSensor(this._Kinect);
                    this._Kinect = null;
                }
                if (value != null && value.Status == KinectStatus.Connected)
                {
                    this._Kinect = value;
                    InitializeKinectSensor(this._Kinect);
                    kinectStatus.Text = string.Format("{0} - {1}", this._Kinect.UniqueKinectId, this._Kinect.Status);
                }
                else
                {
                    UpdateDisplayStatus("No connected device.");
                }
            }
        }
        private void DiscoverKinectSensor()
        {
            KinectSensor.KinectSensors.StatusChanged += KinectSensors_StatusChanged;
            this.Kinect = KinectSensor.KinectSensors.FirstOrDefault(x => x.Status == KinectStatus.Connected);
        }
        private void KinectSensors_StatusChanged(object sender, StatusChangedEventArgs e)
        {
            switch (e.Status)
            {
                case KinectStatus.Connected:
                    if (this.Kinect == null)
                    {
                        this.Kinect = e.Sensor;
                        UpdateDisplayStatus("Sensor connected.");
                    }
                    break;
                case KinectStatus.Disconnected:
                    if (this.Kinect == e.Sensor)
                    {
                        this.Kinect = null;
                        this.Kinect = KinectSensor.KinectSensors.FirstOrDefault(x => x.Status == KinectStatus.Connected);
                        if (this.Kinect == null)
                        {
                            UpdateDisplayStatus("No connected device.");
                        }
                    }
                    break;
                //TODO: Handle all other statuses according to needs
            }
            if (e.Status == KinectStatus.Connected)
            {
                this.Kinect = e.Sensor;
            }
        }
        private void UpdateDisplayStatus(string message)
        {
            kinectStatus.Text = message;
        }
        #endregion Kinect discovery + set up
        #region KinectEventsMethods
        private void NuiSkeletonFrameReady(object sender, SkeletonFrameReadyEventArgs e)
        {
            using (SkeletonFrame frame = e.OpenSkeletonFrame())
            {
                if (frame != null)
                {
                    frame.CopySkeletonDataTo(_FrameSkeletons);
                    Skeleton data = (from s in _FrameSkeletons
                                     where s.TrackingState == SkeletonTrackingState.Tracked
                                     select s).FirstOrDefault();
                    if (data != null)
                    {
                        Brush brush = new SolidColorBrush(Colors.Blue);
                        skeletonCanvas.Children.Clear();
                        //Draw bones
                        skeletonCanvas.Children.Add(GetBodySegment(data.Joints, brush, JointType.HipCenter, JointType.Spine, JointType.ShoulderCenter, JointType.Head));
                        skeletonCanvas.Children.Add(GetBodySegment(data.Joints, brush, JointType.ShoulderCenter, JointType.ShoulderLeft, JointType.ElbowLeft, JointType.WristLeft, JointType.HandLeft));
                        skeletonCanvas.Children.Add(GetBodySegment(data.Joints, brush, JointType.ShoulderCenter, JointType.ShoulderRight, JointType.ElbowRight, JointType.WristRight, JointType.HandRight));
                        skeletonCanvas.Children.Add(GetBodySegment(data.Joints, brush, JointType.HipCenter, JointType.HipLeft, JointType.KneeLeft, JointType.AnkleLeft, JointType.FootLeft));
                        skeletonCanvas.Children.Add(GetBodySegment(data.Joints, brush, JointType.HipCenter, JointType.HipRight, JointType.KneeRight, JointType.AnkleRight, JointType.FootRight));
                        // Draw joints
                        foreach (Joint joint in data.Joints)
                        {
                            Point jointPos = GetDisplayPosition(joint);
                            Ellipse ellipse = new Ellipse
                            {
                                Fill = brush,
                                Width = 10,
                                Height = 10,
                                Margin = new Thickness(jointPos.X, jointPos.Y, 0, 0)
                            };
                            skeletonCanvas.Children.Add(ellipse);
                        }
                    }
                }
            }
        }
        private void NuiColorFrameReady(object sender, ColorImageFrameReadyEventArgs e)
        {
            using (ColorImageFrame frame = e.OpenColorImageFrame())
            {
                if (frame != null)
                {
                    byte[] pixelData = new byte[frame.PixelDataLength];
                    frame.CopyPixelDataTo(pixelData);
                    this._ColorImageBitmap.WritePixels(this._ColorImageBitmapRect, pixelData,
                    this._ColorImageStride, 0);
                }
            }
        }
        private Polyline GetBodySegment(JointCollection joints, Brush brush, params JointType[] ids)
        {
            PointCollection points = new PointCollection(ids.Length);
            for (int i = 0; i < ids.Length; ++i)
            {
                points.Add(GetDisplayPosition(joints[ids[i]]));
            }
            Polyline polyline = new Polyline();
            polyline.Points = points;
            polyline.Stroke = brush;
            polyline.StrokeThickness = 2;
            return polyline;
        }
        private Point GetDisplayPosition(Joint joint)
        {
            ColorImagePoint colorImgpoint = Kinect.MapSkeletonPointToColor(joint.Position, ColorImageFormat.RgbResolution640x480Fps30);
            return new Point(colorImgpoint.X, colorImgpoint.Y);
        }
        #endregion KinectEventsMethods
        #region DTWGestureRecognition
        public void LoadGesturesFromFile(string fileLocation)
        {
            int itemCount = 0;
            string line;
            string gestureName = String.Empty;
            // TODO I'm defaulting this to 12 here for now as it meets my current need but I need to cater for variable lengths in the future
            ArrayList frames = new ArrayList();
            double[] items = new double[12];
            // Read the file and display it line by line.
            System.IO.StreamReader file = new System.IO.StreamReader(fileLocation);
            while ((line = file.ReadLine()) != null)
            {
                if (line.StartsWith("@"))
                {
                    gestureName = line;
                    continue;
                }
                if (line.StartsWith("~"))
                {
                    frames.Add(items);
                    itemCount = 0;
                    items = new double[12];
                    continue;
                }
                if (!line.StartsWith("----"))
                {
                    items[itemCount] = Double.Parse(line);
                }
                itemCount++;
                if (line.StartsWith("----"))
                {
                    _dtw.AddOrUpdate(frames, gestureName);
                    frames = new ArrayList();
                    gestureName = String.Empty;
                    itemCount = 0;
                }
            }
            file.Close();
        }
        private static void SkeletonExtractSkeletonFrameReady(object sender, SkeletonFrameReadyEventArgs e)
        {
            using (SkeletonFrame skeletonFrame = e.OpenSkeletonFrame())
            {
                if (skeletonFrame != null)
                {
                    skeletonFrame.CopySkeletonDataTo(_FrameSkeletons);
                    Skeleton data = (from s in _FrameSkeletons
                                     where s.TrackingState == SkeletonTrackingState.Tracked
                                     select s).FirstOrDefault();
                    Skeleton2DDataExtract.ProcessData(data);
                }
            }
        }
        private void NuiSkeleton2DdataCoordReady(object sender, Skeleton2DdataCoordEventArgs a)
        {
            currentBufferFrame.Text = _video.Count.ToString();
            // We need a sensible number of frames before we start attempting to match gestures against remembered sequences
            if (_video.Count > MinimumFrames && _capturing == false)
            {
                ////Debug.WriteLine("Reading and video.Count=" + video.Count);
                string s = _dtw.Recognize(_video);
                results.Text = "Recognised as: " + s;
                if (!s.Contains("__UNKNOWN"))
                {
                    // There was no match so reset the buffer
                    _video = new ArrayList();
                }
            }
            // Ensures that we remember only the last x frames
            if (_video.Count > BufferSize)
            {
                // If we are currently capturing and we reach the maximum buffer size then automatically store
                if (_capturing)
                {
                    DtwStoreClick(null, null);
                }
                else
                {
                    // Remove the first frame in the buffer
                    _video.RemoveAt(0);
                }
            }
            // Decide which skeleton frames to capture. Only do so if the frames actually returned a number. 
            // For some reason my Kinect/PC setup didn't always return a double in range (i.e. infinity) even when standing completely within the frame.
            // TODO Weird. Need to investigate this
            if (!double.IsNaN(a.GetPoint(0).X))
            {
                // Optionally register only 1 frame out of every n
                _flipFlop = (_flipFlop + 1) % Ignore;
                if (_flipFlop == 0)
                {
                    _video.Add(a.GetCoords());
                }
            }
            // Update the debug window with Sequences information
            //dtwTextOutput.Text = _dtw.RetrieveText();
        }
        private void DtwReadClick(object sender, RoutedEventArgs e)
        {
            // Set the buttons enabled state
            dtwRead.IsEnabled = false;
            dtwCapture.IsEnabled = true;
            dtwStore.IsEnabled = false;
            // Set the capturing? flag
            _capturing = false;
            // Update the status display
            status.Text = "Reading";
        }
        private void DtwCaptureClick(object sender, RoutedEventArgs e)
        {
            _captureCountdown = DateTime.Now.AddSeconds(CaptureCountdownSeconds);
            _captureCountdownTimer = new Timer();
            _captureCountdownTimer.Interval = 50;
            _captureCountdownTimer.Start();
            _captureCountdownTimer.Tick += CaptureCountdown;
        }
        private void CaptureCountdown(object sender, EventArgs e)
        {
            if (sender == _captureCountdownTimer)
            {
                if (DateTime.Now < _captureCountdown)
                {
                    status.Text = "Wait " + ((_captureCountdown - DateTime.Now).Seconds + 1) + " seconds";
                }
                else
                {
                    _captureCountdownTimer.Stop();
                    status.Text = "Recording gesture";
                    StartCapture();
                }
            }
        }
        private void StartCapture()
        {
            // Set the buttons enabled state
            dtwRead.IsEnabled = false;
            dtwCapture.IsEnabled = false;
            dtwStore.IsEnabled = true;
            // Set the capturing? flag
            _capturing = true;
            ////_captureCountdownTimer.Dispose();
            status.Text = "Recording gesture" + gestureList.Text;
            // Clear the _video buffer and start from the beginning
            _video = new ArrayList();
        }
        private void DtwStoreClick(object sender, RoutedEventArgs e)
        {
            // Set the buttons enabled state
            dtwRead.IsEnabled = false;
            dtwCapture.IsEnabled = true;
            dtwStore.IsEnabled = false;
            // Set the capturing? flag
            _capturing = false;
            status.Text = "Remembering " + gestureList.Text;
            // Add the current video buffer to the dtw sequences list
            _dtw.AddOrUpdate(_video, gestureList.Text);
            results.Text = "Gesture " + gestureList.Text + "added";
            // Scratch the _video buffer
            _video = new ArrayList();
            // Switch back to Read mode
            DtwReadClick(null, null);
        }
        private void DtwSaveToFile(object sender, RoutedEventArgs e)
        {
            string fileName = GestureSaveFileNamePrefix + DateTime.Now.ToString("yyyy-MM-dd_HH-mm") + ".txt";
            System.IO.File.WriteAllText(GestureSaveFileLocation + fileName, _dtw.RetrieveText());
            status.Text = "Saved to " + fileName;
        }
        private void DtwLoadFile(object sender, RoutedEventArgs e)
        {
            // Create OpenFileDialog
            Microsoft.Win32.OpenFileDialog dlg = new Microsoft.Win32.OpenFileDialog();
            // Set filter for file extension and default file extension
            dlg.DefaultExt = ".txt";
            dlg.Filter = "Text documents (.txt)|*.txt";
            dlg.InitialDirectory = GestureSaveFileLocation;
            // Display OpenFileDialog by calling ShowDialog method
            Nullable<bool> result = dlg.ShowDialog();
            // Get the selected file name and display in a TextBox
            if (result == true)
            {
                // Open document
                LoadGesturesFromFile(dlg.FileName);
                //dtwTextOutput.Text = _dtw.RetrieveText();
                status.Text = "Gestures loaded!";
            }
        }
        private void DtwShowGestureText(object sender, RoutedEventArgs e)
        {
            //dtwTextOutput.Text = _dtw.RetrieveText();
        }
        #endregion DTWGestureRecognition
    }
}
//Skeleton2DDataExtract.cs - ProcessData method

//Remaining code is unaffected

